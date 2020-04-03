/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <windows.h>
#include <gl/gl.h>
#include "render.h"
#include "rbuffer.h"
/* ------------------------------------------------------------------------ */

#define MAX_ID 256

struct _window {
	HWND h;
};

typedef void (*callbuild)( void * );

struct calllist {
	bool enable;
	void *param;
	callbuild build;
};

struct _render {
	HWND wnd;
	HDC dc;
	HGLRC rc;
	calllist ids[MAX_ID];
};

struct _texture {
	render *render;
	int list_id;
	int rw;
	int rh;
	int tw;
	int th;
	char *buf;
};

/* ------------------------------------------------------------------------ */

static void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC) {
	PIXELFORMATDESCRIPTOR pfd;
	int format;
	*hDC = GetDC( hWnd );
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat( *hDC, &pfd );
	SetPixelFormat( *hDC, format, &pfd );
	*hRC = wglCreateContext( *hDC );
	wglMakeCurrent( *hDC, *hRC );
}

/* ------------------------------------------------------------------------ */

static void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC) {
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );
	ReleaseDC( hWnd, hDC );
}

/* ------------------------------------------------------------------------ */

render *render_init( window *w ) {
	render *r = new _render;
	r->wnd = w->h;
	EnableOpenGL( w->h, &r->dc, &r->rc );
	int i;
	for(i=0;i<MAX_ID;i++)
		r->ids[i].enable = false;
	return r;
}

/* ------------------------------------------------------------------------ */

void render_destroy( render *r ) {
	DisableOpenGL(r->wnd,r->dc,r->rc);
	delete r;
}

/* ------------------------------------------------------------------------ */

void render_enable( render *r, bool e ) {
	if( e == (r->dc != NULL) )
		return;
	if( e ) {
		EnableOpenGL( r->wnd, &r->dc, &r->rc );
		int i;
		for(i=0;i<MAX_ID;i++)
			if( r->ids[i].enable )
				r->ids[i].build(r->ids[i].param);
	} else {
		DisableOpenGL(r->wnd,r->dc,r->rc);
		r->dc = NULL;
		r->rc = NULL;
	}
}

/* ------------------------------------------------------------------------ */

void render_start( render *r, int fillcolor ) {
	// TODO : acquire global lock for OGL rendering ?
	color c(fillcolor);
	RECT pos;
	GetClientRect(r->wnd,&pos);
	glViewport(0,0,pos.right,pos.bottom);
	glClearColor(c.r,c.g,c.b,c.a);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_NORMALIZE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
}

/* ------------------------------------------------------------------------ */

void render_flip( render *r ) {
	// TODO : release global lock
	SwapBuffers( r->dc );
}

/* ------------------------------------------------------------------------ */

void render_set_transform( render *r, matrix *m ) {
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)m);
}

/* ------------------------------------------------------------------------ */

void render_set_light( render *r, vector *pos, unsigned int ambient, unsigned int diffuse, float ray ) {
	color a(ambient);
	color d(diffuse);
	color black(0);
	bool is_pos = (ray > 0);
	float p[] = { pos->x, pos->y, pos->z, is_pos?1.0f:0.0f };
	glLightfv(GL_LIGHT0,GL_DIFFUSE,d);
	glLightfv(GL_LIGHT0,GL_SPECULAR,black);
    glLightfv(GL_LIGHT0,GL_POSITION,p);
	if( is_pos ) {
		glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,0);
		glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,1/ray);
	} else {
		glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,1);
		glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0);
	}
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,a);
}

/* ------------------------------------------------------------------------ */

void render_set_alpha( render *r, bool alpha ) {
	if( alpha ) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
	} else {
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
}

/* ------------------------------------------------------------------------ */

void render_buffer( render *r, rbuffer *b, matrix *m, color *c, texture *t ) {
	if( t == NULL )
		glDisable(GL_TEXTURE_2D);
	else {
		glCallList(t->list_id);
		glEnable(GL_TEXTURE_2D);
	}
	glColor4f(c->r,c->g,c->b,c->a);
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,(float*)c);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)m);
	glBegin(GL_TRIANGLES);
	int i = b->nverts / 3;
	if( !b->have_normals )
		rbuffer_calc_normals(b);	
	switch( b->format ) {
	case VNORMAL: {
		vertex_normal *buf = b->vbuf;
		while( i-- ) {
			glNormal3f(buf->n.x,buf->n.y,buf->n.z);
			glVertex3f(buf->v.x,buf->v.y,buf->v.z);
			buf++;
			glNormal3f(buf->n.x,buf->n.y,buf->n.z);
			glVertex3f(buf->v.x,buf->v.y,buf->v.z);
			buf++;
			glNormal3f(buf->n.x,buf->n.y,buf->n.z);
			glVertex3f(buf->v.x,buf->v.y,buf->v.z);
			buf++;
		}
		break; }
	case VCOLOR: {
		vertex_color *buf = b->cbuf;
		while( i-- ) {
			glNormal3f(buf->n.x,buf->n.y,buf->n.z);
			glColor4f(buf->c.r,buf->c.g,buf->c.b,buf->c.a);
			glVertex3f(buf->v.x,buf->v.y,buf->v.z);
			buf++;
			glNormal3f(buf->n.x,buf->n.y,buf->n.z);
			glColor4f(buf->c.r,buf->c.g,buf->c.b,buf->c.a);
			glVertex3f(buf->v.x,buf->v.y,buf->v.z);
			buf++;
			glNormal3f(buf->n.x,buf->n.y,buf->n.z);
			glColor4f(buf->c.r,buf->c.g,buf->c.b,buf->c.a);
			glVertex3f(buf->v.x,buf->v.y,buf->v.z);
			buf++;
		}
		break; }
	case VTEXTURE: {
		vertex_texture *buf = b->tbuf;
		float sx,sy,px,py;
		if( t != NULL )
			render_texture_props(t,&px,&py,&sx,&sy);
		else {
			px = 0;
			py = 0;
			sx = 1;
			sy = 1;
		}
		while( i-- ) {
			glNormal3f(buf->n.x,buf->n.y,buf->n.z);
			glTexCoord2f(buf->t.tu*sx+px,buf->t.tv*sy+py);
			glVertex3f(buf->v.x,buf->v.y,buf->v.z);
			buf++;
			glNormal3f(buf->n.x,buf->n.y,buf->n.z);
			glTexCoord2f(buf->t.tu*sx+px,buf->t.tv*sy+py);
			glVertex3f(buf->v.x,buf->v.y,buf->v.z);
			buf++;
			glNormal3f(buf->n.x,buf->n.y,buf->n.z);
			glTexCoord2f(buf->t.tu*sx+px,buf->t.tv*sy+py);
			glVertex3f(buf->v.x,buf->v.y,buf->v.z);
			buf++;
		}
		break; }
	}
	glEnd();
}

/* ------------------------------------------------------------------------ */

int render_alloc_listid( render *r, callbuild build, void *param ) {
	int i = 0;
	while( i < MAX_ID ) {
		if( !r->ids[i].enable ) {
			r->ids[i].enable = true;
			r->ids[i].build = build;
			r->ids[i].param = param;
			return i;
		}
		i++;
	}
	return -1;
}

/* ------------------------------------------------------------------------ */

void render_free_listid( render *r, int id ) {
	if( id == -1 )
		return;
	r->ids[id].enable = false;
}

/* ------------------------------------------------------------------------ */

static int pow2( int x ) {
	int nbits = 0;
	x--;
	while( x > 0 ) {
		nbits++;
		x >>=1;
	}
	return (1 << nbits);
}

/* ------------------------------------------------------------------------ */

texture *render_texture_alloc( render *r, int tw, int th ) {
	texture *t = new _texture;
	t->render = r;
	t->list_id = render_alloc_listid(r,(callbuild)render_texture_unlock,t);
	t->tw = tw;
	t->th = th;
	t->rw = pow2(tw);
	t->rh = pow2(th);
	t->buf = new char[t->rw*t->rh*4];
	DWORD *ptr = (DWORD*)buf;
	int k = t->rw * t->rh;
	while( k-- )
		*ptr++ = 0xFFFF00FF;	
	return t;
}

/* ------------------------------------------------------------------------ */

void render_texture_free( texture *t ) {
	render_free_listid(t->render,t->list_id);
	delete t->buf;
	delete t;
}

/* ------------------------------------------------------------------------ */

void render_texture_lock( texture *t, void **ptr, int *stride ) {
	*ptr = t->buf;
	*stride = t->rw * 4;
}

/* ------------------------------------------------------------------------ */

void render_texture_unlock( texture *t ) {
	glNewList(t->list_id,GL_COMPILE);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glTexImage2D(GL_TEXTURE_2D,0,4,t->rw,t->rh,0,GL_BGRA_EXT,GL_UNSIGNED_BYTE,t->buf);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glEndList();
}

/* ------------------------------------------------------------------------ */

void render_texture_update( texture *t, void *buf, int stride ) {
	void *ptr;
	int pstride;
	render_texture_lock(t,&ptr,&pstride);
	int y;
	for(y=0;y<t->th;y++) {
		memcpy(ptr,buf,t->tw * 4);
		ptr = (char*)ptr + pstride;
		buf = (char*)buf + stride;
	}
	render_texture_unlock(t);
}

/* ------------------------------------------------------------------------ */

void render_texture_props( texture *t, float *px, float *py, float *sx, float *sy ) {
	*px = 0;
	*py = 0;
	*sx = t->tw * 1.0f / t->rw;
	*sy = t->th * 1.0f / t->rh;
}

/* ************************************************************************ */

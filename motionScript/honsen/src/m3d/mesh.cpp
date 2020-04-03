/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "mesh.h"
#include "rbuffer.h"
#include "m3d_priv.h"
#include "picture.h"
#include "../../common/mtw.h"
#include "../resources.h"
/* ------------------------------------------------------------------------ */

struct TextureData {
	texture *t;
	float px,py;
	float sx,sy;
	~TextureData() {
		render_texture_free(t);
	}
};

struct _MeshData {
	rbuffer *buf;
	Cached<TextureData> tex;
	~_MeshData() {
		rbuffer_free(buf);
	}
};

/* ------------------------------------------------------------------------ */

Mesh::Mesh( Mesh *p, MeshData *d ) {
	visible = true;
	transform = true;
	sprite = false;
	light = true;
	srcblend = BLEND_SRCALPHA;
	dstblend = BLEND_INVSRCALPHA;
	parent = p;
	data = d;
	alpha = 100;
	color = 0xFFFFFF;
	matrix3d_identity(&pos);
}

/* ------------------------------------------------------------------------ */

Mesh::~Mesh() {
	childs.Delete();
	if( ref != NULL )
		RefRelease(ref);
	delete data;
}

/* ------------------------------------------------------------------------ */

texture *TextureGet( cached_texture *t ) {
	Cached<TextureData> *data = (Cached<TextureData>*)t;
	return data->Available()?data->Data()->t:NULL;
}

/* ------------------------------------------------------------------------ */

void Mesh::Render( m3d *m, bool abs_visible, bool alpha_mode ) {
	if( !alpha_mode ) {
		if( parent == NULL ) {
			matrix3d_copy(&abs_pos,&pos);
			abs_color = ::color(color);
			abs_color.a = (float)(alpha/100);
		} else {
			::color c(color);
			c.a = (float)(alpha/100);
			matrix3d_multiply(&abs_pos,&pos,&parent->abs_pos);
			color_multiply(&abs_color,&c,&parent->abs_color);
		}
	}
	abs_visible = abs_visible && visible;
	if( abs_visible && alpha_mode == (abs_color.a < 1) ) {
		m->stats.nmeshes++;
		m->stats.nvects += data->buf->nverts;
		if( m->transform != transform ) {
			m->transform = transform;
			if( transform )
				render_set_transform(m->render,&m->cur_camera);
			else {
				matrix i;
				matrix3d_identity(&i);
				render_set_transform(m->render,&i);
			}
		}
		if( sprite ) {
			camera *c = *m->camera;
			vector r,d;
			vector_sub(&d,&c->realpos,&c->target);
			vector_normalize(&d,&d);
			vector_cross(&r,&c->realup,&d);
			vector_normalize(&r,&r);
			abs_pos[0] = r.x;
			abs_pos[1] = r.y;
			abs_pos[2] = r.z;
			abs_pos[3] = 0;
			abs_pos[4] = c->realup.x;
			abs_pos[5] = c->realup.y;
			abs_pos[6] = c->realup.z;
			abs_pos[7] = 0;
			abs_pos[8] = d.x;
			abs_pos[9] = d.y;
			abs_pos[10] = d.z;
			abs_pos[11] = 0;			
		}
		rprops p;
		p.buffer = data->buf;
		p.matrix = &abs_pos;
		p.color = &abs_color;
		p.light = light;
		p.texture = data->tex.Available()?data->tex.Data()->t:NULL;
		p.srcblend = srcblend;
		p.dstblend = dstblend;
		render_buffer(m->render,&p);
	}
	FOREACH(Mesh*,childs);
		item->Render(m,abs_visible,alpha_mode);
	ENDFOR;
}

/* ------------------------------------------------------------------------ */

void Mesh::Resize( int nverts ) {
	bool tex = data->tex.Available();
	RBFORMAT fmt = tex?VTEXTURE:VNORMAL;
	if( data->buf->format != fmt || data->buf->maxverts != nverts ) {
		rbuffer_free(data->buf);
		data->buf = rbuffer_alloc(nverts,fmt);
	}
	data->buf->nverts = 0;
}

/* ------------------------------------------------------------------------ */

bool Mesh::DrawVector( float x, float y, float z ) {
	int p = data->buf->nverts;
	if( p == data->buf->maxverts )
		return false;
	switch( data->buf->format ) {
	case VNORMAL:
		data->buf->vbuf[p].v = vector(x,y,z);
		break;
	case VCOLOR:
		data->buf->cbuf[p].v = vector(x,y,z);
		break;
	case VTEXTURE:
		data->buf->tbuf[p].v = vector(x,y,z);
		break;
	}
	data->buf->nverts++;
	return true;
}

/* ------------------------------------------------------------------------ */

bool Mesh::DrawTCoords( float tu, float tv ) {
	int p = data->buf->nverts;
	if( p == data->buf->maxverts || data->buf->format != VTEXTURE )
		return false;
	TextureData *td = data->tex.Data();
	data->buf->tbuf[p].t.tu = tu;
	data->buf->tbuf[p].t.tv = tv;
	return true;
}

/* ------------------------------------------------------------------------ */

bool Mesh::DrawColor( unsigned int col ) {
	int p = data->buf->nverts;
	if( p == data->buf->maxverts || data->buf->format != VCOLOR )
		return false;
	data->buf->cbuf[p].c = ::color(col);
	return true;
}

/* ------------------------------------------------------------------------ */

void Mesh::CalcNormals() {
	rbuffer_calc_normals(data->buf);
}

/* ------------------------------------------------------------------------ */

void Mesh::SetTexture( cached_texture *t ) {
	if( t == NULL )
		data->tex.Release();
	else
		data->tex = *(Cached<TextureData>*)t;
}

/* ------------------------------------------------------------------------ */

cached_texture *Mesh::GetTexture() {
	if( data->tex.Available() )
		return (cached_texture*)&data->tex;
	return NULL;
}

/* ------------------------------------------------------------------------ */

static TextureData *texture_load( render *r, MTW *t ) {
	if( t == NULL || t->tag != MAKETAG(Pict) )
		return NULL;
	MTW *data = t->Child(MAKETAG(DATA));
	if( data == NULL )
		return NULL;
	pic_kind kind;
	if( t->HasChild(MAKETAG(jPEG)) )
		kind = PIC_JPEG;
	else if( t->HasChild(MAKETAG(bRAW)) )
		kind = PIC_RAW;
	else
		return NULL;
	int twidth;
	int theight;
	char *picdata = picture_read(data->data,data->size,kind,&twidth,&theight);
	if( twidth < 0 || theight < 0 || twidth > 1024 || theight > 1024 || picdata == NULL ) {
		delete picdata;
		return NULL;
	}
	texture *tex = render_texture_alloc(r,twidth,theight);
	if( tex == NULL ) {
		delete picdata;
		return NULL;
	}
	render_texture_update(tex,picdata,twidth*4);
	delete picdata;
	TextureData *td = new TextureData;
	td->t = tex;
	render_texture_props(tex,&td->px,&td->py,&td->sx,&td->sy);
	return td;
}

/* ------------------------------------------------------------------------ */

cached_texture *Texture::Make( m3d *m3d, MTW *t ) {
	Cached<TextureData> tdata = res_load<TextureData,render>(m3d->resources,t,texture_load,m3d->render);
	if( !tdata.Available() )
		return NULL;
	return (cached_texture*)new Cached<TextureData>(tdata);
}

/* ------------------------------------------------------------------------ */

void Texture::Delete( cached_texture *t ) {
	delete (Cached<TextureData>*)t;
}

/* ------------------------------------------------------------------------ */

void Texture::Size( cached_texture *t, int *width, int *height ) {
	texture *tex = ((Cached<TextureData>*)t)->Data()->t;
	render_texture_size(tex,width,height);
}

/* ------------------------------------------------------------------------ */

cached_texture *Texture::Copy( cached_texture *t ) {
	return (cached_texture*)new Cached<TextureData>( *(Cached<TextureData>*)t );
}

/* ------------------------------------------------------------------------ */

cached_texture *TextureAlloc( texture *t ) {
	TextureData *td = new TextureData;
	td->t = t;
	render_texture_props(t,&td->px,&td->py,&td->sx,&td->sy);
	return (cached_texture*)new Cached<TextureData>( Cache::Make<TextureData>((MTW*)t,td) ); 
}

/* ------------------------------------------------------------------------ */

MeshData *Mesh::Make( m3d *m3d, MTW *m ) {
	if( m == NULL ) {
		MeshData *d = new MeshData;
		d->buf = rbuffer_alloc(0,VNORMAL);
		return d;
	}
	if( m->tag != MAKETAG(Objt) )
		return NULL;
	MTW *vect = m->Child(MAKETAG(VECT));
	MTW *indx = m->Child(MAKETAG(INDX));
	if( vect == NULL || indx == NULL )
		return NULL;
	int maxvects = vect->size / sizeof(vector);
	int nvects = indx->size / sizeof(unsigned short);
	if( (nvects % 3) != 0 )
		return NULL;

	MTW *tex = m->Child(MAKETAG(TEX0));
	MTW *map = m->Child(MAKETAG(MAP0));
	MTW *mid = m->Child(MAKETAG(MID0));
	rbuffer *rbuf;
	Cached<TextureData> tdata;
	if( tex != NULL ) {
		if( map == NULL || mid == NULL )
			return NULL;
		if( mid->size / sizeof(unsigned short) != nvects )
			return NULL;
		if( tex->size == 0 || ((char*)tex->data)[tex->size-1] != '\0' )
			return NULL;
		char *tname = strdup((char*)tex->data);
		char *dot = strrchr(tname,'.');
		if( dot != NULL ) *dot = 0;
		MTW *texdata = res_find_byname(m3d->resources,tname);
		free(tname);
		if( texdata == NULL )
			return NULL;
		tdata = res_load<TextureData,render>(m3d->resources,texdata,texture_load,m3d->render);
		if( !tdata.Available() )
			return NULL;
		TextureData *td = tdata.Data();
		int maxtcoords = map->size / sizeof(tcoords);
		rbuf = rbuffer_alloc(nvects,VTEXTURE);
		vector *v = (vector*)vect->data;
		tcoords *tc = (tcoords*)map->data;
		vertex_texture *buf = rbuf->tbuf;
		unsigned short *idx = (unsigned short*)indx->data;
		unsigned short *tidx = (unsigned short*)mid->data;
		int i;
		for(i=0;i<nvects;i++,idx++,tidx++,buf++) {
			int k = *idx;
			int t = *tidx;
			if( k >= maxvects || t >= maxtcoords ) {
				rbuffer_free(rbuf);
				return NULL;
			}
			buf->v = v[k];
			buf->t = tc[t];
		}
	} else {
		rbuf = rbuffer_alloc(nvects,VNORMAL);
		vector *v = (vector*)vect->data;
		vertex_normal *buf = rbuf->vbuf;
		unsigned short *idx = (unsigned short*)indx->data;
		int i;
		for(i=0;i<nvects;i++,idx++,buf++) {
			int k = *idx;
			if( k >= maxvects ) {
				rbuffer_free(rbuf);
				return NULL;
			}
			buf->v = v[k];
		}
	}
	rbuf->nverts = nvects;
	rbuffer_calc_normals(rbuf);
	MeshData *d = new MeshData;
	d->buf = rbuf;
	d->tex = tdata;
	return d;
}

/* ************************************************************************ */

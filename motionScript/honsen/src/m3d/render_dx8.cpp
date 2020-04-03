/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "../../common/wstring.h"
#include "../../common/wmap.h"
#include "render.h"
#include "rbuffer.h"
/* ------------------------------------------------------------------------ */

struct _window {
	HWND h;
};

enum LIGHTTYPE {
	LNONE,
	LDIR,
	LPOINT
};

struct HashedString {
	int hash;
	WString str;

	static int compare( HashedString *a, HashedString *b ) {
		if( a->hash == b->hash ) {
			int l1 = a->str.length();
			int l2 = b->str.length();
			return memcmp(a->str.c_str(),b->str.c_str(),l1>l2?l1:l2);
		}
		return a->hash - b->hash;
	}
};

struct _render {
	HWND wnd;
	LPDIRECT3D8 object;
	LPDIRECT3DDEVICE8 device;
	LIGHTTYPE light;
	WMap<HashedString*,DWORD> shader_cache;
	_render() : shader_cache(HashedString::compare) {
	}
};

struct _texture {
	render *render;
	LPDIRECT3DTEXTURE8 tex;
	int rw;
	int rh;
	int tw;
	int th;
};


/*
  shader constants
	0	camera transform & projection
	1
	2
	3
o	4	object position
o	5
o	6
o	7
	8	light position
	9	light ambient
	10	light diffuse
	11	light attenuation
o	12	object ambient+diffuse
	13	zero
	14	one
o	15	texture transform
*/

static WString sh_normal = "\
	vs.1.0					\n\
	m4x4 r0 , v0, c4		\n\
	m4x4 oPos, r0, c0		\n\
";

static WString sh_tex = "\
	vs.1.0					\n\
	m4x4 r0 , v0, c4		\n\
	m4x4 oPos, r0, c0		\n\
	mul oT0, v2, c15		\n\
";

static WString sh_point_light = "\
	add r1.xyz, c8, -r0.xyz	\n\
	dp3 r2.w , r1, r1		\n\
	mul r2.w , r2.w , c11.w	\n\
	rcp r2.w , r2.w			\n\
	m3x3 r3 , v1 , c4		\n\
	dp3 r4.w , r3 , r3		\n\
	rsq r4.w , r4.w			\n\
	mul r3 , r3, r4.w		\n\
	dp3 r1.w , r3 , r1		\n\
	mul r1.w , r1.w, r2.w	\n\
";

static WString sh_dir_light = "\
	m3x3 r0, v1, c4			\n\
	dp3 r1.w, r0, r0		\n\
	rsq r1.w, r1.w			\n\
	mul r0, r0, r1.w		\n\
	dp3 r1.w, r0, c8		\n\
";

static WString sh_end_light = "\
	mul r0, r1.w, c10		\n\
	max r0, r0, c13			\n\
	min r0, r0, c14			\n\
	add r0, r0, c9			\n\
	mul r0, r0, c12			\n\
	mov oD0, r0				\n\
";

static WString sh_no_light = "\
	mov oD0, c12			\n\
";


static DWORD sh_normal_decl[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(0,D3DVSDT_FLOAT3),
	D3DVSD_REG(1,D3DVSDT_FLOAT3),
	D3DVSD_END()
};

static DWORD sh_tex_decl[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(0,D3DVSDT_FLOAT3),
	D3DVSD_REG(1,D3DVSDT_FLOAT3),
	D3DVSD_REG(2,D3DVSDT_FLOAT2),
	D3DVSD_END()
};

/* ------------------------------------------------------------------------ */

static bool CreateDevice( render *r, DWORD flags ) {
	D3DPRESENT_PARAMETERS present; 
	D3DDISPLAYMODE mode;
	RECT winr;
	GetClientRect(r->wnd,&winr);
	r->object->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&mode);
	ZeroMemory( &present, sizeof(present) );	
	present.Windowed = true;
	present.BackBufferWidth = winr.right;
	present.BackBufferHeight = winr.bottom;
	present.BackBufferFormat = mode.Format;
	present.BackBufferCount = 1;
	present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	present.EnableAutoDepthStencil = TRUE;
	present.AutoDepthStencilFormat = D3DFMT_D16;
	present.MultiSampleType = D3DMULTISAMPLE_NONE;
	present.FullScreen_RefreshRateInHz = 0;
	present.FullScreen_PresentationInterval = 0;
	present.hDeviceWindow = r->wnd;
	return( r->object->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,r->wnd,flags | D3DCREATE_FPU_PRESERVE,&present,&r->device) == D3D_OK );
}

/* ------------------------------------------------------------------------ */

static DWORD MakeShader( render *r, DWORD *decl, WString shader ) {
	HashedString s;
	int k = 0;
	const char *c = shader.c_str();
	s.str = shader;
	while( *c )
		k = k * 253 + *c++;
	s.hash = k;
	
	DWORD sh = r->shader_cache.Find(&s);
	if( sh != 0 )
		return sh;

	LPD3DXBUFFER shader_buffer;
	LPD3DXBUFFER error_buffer;
	D3DXAssembleShader(shader.c_str(),shader.length(),0,NULL,&shader_buffer,&error_buffer);
	if( shader_buffer == NULL )
		return 0;
	r->device->CreateVertexShader(decl,(DWORD*)shader_buffer->GetBufferPointer(),&sh,0);
	shader_buffer->Release();
	if( sh != 0 )
		r->shader_cache.Add(new HashedString(s),sh);
	return sh;
}

render *render_init( window *w ) {
	render *r = new _render;
	r->wnd = w->h;
	r->light = LNONE;
	r->object = Direct3DCreate8( D3D_SDK_VERSION );
	if( !CreateDevice(r,D3DCREATE_HARDWARE_VERTEXPROCESSING) )	
		CreateDevice(r,D3DCREATE_SOFTWARE_VERTEXPROCESSING);

	r->device->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL);
	r->device->SetRenderState( D3DRS_ALPHAREF, 0);
	r->device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	r->device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	r->device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	r->device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	r->device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	r->device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	r->device->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	r->device->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	return r;
}

/* ------------------------------------------------------------------------ */

LPDIRECT3DDEVICE8 render_device( render *r ) {
	return r->device;
}

/* ------------------------------------------------------------------------ */

void delete_shaders( HashedString *s, DWORD sh, void *r ) {
	((render*)r)->device->DeleteVertexShader(sh);
	delete s;
}

/* ------------------------------------------------------------------------ */

void render_destroy( render *r ) {	
	r->shader_cache.Iter(delete_shaders,r);	
	r->device->Release();
	r->object->Release();
	delete r;
}

/* ------------------------------------------------------------------------ */

void render_enable( render *r, bool e ) {
	// TODO
}

/* ------------------------------------------------------------------------ */

void render_start( render *r, int fillcolor ) {
	r->device->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,fillcolor,1.0f,0);
	r->device->BeginScene();
	color zero(0,0,0,1);
	color one(1,1,1,1);
	r->device->SetVertexShaderConstant(13,&zero,1);
	r->device->SetVertexShaderConstant(14,&one,1);	
}

/* ------------------------------------------------------------------------ */

void render_flip( render *r ) {
	r->device->EndScene();	
	r->device->Present(NULL,NULL,r->wnd,NULL);
}

/* ------------------------------------------------------------------------ */

void render_set_transform( render *r, matrix *m ) {
	matrix mout;
	matrix3d_transpose(&mout,m);
	r->device->SetVertexShaderConstant(0,&mout,4);
}

/* ------------------------------------------------------------------------ */

void render_set_light( render *r, vector *pos, unsigned int ambient, unsigned int diffuse, float ray ) {
	color a(ambient);
	color d(diffuse);
	float iray = 1 / ray;
	color v(iray,iray,iray,iray);
	a.a = 0;
	d.a = 1;
	r->device->SetVertexShaderConstant(9,&a,1);
	r->device->SetVertexShaderConstant(10,&d,1);
	r->device->SetVertexShaderConstant(11,&v,1);
	r->light = (ray > 0)?LPOINT:LDIR;

	vector p;
	if( ray > 0 )
		p = *pos;
	else
		vector_normalize(&p,pos);	

	D3DXVECTOR4 l;
	l.x = p.x;
	l.y = p.y;
	l.z = p.z;
	l.w = 1;
	r->device->SetVertexShaderConstant(8,&l,1);
}

/* ------------------------------------------------------------------------ */

void render_set_alpha( render *r, bool alpha ) {	
	if( alpha ) {
		r->device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		r->device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		r->device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	} else {
		r->device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		r->device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		r->device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		r->device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	}
}

/* ------------------------------------------------------------------------ */

static DWORD blend_op( int op, bool src ) {
	switch(op) {
	case BLEND_ZERO:
		return D3DBLEND_ZERO;
	case BLEND_ONE:
		return D3DBLEND_ONE;
	case BLEND_COLOR:
		return src?D3DBLEND_DESTCOLOR:D3DBLEND_SRCCOLOR;
	case BLEND_INVCOLOR:
		return src?D3DBLEND_INVDESTCOLOR:D3DBLEND_INVSRCCOLOR;
	case BLEND_SRCALPHA:
		return D3DBLEND_SRCALPHA;
	case BLEND_INVSRCALPHA:
		return D3DBLEND_INVSRCALPHA;
	case BLEND_DSTALPHA:
		return D3DBLEND_DESTALPHA;
	case BLEND_INVDSTALPHA:
		return D3DBLEND_INVDESTALPHA;
	default:
		return D3DBLEND_ZERO;
	}
}

/* ------------------------------------------------------------------------ */

void render_buffer( render *r, rprops *p ) {
	int ntri = p->buffer->nverts / 3;
	if( ntri == 0 )
		return;
	if( !p->buffer->have_normals )
		rbuffer_calc_normals(p->buffer);
	int stride;
	LIGHTTYPE l = p->light?r->light:LNONE;
	DWORD sh = 0;
	switch( p->buffer->format ) {
	case VNORMAL:
		switch( l ) {
		case LNONE:
			sh = MakeShader(r,sh_normal_decl,sh_normal + sh_no_light);
			break;
		case LPOINT:
			sh = MakeShader(r,sh_normal_decl,sh_normal + sh_point_light + sh_end_light);
			break;
		case LDIR:
			sh = MakeShader(r,sh_normal_decl,sh_normal + sh_dir_light + sh_end_light);
			break;
		default:
			return;
		}
		stride = sizeof(vertex_normal);
		break;
	case VTEXTURE:
		switch( l ) {
		case LNONE:
			sh = MakeShader(r,sh_tex_decl,sh_tex + sh_no_light);
			break;
		case LPOINT:
			sh = MakeShader(r,sh_tex_decl,sh_tex + sh_point_light + sh_end_light);
			break;
		case LDIR:
			sh = MakeShader(r,sh_tex_decl,sh_tex + sh_dir_light + sh_end_light);
			break;
		default:
			return;
		}
		stride = sizeof(vertex_texture);
		break;
	default:
		return;
	}
	r->device->SetVertexShader(sh);
	if( p->texture == NULL ) 
		r->device->SetTexture(0,NULL);
	else {
		color c;
		render_texture_props(p->texture,&c.b,&c.a,&c.r,&c.g);
		r->device->SetTexture(0,p->texture->tex);
		r->device->SetVertexShaderConstant(15,&c,1);
	}

	matrix mout;
	matrix3d_transpose(&mout,p->matrix);
	r->device->SetRenderState( D3DRS_SRCBLEND, blend_op(p->srcblend,true) );
	r->device->SetRenderState( D3DRS_DESTBLEND, blend_op(p->dstblend,false) );
	r->device->SetVertexShaderConstant(4,&mout,4); // mtrans
	r->device->SetVertexShaderConstant(12,p->color,1); // ambient
	r->device->DrawPrimitiveUP(D3DPT_TRIANGLELIST,ntri,p->buffer->buf,stride);
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
	t->tw = tw;
	t->th = th;
	t->rw = pow2(tw);
	t->rh = pow2(th);
	r->device->CreateTexture(t->rw,t->rh,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t->tex);
	if( t->tex == NULL ) {
		delete t;
		return NULL;
	}
	DWORD *ptr;
	int stride;
	int x,y;
	render_texture_lock(t,(void**)&ptr,&stride);
	for(y=0;y<t->rh;y++) {
		for(x=0;x<t->rw;x++)
			ptr[x] = 0xFFFF00FF;
		ptr = (DWORD*)((char*)ptr+stride);
	}
	render_texture_unlock(t);
	return t;
}

/* ------------------------------------------------------------------------ */

void render_texture_free( texture *t ) {
	t->tex->Release();
	delete t;
}

/* ------------------------------------------------------------------------ */

void render_texture_lock( texture *t, void **ptr, int *stride ) {
	D3DLOCKED_RECT lr;
	t->tex->LockRect(0,&lr,NULL,D3DLOCK_NOOVERWRITE | D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD);
	*ptr = lr.pBits;
	*stride = lr.Pitch;
}

/* ------------------------------------------------------------------------ */

void render_texture_unlock( texture *t ) {
	t->tex->UnlockRect(0);
}

/* ------------------------------------------------------------------------ */

void render_texture_size( texture *t, int *w, int *h ) {
	if( w )
		*w = t->tw;
	if( h )
		*h = t->th;
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

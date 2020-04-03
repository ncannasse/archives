/* ************************************************************************ */
/*																			*/
/*	M3D-SE	2D Classes														*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "m3d.h"
#include "m3d_2d.h"
#include "m3d_context.h"

namespace M3D {

/* ------------------------------------------------------------------------ */

inline int z_compare( double z, Group *g ) {
	if( z < g->z_base )
		return 1;
	else if( z > g->z_base )
		return -1;
	else
		return 0;
}

inline int depth_compare( int depth, Element *e ) {
	return e->depth - depth;
}

inline void color_alpha_modified( Element *e ) {
	e->ColorAlphaModified();
}

inline void pos_rot_scale_modified( Element *e ) {
	e->PosRotScaleModified();
}

inline void do_render_alpha( Element *e ) {
	if( e->visible )
		e->Render(true);
}

inline void do_render( Element *e ) {
	if( e->visible )
		e->Render(false);
}

struct _RenderData {
	Variable<MATRIX> m;
	Variable<COLOR> color;
};

/* ------------------------------------------------------------------------ */

#define MAX_COUNT	5000

/*
static const char *shader_code = "\
	vs.1.1 \
	mov r0.xy , v0.xy \
	mov r0.zw , c0.zw \
	mov oPos , r0 \
	mov oD0 , v1 \
	mov oT0.xy , v0.zw \
";
*/

static const char *shader_code = "\
	vs.1.1 \
	mov oPos, v0 \
	mov oD0, v1 \
	mov oT0, v2 \
";

static DWORD shader_decl[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(0,D3DVSDT_FLOAT4),
	D3DVSD_REG(1,D3DVSDT_D3DCOLOR),
	D3DVSD_REG(2,D3DVSDT_FLOAT2),
	D3DVSD_END()
};

struct _2DContext {
	FLOAT cur_z_base;
	TEXTURE cur_texture;
	VERTEX2D *cur_ptr;
	int tot_render;
	int cur_count;
	int start;
	VBUF buffer;
	IBUF indexes;
	DWORD shader2D;
	WBTree<double,Group*> groups; // z_compare

	_2DContext() : groups(z_compare) {
		cur_z_base = 0;
		cur_texture = NULL;
		cur_ptr = NULL;
		tot_render = 0;
		cur_count = 0;
		start = 0;
		buffer = NULL;
		indexes = NULL;
		shader2D = 0;
	}
};

/* ------------------------------------------------------------------------ */

bool Init2D( Context *c ) {
	c->_2d = new _2DContext();
	if( M3DERROR(c->device->CreateVertexBuffer(sizeof(VERTEX2D)*MAX_COUNT*4,D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&c->_2d->buffer),"Init2D:VB") )
		return false;
	if( M3DERROR(c->device->CreateIndexBuffer(sizeof(WORD)*MAX_COUNT*6,D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&c->_2d->indexes),"Init2D:IB") )
		return false;
#ifdef _DEBUG
	void *p;
	c->_2d->buffer->Lock(0,0,(BYTE**)&p,0);
	memset(p,sizeof(VERTEX2D)*MAX_COUNT*4,0);
	c->_2d->buffer->Unlock();
#endif
	WORD *indx = NULL;
	int i;
	c->_2d->indexes->Lock(0,0,(BYTE**)&indx,0);
	for(i=0;i<MAX_COUNT;i++) {
		WORD base = i * 4;
		*indx++ = base+0;
		*indx++ = base+1;
		*indx++ = base+2;
		*indx++ = base+1;
		*indx++ = base+3;
		*indx++ = base+2;
	}
	c->_2d->indexes->Unlock();

	LPD3DXBUFFER shader_buffer = NULL;
	if( M3DERROR(D3DXAssembleShader(shader_code,strlen(shader_code),0,NULL,&shader_buffer,NULL),"Init2D:AS") )
		return false;

	if( M3DERROR(c->device->CreateVertexShader(shader_decl,(DWORD*)shader_buffer->GetBufferPointer(),&c->_2d->shader2D,c->use_hwshaders?0:D3DUSAGE_SOFTWAREPROCESSING),"Init2D:CS") ) {
		shader_buffer->Release();
		return false;
	}

	shader_buffer->Release();
	return true;
}

/* ------------------------------------------------------------------------ */

void Close2D( Context *c ) {
	_2DContext *sc = c->_2d;
	if( sc == NULL )
		return;
	c->_2d = NULL;
	if( sc->buffer != NULL )
		sc->buffer->Release();
	if( sc->indexes != NULL )
		sc->indexes->Release();
	if( sc->shader2D != NULL && c->device != NULL )
		c->device->DeleteVertexShader(sc->shader2D);		
	sc->groups.Delete();
	delete sc;
}

/* ------------------------------------------------------------------------ */

void ForceRender( Context *c ) {
	_2DContext *sc = c->_2d;
	if( sc->cur_count == 0 )
		return;
	//sc->buffer->Unlock();
	c->device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,sc->start*4,sc->cur_count*4,sc->start*6,sc->cur_count*2);
	sc->start += sc->cur_count;
	sc->tot_render += sc->cur_count*2;
	sc->cur_count = 0;
	if( sc->start >= MAX_COUNT ) {
		sc->buffer->Lock(0,0,(BYTE**)&sc->cur_ptr,D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE);
		sc->start = 0;
		sc->cur_count = 0;
	} 
	//else
	//	sc->buffer->Lock(sizeof(VERTEX2D)*4*sc->start,(MAX_COUNT-sc->start)*sizeof(VERTEX2D)*4,(BYTE**)&sc->cur_ptr,D3DLOCK_NOOVERWRITE);
}

void SelectTexture( Context *c, TEXTURE t ) {
	_2DContext *sc = c->_2d;
	if( sc->cur_texture != t ) {
		ForceRender(c);
		sc->cur_texture = t;
		c->device->SetTexture(0,t);
	}
}

/* ------------------------------------------------------------------------ */

void Emit( Context *c, Sprite *s ) {
	_2DContext *sc = c->_2d;
	if( sc->start + sc->cur_count == MAX_COUNT )
		ForceRender(c);
/*
	if( m->_43 != cur_z_base ) {
		VECTOR4 vz;
		ForceRender();
		cur_z_base = m->_43;
		vz.x = 0;
		vz.y = 0;
		vz.z = cur_z_base;
		vz.w = 1;
		device->SetVertexShaderConstant(0,&vz,1);
	}
*/
	SelectTexture(c,s->texture.Available()?s->texture.Data()->t:NULL);

	*sc->cur_ptr++ = s->v[0];
	*sc->cur_ptr++ = s->v[1];
	*sc->cur_ptr++ = s->v[2];
	*sc->cur_ptr++ = s->v[3];
	sc->cur_count++;
}

/* ------------------------------------------------------------------------ */

void Emit( Context *c, Text *t, MATRIX *m ) {
	_2DContext *sc = c->_2d;
	int len = t->text.length();
	if( len > MAX_COUNT )
		len = MAX_COUNT;
	if( sc->start + sc->cur_count + len > MAX_COUNT )
		ForceRender(c);
	Font *f = t->font.Data()->f;
	SelectTexture(c,f->t);

	const unsigned char *text = (const unsigned char *)t->text.c_str();
	DWORD argb = t->argb;
	FLOAT h = -f->height;
	FLOAT *coords = f->coords;
	VERTEX2D v[4];
	int i;
	FLOAT x = 0;
	for(i=0;i<len;i++) {
		FLOAT *c = coords+(text[i]*6);

		if( c[5] == SKIP_CHAR ) 
			x += c[4]+1;
		else {
			x -= c[5];
			v[0] = VERTEX2D(x,h,c[0],c[1],argb);
			v[2] = VERTEX2D(x,0,c[0],c[3],argb);
			x += c[4];
			v[1] = VERTEX2D(x,h,c[2],c[1],argb);
			v[3] = VERTEX2D(x,0,c[2],c[3],argb);
			x += c[5]+1;

			D3DXVec2Transform((VECTOR4*)&v[0],(VECTOR2*)&v[0],m);
			D3DXVec2Transform((VECTOR4*)&v[1],(VECTOR2*)&v[1],m);
			D3DXVec2Transform((VECTOR4*)&v[2],(VECTOR2*)&v[2],m);
			D3DXVec2Transform((VECTOR4*)&v[3],(VECTOR2*)&v[3],m);
			*sc->cur_ptr++ = v[0];
			*sc->cur_ptr++ = v[1];
			*sc->cur_ptr++ = v[2];
			*sc->cur_ptr++ = v[3];
			sc->cur_count++;
		}
	}
}


/* ------------------------------------------------------------------------ */

int Render2D( Context *c, bool alpha ) {
	_2DContext *sc = c->_2d;
	if( !alpha ) {
		sc->tot_render = 0;
		sc->start = 0;
		sc->buffer->Lock(0,0,(BYTE**)&sc->cur_ptr,D3DLOCK_NOOVERWRITE);		
	} else {
		c->device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		c->device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		c->device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		c->device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		c->device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		c->device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		c->device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		c->device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	}
	c->device->SetStreamSource(0,sc->buffer,sizeof(VERTEX2D));
	c->device->SetIndices(sc->indexes,0);
	//device->SetVertexShader(shader2D);
	c->device->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
	c->device->SetTexture(0,NULL);
/*
	VERTEX2D l_data[] = {
		VERTEX2D(0,0,0,0,0xFFABCDEF,0.1),
		VERTEX2D(100,0,0,0,0xFFABCDEF,0.1),
		VERTEX2D(0,100,0,0,0xFFABCDEF,0.1),
		VERTEX2D(100,100,0,0,0xFFABCDEF,0.1),
	};

	WORD l_indx[] = {
		0,
		1,
		2,
		1,
		3,
		2
	};

	if( !alpha ) {
		HRESULT r = device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,4,2,l_indx,D3DFMT_INDEX16,l_data,sizeof(VERTEX2D));
	}
*/
	sc->cur_count = 0;
	sc->cur_texture = NULL;
	sc->cur_z_base = 0;
	if( alpha )
		sc->groups.Iter((WBTree<FLOAT,Group*>::ITERATOR)do_render_alpha);
	else
		sc->groups.RIter((WBTree<FLOAT,Group*>::ITERATOR)do_render);
	ForceRender(c);
	if( alpha ) {
		sc->buffer->Unlock();
		sc->cur_ptr = NULL;
		return sc->tot_render;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

Element::Element( Context *c, Group *parent ) : x(0), y(0), depth(0), visible(true), context(c), color(0xFFFFFF), alpha(1), parent(parent), script_ref(NULL) {
	data = new _RenderData();
	if( parent != NULL )
		parent->AddChild(this);
}

Element::~Element() {
	if( parent != NULL )
		parent->RemoveChild(this);
	if( script_ref != NULL )
		CleanRef(script_ref);
	delete data;
}

void
Element::ColorAlphaModified() {
	data->color.Invalidate();
}

void
Element::PosRotScaleModified() {
	data->m.Invalidate();
}

void
Element::UpdateColor() {
	*data->color.Ref() = COLOR(color);
	data->color.Ref()->a = (FLOAT)alpha;
	if( parent != NULL )
		D3DXColorModulate(data->color.Ref(),parent->data->color.Ref(),data->color.Ref());
}

void
Element::SetDepth( int newdepth ) {
	if( parent != NULL ) {
		parent->RemoveChild(this);
		depth = newdepth;
		parent->AddChild(this);
	}
}

/* ------------------------------------------------------------------------ */

Group::Group( Group *parent ) : Element(parent->context,parent), childs(depth_compare) {
	xscale = 1;
	yscale = 1;
	rotation = 0;
	z_base = 0;
}

Group::Group( Context *c, double z ) : Element(c,NULL), childs(depth_compare) {
	parent = NULL;
	xscale = 1;
	yscale = 1;
	rotation = 0;
	z_base = z;
	context->_2d->groups.Add(z,this);
}

Group::~Group() {
	childs.Delete();
	if( parent == NULL ) {
		Group *g = this;
		if( context->_2d != NULL )
			context->_2d->groups.Remove(z_base,&g);
	}
}

void
Group::Render( bool is_alpha ) {
	if( !data->color.IsValid() ) {
		UpdateColor();
		data->color.Validate();
	}

	if( !data->m.IsValid() ) {
		MATRIX tmp;
		D3DXMatrixRotationZ(&tmp,(FLOAT)rotation);
		if( xscale != 1 || yscale != 1 ) {
			MATRIX tmp2;
			mid(&tmp2);
			tmp2._11 = (FLOAT)xscale;
			tmp2._22 = (FLOAT)yscale;
			mmult(&tmp,&tmp2,&tmp);
		}
		tmp._41 = (FLOAT)x;
		tmp._42 = (FLOAT)y;
		tmp._43 = (FLOAT)z_base;
		if( parent == NULL )
			data->m.Set(tmp);
		else
			mmult(data->m.Ref(),parent->data->m.Ref(),&tmp);
		data->m.Validate();
	}

	if( is_alpha )
		childs.Iter(do_render_alpha);
	else {
		if( data->color.Ref()->a != 1 )
			return;
		childs.RIter(do_render);
	}
}

void
Group::AddChild( Element *e ) {
	childs.Add(e->depth,e);
}

void
Group::RemoveChild( Element *e ) {
	childs.Remove(e->depth,&e);
}

void
Group::ColorAlphaModified() {
	data->color.Invalidate();
	childs.Iter(color_alpha_modified);
}

void
Group::PosRotScaleModified() {
	data->m.Invalidate();
	childs.Iter(pos_rot_scale_modified);
}

MATRIX *
Group::Matrix() {
	return data->m.Ref();
}

void
Group::SetZBase( double newz ) {
	if( parent == NULL ) {
		Group *_this = this;
		context->_2d->groups.Remove(z_base,&_this);
		z_base = newz;
		context->_2d->groups.Add(z_base,this);
		PosRotScaleModified();
	}
}

/* ------------------------------------------------------------------------ */

Sprite::Sprite( Group *parent ) : Element(parent->context,parent) {
	width = 30;
	height = 30;
	tu = 0;
	tv = 0;
	tu2 = 1;
	tv2 = 1;
	modified = true;
}

void
Sprite::Modified() {
	modified = true;
}

Sprite::~Sprite() {
}

void
Sprite::Render( bool is_alpha ) {
	if( !data->color.IsValid() ) {
		UpdateColor();
		data->color.Validate();
		argb = *data->color.Ref();
		if( data->m.IsValid() ) {
			v[0].c = argb;
			v[1].c = argb;
			v[2].c = argb;
			v[3].c = argb;
		}
	}

	if( !data->m.IsValid() ) {
		MATRIX *m = data->m.Ref();
		mid(m);
		m->_41 = (FLOAT)x;
		m->_42 = (FLOAT)y;
		mmult(m,m,parent->Matrix());
		modified = true;
		data->m.Validate();
	}

	if( modified ) {
		MATRIX *m = data->m.Ref();
		FLOAT ttu, ttv, ttu2, ttv2;
		if( texture.Available() ) {
			Resource::TPos *p = &texture.Data()->pos;
			ttu = ((FLOAT)tu) * p->sx + p->x;
			ttu2 = ((FLOAT)tu2) * p->sx + p->x;
			ttv = ((FLOAT)tv) * p->sy + p->y;
			ttv2 = ((FLOAT)tv2) * p->sy + p->y;
		} else {
			ttu = (FLOAT)tu;
			ttv = (FLOAT)tv;
			ttu2 = (FLOAT)tu2;
			ttv2 = (FLOAT)tv2;
		}

		modified = false;

		v[0] = VERTEX2D(0.0f,0.0f,ttu,ttv,argb);
		v[1] = VERTEX2D(width,0,ttu2,ttv,argb);
		v[2] = VERTEX2D(0,height,ttu,ttv2,argb);
		v[3] = VERTEX2D(width,height,ttu2,ttv2,argb);

		D3DXVec2Transform((VECTOR4*)&v[0],(VECTOR2*)&v[0],m);
		D3DXVec2Transform((VECTOR4*)&v[1],(VECTOR2*)&v[1],m);
		D3DXVec2Transform((VECTOR4*)&v[2],(VECTOR2*)&v[2],m);
		D3DXVec2Transform((VECTOR4*)&v[3],(VECTOR2*)&v[3],m);
	}

	COLOR *color = data->color.Ref();
	if( (is_alpha && color->a == 1) || (!is_alpha && color->a != 1) )
		return;

	Emit(context,this);
}

/* ------------------------------------------------------------------------ */

Text::Text( Group *parent ) : Element(parent->context,parent), text("") {
}

Text::~Text() {
}

void
Text::Render( bool is_alpha ) {
	if( !data->color.IsValid() ) {
		UpdateColor();
		data->color.Validate();
		argb = *data->color.Ref();
	}

	if( !data->m.IsValid() ) {
		MATRIX *m = data->m.Ref();
		mid(m);
		m->_41 = (FLOAT)x;
		m->_42 = (FLOAT)y;
		mmult(m,m,parent->Matrix());
		data->m.Validate();
	}

	if( !is_alpha )
		return;

	if( font.Available() )
		Emit(context,this,data->m.Ref());
}

/* ------------------------------------------------------------------------ */
}; // namespace
/* ************************************************************************ */

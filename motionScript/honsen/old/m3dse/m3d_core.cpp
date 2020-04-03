/* ************************************************************************ */
/*																			*/
/*	M3D-SE Core Definitions													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "m3d.h"
#include "m3d_context.h"
#include "m3d_pipeline.h"

extern void m3d_anim_cleanref( void * );

namespace M3D {

/* ------------------------------------------------------------------------ */

typedef Renderer* (*RENDERFUNC) (Mesh*);
typedef ShaderConst* (*SHADERCONSTFUNC) (Mesh*);

struct CoreContext {
	WPairBTree<TAG,TAG_FORMAT> tag_formats;
	WPairBTree<TAG,DWORD> tag_strides;
	WPairBTree<const char*,RENDERFUNC> renderers;
	WPairBTree<const char*,SHADERCONSTFUNC> shaders;
	WList<Mesh*> objects;

	CoreContext() : 
		tag_formats( WPairBTree<TAG,TAG_FORMAT>::equals ),
		tag_strides( WPairBTree<TAG,DWORD>::equals ),
		renderers( strcmp ),
		shaders( strcmp) {
	}

	~CoreContext() {
		objects.Delete();
	}

};

/* ------------------------------------------------------------------------ */

void InitCore( Context *c ) {
	c->core = new CoreContext();
}

void CloseCore( Context *c ) {
	delete c->core;
}

/* ------------------------------------------------------------------------ */

void
TagFormat::RegisterFormat( Context *c, TAG t, TAG_FORMAT fmt ) {
	if( c->core->tag_formats.Exists(t) )
		M3DDEBUG("Tag format for tag '"+MTW::ToString(t)+"' registered twice.");
	else {
		c->core->tag_formats.Add(t,fmt);
		RegisterStride(c,t,StrideOfFormat(fmt));
	}
}

void
TagFormat::RegisterStride( Context *c, TAG t, DWORD stride ) {	
	if( c->core->tag_strides.Exists(t) )
		M3DDEBUG("Tag stride for tag '"+MTW::ToString(t)+"' registered twice.");
	else
		c->core->tag_strides.Add(t,stride);
}

TAG_FORMAT
TagFormat::GetFormat( Context *c, TAG t ) {	
	return (TAG_FORMAT)c->core->tag_formats.Find(t);
}

int
TagFormat::GetStride( Context *c, TAG t ) {
	return c->core->tag_strides.Find(t);
}

int
TagFormat::StrideOfFormat( TAG_FORMAT format ) {
	switch( format ) {
	case TF_FLOAT1: return 4;
	case TF_FLOAT2: return 8;
	case TF_FLOAT3: return 12;
	case TF_FLOAT4: return 16;
	case TF_SHORT2: return 4;
	case TF_SHORT4: return 8;
	case TF_COLOR: return 4;
	default: return 0;
	}
}

/* ------------------------------------------------------------------------ */

void
Renderer::Register( Context *c, Renderer* (*create)(Mesh*) ) {
	Renderer *r = create(NULL);
	c->core->renderers.Add( r->Name(), create );
	delete r;
}

Renderer*
Renderer::New( const char *name, Mesh *o ) {
	Context *c = o->context;
	RENDERFUNC create = c->core->renderers.Find(name);	
	if( create == NULL ) {
		M3DDEBUG("Renderer not found : "+(WString)name);
		return NULL;
	}
	return create(o);
}

/* ------------------------------------------------------------------------ */

class ShaderConstObjectVar : ShaderConst {
	VECTOR4 vtmp;
	MATRIX mtmp;
	WString name;
	Mesh *mesh;
	int id;
public:
	ShaderConstObjectVar( Mesh *o, const char *vname ) {
		name = vname;
		mesh = o;
		id = Mesh::MakeId(vname);
	}

	const char *Name() { return "this." + name; }

	int Size() {
		int sz;
		Mesh::GetData(&vtmp,mesh->script_ref, id, &sz);
		return sz;
	}

	void *Data( Context *c ) {
		int sz;
		void *data = Mesh::GetData(&vtmp,mesh->script_ref,id,&sz);		
		if( sz == 0 )
			return NULL;
		if( sz == 4 ) {
			mtranspose(&mtmp,(MATRIX*)data);
			data = &mtmp;
		}
		return data;
	}
};

void
ShaderConst::Register( Context *c, ShaderConst* (*create)(Mesh*) ) {	
	ShaderConst *sc = create(NULL);
	c->core->shaders.Add( sc->Name(), create );
	delete sc;
}	

ShaderConst*
ShaderConst::New( const char *name, Mesh *o ) {
	SHADERCONSTFUNC create = o->context->core->shaders.Find(name);
	if( create == NULL )
		return (ShaderConst*)new ShaderConstObjectVar(o,name);	
	return create(o);
}

/* ------------------------------------------------------------------------ */

int
Mesh::RenderAll( Context *c, bool is_alpha ) {
	int nfaces = 0;
	FOREACH(Mesh*,c->core->objects);
		nfaces += item->Render(is_alpha);
	ENDFOR;
	return nfaces;
}

void
Mesh::InvalidateAll( Context *c ) {	
	FOREACH(Mesh*,c->core->objects);
		item->Invalidate();
	ENDFOR;
}

void
Mesh::ValidateAll( Context *c ) {	
	FOREACH(Mesh*,c->core->objects);
		item->Validate();
	ENDFOR;
}

bool
Mesh::Prepare( MTW *m ) {
	return PipeImpl::Prepare(m);
}

/* ------------------------------------------------------------------------ */

struct Mesh::Data{
	PipeImpl pipe;
	bool is_alpha;
	Data( Mesh *parent, MTW *t ) : pipe(parent,t), is_alpha(false) {
	}
};

Mesh::Mesh( Context *c, Mesh *parent, MTW *objtag ) : parent(parent), context(c) {
	if( parent == NULL )		
		c->core->objects.Add(this);
	else
		parent->childs.Add(this);

	data = new Data(this,objtag);
	visible = true;
	mid(&pos);
	color = 0xFFFFFF;
	alpha = 1;
}

Mesh::~Mesh() {
	if( parent )
		parent->childs.Remove(this);
	else
		context->core->objects.Remove(this);
	childs.Delete();
	if( script_ref != NULL )
		CleanRef(script_ref);
	delete data;	
}

void
Mesh::UpdatePos() {
	if( !abs_pos.IsValid() ) {
		abs_pos.Validate();
		if( parent ) {
			parent->UpdatePos();
			mmult(abs_pos.Ref(),&pos,parent->abs_pos.Ref());
		} else
			*abs_pos.Ref() = pos;
	}
}

void
Mesh::UpdateColor() {
	if( !abs_color.IsValid() ) {
		abs_color.Validate();
		COLOR c = COLOR(color);
		if( parent ) {
			parent->UpdateColor();
			abs_color.Ref()->r = c.r * parent->abs_color.Ref()->r;
			abs_color.Ref()->g = c.g * parent->abs_color.Ref()->g;
			abs_color.Ref()->b = c.b * parent->abs_color.Ref()->b;
			abs_color.Ref()->a = ((FLOAT)alpha) * parent->abs_color.Ref()->a;
			data->is_alpha = (alpha < 1 || parent->abs_color.Ref()->a < 1);
		} else {
			*abs_color.Ref() = c;
			abs_color.Ref()->a = (FLOAT)alpha;
			data->is_alpha = (alpha < 1);
		}
	}
}

MTW *
Mesh::GetTexture() {
	return data->pipe.GetTexture();
}

void
Mesh::SetTexture( MTW *t ) {
	data->pipe.SetTexture(t);
}

void
Mesh::PosModified() {
	abs_pos.Invalidate();
	FOREACH(Mesh*,childs);
		item->PosModified();
	ENDFOR;
}

void
Mesh::ColorAlphaModified() {
	abs_color.Invalidate();
	FOREACH(Mesh*,childs);
		item->ColorAlphaModified();
	ENDFOR;
}

bool
Mesh::AddRenderer( WString name, bool is_static ) {
	Renderer *r = Renderer::New(name.c_str(),this);
	if( r == NULL )
		return false;
	data->pipe.AddRenderer(r,is_static);
	return true;
}

bool
Mesh::RemoveRenderer( WString name ) {
	return data->pipe.RemoveRenderer(name);
}

void
Mesh::ClearRenderers() {
	data->pipe.CleanRenderers();
}

void
Mesh::SetShader( WString code ) {
	data->pipe.SetShader(code);
}

WString
Mesh::GetShader() {
	return data->pipe.GetShader();
}

bool
Mesh::Validate() {
	return data->pipe.Validate();
}

void
Mesh::Invalidate() {
	data->pipe.Invalidate();
}

bool
Mesh::IsValid() {
	return data->pipe.IsValid();
}

bool
Mesh::HasTag( TAG t ) {
	return data->pipe.HasTag(t);
}

int
Mesh::Render( bool is_alpha ) {
	if( !visible )
		return 0;
	int nfaces = 0;
	if( is_alpha == data->is_alpha )
		nfaces += data->pipe.Render();
	FOREACH(Mesh*,childs);
		nfaces += item->Render(is_alpha);
	ENDFOR;
	return nfaces;
}

void 
Mesh::AddAnim( Anim *a ) {
	data->pipe.AddAnim(a);
}

void
Mesh::RemoveAnim( Anim *a ) {
	data->pipe.RemoveAnim(a);
}

/* ------------------------------------------------------------------------ */

struct Anim::KeyFrame {
	int nframe;
	void *data;
};

Anim::Anim( Mesh *m ) : 
	parent(m),
	adata(NULL), 
	nframes(1), 
	name(""), 
	time(0), 
	loop(true), 
	play(true),
	speed(1),
	keys(NULL),
	next_keys(NULL)
{
}

Anim::~Anim() {
	if( script_ref != NULL )
		CleanRef(script_ref);
	delete keys;
	delete next_keys;
	parent->RemoveAnim(this);
}

bool
Anim::SetData( MTW *m ) {
	MTW *nfrm = m->Child(MAKETAG(nfrm));
	MTW *name = m->Child(MAKETAG(NAME));
	WList<MTW*> frames = m->Childs(MAKETAG(Fram));
	if( nfrm == NULL || name == NULL || frames.Empty() || nfrm->number <= 0 )
		return false;
	FOREACH(MTW*,frames);
		MTW *frnb = item->Child(MAKETAG(frnb));
		MTW *data = item->Child(MAKETAG(DATA));
		if( frnb == NULL || data == NULL || frnb->number < 0 )
			return false;
	ENDFOR;
	adata = m;	
	nframes = nfrm->number;
	time = fmod(time,nframes);
	this->name = (char*)name->data;
	UpdateKeyFrames();
	return true;
}

MTW *
Anim::GetData() {
	return adata;
}

void
Anim::UpdateTime() {
	if( play ) {
		time += parent->context->time->elapsed_time * 25 * speed;
		if( !loop ) {
			if( time < 0 )
				time = 0;
			else if( time > nframes-1 )
				time = nframes - 1;
		} else {
			time = fmod(time,nframes);
			if( time < 0 )
				time += nframes;
		}
	}
}

void
Anim::UpdateKeyFrames() {
	int i;

	delete keys;
	delete next_keys;

	if( adata == NULL ) {
		keys = NULL;
		next_keys = NULL;
		return;
	}

	keys = new KeyFrame[nframes];
	next_keys = new KeyFrame[nframes];

	WList<MTW*> frames = adata->Childs(MAKETAG(Fram));
	FOREACH(MTW*,frames);
		MTW *fdata = item->Child(MAKETAG(DATA));
		int frnb = item->Child(MAKETAG(frnb))->number;
		for(i=frnb;i<nframes;i++) {
			keys[i].data = fdata->data; 
			keys[i].nframe = frnb;
		}
	ENDFOR;

	for(i=0;i<nframes;i++) {
		int oldi = i;
		int curf = keys[i].nframe;
		while( i < nframes && keys[i].nframe == curf )
			i++;
		if( i == nframes ) {
			while( oldi < nframes ) {
				next_keys[oldi] = keys[0];
				oldi++;
			}
		} else {
			while( oldi < i ) {
				next_keys[oldi] = keys[i];
				oldi++;
			}
			i--;
		}
	}
}

void
Anim::TimeCoef2( void **f1, void **f2, void **f3, FLOAT *coef ) {
	int keyf = (int)time;
	KeyFrame *kf1 = &keys[keyf];
	KeyFrame *kf2 = &next_keys[keyf];
	*f1 = kf1->data;
	*f2 = kf2->data;
	if( f3 != NULL )
		*f3 = next_keys[ kf2->nframe ].data;
	if( kf1->nframe == kf2->nframe )
		*coef = 0;
	else
		*coef = (FLOAT)((time - kf1->nframe) / (kf2->nframe - kf1->nframe));	
}

}; // namespace M3D
/* ************************************************************************ */



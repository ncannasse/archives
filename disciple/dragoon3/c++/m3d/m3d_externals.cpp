/* **************************************************************************** */
/*																				*/
/* Caml Bindings for M3D														*/
/*		(c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */

#include <m3d.h>
#include <m3d_res.h>
#include <vfs.h>
#include "m3d_caml.h"

/* **************************************************************************** */

#define NUMPRINTS 35

bool fullscreen = false;
bool init = false;

#ifdef USE_R_THREAD
HANDLE renderThread;
DWORD renderTID;
LOCK render_in;
LOCK render_out;
#endif

FID lastmodel = 0;
char *prints[NUMPRINTS];

bool render_stopped = false;

struct idata {
    
	value action;
	value event;

	idata( value f ) {
		action = 0;
		event = 0;
		register_global_root(&action);
		register_global_root(&event);
		action = f;
	}

	virtual ~idata() {
		remove_global_root(&action);
		remove_global_root(&event);
	}
};

/* **************************************************************************** */
// RENDER

void Print() {
	int i;
	for(i=0;i<NUMPRINTS;i++)
		if( prints[i] )
			M3D::Print(prints[i]);
}

#ifdef USE_R_THREAD

DWORD WINAPI DoRenderFunc( LPVOID ) {
	BEGINLOCK(render_out);
	while(true) {
		BEGINLOCK(render_in);
		ENDLOCK(render_out);

		Print();
		M3D::Render();

		ENDLOCK(render_in);
		M3D::Events::FlushUrgent();
		BEGINLOCK(render_out);
	}
}

#endif

CAMLprim BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved) {
	switch( fdwReason ) {
	case DLL_PROCESS_ATTACH:				
		WMem::Init(false);
		INITLOCK(render_in);
		INITLOCK(render_out);
		memset(prints,0,sizeof(prints));
		M3D::ResCache::Init();
		break;
	case DLL_PROCESS_DETACH:
		DELETELOCK(render_in);
		DELETELOCK(render_out);
		M3D::ResCache::Close();
		break;
	}
	return TRUE;
}

/* **************************************************************************** */
// ALLOC

#define Ptr_val(v)	(*((void**)(v)))
#define Res_val(T,v) ((*((M3D::Res<T>**)Data_custom_val(v)))->Data())

static void
res_free(value v)
{
	M3D::Res<void*>*r = (*((M3D::Res<void*>**)Data_custom_val(v)));
	delete r;
}

extern "C" {
	static struct custom_operations res_ops = {
		"3D-Resource",
		res_free,
		custom_compare_default,
		custom_hash_default,
		custom_serialize_default,
		custom_deserialize_default
	};
};

value alloc_ptr( void *s ) {
    CAMLparam0();
	CAMLlocal1(v);
    v = alloc_small(sizeof(void*) / sizeof(value), Abstract_tag);
    Ptr_val(v) = s;
	CAMLreturn(v);
}

template<class T>
value alloc_res( const M3D::Res<T> &r ) {
    CAMLparam0();
	CAMLlocal1(v);
	v =	alloc_custom(&res_ops,sizeof(void*)/sizeof(value),1,1);
	*((M3D::Res<T>**)Data_custom_val(v)) = new M3D::Res<T>(r);
	CAMLreturn(v);
}

value alloc_some( value v ) {
    CAMLparam1(v);
    CAMLlocal1(val);
    val = alloc(1, 0);
    Store_field(val, 0, v);
    CAMLreturn(val);
}

value alloc_cons( value hd, value tl ) {
	CAMLparam2(hd,tl);
	CAMLlocal1(val);
    val = alloc(2, 0);
    Store_field(val, 0, hd);
    Store_field(val, 1, tl);
	CAMLreturn(val);
}

void m3d_raise( WString msg ) {
	static value *exc = caml_named_value("m3d exception");
	raise_with_string(*exc,(char*)msg.c_str());
}

/* **************************************************************************** */
// M3D addons

void M3D::Log( WString msg ) {
	static value *logfunc = caml_named_value("logfunc");
	callback(*logfunc,copy_string(msg.c_str()));
}

/* **************************************************************************** */

extern "C" {

#define WAIT		{ BEGINLOCK(render_out); BEGINLOCK(render_in); }
#define CONTINUE	{ ENDLOCK(render_in); ENDLOCK(render_out); }

/* **************************************************************************** */
// VECTOR CALC

typedef value vector;
typedef value vector4;

value alloc_vector( VECTOR *v ) {	
	CAMLparam0();
	CAMLlocal1(vv);    
    vv = alloc_small(3 * Double_wosize, Double_array_tag);
    Store_double_field(vv, 0, v->x);
	Store_double_field(vv, 1, v->y);
	Store_double_field(vv, 2, v->z);
	CAMLreturn(vv);
}

value alloc_vector4( VECTOR4 *v ) {
	CAMLparam0();
	CAMLlocal1(vv);
    vv = alloc_small(4 * Double_wosize, Double_array_tag);
    Store_double_field(vv, 0, v->x);
	Store_double_field(vv, 1, v->y);
	Store_double_field(vv, 2, v->z);
	Store_double_field(vv, 3, v->w);
	CAMLreturn(vv);
}

void vector_val( vector vv, VECTOR *v ) {
	v->x = Double_field(vv,0);
	v->y = Double_field(vv,1);
	v->z = Double_field(vv,2);
}

void vector4_val( vector4 vv, FLOAT *v ) {
	v[0] = Double_field(vv,0);
	v[1] = Double_field(vv,1);
	v[2] = Double_field(vv,2);
	v[3] = Double_field(vv,3);
}

CAMLprim value point_line_dist( vector va, vector vb, vector vp ) {
	VECTOR a,b,p;
	vector_val(va,&a);
	vector_val(vb,&b);
	vector_val(vp,&p);
	VECTOR delta = b-a;
	VECTOR u = p-a;
	FLOAT lmag = D3DXVec3Length(&delta);
	FLOAT umag = (u.x*delta.x + u.y*delta.y + u.z*delta.z) / (lmag * lmag);
	VECTOR inter( a.x + umag*delta.x, a.y + umag*delta.y, a.z + umag*delta.z );
	FLOAT dist = D3DXVec3Length(&(inter-p))*((umag < 0.0f || umag > 1.0f)?-1:1);
	return copy_double(dist);
}

/* **************************************************************************** */
// MATRIX CALC

typedef value matrix;
typedef value vfloat;

#define Matrix_val(v) (*(MATRIX*)(v))	

value alloc_matrix( MATRIX *m ) {
	CAMLparam0();
	CAMLlocal1(v);    
    v = alloc_small(sizeof(MATRIX)/sizeof(value), Abstract_tag);
    Matrix_val(v) = *m;
	CAMLreturn(v);
}

CAMLprim value mat_id( value _ ) {
	MATRIX r;
	mid(&r);
	return alloc_matrix(&r);
}

CAMLprim value mat_mult( matrix a, matrix b ) {	
	MATRIX r;
	mmult(&r,&Matrix_val(a),&Matrix_val(b));
	return alloc_matrix(&r);
}

CAMLprim value mat_rot( vector vv ) {
	MATRIX r;
	VECTOR v;
	vector_val(vv,&v);
	mrot(&r,v.x,v.y,v.z);
	return alloc_matrix(&r);
}

CAMLprim value mat_trans( vector vv ) {
	MATRIX r;
	VECTOR v;
	vector_val(vv,&v);
	mtrans(&r,v.x,v.y,v.z);
	return alloc_matrix(&r);
}

CAMLprim value mat_scale( vector vv ) {
	MATRIX r;
	VECTOR v;
	vector_val(vv,&v);
	mscale(&r,v.x,v.y,v.z);
	return alloc_matrix(&r);
}

CAMLprim value mat_trs( vector vt, vector vr, vector vs ) {
	MATRIX m,mtmp;
	VECTOR v;
	vector_val(vt,&v);
	mtrans(&mtmp,v.x,v.y,v.z);
	vector_val(vs,&v);
	mscale(&m,v.x,v.y,v.z);
	mmult(&m,&m,&mtmp);
	vector_val(vr,&v);
	mrot(&mtmp,v.x,v.y,v.z);
	mmult(&m,&mtmp,&m);
	return alloc_matrix(&m);
}

CAMLprim value mat_ref_eval( MATRIX *m ) {
	return alloc_matrix(m);
}

/* **************************************************************************** */
// RENDER CONTROL

CAMLprim value m3d_handle_events( value _ );

CAMLprim value m3d_render_stop( value _ ) {
	if( !render_stopped ) {
		WAIT;
		render_stopped = true;
	}
	return Val_unit;
}

CAMLprim value m3d_render_start( value _ ) {
	if( render_stopped ) {
		render_stopped = false;
		CONTINUE;		
	}
	return Val_unit;
}

CAMLprim value m3d_update_time( value _ ) {
	M3D::UpdateTime();
	return Val_unit;
}


CAMLprim value m3d_set_world_transform( matrix m ) {
	M3D::transform.Set( Matrix_val(m) );
	return Val_unit;
}

CAMLprim value m3d_do_world_transform( matrix m ) {
	mmult(M3D::transform.Get(),M3D::transform.Get(),&Matrix_val(m));
	M3D::transform.Invalidate();
	return Val_unit;
}

CAMLprim value m3d_get_world_transform( value _ ) {
	return alloc_matrix(M3D::transform.Get());
}

CAMLprim value m3d_get_render_time( value _ ) {
	return copy_double(M3D::curtime);
}

CAMLprim value m3d_get_elapsed_time( value _ ) {
	return copy_double(M3D::elapsed_time);
}

CAMLprim value m3d_print( value str ) {
	int i;	
	char *old = prints[0];
	prints[0] = NULL;
	WAIT;
	/** memory leak is caused by CRT accumulation **/
	delete old;
	for(i=1;i<NUMPRINTS;i++)
		prints[i-1] = prints[i];
	prints[NUMPRINTS-1] = strdup(String_val(str));
	CONTINUE;
	return Val_unit;
}

CAMLprim value m3d_clear_print( value str ) {
	int i;
	for(i=0;i<NUMPRINTS;i++) {
		delete prints[i];
		prints[i] = NULL;
	}	
	return Val_unit;
}

/* **************************************************************************** */
// INIT / CLOSE / SETUP

CAMLprim value m3d_set_render_target( value v ) {
	M3D::SetConfig(Handle_val(v),fullscreen);
	return Val_unit;
}

CAMLprim value m3d_init( value v, value vsync ) {
	m3d_render_stop(Val_unit);
	m3d_set_render_target(v);
	if( !init ) {
		init = true;
#ifdef USE_R_THREAD
		renderThread = CreateThread(NULL,0,DoRenderFunc,NULL,0,&renderTID);	
		if( renderThread == NULL )
			return Val_false;
		SetThreadPriority(renderThread,THREAD_PRIORITY_BELOW_NORMAL);
#endif
	}	
	return Val_bool(M3D::Init(Bool_val(vsync)));
}

CAMLprim value m3d_close( value v ) {
	int i;
	m3d_render_stop(Val_unit);
#ifdef USE_R_THREAD
	TerminateThread(renderThread,0);
#endif
	init = false;
	M3D::Close();
	for(i=0;i<NUMPRINTS;i++)
		if( prints[i] ) {
			delete prints[i];
			prints[i] = NULL;
		}
	return Val_unit;
}

CAMLprim m3d_do_garbage( value _ ) {
	WAIT;
	M3D::Res<void*>::Garbage();
	CONTINUE;
	return Val_unit;
}

CAMLprim value m3d_pick( value pos ) {
	CAMLparam1(pos);
	CAMLlocal1(list);	
	WList<M3D::Inst*> found;
	list = Val_int(0);
	M3D::Inst::Pick(
		Double_val(Field(pos,0)),
		Double_val(Field(pos,1)),
		&found);
	FOREACH(M3D::Inst*,found);
		list = alloc_cons( alloc_res(M3D::Res<M3D::Inst*>::Share((int)item,item,M3D::Inst::Free)), list );
	ENDFOR;
	CAMLreturn(list);
}

CAMLprim value m3d_projection( value pos, value z ) {
	CAMLparam2(pos,z);
	CAMLlocal3(result,vx,vy);
	VECTOR2 v = M3D::Project( Double_val(Field(pos,0)), Double_val(Field(pos,1)), Double_val(z));
	vx = copy_double(v.x);
	vy = copy_double(v.y);
	result = alloc_small(2,0);
	Field(result,0) = vx;
	Field(result,1) = vy;
	CAMLreturn(result);
}

CAMLprim value m3d_set_option( value opt ) {
	switch( Is_long(opt)?0:Wosize_val(opt) ) {
	case 1: {
		value v = Field(opt,0);
		switch( Tag_val(opt) ) {
		case 0:	M3D::show_infos = Bool_val(v); break;
		case 1: M3D::do_shading = Bool_val(v); break;
		case 2: M3D::bgcolor = D3DCOLOR_XRGB(Int_val(Field(v,0)),Int_val(Field(v,1)),Int_val(Field(v,2)));
		case 3: fullscreen = Bool_val(v); break;
		default:
			failwith("invalid option");
			break;
		}}
		break;
	default:
		failwith("invalid option");
		break;
	}

	return Val_unit;
}

CAMLprim value m3d_draw_line( value v1, value v2, value vcol ) {
	VECTOR p1,p2;
	vector_val(v1,&p1);
	vector_val(v2,&p2);
	M3D::Infos::DrawLine( Int32_val(vcol), &p1, &p2 );
	return Val_unit;
}

/* **************************************************************************** */
// INSTANCE CONTROL

#define Inst_val(v) Res_val(M3D::Inst*,v)

CAMLprim value m3d_inst_attach( value id ) {
	WAIT;
	M3D::Res<M3D::Mesh*> r = M3D::Res<M3D::Mesh*>::Load(Int_val(id),Vfs::ftype("MTW"),M3D::Mesh::Load,M3D::Mesh::Free);
	if( !r.IsLoaded() ) {
		CONTINUE;
		m3d_raise("Failed to load MTW file #"+itos(Int_val(id)));
	}
	M3D::Inst *i = r.Data()->CreateInstance();
	CONTINUE;
	return alloc_res( M3D::Res<M3D::Inst*>::Share((int)i,i,M3D::Inst::Free) );
}

CAMLprim value m3d_inst_detach( value inst ) {
	WAIT;
	Inst_val(inst)->Detach();
	CONTINUE;
	return Val_unit;
}

CAMLprim value m3d_inst_anim_list( value inst ) {
	CAMLparam1(inst);
	CAMLlocal1(result);
	WList<const char*> names;
	Inst_val(inst)->AnimList(&names);
	result = Val_int(0);
	FOREACH(const char*,names);
		result = alloc_cons( copy_string(item), result);
	ENDFOR;
	CAMLreturn(result);
}

CAMLprim value m3d_inst_set_texture( value inst, value texture ) {
	WAIT;
	WString realfile;
	TEXTURE t;
	if( !Vfs::get_hlink(Int_val(texture),&realfile) ) {
		CONTINUE;
		m3d_raise("This texture has not and hard link (#"+itos(Int_val(texture))+")");
	} else {
		t = M3D::Res<TEXTURE>::Load(Int_val(texture),Vfs::ftype("PIC"),M3D::Material::LoadTexture,M3D::Material::FreeTexture).Data();
		if( !t ) {
			CONTINUE;
			m3d_raise("Failed to load texture "+realfile+" (#"+itos(Int_val(texture))+")");
		}
		Inst_val(inst)->SetTexture(t);
	}
	CONTINUE;
	return Val_unit;
}

CAMLprim value m3d_inst_set_current_anim( value inst, value anim ) {
	if( !Inst_val(inst)->SetAnim( Is_some(anim)?String_val(Value_of(anim)):NULL ) )
		m3d_raise("Invalid anim for this Object");
	return Val_unit;
}

CAMLprim value m3d_inst_set_color( value inst, value color ) {
	Inst_val(inst)->color = Int32_val(color);
	return Val_unit;
}

CAMLprim value m3d_inst_get_time( value inst ) {
	return copy_double(Inst_val(inst)->GetAnimTime());
}

CAMLprim value m3d_inst_set_time( value inst, value vtime ) {
	Inst_val(inst)->SetAnimTime(Double_val(vtime));
	return Val_unit;
}

CAMLprim value m3d_inst_get_speed( value inst ) {
	return copy_double( Inst_val(inst)->GetAnimSpeed() );
}

CAMLprim value m3d_inst_set_speed( value inst, value speed ) {
	Inst_val(inst)->SetAnimSpeed( Double_val(speed) );
	return Val_unit;
}

CAMLprim value m3d_inst_reset( value inst ) {
	WAIT;
	Inst_val(inst)->ResetAnim();
	CONTINUE;
	return Val_unit;
}

CAMLprim value m3d_inst_color_fade( value inst, value color, value time ) {	
	Inst_val(inst)->Fade(Int32_val(color),Double_val(time));
	return Val_unit;
}

CAMLprim value m3d_inst_set_dest( value inst, value dest, value time ) {
	Inst_val(inst)->SetDest(&Matrix_val(dest),Double_val(time));
	return Val_unit;
}

CAMLprim value m3d_inst_is_visible( value inst ) {
	return Val_bool( Inst_val(inst)->visible );
}

CAMLprim value m3d_inst_set_visible( value inst, value v ) {
	Inst_val(inst)->visible = Bool_val(v);
	if( Inst_val(inst)->visible )
		Inst_val(inst)->Invalidate();
	return Val_unit;
}

CAMLprim value m3d_inst_get_pos( value inst ) {
	return (value)Inst_val(inst)->Position();
}

CAMLprim value m3d_inst_set_pos( value inst, value pos ) {
	Inst_val(inst)->SetPosition(&Matrix_val(pos));
	return Val_unit;
}

CAMLprim value m3d_inst_do_transform( value inst, value trans ) {
	Inst_val(inst)->Transform(&Matrix_val(trans));	
	return Val_unit;
}

/* **************************************************************************** */
// CAMERA CONTROL

CAMLprim value m3d_camera_zoom( vfloat x ) {
	WAIT;
	M3D::Camera::Zoom( Double_val(x) );
	CONTINUE;
	return Val_unit;
}

CAMLprim value m3d_camera_reset( value _ ) {
	WAIT;
	M3D::Camera::Reset();
	CONTINUE;
	return Val_unit;
}

CAMLprim value m3d_camera_set_pos( vector vv ) {
	vector_val(vv,M3D::Camera::pos.Get());
	M3D::Camera::pos.Invalidate();
	return Val_unit;
}

CAMLprim value m3d_camera_set_target( vector vv ) {
	vector_val(vv,M3D::Camera::target.Get());
	M3D::Camera::target.Invalidate();
	return Val_unit;
}

CAMLprim value m3d_camera_get_pos( value _ ) {
	return alloc_vector(M3D::Camera::pos.Get());
}

CAMLprim value m3d_camera_get_target( value _ ) {
	return alloc_vector(M3D::Camera::target.Get());
}

CAMLprim value m3d_camera_goto( value vpos, value vtarget, value vtime ) {
	VECTOR pos;
	VECTOR target;
	vector_val(vpos,&pos);
	vector_val(vtarget,&target);
	M3D::Camera::Goto(pos,target,Double_val(vtime));
	return Val_unit;
}

CAMLprim value m3d_camera_stop( value _ ) {
	M3D::Camera::Stop();
	return Val_unit;
}

/* **************************************************************************** */
// SPRITES

#define Sprite_val(v)	Res_val(M3D::Sprite*,v)

CAMLprim value m3d_sprite_attach( value tex, vector4 vcoords ) {	
	M3D::Res<TEXTURE> t = M3D::Res<TEXTURE>::Load(Int_val(tex),Vfs::ftype("PIC"),M3D::Material::LoadTexture,M3D::Material::FreeTexture);
	if( !t.IsLoaded() )
		m3d_raise("Invalid texture for sprite");
	FRECT coords;
	vector4_val(vcoords,(FLOAT*)&coords);
	WAIT;
	M3D::Sprite *s = new M3D::Sprite(t.Data());
	s->SetCoords(&coords);
	CONTINUE;
	return alloc_res(M3D::Res<M3D::Sprite*>::Share((int)s,s,M3D::Sprite::Free));
}

CAMLprim value m3d_sprite_set_pos( value sprite, value pos ) {
	VECTOR v;
	vector_val(pos,&v);
	Sprite_val(sprite)->pos = v;
	if( Sprite_val(sprite)->GetDepth() != v.z ) {
		WAIT;
		Sprite_val(sprite)->SetDepth(v.z);
		CONTINUE;
	}
	return Val_unit;
}

CAMLprim value m3d_sprite_get_pos( value sprite ) {
	M3D::Sprite *s = Sprite_val(sprite);
	VECTOR v(s->pos.x,s->pos.y,s->GetDepth());
	return alloc_vector( &v );
}

CAMLprim value m3d_sprite_set_size( value sprite, value size ) {
	Sprite_val(sprite)->size = VECTOR2(
		Double_val(Field(size,0)),
		Double_val(Field(size,1)));	
	return Val_unit;
}

CAMLprim value m3d_sprite_get_size( value sprite ) {
	CAMLparam1(sprite);
	CAMLlocal1(size);
    size = alloc_tuple(2);
    Field(size,0) = copy_double(Sprite_val(sprite)->size.x);
	Field(size,1) = copy_double(Sprite_val(sprite)->size.y);
	CAMLreturn(size);
}

CAMLprim value m3d_sprite_set_coords( value sprite, vector4 vcoords ) {
	FRECT coords;	
	vector4_val(vcoords,(FLOAT*)&coords);
	WAIT;
	Sprite_val(sprite)->SetCoords(&coords);
	CONTINUE;
	return Val_unit;
}

CAMLprim value m3d_sprite_get_coords( value sprite ) {
	FRECT coords;
	Sprite_val(sprite)->GetCoords(&coords);
	return alloc_vector4((VECTOR4*)&coords);
}

CAMLprim value m3d_sprite_detach( value sprite ) {
	WAIT;
	Sprite_val(sprite)->Detach();
	CONTINUE;
	return Val_unit;
}

CAMLprim value m3d_sprite_set_dest( value sprite, value pos, value time ) {
	VECTOR2 v( Double_val(Field(pos,0)),Double_val(Field(pos,1)) );
	Sprite_val(sprite)->SetDestination(&v,Double_val(time));
	return Val_unit;
}

CAMLprim value m3d_sprite_set_fade( value sprite, value color, value time ) {
	Sprite_val(sprite)->SetFade(Int32_val(color),Double_val(time));
	return Val_unit;
}

CAMLprim value m3d_sprite_set_color( value sprite, value color ) {
	Sprite_val(sprite)->color = Int32_val(color);
	return Val_unit;
}

/* **************************************************************************** */
// SPECIAL FX

#define Gfx_val(v) (Tag_val(v)?Res_val(M3D::Inst*,Field(v,0)):Res_val(M3D::Sprite*,Field(v,0)))

TEXTUREOP texture_op[] = {
    D3DTOP_SELECTARG1,
    D3DTOP_SELECTARG2,
    D3DTOP_MODULATE,
    D3DTOP_MODULATE2X,
    D3DTOP_MODULATE4X,
    D3DTOP_ADD,
    D3DTOP_ADDSIGNED,
    D3DTOP_ADDSIGNED2X,
    D3DTOP_SUBTRACT,
    D3DTOP_ADDSMOOTH,
    D3DTOP_BLENDDIFFUSEALPHA,
    D3DTOP_BLENDTEXTUREALPHA,
	/* color op only */
    D3DTOP_MODULATEALPHA_ADDCOLOR,
    D3DTOP_MODULATECOLOR_ADDALPHA,
    D3DTOP_MODULATEINVALPHA_ADDCOLOR,
    D3DTOP_MODULATEINVCOLOR_ADDALPHA,
};

BLEND blend_factor[] = {
    D3DBLEND_ZERO,
    D3DBLEND_ONE,
    D3DBLEND_SRCCOLOR,
    D3DBLEND_INVSRCCOLOR,
    D3DBLEND_SRCALPHA,
    D3DBLEND_INVSRCALPHA,
    D3DBLEND_DESTALPHA,
    D3DBLEND_INVDESTALPHA,
    D3DBLEND_DESTCOLOR,
    D3DBLEND_INVDESTCOLOR,
    D3DBLEND_SRCALPHASAT,
};

CAMLprim value m3d_load_library( value libname ) {
	WAIT;
	bool b = M3D::FX::LoadLib(String_val(libname));
	CONTINUE;
	return Val_bool(b);
}

CAMLprim value m3d_get_fx_names( value _ ) {
	CAMLparam1(_);
	CAMLlocal1(result);
	WList<WString> names;
	M3D::FX::Names(&names);
	result = Val_int(0);
	FOREACH(WString,names);
		result = alloc_cons( copy_string(item.c_str()), result);
	ENDFOR;
	CAMLreturn(result);
}

CAMLprim value m3d_fx_play( value name, value vparams ) {
	WList<M3D::FX::FXParam*> params;
	while( vparams != Val_int(0) ) {
		const char *name = String_val(Field(Field(vparams,0),0));
		value v = Field(Field(vparams,0),1);
		vparams = Field(vparams,1);
		M3D::FX::FXParam *p = new M3D::FX::FXParam();
		p->name = name;		
		switch( Tag_val(v) ) {
/*
		| PInt of int
		| PFloat of float
		| PBool of bool
		| PMatrix of Matrix.t
		| PVector of vector
		| PMatrixRef of Matrix.ref
*/
		case 0:
			p->type = M3D::FX::FXParam::PINT;
			p->data = (void*)Int_val(Field(v,0));
			break;
		case 1:
			{
				FLOAT f = Double_val(Field(v,0));
				p->type = M3D::FX::FXParam::PFLOAT;
				p->data = (void*)*(DWORD*)(FLOAT*)&f;
			}
			break;
		case 2:
			p->type = M3D::FX::FXParam::PBOOL;
			p->data = (void*)Bool_val(Field(v,0));
			break;
		case 3:
			p->type = M3D::FX::FXParam::PMATRIX;
			p->data = new MATRIX();
			*((MATRIX*)p->data) = Matrix_val(Field(v,0));
			break;
		case 4:
			p->type = M3D::FX::FXParam::PVECTOR;
			p->data = new VECTOR();
			vector_val(Field(v,0),((VECTOR*)p->data));
			break;
		case 5:
			p->type = M3D::FX::FXParam::PMATREF;
			p->data = (void*)Field(v,0);
			break;
		default:
			delete p;
			params.Delete();
			m3d_raise("Unknown fx tag param");
			break;
		}
		params.Add(p);
	}

	WAIT;
	bool b = M3D::FX::Play(String_val(name),&params);
	CONTINUE;

	FOREACH(M3D::FX::FXParam*,params);
		switch( item->type ) {
		case M3D::FX::FXParam::PMATRIX: delete ((MATRIX*)item->data); break;
		case M3D::FX::FXParam::PVECTOR: delete ((VECTOR*)item->data); break;
		default: break;
		}
	ENDFOR;
	params.Delete();
	if( !b )
		m3d_raise("Error playing FX#"+(WString)String_val(name));
	return Val_unit;
}

CAMLprim value m3d_fx_color_op( value inst, value op ) {
	switch( Tag_val(inst) ) {
	case 0: Res_val(M3D::Sprite*,Field(inst,0))->fx_color = texture_op[Int_val(op)];
	default: Res_val(M3D::Inst*,Field(inst,0))->fx_color = texture_op[Int_val(op)];
	}
	return Val_unit;
}

CAMLprim value m3d_fx_alpha_op( value inst, value op ) {
	switch( Tag_val(inst) ) {
	case 0: Res_val(M3D::Sprite*,Field(inst,0))->fx_alpha = texture_op[Int_val(op)];
	default: Res_val(M3D::Inst*,Field(inst,0))->fx_alpha = texture_op[Int_val(op)];
	}
	return Val_unit;
}

CAMLprim value m3d_fx_src_blend( value inst, value op ) {
	switch( Tag_val(inst) ) {
	case 0: Res_val(M3D::Sprite*,Field(inst,0))->fx_src = blend_factor[Int_val(op)];
	default: Res_val(M3D::Inst*,Field(inst,0))->fx_src = blend_factor[Int_val(op)];
	}
	return Val_unit;
}

CAMLprim value m3d_fx_dst_blend( value inst, value op ) {
	switch( Tag_val(inst) ) {
	case 0: Res_val(M3D::Sprite*,Field(inst,0))->fx_dst = blend_factor[Int_val(op)];
	default: Res_val(M3D::Inst*,Field(inst,0))->fx_dst = blend_factor[Int_val(op)];
	}
	return Val_unit;
}

/* **************************************************************************** */
// TEXT & FONT

#define Font_val(v) Res_val(M3D::Text::Font*,v)

CAMLprim value m3d_text_create_font( value font, value height ) {
	M3D::Text::Font *f = M3D::Text::CreateFont( Int_val(height), String_val(font), false );
	if( f == NULL )
		m3d_raise("CreateFont");
	return alloc_res(M3D::Res<M3D::Text::Font*>::Share((FID)f,f,M3D::Text::FreeFont));
}

CAMLprim value m3d_text_create( value font ) {
	M3D::Text::TData r = M3D::Text::Create( Font_val(font) );
	return Val_int(r.Ref());
}

CAMLprim value m3d_text_remove( value text ) {
	M3D::Text::Remove( Int_val(text) );
	return Val_unit;
}

CAMLprim value m3d_text_set_string( value text, value string ) {
	M3D::Text::TData t = M3D::Text::TData(Int_val(text));
	t.SetString(String_val(string));
	return Val_unit;
}

CAMLprim value m3d_text_set_color( value text, value color ) {
	M3D::Text::TData t = M3D::Text::TData(Int_val(text));
	t.SetColor(Int32_val(color));
	return Val_unit;
}

CAMLprim value m3d_text_set_pos( value text, value pos ) {
	M3D::Text::TData t = M3D::Text::TData(Int_val(text));
	VECTOR v;
	vector_val(pos,&v);
	t.SetPos(&v);
	return Val_unit;
}

CAMLprim value m3d_text_set_dest( value text, value dst, value time ) {
	M3D::Text::TData t = M3D::Text::TData(Int_val(text));
	t.SetDest( Double_val(Field(dst,0)), Double_val(Field(dst,1)), Double_val(time) );
	return Val_unit;
}

CAMLprim m3d_text_set_center( value text, value iscenter ) {
	M3D::Text::TData t = M3D::Text::TData(Int_val(text));
	t.SetCenter( Bool_val(iscenter) );
	return Val_unit;
}

/* **************************************************************************** */
// EVENTS 

CAMLprim value m3d_inst_set_event( value inst, value func ) {
	CAMLparam2(inst,func);
	idata *data = (idata*)Inst_val(inst)->customdata;

	if( func == Val_none ) {
		if( data )
			data->event = 0;
	}
	else {
		if( data )
			data->event = func;
		else		
			Inst_val(inst)->customdata = data = new idata(Value_of(func));
	}
	CAMLreturn(Val_unit);
}

CAMLprim value m3d_inst_set_action( value inst, value func ) {
	CAMLparam2(inst,func);
	idata *data = (idata*)Inst_val(inst)->customdata;
	if( data )
		data->action = func;
	else
		Inst_val(inst)->customdata = new idata(func);
	CAMLreturn(Val_unit);
}

CAMLprim value m3d_inst_do_action( value inst ) {
	idata *data = (idata*)Inst_val(inst)->customdata;
	if( data && data->action )		
		callback(data->action,Val_unit);
	return Val_unit;
}

CAMLprim value m3d_event_add_delayed( value func, value time ) {
	M3D::Events::Delayed(Double_val(time),M3D::Events::E_CALLFUNC,new idata(func));
	return Val_unit;
}

CAMLprim value m3d_handle_events( value _ ) {
	if( render_stopped )
		M3D::Res<void*>::Garbage();
	M3D::Events::EVENT e;
	void *obj;

#ifndef USE_R_THREAD
	Print();
	M3D::Render();
	M3D::Camera::Update();
#endif

	M3D::Events::Process();
	while( M3D::Events::Get(&e,&obj) ) {
		switch( e ) {
		case M3D::Events::E_ENDANIM:
			{
				idata *data = (idata*)((M3D::Inst*)obj)->customdata;
				if( data && data->event )
					callback(data->event,Val_int(0));
			}
			break;
		case M3D::Events::E_CALLFUNC:
			{
				idata *data = (idata*)obj;
				value r = callback_exn(data->action,Val_unit);
				delete data;
				if( Is_exception_result(r) ) {
					M3D::Events::Next();
					mlraise(Extract_exception(r));
				}
			}
			break;
		default:
			M3DTRACE("Unknown event occurs : #"+itos(e));
			break;
		}
		M3D::Events::Next();
	}
	return Val_unit;
}

/* **************************************************************************** */

}
/* ************************************************************************ */
/*																			*/
/*	M3D-SE Mesh Anim for MTS												*/
/*	Nicolas Cannasse														*/
/*	(c)2004 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "script.h"
#include "../../common/mtw.h"

#define val_anim(o)		((M3D::Anim*)val_odata(o))

DEFINE_CLASS(m3dse,anim);

void anim_delete( value o ) {
	if( val_anim(o) )
		val_anim(o)->script_ref = NULL;
}

void M3D::Anim::CleanRef( void *ref ) {
	val_odata(ref) = NULL;
	val_otype(ref) = NULL;
	val_gc((value)ref,NULL);
}

value anim_new1( value m ) {
	value o = val_this();
	val_check_obj(o,t_anim);
	val_check_obj(m,t_mesh);

	M3D::Mesh *mesh = val_mesh(m);
	M3D::Anim *a = new M3D::Anim(mesh);
	mesh->AddAnim(a);
	val_odata(o) = (value)a;
	a->script_ref = o;
	val_gc(o,anim_delete);
	return o;
}

value anim_play() {
	value o = val_this();
	val_check_obj(o,t_anim);
	val_anim(o)->play = true;
	return val_true;
}

value anim_stop() {
	value o = val_this();
	val_check_obj(o,t_anim);
	val_anim(o)->play = false;
	return val_true;
}

value anim_is_playing() {
	value o = val_this();
	val_check_obj(o,t_anim);
	return alloc_bool(val_anim(o)->play);
}

value anim_remove() {
	value o = val_this();
	val_check_obj(o,t_anim);
	delete val_anim(o);		
	return val_true;
}

value anim_get_anim() {
	value o = val_this();
	val_check_obj(o,t_anim);
	return alloc_resource(val_anim(o)->GetData());
}

value anim_set_anim( value r ) {
	value o = val_this();
	val_check_obj(o,t_anim);
	val_check_obj(r,t_resource);

	M3D::Anim *a = val_anim(o);
	MTW *data = (MTW*)val_resource(r);

	return alloc_bool( a->SetData(data) );
}

value anim_get_frame() {
	value o = val_this();
	val_check_obj(o,t_anim);
	return alloc_float( val_anim(o)->time );
}

value anim_set_frame( value f ) {
	value o = val_this();
	val_check_obj(o,t_anim);
	if( !val_is_number(f) )
		return NULL;
	val_anim(o)->time = fmod( val_number(f) , (FLOAT)val_anim(o)->nframes );
	if( val_anim(o)->time < 0 )
		val_anim(o)->time += val_anim(o)->nframes;
	return alloc_float( val_anim(o)->time );
}

value anim_get_framecount() {
	value o = val_this();
	val_check_obj(o,t_anim);
	if( val_anim(o)->GetData() == NULL )
		return alloc_int(0);
	return alloc_int( val_anim(o)->nframes );
}

value anim_get_speed() {
	value o = val_this();
	val_check_obj(o,t_anim);
	return alloc_float( val_anim(o)->speed );
}

value anim_set_speed( value f ) {
	value o = val_this();
	val_check_obj(o,t_anim);
	if( !val_is_number(f) )
		return NULL;
	val_anim(o)->speed = val_number(f);
	return f;
}

value anim_get_loop() {
	value o = val_this();
	val_check_obj(o,t_anim);
	return alloc_bool( val_anim(o)->loop );
}

value anim_set_loop( value l ) {
	value o = val_this();
	val_check_obj(o,t_anim);
	if( !val_is_bool(l) )
		return NULL;
	val_anim(o)->loop = val_bool(l);
	return l;
}

value m3dse_anim() {
	value o = alloc_class(&t_anim);
	Constr(o,anim,Anim,1);
	Method(o,anim,play,0);
	Method(o,anim,stop,0);
	Method(o,anim,is_playing,0);
	Method(o,anim,remove,0);
	Method(o,anim,get_framecount,0);
	Property(o,anim,anim);
	Property(o,anim,frame);	
	Property(o,anim,speed);
	Property(o,anim,loop);
	return o; 
}

/* ************************************************************************ */

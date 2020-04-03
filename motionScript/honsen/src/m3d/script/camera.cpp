/* ************************************************************************ */
/*																			*/
/*	M3D-SE Camera for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <math.h>
#include "script.h"
#include "../m3d_priv.h"

#define val_camera(o) ((camera*)val_odata(o))

static vector defpos(-40,-60,70);
static vector deftarget(0,0,20);
static vector defworldup(0,0,1);

DEFINE_CLASS(honsen,camera);

static value camera_new0() {
	value o = val_this();
	val_check_obj(o,t_camera);	
	camera *c = (camera*)alloc_small_no_scan(sizeof(camera));
	c->pos = defpos;
	c->target = deftarget;
	c->worldup = defworldup;
	c->zoom = 1;
	c->roll = 0;
	matrix3d_identity(&c->world);
	float ratio = M3D()->width * 1.0f / M3D()->height;
	matrix3d_proj(&c->proj,3.1416f/4,ratio,1,1000);
	val_odata(o) = (value)c;
	return o;
}

static value camera_get_pos() {
	value o = val_this();
	value v;
	val_check_obj(o,t_camera);
	v = alloc_vector(NULL);
	val_odata(v) = (value)&val_camera(o)->pos;
	return v;
}

static value camera_get_target() {
	value o = val_this();
	value v;
	val_check_obj(o,t_camera);
	v = alloc_vector(NULL);
	val_odata(v) = (value)&val_camera(o)->target;
	return v;
}

static value camera_get_roll() {
	value o = val_this();
	val_check_obj(o,t_camera);
	return alloc_float( val_camera(o)->roll );
}

static value camera_get_proj() {
	value o = val_this();
	val_check_obj(o,t_camera);
	return alloc_matrix( &val_camera(o)->proj );
}

static value camera_get_zoom() {
	value o = val_this();
	val_check_obj(o,t_camera);
	return alloc_float( val_camera(o)->zoom );
}

static value camera_set_pos( value v ) {
	value o = val_this();
	val_check_obj(o,t_camera);
	val_check_obj(v,t_vector);
	camera *c = val_camera(o);
	c->pos = *val_vector(v);
	c->modified = true;
	return v;
}

static value camera_set_target( value v ) {
	value o = val_this();
	val_check_obj(o,t_camera);
	val_check_obj(v,t_vector);
	camera *c = val_camera(o);
	c->target = *val_vector(v);
	c->modified = true;
	return v;
}

static value camera_set_roll( value v ) {
	value o = val_this();
	val_check_obj(o,t_camera);
	if( !val_is_number(v) )
		return NULLVAL;
	camera *c = val_camera(o);
	c->roll = val_number(v);
	matrix m;
	quaternion q;
	vector vec;
	vector vecUp(0,0,1);
	vector_sub(&vec,&c->pos,&c->target);
	quaternion_rotation_axis(&q,&vec,(float)c->roll);
	matrix3d_of_quaternion(&m,&q);
	vector_transform_coord(&c->worldup,&vecUp,&m);
	c->modified = true;
	return v;
}

static value camera_set_proj( value m ) {
	value o = val_this();
	val_check_obj(o,t_camera);
	val_check_obj(m,t_matrix);
	camera *c = val_camera(o);
	matrix3d_copy(&c->proj,val_matrix(m));
	c->modified = true;
	return m;
}

static value camera_set_zoom( value v ) {
	value o = val_this();
	val_check_obj(o,t_camera);
	if( !val_is_number(v) || fabs(val_number(v)) < 0.000001 ) 
		return NULLVAL;
	camera *c = val_camera(o);
	c->zoom = val_number(v);
	c->modified = true;
	return val_true;
}

static value camera_set_world( value m ) {
	value o = val_this();
	val_check_obj(o,t_camera);
	val_check_obj(m,t_matrix);
	camera *c = val_camera(o);
	matrix3d_copy(&c->world,val_matrix(m));
	c->modified = true;
	return m;
}

static value camera_get_world() {
	value o = val_this();
	val_check_obj(o,t_camera);
	return alloc_matrix( &val_camera(o)->world );
}

static value camera_set_worldup( value v ) {
	value o = val_this();
	val_check_obj(o,t_camera);
	val_check_obj(v,t_vector);
	camera *c = val_camera(o);
	c->worldup = *val_vector(v);
	c->modified = true;
	return v;
}

static value camera_get_worldup() {
	value o = val_this();
	val_check_obj(o,t_camera);
	return alloc_vector( &val_camera(o)->worldup );
}

static value camera_toString() {
	return alloc_string("#camera");
}

static value honsen_camera() {
	value o = alloc_class(&t_camera);
	Constr(o,camera,Camera,0);
	Property(o,camera,target);
	Property(o,camera,pos);
	Property(o,camera,proj);
	Property(o,camera,roll);
	Property(o,camera,zoom);
	Property(o,camera,world);
	Property(o,camera,worldup);
	Method(o,camera,toString,0);
	return o; 
}

static value honsen_set_camera( value c ) {
	m3d *m = M3D();
	if( c == NULL ) {
		if( m->camera != NULL ) {
			free_root((value*)m->camera);
			m->camera = NULL;
		}
		return val_true;
	}
	val_check_obj(c,t_camera);
	if( m->camera == NULL )
		m->camera = (camera**)alloc_root(1);
	*m->camera = val_camera(c);
	val_camera(c)->modified = true;
	return val_true;
}

void m3d_free_camera( m3d *m ) {
	if( m->camera != NULL ) {
		free_root((value*)m->camera);
		m->camera = NULL;
	}
}

DEFINE_PRIM(honsen_set_camera,1);

/* ************************************************************************ */

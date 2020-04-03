/* ************************************************************************ */
/*																			*/
/*	M3D-SE Light for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "script.h"
#include "../m3d_priv.h"

#define val_light(o) ((light*)val_odata(o))

static vector defpos(0,0,1);
static unsigned int defambient = 0x707070;
static unsigned int defdiffuse = 0x909090;

DEFINE_CLASS(honsen,light);

static value light_new0() {
	value o = val_this();
	val_check_obj(o,t_light);	
	light *l = (light*)alloc_small_no_scan(sizeof(light));
	l->pos = defpos;
	l->ambient = defambient;
	l->diffuse = defdiffuse;
	l->ray = 0;
	val_odata(o) = (value)l;
	return o;
}

static value light_get_pos() {
	value o = val_this();
	val_check_obj(o,t_light);
	return alloc_vector( &val_light(o)->pos );
}

static value light_get_ray() {
	value o = val_this();
	val_check_obj(o,t_light);
	return alloc_float( val_light(o)->ray );
}

static value light_get_ambient() {
	value o = val_this();
	val_check_obj(o,t_light);
	return alloc_int( val_light(o)->ambient );
}

static value light_get_diffuse() {
	value o = val_this();
	val_check_obj(o,t_light);
	return alloc_int( val_light(o)->diffuse );
}

static value light_set_pos( value v ) {
	value o = val_this();
	val_check_obj(o,t_light);
	val_check_obj(v,t_vector);
	val_light(o)->pos = *val_vector(v);
	return v;
}

static value light_set_ray( value v ) {
	value o = val_this();
	val_check_obj(o,t_light);
	if( !val_is_number(v) )
		return NULLVAL;
	float r = val_number(v);
	if( r < 0 )
		r = 0;
	val_light(o)->ray = r;
	return alloc_float(r);
}

static value light_set_ambient( value c ) {
	value o = val_this();
	val_check_obj(o,t_light);
	if( !val_is_int(c) )
		return NULLVAL;
	c = alloc_int(val_int(c) & 0xFFFFFF);
	val_light(o)->ambient = val_int(c);
	return c;
}

static value light_set_diffuse( value c ) {
	value o = val_this();
	val_check_obj(o,t_light);
	if( !val_is_int(c) )
		return NULLVAL;
	c = alloc_int(val_int(c) & 0xFFFFFF);
	val_light(o)->diffuse = val_int(c);
	return c;
}

static value light_toString() {
	return alloc_string("#light");
}

static value honsen_light() {
	value o = alloc_class(&t_light);
	Constr(o,light,Light,0);
	Property(o,light,pos);
	Property(o,light,ambient);
	Property(o,light,diffuse);
	Property(o,light,ray);
	Method(o,light,toString,0);
	return o; 
}

static value honsen_set_light( value l ) {
	m3d *m = M3D();
	if( l == NULL ) {
		if( m->light != NULL ) {
			free_root((value*)m->light);
			m->light = NULL;
		}
		return val_true;
	}
	val_check_obj(l,t_light);
	if( m->light == NULL )
		m->light = (light**)alloc_root(1);
	*m->light = val_light(l);
	return val_true;
}

void m3d_free_light( m3d *m ) {
	if( m->light != NULL ) {
		free_root((value*)m->light);
		m->light = NULL;
	}
}

DEFINE_PRIM(honsen_set_light,1);

/* ************************************************************************ */

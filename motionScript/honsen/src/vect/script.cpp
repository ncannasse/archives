/* ************************************************************************ */
/*																			*/
/*	Honsen Vector Rendering													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <value.h>
#include <math.h>
#include "vectpriv.h"
#define matrix matrix3d // workaround MSVC compiler error
#include "../m3d/m3d_priv.h"

#define val_vect(o)		((vect*)val_odata(o))

DEFINE_CLASS(honsen,vect);

value alloc_texture( cached_texture *t );

static void delete_vect( value o ) {
	m3d *m = M3D();
	if( m != NULL )
		m3d_remove_vect(m,val_vect(o));
	vect_destroy( val_vect(o) );
}

static value vect_new3( value r, value w, value h ) {
	value o = val_this();
	val_check_obj(o,t_vect);
	if( !val_is_resource(r) )
		return NULL;
	if( !val_is_int(w) || !val_is_int(h) || val_int(w) < 0 || val_int(w) > 1024 || val_int(h) < 0 || val_int(h) > 1024 )
		return NULL;
	m3d *m = M3D();
	m->vcache = vect_cache_init(m->vcache,(MTW*)val_resource(r));
	vect *v = vect_init(m->vcache,val_int(w),val_int(h));
	m3d_add_vect(m,v);
	val_odata(o) = (value)v;
	val_gc(o,delete_vect);
	return o;
}

static value vect_get_texture() {
	value o = val_this();
	val_check_obj(o,t_vect);
	FOREACH(Vect*,M3D()->vects);
		if( item->v == val_vect(o) )
			return alloc_texture(item->t);
	ENDFOR;
	return NULL;
}

static value honsen_vect() {
	value o = alloc_class(&t_vect);
	Constr(o,vect,ClipPlayer,3);
	Method(o,vect,get_texture,0);
	return o;
}

/* ************************************************************************ */

#define val_clip(o)		((Clip*)val_odata(o))

DEFINE_CLASS(honsen,clip);

static void set_null_ref( value o ) {
	Clip *c = val_clip(o);
	if( c != NULL )
		c->ref = NULL;
}

void Clip::RefRelease( void *ref ) {
	val_odata(ref) = NULL;
	val_otype(ref) = NULL;
}

static value clip_new4( value v, value r, value parent, value vdepth ) {
	value o = val_this();
	val_check_obj(o,t_clip);
	val_check_obj(v,t_vect);
	if( parent != NULLVAL )
		val_check_obj(parent,t_clip);
	if( r != NULLVAL && !val_is_resource(r) )
		return NULL;
	if( !val_is_int(vdepth) )
		return NULL;
	int depth = val_int(vdepth);
	if( depth < 0 || depth > MAX_DEPTH )
		return NULL;

	ClipData *d;
	if( r == NULLVAL )
		d = NULL;
	else {
		MTW *m = (MTW*)val_resource(r);
		d = Clip::MakeData(val_vect(v),m);
		if( d == NULL ) {
			val_print(alloc_string("Invalid MTV data could not be loaded"));
			return NULL;
		}
	}
	Clip *p = (parent == NULLVAL)?NULL:val_clip(parent);
	Clip *c = new Clip(p,d);
	c->modified = true;
	c->ref = o;
	val_odata(o) = (value)c;
	val_gc(o,set_null_ref);

	WArray<Element*> *ctbl = p?&p->depths:(WArray<Element*>*)&(val_vect(v)->clips);
	delete (*ctbl)[depth];
	(*ctbl)[depth] = c;
	return o;
}

static value clip_set_x( value v ) {
	value o = val_this();
	val_check_obj(o,t_clip);
	if( !val_is_number(v) )
		return NULL;
	val_clip(o)->mat[4] = val_number(v);
	val_clip(o)->modified = true;
	return v;
}

static value clip_get_x() {
	value o = val_this();
	val_check_obj(o,t_clip);
	return alloc_float(val_clip(o)->mat[4]);
}

static value clip_set_y( value v ) {
	value o = val_this();
	val_check_obj(o,t_clip);
	if( !val_is_number(v) )
		return NULL;
	val_clip(o)->mat[5] = val_number(v);
	return v;
}

static value clip_get_y() {
	value o = val_this();
	val_check_obj(o,t_clip);
	return alloc_float(val_clip(o)->mat[5]);
}

static value clip_set_xscale( value v ) {
	value o = val_this();
	val_check_obj(o,t_clip);
	if( !val_is_number(v) )
		return NULL;
	Clip *c = val_clip(o);
	double rot = c->Rotation();
	c->mat[0] = val_number(v) * cos(rot);
	c->mat[1] = -val_number(v) * sin(rot);
	c->modified = true;
	return v;
}

static value clip_get_xscale() {
	value o = val_this();
	val_check_obj(o,t_clip);
	return alloc_float(val_clip(o)->XScale());
}

static value clip_set_yscale( value v ) {
	value o = val_this();
	val_check_obj(o,t_clip);
	if( !val_is_number(v) )
		return NULL;
	Clip *c = val_clip(o);
	double rot = c->Rotation();
	c->mat[2] = val_number(v) * sin(rot);
	c->mat[3] = val_number(v) * cos(rot);
	c->modified = true;
	return v;
}

static value clip_get_yscale() {
	value o = val_this();
	val_check_obj(o,t_clip);
	return alloc_float( val_clip(o)->YScale() );
}

static value clip_set_rotation( value v ) {
	value o = val_this();
	val_check_obj(o,t_clip);
	if( !val_is_number(v) )
		return NULL;
	Clip *c = val_clip(o);
	double alpha = val_number(v);
	double sx = c->XScale();
	double sy = c->YScale();
	c->rotation = alpha;
	c->mat[0] = sx * cos(alpha);
	c->mat[1] = -sx * sin(alpha);
	c->mat[2] = sy * sin(alpha);
	c->mat[3] = sy * cos(alpha);
	c->modified = true;
	return v;
}

static value clip_get_rotation() {
	value o = val_this();
	val_check_obj(o,t_clip);
	return alloc_float(val_clip(o)->Rotation());
}

static value clip_totalFrames() {
	value o = val_this();
	val_check_obj(o,t_clip);
	return alloc_int( val_clip(o)->TotalFrames() );
}

static value clip_get_speed() {
	value o = val_this();
	val_check_obj(o,t_clip);
	return alloc_float( val_clip(o)->speed );
}

static value clip_set_speed( value v ) {
	value o = val_this();
	val_check_obj(o,t_clip);
	if( !val_is_number(v) )
		return NULLVAL;
	val_clip(o)->speed = val_number(v);
	return v;
}

static value clip_get_frame() {
	value o = val_this();
	val_check_obj(o,t_clip);
	return alloc_float( val_clip(o)->frame );
}

static value clip_set_frame( value v ) {
	value o = val_this();
	val_check_obj(o,t_clip);
	if( !val_is_number(v) )
		return NULLVAL;
	Clip *c = val_clip(o);
	c->frame = fmod( val_number(v), c->TotalFrames() );
	c->modified = true;
	return alloc_float( val_clip(o)->frame );
}

static value clip_get_parent() {
	value o = val_this();
	val_check_obj(o,t_clip);
	Clip *c = val_clip(o)->parent;
	if( c == NULL )
		return NULL;
	if( c->ref != NULL )
		return (value)c->ref;
	o = alloc_object(&t_clip);
	c->ref = o;
	val_odata(o) = (value)c;
	val_gc(o,set_null_ref);
	return o;
}

static value clip_get_visible() {
	value o = val_this();
	val_check_obj(o,t_clip);
	return alloc_bool( val_clip(o)->visible );
}

static value clip_set_visible( value v ) {
	value o = val_this();
	val_check_obj(o,t_clip);
	if( !val_is_bool(v) )
		return NULL;
	val_clip(o)->visible = val_bool(v);
	val_clip(o)->modified = true;
	return v;
}

static value clip_get_alpha() {
	value o = val_this();
	val_check_obj(o,t_clip);
	return alloc_float( val_clip(o)->col[6] );
}

static value clip_set_alpha( value v ) {
	value o = val_this();
	val_check_obj(o,t_clip);
	if( !val_is_number(v) )
		return NULL;
	val_clip(o)->col[6] = (float)val_number(v);
	val_clip(o)->modified = true;
	return v;
}

static value honsen_clip() {
	value o = alloc_class(&t_clip);
	Constr(o,clip,Clip,4);
	Method(o,clip,get_x,0);
	Method(o,clip,set_x,1);
	Method(o,clip,get_y,0);
	Method(o,clip,set_y,1);
	Method(o,clip,get_xscale,0);
	Method(o,clip,set_xscale,1);
	Method(o,clip,get_yscale,0);
	Method(o,clip,set_yscale,1);
	Method(o,clip,get_rotation,0);
	Method(o,clip,set_rotation,1);
	Method(o,clip,get_speed,0);
	Method(o,clip,set_speed,1);
	Method(o,clip,get_frame,0);
	Method(o,clip,set_frame,1);
	Method(o,clip,totalFrames,1);
	Method(o,clip,get_parent,0);
	Method(o,clip,get_visible,0);
	Method(o,clip,set_visible,1);
	Method(o,clip,get_alpha,0);
	Method(o,clip,set_alpha,1);
	return o;
}

/* ************************************************************************ */

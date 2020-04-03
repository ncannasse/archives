/* ************************************************************************ */
/*																			*/
/*	M3D-SE TRS for MTS														*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "script.h"

DEFINE_CLASS(honsen,trs);
vector default_scale(1,1,1);
quaternion default_quaternion(0,0,0,1);
vector default_position(0,0,0);

#define val_trs(o)		((trs*)val_odata(o))

struct trs {
	value t;
	value r;
	value s;
};

static value alloc_trs_data(const vector *t,const quaternion *r, const vector *s) {
	trs *v = (trs*)alloc_small(sizeof(trs));
	v->t = alloc_vector(t?t:&default_position);
	v->r = alloc_quaternion(r?r:&default_quaternion);
	v->s = alloc_vector(s?s:&default_scale);
	return (value)v;
}

static value trs_new0() {
	value o = val_this();
	val_check_obj(o,t_trs);
	val_odata(o) = alloc_trs_data(NULL,NULL,NULL);
	return o;
}

static value trs_new1( value t ) {
	value o = val_this();
	val_check_obj(o,t_trs);
	if( val_is_obj(t,t_trs) ) {
		trs *vt = val_trs(t);
		val_odata(o) = alloc_trs_data(val_vector(vt->t),val_quat(vt->r),val_vector(vt->s));
	} else if( val_is_obj(t,t_vector) ) {
		val_odata(o) = alloc_trs_data(val_vector(t),NULL,NULL);
	} else
		return NULLVAL;
	return o;
}

static value trs_new3( value t, value r, value s ) {
	value o = val_this();
	vector *vt,*vs;
	quaternion *vr;
	val_check_obj(o,t_trs);
	if( t == NULLVAL )
		vt = NULL;
	else {
		val_check_obj(t,t_vector);
		vt = val_vector(t);
	}
	if( r == NULLVAL )
		vr = NULL;
	else {
		val_check_obj(r,t_quat);
		vr = val_quat(r);
	}
	if( s == NULLVAL ) 
		vs = NULL;
	else {
		val_check_obj(s,t_vector);
		vs = val_vector(s);
	}
	val_odata(o) = alloc_trs_data(vt,vr,vs);
	return o;
}

static void vec_mult( vector *v, const vector *v1, const vector *v2 ) {
	v->x = v1->x * v2->x;
	v->y = v1->y * v2->y;
	v->z = v1->z * v2->z;
}

static value trs___mult( value t2 ) {
	value t1 = val_this();
	val_check_obj(t1,t_trs);
	val_check_obj(t2,t_trs);
	value o = alloc_trs();
	trs *_t1 = val_trs(t1),*_t2 = val_trs(t2),*t = val_trs(o);
	vector_add(val_vector(t->t),val_vector(_t1->t),val_vector(_t2->t));
	vec_mult(val_vector(t->s),val_vector(_t1->s),val_vector(_t2->s));
	quaternion_multiply(val_quat(t->r),val_quat(_t1->r),val_quat(_t2->r)); // ? inverse _t1 and _t2 ?
	return o;
}

static void trs_to_stringbuf( const trs *t, stringbuf b ) {
	stringbuf_append(b,"\n  T=");
	vector_to_stringbuf(val_vector(t->t),b);
	stringbuf_append(b,"\n  R=");
	quaternion_to_stringbuf(val_quat(t->r),b);
	stringbuf_append(b,"\n  S=");
	vector_to_stringbuf(val_vector(t->s),b);
}

static value trs_toString() {
	value o = val_this();
	val_check_obj(o,t_trs);
	stringbuf b = alloc_stringbuf("#TRS<");
	trs_to_stringbuf(val_trs(o),b);
	stringbuf_append(b,"\n>");
	return stringbuf_to_string(b);
}

void matrix_of_trs(value vt, matrix *out ) {
	trs *t = val_trs(vt);
	matrix3d_build(out,val_vector(t->t),val_quat(t->r),val_vector(t->s));
}

static value trs_toMatrix() {
	value o = val_this();
	val_check_obj(o,t_trs);
	value m = alloc_matrix(NULL);
	matrix_of_trs(o,val_matrix(m));
	return m;
}

#define TRS_VGET(t,x) \
	static value trs_get_##t##x() { \
		value o = val_this(); \
		val_check_obj(o,t_trs); \
		return alloc_float(val_vector(val_trs(o)->t)->x); \
	}

#define TRS_VSET(t,x) \
	static value trs_set_##t##x( value f ) { \
		value o = val_this(); \
		val_check_obj(o,t_trs); \
		if( !val_is_number(f) ) \
			return NULLVAL; \
		val_vector(val_trs(o)->t)->x = val_number(f); \
		return f; \
	}

#define TRS_GET(t,tt) \
	static value trs_get_##t() { \
		value o = val_this(); \
		val_check_obj(o,t_trs); \
		return val_trs(o)->t; \
	}

#define TRS_SET(t,tt) \
	static value trs_set_##t( value v ) { \
		value o = val_this(); \
		val_check_obj(o,t_trs); \
		val_check_obj(v,tt); \
		val_trs(o)->t = v; \
		return v; \
	}

#define TRS_VOP(op)		op(t,x) op(t,y) op(t,z) op(s,x) op(s,y) op(s,z)
#define TRS_OP(op)		op(t,t_vector) op(r,t_quat) op(s,t_vector)

TRS_VOP(TRS_VGET);
TRS_VOP(TRS_VSET);
TRS_OP(TRS_GET);
TRS_OP(TRS_SET);

#define trs_mult trs___mult

static value honsen_trs() {
	value o = alloc_class(&t_trs);
	Constr(o,trs,Trs,0);
	Constr(o,trs,Trs,1);
	Constr(o,trs,Trs,3);
	Method(o,trs,toString,0);
	Method(o,trs,toMatrix,0);
	Method(o,trs,__mult,1);
	Method(o,trs,mult,1);
	Property(o,trs,tx);
	Property(o,trs,ty);
	Property(o,trs,tz);
	Property(o,trs,sx);
	Property(o,trs,sy);
	Property(o,trs,sz);
	Property(o,trs,t);
	Property(o,trs,r);
	Property(o,trs,s);
	return o; 
}

value alloc_trs() {
	value o = alloc_object(&t_trs);
	val_odata(o) = alloc_trs_data(NULL,NULL,NULL);
	return o;
}

/* ************************************************************************ */

/* ************************************************************************ */
/*																			*/
/*	M3D-SE Vector for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "script.h"

DEFINE_CLASS(honsen,vector);

#define alloc_vector_data()		(value)alloc_small_no_scan(sizeof(vector))

static value vector_new0() {
	value o = val_this();
	val_check_obj(o,t_vector);
	val_odata(o) = alloc_vector_data();
	return o;
}

static value vector_new3(value x, value y, value z) {
	value o = val_this();
	val_check_obj(o,t_vector);
	if( !val_is_number(x) || !val_is_number(y) || !val_is_number(z) )
		return NULLVAL;
	val_odata(o) = alloc_vector_data();
	val_vector(o)->x = val_number(x);
	val_vector(o)->y = val_number(y);
	val_vector(o)->z = val_number(z);
	return o;
}

void vector_to_stringbuf( const vector *v, stringbuf b ) {
	value f = alloc_float(v->x);
	val_to_string(f,b);
	stringbuf_append(b,",");
	val_float(f) = v->y;
	val_to_string(f,b);
	stringbuf_append(b,",");
	val_float(f) = v->z;
	val_to_string(f,b);
}

static value vector_toString() {
	value o = val_this();
	val_check_obj(o,t_vector);
	stringbuf b = alloc_stringbuf("#vector<");
	vector_to_stringbuf(val_vector(o),b);
	stringbuf_append(b,">");
	return stringbuf_to_string(b);
}

static value vector___add( value v ) {
	value o = val_this();
	val_check_obj(o,t_vector);
	val_check_obj(v,t_vector);
	value v2 = alloc_vector(NULL);
	vector_add(val_vector(v2),val_vector(o),val_vector(v));
	return v2;
}

static value vector___sub( value v ) {
	value o = val_this();
	val_check_obj(o,t_vector);
	val_check_obj(v,t_vector);
	value v2 = alloc_vector(NULL);
	vector_sub(val_vector(v2),val_vector(o),val_vector(v));
	return v2;
}

static value vector___mult( value v ) {
	value o = val_this();
	val_check_obj(o,t_vector);
	if( val_is_number(v) ) {
		value v2 = alloc_vector(NULL);
		vector_scale(val_vector(v2),val_vector(o),val_number(v));
		return v2;
	} else {
		val_check_obj(v,t_matrix);
		vector vt;
		vector_transform(&vt,val_vector(o),val_matrix(v));
		return alloc_vector(&vt);
	}
}

static value vector___div( value v ) {
	value o = val_this();
	val_check_obj(o,t_vector);
	value v2 = alloc_vector(NULL);
	vector_scale(val_vector(v2),val_vector(o),1.0f/val_number(v));
	return v2;
}

static value vector_get_length() {
	value o = val_this();
	val_check_obj(o,t_vector);
	return alloc_float( vector_length(val_vector(o)) );
}

static value vector_set_length( value v ) {
	value o = val_this();
	val_check_obj(o,t_vector);
	if( !val_is_number(v) )
		return NULLVAL;
	vector_scale(val_vector(o),val_vector(o),val_number(v) / vector_length(val_vector(o)));
	return val_true;
}

static value vector_normalize2() {
	value o = val_this();
	val_check_obj(o,t_vector);
	vector_normalize(val_vector(o),val_vector(o));
	return val_true;
}

#define VEC_GET(x)	\
	static value vector_get_##x() { \
		value o = val_this(); \
		val_check_obj(o,t_vector); \
		return alloc_float(val_vector(o)->x); \
	}

#define VEC_SET(x)	\
	static value vector_set_##x( value f ) { \
		value o = val_this(); \
		val_check_obj(o,t_vector); \
		if( !val_is_number(f) ) \
			return NULLVAL; \
		val_vector(o)->x = val_number(f); \
		return f; \
	}

#define VEC_OP(op)	op(x)	op(y)	op(z)

VEC_OP(VEC_GET);
VEC_OP(VEC_SET);

#define vector_normalize vector_normalize2
#define vector_add vector___add
#define vector_div vector___div
#define vector_mult vector___mult
#define vector_sub vector___sub

static value honsen_vector() {
	value o = alloc_class(&t_vector);
	Constr(o,vector,Vector,0);
	Constr(o,vector,Vector,3);
	Method(o,vector,toString,0);
	Property(o,vector,x);
	Property(o,vector,y);
	Property(o,vector,z);
	Property(o,vector,length);
	Method(o,vector,add,1);
	Method(o,vector,sub,1);
	Method(o,vector,mult,1);
	Method(o,vector,div,1);
	Method(o,vector,__add,1);
	Method(o,vector,__sub,1);
	Method(o,vector,__mult,1);
	Method(o,vector,__div,1);
	Method(o,vector,normalize,0);
	return o; 
}

value alloc_vector( const vector *v ) {
	value o = alloc_object(&t_vector);
	val_odata(o) = alloc_vector_data();
	if( v )
		*val_vector(o) = *v;
	return o;
}

static value honsen_vector_dot(value v1, value v2) {
	val_check_obj(v1,t_vector);
	val_check_obj(v2,t_vector);
	return alloc_float( vector_dot(val_vector(v1),val_vector(v2)) );
}

static value honsen_vector_cross(value v1, value v2) {
	val_check_obj(v1,t_vector);
	val_check_obj(v2,t_vector);
	value v = alloc_vector(NULL);
	vector_cross(val_vector(v),val_vector(v1),val_vector(v2));
	return v;
}

static value honsen_vector_lerp(value v1, value v2, value f) {
	val_check_obj(v1,t_vector);
	val_check_obj(v2,t_vector);
	if( !val_is_number(f) )
		return NULLVAL;
	value v = alloc_vector(NULL);
	vector_lerp(val_vector(v),val_vector(v1),val_vector(v2),val_number(f));
	return v;
}

static value honsen_vector_transform( value v, value m ) {
	value vout;
	val_check_obj(v,t_vector);
	val_check_obj(m,t_matrix);
	vout = alloc_vector(NULL);
	vector_transform_coord(val_vector(vout),val_vector(v),val_matrix(m));
	return vout;
}

DEFINE_PRIM(honsen_vector_dot,2);
DEFINE_PRIM(honsen_vector_cross,2);
DEFINE_PRIM(honsen_vector_lerp,3);
DEFINE_PRIM(honsen_vector_transform,2);

/* ************************************************************************ */

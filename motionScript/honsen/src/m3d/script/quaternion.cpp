/* ************************************************************************ */
/*																			*/
/*	M3D-SE Quaternion for MTS												*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "script.h"

DEFINE_CLASS(honsen,quat);

#define alloc_quat_data()	(value)alloc_small_no_scan(sizeof(quaternion))

static value quaternion_new0() {
	value o = val_this();
	val_check_obj(o,t_quat);
	val_odata(o) = alloc_quat_data();
	quaternion_identity(val_quat(o));
	return o;
}

static value quaternion_new1(value q) {
	value o = val_this();
	val_check_obj(o,t_quat);
	val_check_obj(q,t_quat);
	val_odata(o) = alloc_quat_data();
	*val_quat(o) = *val_quat(q);
	return o;
}

static value quaternion_new2(value v, value a) {
	value o = val_this();
	val_check_obj(o,t_quat);
	val_check_obj(v,t_vector);
	if( !val_is_number(a) )
		return NULLVAL;
	val_odata(o) = alloc_quat_data();
	quaternion_rotation_axis(val_quat(o),val_vector(v),val_number(a));
	return o;
}

static value quaternion_new3(value x, value y, value z) {
	value o = val_this();
	val_check_obj(o,t_quat);
	if( !val_is_number(x) || !val_is_number(y) || !val_is_number(z) )
		return NULLVAL;
	val_odata(o) = alloc_quat_data();
	quaternion_rotation_angles(val_quat(o),val_number(x),val_number(y),val_number(z));
	return o;
}

static value quaternion_inv() {
	value o = val_this();
	val_check_obj(o,t_quat);
	value q = alloc_quaternion(NULL);
	quaternion_inverse(val_quat(q),val_quat(o));
	return q;
}

static value quaternion___mult(value q2) {
	value q1 = val_this();
	val_check_obj(q1,t_quat);
	val_check_obj(q2,t_quat);
	value q = alloc_quaternion(NULL);
	quaternion_multiply(val_quat(q),val_quat(q1),val_quat(q2));
	return q;
}

void quaternion_to_stringbuf( const quaternion *q, stringbuf b ) {
	value f = alloc_float(q->x);
	val_to_string(f,b);
	stringbuf_append(b,",");
	val_float(f) = q->y;
	val_to_string(f,b);
	stringbuf_append(b,",");
	val_float(f) = q->y;
	val_to_string(f,b);
	stringbuf_append(b,",");
	val_float(f) = q->w;
	val_to_string(f,b);
}

static value quaternion_toString() {
	value o = val_this();
	val_check_obj(o,t_quat);
	stringbuf b = alloc_stringbuf("#quaternion<");
	quaternion_to_stringbuf(val_quat(o),b);
	stringbuf_append(b,">");
	return stringbuf_to_string(b);
}

static value quaternion_toMatrix() {
	value o = val_this();
	val_check_obj(o,t_quat);
	value m = alloc_matrix(NULL);
	matrix3d_of_quaternion(val_matrix(m),val_quat(o));
	return m;
}

#define quaternion_mult quaternion___mult

static value honsen_quat() {
	value o = alloc_class(&t_quat);
	Constr(o,quaternion,Quaternion,0);
	Constr(o,quaternion,Quaternion,1);
	Constr(o,quaternion,Quaternion,2);
	Constr(o,quaternion,Quaternion,3);
	Method(o,quaternion,toString,0);
	Method(o,quaternion,toMatrix,0);
	Method(o,quaternion,mult,1);
	Method(o,quaternion,__mult,1);
	Method(o,quaternion,inv,0);
	return o; 
}

value alloc_quaternion( const quaternion *q ) {
	value o = alloc_object(&t_quat);
	val_odata(o) = alloc_quat_data();
	if( q ) 
		*val_quat(o) = *q;
	return o;
}

static value honsen_quaternion_slerp( value q1, value q2, value f ) {
	val_check_obj(q1,t_quat);
	val_check_obj(q2,t_quat);
	if( !val_is_number(f) )
		return NULLVAL;
	value q = alloc_quaternion(NULL);
	quaternion_slerp(val_quat(q),val_quat(q1),val_quat(q2),val_number(f));
	return q;
}

DEFINE_PRIM(honsen_quaternion_slerp,3);

/* ************************************************************************ */

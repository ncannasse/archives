/* ************************************************************************ */
/*																			*/
/*	M3D-SE Matrix for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "script.h"

DEFINE_CLASS(m3dse,matrix);

#define alloc_matrix_data()	(value)alloc_small_no_scan(sizeof(MATRIX))

value matrix_new0() {
	value o = val_this();
	val_check_obj(o,t_matrix);
	val_odata(o) = alloc_matrix_data();
	mid(val_matrix(o));
	return o;
}

value matrix_new1( value v ) {
	value o = val_this();
	val_check_obj(o,t_matrix);
	if( val_is_obj(v,t_matrix) ) {
		val_odata(o) = alloc_matrix_data();
		*val_matrix(o) = *val_matrix(v);
	} else if( val_is_obj(v,t_quat) ) {
		val_odata(o) = alloc_matrix_data();
		D3DXMatrixRotationQuaternion(val_matrix(o),val_quat(v));
	} else if( val_is_obj(v,t_trs) ) {
		val_odata(o) = alloc_matrix_data();
		matrix_of_trs(v,val_matrix(o));
	} else
		return NULLVAL;
	return o;
}

value matrix_new3(value t, value r, value s) {
	value o = val_this();
	VECTOR *vt, *vs;
	QUATERNION *vr;
	val_check_obj(o,t_matrix);
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
	val_odata(o) = alloc_matrix_data();
	D3DXMatrixTransformation(val_matrix(o),NULL,NULL,vs,NULL,vr,vt);
	return o;
}

void matrix_to_stringbuf( const MATRIX *q, stringbuf b ) {
	stringbuf_append(b,"...");
}

value matrix_pos() {
	value o = val_this();
	val_check_obj(o,t_matrix);
	VECTOR v(val_matrix(o)->_41,val_matrix(o)->_42,val_matrix(o)->_43);
	return alloc_vector(&v);
}

value matrix_toString() {
	value o = val_this();
	val_check_obj(o,t_matrix);
	stringbuf b = alloc_stringbuf("#matrix<");
	matrix_to_stringbuf(val_matrix(o),b);
	stringbuf_append(b,">");
	return stringbuf_to_string(b);
}

value matrix___mult( value m ) {
	value o = val_this();
	val_check_obj(o,t_matrix);
	val_check_obj(m,t_matrix);
	value m2 = alloc_matrix(NULL);
	mmult(val_matrix(m2),val_matrix(o),val_matrix(m));
	return m2;
}

value matrix_inv() {
	value o = val_this();
	val_check_obj(o,t_matrix);
	value m2 = alloc_matrix(NULL);
	minv(val_matrix(m2),val_matrix(o));
	return m2;
}

value m3dse_matrix() {
	value o = alloc_class(&t_matrix);
	Constr(o,matrix,Matrix,0);
	Constr(o,matrix,Matrix,1);
	Constr(o,matrix,Matrix,3);
	Method(o,matrix,pos,0);
	Method(o,matrix,toString,0);
	Method(o,matrix,__mult,1);
	Method(o,matrix,inv,0);
	return o; 
}

value alloc_matrix( const MATRIX *m ) {
	value o = alloc_object(&t_matrix);
	val_odata(o) = alloc_matrix_data();
	if( m != NULL )
		*val_matrix(o) = *m;
	return o;
}

/* ************************************************************************ */

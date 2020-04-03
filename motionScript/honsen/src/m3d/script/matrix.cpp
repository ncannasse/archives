/* ************************************************************************ */
/*																			*/
/*	M3D-SE Matrix for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <stdio.h>
#include "script.h"

DEFINE_CLASS(honsen,matrix);

#define alloc_matrix_data()	(value)alloc_small_no_scan(sizeof(matrix))

static value matrix_new0() {
	value o = val_this();
	val_check_obj(o,t_matrix);
	val_odata(o) = alloc_matrix_data();
	matrix3d_identity(val_matrix(o));
	return o;
}

static value matrix_new1( value v ) {
	value o = val_this();
	val_check_obj(o,t_matrix);
	if( val_is_obj(v,t_matrix) ) {
		val_odata(o) = alloc_matrix_data();
		*val_matrix(o) = *val_matrix(v);
	} else if( val_is_obj(v,t_quat) ) {
		val_odata(o) = alloc_matrix_data();
		matrix3d_of_quaternion(val_matrix(o),val_quat(v));
	} else if( val_is_obj(v,t_trs) ) {
		val_odata(o) = alloc_matrix_data();
		matrix_of_trs(v,val_matrix(o));
	} else
		return NULLVAL;
	return o;
}

static value matrix_new3(value t, value r, value s) {
	value o = val_this();
	vector *vt, *vs;
	quaternion *vr;
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
	matrix3d_build(val_matrix(o),vt,vr,vs);
	return o;
}

static void matrix_to_stringbuf( const matrix *q, stringbuf b ) {
	char buf[32];
	int i;
	for(i=0;i<16;i++) {
		sprintf(buf,"%.6f%c",((float*)(matrix)(*q))[i],(i==15)?'\0':(((i&3) == 3)?'\n':','));
		stringbuf_append(b,buf);
	}
}

static value matrix_pos() {
	value o = val_this();
	val_check_obj(o,t_matrix);
	vector v;
	matrix3d_get_translation(&v,val_matrix(o));
	return alloc_vector(&v);
}

static value matrix_toString() {
	value o = val_this();
	val_check_obj(o,t_matrix);
	stringbuf b = alloc_stringbuf("#matrix<\n");
	matrix_to_stringbuf(val_matrix(o),b);
	stringbuf_append(b,"\n>");
	return stringbuf_to_string(b);
}

static value matrix___mult( value m ) {
	value o = val_this();
	val_check_obj(o,t_matrix);
	val_check_obj(m,t_matrix);
	value m2 = alloc_matrix(NULL);
	matrix3d_multiply(val_matrix(m2),val_matrix(o),val_matrix(m));
	return m2;
}

static value matrix_inv() {
	value o = val_this();
	val_check_obj(o,t_matrix);
	value m2 = alloc_matrix(NULL);
	matrix3d_inverse(val_matrix(m2),val_matrix(o));
	return m2;
}

static value matrix_proj( value angle, value ratio, value znear, value zfar ) {
	value o = val_this();
	val_check_obj(o,t_matrix);
	if( !val_is_number(angle) || !val_is_number(ratio) || !val_is_number(znear) || !val_is_number(zfar) )
		return NULL;
	matrix3d_proj(val_matrix(o),val_number(angle),val_number(ratio),val_number(znear),val_number(zfar));
	return val_true;
}

#define matrix_mult matrix___mult

static value honsen_matrix() {
	value o = alloc_class(&t_matrix);
	Constr(o,matrix,Matrix,0);
	Constr(o,matrix,Matrix,1);
	Constr(o,matrix,Matrix,3);
	Method(o,matrix,pos,0);
	Method(o,matrix,toString,0);
	Method(o,matrix,mult,1);
	Method(o,matrix,__mult,1);
	Method(o,matrix,inv,0);
	Method(o,matrix,proj,4);
	return o;
}

value alloc_matrix( const matrix *m ) {
	value o = alloc_object(&t_matrix);
	val_odata(o) = alloc_matrix_data();
	if( m != NULL )
		*val_matrix(o) = *m;
	return o;
}

/* ************************************************************************ */

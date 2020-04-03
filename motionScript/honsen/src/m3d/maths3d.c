/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "maths3d.h"

#ifdef _WIN32

#include <d3dx8.h>
#define MAT(m) ((D3DXMATRIX*)m)
#define VEC(v) ((D3DXVECTOR3*)v)
#define QUAT(q) ((D3DXQUATERNION*)q)

void matrix3d_identity( matrix *out ) {
	D3DXMatrixIdentity(MAT(out));
}

void matrix3d_multiply( matrix *out, matrix *a, matrix *b ) {
	D3DXMatrixMultiply(MAT(out),MAT(a),MAT(b));
}

void matrix3d_affine( matrix *out, matrix *a, matrix *b ) {
	D3DXMatrixMultiply(MAT(out),MAT(a),MAT(b));
}

void matrix3d_get_translation( vector *out, matrix *m ) {
	out->x = m->_12;
	out->y = m->_13;
	out->z = m->_14;
}

void matrix3d_inverse( matrix *out, matrix *m ) {
	D3DXMatrixInverse(MAT(out),NULL,MAT(m));
}

void matrix3d_build( matrix *out, vector *t, quaternion *r, vector *s ) {
	D3DXMatrixTransformation(MAT(out),NULL,NULL,VEC(s),NULL,QUAT(r),VEC(t));
}

void matrix3d_of_quaternion( matrix *out, quaternion *q ) {
	D3DXMatrixRotationQuaternion(MAT(out),QUAT(q));
}

void matrix3d_lookat( matrix *out, vector *pos, vector *target, vector *up ) {
	D3DXMatrixLookAtRH(MAT(out),VEC(pos),VEC(target),VEC(up));
}

void matrix3d_proj( matrix *out, float field, float ratio, float znear, float zfar ) {
	D3DXMatrixPerspectiveFovRH( MAT(out), field, ratio, znear, zfar );
}

void matrix3d_transpose( matrix *out, matrix *m ) {
	D3DXMatrixTranspose( MAT(out), MAT(m) );
}

void vector_add( vector *out, vector *a, vector *b ) {
	D3DXVec3Add(VEC(out),VEC(a),VEC(b));
}
void vector_sub( vector *out, vector *a, vector *b ) {
	D3DXVec3Subtract(VEC(out),VEC(a),VEC(b));
}

void vector_scale( vector *out, vector *a, float v ) {
	D3DXVec3Scale(VEC(out),VEC(a),v);
}

void vector_transform( vector *out, vector *v, matrix *m ) {
	D3DXVECTOR4 tmp;
	D3DXVec3Transform(&tmp,VEC(v),MAT(m));
	out->x = tmp.x;
	out->y = tmp.y;
	out->z = tmp.z;
}

void vector_transform_coord( vector *out, vector *v, matrix *m ) {
	D3DXVec3TransformCoord(VEC(out),VEC(v),MAT(m));
}

void vector_normalize( vector *out, vector *v ) {
	D3DXVec3Normalize(VEC(out),VEC(v));
}

float vector_length( vector *v ) {
	return D3DXVec3Length(VEC(v));
}

float vector_dot( vector *a, vector *b ) {
	return D3DXVec3Dot(VEC(a),VEC(b));
}

void vector_cross( vector *out, vector *a, vector *b ) {
	D3DXVec3Cross(VEC(out),VEC(a),VEC(b));
}

void vector_lerp( vector *out, vector *a, vector *b, float f ) {
	D3DXVec3Lerp(VEC(out),VEC(a),VEC(b),f);
}

void quaternion_identity( quaternion *q ) {
	D3DXQuaternionIdentity(QUAT(q));
}

void quaternion_multiply( quaternion *out, quaternion *a, quaternion *b ) {
	D3DXQuaternionMultiply(QUAT(out),QUAT(a),QUAT(b));
}

void quaternion_rotation_axis( quaternion *out, vector *v, float r ) {
	D3DXQuaternionRotationAxis(QUAT(out),VEC(v),r);
}

void quaternion_rotation_angles( quaternion *out, float rx, float ry, float rz ) {
	D3DXQuaternionRotationYawPitchRoll(QUAT(out),-ry,rx,rz);
}

void quaternion_inverse( quaternion *out, quaternion *q ) {
	D3DXQuaternionInverse(QUAT(out),QUAT(q));
}

void quaternion_slerp( quaternion *out, quaternion *a, quaternion *b, float f ) {
	D3DXQuaternionSlerp(QUAT(out),QUAT(a),QUAT(b),f);
}

void quaternion_of_matrix( quaternion *out, matrix *m ) {
	D3DXQuaternionRotationMatrix(QUAT(out),MAT(m));
}

#else

void matrix3d_identity( matrix *out ) {
	out->_0 = 1;
	out->_1 = 0;
	out->_2 = 0;
	out->_3 = 0;
	out->_4 = 0;
	out->_5 = 1;
	out->_6 = 0;
	out->_7 = 0;
	out->_8 = 0;
	out->_9 = 0;
	out->_10 = 1;
	out->_11 = 0;
	out->_12 = 0;
	out->_13 = 0;
	out->_14 = 0;
	out->_15 = 1;
}

void matrix3d_multiply( matrix *out, matrix *a, matrix *b ) {
	if( out == a || out == b ) {
		matrix tmp;
		matrix3d_multiply(&tmp,a,b);
		matrix3d_copy(out,tmp);
		return;
	}
	out->_0 = a->_0 * b->_0 + a->_1 * b->_4 + a->_2 * b->_8 + a->_3 * b->_12;
	out->_1 = a->_0 * b->_1 + a->_1 * b->_5 + a->_2 * b->_9 + a->_3 * b->_13;
	out->_2 = a->_0 * b->_2 + a->_1 * b->_6 + a->_2 * b->_10 + a->_3 * b->_14;
	out->_3 = a->_0 * b->_4 + a->_1 * b->_7 + a->_2 * b->_11 + a->_3 * b->_15;

	out->_4 = a->_4 * b->_0 + a->_5 * b->_4 + a->_6 * b->_8 + a->_7 * b->_12;
	out->_5 = a->_4 * b->_1 + a->_5 * b->_5 + a->_6 * b->_9 + a->_7 * b->_13;
	out->_6 = a->_4 * b->_2 + a->_5 * b->_6 + a->_6 * b->_10 + a->_7 * b->_14;
	out->_7 = a->_4 * b->_4 + a->_5 * b->_7 + a->_6 * b->_11 + a->_7 * b->_15;

	out->_8 = a->_8 * b->_0 + a->_9 * b->_4 + a->_10 * b->_8 + a->_11 * b->_12;
	out->_9 = a->_8 * b->_1 + a->_9 * b->_5 + a->_10 * b->_9 + a->_11 * b->_13;
	out->_10 = a->_8 * b->_2 + a->_9 * b->_6 + a->_10 * b->_10 + a->_11 * b->_14;
	out->_11 = a->_8 * b->_4 + a->_9 * b->_7 + a->_10 * b->_11 + a->_11 * b->_15;

	out->_12 = a->_12 * b->_0 + a->_13 * b->_4 + a->_14 * b->_8 + a->_15 * b->_12;
	out->_13 = a->_12 * b->_1 + a->_13 * b->_5 + a->_14 * b->_9 + a->_15 * b->_13;
	out->_14 = a->_12 * b->_2 + a->_13 * b->_6 + a->_14 * b->_10 + a->_15 * b->_14;
	out->_15 = a->_12 * b->_4 + a->_13 * b->_7 + a->_14 * b->_11 + a->_15 * b->_15;
}

void matrix3d_affine( matrix *out, matrix *a, matrix *b ) {
	if( out == a || out == b ) {
		matrix tmp;
		matrix3d_affine(&tmp,a,b);
		matrix3d_copy(out,tmp);
		return;
	}
	out->_0 = a->_0 * b->_0 + a->_1 * b->_4 + a->_2 * b->_8;
	out->_1 = a->_0 * b->_1 + a->_1 * b->_5 + a->_2 * b->_9;
	out->_2 = a->_0 * b->_2 + a->_1 * b->_6 + a->_2 * b->_10;
	out->_3 = 0;

	out->_4 = a->_4 * b->_0 + a->_5 * b->_4 + a->_6 * b->_8;
	out->_5 = a->_4 * b->_1 + a->_5 * b->_5 + a->_6 * b->_9;
	out->_6 = a->_4 * b->_2 + a->_5 * b->_6 + a->_6 * b->_10;
	out->_7 = 0;

	out->_8 = a->_8 * b->_0 + a->_9 * b->_4 + a->_10 * b->_8;
	out->_9 = a->_8 * b->_1 + a->_9 * b->_5 + a->_10 * b->_9;
	out->_10 = a->_8 * b->_2 + a->_9 * b->_6 + a->_10 * b->_10;
	out->_11 = 0;

	out->_12 = a->_12 * b->_0 + a->_13 * b->_4 + a->_14 * b->_8 + b->_12;
	out->_13 = a->_12 * b->_1 + a->_13 * b->_5 + a->_14 * b->_9 + b->_13;
	out->_14 = a->_12 * b->_2 + a->_13 * b->_6 + a->_14 * b->_10 + b->_14;
	out->_15 = 1;
}

#endif

void matrix3d_copy( matrix *out, matrix *m ) {
	out->_0 = m->_0;
	out->_1 = m->_1;
	out->_2 = m->_2;
	out->_3 = m->_3;
	out->_4 = m->_4;
	out->_5 = m->_5;
	out->_6 = m->_6;
	out->_7 = m->_7;
	out->_8 = m->_8;
	out->_9 = m->_9;
	out->_10 = m->_10;
	out->_11 = m->_11;
	out->_12 = m->_12;
	out->_13 = m->_13;
	out->_14 = m->_14;
	out->_15 = m->_15;
}

void color_multiply( color *out, color *a, color *b ) {
	out->r = a->r * b->r;
	out->g = a->g * b->g;
	out->b = a->b * b->b;
	out->a = a->a * b->a;
}

/* ************************************************************************ */

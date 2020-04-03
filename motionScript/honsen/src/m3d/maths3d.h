/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

typedef struct vector {
	float x;
	float y;
	float z;
#ifdef __cplusplus
	vector() { }
	vector( float x, float y, float z ) : x(x), y(y), z(z) { }
	operator float* () { return (float*)this; }
#endif
} vector;

typedef struct quaternion {
	float x;
	float y;
	float z;
	float w;
#ifdef __cplusplus
	quaternion( ) { }
	quaternion( float x, float y, float z, float w ) : x(x), y(y), z(z), w(w) { }
	operator float* () { return (float*)this; }
#endif
} quaternion;

typedef struct matrix {
	float _0, _1, _2, _3;
	float _4, _5, _6, _7;
	float _8, _9, _10, _11;
	float _12, _13, _14, _15;
#ifdef __cplusplus
	operator float* () { return (float*)this; }
#endif
} matrix;

typedef struct color {
	float r, g, b, a;
#ifdef __cplusplus
	color( ) { }
	color( float r, float g, float b, float a ) : r(r), g(g), b(b), a(a) { }
	color( unsigned int c ) : r(((c >> 16) & 0xFF) / 255.0f), g(((c >> 8) & 0xFF) / 255.0f), b((c & 0xFF) / 255.0f), a((c >> 24) / 255.0f) { }
	operator float* () { return (float*)this; }
#endif
} color;

#ifdef __cplusplus
extern "C" {
#endif

void matrix3d_identity( matrix *out );
void matrix3d_copy( matrix *out, matrix *m );
void matrix3d_multiply( matrix *out, matrix *a, matrix *b );
void matrix3d_affine( matrix *out, matrix *a, matrix *b );
void matrix3d_get_translation( vector *out, matrix *m );
void matrix3d_inverse( matrix *out, matrix *m );
void matrix3d_build( matrix *out, vector *t, quaternion *r, vector *s );
void matrix3d_of_quaternion( matrix *out, quaternion *q );
void matrix3d_lookat( matrix *out, vector *pos, vector *target, vector *up );
void matrix3d_proj( matrix *out, float field, float ratio, float znear, float zfar );
void matrix3d_transpose( matrix *out, matrix *m );

void vector_add( vector *out, vector *a, vector *b );
void vector_sub( vector *out, vector *a, vector *b );
void vector_scale( vector *out, vector *a, float v );
void vector_transform( vector *out, vector *v, matrix *m );
void vector_transform_coord( vector *out, vector *v, matrix *m );
void vector_normalize( vector *out, vector *v );
float vector_length( vector *v );
float vector_dot( vector *a, vector *b );
void vector_cross( vector *out, vector *a, vector *b );
void vector_lerp( vector *out, vector *a, vector *b, float f );

void quaternion_identity( quaternion *q );
void quaternion_multiply( quaternion *out, quaternion *a, quaternion *b );
void quaternion_rotation_axis( quaternion *out, vector *v, float r );
void quaternion_rotation_angles( quaternion *out, float rx, float ry, float rz );
void quaternion_inverse( quaternion *out, quaternion *q );
void quaternion_slerp( quaternion *out, quaternion *a, quaternion *b, float f );

void color_multiply( color *out, color *a, color *b );

#ifdef __cplusplus
}
#endif

/* ************************************************************************ */

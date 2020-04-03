/* ************************************************************************ */
/*																			*/
/*	Honsen Vector Rendering													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

typedef double matrix[6];

typedef enum {
	PATH_MOVETO,
	PATH_LINETO,
	PATH_CURVETO,
	PATH_END
} BCODE;

typedef struct _BPath {
	BCODE code;
	float x;
	float y;
	float cx;
	float cy;
} BPath;

typedef struct _VPath {
	BCODE code;
	float x;
	float y;
} VPath;

#ifdef __cplusplus
extern "C" {
#endif

void matrix_identity( matrix m );
void matrix_multiply( matrix out, matrix a, matrix b );
void matrix_inverse( matrix out, matrix m );
void matrix_copy( matrix out, matrix m );
BPath *bpath_transform( BPath *path, matrix m, int nsegs );
int bpath_segment( VPath *vec, int nvecs, BPath *path, float epsilon );

#ifdef __cplusplus
}
#endif

/* ************************************************************************ */

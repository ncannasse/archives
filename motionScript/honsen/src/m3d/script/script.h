/* ************************************************************************ */
/*																			*/
/*	M3D-SE Scripting for MTS												*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#define CONV_FLOAT (float)
#include <value.h>
#include "../maths3d.h"

#define val_vector(o)	((vector*)val_odata(o))
#define val_quat(o)		((quaternion*)val_odata(o))
#define val_matrix(o)	((matrix*)val_odata(o))
#define val_mesh(o)		((Mesh*)val_odata(o))
#define val_texture(o) ((cached_texture*)val_odata(o))

DECLARE_CLASS(vector);
DECLARE_CLASS(quat);
DECLARE_CLASS(trs);
DECLARE_CLASS(matrix);
DECLARE_CLASS(texture);
DECLARE_CLASS(mesh);

value alloc_trs();
value alloc_matrix( const matrix *m );
value alloc_vector( const vector *v );
value alloc_quaternion( const quaternion *q );
void matrix_of_trs( value trs, matrix *out );

void vector_to_stringbuf( const vector *v, stringbuf b );
void quaternion_to_stringbuf( const quaternion *q, stringbuf b );

/* ************************************************************************ */

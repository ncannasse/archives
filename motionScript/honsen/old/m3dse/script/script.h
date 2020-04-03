/* ************************************************************************ */
/*																			*/
/*	M3D-SE Scripting for MTS												*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#define CONV_FLOAT (FLOAT)
#include "../../std/value.h"
#include "../m3d.h"
#include "../m3d_context.h"

namespace M3D {
	struct ScriptContext {
		VECTOR camera_pos;
		VECTOR camera_start_pos;
		VECTOR camera_target;
		VECTOR worldup;
		double camera_roll;
		double camera_zoom;		
	};
};

#define val_vector(o)	((VECTOR*)val_odata(o))
#define val_quat(o)		((QUATERNION*)val_odata(o))
#define val_matrix(o)	((MATRIX*)val_odata(o))
#define val_mesh(o)		((M3D::Mesh*)val_odata(o))

DECLARE_CLASS(vector);
DECLARE_CLASS(quat);
DECLARE_CLASS(trs);
DECLARE_CLASS(matrix);

DECLARE_CLASS(mesh);
DECLARE_CLASS(anim);
DECLARE_CLASS(group);
DECLARE_CLASS(sprite);
DECLARE_CLASS(text);

DECLARE_PRIM(m3dse_camera,0);

value alloc_trs();
value alloc_matrix( const MATRIX *m );
value alloc_vector( const VECTOR *v );
value alloc_quaternion( const QUATERNION *q );
void matrix_of_trs( value trs, MATRIX *out );

void vector_to_stringbuf( const VECTOR *v, stringbuf b );
void quaternion_to_stringbuf( const QUATERNION *q, stringbuf b );

/* ************************************************************************ */

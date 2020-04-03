/* ************************************************************************ */
/*																			*/
/*	M3D-SE Element for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "../m3d_2d.h"
#include "../m3d_context.h"
#include "../../std/value.h"

#define FLOAT_GET(t,name) \
	value t##_get_##name() { \
		value o = val_this(); \
		val_check_obj(o,t_##t); \
		return alloc_float( val_##t(o)->name ); \
	}

#define FLOAT_SET(t,name,update) \
	value t##_set_##name( value v ) { \
		value o = val_this(); \
		val_check_obj(o,t_##t); \
		if( !val_is_number(v) ) \
			return NULLVAL; \
		val_##t(o)->name = val_number(v); \
		val_##t(o)->update; \
		return v; \
	}

#define FLOAT_PROP(t,name,update)	FLOAT_GET(t,name)	FLOAT_SET(t,name,update)

extern otype t_group;
extern otype t_sprite;
extern otype t_text;

#define val_group(o)	((M3D::Group*)val_odata(o))

value element_get_x();
value element_set_x( value v );
value element_get_y();
value element_set_y( value v );
value element_get_visible();
value element_set_visible( value v );
value element_get_depth();
value element_set_depth( value v );
value element_get_color();
value element_set_color( value v );
value element_get_alpha();
value element_set_alpha( value v );
value element_get_parent();
value element_remove();

void element_delete( value v );

/* ************************************************************************ */

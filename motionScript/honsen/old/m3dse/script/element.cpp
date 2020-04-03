/* ************************************************************************ */
/*																			*/
/*	M3D-SE Element for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "element.h"

#define val_check_element(o)	if( !val_is_object(o) ) return NULLVAL; if( val_otype(o) != t_sprite && val_otype(o) != t_group && val_otype(o) != t_text ) return NULLVAL;
#define val_element(o)		((M3D::Element*)val_odata(o))

#define ELEMENT_GET(name) \
	value element_get_##name() { \
		value o = val_this(); \
		val_check_element(o); \
		return alloc_float( val_element(o)->name ); \
	}

#define ELEMENT_SET(name,update) \
	value element_set_##name( value v ) { \
		value o = val_this(); \
		val_check_element(o); \
		if( !val_is_number(v) ) \
			return NULLVAL; \
		val_element(o)->name = val_number(v); \
		val_element(o)->update; \
		return v; \
	}

#define ELEMENT_IGET(name) \
	value element_get_##name() { \
		value o = val_this(); \
		val_check_element(o); \
		return alloc_int( val_element(o)->name ); \
	}

#define ELEMENT_PROP(name,update)	ELEMENT_GET(name)	ELEMENT_SET(name,update)

ELEMENT_PROP(x,PosRotScaleModified());
ELEMENT_PROP(y,PosRotScaleModified());
ELEMENT_PROP(alpha,ColorAlphaModified());

ELEMENT_IGET(visible);
ELEMENT_IGET(depth);
ELEMENT_IGET(color);

value element_set_visible( value v ) {
	value o = val_this();
	val_check_element(o);
	val_element(o)->visible = val_bool(v);
	return alloc_bool(val_bool(v));
}

value element_set_depth( value v )  {
	value o = val_this();
	val_check_element(o);
	if( !val_is_int(v) )
		return NULLVAL;
	val_element(o)->SetDepth( val_int(v) );
	return v;
}

value element_set_color( value v ) {
	value o = val_this();
	val_check_element(o);
	if( !val_is_int(v) )
		return NULLVAL;
	val_element(o)->color = val_int(v);
	val_element(o)->ColorAlphaModified();
	return v;
}

value alloc_group( M3D::Group *g );

value element_get_parent() {
	value o = val_this();
	val_check_element(o);
	if( val_element(o)->parent == NULL )
		return NULLVAL;
	M3D::Group *g = val_element(o)->parent;
	return g->script_ref?(value)g->script_ref:alloc_group(g);
}

value element_remove() {
	value o = val_this();
	val_check_element(o);
	if( val_element(o)->parent != NULL )
		val_element(o)->parent->RemoveChild(val_element(o));
	delete val_element(o);
	val_otype(o) = NULL;
	val_odata(o) = NULL;
	val_gc(o,NULL);
	return val_true;
}

void element_delete( value o ) {
	if( val_element(o) )
		val_element(o)->script_ref = NULL;
}

void M3D::Element::CleanRef( void *ref ) {
	val_odata(ref) = NULL;
	val_otype(ref) = NULL;
	val_gc((value)ref,NULL);
}

/* ************************************************************************ */

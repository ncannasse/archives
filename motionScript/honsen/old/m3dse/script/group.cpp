/* ************************************************************************ */
/*																			*/
/*	M3D-SE Group for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "element.h"

DEFINE_CLASS(m3dse,group);

value group_new1( value p ) {
	value o = val_this();
	val_check_obj(o,t_group);
	if( val_is_number(p) ) {
		if( val_number(p) < 0 || val_number(p) > 1 )
			return NULLVAL;
		val_odata(o) = (value)new M3D::Group(M3D::__GetContext(),val_number(p));
	} else if( val_is_obj(p,t_group) )
		val_odata(o) = (value)new M3D::Group(val_group(p));
	else
		return NULLVAL;
	val_group(o)->script_ref = o;
	val_gc(o,element_delete);
	return o;
}

FLOAT_PROP(group,xscale,PosRotScaleModified());
FLOAT_PROP(group,yscale,PosRotScaleModified());
FLOAT_PROP(group,rotation,PosRotScaleModified());

value group_get_z() {
	value o = val_this();
	val_check_obj(o,t_group);
	return alloc_float( val_group(o)->z_base );
}

value group_set_z( value v ) {
	value o = val_this();
	val_check_obj(o,t_group);
	if( val_group(o)->parent != NULL )
		return NULLVAL;
	if( !val_is_number(v) )
		return NULLVAL;
	if( val_number(v) < 0 || val_number(v) > 1 )
		return NULLVAL;
	val_group(o)->SetZBase(val_number(v));
	return v;
}

value group_toString() {
	return alloc_string("#group");
}

value m3dse_group() {
	value o = alloc_class(&t_group);
	Constr(o,group,Group,1);
	Property(o,element,x);
	Property(o,element,y);
	Property(o,element,visible);
	Property(o,element,depth);
	Property(o,element,color);
	Property(o,element,alpha);
	Method(o,element,get_parent,0);
	Method(o,element,remove,0);
	Property(o,group,xscale);
	Property(o,group,yscale);
	Property(o,group,rotation);
	Property(o,group,z);
	Method(o,group,toString,0);
	return o; 
}

value alloc_group( M3D::Group *g ) {
	value o = alloc_object(&t_group);
	val_odata(o) = (value)g;
	g->script_ref = o;
	val_gc(o,element_delete);
	return o;
}

/* ************************************************************************ */

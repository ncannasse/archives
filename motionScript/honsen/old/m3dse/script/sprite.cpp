/* ************************************************************************ */
/*																			*/
/*	M3D-SE Sprite for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "element.h"

DEFINE_CLASS(m3dse,sprite);

#define val_sprite(o)	((M3D::Sprite*)val_odata(o))

value sprite_new1( value g ) {
	value o = val_this();
	val_check_obj(o,t_sprite);
	val_check_obj(g,t_group);	
	val_odata(o) = (value)new M3D::Sprite(val_group(g));
	val_sprite(o)->script_ref = o;
	val_gc(o,element_delete);
	return o;
}

FLOAT_PROP(sprite,width,Modified());
FLOAT_PROP(sprite,height,Modified());
FLOAT_PROP(sprite,tu,Modified());
FLOAT_PROP(sprite,tu2,Modified());
FLOAT_PROP(sprite,tv,Modified());
FLOAT_PROP(sprite,tv2,Modified());

value sprite_set_bitmap( value v ) {
	value o = val_this();
	val_check_obj(o,t_sprite);
	if( v == NULL )
		val_sprite(o)->texture = M3D::Cached<M3D::Resource::Texture>();
	else {
		val_check_obj(v,t_resource);
		val_sprite(o)->texture = M3D::Resource::LoadTexture(val_sprite(o)->context,(MTW*)val_resource(v));
	}
	return v;
}

value sprite_get_bitmap() {
	value o = val_this();
	val_check_obj(o,t_sprite);
	if( !val_sprite(o)->texture.Available() )
		return NULLVAL;
	return alloc_resource(val_sprite(o)->texture.Data()->res);
}

value sprite_toString() {
	return alloc_string("#sprite");
}

value m3dse_sprite() {
	value o = alloc_class(&t_sprite);	
	Constr(o,sprite,Sprite,1);
	Property(o,element,x);
	Property(o,element,y);
	Property(o,element,visible);
	Property(o,element,depth);
	Property(o,element,color);
	Property(o,element,alpha);
	Method(o,element,get_parent,0);
	Method(o,element,remove,0);
	Property(o,sprite,width);
	Property(o,sprite,height);
	Property(o,sprite,tu);
	Property(o,sprite,tu2);
	Property(o,sprite,tv);
	Property(o,sprite,tv2);
	Property(o,sprite,bitmap);
	Method(o,sprite,toString,0);
	return o; 
}

/* ************************************************************************ */

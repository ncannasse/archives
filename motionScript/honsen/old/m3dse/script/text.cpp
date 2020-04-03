/* ************************************************************************ */
/*																			*/
/*	M3D-SE Others for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "element.h"

DEFINE_CLASS(m3dse,text);

#define val_text(o)	((M3D::Text*)val_odata(o))

value text_new1( value g ) {
	value o = val_this();
	val_check_obj(o,t_text);
	val_check_obj(g,t_group);
	val_odata(o) = (value)new M3D::Text(val_group(g));
	val_text(o)->script_ref = o;
	val_gc(o,element_delete);
	return o;
}

value text_toString() {
	value o = val_this();
	val_check_obj(o,t_text);
	stringbuf b = alloc_stringbuf("#text:");
	stringbuf_append(b,val_text(o)->text.c_str());
	return stringbuf_to_string(b);
}

value text_get_font() {
	value o = val_this();
	val_check_obj(o,t_text);
	if( !val_text(o)->font.Available() )
		return NULLVAL;
	return alloc_resource(val_text(o)->font.Data()->res);
}

value text_set_font( value v ) {
	value o = val_this();
	val_check_obj(o,t_text);
	if( v == NULL )
		val_text(o)->font = M3D::Cached<M3D::Resource::FontRes>();
	else {
		val_check_obj(v,t_resource);
		val_text(o)->font = M3D::Resource::LoadFont(val_text(o)->context,(MTW*)val_resource(v));
	}
	return v;
}

value text_get_text() {
	value o = val_this();
	val_check_obj(o,t_text);
	return alloc_string( val_text(o)->text.c_str() );
}

value text_set_text( value v ) {
	value o = val_this();
	val_check_obj(o,t_text);
	if( !val_is_string(v) )
		return NULLVAL;
	val_text(o)->text = val_string(v);
	return v;
}

value m3dse_text() {
	value o = alloc_class(&t_text);
	Constr(o,text,Text,1);
	Property(o,element,x);
	Property(o,element,y);
	Property(o,element,visible);
	Property(o,element,depth);
	Property(o,element,color);
	Property(o,element,alpha);
	Method(o,element,get_parent,0);
	Method(o,element,remove,0);
	Property(o,text,font);
	Property(o,text,text);
	Method(o,text,toString,0);
	return o;
}

/* ************************************************************************ */

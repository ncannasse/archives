/* ************************************************************************ */
/*																			*/
/*	M3D-SE Others for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "script.h"

value m3dse_get_time() {
	M3D::Context *c = M3D::__GetContext();	
	return alloc_float( c->time?c->time->curtime:0 );
}

value m3dse_get_width() {
	return alloc_int( M3D::__GetContext()->width );
}

value m3dse_get_height() {
	return alloc_int( M3D::__GetContext()->height );
}

value m3dse_display( value *args, unsigned int argc ) {
	unsigned int n;
	stringbuf b = alloc_stringbuf(NULL);
	for(n = 0; n < argc; n++ )
		val_to_string( args[n], b );
	M3D::Display( M3D::__GetContext(), val_string(stringbuf_to_string(b)) );
	return val_true;
}

value light_set_direction(value v) {
	val_check_obj(v,t_vector);
	M3D::Context *c = M3D::__GetContext();
	M3D::SetLightDir(c, val_vector(v));
	return v;
}

value light_get_direction() {
	M3D::Context *c = M3D::__GetContext();
	return alloc_vector(M3D::GetLightDir(c));
}

value light_set_color(value v) {
	if( !val_is_int(v) )
		return NULLVAL;
	M3D::Context *c = M3D::__GetContext();
	M3D::SetLightColor(c, val_int(v));
	return v;
}

value light_get_color() {
	M3D::Context *c = M3D::__GetContext();
	return alloc_int(M3D::GetLightColor(c));
}

value light_set_ambient(value v) {
	if( !val_is_int(v) )
		return NULLVAL;
	M3D::Context *c = M3D::__GetContext();
	M3D::SetAmbient(c, val_int(v));
	return v;
}

value light_get_ambient() {
	M3D::Context *c = M3D::__GetContext();
	return alloc_int(M3D::GetAmbient(c));
}

value light_toString() {
	return alloc_string("#light");
}

value m3dse_light() {
	value o = alloc_object(NULL);
	Property(o,light,direction);
	Property(o,light,color);
	Property(o,light,ambient);
	Method(o,light,toString,0);
	return o; 
}

DEFINE_PRIM(m3dse_light,0);

DEFINE_PRIM_MULT(m3dse_display);
DEFINE_PRIM(m3dse_get_time,0);
DEFINE_PRIM(m3dse_get_width,0);
DEFINE_PRIM(m3dse_get_height,0);

/* ************************************************************************ */

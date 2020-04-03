/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "script.h"
#include "../m3d_priv.h"
#include "../mesh.h"

#define val_texture(o)		((cached_texture*)val_odata(o))

DEFINE_CLASS(honsen,texture);

static void delete_cached( value o ) {
	Texture::Delete( val_texture(o) );
}

static value texture_new1( value res ) {
	value o = val_this();
	val_check_obj(o,t_texture);
	if( !val_is_resource(res) )
		return NULLVAL;
	MTW *m = (MTW*)val_resource(res);
	cached_texture *t = Texture::Make(M3D(),m);
	if( t == NULL ) {
		val_print(alloc_string("Invalid texture could not be loaded"));
		return NULLVAL;
	}
	val_odata(o) = (value)t;
	val_gc(o,delete_cached);
	return o;
}

static value texture_get_width() {
	int s;
	value o = val_this();
	val_check_obj(o,t_texture);
	Texture::Size( val_texture(o), &s, NULL );
	return alloc_int(s);
}

static value texture_get_height() {
	int s;
	value o = val_this();
	val_check_obj(o,t_texture);
	Texture::Size( val_texture(o), NULL, &s );
	return alloc_int(s);
}

static value texture_toString() {
	return alloc_string("#texture");
}

static value honsen_texture() {
	value o = alloc_class(&t_texture);
	Constr(o,texture,Texture,1);
	Method(o,texture,get_width,0);
	Method(o,texture,get_height,0);
	Method(o,texture,toString,0);
	return o;
}

value alloc_texture( cached_texture *t ) {
	if( t == NULL )
		return NULL;
	value o = alloc_object(&t_texture);
	val_odata(o) = (value)Texture::Copy(t);
	val_gc(o,delete_cached);
	return o;
}

/* ************************************************************************ */

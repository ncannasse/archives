/* ************************************************************************ */
/*																			*/
/*	Honsen Lib																*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <string.h>
#include <stdlib.h> 
typedef struct _HWND *HWND;
#include "hslib.h"
#include "../std/value.h"

#define val_key(o)		val_int(val_odata(o))

DEFINE_CLASS(hslib,key);

int get_key_id( const char *name ) {
	int i;
	HSLib::LibData *d = HSLib::GetData();
	for(i=0;i<MAX_KEY;i++)
		if( d->keynames[i] && strcmp(name,d->keynames[i]) == 0 )
			return i;
	return INVALID_KEY;
}

int get_key_code( int code ) {
	int i;
	if( code == -1 )
		return INVALID_KEY;
	HSLib::LibData *d = HSLib::GetData();
	for(i=0;i<MAX_KEY;i++)
		if( d->scancodes[i] == code )
			return i;
	return INVALID_KEY;
}
value key_new1( value code ) {
	value o = val_this();
	val_check_obj(o,t_key);
	if( val_is_int(code) ) {
		int k = get_key_code(val_int(code));
		if( k == INVALID_KEY )
			return NULLVAL;
		val_odata(o) = alloc_int(k);
	} else if( val_is_string(code) ) {
		int k = get_key_id(val_string(code));
		if( k == INVALID_KEY )
			return NULLVAL;
		val_odata(o) = alloc_int(k);
	} else
		return NULLVAL;
	return o;
}

value key_toString() {
	value o = val_this();
	val_check_obj(o,t_key);
	stringbuf b = alloc_stringbuf("#key:");
	HSLib::LibData *d = HSLib::GetData();
	if( d->keynames[val_key(o)] == NULL )
		val_to_string(alloc_int(val_key(o)),b);
	else
		stringbuf_append(b,d->keynames[val_key(o)]);
	return stringbuf_to_string(b);
}

value key_isDown() {
	value o = val_this();
	val_check_obj(o,t_key);
	HSLib::LibData *d = HSLib::GetData();
	return alloc_bool( d->keys[val_key(o)] > 0 );
}

value key_isPressed() {
	value o = val_this();
	val_check_obj(o,t_key);
	HSLib::LibData *d = HSLib::GetData();
	return alloc_bool( d->keys[val_key(o)] == 1 );
}

value key_isReleased() {
	value o = val_this();
	val_check_obj(o,t_key);
	HSLib::LibData *d = HSLib::GetData();
	return alloc_bool( d->keys[val_key(o)] < 0 );
}

value key_time() {
	value o = val_this();
	val_check_obj(o,t_key);
	HSLib::LibData *d = HSLib::GetData();
	return alloc_int( abs(d->keys[val_key(o)]) );
}

value key_name() {
	value o = val_this();
	val_check_obj(o,t_key);
	HSLib::LibData *d = HSLib::GetData();
	if( d->keynames[val_key(o)] == NULL )
		return NULLVAL;
	return alloc_string(d->keynames[val_key(o)]);
}

value key_code() {
	value o = val_this();
	val_check_obj(o,t_key);
	HSLib::LibData *d = HSLib::GetData();
	return alloc_int(d->scancodes[val_key(o)]);
}

value key_get_repeat() {
	value o = val_this();
	val_check_obj(o,t_key);
	HSLib::LibData *d = HSLib::GetData();
	return alloc_int(d->krepeat[val_key(o)]);
}

value key_set_repeat( value v ) {
	value o = val_this();
	val_check_obj(o,t_key);
	if( !val_is_int(v) )
		return NULLVAL;
	HSLib::LibData *d = HSLib::GetData();
	d->krepeat[val_key(o)] = val_int(v);
	return v;
}

value hslib_key() {
	value o = alloc_class(&t_key);
	Constr(o,key,Key,1);
	Method(o,key,toString,0);
	Method(o,key,isDown,0);
	Method(o,key,isPressed,0);
	Method(o,key,isReleased,0);
	Method(o,key,name,0);
	Method(o,key,code,0);
	Method(o,key,time,0);
	Property(o,key,repeat);
	return o;
}

value alloc_key( int code ) {
	value o = alloc_object(&t_key);
	val_odata(o) = alloc_int(code);
	return o;
}

/* ************************************************************************ */

value mouse_toString() {
	return alloc_string("#mouse");
}

value mouse_get_x() {
	HSLib::LibData *d = HSLib::GetData();
	return alloc_int(d->mouse_x);
}

value mouse_get_y() {
	HSLib::LibData *d = HSLib::GetData();
	return alloc_int(d->mouse_y);
}

value hslib_mouse() {
	value o = alloc_object(NULL);
	Method(o,mouse,toString,0);
	Method(o,mouse,get_x,0);
	Method(o,mouse,get_y,0);
	alloc_field(o,val_id("left"),alloc_key(MOUSE_BLEFT));
	alloc_field(o,val_id("right"),alloc_key(MOUSE_BRIGHT));
	alloc_field(o,val_id("middle"),alloc_key(MOUSE_BMIDDLE));
	return o;
}

DEFINE_PRIM(hslib_mouse,0);

/* ************************************************************************ */

value hslib_progress() {
	return alloc_float( HSLib::GetData()->progress );
}

DEFINE_PRIM(hslib_progress,0);

/* ************************************************************************ */

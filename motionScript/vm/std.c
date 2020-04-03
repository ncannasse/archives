#include "value.h"
#include "random.h"
#include "vmcontext.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// -------------------------------------------------------------------------//
// PRINTING

static value std_to_string( value v ) {
	stringbuf b;
	if( val_is_string(v) )
		return v;
	b = alloc_stringbuf(NULL);
	val_to_string(v,b);
	return stringbuf_to_string(b);
}

static value std_print( value *args, unsigned int argc ) {
	unsigned int n;
	value s;
	stringbuf b = alloc_stringbuf(NULL);
	context *c = get_std_context();
	for(n = 0; n < argc; n++ )
		val_to_string( args[n], b );
	s = stringbuf_to_string(b);
	if( c->params.printer )
		c->params.printer(c,val_string(s));
	else
		printf("%s",val_string(s));
	return val_true;
}

EXTERN void val_print( value v ) {
	if( val_is_string(v) ) {
		context *c = get_std_context();
		if( c->params.printer )
			c->params.printer(c,val_string(v));
		else
			printf("%s",val_string(v));
	} else
		std_print(&v,1);
}

// -------------------------------------------------------------------------//
// MISC

static value std_clock() {
	return alloc_float( (clock() * 1.0) / CLOCKS_PER_SEC );
}

static value std_abort() {
	get_std_context()->error = USER_ABORT;
	return NULLVAL;
}

static value std_compare( value a, value b ) {
	int r = val_compare(a,b);
	if( r == 0xFF )
		return NULLVAL;
	return alloc_int(r);
}

static value std_number( value n ) {
	switch( val_type(n) ) {
	case VAL_INT:
	case VAL_FLOAT:
		return n;
	case VAL_STRING: {
		tfloat f = atof( val_string(n) );
		if( f == (int)f )
			return alloc_int( (int)f );
		return alloc_float(f);
		}
	default:
		return NULLVAL;
	}
}

static value std_int( value n ) {
	switch( val_type(n) ) {
	case VAL_INT:
		return n;
	case VAL_FLOAT:
		return alloc_int( (int)val_float(n) );
	case VAL_STRING:
		return alloc_int( atoi(val_string(n)) );
	default:
		return NULLVAL;
	}
}

static value std_field_id( value s ) {
	if( !val_is_string(s) )
		return NULLVAL;
	return alloc_int( (int)val_id(val_string(s)));
}

static value std_obj_field( value o, value f ) {
	if( !val_is_object(o) || !val_is_int(f) )
		return NULLVAL;
	return val_field(o,(field)val_int(f));
}

static value std_set_main( value callback ) {
	context *c = get_std_context();
	if( callback == NULL ) {
		free_root( c->main );
		c->main = NULL;
	} else if( val_is_function(callback) && val_fun_nargs(callback) <= 0  ) {
		if( c->main == NULL )
			c->main = alloc_root(1);
		*c->main = callback;
		return val_true;
	}
	return NULLVAL;
}

void std_random_init( context *c ) {
	c->rnd = malloc(rnd_size());
	rnd_init(c->rnd);
}

static value std_random_float() {
	double big = 4294967296.0;
	rnd *r = (rnd*)get_std_context()->rnd;
	return alloc_float( ((rnd_int(r) / big + rnd_int(r)) / big + rnd_int(r)) / big );
}

static value std_do_random( value max ) {
	rnd *r = (rnd*)get_std_context()->rnd;
	if( !val_is_int(max) )
		return NULLVAL;
	if( val_int(max) <= 0 )
		return alloc_int(0);
	return alloc_int( (rnd_int(r) & 0x3FFFFFFF) % val_int(max) );
}

DEFINE_PRIM_MULT(std_print);
DEFINE_PRIM(std_random_float,0);
DEFINE_PRIM(std_do_random,1);
DEFINE_PRIM(std_int,1);
DEFINE_PRIM(std_number,1);
DEFINE_PRIM(std_to_string,1);
DEFINE_PRIM(std_compare,2);
DEFINE_PRIM(std_clock,0);
DEFINE_PRIM(std_abort,0);
DEFINE_PRIM(std_field_id,1);
DEFINE_PRIM(std_obj_field,2);
DEFINE_PRIM(std_set_main,1);

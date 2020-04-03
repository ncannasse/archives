/* ************************************************************************ */
/*																			*/
/*	 FX LIB EXPORTS															*/
/*																			*/
/* ************************************************************************ */
// INCLUDES

#include "fx.h"

MATRIX MID( 1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1);

FCOORDS DEFCOORDS( 0, 0, 1, 1 );

/* ************************************************************************ */
// FX DECLATION

#include "test.h"

const char *fx_names[] = {
	"Test",
};

FX_NEW fx_new[] = {
	TestFX::New
};

/* ************************************************************************ */
// EXPORT FUNCTIONS

int get_num_fx() {
	return sizeof(fx_names) / sizeof(const char *);
}

const char *get_fx_name( int i ) {
	return fx_names[i];
}

AnyFX* get_new_fx( int i, Params *p ) {
	return NULL;//fx_new[i](p);
}

/* ************************************************************************ */

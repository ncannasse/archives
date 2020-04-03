#include "value.h"
#include <math.h>

#define MATH_PRIM(f) \
	value std_##f( value n ) { \
		if( !val_is_number(n) ) \
			return NULLVAL; \
		return alloc_float( f( val_number(n) ) ); \
	} \
	DEFINE_PRIM(std_##f,1)

value std_atan2( value a, value b ) {
	if( !val_is_number(a) || !val_is_number(b) )
		return NULLVAL;
	return alloc_float( atan2(val_number(a),val_number(b)) );
}

value std_pow( value a, value b ) {
	if( val_is_int(a) && val_is_int(b) )
		return alloc_int( (int)pow(val_int(a),val_int(b)) );
	if( !val_is_number(a) || !val_is_number(b) )
		return NULLVAL;	
	return alloc_float( pow(val_number(a),val_number(b)) );
}

value std_abs( value n ) {
	switch( val_type(n) ) {
	case VAL_INT:
		return alloc_int( abs(val_int(n)) );
	case VAL_FLOAT:
		return alloc_float( fabs(val_float(n)) ); 
	default:
		return NULLVAL;
	}
}

value std_ceil( value n ) {
	switch( val_type(n) ) {
	case VAL_INT:
		return n;
	case VAL_FLOAT:
		return alloc_int( (int)ceil(val_float(n)) );
	default:
		return NULLVAL;
	}
}

value std_floor( value n ) {
	switch( val_type(n) ) {
	case VAL_INT:
		return n;
	case VAL_FLOAT:
		return alloc_int( (int)floor(val_float(n)) );
	default:
		return NULLVAL;
	}
}

value std_round( value n ) {
	switch( val_type(n) ) {
	case VAL_INT:
		return n;
	case VAL_FLOAT:
		{
			int ival = (int)val_float(n);
			if( val_float(n) > ival+0.5 )
				return alloc_int(ival+1);			
			return alloc_int(ival);
		}
		break;
	default:
		return NULLVAL;
	}
}

#define PI 3.1415926535897932384626433832795

value std_get_pi() {
	return alloc_float(PI);
}

MATH_PRIM(sqrt);
MATH_PRIM(atan);
MATH_PRIM(cos);
MATH_PRIM(sin);
MATH_PRIM(tan);
MATH_PRIM(log);
MATH_PRIM(exp);
MATH_PRIM(acos);
MATH_PRIM(asin);
DEFINE_PRIM(std_get_pi,0);
DEFINE_PRIM(std_atan2,2);
DEFINE_PRIM(std_pow,2);
DEFINE_PRIM(std_abs,1);
DEFINE_PRIM(std_ceil,1);
DEFINE_PRIM(std_floor,1);
DEFINE_PRIM(std_round,1);

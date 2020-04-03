#include <string.h>
#include "value.h"
#include "datastruct.h"

DEFINE_CLASS(std,array);

value array_new0() {
	value o = val_this();
	val_check_obj(o,t_array);
	val_odata(o) = alloc_block(0);
	return o;
}

value array_new1( value n ) {
	int size;
	value o;
	if( !val_is_int(n) )
		return NULLVAL;
	size = val_int(n);
	if( size < 0 || size > Max_block_size )
		return NULLVAL;
	o = val_this();
	val_check_obj(o,t_array);
	val_odata(o) = alloc_block(size);
	return o;
}

value array_new2( value x, value y ) {
	int i,sx,sy;
	value o, a, *aptr;
	if( !val_is_int(x) || !val_is_int(y) )
		return NULLVAL;
	sx = val_int(x);
	sy = val_int(y);
	if( sx < 0 || sy < 0 || sx > Max_block_size || sy > Max_block_size )
		return NULLVAL;
	o = val_this();
	val_check_obj(o,t_array);
	a = alloc_block(sx);
	aptr = val_block_ptr(a);
	val_odata(o) = a;	
	for(i=0;i<sx;i++) {
		aptr[i] = alloc_object(&t_array);
		val_odata(aptr[i]) = alloc_block(sy);
	}
	return o;
}

value array_new3( value x, value y, value z ) {
	int i,j;
	int sx, sy, sz;
	value o, a, *aptr;
	if( !val_is_int(x) || !val_is_int(y) || !val_is_int(z) )
		return NULLVAL;
	sx = val_int(x);
	sy = val_int(y);
	sz = val_int(z);
	if( sx < 0 || sy < 0 || sz < 0 || sx > Max_block_size || sy > Max_block_size || sz > Max_block_size )
		return NULLVAL;	
	o = val_this();
	a = alloc_block(sx);
	aptr = val_block_ptr(a);
	val_odata(o) = a;
	for(i=0;i<sx;i++) {
		value ai = alloc_block(sy);
		value *aiptr = val_block_ptr(ai);
		aptr[i] = alloc_object(&t_array);
		val_odata(aptr[i]) = ai;
		for(j=0;j<sy;j++) {
			aiptr[j] = alloc_object(&t_array);
			val_odata(aiptr[j]) = alloc_block(sz);
		}
	}
	return o;
}

value array_toString() {
	value o = val_this();
	stringbuf b;
	value *a;
	int size;
	int i;
	val_check_obj(o,t_array);
	b = alloc_stringbuf("[");
	a = val_array(o);
	size = val_array_size(o);
	for(i=0;i<size;i++) {
		val_to_string(a[i],b);
		if( i != size - 1 )
			stringbuf_append(b,";");
	}
	stringbuf_append(b,"]");
	return stringbuf_to_string(b);
}

value array_join( value sep ) {
	value o = val_this();
	stringbuf b;
	value *a;
	int size;
	int i;
	val_check_obj(o,t_array);
	if( !val_is_string(sep) )
		return NULLVAL;
	b = alloc_stringbuf(NULL);
	a = val_array(o);
	size = val_array_size(o);
	for(i=0;i<size;i++) {
		val_to_string(a[i],b);
		if( i != size - 1 )
			val_to_string(sep,b);
	}
	return stringbuf_to_string(b);
}

value array_get_length() {
	value o = val_this();
	val_check_obj(o,t_array);
	return alloc_int( val_array_size(o) );
}

value array_set_length( value v ) {
	int i;
	int size, msize;
	value o, *old_a, a, *aptr;
	if( !val_is_int(v) )
		return NULLVAL;
	size = val_int(v);
	if( size < 0 )
		size = 0;
	o = val_this();
	val_check_obj(o,t_array);
	old_a = val_array(o);
	a = alloc_block(size);
	aptr = val_block_ptr(a);
	msize = (size < val_array_size(o))?size:val_array_size(o);
	for(i=0;i<msize;i++)
		aptr[i] = old_a[i];
	// don't need to fill with 0 the array since the GC block
	// is already 0-filled.
	val_odata(o) = a;
	return alloc_int(size);
}

value array_at( value n ) {
	int p;
	value o, *a;
	if( !val_is_int(n) )
		return NULLVAL;
	p = val_int(n);
	o = val_this();
	val_check_obj(o,t_array);
	a = val_array(o);
	if( p < 0 || p >= val_array_size(o) )
		return NULLVAL;
	return a[p];
}

value array_set_at( value n, value v ) {
	int p;
	value o, *a;
	if( !val_is_int(n) )
		return NULLVAL;
	p = val_int(n);
	if( p < 0 )
		return NULLVAL;
	o = val_this();
	val_check_obj(o,t_array);
	a = val_array(o);
	if( p >= val_array_size(o) ) {
		array_set_length( alloc_int(p+1) );
		a = val_array(o);
	}
	a[p] = v;
	return v;
}

value array_concat( value *args, int nargs ) {
	value o = val_this();
	value *aptr;
	unsigned int size = 0, old_size;
	int n;
	val_check_obj(o,t_array);
	for(n=0;n<nargs;n++) {
		if( val_is_object(args[n]) && val_otype(args[n]) == t_array ) {
			if( args[n] == o )
				return NULLVAL;
			size += val_array_size(args[n]);
		}else
			size++;
	}
	if( size == 0 )
		return o;
	old_size = val_array_size(o);
	array_set_length( alloc_int(old_size + size) );
	aptr = val_array(o);
	size = old_size;
	for(n=0;n<nargs;n++) {
		if( val_is_object(args[n]) && val_otype(args[n]) == t_array ) {
			value *a2ptr = val_array(args[n]);
			int size2 = val_array_size(args[n]);
			memcpy(aptr,a2ptr,size2*sizeof(value));
			size += size2;
		}
		else
			aptr[size++] = args[n];
	}
	return o;
}

value array_sort( value fcmp ) {
	int i,j,len;
	value o = val_this();
	value *a;
	bool swap;
	val_check_obj(o,t_array);
	if( !val_is_function(fcmp) || (val_fun_nargs(fcmp) != 2 && val_fun_nargs(fcmp) != -1) )
		return NULLVAL;
	len = val_array_size(o);
	a = val_array(o);
	for(i=0;i<len;i++) {
		swap = false;
		for(j=0;j<len-i-1;j++) {
			value r = val_callN(fcmp,a+j,2);
			if( !val_is_int(r) )
				return NULLVAL;
			if( val_int(r) > 0 ) {
				r = a[j+1];
				a[j+1] = a[j];
				a[j] = r;
				swap = true;
			}	
		}
		if( !swap )
			break;
	}
	return val_true;
}

value array_toList() {
	value l;
	int i,len;
	value *a;
	value o = val_this();
	val_check_obj(o,t_array);
	len = val_array_size(o);
	a = val_array(o);
	l = alloc_list();
	for(i=0;i<len;i++)
		add_to_list(l,a[i]);
	return l;
}

value array_sub( value start, value len ) {
	value *aptr, *a2ptr;
	value o = val_this();
	value na;
	int i;
	int istart, ilen;
	val_check_obj(o,t_array);
	if( !val_is_int(start) || !val_is_int(len) )
		return NULLVAL;
	istart = val_int(start);
	ilen = val_int(len);
	if( istart < 0 || ilen < 0 || istart >= val_array_size(o) || istart + ilen > val_array_size(o) )
		return NULLVAL;
	na = alloc_array(ilen,&a2ptr);
	aptr = val_array(o);	
	for(i=0;i<ilen;i++)
		a2ptr[i] = aptr[i+istart];
	return na;
}

value array_iter( value f ) {
	value o = val_this();
	value *aptr;
	int i,len;
	val_check_obj(o,t_array);
	if( !val_is_function(f) )
		return NULLVAL;
	len = val_array_size(o);
	aptr = val_array(o);
	for(i=0;i<len;i++)
		val_call1(f,aptr[i]);
	return val_true;
}

value array_map( value f ) {
	value o = val_this();
	value o2;
	value *aptr, *aptr2;
	int i,len;
	val_check_obj(o,t_array);
	if( !val_is_function(f) )
		return NULLVAL;
	len = val_array_size(o);
	aptr = val_array(o);
	o2 = alloc_array(len,&aptr2);
	for(i=0;i<len;i++)
		aptr2[i] = val_call1(f,aptr[i]);
	return o2;
}

value std_array() {
	value o = alloc_class(&t_array);
	Constr(o,array,Array,0);
	Constr(o,array,Array,1);
	Constr(o,array,Array,2);
	Constr(o,array,Array,3);
	Method(o,array,toString,0);
	Property(o,array,length);
	Method(o,array,at,1);
	Method(o,array,set_at,2);
	Method(o,array,sub,2);
	Method(o,array,sort,1);
	Method(o,array,join,1);
	Method(o,array,iter,1);
	Method(o,array,map,1);
	Method(o,array,toList,0);
	MethodMult(o,array,concat);
	return o;
}

EXTERN value alloc_array( int size, value **ptr ) {
	value o = alloc_object(&t_array);
	val_odata(o) = alloc_block(size);
	if( ptr != NULL )
		*ptr = val_array(o);
	return o;
}

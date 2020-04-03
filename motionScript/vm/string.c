#include <malloc.h>
#include <string.h>
#include "value.h"
#include "datastruct.h"

#define stralloc(o,size)	val_odata(o) = (value)alloc_small_no_scan(size+5); *(int*)val_odata(o) = size

DEFINE_CLASS(std,string);

static value string_new0() {
	value o = val_this();
	val_check_obj(o,t_string);
	stralloc(o,0);
	return o;
}

static value string_new1( value s ) {
	int size;
	value o = val_this();
	val_check_obj(o,t_string);
	if( !val_is_string(s) )
		return NULL;
	size = val_strlen(s);
	stralloc(o,size);
	memcpy((char*)val_string(o),val_string(s),size+1);
	return o;
}

static value string_at( value n ) {
	int pos;
	value o = val_this();
	val_check_obj(o,t_string);
	if( !val_is_int(n) )
		return NULL;
	pos = val_int(n);
	if( pos < 0 || pos >= val_strlen(o) )
		return NULL;
	return alloc_int( val_string(o)[pos] );
}

static value string_charAt( value n ) {
	int pos;
	value s;
	value o = val_this();
	val_check_obj(o,t_string);
	if( !val_is_int(n) )
		return NULL;
	pos = val_int(n);
	if( pos < 0 || pos >= val_strlen(o) )
		return NULL;
	s = alloc_string(" ");
	*(char*)val_string(s) = val_string(o)[pos];
	return s;
}

static value string_get_length() {
	value o = val_this();
	val_check_obj(o,t_string);
	return alloc_int(val_strlen(o));
}

static value string_toString() {
	value o = val_this();
	val_check_obj(o,t_string);
	return o;
}

static value string_sub( value start, value len ) {
	value o = val_this();
	value v;
	int ilen,istart,slen;
	val_check_obj(o,t_string);
	if( !val_is_int(start) )
		return NULLVAL;
	istart = val_int(start);
	slen = val_strlen(o);
	if( istart < 0 || istart > slen )
		return NULLVAL;
	if( len == NULL )
		ilen = slen - istart;
	else if( val_is_int(len) ) {
		ilen = val_int(len);
		if( ilen + istart > slen )
			ilen = slen - istart;
		if( ilen < 0 )
			return NULL;
	} else
		return NULLVAL;
	v = alloc_raw_string(ilen);
	memcpy((char*)val_string(v),val_string(o) + istart,ilen);
	return v;
}

static value string_split( value s ) {
	value o = val_this();
	value l;
	int ilen;
	int slen;
	int start = 0;
	int pos;
	val_check_obj(o,t_string);
	if( !val_is_string(s) )
		return NULLVAL;
	ilen = val_strlen(o);
	slen = val_strlen(s);
	l = alloc_list();
	for(pos=0;pos<=ilen-slen;pos++)
		if( memcmp(val_string(o)+pos,val_string(s),slen) == 0 ) {
			add_to_list(l,copy_string(val_string(o)+start,pos-start));
			start = pos + slen;
			pos = start - 1;
		}
	if( ilen > 0 )
		add_to_list(l,copy_string(val_string(o)+start,ilen-start));
	return l;
}

value std_string() {
	value o = alloc_class(&t_string);
	Constr(o,string,String,0);
	Constr(o,string,String,1);
	Method(o,string,at,1);
	Method(o,string,charAt,1);
	Method(o,string,get_length,0);
	Method(o,string,toString,0);
	Method(o,string,split,1);
	Method(o,string,sub,2);
	return o;
}

EXTERN value alloc_raw_string( unsigned int size ) {
	value v = alloc_object(&t_string);
	v->h = val_mkheader(VAL_STRING,sizeof(vobject));
	stralloc(v,size);
	((char*)val_string(v))[size] = 0;
	return v;
}

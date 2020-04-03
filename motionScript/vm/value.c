#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "value.h"
#include "objtable.h"
#include "datastruct.h"

#define C(x,y)	((x << 8) | y)

__inline int icmp( int a, int b ) {
	return (a == b)?0:((a < b)?-1:1);
}

__inline int fcmp( tfloat a, tfloat b ) {
	return (a == b)?0:((a < b)?-1:1);
}

EXTERN int val_compare( value a, value b ) {
	char tmp_buf[32];
	switch( C(val_type(a),val_type(b)) ) {
	case C(VAL_NULL,VAL_NULL): return 0;
	case C(VAL_INT,VAL_INT): return icmp(val_int(a),val_int(b));
	case C(VAL_INT,VAL_FLOAT): return fcmp(val_int(a),val_float(b));
	case C(VAL_INT,VAL_STRING): 
		sprintf(tmp_buf,"%d",val_int(a));
		return strcmp(tmp_buf,val_string(b));
	case C(VAL_FLOAT,VAL_INT): return fcmp(val_float(a),val_int(b));
	case C(VAL_FLOAT,VAL_FLOAT): return fcmp(val_float(a),val_float(b));
	case C(VAL_FLOAT,VAL_STRING):
		sprintf(tmp_buf,"%.10g",val_float(a));
		return strcmp(tmp_buf,val_string(b));
	case C(VAL_STRING,VAL_INT):
		sprintf(tmp_buf,"%d",val_int(b));
		return strcmp(val_string(a),tmp_buf);
	case C(VAL_STRING,VAL_FLOAT):
		sprintf(tmp_buf,"%.10g",val_float(b));
		return strcmp(val_string(a),tmp_buf);
	case C(VAL_STRING,VAL_STRING): {
			int la = val_strlen(a);
			int lb = val_strlen(b);
			int r = memcmp(val_string(a),val_string(b),(la < lb)?la:lb); 
			if( r == 0 )
				return icmp(la,lb);
			return r;
		}
	case C(VAL_FUNCTION,VAL_FUNCTION): return (a == b)?0:0xFF;
	case C(VAL_PRIMITIVE,VAL_PRIMITIVE): return (a == b)?0:0xFF;
	case C(VAL_OBJECT,VAL_OBJECT):
		if( a == b )
			return 0;
		if( val_otype(a) == t_resource && val_otype(b) == t_resource )
			return (val_odata(a) == val_odata(b))?0:0xFF;
		return 0xFF;	
	default:
		if( a == b )
			return 0;
		return 0xFF;
	}
}

typedef struct _stringitem {
	char *str;
	int len;
	struct _stringitem *next;
} * stringitem;

struct _stringbuf {
	int totlen;
	stringitem data;
};

EXTERN stringbuf alloc_stringbuf( const char *init ) {
	stringbuf b = (stringbuf)alloc_small(sizeof(struct _stringbuf));
	b->totlen = 0;
	b->data = NULL;
	stringbuf_append(b,init);
	return b;
}

EXTERN void stringbuf_append_sub( stringbuf b, const char *s, int len ) {	
	stringitem it;
	if( s == NULL || len <= 0 )
		return;
	b->totlen += len;
	it = (stringitem)alloc_small(sizeof(struct _stringitem));
	it->str = alloc_small_no_scan(len);
	memcpy(it->str,s,len);
	it->len = len;
	it->next = b->data;
	b->data = it;
}

EXTERN void stringbuf_append( stringbuf b, const char *s ) {
	if( s == NULL )
		return;
	stringbuf_append_sub(b,s,strlen(s));
}

EXTERN value stringbuf_to_string( stringbuf b ) {
	value v = alloc_raw_string(b->totlen);
	stringitem it = b->data;
	char *s = (char*)val_string(v) + b->totlen;
	while( it != NULL ) {
		stringitem tmp;
		s -= it->len;
		memcpy(s,it->str,it->len);
		tmp = it->next;
		it = tmp;
	}
	return v;
}

EXTERN void val_to_string( value v, stringbuf b ) {
	val_t t = val_type(v);
	char buf[32];
	switch( t ) {
	case VAL_INT:
		sprintf(buf,"%d",val_int(v));
		stringbuf_append(b,buf);
		break;
	case VAL_STRING:
		{
			stringitem it;
			b->totlen += val_strlen(v);
			it = (stringitem)alloc_small(sizeof(struct _stringitem));
			it->str = (char*)val_string(v);
			it->len = val_strlen(v);
			it->next = b->data;
			b->data = it;
		}
		break;
	case VAL_FLOAT:
		sprintf(buf,"%.10g",val_float(v));
		stringbuf_append(b,buf);
		break;
	case VAL_NULL:
		stringbuf_append(b,"null");
		break;
	case VAL_FUNCTION:
	case VAL_PRIMITIVE:
		if( *val_fun_name(v) ) {
			stringbuf_append(b,"<");
			stringbuf_append(b,val_fun_name(v));
			stringbuf_append(b,">");
		} else
			stringbuf_append(b,"#function");
		break;	
	case VAL_OBJECT:
		{
			static field to_string = 0;
			value pr;
			if( !to_string )
				to_string = val_id("toString");
			pr = val_ocall0(v,to_string);
			if( !val_is_string(pr) )
				stringbuf_append(b,"#object");
			else
				val_to_string(pr,b);
		}
		break;
	case VAL_BLOCK:
		if( val_block_size(v) == 1 ) { // REF
			stringbuf_append(b,"#ref:");
			val_to_string(*val_block_ptr(v),b);
		} else {
			sprintf(buf,"#block:%d",val_block_size(v));
			stringbuf_append(b,buf);
		}
		break;
	default:
		stringbuf_append(b,"#unknown");
		break;
	}
}

EXTERN field val_id( const char *name ) {
	value acc = alloc_int(0);
	while( *name ) {
		acc = alloc_int(223 * val_int(acc) + *((unsigned char*)name));
		name++;
	}	
	return (field)val_int(acc);
}

EXTERN value val_field( value o, field id ) {
	value *f;
	if( !val_is_object(o) )
		return NULLVAL;
	f = otable_find(((vobject*)o)->table,id);
	if( f == NULL )
		return NULL;
	return *f;
}

static void std_obj_fields_rec( value v, field id, void *l ) {
	add_to_list(l, alloc_int((int)id));
}

static value std_obj_fields( value o ) {
	value l;
	if( !val_is_object(o) )
		return NULLVAL;
	l = alloc_list();	
	otable_iter(((vobject*)o)->table,std_obj_fields_rec,l);
	return l;
}

static value std_obj_set_field( value o, value f, value v ) {
	if( !val_is_object(o) || !val_is_int(f) )
		return NULLVAL;	
	alloc_field(o,(field)val_int(f),v);
	return val_true;
}

DEFINE_PRIM(std_obj_set_field,3);
DEFINE_PRIM(std_obj_fields,1);
DEFINE_CLASS(std,stringbuf);

static value _stringbuf_new0() {
	value o = val_this();
	val_check_obj(o,t_stringbuf);
	val_odata(o) = (value)alloc_stringbuf(NULL);
	return o;
}

static value _stringbuf_add( value v ) {
	value o = val_this();
	val_check_obj(o,t_stringbuf);
	val_to_string(v,(stringbuf)val_odata(o));
	return val_true;
}

static value _stringbuf_toString( value v ) {
	value o = val_this();
	val_check_obj(o,t_stringbuf);	
	return stringbuf_to_string((stringbuf)val_odata(o));
}

value std_stringbuf() {
	value o = alloc_class(&t_stringbuf);
	Constr(o,_stringbuf,StringBuf,0);
	Method(o,_stringbuf,add,1);
	Method(o,_stringbuf,toString,0);
	return o;
}

#include <string.h>
#include "value.h"
#include "datastruct.h"

DEFINE_CLASS(std,hash);

#define HASH_MASK 0xFF
#define hash_str(k)		(((int)val_id(val_string(k))) & HASH_MASK)

value hash_new0() {
	value o = val_this();
	val_check_obj(o,t_hash);
	val_odata(o) = alloc_block(HASH_SIZE);
	memset(val_odata(o),0,sizeof(value)*HASH_SIZE);
	return o;
}

value hash_toString() {
	value o = val_this();
	val_check_obj(o,t_hash);
	return alloc_string("#hash");
}

value hash_get( value k ) {	
	hitem *h;
	int p;
	int l;
	value o = val_this();
	val_check_obj(o,t_hash);
	if( !val_is_string(k) )
		return NULLVAL;
	l = val_strlen(k);
	p = hash_str(k);
	h = hash_item(o,p);
	while( h != NULL ) {
		if( l == val_strlen(h->str) && memcmp(val_string(h->str),val_string(k),l) == 0 )
			return h->v;
		h = h->next;
	}
	return NULLVAL;
}

value hash_exists( value k ) {	
	hitem *h;
	int p;
	int l;
	value o = val_this();
	val_check_obj(o,t_hash);
	if( !val_is_string(k) )
		return NULLVAL;
	l = val_strlen(k);
	p = hash_str(k);
	h = hash_item(o,p);
	while( h != NULL ) {
		if( l == val_strlen(h->str) && memcmp(val_string(h->str),val_string(k),l) == 0 )
			return val_true;
		h = h->next;
	}
	return val_false;
}

EXTERN bool hash_replace( value o, const char *k, int klen, value v ) {
	value kstr = copy_string(k,klen);
	const char *k2 = val_string(kstr);
	int p = ((int)val_id(k2)) & HASH_MASK;
	hitem *h = hash_item(o,p);
	while( h != NULL ) { 
		if( klen == val_strlen(h->str) && memcmp(val_string(h->str),k2,klen) == 0 ) {
			h->v = v;
			return false;
		}
		h = h->next;
	}
	h = (hitem*)alloc_block(3);
	h->next = hash_item(o,p);
	h->str = kstr;
	h->v = v;
	hash_item(o,p) = h;
	return true;
}

value hash_set( value k, value v ) {	
	value o = val_this();
	val_check_obj(o,t_hash);
	if( !val_is_string(k) )
		return NULLVAL;
	return alloc_bool( hash_replace(o,val_string(k),val_strlen(k),v) );
}

value hash_remove( value k ) {
	hitem *h, *prev;
	int p;
	int l;
	value o = val_this();
	val_check_obj(o,t_hash);
	if( !val_is_string(k) )
		return NULLVAL;
	l = val_strlen(k);
	p = hash_str(k);
	h = hash_item(o,p);
	prev = NULL;
	while( h != NULL ) { 
		if( l == val_strlen(h->str) && memcmp(val_string(h->str),val_string(k),l) == 0 ) {
			if( prev != NULL )
				prev->next = h->next;
			else
				hash_item(o,p) = h->next;			
			return val_true;
		}
		h = h->next;
	}
	return val_false;
}

value hash_iter( value f ) {
	hitem *h;
	int p;
	value o = val_this();
	val_check_obj(o,t_hash);
	if( !val_is_function(f) || (val_fun_nargs(f) != 2 && val_fun_nargs(f) != -1) )
		return NULLVAL;
	for(p=0;p<HASH_SIZE;p++) {
		h = hash_item(o,p);
		while( h != NULL ) {
			val_call2(f,h->str,h->v);
			h = h->next;
		}
	}
	return val_true;
}

value std_hash() {
	value o = alloc_class(&t_hash);
	Constr(o,hash,Hash,0);
	Method(o,hash,toString,0);
	Method(o,hash,get,1);
	Method(o,hash,set,2);
	Method(o,hash,remove,1);
	Method(o,hash,exists,1);
	Method(o,hash,iter,1);
	return o;
}

EXTERN value alloc_hash() {
	value o = alloc_object(&t_hash);
	val_odata(o) = alloc_block(HASH_SIZE);
	memset(val_odata(o),0,sizeof(value)*HASH_SIZE);
	return o;
}

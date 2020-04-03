#include "value.h"
#include "datastruct.h"

#define alloc_list_data() (value)alloc_small(sizeof(struct _list))

DEFINE_CLASS(std,list);

value list_new0() {
	value o = val_this();
	val_check_obj(o,t_list);
	val_odata(o) = alloc_list_data();
	return o;
}

EXTERN void add_to_list( value o, value v ) {
	list l;
	litem i;

	l = val_list(o);
	i = (litem)alloc_small(sizeof(struct _litem));
	i->item = v;
	i->next = NULLVAL;
	if( l->last == NULL ) {
		l->first = i;
		l->last = i;
	} else {
		l->last->next = i;
		l->last = i;
	}
	l->len++;
}

value list_toString() {
	value o = val_this();
	stringbuf b;
	litem l;
	val_check_obj(o,t_list);
	b = alloc_stringbuf("[");
	l = val_list(o)->first;
	while( l != NULL ) {
		val_to_string(l->item,b);
		l = l->next;
		if( l != NULL )
			stringbuf_append(b,";");
	}
	stringbuf_append(b,"]");
	return stringbuf_to_string(b);
}

static value list_add( value v ) {
	value o = val_this();
	val_check_obj(o,t_list);
	add_to_list(o,v);
	return v;
}

value list_push( value v ) {
	list l;
	litem i;
	value o = val_this();
	val_check_obj(o,t_list);
	l = val_list(o);
	i = (litem)alloc_small(sizeof(struct _litem));
	i->item = v;
	i->next = l->first;
	l->first = i;
	l->len++;
	return v;
}

value list_pop() {
	value o = val_this();
	list l;
	value v;
	val_check_obj(o,t_list);
	l = val_list(o);
	if( l->first == NULL )
		return NULLVAL;
	v = l->first->item;
	l->first = l->first->next;
	if( l->len == 1 )
		l->last = l->first;
	l->len--;
	return v;
}

static value list_remove( value v ) {
	value o = val_this();
	list l;
	litem i;
	litem prev = NULL;
	val_check_obj(o,t_list);
	l = val_list(o);
	i = l->first;
	while( i != NULL && val_compare(i->item,v) != 0 ) {
		prev = i;
		i = i->next;
	}
	if( i == NULL )
		return val_false;
	if( l->last == i )
		l->last = prev;
	if( prev == NULL )
		l->first = i->next;
	else
		prev->next = i->next;
	l->len--;
	return val_true;
}

value list_get_length() {
	value o = val_this();
	val_check_obj(o,t_list);
	return alloc_int( val_list(o)->len );
}

value list_iter( value f ) {
	value o = val_this();
	list l;
	value i;
	val_check_obj(o,t_list);
	if( !val_is_function(f) )
		return NULLVAL;
	l = val_list(o);
	i = (value)l->first;
	// concurrent mutation of the list are handled
	// by the GC : we are following the links...
	while( i != NULL ) {
		val_call1(f,((litem)i)->item);
		i = (value)((litem)i)->next;
	}
	return NULLVAL;
}

value list_map( value f ) {
	value o = val_this();
	list l;
	value i, l2;
	val_check_obj(o,t_list);
	if( !val_is_function(f) )
		return NULLVAL;
	l = val_list(o);
	i = (value)l->first;
	l2 = alloc_list();
	while( i != NULL ) {
		add_to_list(l2 , val_call1(f,((litem)i)->item) );
		i = (value)((litem)i)->next;
	}
	return l2;
}

value list_find( value f ) {
	value o = val_this();
	list l;
	value i;
	val_check_obj(o,t_list);
	if( !val_is_function(f) )
		return NULLVAL;
	l = val_list(o);
	i = (value)l->first;
	while( i != NULL ) {
		value v = val_call1(f,((litem)i)->item);
		if( v != NULLVAL )
			return v;
		i = (value)((litem)i)->next;
	}
	return NULLVAL;
}

value list_exists( value f ) {
	value o = val_this();
	list l;
	value i;
	val_check_obj(o,t_list);
	if( !val_is_function(f) )
		return NULLVAL;
	l = val_list(o);
	i = (value)l->first;
	while( i != NULL ) {
		value v = val_call1(f,((litem)i)->item);
		if( v == val_true )
			return v;
		i = (value)((litem)i)->next;
	}
	return val_false;
}

value list_toArray() {
	value a;
	value *aptr;
	list l;
	litem li;
	value o = val_this();
	val_check_obj(o,t_list);
	l = val_list(o);
	li = l->first;
	a = alloc_array(l->len,&aptr);
	while( li != NULLVAL ) {
		*aptr++ = li->item;
		li = li->next;
	}
	return a;
}

value list_at( value p ) {
	value o = val_this();
	litem l;
	int n;
	val_check_obj(o,t_list);
	if( !val_is_int(p) )
		return NULLVAL;
	n = val_int(p);
	l = val_list(o)->first;
	while( l != NULL ) {
		if( n == 0 )
			return l->item;
		n--;
		l = l->next;
	}
	return NULLVAL;
}

value list_first() {
	value o = val_this();
	litem l;
	val_check_obj(o,t_list);
	l = val_list(o)->first;
	return (l == NULL)?NULL:l->item;
}

value list_last() {
	value o = val_this();
	litem l;
	val_check_obj(o,t_list);
	l = val_list(o)->last;
	return (l == NULL)?NULL:l->item;
}

value list_join( value sep ) {
	value o = val_this();
	stringbuf b;
	litem l;
	val_check_obj(o,t_list);
	if( !val_is_string(sep) )
		return NULLVAL;
	b = alloc_stringbuf(NULL);
	l = val_list(o)->first;
	while( l != NULL ) {
		val_to_string(l->item,b);
		l = l->next;
		if( l != NULL )
			val_to_string(sep,b);
	}
	return stringbuf_to_string(b);
}


value std_list() {
	value o = alloc_class(&t_list);
	Constr(o,list,List,0);
	Method(o,list,get_length,0);
	Method(o,list,toString,0);
	Method(o,list,add,1);
	Method(o,list,remove,1);
	Method(o,list,push,1);
	Method(o,list,pop,0);
	Method(o,list,iter,1);
	Method(o,list,map,1);
	Method(o,list,find,1);
	Method(o,list,exists,1);
	Method(o,list,at,1);
	Method(o,list,first,0);
	Method(o,list,last,0);
	Method(o,list,join,1);
	Method(o,list,toArray,0);
	return o;
}

EXTERN value alloc_list() {
	value l = alloc_object(&t_list);
	val_odata(l) = alloc_list_data();
	return l;
}

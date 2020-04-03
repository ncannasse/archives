#include <string.h>
#include <malloc.h>
#include <string.h>
#include "value.h"
#include "objtable.h"
#ifdef _WIN32
#	define GC_DLL
#	define GC_THREADS
#	define GC_WIN32_THREADS
#endif
#include "gc/gc.h"

struct _otype {
	value (*init)();
};

static void null_warn_proc( char *msg, int arg ) {
}

void gc_init() {
	GC_no_dls = 1;
	GC_dont_expand = 1;
	GC_clear_roots();
	GC_set_warn_proc((GC_warn_proc)null_warn_proc);
}

void gc_loop() {
	GC_collect_a_little();
}

void gc_major() {
	GC_gcollect();
}

EXTERN char *alloc_small( unsigned int nbytes ) {
	return GC_MALLOC(nbytes);
}

EXTERN char *alloc_small_no_scan( unsigned int nbytes ) {
	return GC_MALLOC_ATOMIC(nbytes);
}

EXTERN value alloc_string( const char *str ) {
	if( str == NULL )
		return NULLVAL;
	return copy_string(str,strlen(str));
}

EXTERN value alloc_float( tfloat f ) {
	vfloat *v = (vfloat*)GC_MALLOC_ATOMIC(sizeof(vfloat));
	v->h = val_mkheader(VAL_FLOAT,sizeof(tfloat));
	v->f = f;
	return (value)v;
}

EXTERN value alloc_block( unsigned int n ) {
	value v;
	if( n > Max_block_size )
		return NULLVAL;
	v = (value)GC_MALLOC((n<<2)+sizeof(header));
	v->h = val_mkheader(VAL_BLOCK,n*sizeof(value));
	return v;
}

static value resource_toString() {
	value v = val_this();
	val_check_obj(v,t_resource);
	return alloc_string("#resource");
}

EXTERN value alloc_resource( void *r ) {
	value v = alloc_object(NULL);
	val_otype(v) = t_resource;
	val_odata(v) = (value)r;
	Method(v,resource,toString,0);
	return v;
}

EXTERN value alloc_fun( const char *name, void *c_prim, unsigned int nargs ) {
	vfunction *v;
	if( c_prim == NULL )
		return NULLVAL;
	v = (vfunction*)GC_MALLOC(sizeof(vfunction));
	v->h = val_mkheader(VAL_PRIMITIVE,sizeof(vfunction));
	v->name = name;
	v->addr = c_prim;
	v->nargs = nargs;
	v->env = NULLVAL;
	return (value)v;
}

EXTERN value copy_object( value cpy ) {
	vobject *v;
	if( cpy != NULLVAL && !val_is_object(cpy) )
		return NULLVAL;
	v = (vobject*)GC_MALLOC(sizeof(vobject));
	v->h = val_mkheader(VAL_OBJECT,sizeof(vobject));
	v->data = cpy?((vobject*)cpy)->data:NULL;
	v->t = cpy?((vobject*)cpy)->t:(otype)0xFFFFFFFF;
	v->table = cpy?otable_copy(((vobject*)cpy)->table):otable_empty();
	return (value)v;
}

EXTERN value copy_string( const char *str, unsigned int strlen ) {
	value v = alloc_raw_string(strlen);
	char *c = (char*)val_string(v);
	memcpy(c,str,strlen);
	return v;
}

// not static, used by interp
value create_instance( value baseclass ) {
	value o;
	if( !val_is_object(baseclass) )
		return NULLVAL;
	o = copy_object(baseclass);
	val_odata(o) = NULLVAL;
	val_otype(o) = (otype)val_odata(baseclass);
	return o;
}

EXTERN value alloc_object( otype *t ) {
	value baseclass;
	if( !t )
		return copy_object(NULL);
	if( GC_base(*t) )
		baseclass = (value)(*t);
	else {
		baseclass = ((otype)t)->init();
 		*t = (otype)baseclass;
	}
	return create_instance(baseclass);
}

EXTERN value alloc_class( otype *t ) {
	value baseclass;
	if( !t ) {
		baseclass = copy_object(NULL);
		val_otype(baseclass) = t_class;
	} else if( GC_base(*t) )
		baseclass = (value)(*t);
	else {
		baseclass = (value)GC_MALLOC_UNCOLLECTABLE(sizeof(vobject));
		baseclass->h = val_mkheader(VAL_OBJECT,sizeof(vobject));
		((vobject*)baseclass)->table = otable_empty();
		val_otype(baseclass) = t_class;
		val_odata(baseclass) = baseclass;
		*t = (otype)baseclass;
	}
	return baseclass;
}

EXTERN void alloc_field( value obj, field f, value v ) {
	if( v == NULL )
		otable_remove(((vobject*)obj)->table,f);
	else
		otable_replace(((vobject*)obj)->table,f,v);
}

static void __on_finalize(value v, void *f ) {
	((finalizer)f)(v);
}

EXTERN void val_gc(value v, finalizer f ) {
	if( f )
		GC_register_finalizer(v,(GC_finalization_proc)__on_finalize,f,0,0);
	else
		GC_register_finalizer(v,NULL,NULL,0,0);
}

#ifdef _DEBUG
#include "context.h"
typedef struct root_list {
	value *v;
	int size;
	int thread;
	struct root_list *next;
} root_list;
_context *roots_context = NULL;
root_list *roots = NULL;
int thread_count = 0;
#endif

EXTERN value *alloc_root( unsigned int size ) {
	value *v = (value*)GC_MALLOC_UNCOLLECTABLE(size*sizeof(value));
#ifdef _DEBUG
	root_list *r = malloc(sizeof(root_list));
	if( roots_context == NULL )
		roots_context = context_new();
	if( context_get_data(roots_context) == NULL )
		context_set_data(roots_context,(void*)++thread_count);
	r->v = v;
	r->size = size;
	r->next = roots;
	r->thread = (int)context_get_data(roots_context);
	roots = r;
#endif
	return v;
}

EXTERN void free_root(value *v) {
#ifdef _DEBUG
	root_list *r = roots;
	root_list *prev = NULL;
	if( v == NULL )
		return;
	while( r != NULL && r->v != v ) {
		prev = r;
		r = r->next;
	}
	if( prev == NULL )
		roots = r->next;
	else
		prev->next = r->next;
	free(r);
#endif
	GC_free(v);
}

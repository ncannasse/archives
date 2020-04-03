#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "interp.h"
#include "vmcontext.h"
#include "api.h"

_context *mtsvm_context = NULL;

extern void std_random_init( context *c );

// -------------------------------------------------------------------------//

struct _read_h {
	const char *data;
	const char *limit;
};

static int read_mem(read_h h, void *data, int size ) {
	if( h->data + size > h->limit || h->data + size < h->data )
		return -1;
	memcpy(data,h->data,size);
	h->data += size;
	return size;
}

static binary *load_code(const void *data, int datasize ) {
	struct _read_h h;
	binary *b;
	h.data = (char*)data;
	h.limit = h.data+datasize;
	b = generic_load(&h,read_mem);
	return b;
}

// -------------------------------------------------------------------------//

EXTERN void vm_global_init() {
	gc_init();
	if( mtsvm_context == NULL )
		mtsvm_context = context_new();
}

EXTERN void vm_global_free() {
	context_delete(mtsvm_context);
}

EXTERN void vm_gc_loop() {
	gc_loop();
}

EXTERN void vm_gc_major() {
	gc_major();
}

EXTERN vm* vm_alloc( vmparams *params ) {
	vm *v = context_alloc_data(mtsvm_context,sizeof(context));
	std_random_init(v);
	v->params = *params;
	return v;
}

EXTERN vmparams *vm_params( vm *v ) {
	return &v->params;
}

EXTERN VM_ERRORCODE vm_error( vm *v ) {
	return v->error;
}

EXTERN void vm_reset_error( vm *v ) {
	v->error = NOERROR;
}

EXTERN void vm_select( vm *v ) {
	context_set_data(mtsvm_context,v);
}

EXTERN vm *vm_current() {
	return (vm*)context_get_data(mtsvm_context);
}

EXTERN void vm_init( vm *v, void *code_data, int code_size ) {
	binary *b;
	vm_select(v);
	b = load_code(code_data,code_size);
	if( b == NULL )
		return;
	init(b);
	if( v->bin != NULL )
		free_code(v->bin);
	v->bin = b;
}

EXTERN void vm_execute( vm *v ) {
	vm_select(v);
	exec(0,NULL);
}

EXTERN void vm_reset( vm *v ) {
	vm_select(v);
	if( v->bin != NULL ) {
		free_root((value*)v->globals);
		free_root((value*)v->spmin);
		free_root(v->main);
		init(v->bin);
	}
}

EXTERN void vm_abort( vm *v ) {
	v->pcmax = NULL;
}

EXTERN void vm_free( vm *v ) {
	void *old = context_get_data(mtsvm_context);

	free(v->rnd);
	free_root((value*)v->globals);
	free_root((value*)v->spmin);
	free_root(v->main);
	if( v->bin != NULL )
		free_code(v->bin);

	context_set_data(mtsvm_context,v);
	context_free_data(mtsvm_context);	
	context_set_data(mtsvm_context,old);
}

EXTERN value vm_main( vm *v ) {
	if( v->main == NULL )
		return NULL;
	return *v->main;
}

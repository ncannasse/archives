#ifndef API_H
#define API_H

#include "value.h"

typedef enum {
	NOERROR = 0,
	// Load Errors
	FILE_NOT_FOUND,
	INVALID_FILE,
	NOT_LINKED,
	NO_RESLOADER,
	PRIMITIVE_NOT_FOUND,
	// Execute errors
	ABORTED,
	USER_ABORT,
	INVALID_OPCODE,
	OVERFLOW,
	UNDERFLOW,
	OUT_CODE,
	OUT_STACK,
	OUT_ENV
} VM_ERRORCODE;

typedef struct context vm;

typedef value (*resloader)( vm *vm, unsigned int id );
typedef void (*printer)( vm *vm, const char *log );
typedef void *(*prim_resolve)( vm *vm, const char *name, int nargs );

typedef struct {
	resloader loader;
	printer printer;
	prim_resolve resolve;
	void *custom;
} vmparams;

C_FUNCTION_BEGIN

EXTERN void vm_global_init();
EXTERN void vm_global_free();
EXTERN void vm_gc_loop();
EXTERN void vm_gc_major();

EXTERN vm *vm_alloc( vmparams *init_params ); // copy
EXTERN void vm_init( vm *vm, void *code_data, int code_size );
EXTERN vmparams *vm_params( vm *v );
EXTERN VM_ERRORCODE vm_error( vm *v );
EXTERN void vm_reset_error( vm *v );
EXTERN value vm_main( vm *v );
EXTERN void vm_select( vm *v );
EXTERN void vm_abort( vm *v );
EXTERN void vm_execute( vm *v );
EXTERN void vm_reset( vm *v );
EXTERN void vm_free( vm *v );
EXTERN vm *vm_current();

EXTERN void *dll_primitive_resolve( vm *v, const char *, int nargs );
EXTERN void dll_primitive_free();

C_FUNCTION_END

#endif

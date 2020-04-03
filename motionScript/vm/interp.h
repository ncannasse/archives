#ifndef _INTERP_H
#define _INTERP_H

#include "value.h"
#define CALLBACK_RETURN		((unsigned int)-1)
#define BINARY_DEFINED

typedef struct _binary binary;

struct _binary {
	int nglobals;
	int codesize;
	value *globals;
	unsigned int *code;
};

typedef struct _read_h *read_h;
typedef int (*read_f)(read_h,void *,int);

void gc_init();
void gc_loop();
void gc_major();

binary *generic_load(read_h h, read_f read );
void init( binary *b );
void free_code( binary *b );
value exec(unsigned int entry, value env);

#endif

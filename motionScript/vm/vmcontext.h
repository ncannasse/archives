#ifndef _CONTEXT_H
#define _CONTEXT_H

#include "context.h"
#include "api.h"

#ifndef BINARY_DEFINED
typedef struct _binary binary;
#endif

typedef struct context {
	unsigned int *sp;
	unsigned int *csp;
	unsigned int _this;
	unsigned int *globals;
	unsigned int globals_size;

	unsigned int *spmin;
	unsigned int *spmax;
	int *pcmin;
	int *pcmax;

	binary *bin;
	value *main;
	vmparams params;
	int error;
	unsigned int error_pc;

	char tmp[100];
	void *env;
	void *rnd;
} context;

extern _context *mtsvm_context;

#ifdef _WIN32
	__inline
#else
	static inline
#endif
	context *get_std_context() { return (context*)context_get_data(mtsvm_context); }

#endif

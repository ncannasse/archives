#include "context.h"
#include <malloc.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>

_context *context_new() {
	DWORD t = TlsAlloc();
	TlsSetValue(t,NULL);
	return (_context*)t;
}

void context_delete( _context *ctx ) {
	TlsFree((DWORD)ctx);
}

void *context_alloc_data( _context *ctx, int size ) {
	void *data;
	if( ctx == NULL )
		return NULL;
	data = malloc(size);
	memset(data,0,size);
	TlsSetValue((DWORD)ctx,data);
	return data;
}

void context_set_data( _context *ctx, void *c ) {
	TlsSetValue((DWORD)ctx,c);
}

void context_free_data( _context *ctx ) {
	void *data;
	if( ctx == NULL )
		return;
	data = (void*)TlsGetValue((DWORD)ctx);
	TlsSetValue((DWORD)ctx,NULL);
	free(data);
}

void *context_get_data( _context *ctx ) {
	if( ctx == NULL )
		return NULL;
	return (void*)TlsGetValue((DWORD)ctx);
}

#else

struct _context {
	void *data;
};

_context *context_new() {
	_context *c = malloc(sizeof(_context));
	c->data = NULL;
	return c;
}

void context_delete( _context *ctx ) {
	free(ctx);
}

void *context_alloc_data( _context *ctx, int size ) {
	void *data;
	if( ctx == NULL )
		return NULL;
	data = malloc(size);
	memset(data,0,size);
	ctx->data = data;
	return data;
}

void context_set_data( _context *ctx, void *data ) {
	ctx->data = data;
}

void context_free_data( _context *ctx ) {
	void *data;
	if( ctx == NULL )
		return;
	data = ctx->data;
	ctx->data = NULL;
	free(data);
}

void *context_get_data( _context *ctx ) {
	if( ctx == NULL )
		return NULL;
	return ctx->data;
}

#endif

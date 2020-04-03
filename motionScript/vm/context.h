#ifndef _COMMON_CONTEXT_H
#define _COMMON_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _context _context;

_context *context_new();
void context_delete( _context *ctx );

void *context_alloc_data( _context *ctx, int size );
void context_set_data( _context *ctx, void *v );
void *context_get_data( _context *ctx );
void context_free_data( _context *ctx );


#ifdef __cplusplus
};
#endif

#endif

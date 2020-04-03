#ifndef MODMTS_CONTEXT_H
#define MODMTS_CONTEXT_H

#include <httpd.h>
#include <http_config.h>
#include <http_core.h>
#include <http_log.h>
#include <http_main.h>
#include <http_protocol.h>
#undef NOERROR
#undef INLINE
#include <vmcontext.h>
#include <api.h>

typedef struct {
	request_rec *r;
	value post_data;
	bool headers_sent;
	bool allow_write;
} mcontext;

#define CONTEXT()	((mcontext*)get_std_context()->params.custom)

void request_print( vm *v, const char *data );

#endif

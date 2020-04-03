#include <value.h>
#include "context.h"

#define	SESSION_CYCLE		100				// every 100 requests
#define SESSION_TIMEOUT		60 * 60 * 5		// 5 hours

#ifdef _WIN32
long _ftol( double f );
long _ftol2( double f) { return _ftol(f); };
#endif

static void send_headers( mcontext *c ) {
	if( !c->headers_sent ) {
		ap_send_http_header(c->r);
		c->headers_sent = true;
	}
}

void request_print( vm *v, const char *data ) {
	mcontext *c = CONTEXT();
	send_headers(c);
	if( c->allow_write )
		ap_rputs(data,c->r);
}

extern void file_gc_run( int timeout );
extern vm *load_mtx( vmparams *p, const char *file );
extern void *dll_primitive_resolve( vm *vm, const char *name, int nargs );

static int request_count = 0;

#ifdef _WIN32
extern int gettimeofday( struct timeval *t, struct timezone *tz );
#endif

static int mts_handler(request_rec *r) {
	mcontext ctx;
	vm *v;
	vmparams p;
	struct timeval time;
	VM_ERRORCODE error;

    r->content_type = "text/html";
	gettimeofday(&time,NULL);
	srand(time.tv_sec * 1000000 + time.tv_usec);

	ctx.r = r;
	ctx.post_data = NULL;
	ctx.allow_write = true;
	ctx.headers_sent = false;

	p.custom = &ctx;
	p.loader = NULL;
	p.resolve = dll_primitive_resolve;
	p.printer = request_print;

	v = load_mtx(&p,r->filename);

	if( v == NULL )
		return DECLINED;

	if( v == (vm*)-1 ) {
		ap_send_http_header(r);
		ap_rprintf(r,"<html><body><b>Error</b> : failed to read file %s</body></html>",r->uri);		
		return OK;
	}

	// in case we're getting a cached VM
	vm_params(v)->custom = &ctx;

	if( vm_error(v) ) {
		send_headers(&ctx);
		ap_rprintf(r,"<html><body><b>Error</b> : invalid file format %s (#%d)</body></html>",r->uri,vm_error(v));
		vm_free(v);
		return OK;
	}

	if( ap_setup_client_block(r,REQUEST_CHUNKED_ERROR) != 0 ) {
		send_headers(&ctx);
		ap_rprintf(r,"<b>Error</b> : ap_setup_client_block failed");
		return OK;
	}

	ap_soft_timeout("mtx timeout",r);

	if( ap_should_client_block(r) ) {
#		define MAXLEN 1024
		char buf[MAXLEN];
		int len;
		stringbuf b = alloc_stringbuf(NULL);
		while( (len = ap_get_client_block(r,buf,MAXLEN)) > 0 )
			stringbuf_append_sub(b,buf,len);
		ctx.post_data = stringbuf_to_string(b);
	}

	if( vm_main(v) != NULL ) {
		vm_select(v);
		val_call0(vm_main(v));
	} else {
		vm_execute(v);
		if( vm_main(v) == NULL )
			vm_reset(v);
	}

	send_headers(&ctx);
	ap_kill_timeout(r);
	if( ((request_count++)%SESSION_CYCLE) == 0 )
		file_gc_run(SESSION_TIMEOUT);

	error = vm_error(v);
	vm_reset_error(v);
	if( error != NOERROR && error != USER_ABORT ) {
		ap_rprintf(r,"<b>Error</b> : execution aborted %s (#%d)",r->uri,error);
		return OK;
	}
	ctx.post_data = NULL;
	v = NULL;
	vm_gc_major();
    return OK;
}

static void mts_init(server_rec *s, pool *p) {
	vm_global_init();
}

static const handler_rec mts_handlers[] = {
    {"mts-handler", mts_handler},
    {NULL}
};

module MODULE_VAR_EXPORT mts_module = {
    STANDARD_MODULE_STUFF,
    mts_init,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    mts_handlers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

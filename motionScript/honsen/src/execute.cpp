/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <api.h>
#undef EXTERN
#include "unzip.h"
#include "execute.h"
#include "window.h"
#include "timer.h"
#include "resources.h"
#include "controls.h"
#include "m3d/m3d.h"
#include "../common/mtw.h"
#include "../common/wmap.h"
/* ------------------------------------------------------------------------ */

struct _hscontext {
	MTW *mtw;
	window *hwnd;
	window *logwnd;
	m3d *m3d;
	timer *timer;
	vm *vm;
	resources *res;
	controls *ctrl;
	HONSEN_ERROR error;
	int width;
	int height;
	unsigned int bgcolor;
	double last_time;
	double fps_delta;
	bool extern_wnd;
};

/* ------------------------------------------------------------------------ */

static value honsen_close_log() {	
	hscontext *c = (hscontext*)vm_params(vm_current())->custom;
	if( c->logwnd != NULL )
		window_log_set(c->logwnd,"");
	return val_true;
}

DEFINE_PRIM(honsen_close_log,0);

/* ------------------------------------------------------------------------ */

static void print( vm *v, const char *text ) {
	hscontext *c = (hscontext*)vm_params(v)->custom;
	if( c->logwnd == NULL )
		c->logwnd = window_create_log();
	window_log_text(c->logwnd,text);
	window_log_text(c->logwnd,"\n");
}

/* ------------------------------------------------------------------------ */

static value resource_loader( vm *v, unsigned int id ) {
	hscontext *c = (hscontext*)vm_params(v)->custom;
	MTW *t = res_find(c->res,id);
	if( t == NULL )
		return NULLVAL;
	return alloc_resource(t);
}

/* ------------------------------------------------------------------------ */

EXTERN void honsen_init() {
	window_init();
	vm_global_init();
}

/* ------------------------------------------------------------------------ */

EXTERN void honsen_exit() {
	vm_global_free();
	dll_primitive_free();
}

/* ------------------------------------------------------------------------ */

EXTERN hscontext *honsen_context( MTW *mtw, window *wnd ) {
	hscontext *ctx = new hscontext;
	memset(ctx,0,sizeof(hscontext));
	if( mtw == NULL ) {
		ctx->error = EFILE_NOT_FOUND;
		return ctx;
	}
	mtw = unzip_mtw(mtw);
	ctx->mtw = mtw;
	if( mtw == NULL ) {
		ctx->error = EUNZIP_FAILED;
		return ctx;
	}
	if( mtw->tag != MAKETAG(Hsen) ) {
		ctx->error = EINVALID_FILE;
		return ctx;
	}
	MTW *head = mtw->Child(MAKETAG(Head));
	if( head == NULL ) {
		ctx->error = EINVALID_FILE;
		return ctx;
	}
	MTW *hs_width = head->Child(MAKETAG(wdth));
	MTW *hs_height = head->Child(MAKETAG(hght));
	MTW *fps = head->Child(MAKETAG(nfps));
	MTW *bgcolor = head->Child(MAKETAG(bgcl));
	if( hs_width == NULL || hs_height == NULL || fps == NULL || bgcolor == NULL ) {
		ctx->error = EINVALID_HEADER;
		return ctx;
	}

	ctx->width = hs_width->number;
	ctx->height = hs_height->number;
	ctx->bgcolor = bgcolor->number;
	if( ctx->width < 0 || ctx->width > 4096 || ctx->height > 4096 || ctx->height < 0 || fps->number > 256 || fps->number == 0 ) {
		ctx->error = EINVALID_HEADER;
		return ctx;
	}
	
	MTW *code = mtw->Child(MAKETAG(CODE));
	if( code == NULL ) {
		ctx->error = EINVALID_FILE;
		return ctx;
	}

	vmparams vmp;
	vmp.loader = resource_loader;
	vmp.printer = print;
	vmp.resolve = dll_primitive_resolve;
	vmp.custom = ctx;

	ctx->vm = vm_alloc(&vmp);
	if( ctx->vm == NULL ) {
		ctx->error = ECANNOT_CREATE_VM;
		return ctx;
	}

	ctx->res = res_init(ctx->mtw);
	vm_init(ctx->vm,code->data,code->size);
	if( vm_error(ctx->vm) ) {
		ctx->error = EVM(vm_error(ctx->vm));
		return ctx;
	}

	ctx->hwnd = wnd?wnd:window_create("Honsen",ctx->width,ctx->height);
	ctx->extern_wnd = (wnd != NULL);
	ctx->m3d = m3d_init(ctx->hwnd,ctx->width,ctx->height,ctx->bgcolor);
	ctx->timer = timer_init();
	ctx->fps_delta = 1.0 / fps->number;
	m3d_set_resources(ctx->m3d,ctx->res);
	ctx->ctrl = controls_init();
	if( !ctx->extern_wnd )
		honsen_set_controls(ctx,ctx->hwnd,true);
	return ctx;
}

/* ------------------------------------------------------------------------ */

EXTERN void honsen_context_init( hscontext *ctx ) {
	if( ctx->error )
		return;
	vm_execute(ctx->vm);
	ctx->last_time = ctx->timer->curtime;
	if( vm_error(ctx->vm) )
		ctx->error = EVM(vm_error(ctx->vm));
}

/* ------------------------------------------------------------------------ */

EXTERN void honsen_context_abort( hscontext *ctx ) {
	if( ctx->error )
		return;
	vm_abort(ctx->vm);
	ctx->error = EABORTED;
}

/* ------------------------------------------------------------------------ */

EXTERN HONSEN_ERROR honsen_error( hscontext *ctx ) {
	return ctx->error;
}

/* ------------------------------------------------------------------------ */

EXTERN void honsen_context_loop( hscontext *ctx ) {
	bool render = false;
	int skip = 0;
	timer_update(ctx->timer,0.95);
	while( ctx->timer->curtime - ctx->last_time > ctx->fps_delta && skip++ < 10 ) {
		ctx->last_time += ctx->fps_delta;
		controls_refresh(ctx->ctrl,(int)(ctx->fps_delta*1000));
		m3d_nextframe(ctx->m3d);
		value m = vm_main(ctx->vm);
		if( m != NULLVAL )
			val_call0(m);
		render = true;
	}
	if( render && ctx->m3d != NULL )
		m3d_render(ctx->m3d);
	int sleep = (int)((ctx->last_time+ctx->fps_delta-ctx->timer->curtime)*1000);
	timer_sleep(ctx->timer,sleep/2);
	if( !ctx->extern_wnd )
		window_set_title(ctx->hwnd,wprintf("Honsen %d @%.1f",skip,ctx->timer->fps).c_str());
	if( vm_error(ctx->vm) )
		ctx->error = EVM(vm_error(ctx->vm));
	vm_gc_loop();
	Cache::GarbageAll();
}

/* ------------------------------------------------------------------------ */

EXTERN void honsen_render_enable( hscontext *ctx, bool e ) {
	m3d_render_enable(ctx->m3d,e);
}

/* ------------------------------------------------------------------------ */

EXTERN void honsen_event( hscontext *ctx, int event, unsigned int param ) {
	controls_event(ctx->ctrl,(CONTROLEVENT)event,param);
}

/* ------------------------------------------------------------------------ */

EXTERN void honsen_set_controls( hscontext *ctx, window *wnd, bool enable ) {
	window_set_controls(wnd,enable?ctx->ctrl:NULL);
}

/* ------------------------------------------------------------------------ */

EXTERN void honsen_context_free( hscontext *ctx ) {
	if( ctx->mtw )
		delete ctx->mtw;
	if( ctx->m3d )
		m3d_clear(ctx->m3d);
	if( ctx->vm )
		vm_free(ctx->vm);
	vm_gc_major();
	if( ctx->res )
		Cache::GarbageAll();
	if( ctx->m3d )
		m3d_destroy(ctx->m3d);
	if( ctx->res )
		res_destroy(ctx->res);
	if( ctx->timer )
		timer_close(ctx->timer);
	if( ctx->hwnd && ctx->ctrl && !ctx->extern_wnd )
		honsen_set_controls(ctx,ctx->hwnd,false);
	if( ctx->hwnd && !ctx->extern_wnd )
		window_destroy(ctx->hwnd);
	if( ctx->logwnd )
		window_destroy(ctx->logwnd);
	if( ctx->ctrl )
		controls_destroy(ctx->ctrl);
	delete ctx;
}

/* ------------------------------------------------------------------------ */

EXTERN void honsen_execute( const char *file ) {
	MTW *mtw = MTW::ReadFile(file);
	hscontext *ctx = honsen_context(mtw,NULL);
	honsen_context_init(ctx);
	if( ctx->error == EFILE_NOT_FOUND ) {
		FILE *f = fopen(file,"rb");
		if( f != NULL ) {
			ctx->error = EINVALID_FILE;
			fclose(f);
		}
	}

	while( !honsen_error(ctx) && window_loop() )
		honsen_context_loop(ctx);

	HONSEN_ERROR r = honsen_error(ctx);
	switch( r ) {
	case EOK:
		break;
	case EFILE_NOT_FOUND:
		window_error(("File not found : "+(WString)file).c_str());
		break;
	case EINVALID_HEADER:
		window_error(("Invalid header : "+(WString)file).c_str());
		break;
	case EINVALID_FILE:
		window_error(("Invalid file : "+(WString)file).c_str());
		break;
	case EUNZIP_FAILED:
		window_error(("Decompession failed : "+(WString)file).c_str());
		break;
	default:
		if( r > EVM(0) )
			window_error(("VM error #"+itos(r >> 16)).c_str());
		else
			window_error(("Unknown error : "+itos(r)).c_str());
		break;
	}
	honsen_context_free(ctx);
	delete mtw;
}

/* ************************************************************************ */

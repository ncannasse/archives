/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#ifdef _WIN32
#	ifdef HONSEN_DLL_EXPORTS
#		define EXTERN	__declspec( dllexport )
#	else
#		define EXTERN	__declspec( dllimport )
#	endif
#else
#	define EXTERN
#endif

typedef enum {
	EOK,
	EFILE_NOT_FOUND,
	EINVALID_HEADER,
	EINVALID_FILE,
	ECANNOT_CREATE_VM,
	EUNZIP_FAILED,
	EABORTED,
} HONSEN_ERROR;

#define EVM(e)	((HONSEN_ERROR)((e) << 16))

class MTW;
typedef struct _window window;
typedef struct _hscontext hscontext;

EXTERN void honsen_init();
EXTERN void honsen_exit();
EXTERN void honsen_execute( const char *file );

EXTERN hscontext *honsen_context( MTW *mtw, window *wnd );
EXTERN void honsen_context_init( hscontext *ctx );
EXTERN void honsen_context_abort( hscontext *ctx );
EXTERN HONSEN_ERROR honsen_error( hscontext *ctx );
EXTERN void honsen_set_controls( hscontext *ctx, window *wnd, bool enable );
EXTERN void honsen_event( hscontext *ctx, int event, unsigned int param );
EXTERN void honsen_context_free( hscontext *ctx );
EXTERN void honsen_context_loop( hscontext *ctx );
EXTERN void honsen_render_enable( hscontext *ctx, bool e );

/* ************************************************************************ */

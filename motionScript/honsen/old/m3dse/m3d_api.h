/* ************************************************************************ */
/*																			*/
/*	M3D-SE																	*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#undef EXTERN
#ifdef M3DSE_EXPORTS
#	define EXTERN	__declspec(dllexport) extern
#else
#	define EXTERN	__declspec(dllimport)
#endif

class MTW;

namespace M3D {

	struct Context;

	struct InitData {
		MTW *root;
		HWND target;
		int width;
		int height;
		DWORD bgcolor;
		int (*get_config)( const char * );
		void (*debug_out)( const char * );
	};

	EXTERN Context *Init( InitData *data );
	EXTERN void ShareContext( Context *c );
	EXTERN void Close( Context *c );
	EXTERN void Render( Context *c );
	EXTERN void ClearDisplay( Context *c );
	EXTERN void SetDebugOutput( Context *c, void (*callback)( const char * ) );

	namespace Resource {
		EXTERN MTW *Find( Context *c, const char *name );
		EXTERN MTW *Find( Context *c, unsigned int rid );
	};

};

/* ************************************************************************ */

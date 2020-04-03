/* ************************************************************************ */
/*																			*/
/*	Honsen Lib																*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#undef EXTERN
#ifdef HSLIB_EXPORTS
#	define EXTERN	__declspec(dllexport) extern
#else
#	define EXTERN	__declspec(dllimport)
#endif

#define MOUSE_BLEFT		(0xFF + 1)
#define MOUSE_BRIGHT	(0xFF + 2)
#define MOUSE_BMIDDLE	(0xFF + 3)

#define MOUSE_SCAN_BLEFT	(0xFFFF - 0)
#define MOUSE_SCAN_BRIGHT	(0xFFFF - 1)
#define MOUSE_SCAN_BMIDDLE	(0xFFFF - 2)

#define MAX_KEY 259
#define INVALID_KEY	MAX_KEY

typedef float FLOAT;
typedef unsigned long DWORD;

struct VECTOR {
	FLOAT x,y,z;
};

#ifndef NULL
#	define	NULL 0
#endif

namespace HSLib {

	EXTERN void InitContext();
	EXTERN bool Init( HWND target );
	EXTERN void Close(); 
	EXTERN void Refresh( bool process ); 
	EXTERN void SetDebugOutput( void (*callback)( const char * ) );
	EXTERN void SetConfig( int (*get_config)( const char * ) );
	EXTERN void EmitEvent( unsigned int msg, unsigned int wp, unsigned int lp );
	EXTERN void SetProgress(double progress);
	EXTERN bool Compress( int level, void *output, int *outlen, VECTOR *input, int inlen );
	EXTERN int UncompressHeader( void *input, int inlen );
	EXTERN bool Uncompress( VECTOR *output, int *outlen, void *input, int inlen );
	EXTERN bool Unzip( void *output, int *outlen, void *input, int inlen );

	struct LibData {
		char *keynames[MAX_KEY];
		int keys[MAX_KEY];
		int scancodes[MAX_KEY];
		int krepeat[MAX_KEY];
		int mouse_x;
		int mouse_y;
		double progress;
	};

	EXTERN LibData *GetData();
};

/* ************************************************************************ */

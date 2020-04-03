/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <windows.h>
#include <time.h>
#include "../common/wmem.h"

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )  {
	if( fdwReason == DLL_PROCESS_ATTACH )
		WMem::Init(false);
	return TRUE;
}

/* ************************************************************************ */

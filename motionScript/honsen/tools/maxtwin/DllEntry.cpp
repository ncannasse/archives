/* ************************************************************************ */
/*																			*/
/*	MaxTwin																	*/
/*	Morgan Bachelier, Nicolas Cannasse										*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "maxtwin.h"
#include <iparamb2.h>

/* ------------------------------------------------------------------------ */
#define MAXTWIN_CLASS_ID	Class_ID(0x4f0dcbbb, 0xa64c256)

HINSTANCE hInstance = NULL;


//*****************************************************************************
// Description class
//*****************************************************************************

class MaxtwinClassDesc : public ClassDesc2
{

public:

    void *			Create(BOOL loading=FALSE) { return new Maxtwin(); }
	int 			IsPublic()      { return TRUE; }
	const TCHAR*	ClassName()     { return _T("Maxtwin"); }
	SClass_ID		SuperClassID()  { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID()       { return MAXTWIN_CLASS_ID; }
	const TCHAR* 	Category()      { return _T("Exporter"); }
	const TCHAR*	InternalName()  { return _T("Maxtwin"); }	    // returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance()     { return hInstance; }			// returns owning module handle
};


static MaxtwinClassDesc MaxtwinDesc;


//*****************************************************************************
// Global Functions needed by 3dsmax plug-ins
//*****************************************************************************

//-----------------------------------------------------------------------------
// DllMain
//
BOOL WINAPI
DllMain(HINSTANCE hinstDLL, ULONG fdwReason,LPVOID lpvReserved)
{
	hInstance = hinstDLL;				// Hang on to this DLL's instance handle.
	return (TRUE);
}


//-----------------------------------------------------------------------------
// Description (appears if the plug-in is unavailable)
//
__declspec( dllexport ) const TCHAR* LibDescription() { 
    return _T("Motion-Twin 3D Exporter - contact@motion-twin.com"); 
}


//-----------------------------------------------------------------------------
// Number of plug-in classes in DLL
// TODO: Must change this number when adding a new plug-in class
//
__declspec( dllexport ) int LibNumberClasses() { 
    return 1; 
}


//-----------------------------------------------------------------------------
// Version of max sdk used
//
__declspec( dllexport ) ULONG LibVersion() { 
    return VERSION_3DSMAX; 
}


//-----------------------------------------------------------------------------
// Return a description class of the plug-in
//
__declspec( dllexport ) ClassDesc* LibClassDesc(int i) {
	switch(i) {
		case 0: return &MaxtwinDesc;
		default: return 0;
	}
}

/* ************************************************************************ */


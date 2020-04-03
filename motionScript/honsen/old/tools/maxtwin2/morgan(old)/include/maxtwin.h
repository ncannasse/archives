//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : maxtwin.h
//*     Author  : M.B.
//*     Desc    : exporter main class definition
//*     Date    : 19.09.03
//*
//*****************************************************************************

#ifndef __MAXTWIN_H__
#define __MAXTWIN_H__

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <max.h>

#include "../../common/mtw.h"   // MTW, TAG

class Maxtwin;

#include "include/mtw_obj.h"        // Obj
#include "include/mtw_log.h"        // Log
#include "include/mtw_error.h"      // Errors


//*****************************************************************************
//
class Maxtwin : public SceneExport
{

public:

    //-------------------------------------------------------------------------
    // Description and Message Functions
    //-------------------------------------------------------------------------
	int				ExtCount()         { return 1; }					        // Number of extensions supported
	const TCHAR*	Ext(int n)         { return _T("mtw"); }					// Extension #n (i.e. "3DS")
	const TCHAR*	LongDesc()         { return _T("Motion-Twin 3D File"); }	// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR*	ShortDesc()        { return _T("Motion-Twin 3D"); }         // Short ASCII description (i.e. "3D Studio"
	const TCHAR*	AuthorName()       { return _T(""); }				        // ASCII Author name
	const TCHAR*	CopyrightMessage() { return _T("(c)2002 Motion-Twin"); }	// ASCII Copyright message
	const TCHAR*	OtherMessage1()    { return _T("http://www.motion-twin.com"); }	// Other message #1
	const TCHAR*	OtherMessage2()    { return _T(""); }			            // Other message #2
	unsigned int	Version()          { return 200; };					        // Version number * 100 (i.e. v3.01 = 301)
    void			ShowAbout(HWND hWnd) { }	                                // Show DLL's "About..." box (no About box)
    BOOL            SupportsOptions(int ext, DWORD options) { return TRUE; }    // Do not support "export selected"
    
    int	            DoExport(const TCHAR *name, ExpInterface *ei, Interface *i,
                             BOOL suppressPrompts=FALSE, DWORD options=0);

    //-------------------------------------------------------------------------
	// Constructor & Destructor
	//-------------------------------------------------------------------------
	Maxtwin();
	~Maxtwin();

    //-------------------------------------------------------------------------
	// Utils
	//-------------------------------------------------------------------------
    static MATRIX  MaxToD3DMatrix(Matrix3* m);
    static int     GetNFrames();

    static TimeValue startTime, endTime;

private:

    int     EnumNode(MTW* parentTag, INode* node);
    bool    IsGeomObject(INode* node) const; 
    void    SetLog(int res);

    MTW*    rootTag;
    Obj*    obj;                // current object being exported

    Errors  errors;
    Log     log;

};


#endif // __MAXTWIN_H__


/* ************************************************************************ */
/*																			*/
/*	MaxTwin																	*/
/*	Morgan Bachelier, Nicolas Cannasse										*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#ifndef __MAXTWIN_H__
#define __MAXTWIN_H__

/* ************************************************************************ */
#include <max.h>
#include "../../common/wstring.h"

class MTW;

/* ************************************************************************ */

class Maxtwin : public SceneExport
{

public:

	//-------------------------------------------------------------------------
	// Description and Message Functions
	//-------------------------------------------------------------------------
	int 			ExtCount()		   { return 1; }							// Number of extensions supported
	const TCHAR*	Ext(int n)		   { return _T("mtw"); }					// Extension #n (i.e. "3DS")
	const TCHAR*	LongDesc()		   { return _T("Motion-Twin 3D File"); }	// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR*	ShortDesc() 	   { return _T("Motion-Twin 3D"); } 		// Short ASCII description (i.e. "3D Studio"
	const TCHAR*	AuthorName()	   { return _T(""); }						// ASCII Author name
	const TCHAR*	CopyrightMessage() { return _T("(c)2003 Motion-Twin"); }	// ASCII Copyright message
	const TCHAR*	OtherMessage1()    { return _T("http://www.motion-twin.com"); } // Other message #1
	const TCHAR*	OtherMessage2()    { return _T(""); }						// Other message #2
	unsigned int	Version()		   { return 210; }; 						// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout(HWND hWnd) { }									// Show DLL's "About..." box (no About box)
	BOOL			SupportsOptions(int ext, DWORD options) { return TRUE; }	// Do not support "export selected"
	
	int 			DoExport(const TCHAR *name, ExpInterface *ei, Interface *i,
							 BOOL suppressPrompts=FALSE, DWORD options=0);

	//-------------------------------------------------------------------------
	// Constructor & Destructor
	//-------------------------------------------------------------------------
	Maxtwin();
	~Maxtwin();

private:

	MTW *root;
	WString log_text;
	void Log( WString str );
	bool EnumNode( MTW *parent, INode *node );
	MTW *CreateMTW( INode *node );
	MTW *FindMTW( const char *name, MTW **parent );
	bool IsGeomObject( INode *node );
};


#endif // __MAXTWIN_H__

/* ************************************************************************ */

/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <windows.h>
#include "../common/wmem.h"
#include "execute.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WMem::Init(false);
	honsen_init();
	if( !*lpCmdLine ) {
		char curdir[256];
		char filename[256];
		OPENFILENAME opfile;
		*filename = 0;
		memset(&opfile,0,sizeof(opfile));
		GetCurrentDirectory(256,curdir);
		opfile.lStructSize = sizeof(opfile);
		opfile.lpstrFilter = "Honsen File\0*.HS\0\0\0";
		opfile.lpstrFile = filename;
		opfile.nMaxFile = 256;
		opfile.lpstrInitialDir = curdir;
		opfile.lpstrTitle = "Open Honsen file...";
		opfile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
		if( GetOpenFileName(&opfile) )
			honsen_execute(filename);
	}
	else {
		if( *lpCmdLine == '"' ) {
			lpCmdLine++;
			*strchr(lpCmdLine,'"') = 0;
		}
		honsen_execute(lpCmdLine);
	}
	honsen_exit();
	return 0;
}

/* ************************************************************************ */

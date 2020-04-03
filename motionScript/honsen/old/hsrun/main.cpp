/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <stdio.h>
#include <windows.h>
#include "../common/wmem.h"
#include "../common/mtw.h"
#include "hsfile.h"
#include "hsconfig.h"


#include "../std/value.h"
C_FUNCTION_BEGIN

EXTERN void gc_init( int (*get_config)(const char *) );

C_FUNCTION_END

DWORD WINAPI RunThread(LPVOID filename) {
	if( !open_hs_file(MTW::ReadFile((char *)filename),(char*)filename, NULL, NULL, NULL) ) {
		MessageBox(NULL,get_error_msg(),"Error :",MB_OK | MB_ICONERROR );
		free_error_msg();
		return -1;
	}	
	return 0;
}

int OpenHS( int nthreads, const char *filename ) {
	if( *filename == '"' ) {
		filename++;
		*strchr(filename,'"') = 0;
	}
	if( GetFileAttributes(filename) == 0xFFFFFFFF ) {
		MessageBox(NULL,filename,"File not found :",MB_OK | MB_ICONERROR );
		return -2;
	}
	if( nthreads <= 0 )
		nthreads = 1;
	HANDLE *h = new HANDLE[nthreads];
	int i;
	for(i=0;i<nthreads;i++) {
		DWORD tid;
		h[i] = CreateThread(NULL,0,RunThread,(void*)filename,0,&tid);		
	}
	WaitForMultipleObjects(nthreads,h,TRUE,INFINITE);
	delete h;
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int r = 0;
	char path[256];
	char *fname_pos;
	WMem::Init(false);
	GetModuleFileName(NULL,path,246);
	fname_pos = strrchr(path,'\\');
	if( fname_pos )
		strcpy(fname_pos+1,"config");
	else
		strcpy(path,"config");
	read_config(path);
	gc_init(get_config);
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
			r = OpenHS(get_config("NTHREADS"),filename);
	}
	else
		r = OpenHS(get_config("NTHREADS"),lpCmdLine);
	free_config();
	return r;
}

/* ************************************************************************ */

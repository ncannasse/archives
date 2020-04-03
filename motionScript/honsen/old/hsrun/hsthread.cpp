#include <windows.h>
#include "hsthread.h"
#include "hsconfig.h"
#include "hsfile.h"
#include "../common/mtw.h"
#include "../std/interp.h"

C_FUNCTION_BEGIN

EXTERN void gc_init( int (*get_config)(const char *) );

C_FUNCTION_END

struct ThreadParams {
	Params *params;
	HANDLE lock;
};

bool honsen_global_init() {
	char path[256];
	char *fname_pos;
	GetModuleFileName(NULL,path,246);
	fname_pos = strrchr(path,'\\');
	if( fname_pos )
		strcpy(fname_pos+1,"config");
	else
		strcpy(path,"config");
	read_config(path);
	gc_init(get_config);
	return true;
}

void honsen_global_free() {
	free_config();
}

DWORD ThreadFunction(LPVOID pParam)
{
	ThreadParams *params = (ThreadParams *)pParam;
	Params *p = params->params;
	MSG m;

	PeekMessage(&m,NULL,WM_USER,WM_USER,PM_NOREMOVE);
	ReleaseSemaphore(params->lock,1,NULL);
	params = NULL;

	if( p->hs == NULL ) {
		HMODULE h = LoadLibrary("m3dse.dll");
		char path[256];
		*path = 0;
		GetModuleFileName(h,path,256-9);
		FreeLibrary(h);
		strcpy(path+strlen(path)-9,"loading.hs");
		if( !open_hs_file(MTW::ReadFile(path),path,p->hwnd,p->callback,p->cbparam) ) {
			MessageBox(NULL, get_error_msg(), "Error:", MB_OK | MB_ICONERROR);
			free_error_msg();
		}
	}
	
	if( p->hs != NULL && !open_hs_file(p->hs,p->filename,p->hwnd,p->callback,p->cbparam) ) {
		MessageBox(NULL, get_error_msg(), "Error:", MB_OK | MB_ICONERROR);
		free_error_msg();
	}

	return 0;
}

DWORD honsen_start_thread( Params *params ) {
	DWORD threadID;
	ThreadParams p;
	p.params = params;
	p.lock = CreateSemaphore(NULL,0,1,NULL);
	HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadFunction, &p, 0, &threadID);
	WaitForSingleObject(p.lock,INFINITE);
	CloseHandle(h);
	return threadID;
}

void honsen_stop_thread(DWORD threadID) {
	HANDLE h = OpenThread(THREAD_ALL_ACCESS, true, threadID);
	PostThreadMessage( threadID, WM_DESTROY, 0, 0 );
	if( h ) {
		WaitForSingleObject(h,INFINITE);
		CloseHandle(h);
	}
}
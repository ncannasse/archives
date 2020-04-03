/* ************************************************************************ */
/*																			*/
/*	Honsen Mozilla Plugin													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <windows.h>
#include <stdio.h>
#include "../execute.h"
#include "../../common/mtw.h"
#include "implementation.h"

#define WM_LOAD_COMPLETE (WM_USER + 1)
#define WM_PARENT_RESIZE (WM_USER + 2)

struct _window {
	HWND h;
};

/* ------------------------------------------------------------------------ */

NPError NS_PluginInitialize() {
	honsen_init();
	return NPERR_NO_ERROR;
}

/* ------------------------------------------------------------------------ */

void NS_PluginShutdown() {
	honsen_exit();
}

/* ------------------------------------------------------------------------ */

nsPluginInstanceBase * NS_NewPluginInstance(nsPluginCreateData *cs) {
	if( cs == NULL )
		return NULL;
	int i;
	const char *url = NULL;
	for(i=0;i<cs->argc;i++) {
		if( strcmpi(cs->argn[i],"url") == 0 ) {
			url = cs->argv[i];
			break;
		}
	}
	if( url == NULL )
		return NULL;
	return new honsenPlugin(cs->instance,url);
}

/* ------------------------------------------------------------------------ */

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin) {
	if( aPlugin )
		delete (honsenPlugin*)aPlugin;
}

/* ------------------------------------------------------------------------ */

honsenPlugin::honsenPlugin(NPP aInstance, const char *file) : nsPluginInstanceBase(), inst(aInstance), initflag(false) {
	wnd = NULL;
	buf = NULL;
	ctx = NULL;
	bufsize = 0;
	bufpos = 0;
	url = strdup(file);
}

/* ------------------------------------------------------------------------ */

honsenPlugin::~honsenPlugin() {
	CleanBuffer();
	free(url);
}

/* ------------------------------------------------------------------------ */

static DWORD CALLBACK ThreadFunction(LPVOID pParam) {
	honsenPlugin *i = (honsenPlugin*)pParam;
	MSG m;
	RECT r;
	_window wnd;
	PeekMessage(&m,NULL,WM_USER,WM_USER,PM_NOREMOVE);
	ReleaseSemaphore(i->lock,1,NULL);
	wnd.h = NULL;
	while( true ) {
		while( PeekMessage(&m,NULL,0,0,PM_REMOVE) ) {
			switch( m.message ) {
			case WM_LOAD_COMPLETE:
				GetClientRect(i->wnd,&r);
				SetWindowLong(i->wnd,GWL_STYLE,GetWindowLong(i->wnd,GWL_STYLE)|WS_CLIPCHILDREN);
				wnd.h = CreateWindow("honsen window","",WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,0,0,r.right,r.bottom,i->wnd,NULL,NULL,NULL);
				i->lasttime = GetTickCount();
				i->ctx = honsen_context(i->mtw,&wnd);
				honsen_set_controls(i->ctx,&wnd,true);
				honsen_context_init(i->ctx);
				break;
			case WM_DESTROY:
				goto end;
			case WM_PARENT_RESIZE:
				GetClientRect(i->wnd,&r);
				MoveWindow(wnd.h,0,0,r.right,r.bottom,TRUE);
				if( i->ctx != NULL )
					goto refresh;
				break;
			default:
				DispatchMessage(&m);
				break;
			}
		}
refresh:
		if( i->ctx != NULL )
			honsen_context_loop(i->ctx);
		else
			Sleep(10);
		if( i->lasttime != -1 )
			i->lasttime = GetTickCount();
	}
end:
	if( i->ctx != NULL ) {
		honsen_context_free(i->ctx);
		i->ctx = NULL;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

static DWORD CALLBACK WaitFunction(LPVOID pParam) {
	honsenPlugin *i = (honsenPlugin*)pParam;
	while(true) {
		i->loop();
		Sleep(500);
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

NPBool honsenPlugin::init(NPWindow* aWindow) {
	if( aWindow == NULL )
		return FALSE;
	wnd = (HWND)aWindow->window;
	if( wnd == NULL )
		return FALSE;
	initflag = true;
	lock = CreateSemaphore(NULL,0,1,NULL);
	thread = CreateThread(0, 0, ThreadFunction, this, 0, &tid);
	WaitForSingleObject(lock,INFINITE);
	CloseHandle(lock);

	DWORD wid;
	wait = CreateThread(0,0, WaitFunction, this, 0, &wid );

	NPN_GetURL(inst,url,NULL);
	return true;
}

/* ------------------------------------------------------------------------ */

void honsenPlugin::shut() {	
	TerminateThread(wait,0);
	PostThreadMessage(tid,WM_DESTROY,0,0);
	WaitForSingleObject(thread,INFINITE);
	delete mtw;
	mtw = NULL;
	wnd = NULL;
	lasttime = -1;
	initflag = false;
}

/* ------------------------------------------------------------------------ */

NPBool honsenPlugin::isInitialized() {
	return initflag;
}

/* ------------------------------------------------------------------------ */

NPError honsenPlugin::DestroyStream(NPStream* stream, NPReason  reason) {
	if( reason == NPRES_DONE ) {
		mtw = MTW::ReadData(buf,bufpos);
		if( mtw != NULL )
			PostThreadMessage(tid,WM_LOAD_COMPLETE,0,0);
	}
	CleanBuffer();
	return NPERR_NO_ERROR;
}

/* ------------------------------------------------------------------------ */

void honsenPlugin::CleanBuffer() {
	delete buf;
	buf = NULL;
	bufsize = 0;
	bufpos = 0;
}

/* ------------------------------------------------------------------------ */

NPError honsenPlugin::SetWindow(NPWindow* w) {	
	PostThreadMessage(tid,WM_PARENT_RESIZE,0,0);
	return NPERR_NO_ERROR;
}

/* ------------------------------------------------------------------------ */

void honsenPlugin::loop() {
	if( lasttime != -1 && GetTickCount() - lasttime > 3000 ) {		
		lasttime = -1;		
		SuspendThread(thread);
		if( MessageBox(NULL,"The Honsen Program is too slow, stop it ?","Warning",MB_YESNO | MB_ICONWARNING | MB_SYSTEMMODAL) == IDYES )
			honsen_context_abort(ctx);
		else
			lasttime = GetTickCount();
		ResumeThread(thread);
	}
}

/* ------------------------------------------------------------------------ */

int32 honsenPlugin::Write( NPStream *stream, int32 offset, int32 len, void *buffer ) {
	if( offset != bufpos )
		return -1;
	while( bufsize - bufpos < len ) {
		bufsize = bufsize?(bufsize*2):1024;
		char *nbuf = new char[bufsize];
		memcpy(nbuf,buf,bufpos);
		delete buf;
		buf = nbuf;
	}
	memcpy(buf+bufpos,buffer,len);
	bufpos += len;
	return len;
}

/* ************************************************************************ */

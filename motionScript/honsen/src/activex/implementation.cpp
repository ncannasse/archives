/* ************************************************************************ */
/*																			*/
/*	Honsen ActiveX Plugin													*/
/*	Julien Chevreux, Nicolas Cannasse										*/
/*	(c)2004-2005 Motion-Twin												*/
/*																			*/
/* ************************************************************************ */
#define WINVER 0x0400
#include "HonsenDataPath.h"
#include "implementation.h"
#include "../execute.h"
#include "../controls.h"
#include "../../common/wmem.h"
#include "../../common/mtw.h"

#define WM_LOAD_COMPLETE (WM_USER + 1)
#define WM_CONTROL_EVENT (WM_USER + 2)

struct _window {
	HWND h;
};

struct Impl {
	MTW *mtw;
	hscontext *ctx;
	HonsenDataPath *hdp;
	COleControl *ctrl;
	char *url;
	HWND hwnd;
	HANDLE thread;
	DWORD tid;
	int lasttime;
};

void honsen_plugin_init() {
	int size_path = 32767;
	char *path = new char[size_path];
	char mtpath[256];
	*mtpath = ';';
	GetSystemDirectory(mtpath+1, 256);
	strcpy(mtpath+strlen(mtpath),"\\mt\\honsen");
	GetEnvironmentVariable("PATH", path, size_path - strlen(mtpath) - 1);
	strcpy(path+strlen(path),mtpath);
	SetEnvironmentVariable("PATH",path);
	delete path;
	WMem::Init(false);
	honsen_init();
}

#undef new

void honsen_plugin_close() {
	honsen_exit();
}

DWORD ThreadFunction(LPVOID pParam) {
	Impl *i = (Impl*)pParam;
	MSG m;
	_window wnd;
	PeekMessage(&m,NULL,WM_USER,WM_USER,PM_NOREMOVE);
	ReleaseSemaphore(i->thread,1,NULL);
	while( true ) {
		while( PeekMessage(&m,NULL,0,0,PM_REMOVE) ) {
			switch( m.message ) {
			case WM_LOAD_COMPLETE:
				wnd.h = i->hwnd;
				i->lasttime = GetTickCount();
				i->ctx = honsen_context(i->mtw,&wnd);
				honsen_context_init(i->ctx);
				break;
			case WM_CONTROL_EVENT:
				if( i->ctx != NULL )
					honsen_event(i->ctx,m.wParam,m.lParam);
				break;
			case WM_DESTROY:
				goto end;
			default:
				DispatchMessage(&m);
				break;
			}
		}
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

Impl *implementation_init( COleControl *ctrl, const char *url ) {
	Impl *i = new Impl;
	i->ctrl = ctrl;
	i->ctx = NULL;
	i->url = strdup(url);
	i->lasttime = -1;
	i->thread = CreateSemaphore(NULL,0,1,NULL);
	HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadFunction, i, 0, &i->tid);
	WaitForSingleObject(i->thread,INFINITE);
	CloseHandle(i->thread);
	i->mtw = NULL;
	i->thread = h;
	i->hdp = new HonsenDataPath(ctrl,url);
	i->hdp->Open();
	return i;
}

void implementation_close( Impl *i ) {
	PostThreadMessage(i->tid, WM_DESTROY, 0, 0);
	WaitForSingleObject(i->thread,INFINITE);
	CloseHandle(i->thread);
	delete i->hdp;
	delete i->mtw;
	free(i->url);
	delete i;
}

bool implementation_main( Impl *i, HWND hwnd ) {
	if( i->ctx != NULL && i->lasttime != -1 && GetTickCount() - i->lasttime > 3000 ) {
		i->lasttime = -1;
		SuspendThread(i->thread);
		if( MessageBox(NULL,"The Honsen Program is too slow, stop it ?","Warning",MB_YESNO | MB_ICONWARNING | MB_SYSTEMMODAL) == IDYES )
			honsen_context_abort(i->ctx);
		else
			i->lasttime = GetTickCount();
		ResumeThread(i->thread);
	}
	if( i->mtw == NULL && i->ctrl->GetReadyState() == READYSTATE_COMPLETE ) {
		int length = (int)i->hdp->GetLength();
		char *buf = new char[length];
		i->hdp->Read(buf,length);
		i->hwnd = hwnd;
		i->mtw = MTW::ReadData(buf,length);
		delete buf;
		PostThreadMessage(i->tid,WM_LOAD_COMPLETE,(WPARAM)hwnd,0);
	}
	return (i->mtw == NULL);
}

static void send_event( Impl *i, CONTROLEVENT e, unsigned int param ) {
	PostThreadMessage(i->tid,WM_CONTROL_EVENT,e,param);
}

void implementation_event( Impl *i, unsigned int msg, int wparam, int lparam ) {
	switch( msg ) {
	case WM_KILLFOCUS:
		send_event(i,EVT_LOSTFOCUS,0);
		break;
	case WM_LBUTTONDOWN:
		SetCapture(i->hwnd);
		send_event(i,EVT_KEY_DOWN,MOUSE_BLEFT);
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		send_event(i,EVT_KEY_UP,MOUSE_BLEFT);
		break;
	case WM_RBUTTONDOWN:
		SetCapture(i->hwnd);
		send_event(i,EVT_KEY_DOWN,MOUSE_BRIGHT);
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		send_event(i,EVT_KEY_UP,MOUSE_BRIGHT);
		break;
	case WM_MBUTTONDOWN:
		SetCapture(i->hwnd);
		send_event(i,EVT_KEY_DOWN,MOUSE_BMIDDLE);
		break;
	case WM_MBUTTONUP:
		ReleaseCapture();
		send_event(i,EVT_KEY_UP,MOUSE_BMIDDLE);
		break;
	case WM_MOUSEMOVE:
		send_event(i,EVT_SET_MOUSEPOS,lparam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		if( (lparam & 65535) != 1 ) // repeat flag
			break;
		//_RPTF3(_CRT_WARN,"Event %s 0x%X 0x%X\n",(msg == WM_KEYDOWN)?"down":"up",wparam,lparam);
		switch( (lparam >> 16) & ((1 << 9) - 1) ) {
		case 54:
			wparam = VK_RSHIFT;
			break;
		case 285:
			wparam = VK_RCONTROL;
			break;
		case 312:
			wparam = VK_RMENU;
			break;
		}
		send_event(i,(msg == WM_KEYDOWN)?EVT_KEY_DOWN:EVT_KEY_UP,wparam);
		break;
	}
}

/* ************************************************************************ */

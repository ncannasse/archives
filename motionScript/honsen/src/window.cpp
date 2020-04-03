/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <windows.h>
#include <richedit.h>
#include "../common/wmem.h"

#include "window.h"
#include "controls.h"
#define WND_CLASS_NAME "honsen window"

struct _window {
	HWND h;
};

/* ------------------------------------------------------------------------ */

static LRESULT CALLBACK on_message(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam) {
	switch( iMsg ) {
	case WM_CREATE:
		return TRUE;
	case WM_SIZE:
		{
			HWND eh = (HWND)GetProp(hwnd,"log");
			if( eh != NULL ) {
				RECT r;
				GetClientRect(hwnd,&r);
				SetWindowPos(eh,NULL,0,0,r.right,r.bottom,SWP_NOMOVE | SWP_NOZORDER);
			}
		}
		break;
	case WM_CLOSE:
		{
			HWND eh = (HWND)GetProp(hwnd,"log");
			if( eh != NULL ) {
				SendMessage(eh,WM_SETTEXT,0,(LPARAM)"");
				ShowWindow(hwnd,SW_HIDE);
				return TRUE;
			}
		}
		break;
	case WM_DESTROY:
		{
			HWND eh = (HWND)GetProp(hwnd,"log");
			if( eh == NULL ) {
				PostQuitMessage(0);
				return FALSE;
			}
		}
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

/* ------------------------------------------------------------------------ */

static void send_event( HWND h, CONTROLEVENT evt, unsigned int key ) {
	controls_event((controls*)GetProp(h,"controls"),evt,key);
}

static LRESULT CALLBACK capture_events(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam) {
	WNDPROC old = (WNDPROC)GetProp(hwnd,"oldproc");
	switch( iMsg ) {
	case WM_KILLFOCUS:
		send_event(hwnd,EVT_LOSTFOCUS,0);
		break;
	case WM_LBUTTONDOWN:
		SetFocus(hwnd);
		SetCapture(hwnd);
		send_event(hwnd,EVT_KEY_DOWN,MOUSE_BLEFT);
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		send_event(hwnd,EVT_KEY_UP,MOUSE_BLEFT);
		break;
	case WM_RBUTTONDOWN:
		SetFocus(hwnd);
		SetCapture(hwnd);
		send_event(hwnd,EVT_KEY_DOWN,MOUSE_BRIGHT);
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		send_event(hwnd,EVT_KEY_UP,MOUSE_BRIGHT);
		break;
	case WM_MBUTTONDOWN:
		SetFocus(hwnd);
		SetCapture(hwnd);
		send_event(hwnd,EVT_KEY_DOWN,MOUSE_BMIDDLE);
		break;
	case WM_MBUTTONUP:
		ReleaseCapture();
		send_event(hwnd,EVT_KEY_UP,MOUSE_BMIDDLE);
		break;
	case WM_MOUSEMOVE:
		send_event(hwnd,EVT_SET_MOUSEPOS,lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		if( (lParam & 65535) != 1 ) // repeat flag
			break;
		switch( (lParam >> 16) & ((1 << 9) - 1) ) {
		case 54:
			wParam = VK_RSHIFT;
			break;
		case 285:
			wParam = VK_RCONTROL;
			break;
		case 312:
			wParam = VK_RMENU;
			break;
		}
		send_event(hwnd,(iMsg == WM_KEYDOWN)?EVT_KEY_DOWN:EVT_KEY_UP,wParam);
		break;
	}
	return CallWindowProc(old,hwnd,iMsg,wParam,lParam);
}

/* ------------------------------------------------------------------------ */

window *window_create(const char *title, int width, int height) {
	RECT r;
	GetClientRect(GetDesktopWindow(),&r);
	HWND h = CreateWindow(WND_CLASS_NAME,title,WS_CAPTION | WS_POPUPWINDOW,(r.right-r.left-width)/2,(r.bottom-r.top-height)/2,width,height,NULL,NULL,NULL,NULL);
	ShowWindow(h,SW_NORMAL);
	UpdateWindow(h);
	GetClientRect(h,&r);
	SetWindowPos(h,NULL,0,0,width*2-(r.right - r.left),height*2-(r.bottom - r.top),SWP_NOMOVE | SWP_NOZORDER);
	window *w = new window;
	w->h = h;
	return w;
}

/* ------------------------------------------------------------------------ */

bool window_loop() {
	MSG m;
	int count = 0;
	while( PeekMessage(&m,NULL,0,0,PM_NOREMOVE) ) {
		if( !GetMessage(&m,NULL,0,0) )
			return false;
		TranslateMessage(&m);
		DispatchMessage(&m);
		if( count++ == 10 )
			break;
	}
	return true;
}

/* ------------------------------------------------------------------------ */

void window_init() {
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		on_message,
		0,
		0,
		GetModuleHandle(NULL),
		NULL,
		NULL,
		NULL,
		NULL,
		WND_CLASS_NAME,
		NULL
	};
    RegisterClassEx( &wc );
	LoadLibrary("RICHED32.DLL");
}

/* ------------------------------------------------------------------------ */

void window_destroy( window *w ) {
	DestroyWindow(w->h);
	delete w;
}

/* ------------------------------------------------------------------------ */

window *window_create_log() {
	RECT r;
	HWND h = CreateWindowEx(WS_EX_CONTROLPARENT,WND_CLASS_NAME,"Log :",WS_OVERLAPPEDWINDOW,10,10,400,300,NULL,NULL,NULL,NULL);
	GetClientRect(h,&r);
	HWND eh = CreateWindowEx(WS_EX_CLIENTEDGE,RICHEDIT_CLASS,NULL,WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_READONLY | ES_SAVESEL,0,0,r.right,r.bottom,h,NULL,NULL,NULL);
	HFONT f = CreateFont(-10,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Courier");
	SendMessage(eh,WM_SETFONT,(WPARAM)f,0);
	SendMessage(eh,EM_SETBKGNDCOLOR,0,(WPARAM)RGB(220,220,220));
	SetProp(h,"log",eh);
	ShowWindow(h,SW_SHOWNOACTIVATE);
	UpdateWindow(h);
	UpdateWindow(eh);
	window *w = new window;
	w->h = h;
	return w;
}

/* ------------------------------------------------------------------------ */

void window_log_text( window *w, const char *data ) {
	HWND h = w->h;
	HWND eh = (HWND)GetProp(h,"log");
	if( eh == NULL )
		MessageBox(h,data,"Log:",MB_OK);
	else {
		CHARRANGE old;
		CHARRANGE cur;
		DWORD sel = SendMessage(eh,WM_GETTEXTLENGTH,0,0);
		SendMessage(eh,EM_EXGETSEL,NULL,(LPARAM)&old);
		char *next_line = (char*)strchr(data,'\n');
		while( next_line != NULL ) {
			cur.cpMax = sel;
			cur.cpMin = sel;
			SendMessage(eh,EM_EXSETSEL,NULL,(LPARAM)&cur);
			if( next_line == data ) {
				SendMessage(eh,EM_REPLACESEL,FALSE,(LPARAM)"\r\n");
				data++;
				sel += 2;
				next_line = (char*)strchr(data,'\n');
			} else if( next_line[-1] != '\r' ) {
				*next_line = 0;
				SendMessage(eh,EM_REPLACESEL,FALSE,(LPARAM)data);
				*next_line = '\n';
				sel += next_line - data;
				data = next_line;
				next_line = (char*)strchr(data,'\n');
			} else
				next_line = (char*)strchr(next_line+1,'\n');
		}
		cur.cpMax = sel;
		cur.cpMin = sel;
		SendMessage(eh,EM_EXSETSEL,NULL,(LPARAM)&cur);
		SendMessage(eh,EM_REPLACESEL,FALSE,(LPARAM)data);
		SendMessage(eh,EM_EXSETSEL,NULL,(LPARAM)&old);
		SendMessage(eh,EM_SCROLL,SB_PAGEDOWN,0);
		ShowWindow(h,SW_SHOWNOACTIVATE); // if hidden
	}
}

/* ------------------------------------------------------------------------ */

void window_log_set( window *w, const char *data ) {
	HWND h = w->h;
	HWND eh = (HWND)GetProp(h,"log");
	if( eh == NULL )
		MessageBox(h,data,"Log:",MB_OK);
	else {
		SetWindowText(eh,"");
		window_log_text(w,data);
	}
}

/* ------------------------------------------------------------------------ */

void window_set_title( window *w, const char *title ) {
	SetWindowText(w->h,title);
}

/* ------------------------------------------------------------------------ */

void window_set_controls( window *w, controls *c ) {
	SetProp(w->h,"controls",c);
	HANDLE old = GetProp(w->h,"oldproc");
	if( old == NULL ) {
		SetProp(w->h,"oldproc",(HANDLE)GetWindowLong(w->h,GWL_WNDPROC));
		SetWindowLong(w->h,GWL_WNDPROC,(LONG)capture_events);
	} else if( c == NULL ) {
		SetWindowLong(w->h,GWL_WNDPROC,(LONG)old);
		RemoveProp(w->h,"controls");
		RemoveProp(w->h,"oldproc");
	}
}

/* ------------------------------------------------------------------------ */

void window_error( const char *msg ) {
	MessageBox(NULL,msg,"Error :",MB_OK | MB_ICONERROR);
}

/* ************************************************************************ */

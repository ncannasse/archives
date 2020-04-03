/* ************************************************************************ */
/*																			*/
/*	Asynchronous Socket - Win32 IMPL										*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <windows.h>
#include <wlist.h>
#include "wsocket.h"

/* ------------------------------------------------------------------------ */

#define SOCKET_CLASS_NAME	"SAS Socket Window"
#define WM_CONNECT			(WM_USER + 1)
#define WM_DATA				(WM_USER + 2)
#define WM_CONNECTED		(WM_USER + 3)
#define SOCK_LISTEN_VALUE	5


struct SocketData {
	SOCKET s;
	int port;
	IPAddr addr;
	SocketEvent2 on_client_connect;
	SocketEvent on_close;
	SocketEvent on_read;
	SocketEvent on_connected;
	char *buffer;
	int  buflen;
	bool connected;
	WSocket *o;

	SocketData( SOCKET s ) : s(s), addr(0), buffer(NULL), buflen(0), on_client_connect(NULL), on_connected(NULL), on_close(NULL), on_read(NULL), connected(false) { 
		port = 0;
		addr = INADDR_NONE;
	}
	~SocketData() {
		delete buffer;
	}
};

/* ------------------------------------------------------------------------ */

static bool first_init = true; 
static WList<SocketData*> sockets;
static char localhost[256];
static IPAddr local_ip = 0;
static HWND socket_wnd = NULL;

/* ------------------------------------------------------------------------ */

SocketData *FindSocket( SOCKET s ) {
	FOREACH(SocketData*,sockets);
		if( item->s == s )
			return item;
	ENDFOR;
	return NULL;
}

/* ------------------------------------------------------------------------ */

void SocketRead( SocketData *s ) {
	unsigned long len = 0;
	if( ioctlsocket(s->s,FIONREAD,&len) == SOCKET_ERROR )
		return;
	if( len <= 0 )
		return;
	char *newbuf = new char[s->buflen+len];
	memcpy(newbuf,s->buffer,s->buflen);
	s->buflen += recv(s->s,newbuf+s->buflen,len,0);
	delete s->buffer;
	s->buffer = newbuf;
}

/* ------------------------------------------------------------------------ */

LRESULT CALLBACK SocketWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	if( uMsg == WM_DATA ) {
		WORD event = LOWORD(lParam);
		SOCKET from = wParam;
		SocketData *s = FindSocket(from);
		if( s != NULL ) {
			if( event == FD_READ ) {
				SocketRead(s);
				if( s->on_read != NULL )
					s->on_read(s->o);
			} else { // event = FD_CLOSE
				closesocket(s->s);
				s->connected = false;
				if( s->on_close != NULL )
					s->on_close(s->o);
			}
		}
	}
	else if( uMsg == WM_CONNECT ) {
		sockaddr_in peer;
		int addr_len = sizeof(peer);
		SOCKET from = wParam;
		SOCKET cl_sock = accept(from,(sockaddr*)&peer,&addr_len);
		IPAddr ip = peer.sin_addr.s_addr;

		if( WSAAsyncSelect(cl_sock,hwnd,WM_DATA,FD_READ | FD_CLOSE) == SOCKET_ERROR ) {
			closesocket(cl_sock);
			return TRUE;
		}
		
		SocketData *server = FindSocket(from);
		if( server == NULL || server->on_client_connect == NULL )
			closesocket(cl_sock);
		else {
			SocketData *cl = new SocketData(cl_sock);
			WSocket *o = new WSocket(cl);
			sockets.Add(cl);
			cl->addr = ip;
			cl->connected = true;
			server->on_client_connect( server->o, o );
		}

	} else if( uMsg == WM_CONNECTED ) {
		SOCKET from = wParam;
		SocketData *s = FindSocket(from);
		if( s != NULL ) {
			s->connected = (HIWORD(lParam) == 0);
			if( s->connected && WSAAsyncSelect(s->s,socket_wnd,WM_DATA,FD_READ | FD_CLOSE) == SOCKET_ERROR )		
				s->connected = false;
			if( !s->connected )
				closesocket(s->s);
			if( s->on_connected )
				s->on_connected(s->o);
		}
	}
	return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

/* ------------------------------------------------------------------------ */

bool SocketsInit() {
	WSADATA init_data;
	if( WSAStartup( MAKEWORD(2,0) , &init_data ) != 0 )
		return false;

	DWORD s = sizeof(localhost);
	GetComputerName(localhost,&s);
	local_ip = WSocket::Resolve(localhost);
	if( local_ip == -1 )
		return false;

	if( first_init ) {
		first_init = false;
		WNDCLASSEX wc;
		wc.cbSize = sizeof(wc);
		wc.style = 0;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = NULL;
		wc.hCursor = NULL;
		wc.hIcon = NULL;
		wc.hIconSm = NULL;
		wc.hInstance = NULL;
		wc.lpfnWndProc = SocketWindowProc;
		wc.lpszClassName = SOCKET_CLASS_NAME;
		wc.lpszMenuName = NULL;
		RegisterClassEx(&wc);
	}

	socket_wnd = CreateWindowEx(0,SOCKET_CLASS_NAME,SOCKET_CLASS_NAME,0,0,0,0,0,NULL,NULL,NULL,NULL);
	if( socket_wnd == NULL )
		return false;

	return true;
}

/* ------------------------------------------------------------------------ */

void SocketsClose() {
	DestroyWindow(socket_wnd);
	WSACleanup();
}

/* ------------------------------------------------------------------------ */

WSocket::WSocket() {
	d = new SocketData(INVALID_SOCKET);
	d->o = this;
	if( sockets.Empty() )
		SocketsInit();
	sockets.Add(d);
}

/* ------------------------------------------------------------------------ */

WSocket::WSocket( SocketData *d ) : d(d) {
	d->o = this;
}

/* ------------------------------------------------------------------------ */

WSocket::~WSocket() {
	sockets.Remove(d);
	if( d->connected ) 
		closesocket(d->s);
	delete d;
	if( sockets.Empty() )
		SocketsClose();
}

/* ------------------------------------------------------------------------ */

bool 
WSocket::Connected() {
	return d->connected;
}

/* ------------------------------------------------------------------------ */

bool 
WSocket::Bind( IPAddr addr, int port ) {

	SOCKET s = socket(AF_INET,SOCK_STREAM,0);
	if( s == INVALID_SOCKET )
		return false;

	sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port = htons(port);
    peer.sin_addr.s_addr = addr;
	if( bind(s, (sockaddr*)&peer, sizeof(peer)) == SOCKET_ERROR ) {
		closesocket(s);
		return false;
	}

	if( WSAAsyncSelect(s,socket_wnd,WM_CONNECT,FD_ACCEPT) == SOCKET_ERROR ) {
		closesocket(s);
		return false;
	}

	Disconnect();

	d->s = s;
	d->connected = true;
	d->addr = addr;
	d->port = port;
	listen(s,SOCK_LISTEN_VALUE);
	return true;
}

/* ------------------------------------------------------------------------ */

void
WSocket::Disconnect() {
	if( d->connected ) {
		d->connected = false;
		closesocket(d->s);
	}
}

/* ------------------------------------------------------------------------ */

bool
WSocket::Connect( IPAddr addr, int port ) {
	Disconnect();

	SOCKET s = socket(AF_INET,SOCK_STREAM,0);
	if( s == INVALID_SOCKET ) {
		if( d->on_connected )
			d->on_connected(this);
		return false;
	}

	if( d->on_connected && WSAAsyncSelect(s,socket_wnd,WM_CONNECTED,FD_CONNECT) == SOCKET_ERROR ) {
		closesocket(s);
		d->on_connected(this);
		return false;
	}

	d->s = s;
	d->addr = addr;
	d->port = port;

	sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port = htons(port);
    peer.sin_addr.s_addr = addr;

	if( connect(s,(sockaddr*)&peer,sizeof(peer)) == SOCKET_ERROR )
		return false;

	if( WSAAsyncSelect(s,socket_wnd,WM_DATA,FD_READ | FD_CLOSE) == SOCKET_ERROR ) {
		closesocket(s);
		return false;
	}

	d->connected = true;
	return true;
}

/* ------------------------------------------------------------------------ */

char *
WSocket::Buffer( int *buflen ) {
	*buflen = d->buflen;
	return d->buffer;
}

/* ------------------------------------------------------------------------ */

int
WSocket::Consume( int len ) {
	if( len <= 0 )
		return d->buflen;
	if( len >= d->buflen ) {
		delete d->buffer;
		d->buffer = NULL;
		d->buflen = 0;
		return 0;
	}
	char *newbuf = new char[d->buflen - len];
	memcpy(newbuf,d->buffer+len,d->buflen-len);
	delete d->buffer;
	d->buffer = newbuf;
	d->buflen -= len;
	return d->buflen;
}

/* ------------------------------------------------------------------------ */

IPAddr 
WSocket::HostIP() {
	return d->addr;
}

/* ------------------------------------------------------------------------ */

int
WSocket::HostPort() {
	return d->port;
}

/* ------------------------------------------------------------------------ */

void 
WSocket::OnRead( SocketEvent e ) {
	d->on_read = e;
	if( d->connected )
		recv(d->s,NULL,0,0);
}

/* ------------------------------------------------------------------------ */

void 
WSocket::OnClose( SocketEvent e ) {
	d->on_close = e;
}

/* ------------------------------------------------------------------------ */

void 
WSocket::OnConnected( SocketEvent e ) {
	d->on_connected = e;
}

/* ------------------------------------------------------------------------ */

void
WSocket::OnClientConnect( SocketEvent2 e ) {
	d->on_client_connect = e;
}

/* ------------------------------------------------------------------------ */

bool 
WSocket::Send( const char *data, int datalen ) {
	if( !d->connected ) 
		return false;
	int slen;
	while( datalen > 0 && (slen = send(d->s,data,datalen,0)) != SOCKET_ERROR ) {
		data += slen;
		datalen -= slen;
	}
	if( slen == SOCKET_ERROR ) {
		if( GetLastError() == WSAEWOULDBLOCK )  {
			Sleep(100);
			return Send(data,datalen);
		}
		return false;
	}
	return true;
}

/* ------------------------------------------------------------------------ */

bool
WSocket::Process() {
	MSG m;
	if( !GetMessage(&m,NULL,0,0) ) {
		PostQuitMessage(0);
		return false;
	}
	TranslateMessage(&m);
	DispatchMessage(&m);
	return true;
}

/* ------------------------------------------------------------------------ */

const char *
WSocket::LocalHost() {
	return localhost;
}

/* ------------------------------------------------------------------------ */

IPAddr 
WSocket::LocalHostIP() {
	return local_ip;
}

/* ------------------------------------------------------------------------ */

IPAddr
WSocket::Resolve(const char * host) {
	IPAddr dns = inet_addr(host);
	if( dns == INADDR_NONE ) {
		hostent* pHE = gethostbyname(host);
		if( pHE == 0 )
			return INADDR_NONE;
		dns = *((IPAddr*)pHE->h_addr_list[0]);
	}
	return dns;
}

/* ------------------------------------------------------------------------ */

const char *
WSocket::IPAddrString( IPAddr ip ) {
	in_addr sip;
	sip.s_addr = ip;
	return inet_ntoa(sip);
}

/* ************************************************************************ */

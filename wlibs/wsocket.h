/* ************************************************************************ */
/*																			*/
/*	Asynchronous Socket														*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#define INVALID_IP	((IPAddr)-1)

class WSocket;

typedef unsigned int IPAddr;
typedef void (*SocketEvent)( WSocket * );
typedef void (*SocketEvent2)( WSocket *, WSocket * );

struct SocketData;

class WSocket {
	SocketData *d;
public:

	WSocket();
	WSocket( SocketData *d );
	~WSocket();

	bool Bind( IPAddr host, int port );
	bool Connect( IPAddr host, int port );
	bool Send( const char *data, int datalen );
	char *Buffer( int *len );
	int Consume( int len );
	void Disconnect();
	
	bool Connected();

	void OnConnected( SocketEvent evt );
	void OnClientConnect( SocketEvent2 evt );
	void OnClose( SocketEvent evt );
	void OnRead( SocketEvent evt );

	IPAddr HostIP();
	int HostPort();

	void *data;

	static bool Process();
	static IPAddr Resolve( const char *host );
	static IPAddr LocalHostIP();
	static const char *LocalHost();
	static const char *IPAddrString( IPAddr addr );
};

/* ************************************************************************ */

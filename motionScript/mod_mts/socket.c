#include <value.h>
#ifdef _WIN32
#	include <winsock2.h>
	static bool init_done = false;
	static WSADATA init_data;
#else
#	include <sys/socket.h>
#	include <arpa/inet.h>
#	include <netdb.h>
typedef int SOCKET;
#define closesocket close
#define SOCKET_ERROR (-1)
#endif

DEFINE_CLASS(std,socket);

#define val_sock(o)		((SOCKET)val_odata(o))

value socket_setTimeout( value t ) {
	value o = val_this();
	int time;
	val_check_obj(o,t_socket);
	if( !val_is_int(t) )
		return NULLVAL;
	time = val_int(t);
	setsockopt(val_sock(o),SOL_SOCKET,SO_SNDTIMEO,(char*)&time,sizeof(int));
	setsockopt(val_sock(o),SOL_SOCKET,SO_RCVTIMEO,(char*)&time,sizeof(int));
	return val_true;
}

value socket_new0() {
	value o = val_this();
	val_check_obj(o,t_socket);
#ifdef _WIN32
	if( !init_done ) {
		WSAStartup(MAKEWORD(2,0),&init_data);
		init_done = true;
	}
#endif
	val_odata(o) = (value)socket(AF_INET,SOCK_STREAM,0);
	socket_setTimeout(alloc_int(5000));
	return o;
}

value socket_connect( value host, value port ) {
	value o = val_this();
	unsigned int ip;
	struct sockaddr_in peer;
	val_check_obj(o,t_socket);
	if( !val_is_string(host) || !val_is_int(port) )
		return NULLVAL;
	ip = inet_addr(val_string(host));
	if( ip == INADDR_NONE ) {
		struct hostent* pHE = gethostbyname(val_string(host));
		if( pHE == 0 )
			return false;
		memcpy(&ip,pHE->h_addr,sizeof(ip));
	}
	peer.sin_family = AF_INET;
	peer.sin_port = htons(val_int(port));
	peer.sin_addr.s_addr = ip;
	return alloc_bool( connect(val_sock(o),(struct sockaddr*)&peer,sizeof(peer)) <= 0 );
}

value socket_send( value data ) {
	value o = val_this();
	const char *cdata;
	int datalen, slen;
	val_check_obj(o,t_socket);
	if( !val_is_string(data) )
		return NULLVAL;	
	cdata = val_string(data);
	datalen = val_strlen(data);
	while( datalen > 0 && (slen = send(val_sock(o),cdata,datalen,0)) != SOCKET_ERROR ) {
		cdata += slen;
		datalen -= slen;
	}
	return alloc_bool( slen != SOCKET_ERROR );
}

value socket_receive() {
	value o = val_this();
	value s;
	stringbuf b;
	char buf[256];
	int len;	
	val_check_obj(o,t_socket);
	b = alloc_stringbuf(NULL);
	while( (len = recv(val_sock(o),buf,256,0)) != SOCKET_ERROR && len > 0 )
		stringbuf_append_sub(b,buf,len);	
	s = stringbuf_to_string(b);
	if( val_strlen(s) == 0 )
		return NULL;
	return s;
}

value socket_close() {
	value o = val_this();
	val_check_obj(o,t_socket);
	closesocket(val_sock(o));
	return val_true;
}

value std_socket() {
	value o = alloc_class(&t_socket);
	Constr(o,socket,Socket,0);
	Method(o,socket,connect,2);
	Method(o,socket,send,1);
	Method(o,socket,receive,0);
	Method(o,socket,close,0);
	Method(o,socket,setTimeout,1);
	return o;
}

#include "context.h"
#include <datastruct.h>

#define PARSE_HEADER(start,cursor) \
	cursor = start; \
	if( *cursor == '"' ) { \
		start++; \
		cursor++; \
		while( *cursor != '"' && *cursor != 0 ) \
			cursor++; \
	} else { \
		while( *cursor != 0 && *cursor != '\r' && *cursor != '\n' && *cursor != '\t' ) \
			cursor++; \
	}

#define HEADERS_NOT_SENT(msg) \
	if( c->headers_sent ) { \
		request_print(NULL,"Cannot set "); \
		request_print(NULL,msg); \
		request_print(NULL," <b>Headers already sent</b>.<br/>"); \
		return NULLVAL; \
	}

extern value std_url_decode( value v );

value std_cgi_getCookies() {
	const char *k = ap_table_get(CONTEXT()->r->headers_in,"Cookie");
	char *start, *end;
	value h = alloc_hash();
	if( k == NULL )
		return h;
	while( (start = strchr(k,'=')) != NULL ) {
		start++;
		end = start;
		while( *end != 0 && *end != '\r' && *end != '\n' && *end != ';' )
			end++;
		hash_replace(h,k,(int)(start-k-1),copy_string(start,(int)(end-start)));
		if( *end == '"' )
			end++;
		if( *end != ';' || end[1] != ' ' )
			break;
		k = end + 2;
	}
	return h;
}

value std_cgi_setCookie( value name, value v ) {
	mcontext *c = CONTEXT();
	stringbuf b;
	value str;
	if( !val_is_string(name) || !val_is_string(v) )
		return NULLVAL;
	HEADERS_NOT_SENT("Cookie");
	b = alloc_stringbuf(NULL);
	val_to_string(name,b);
	stringbuf_append(b,"=");
	val_to_string(v,b);
	stringbuf_append(b,";");
	str = stringbuf_to_string(b);
	ap_table_add(c->r->headers_out,"Set-Cookie",val_string(str));
	return val_true;
}

value std_cgi_getContentType() {
	return alloc_string( CONTEXT()->r->content_type );
}

value std_cgi_setContentType( value s ) {
	mcontext *c = CONTEXT();
	if( !val_is_string(s) )
		return NULLVAL;
	HEADERS_NOT_SENT("Content Type");
	c->r->content_type = val_string(s);
	return val_true;
}

value std_cgi_getHostName() {
	mcontext *c = CONTEXT();
	const char *h = c->r->connection->local_host;
	return alloc_string( h?h:c->r->connection->local_ip );
}

value std_cgi_getClientIP() {
	return alloc_string( CONTEXT()->r->connection->remote_ip );
}

value std_cgi_getURI() {
	request_rec *r = CONTEXT()->r;
	while( r->prev != NULL )
		r = r->prev;
	return alloc_string( r->uri );
}

value std_cgi_redirect( value s ) {
	mcontext *c = CONTEXT();
	if( !val_is_string(s) )
		return NULLVAL;
	HEADERS_NOT_SENT("Redirection");
	ap_table_set(c->r->headers_out,"Location",val_string(s));
	c->r->status = REDIRECT;
	return val_true;
}

value std_cgi_header( value s, value k ) {
	mcontext *c = CONTEXT();
	if( !val_is_string(s) || !val_is_string(k) ) 
		return NULLVAL;
	HEADERS_NOT_SENT("Header");
	ap_table_set(c->r->headers_out,val_string(s),val_string(k));
	return val_true;
}

value std_cgi_getClientHeader( value s ) {
	mcontext *c = CONTEXT();
	if( !val_is_string(s) ) 
		return NULLVAL;
	return alloc_string( ap_table_get(c->r->headers_in,val_string(s)) );
}

value std_cgi_getParamsString() {	
	return alloc_string(CONTEXT()->r->args);
}

value std_cgi_getPostData() {
	return CONTEXT()->post_data;
}


char *memfind( char *mem, int mlen, const char *v ) {
	char *found;
	int len = strlen(v);
	if( len == 0 )
		return mem;
	while( (found = memchr(mem,*v,mlen)) != NULL ) {
		if( (int)(found - mem) + len > mlen )
			break;
		if( memcmp(found,v,len) == 0 )
			return found;
		mlen -= (int)(found - mem + 1);
		mem = found + 1;		
	}
	return NULL;
}

void parse_multipart( mcontext *c, const char *ctype, const char *args, int argslen, value h ) {
	char *boundary = strstr(ctype,"boundary=");
	char *bend;
	char old, oldb1, oldb2;
	if( boundary == NULL )
		return;
	boundary += 9;
	PARSE_HEADER(boundary,bend);
	boundary-=2;
	oldb1 = *boundary;
	oldb2 = boundary[1];
	*boundary = '-';
	boundary[1] = '-';
	old = *bend;
	*bend = 0;
	{
		char *name, *end_name;
		char *data, *end_data;
		char tmp;
		name = strstr(args,boundary);
		while( name != NULL ) {
			name = strstr(name,"Content-Disposition:");
			if( name == NULL )
				break;
			name = strstr(name,"name=");
			if( name == NULL )
				break;
			name += 5;
			PARSE_HEADER(name,end_name);
			data = strstr(end_name,"\r\n\r\n");
			if( data == NULL )
				break;
			data += 4;
			end_data = memfind(data,argslen - (int)(data - args),boundary);
			if( end_data == NULL )
				break;
			tmp = *end_name;
			*end_name = 0;
			hash_replace(h,name,(int)(end_name - name),copy_string(data,(int)(end_data-data-2)));
			*end_name = tmp;

			name = end_data;
		}
	}
	*boundary = oldb1;
	boundary[1] = oldb2;
	*bend = old;
}

void parse_get( value h, const char *args ) {
	char *aand, *aeq, *asep;
	while( true ) {
		aand = strchr(args,'&');
		if( aand == NULL ) {
			asep = strchr(args,';');
			aand = asep;
		} else {
			asep = strchr(args,';');
			if( asep != NULL && asep < aand )
				aand = asep;
		}
		if( aand != NULL )
			*aand = 0;
		aeq = strchr(args,'=');
		if( aeq != NULL ) {
			*aeq = 0;
			hash_replace(h,args,(int)(aeq-args),std_url_decode(alloc_string(aeq+1)));
			*aeq = '=';
		}
		if( aand == NULL )
			break;
		*aand = (aand == asep)?';':'&';
		args = aand+1;
	}
}

value std_cgi_getParams() {
	mcontext *c = CONTEXT();
	const char *args = c->r->args;
	value h = alloc_hash();

	// PARSE "GET" PARAMS
	if( args != NULL )
		parse_get(h,args);
	 
	// PARSE "POST" PARAMS
	if( c->post_data != NULL ) {
		const char *ctype = ap_table_get(c->r->headers_in,"Content-Type");
		if( strstr(ctype,"multipart/form-data") != NULL )
			parse_multipart(c,ctype,val_string(c->post_data),val_strlen(c->post_data),h);		
		else if( strstr(ctype,"application/x-www-form-urlencoded") != NULL )
			parse_get(h,val_string(c->post_data));
	}

	return h;
}

value get_cwd() {
	mcontext *c = CONTEXT();
	char *s = strrchr(c->r->filename,'/');
	value v;
	char old;
	if( s != NULL ) {
		old = s[1];
		s[1] = 0;
	}
	v = alloc_string(c->r->filename);
	if( s != NULL )
		s[1] = old;
	return v;
}

DEFINE_PRIM(std_cgi_getCookies,0);
DEFINE_PRIM(std_cgi_setCookie,2);
DEFINE_PRIM(std_cgi_getContentType,0);
DEFINE_PRIM(std_cgi_setContentType,1);
DEFINE_PRIM(std_cgi_getHostName,0);
DEFINE_PRIM(std_cgi_getClientIP,0);
DEFINE_PRIM(std_cgi_getURI,0);
DEFINE_PRIM(std_cgi_redirect,1);
DEFINE_PRIM(std_cgi_getParams,0);
DEFINE_PRIM(std_cgi_getParamsString,0);
DEFINE_PRIM(std_cgi_getPostData,0);
DEFINE_PRIM(std_cgi_header,2);
DEFINE_PRIM(std_cgi_getClientHeader,1);

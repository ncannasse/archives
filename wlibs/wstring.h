/* ****************************************************************	*/
/*																	*/
/*	class WString													*/
/*		(c)2000 by Nicolas Cannasse									*/
/*		standard string class										*/
/*																	*/
/* ****************************************************************	*/
#ifndef WSTRING_H_
#define WSTRING_H_
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

class WString {
	int *count;
	char *buf;
	int maxlen;
	int slen;

	void _release() {
		if( count ) {
			if( --(*count) == 0 ) {
				delete buf;	
				delete count;
			}
			buf = NULL;
			count = NULL;
			maxlen = 0;
			slen = 0;
		}
	}

	bool _unique() {
		if( count && *count > 1 ) {
			(*count)--;
			count = new int(1);
			return false;
		}
		return true;
	}

	void _append( const char *s, int len ) {
		if( len == 0 )
			return;
		int newlen = slen + len;
		bool u = _unique();
		if( u && maxlen >= newlen ) {
			memcpy(buf+slen,s,len);
			buf[newlen] = 0;
			slen = newlen;
		} else {
			if( count == NULL )
				count = new int(1);
			if( newlen < slen * 2 )
				newlen = slen * 2;
			char *newbuf = new char[newlen+1];
			memcpy(newbuf,buf,slen);
			memcpy(newbuf+slen,s,len);
			newbuf[slen+len] = 0;
			if( u )
				delete buf;
			buf = newbuf;
			maxlen = newlen;
			slen += len;
		}
	}

public:
	
	WString() {
		count = NULL;
		slen = 0;
		maxlen = 0;
		buf = NULL;
	}

	WString( char c ) {
		count = NULL;
		slen = 0;
		maxlen = 0;
		buf = NULL;
		set(c);
	}

	WString( const char *s ) {
		count = NULL;
		slen = 0;
		maxlen = 0;
		buf = NULL;
		set(s);
	}
	
	WString( const WString &s ) {
		count = NULL;
		slen = 0;
		maxlen = 0;
		buf = NULL;
		set(s);
	}

	operator const char *() {
		return c_str();
	}

	WString& operator = ( const WString &s ) {
		set(s);
		return *this;
	}

	WString& operator = ( const char *s ) {
		set(s);
		return *this;
	}

	WString& operator = ( char c ) {
		set(c);
		return *this;
	}

	WString& operator += ( const WString& s ) {
		append(s);
		return *this;
	}

	WString& operator += ( const char *s ) {
		append(s);
		return *this;
	}

	WString& operator += ( char c ) {
		append(c);
		return *this;
	}

	inline char& operator []( int index ) {
		assert( index >= 0 && index < slen );
		return buf[index];
	}
	
	inline char operator[] (int index) const {
		assert( index >= 0 && index < slen );
		return buf[index];
	}

	bool equals( const WString &s ) const {
		return (slen == s.slen) && (s.buf == buf || memcmp(s.buf,buf,slen) == 0);
	}

	bool equals( const char *s ) const {
		if( buf == NULL )
			return (s && *s == 0);
		return (strcmp(s,buf) == 0);
	}

	void unique() {
		if( !_unique() ) {
			char *nbuf = new char[slen+1];
			memcpy(nbuf,buf,slen+1);
			buf = nbuf;
			maxlen = slen;
		}
	}

	void append( const WString &s ) {
		_append(s.buf,s.slen);
	}

	void append( const char *s ) {
		_append(s,strlen(s));
	}

	void append( char c ) {
		_append(&c,1);
	}

	void set( char c ) {
		_release();
		count = new int(1);
		slen = 1;
		maxlen = 1;
		buf = new char[maxlen+1];
		buf[0] = c;
		buf[1] = 0;
	}

	void set( const char *s ) {
		_release();
		if( s && *s ) {
			count = new int(1);
			slen = strlen(s);
			maxlen = slen;
			buf = new char[maxlen+1];
			memcpy(buf,s,maxlen+1);
		}
	}

	void set( const WString &s ) {
		_release();
		if( s.count ) {
			(*s.count)++;
			count = s.count;
			buf = s.buf;
			maxlen = s.maxlen;
			slen = s.slen;
		}
	}
	
	void resize( int n ) {
		assert( n >= 0 );
		if( n == 0 )
			_release();
		else {
			bool u = _unique();
			int newlen = (slen<n?slen:n);
			char *newbuf = new char[n+1];
			memcpy(newbuf,buf,newlen);
			newbuf[newlen] = 0;
			if( u )
				delete buf;
			buf = newbuf;
			slen = newlen;
			maxlen = n;
		}
	}

	int find( WString seq, int start = 0 ) const {
		int len = seq.slen;
		int i;
		assert( start >= 0 );
		for(i=start;i<slen-len+1;i++)
			if( memcmp(buf+i,seq.buf,len) == 0 )
				return i;
		return -1;
	}

	WString substr( int start, int len = -1 ) const {
		assert( start >= 0 && start <= slen );
		if( len < 0 )
			len = slen - start - (len + 1);
		assert( len >= 0 && len + start <= slen );
		char c = buf[start+len];
		buf[start+len] = 0;
		WString s(buf+start);
		buf[start+len] = c;
		return s;
	}

	void replace( int pos, int len, WString r ) {
		assert( pos >= 0 && pos < slen );
		assert( len >= 0 && pos+len <= slen && len <= r.slen );
		int newlen = slen - len + r.slen;
		if( newlen == 0 ) {
			_release();
			return;
		}
		bool u = _unique();
		if( u && newlen <= maxlen ) {
			memcpy(buf+pos+r.slen,buf+pos+len,slen-(pos+len)+1);
			memcpy(buf+pos,r.buf,len);
		} else {
			char *newbuf = new char[newlen+1];
			memcpy(newbuf,buf,pos);
			memcpy(newbuf+pos,r.buf,r.slen);
			memcpy(newbuf+pos+r.slen,buf+pos+len,slen - (pos+len) + 1);
			if( u )
				delete buf;
			buf = newbuf;
			maxlen = newlen;
		}
		slen = newlen;
	}

	~WString() {
		_release();
	}

	void clear() {
		_release();
	}

	int length() const {
		return slen;
	}

	const char *c_str() const {
		return buf?buf:"";
	}

};

inline WString __cdecl operator + ( const WString &a, const WString &b ) { return ((WString)a) += b; }
inline WString __cdecl operator + ( const WString &a, const char *b ) { return ((WString)a) += b; }
inline WString __cdecl operator + ( const WString &a, char b ) { return ((WString)a) += b; }
inline WString __cdecl operator + ( const char *a, const WString &b ) { return ((WString)a) += b; }
inline WString __cdecl operator + ( char a, const WString &b ) { return ((WString)a) += b; }

inline bool __cdecl operator == ( const WString &a, const WString &b ) { return a.equals(b); }
inline bool __cdecl operator == ( const WString &a, const char *b ) { return a.equals(b); }
inline bool __cdecl operator == ( const char *a, const WString &b ) { return b.equals(a); }

inline bool __cdecl operator != ( const WString &a, const WString &b ) { return !a.equals(b); }
inline bool __cdecl operator != ( const WString &a, const char *b ) { return !a.equals(b); }
inline bool __cdecl operator != ( const char *a, const WString &b ) { return !b.equals(a); }

inline WString wprintf( const char *format, ... ) {
	static char buf[256];
	va_list args;
	va_start(args,format);
	int n = vsprintf(buf,format,args);
	assert( n < 255 );
	return (WString)buf;
}

inline WString itos( long l ) {
	char buf[20];
	sprintf(buf,"%ld",l);
	return (WString)buf;
}

inline WString ftos( double d ) {
	char buf[30];
	sprintf(buf,"%g",d);
	return (WString)buf;
}

inline WString htos( long h ) {
	char buf[20];
	sprintf(buf,"0x%.8X",h);
	return (WString)buf;
}

inline WString strrep( WString s, WString seq, WString by ) {
	int p;
	int st = 0;
	int l = seq.length();
	int lb = by.length();
	while( (p=s.find(seq,st)) >=0 ) {
		s.replace(p,l,by);
		st = p+lb+1;
	}
	return s;
}

inline int wstrcmp( WString a, WString b ) {
	return strcmp(a.c_str(),b.c_str());
}

#endif
/* ****************************************************************	*/

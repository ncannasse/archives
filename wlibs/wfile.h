/* ****************************************************************	*/
/*																	*/
/*	class WFile														*/
/*		(c)2000 by Nicolas Cannasse									*/
/*		standard file class											*/
/*																	*/
/* ****************************************************************	*/
#ifndef WFILE_H
#define WFILE_H

#include <stdio.h>
#include "wstring.h"
#include "res/zlib.h"

typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;

class WFile {

	FILE	*handle;
	WString name;
		
	char *buf;
	int   buflen;
	int   bufsize;
	bool reading;

	long size;
	long pos;

public:

	long Size() {
		return size;
	}

	long Pos() {
		return pos;
	}

	bool Eof() {
		return (pos >= size);
	}

	WString Name() {
		return name;
	}

	void Close() {
		if( handle != NULL ) {
			fclose(handle);
			handle = NULL;
		}
	}

	void Flush() {
		if( handle != NULL )
			fflush(handle);
	}

	bool Open() {
		Close();
		if( name.length() > 0 ) 
			handle=fopen(name.c_str(),"rb");
		if( handle != NULL ) {
			fseek( handle, 0, 2 );
			size = ftell(handle);
			fseek( handle, 0, 0 );
		}
		else
			size = 0;
		pos = 0;
		buflen = 0;
		reading = true;
		return ( handle != NULL );
	}

	bool Append() {
		Close();
		handle=fopen(name.c_str(),"ab");
		if( handle != NULL ) {
			size = ftell(handle);
			pos = size;
		}
		else {
			size = 0;
			pos = 0;
		}
		reading = false;
		return ( handle != NULL );
	}

	bool Create() {
		Close();
		size = 0;
		pos = 0;
		reading = false;
		return ((handle=fopen(name.c_str(),"wb")) != NULL );
	}
	
	void SetPos( int p ) {
		if( p < pos - bufsize || !reading || !pos) {
			fseek(handle,p,0);//SET
			buflen = 0;
			pos = ftell(handle);
		} else {
			buflen += pos - p;
			pos = p;
		}
	}

	void Seek( int p ) {
		int l = 0;
		if( p <= buflen ) {
			buflen-=p;
			pos+=p;
			return;
		}
		while( p > buflen ) {
			p-=buflen;
			l+=buflen;			
			bufsize = fread( buf, 1, bufsize, handle );
			buflen = bufsize;
			if( buflen == 0 ) {
				pos+=l;
				return;
			}
		}
		buflen-=p;
		l+=p;
		pos+=l;
	}

	int Read( void *rbuf, int maxlen ) {
		int l = 0;
		if( maxlen <= buflen ) {
			memcpy( rbuf, buf+bufsize-buflen, maxlen );
			buflen-=maxlen;
			pos+=maxlen;
			return maxlen;
		}
		while( maxlen > buflen ) {
			memcpy( rbuf, buf+bufsize-buflen, buflen );
			maxlen-=buflen;
			l+=buflen;
			rbuf=(char*)rbuf+buflen;
			bufsize = fread( buf, 1, bufsize, handle );
			buflen = bufsize;
			if( buflen == 0 ) {
				pos+=l;
				return l;
			}
		}
		memcpy( rbuf, buf+bufsize-buflen, maxlen );
		buflen-=maxlen;
		l+=maxlen;
		pos+=l;
		return l;
	}

	BYTE ReadByte() {
		BYTE b;
		Read(&b,1);
		return b;
	}

	WORD ReadWord() {
		WORD w;
		Read(&w,2);
		return w;
	}

	DWORD ReadDWord() {
		DWORD dw;
		Read(&dw,4);
		return dw;
	}

	bool ReadLine( WString *string, const WString stopchars = "\r\n" ) {
		int p = 0;
		bool r = true;
		char c;

		string->resize(0);
		while(1) {
			if( Read( &c, 1 ) == 0 ) {
				if( p == 0 )
					r = false;
				break;
			}			
			if( strchr(stopchars.c_str(),c) != NULL ) {
				// \r\n EOL for Win32
				if( c == '\r' && Read(&c,1) && c != '\n' ) {
					pos--;
					buflen++;
				}
				break;
			}
			*string += c;
			p++;
		}
		return r;
	}

	bool ReadString( WString *string ) {
		int p = 0;
		bool r = true;
		char c;
		string->resize(0);
		while(1) {
			if( Read( &c, 1 ) == 0 ) {
				if( p == 0 )
					r = false;
				break;
			}			
			if( c == 0 )
				break;
			*string += c;
			p++;
		}
		return r;
	}

	void Write( const void *buffer, int len ) {
		fwrite( (char*)buffer,1,len, handle );
		pos+=len;
		if( pos > size )
			size = pos;
	}

	void Write( const WString string ) {
		Write(string.c_str(),string.length());
	}

	void WriteLine( const WString string ) {
		Write(string);
		Write("\r\n",2);
	}

	void WriteString( const WString string ) {
		char c = 0;
		Write(string);
		Write(&c,1);
	}

	WFile( WString Name, int BufferSize = 1024 ) {
		handle = NULL;
		name = Name;
		size = 0;
		pos = 0;
		bufsize = BufferSize;
		buf = new char[bufsize];
		buflen = 0;
	}

	bool Decompress( int tsize ) {
		int curp = ftell(handle);
		fseek(handle,0,SEEK_END);
		int rsize = ftell(handle)-curp;
		fseek(handle,curp,SEEK_SET);
		char *nbuf = new char[buflen+rsize];
		memcpy(nbuf,buf+bufsize-buflen,buflen);
		fread(nbuf+buflen,1,rsize,handle);
		delete buf;
		buf = new char[tsize];
		if( uncompress((unsigned char *)buf,(unsigned long *)&tsize,(unsigned char *)nbuf,rsize+buflen) != Z_OK ) {
			delete buf;
			buf = NULL;
			delete nbuf;
			return false;
		}
		delete nbuf;
		bufsize = tsize;
		buflen = tsize;
		size = tsize+pos;
		return true;
	}

	~WFile() {
		Close();
		delete buf;
	}
};

#endif
/* ****************************************************************	*/

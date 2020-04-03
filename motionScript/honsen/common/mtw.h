/* ************************************************************************ */
/*																			*/
/*	MTW File Format															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include "wstring.h"
#include "wlist.h"

typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef DWORD TAG;

#define MAKETAG(x)			((#x[3]<<24) | (#x[2]<<16) | (#x[1]<<8) | #x[0])

class MTWReader {
public:
	virtual bool read(void *buf, unsigned int size) = 0;
};

class MTWWriter {
public:
	virtual bool write(void *buf, unsigned int size) = 0;
};

class MTW {
public:
	union {
		TAG tag;
		char ctag[4];
	};
	DWORD size;
	union {
		void *data;
		WList<MTW*> *childs;
		DWORD number;
	};

	MTW( TAG tag );
	MTW( TAG tag, DWORD number );
	MTW( TAG tag, DWORD size, void *data);
    MTW( const MTW &t );
	~MTW();

	bool IsGroup() const;
	bool IsData() const;
	bool IsIdent() const;
	bool IsNumber() const;
	bool HasChild( TAG t ) const;
	WList<MTW*> Childs( TAG t = 0 ) const;
	MTW* Child( TAG t ) const ;
	bool AddChild( MTW *t );
	bool RemoveChild( MTW *t );
	bool WriteFile( WString file );
	bool Write( MTWWriter *writer );
	int CalcSize();

	static MTW* ReadFile( WString file );
	static MTW* ReadData( char *d, int length );
	static MTW* Read( MTWReader *reader );
	static WString ToString( TAG t );
};

/* ************************************************************************ */

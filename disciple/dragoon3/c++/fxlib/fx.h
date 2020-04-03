#pragma once
#include <mdx.h>

#pragma warning (disable : 4305)
#pragma warning (disable : 4244)
#define cos cosf
#define sin sinf

struct FSIZE {
	FLOAT width, height;
	FSIZE() { }
	FSIZE( FLOAT width, FLOAT height ) : width(width), height(height) { }
};

struct FCOORDS {
	FLOAT tu,tv;
	FLOAT tu2,tv2;
	FCOORDS() { }
	FCOORDS( FLOAT tu, FLOAT tv, FLOAT tu2, FLOAT tv2 ) : tu(tu), tv(tv), tu2(tu2), tv2(tv2) { }
};


extern MATRIX MID;
extern FCOORDS DEFCOORDS;

DWORD color_lerp( DWORD a, DWORD b, FLOAT delta);

#include <math.h>

#include "anyfx.h"
#include "spritedraw.h"
#include "linedraw.h"

#include <wlist.h>

template<class P> class DataList {
	WList<P*> data;
public:
	DataList() { }
	~DataList() { data.Delete(); }
	void Add( P* p ) { data.Add(p); }
	void Kill( P* p ) { data.Delete(p); }
	int Length() { return data.Length(); }
	template<class T> void Process( T *t, void (T::*f)( P*) ) {
		FOREACH(P*,data);
			((*t).*f)(item);
		ENDFOR;
	}
};
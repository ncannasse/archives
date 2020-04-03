/* ************************************************************************ */
/*																			*/
/*	WXML																	*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */

#include "wstring.h"
#include "wlist.h"


class WXml {
public:
	struct WAttrib {
		WString name;
		WString value;
	};

	WString tag;
	WList<WAttrib*> attribs;
	WList<WXml*> childs;

	WXml( WString tag );
	WXml( const WXml &cpy );
	~WXml();

	bool HasAttrib( WString name );
	WString Attrib( WString name );
	int AttribInt( WString name );

	WString ToString();
	WString ToStringFmt();

	static WXml *ParseFile( WString name );
	static WXml *ParseString( WString str );
	static WXml *ParseString( const char *str, int *strlen );
};

/* ************************************************************************ */

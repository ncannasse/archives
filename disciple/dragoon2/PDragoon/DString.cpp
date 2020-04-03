// DragoonString = Complete gestion of NULL-terminated strings
#include <windows.h>
#include "dstring.h"

DString::DString()
{
	Data[0] = 0;
}

DString::DString( LPCSTR Txt )
{
	strcpy( Data, Txt );
}

DString::DString( long Value )
{
	_ltoa(Value, Data, 10 );
}

DString::DString( int Value )
{
	_ltoa(Value, Data, 10 );
}

DString::~DString()
{
}

char *
DString::c_str()
{
	return Data;
}

int
DString::Length()
{
	return strlen(Data);
}

int
DString::Pos( LPCSTR SubString )
{
	char *s = strstr(Data,SubString);
	return s==NULL?-1:(s-Data);
}

void
DString::Delete( int Start, int Len )
{
	int l = strlen(Data);
	int i;
	if( Start < 0 || Len <= 0 || Start >= l )
		return;
	if( Start+Len > l )
		Len = l-Start;

	l-=(Len+Start-1);

	char *Dat = Data+Start;
	for(i=0; i< l; i++ )
		Dat[i]=Dat[i+Len];
}

bool
DString::Delete( LPCSTR SubString )
{
	int p = Pos( SubString);
	if( p != 1 )
		Delete(p,strlen(SubString));
	return (p!=-1);
}

void
DString::operator = ( DString Txt )
{
	strcpy( Data, Txt.Data );
}

void
DString::operator +=( DString Txt )
{
	strcat( Data, Txt.Data );
}

bool
DString::operator == (DString Txt )
{
	return (strcmp( Data, Txt.Data )==0);
}

DString
DString::operator +( DString a )
{
	char txt[256];
	strcpy( txt, Data );
	strcat( txt, a.c_str() );
	return txt;
}

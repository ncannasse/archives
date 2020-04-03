// DragoonString = Complete gestion of NULL-terminated strings
#pragma once

class DString {


public:

	DString operator +( DString a );

	int		Length	( void				 );
	void	Delete	( int Start, int Len );
	bool	Delete	( LPCSTR SubString	 );
	int		Pos		( LPCSTR SubString	 );

	void operator = ( DString Txt );

	void operator += ( DString Txt );

	bool operator == ( DString Txt );

	char *	c_str( void );

	DString();
	DString( LPCSTR chain );
	DString( long nb );
	DString( int nb );

	~DString();


private:

	char Data[256];

};
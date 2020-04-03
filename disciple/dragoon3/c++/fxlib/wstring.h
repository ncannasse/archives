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
#include <string>
typedef std::string WString;

#ifdef WUSE_MEM
#include <wmem.h>
#endif

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
	sprintf(buf,"%.8X",h);
	return (WString)buf;
}

inline WString trim( WString s ) {
	int d = s.find_first_not_of(" \t");
	int e = s.find_last_not_of(" \t");
	if( d < 0 || e < 0 )
		return "";
	return s.substr(d,e-d+1);
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

#endif
/* ****************************************************************	*/

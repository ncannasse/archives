/* ******************************************************************** */
/* erreurs.h                                                            */
/* ******************************************************************** */
/* - gestion du fichier log                                             */
/* ******************************************************************** */
#pragma once
/* ******************************************************************** */
#include <windows.h>
#include <stdio.h>
#include <crtdbg.h>
/* ******************************************************************** */

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define delete(x) delete (__FILE__, __LINE__,x)

#define Log(x)					Erreurs->_Log(x)
#define FichierNonTrouve(x)		Erreurs->_FichierNonTrouve(x)
#define FichierNonValide(x)		Erreurs->_FichierNonValide(x)
#define ResNonTrouve(x,y)		Erreurs->_ResNonTrouve(x,y)
#define ResNonValide(x,y)		Erreurs->_ResNonValide(x,y)
#define FatalError(x)			Erreurs->_FatalError(x)
#define DirectXError(x,y)		Erreurs->_DirectXError(x,y)
#define DirectXFatalError(x,y)	Erreurs->_DirectXFatalError(x,y)

#else

#define Log(x)					rDX++
#define FichierNonTrouve(x)		rDX++
#define FichierNonValide(x)		rDX++
#define ResNonTrouve(x,y)		rDX++
#define ResNonValide(x,y)		rDX++
#define FatalError(x)			rDX++
#define DirectXError(x,y)		rDX++
#define DirectXFatalError(x,y)	rDX++

#endif

/* ******************************************************************** */

/* ******************************************************************** */

class CErreurs {
	FILE *f;

#ifdef _DEBUG
	_CrtMemState checkPt1;
	_CRT_ALLOC_HOOK  OldAllocHook;
	static int __cdecl  MyAllocHook(int,void*,size_t,int,long,const unsigned char*,int);
#endif

	bool IsError;
	void Gere_Erreurs ( LPCSTR msg );

public:

#ifdef _DEBUG
	void _Log				( long i );
	void _Log				( LPCSTR c );

	void _DirectXError		( LPCSTR Mes, long Err );
#endif
	void _FichierNonTrouve	( LPCSTR Nom );
	void _FichierNonValide	( LPCSTR Nom );

	void _ResNonTrouve		( LPCSTR Pak, DWORD ID );
	void _ResNonValide		( LPCSTR Pak, DWORD ID );

	void _FatalError		( LPCSTR Mes );
	void _DirectXFatalError	( LPCSTR Mes, long Err );

	CErreurs();
	~CErreurs();
};
/* ******************************************************************** */
extern CErreurs   *Erreurs;
extern HRESULT rDX;
/* ******************************************************************** */
/* fin du fichier erreurs.h                                             */
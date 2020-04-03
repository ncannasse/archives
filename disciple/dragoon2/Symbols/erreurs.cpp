/* ******************************************************************** */
/* erreurs.cpp                                                          */
/* ******************************************************************** */
/* - gestion du fichier log                                             */
/* ******************************************************************** */
#pragma once
/* ******************************************************************** */
#include "erreurs.h"
#include <crtdbg.h>
#include <stdlib.h>
#include <stdio.h>
/* ******************************************************************** */
CErreurs *Erreurs = NULL;
HRESULT	  rDX;
/* ******************************************************************** */
static const char N[2]={ 13,10 };
static const char *Fin = "FIN";
/* ******************************************************************** */
#ifdef _DEBUG
static FILE* fi;
#define LOG_FREE
#endif _DEBUG
/* ******************************************************************** */

CErreurs::CErreurs()
{
#ifdef _DEBUG
	fi = fopen("memoire.txt","w");
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF
		| _CRTDBG_CHECK_ALWAYS_DF
		| _CRTDBG_CHECK_CRT_DF 
		| _CRTDBG_DELAY_FREE_MEM_DF 
		| _CRTDBG_LEAK_CHECK_DF
		| _CRTDBG_REPORT_FLAG);
	OldAllocHook = _CrtSetAllocHook(MyAllocHook);
	_CrtSetReportMode (_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_REPORT_MODE);
	_CrtMemCheckpoint( &checkPt1 );
	const char *Fichier = "erreurs.txt";
	const char *Debut   = "DEBUT";
	f = fopen(Fichier,"wt");
	fwrite (Debut,1,strlen(Debut),f);
	fwrite(N,1,2,f);
	fwrite(N,1,2,f);
	fflush(f);
#endif
	IsError = false;
}

/* ******************************************************************** */

CErreurs::~CErreurs()
{
#ifdef _DEBUG
	_CrtMemDumpAllObjectsSince( &checkPt1 );
	_CrtMemDumpStatistics( &checkPt1 );
	_CrtCheckMemory( );
	OldAllocHook = _CrtSetAllocHook(OldAllocHook);
	_CrtMemState checkPtMaintenant
		,checkPtDiff;
	_CrtMemCheckpoint (&checkPtMaintenant);
	fclose (fi);
	if (_CrtMemDifference (&checkPtDiff,&checkPt1,&checkPtMaintenant))
	{
		_Log("Probleme de désallocation !");
		IsError = true;
	}
	if (IsError)
		MessageBox(NULL,"An error may have occured","WARNING !",MB_OK | MB_ICONEXCLAMATION);
	_Log(Fin);
	fclose (f);
#endif
}

/* ******************************************************************** */
#ifdef _DEBUG
int __cdecl 
CErreurs::MyAllocHook(int nAllocType
					  ,void *pvData
					  ,size_t nSize
					  ,int nBlockUse
					  ,long lRequest
					  ,const unsigned char *szFileName
					  ,int nLine)
{
   const char *operation[] = { "", "allocating", "re-allocating", "freeing" };
   const char *blockType[] = { "Free", "Normal", "CRT", "Ignore", "Client" };
   // Ignore internal C runtime library allocations
   if ( nBlockUse == _CRT_BLOCK )
      return( TRUE );
	// Log ou pas les free ?
#ifndef LOG_FREE
   if ( nAllocType == 3)
	return (TRUE);
#endif
	// Ne traite pas les cas bizarres
   _ASSERT( ( nAllocType > 0 ) && ( nAllocType < 4 ) );
   _ASSERT( ( nBlockUse >= 0 ) && ( nBlockUse < 5 ) );
	// log le message
	if ( pvData != NULL )
		fprintf( fi, 
		"Memory operation in %s, line %d: %s a %d-byte '%s' block (# %ld) at %X\n"
		,szFileName, nLine, operation[nAllocType], nSize
	   ,blockType[nBlockUse], lRequest, pvData );
	else fprintf( fi, 
		"Memory operation in %s, line %d: %s a %d-byte '%s' block (# %ld)\n"
		,szFileName, nLine, operation[nAllocType], nSize
		,blockType[nBlockUse], lRequest );
   fflush (fi);
   return( TRUE );         // Allow the memory operation to proceed
}
#endif

/* ******************************************************************** */

#ifdef _DEBUG

void
CErreurs::_Log(long i)
{
	char buf[20];
	_ltoa(i,buf,10);
	fwrite(buf,1,strlen(buf),f);
	fwrite(N,1,2,f);
	fflush(f);
}

/* ******************************************************************** */

void
CErreurs::_Log(LPCSTR c)
{
#ifdef _DEBUG
	fwrite(c,1,strlen(c),f);
	fwrite(N,1,2,f);
	fflush(f);
#endif
}

#endif

/* ******************************************************************** */

void
CErreurs::_FichierNonTrouve( LPCSTR Nom )
{
	char buf[256];
	strcpy(buf,"File not found: ");
	strcat(buf,Nom);
#ifdef _DEBUG
	_Log(buf);
#endif
	Gere_Erreurs(buf);
}

/* ******************************************************************** */

void
CErreurs::_FichierNonValide( LPCSTR Nom )
{
	char buf[256];
	strcpy(buf,"File not valid: ");
	strcat(buf,Nom);
#ifdef _DEBUG
	_Log(buf);
#endif
	Gere_Erreurs(buf);
}

/* ******************************************************************** */

void
CErreurs::_FatalError( LPCSTR ErrMsg )
{
	char buf[256];
	strcpy(buf,"FATAL ERROR ! ");
	strcat(buf,ErrMsg);
#ifdef _DEBUG
	_Log(buf);
#endif
	Gere_Erreurs(buf);
}

/* ******************************************************************** */

#ifdef _DEBUG
void
CErreurs::_DirectXError( LPCSTR Mes, long Err)
{
	char buf[256];
	strcpy(buf,"DirecXError: ");
	strcat(buf,Mes);
	_ltoa(Err,buf+strlen(buf), 10 );
	_Log(buf);
	IsError = true;
}
#endif

/* ******************************************************************** */

void
CErreurs::_DirectXFatalError( LPCSTR Mes, long Err)
{
	char buf[256];
	strcpy(buf,"DirecXError: ");
	strcat(buf,Mes);
	_ltoa(Err,buf+strlen(buf), 10 );
#ifdef _DEBUG
	_Log(buf);
#endif
	Gere_Erreurs(buf);
}

/* ******************************************************************** */

void
CErreurs::_ResNonTrouve( LPCSTR Pak, DWORD ID )
{
	char buf[256];
	strcpy(buf,"Resource missing in file: ");
	strcat(buf,Pak);
	strcat(buf," ID: ");
	_ltoa(ID,buf+strlen(buf), 10 );
#ifdef _DEBUG
	_Log(buf);
#endif
	Gere_Erreurs(buf);
}

/* ******************************************************************** */

void
CErreurs::_ResNonValide( LPCSTR Pak, DWORD ID )
{
	char buf[256];
	strcpy(buf,"Resource not valid in file: ");
	strcat(buf,Pak);
	strcat(buf," ID: ");
	_ltoa(ID,buf+strlen(buf), 10 );
#ifdef _DEBUG
	_Log(buf);
#endif
	Gere_Erreurs(buf);
}

/* ******************************************************************** */

void
CErreurs::Gere_Erreurs ( LPCSTR Msg) 
{
/*	MessageBox(NULL,Msg,"Error:",MB_OK);
	PostQuitMessage(0);*/
}

/* ******************************************************************** */
/* fin de erreurs.cpp													*/


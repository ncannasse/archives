#pragma once

// HonsenPlugin.h : fichier d'en-t�te principal pour HonsenPlugin.DLL

#if !defined( __AFXCTL_H__ )
//#error incluez 'afxctl.h' avant d'inclure ce fichier
#endif

#include "resource.h"       // symboles principaux


// CHonsenPluginApp : consultez HonsenPlugin.cpp pour l'impl�mentation.

class CHonsenPluginApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;


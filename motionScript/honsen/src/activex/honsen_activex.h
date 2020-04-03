#pragma once

// honsen_activex.h : fichier d'en-tête principal pour honsen_activex.DLL

#if !defined( __AFXCTL_H__ )
#error incluez 'afxctl.h' avant d'inclure ce fichier
#endif

#include "resource.h"       // symboles principaux


// Chonsen_activexApp : consultez honsen_activex.cpp pour l'implémentation.

class Chonsen_activexApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;


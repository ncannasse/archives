// HonsenPlugin.cpp : implémentation de CHonsenPluginApp et inscription de DLL.
#define WINVER 0x4000
#include <afxctl.h>
#include "HonsenPlugin.h"
#include "../hsrun/hsthread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CHonsenPluginApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x958F3F6D, 0xEF8D, 0x4C52, { 0xA2, 0x8A, 0x54, 0xBF, 0x52, 0x6E, 0x68, 0x10 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

// CHonsenPluginApp::InitInstance - Initialisation de la DLL

BOOL CHonsenPluginApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		int size_path = 32767;
		char *path = new char[size_path];
		char mtpath[256];
		*mtpath = ';';
		GetSystemDirectory(mtpath+1, 256);
		strcpy(mtpath+strlen(mtpath),"\\mt\\honsen");
		GetEnvironmentVariable("PATH", path, size_path - strlen(mtpath) - 1);
		strcpy(path+strlen(path),mtpath);
		SetEnvironmentVariable("PATH",path);
		delete path;

		honsen_global_init();
	}

	return bInit;
}



// CHonsenPluginApp::ExitInstance - Fin de la DLL

int CHonsenPluginApp::ExitInstance()
{
	// TODO : ajoutez ici votre propre code d'arrêt de module.
	honsen_global_free();
	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Ajoute des entrées à la base de registres

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - Supprime les entrées de la base de registres

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
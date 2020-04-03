// honsen_activex.cpp : implémentation de Chonsen_activexApp et inscription de DLL.

#include "stdafx.h"
#include "honsen_activex.h"
#include "implementation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


Chonsen_activexApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x9ACC075A, 0xEDE1, 0x4B0E, { 0x8F, 0x4D, 0xD3, 0xAD, 0x89, 0x15, 0x3A, 0xBA } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// Chonsen_activexApp::InitInstance - Initialisation de la DLL

BOOL Chonsen_activexApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		honsen_plugin_init();
	}

	return bInit;
}



// Chonsen_activexApp::ExitInstance - Fin de la DLL

int Chonsen_activexApp::ExitInstance()
{
	honsen_plugin_close();
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

	// mark as safe for initialization
	HKEY k;
	RegOpenKey(HKEY_CLASSES_ROOT,"CLSID",&k);
	RegOpenKey(k,"{0F334BAB-D558-403A-9D71-6C6F65A4FEB3}",&k);
	RegCreateKey(k,"Implemented Categories",&k);
	RegCreateKey(k,"{7DD95802-9882-11CF-9FA9-00AA006C42C4}",&k);

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

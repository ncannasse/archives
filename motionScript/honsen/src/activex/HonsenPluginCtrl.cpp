// HonsenPluginCtrl.cpp : implémentation de la classe du contrôle ActiveX CHonsenPluginCtrl.

#include "stdafx.h"
#include "honsen_activex.h"
#include "HonsenPluginCtrl.h"
#include "HonsenPluginPpg.h"
#include "implementation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CHonsenPluginCtrl, COleControl)



// Table des messages

BEGIN_MESSAGE_MAP(CHonsenPluginCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()



// Table de dispatch

BEGIN_DISPATCH_MAP(CHonsenPluginCtrl, COleControl)
	DISP_PROPERTY(CHonsenPluginCtrl, "url", url, VT_BSTR)
END_DISPATCH_MAP()



// Table d'événement

BEGIN_EVENT_MAP(CHonsenPluginCtrl, COleControl)
END_EVENT_MAP()



// Pages de propriétés

// TODO : ajoutez autant de pages de propriétés que nécessaire. N'oubliez pas d'augmenter le compteur !
BEGIN_PROPPAGEIDS(CHonsenPluginCtrl, 1)
	PROPPAGEID(CHonsenPluginPropPage::guid)
END_PROPPAGEIDS(CHonsenPluginCtrl)



// Initialisation de la fabrique de classe et du guid

IMPLEMENT_OLECREATE_EX(CHonsenPluginCtrl, "HONSEN.HonsenPluginCtrl.1",
	0xf334bab, 0xd558, 0x403a, 0x9d, 0x71, 0x6c, 0x6f, 0x65, 0xa4, 0xfe, 0xb3)



// ID et version de bibliothèque de types

IMPLEMENT_OLETYPELIB(CHonsenPluginCtrl, _tlid, _wVerMajor, _wVerMinor)



// ID d'interface

const IID BASED_CODE IID_Dhonsen_activex =
		{ 0xFC55D090, 0x25DB, 0x425A, { 0xB4, 0x2F, 0x50, 0x74, 0xC6, 0x19, 0xBC, 0xCF } };
const IID BASED_CODE IID_Dhonsen_activexEvents =
		{ 0xD16FC0AD, 0xE137, 0x4A09, { 0xA7, 0xD2, 0x80, 0xB3, 0x9B, 0x4E, 0xAA, 0xD5 } };



// Informations de type du contrôle

static const DWORD BASED_CODE _dwhonsen_activexOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CHonsenPluginCtrl, IDS_HONSEN_ACTIVEX, _dwhonsen_activexOleMisc)



// CHonsenPluginCtrl::CHonsenPluginCtrlFactory::UpdateRegistry -
// Ajoute ou supprime les entrées de la base de registres pour CHonsenPluginCtrl

BOOL CHonsenPluginCtrl::CHonsenPluginCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO : vérifiez que votre contrôle suit les règles du modèle de thread apartment.
	// Reportez-vous à MFC TechNote 64 pour plus d'informations.
	// Si votre contrôle ne se conforme pas aux règles du modèle apartment, vous
	// devez modifier le code ci-dessous, en modifiant le 6è paramètre de
	// afxRegApartmentThreading en 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_HONSEN_ACTIVEX,
			IDB_HONSEN_ACTIVEX,
			afxRegApartmentThreading,
			_dwhonsen_activexOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CHonsenPluginCtrl::CHonsenPluginCtrl - Constructeur

CHonsenPluginCtrl::CHonsenPluginCtrl()
{
	InitializeIIDs(&IID_Dhonsen_activex, &IID_Dhonsen_activexEvents);
	InternalSetReadyState(READYSTATE_UNINITIALIZED);
	impl = NULL;
}



// CHonsenPluginCtrl::~CHonsenPluginCtrl - Destructeur

CHonsenPluginCtrl::~CHonsenPluginCtrl()
{
	if( impl != NULL ) {
		::KillTimer(m_hWnd,timerid);
		implementation_close(impl);
	}
}



// CHonsenPluginCtrl::OnDraw - Fonction de dessin

void CHonsenPluginCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if( m_hWnd == NULL ) {
		if( impl == NULL || GetReadyState() == READYSTATE_COMPLETE )
			Invalidate(FALSE);
		return;
	}
	bool refresh = true;
	if( impl == NULL ) {
		if( !url.IsEmpty() ) {
			timerid = ::SetTimer(m_hWnd,1,500,NULL);
			impl = implementation_init(this,url);
			::SetFocus(m_hWnd);
		}
	} else
		refresh = implementation_main(impl,m_hWnd);
	if( refresh )
		Invalidate(FALSE);
}

// CHonsenPluginCtrl::DoPropExchange - Prise en charge de la persistance

void CHonsenPluginCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
	PX_String(pPX, _T("url"), url);
}

// CHonsenPluginCtrl::GetControlFlags -
// Indicateurs permettant de personnaliser l'implémentation MFC des contrôles ActiveX.
//
DWORD CHonsenPluginCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// Ce contrôle peut recevoir des notifications de la souris lorsqu'il est inactif.
	// TODO : si vous écrivez des gestionnaires pour WM_SETCURSOR et WM_MOUSEMOVE,
	//		n'utilisez pas la variable de membre m_hWnd sans contrôler au préalable
	//		que sa valeur n'est pas NULL.
	dwFlags |= pointerInactive;
	return dwFlags;
}



// CHonsenPluginCtrl::OnResetState - Réinitialise le contrôle à son état par défaut

void CHonsenPluginCtrl::OnResetState()
{
	COleControl::OnResetState();  // Réinitialise les valeurs par défaut trouvées dans DoPropExchange

	// TODO : réinitialisez ici les autres valeurs d'état du contrôle.
}

LRESULT CHonsenPluginCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	if( impl != NULL )
		switch( message ) {
		case WM_KILLFOCUS:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_MOUSEMOVE:
			implementation_event(impl,message,wParam,lParam);
			break;
		case WM_TIMER:
			implementation_main(impl,m_hWnd);
			break;
		}
	return COleControl::WindowProc(message, wParam, lParam);
}

BOOL CHonsenPluginCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if( impl != NULL )
		implementation_event(impl,pMsg->message,pMsg->wParam,pMsg->lParam);
	return TRUE;
}

// Gestionnaires de messages CHonsenPluginCtrl


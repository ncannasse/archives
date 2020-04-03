// HonsenPluginCtrl.cpp : impl�mentation de la classe du contr�le ActiveX CHonsenPluginCtrl.
#define WINVER 0x4000
#include <afxctl.h>
#include "HonsenPlugin.h"
#include "HonsenPluginCtrl.h"
#include "HonsenPluginPropPage.h"
#include "../hsrun/hsthread.h"
#include "../hslib/hslib.h"
#include "../common/mtw.h"


#include "HonsenDataPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHonsenPluginCtrl, COleControl)

// Table des messages

BEGIN_MESSAGE_MAP(CHonsenPluginCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// Table de dispatch

BEGIN_DISPATCH_MAP(CHonsenPluginCtrl, COleControl)
	DISP_FUNCTION_ID(CHonsenPluginCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY(CHonsenPluginCtrl, "honsenURL", sc, VT_BSTR)
END_DISPATCH_MAP()

// Table d'�v�nement

BEGIN_EVENT_MAP(CHonsenPluginCtrl, COleControl)
END_EVENT_MAP()

// Pages de propri�t�s

// TODO�: ajoutez autant de pages de propri�t�s que n�cessaire. N'oubliez pas d'augmenter le compteur�!
BEGIN_PROPPAGEIDS(CHonsenPluginCtrl, 1)
	PROPPAGEID(CHonsenPluginPropPage::guid)
END_PROPPAGEIDS(CHonsenPluginCtrl)



// Initialisation de la fabrique de classe et du guid

IMPLEMENT_OLECREATE_EX(CHonsenPluginCtrl, "ACTIVEXTEST.HonsenPluginCtrl.1",
	0x7e47c3f9, 0xb809, 0x48e2, 0x88, 0xbf, 0x74, 0x5d, 0xed, 0x4c, 0x64, 0x14)


// ID et version de biblioth�que de types

IMPLEMENT_OLETYPELIB(CHonsenPluginCtrl, _tlid, _wVerMajor, _wVerMinor)


// ID d'interface

const IID BASED_CODE IID_DHonsenPlugin =
		{ 0x8D62FB73, 0xABE7, 0x4FBB, { 0xA3, 0x2C, 0xB6, 0xA3, 0x47, 0xFF, 0xF9, 0x47 } };
const IID BASED_CODE IID_DHonsenPluginEvents =
		{ 0x7A81ABDA, 0x7A1C, 0x489D, { 0xA9, 0x26, 0x64, 0x63, 0x74, 0x8A, 0xE3, 0x21 } };

// Informations de type du contr�le

static const DWORD BASED_CODE _dwHonsenPluginOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CHonsenPluginCtrl, IDS_ACTIVEXTEST, _dwHonsenPluginOleMisc)

// CHonsenPluginCtrl::CHonsenPluginCtrlFactory::UpdateRegistry -
// Ajoute ou supprime les entr�es de la base de registres pour CHonsenPluginCtrl

BOOL CHonsenPluginCtrl::CHonsenPluginCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO�: v�rifiez que votre contr�le suit les r�gles du mod�le de thread apartment.
	// Reportez-vous � MFC TechNote 64 pour plus d'informations.
	// Si votre contr�le ne se conforme pas aux r�gles du mod�le apartment, vous
	// devez modifier le code ci-dessous, en modifiant le 6� param�tre de
	// afxRegApartmentThreading en 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_ACTIVEXTEST,
			IDB_ACTIVEXTEST,
			afxRegApartmentThreading,
			_dwHonsenPluginOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

CHonsenPluginCtrl::CHonsenPluginCtrl()
{
	InitializeIIDs(&IID_DHonsenPlugin, &IID_DHonsenPluginEvents);
	// TODO�: initialisez ici les donn�es de l'instance de votre contr�le.
	InternalSetReadyState(READYSTATE_UNINITIALIZED);
	threadParams = new Params;
	started = false;
	loadingStarted = false;
	inLoading = false;
	hdp = NULL;
}

// CHonsenPluginCtrl::~CHonsenPluginCtrl - Destructeur

CHonsenPluginCtrl::~CHonsenPluginCtrl()
{
	// TODO�: nettoyez ici les donn�es de l'instance de votre contr�le.
	honsen_stop_thread(threadID);
	delete threadParams;
	if( hdp ) {
		hdp->Close();
		delete hdp;
	}
}

void CallbackUpdateProgress( void *pctrl ) {
	((CHonsenPluginCtrl*)pctrl)->UpdateProgress();
}

void CHonsenPluginCtrl::UpdateProgress() {
	inLoading = true;
	HSLib::SetProgress(hdp->progress);
}

// CHonsenPluginCtrl::OnDraw - Fonction de dessin

void CHonsenPluginCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if(!loadingStarted && !sc.IsEmpty()) {
		if(!hdp){
			hdp = new HonsenDataPath(this, sc);
			InternalSetReadyState(READYSTATE_LOADING);
			hdp->Open();
		}
		if(hdp->started && !loadingStarted){
			loadingStarted = true;
			threadParams->filename = sc;
			threadParams->hs = NULL;
			threadParams->hwnd = m_hWnd;
			threadParams->callback = CallbackUpdateProgress;
			threadParams->cbparam = this;
			threadID = honsen_start_thread(threadParams);
		}
	}
	if( loadingStarted && inLoading && !started && !sc.IsEmpty() && GetReadyState() == READYSTATE_COMPLETE ) {
		int length = (int)hdp->GetLength();
		char *buf = new char[length];
		hdp->Read(buf, length);
		threadParams->hs = MTW::ReadData(buf,length);
		threadParams->callback = NULL;
		threadParams->cbparam = NULL;
		delete buf;
		started = true;
		PostThreadMessage( threadID, WM_DESTROY, 0, 0);		
	} else if( !started )
		Invalidate(TRUE);
}

// CHonsenPluginCtrl::DoPropExchange - Prise en charge de la persistance

void CHonsenPluginCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO�: appelez les fonctions PX_ pour chaque propri�t� personnalis�e persistante.
	PX_String(pPX, _T("honsenURL"), sc);
}

// CHonsenPluginCtrl::OnResetState - R�initialise le contr�le � son �tat par d�faut

void CHonsenPluginCtrl::OnResetState()
{
	COleControl::OnResetState();  // R�initialise les valeurs par d�faut trouv�es dans DoPropExchange

	// TODO�: r�initialisez ici les autres valeurs d'�tat du contr�le.
}

// CHonsenPluginCtrl::AboutBox - Affiche une bo�te de dialogue "� propos de" � l'utilisateur

void CHonsenPluginCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_ACTIVEXTEST);
	dlgAbout.DoModal();
}

// Gestionnaires de messages CHonsenPluginCtrl

LRESULT CHonsenPluginCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_KILLFOCUS || message == WM_DESTROY)
		PostThreadMessage( threadID, message, 0, 0 );
	return COleControl::WindowProc(message, wParam, lParam);
}

void CHonsenPluginCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	::SetCapture(m_hWnd);
	PostThreadMessage( threadID, WM_LBUTTONDOWN, nFlags, 0 );
	COleControl::OnLButtonDown(nFlags, point);
}

void CHonsenPluginCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	::ReleaseCapture();
	PostThreadMessage( threadID, WM_LBUTTONUP, nFlags, 0 );
	COleControl::OnLButtonUp(nFlags, point);
}

void CHonsenPluginCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	::SetCapture(m_hWnd);
	PostThreadMessage( threadID, WM_RBUTTONDOWN, nFlags, 0 );
	COleControl::OnRButtonDown(nFlags, point);
}

void CHonsenPluginCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	::ReleaseCapture();
	PostThreadMessage( threadID, WM_RBUTTONUP, nFlags, 0 );
	COleControl::OnRButtonUp(nFlags, point);
}

void CHonsenPluginCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	::SetCapture(m_hWnd);
	PostThreadMessage( threadID, WM_MBUTTONDOWN, nFlags, 0 );
	COleControl::OnMButtonDown(nFlags, point);
}

void CHonsenPluginCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	::ReleaseCapture();
	PostThreadMessage( threadID, WM_MBUTTONUP, nFlags, 0 );
	COleControl::OnMButtonUp(nFlags, point);
}

BOOL CHonsenPluginCtrl::PreTranslateMessage(MSG* pMsg) 
{
	PostThreadMessage( threadID, pMsg->message, pMsg->wParam, pMsg->lParam );
	return TRUE;
}


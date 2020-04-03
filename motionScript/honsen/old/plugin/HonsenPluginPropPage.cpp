// HonsenPluginPropPage.cpp : impl�mentation de la classe de la page de propri�t�s CHonsenPluginPropPage.
#define WINVER 0x4000
#include <afxctl.h>
#include "HonsenPlugin.h"
#include "HonsenPluginPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CHonsenPluginPropPage, COlePropertyPage)



// Table des messages

BEGIN_MESSAGE_MAP(CHonsenPluginPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialisation de la fabrique de classe et du guid

IMPLEMENT_OLECREATE_EX(CHonsenPluginPropPage, "ACTIVEXTEST.HonsenPluginPropPage.1",
	0x6a9d06b7, 0x11e9, 0x4249, 0xa8, 0x1e, 0xa7, 0x56, 0xcd, 0xb9, 0xb9, 0xb4)



// CHonsenPluginPropPage::CHonsenPluginPropPageFactory::UpdateRegistry -
// Ajoute ou supprime des entr�es de la base de registres pour CHonsenPluginPropPage

BOOL CHonsenPluginPropPage::CHonsenPluginPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_ACTIVEXTEST_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CHonsenPluginPropPage::CHonsenPluginPropPage - Constructeur

CHonsenPluginPropPage::CHonsenPluginPropPage() :
	COlePropertyPage(IDD, IDS_ACTIVEXTEST_PPG_CAPTION)
{
}



// CHonsenPluginPropPage::DoDataExchange - Transf�re les donn�es entre la page et les propri�t�s

void CHonsenPluginPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// Gestionnaires de messages CHonsenPluginPropPage

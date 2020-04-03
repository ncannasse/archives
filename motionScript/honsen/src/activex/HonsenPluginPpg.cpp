// HonsenPluginPpg.cpp : impl�mentation de la classe de la page de propri�t�s CHonsenPluginPropPage.

#include "stdafx.h"
#include "honsen_activex.h"
#include "HonsenPluginPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CHonsenPluginPropPage, COlePropertyPage)



// Table des messages

BEGIN_MESSAGE_MAP(CHonsenPluginPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialisation de la fabrique de classe et du guid

IMPLEMENT_OLECREATE_EX(CHonsenPluginPropPage, "HONSEN.HonsenPluginPropPage.1",
	0xb5af852e, 0x8877, 0x4ba2, 0x8e, 0x80, 0x78, 0xf1, 0x63, 0x2e, 0x8c, 0x5f)



// CHonsenPluginPropPage::CHonsenPluginPropPageFactory::UpdateRegistry -
// Ajoute ou supprime des entr�es de la base de registres pour CHonsenPluginPropPage

BOOL CHonsenPluginPropPage::CHonsenPluginPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_HONSEN_ACTIVEX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CHonsenPluginPropPage::CHonsenPluginPropPage - Constructeur

CHonsenPluginPropPage::CHonsenPluginPropPage() :
	COlePropertyPage(IDD, IDS_HONSEN_ACTIVEX_PPG_CAPTION)
{
}



// CHonsenPluginPropPage::DoDataExchange - Transf�re les donn�es entre la page et les propri�t�s

void CHonsenPluginPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// Gestionnaires de messages CHonsenPluginPropPage

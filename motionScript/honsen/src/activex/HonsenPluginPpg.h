#pragma once

// HonsenPluginPpg.h : déclaration de la classe de page de propriétés CHonsenPluginPropPage.


// CHonsenPluginPropPage : consultez HonsenPluginPpg.cpp pour l'implémentation.

class CHonsenPluginPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CHonsenPluginPropPage)
	DECLARE_OLECREATE_EX(CHonsenPluginPropPage)

// Constructeur
public:
	CHonsenPluginPropPage();

// Données de boîte de dialogue
	enum { IDD = IDD_PROPPAGE_HONSEN_ACTIVEX };

// Implémentation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

// Tables des messages
protected:
	DECLARE_MESSAGE_MAP()
};


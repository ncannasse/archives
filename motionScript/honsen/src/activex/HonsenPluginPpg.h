#pragma once

// HonsenPluginPpg.h : d�claration de la classe de page de propri�t�s CHonsenPluginPropPage.


// CHonsenPluginPropPage : consultez HonsenPluginPpg.cpp pour l'impl�mentation.

class CHonsenPluginPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CHonsenPluginPropPage)
	DECLARE_OLECREATE_EX(CHonsenPluginPropPage)

// Constructeur
public:
	CHonsenPluginPropPage();

// Donn�es de bo�te de dialogue
	enum { IDD = IDD_PROPPAGE_HONSEN_ACTIVEX };

// Impl�mentation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

// Tables des messages
protected:
	DECLARE_MESSAGE_MAP()
};


#pragma once

// HonsenPluginCtrl.h : déclaration de la classe du contrôle ActiveX CHonsenPluginCtrl.

struct Impl;

// CHonsenPluginCtrl : consultez HonsenPluginCtrl.cpp pour l'implémentation.

class CHonsenPluginCtrl : public COleControl
{
	DECLARE_DYNCREATE(CHonsenPluginCtrl)
	Impl *impl;
	int timerid;
	CString url;
// Constructeur
public:
	CHonsenPluginCtrl();

// Overrides
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();

// Implémentation
protected:
	~CHonsenPluginCtrl();

	DECLARE_OLECREATE_EX(CHonsenPluginCtrl)    // Fabrique de classe et guid
	DECLARE_OLETYPELIB(CHonsenPluginCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CHonsenPluginCtrl)     // ID de page de propriétés
	DECLARE_OLECTLTYPE(CHonsenPluginCtrl)		// Nom de type et état divers

// Tables des messages
	DECLARE_MESSAGE_MAP()

// Tables de dispatch
	DECLARE_DISPATCH_MAP()

// Tables d'événements
	DECLARE_EVENT_MAP()

// ID de dispatch et d'événement
public:
	enum {
	};
};


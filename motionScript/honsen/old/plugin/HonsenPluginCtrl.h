#pragma once

// HonsenPluginCtrl.h : déclaration de la classe du contrôle ActiveX CHonsenPluginCtrl.


// CHonsenPluginCtrl : consultez HonsenPluginCtrl.cpp pour l'implémentation.

#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <Mmsystem.h>

class HonsenDataPath;
struct Params;

class CHonsenPluginCtrl : public COleControl
{
	DECLARE_DYNCREATE(CHonsenPluginCtrl)

// Constructeur
public:
	CHonsenPluginCtrl();

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void UpdateProgress();

// Implémentation
protected:
	~CHonsenPluginCtrl();
	//CRefresh *m_refresher;
	CString sc;
	DWORD threadID;
	bool started;
	bool inLoading;
	bool loadingStarted;
	HonsenDataPath *hdp;
	Params *threadParams;

	DECLARE_OLECREATE_EX(CHonsenPluginCtrl)    // Fabrique de classe et guid
	DECLARE_OLETYPELIB(CHonsenPluginCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CHonsenPluginCtrl)     // ID de page de propriétés
	DECLARE_OLECTLTYPE(CHonsenPluginCtrl)		// Nom de type et état divers

// Message maps
	//{{AFX_MSG(CBlenderPlayerCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Tables de dispatch
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Tables d'événements
	DECLARE_EVENT_MAP()

// ID de dispatch et d'événement
public:
	enum {
	};
};


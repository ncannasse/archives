 /* *********************************************************************** */
// plugin.h - all classes
/* *********************************************************************** */
#pragma once

#define CODE	0xDAFF00CE
#ifndef DRAGOON
#include "DString.h"
#endif
/* *********************************************************************** */
#define PosPlugObj(obj,left,top,width,height) { obj->Left = left; obj->Top = top; obj->Width = width; obj->Height = height; }
/* *********************************************************************** */
#define LABEL	0
#define	BUTTON	1
#define EDIT	2
#define LIST	3
#define GRID	4
#define COMBO	5
#define SBUTTON 6
#define MENUIT  7
/* *********************************************************************** */
#define EVT_LCLICK 1
#define EVT_RCLICK 2
#define EVT_DCLICK 3
#define EVT_KPRESS 4
#define EVT_CHANGE 5
#define EVT_PRJLOAD 6
#define EVT_PRJSAVE 7
#define EVT_PRJINIT 8
#define EVT_PAKCHANGE 9
#define EVT_RENAME	10
#define EVT_DELETE	11
#define EVT_LOAD	12
#define EVT_SAVE	13
#define EVT_DROP	14
/* *********************************************************************** */
#define F_WIDTH		0
#define F_TOP		1
#define F_LEFT		2
#define F_HEIGHT	3
#define F_HINT		4
#define F_CAPTION	5
#define F_VISIBLE	6
#define F_ENABLED	7
#define F_CHECKED	8
#define F_ICON		9
#define F_PARENT	10
#define F_TEXT		11

#define F_ITEM		12
#define F_ADDITEM	13
#define F_DELITEM	14
#define F_ITEMCOUNT	15
#define F_INDEX		16
#define F_SETINDEX	17
#define F_TOPINDEX	18
#define F_CLEAR		19

#define F_COLCOUNT	20
#define F_ROWCOUNT	21
#define F_FIXCOLS	22
#define F_FIXROWS	23
#define F_SELECTION	24
#define F_GRIDITEM	25
#define F_COLWIDTH	26
#define F_GRIDOPT	27
#define F_COLWIDTHS	28
#define F_ROWHEIGHT	29

/* *********************************************************************** */
typedef bool (*PLUGPROC) ( void * = NULL, void * = NULL, void * = NULL, void * = NULL );
typedef void * OBJHANDLE;
typedef void * NOTUSED;
/* *********************************************************************** */

enum GOPTIONS {
	FIXED_VLINE = 1,
	FIXED_HLINE = 2,
	DRAW_VLINE = 4,
	DRAW_HLINE = 8,
	RANGE_SEL = 16,
	ROW_SIZING = 32,
	COL_SIZING = 64,
	CANEDIT = 128
};

/* *********************************************************************** */

typedef struct TPlugInfo {

	DWORD	Code;
	char*	Name;
	char*	Author;
	char*	Infos;
	HANDLE	Handle;
	void	*PluginInst;
	PLUGPROC MainEvents;
	char*	RegBalise;
	HBITMAP	HIcon;
	char	Reserved;

} TPlugInfo, *LPPlugInfo;

/* *********************************************************************** */

typedef struct TActions {

	PLUGPROC	LogTxt;			/* ( Txt, NULL, NULL, NULL )-> void						*/
	PLUGPROC	RunPlugin;		/* ( Name, Function, Param, NULL )-> success			*/
	PLUGPROC	RegisterButton; /* ( Plugin, &OBJHANDLE, PLUGPROC, Class )-> success	*/
	PLUGPROC	RegisterMenu;	/* ( Plugin, &OBJHANDLE, PLUGPROC, Class )-> success	*/

	PLUGPROC	CreateObj;		/* ( Plugin, TYPE, &OBJHANDLE, NOTUSED )-> success	*/
	PLUGPROC	SetEventFunc;	/* ( Plugin, OBJHANDLE, PLUGPROC, Class )			*/
	PLUGPROC	DeleteObj;		/* ( Plugin, OBJHANDLE, NULL, NULL )-> exists		*/

	PLUGPROC	SetField;		/* ( Plugin, OBJHANDLE, value *, Field )-> exists	(if Plugin == NULL, do GET) */

	PLUGPROC	PlugFormVis;    /* ( Plugin, bool *state )-> sucesss (if Plugin == NULL, do GET) */
	PLUGPROC	RunPAKBrowser;	/* ( Name, NULL, NULL, NULL )->success					*/
	PLUGPROC	CnvIDToFileName;/* ( &ID, buf, NULL, NULL )->success					*/
	PLUGPROC	InputText;		/* ( buf, Title, Default, NULL )->success				*/
	PLUGPROC	CreateItem;		/* ( Plugin, Name, NULL, NULL )->success				*/

	PLUGPROC	LoadFile;		/* ( Filemask, Title, buf, NULL )->success				*/
	PLUGPROC	SaveFile;		/* ( Filemask, Title, buf, NULL )->success				*/

} TActions, *LPActions;

/* *********************************************************************** */


#ifndef DRAGOON

class CPlugObj {

public:

	__declspec( property( get=GetTop,put=SetTop ) )			int  Top;
	__declspec( property( get=GetLeft,put=SetLeft ) )		int  Left;
	__declspec( property( get=GetWidth,put=SetWidth ) )		int  Width;
	__declspec( property( get=GetHeight,put=SetHeight ) )	int  Height;
	__declspec( property( get=GetEnabled,put=SetEnabled ) )	bool Enabled;
	__declspec( property( get=GetVisible,put=SetVisible ) )	bool Visible;

	virtual void SetEnabled( bool state );
	virtual void SetVisible( bool state );
	virtual void SetHeight( int H );
	virtual void SetWidth( int W );
	virtual void SetLeft( int L );
	virtual void SetTop( int T );

	virtual bool GetEnabled();
	virtual bool GetVisible();
	virtual int  GetHeight();
	virtual int  GetWidth();
	virtual int  GetLeft();
	virtual int  GetTop();

protected:

	OBJHANDLE Obj;
	CPlugObj();
	~CPlugObj();

};


/* *********************************************************************** */
typedef CPlugObj *OBJECT;
typedef bool (*PLUGEVENT)( OBJECT Sender );
typedef bool (*PLUGEVENTSTRING) ( LPCSTR Param );
typedef bool (*PLUGEVENTSTRINGEX) ( LPCSTR Param, LPCSTR Extend );
typedef bool (*PLUGEVENTEX)( OBJECT Sender, void *Param );
/* *********************************************************************** */


class CDragoonButton : public CPlugObj {

public:

	CDragoonButton();
	~CDragoonButton();

	__declspec( property( get=GetHint,put=SetHint ) )	DString  Hint;
	__declspec( property( get=GetIcon,put=SetIcon ) )	HBITMAP Icon;

	PLUGEVENT OnLClick;
	PLUGEVENT OnRClick;

	void SetHint( DString h	);
	void SetIcon( HBITMAP Ico	);
	DString GetHint();
	HBITMAP GetIcon();

	void SetHeight( int ) { };
	void SetWidth( int ) { };
	void SetLeft( int ) { };
	void SetTop( int ) { };

private:

	static bool OnEvent( void *Sender, void *EventType, void *Class, NOTUSED );

};

typedef CDragoonButton *DragoonButton;

/* *********************************************************************** */

class CDragoonMenu {

public:
	CDragoonMenu();
	~CDragoonMenu();

	__declspec( property( get=GetCaption,put=SetCaption ) )	DString  Caption;
	__declspec( property( get=GetChecked,put=SetChecked ) )	bool	Checked;
	__declspec( property( get=GetEnabled,put=SetEnabled ) )	bool	Enabled;
	__declspec( property( get=GetVisible,put=SetVisible ) )	bool	Visible;
	__declspec( property( put=SetParent ) )	CDragoonMenu *Parent;

	virtual void SetEnabled( bool state );
	virtual void SetVisible( bool state );
	virtual void SetChecked( bool state );
	virtual bool GetEnabled();
	virtual bool GetVisible();
	virtual bool GetChecked();

	virtual void SetCaption( DString Txt );
	virtual void SetParent( CDragoonMenu *Par );

	PLUGEVENT OnClick;

private:

	static bool OnEvent( void *Sender, void *EventType, void *Class, NOTUSED );
	OBJHANDLE Obj;
};

typedef CDragoonMenu *DragoonMenu;

/* *********************************************************************** */

class CLabel:public CPlugObj {

public:


	CLabel();
	~CLabel();

	__declspec( property( get = GetCaption,put=SetCaption ) ) DString Caption;
	
	PLUGEVENT	OnLClick;
	PLUGEVENT	OnRClick;
	PLUGEVENT	OnDblClick;

	void SetCaption( DString Text );
	DString GetCaption();

private:

	static bool OnEvent( void *Sender, void *EventType, void *Class, NOTUSED );

};

typedef CLabel *Label;

/* *********************************************************************** */

class CButton : public CPlugObj {

public:


	CButton();
	~CButton();

	__declspec( property( get = GetCaption,put=SetCaption ) ) DString Caption;
	
	PLUGEVENT	OnClick;
	PLUGEVENT	OnRClick;

	void SetCaption( DString Text );
	DString GetCaption();

private:

	static bool OnEvent( void *Sender, void *EventType, void *Class, NOTUSED );

};

typedef CButton *Button;

/* *********************************************************************** */

class CEdit:public CPlugObj {

public:


	CEdit();
	~CEdit();

	__declspec( property( get = GetText,put=SetText ) )			  DString Text;
	
	PLUGEVENT	OnChange;
	PLUGEVENTEX	OnKeyPress;

	void SetText( DString Text );
	DString GetText();

private:

	static bool OnEvent( void *Sender, void *EventType, void *Class, NOTUSED );

};

typedef CEdit *Edit;

/* *********************************************************************** */

class CCombo:public CPlugObj {


public:

	CCombo();
	~CCombo();

	__declspec( property( get = GetItem,put=SetItem ) )			DString  Items[];
	__declspec( property( get = GetIndex,put=SetIndex ) )		int ItemIndex;
	__declspec( property( get = GetCount ) )					int ItemCount;


	PLUGEVENT	OnChange;

	void AddItem( DString ItemText );
	void DelItem( int Index );

	DString GetItem	( int Index );
	void SetItem	( int Index, DString val );
	int  GetCount	( void );
	int  GetIndex	( void );
	void SetIndex	( int Index );

private:

	static bool OnEvent( void *Sender, void *EventType, void *Class, NOTUSED );

};

typedef CCombo *Combo;

/* *********************************************************************** */

class CList:public CPlugObj {


public:

	CList();
	~CList();

	__declspec( property( get = GetItem,put=SetItem ) )			DString Items[];
	__declspec( property( get = GetIndex,put=SetIndex ) )		int ItemIndex;
	__declspec( property( get = GetTopIndex,put=SetTopIndex ) )	int TopIndex;
	__declspec( property( get = GetCount ) )					int ItemCount;


	PLUGEVENT	OnClick;
	PLUGEVENT	OnRClick;
	PLUGEVENT	OnDblClick;
	PLUGEVENTEX	OnKeyPressed;

	void AddItem( DString ItemText );
	void DelItem( int Index );
	void Clear	( void );

	DString GetItem	( int Index );
	void SetItem	( int Index, DString val );
	int  GetCount	( void );
	int  GetIndex	( void );
	void SetIndex	( int Index );
	int  GetTopIndex( void );
	void SetTopIndex( int Index );

private:

	static bool OnEvent( void *Sender, void *EventType, void *Class, NOTUSED );

};

typedef CList *List;

/* *********************************************************************** */

class CGrid:public CPlugObj {


public:


	CGrid();
	~CGrid();

	__declspec( property( get = GetCell,put=SetCell ) )			DString Cells[][];
	__declspec( property( get = GetSel,put=SetSel ) )			RECT	Selection;
	__declspec( property( get = GetColCount,put=SetColCount ) )	int		ColCount;
	__declspec( property( get = GetRowCount,put=SetRowCount ) )	int		RowCount;
	__declspec( property( get = GetFixedCols,put=SetFixedCols ))int		FixedCols;
	__declspec( property( get = GetFixedRows,put=SetFixedRows ))int		FixedRows;
	__declspec( property( get = GetFlags,put=SetFlags ) )		BYTE	Flags;

	__declspec( property( get = GetColWidth,put=SetColWidth ) )	int		ColWidths[];
	__declspec( property( get = GetRowHeight,put=SetRowHeight ))int		RowHeight;


	PLUGEVENT	OnClick;
	PLUGEVENT	OnRClick;
	PLUGEVENT	OnDblClick;
	PLUGEVENTEX	OnKeyPressed;

	DString GetCell	( int x, int y );
	void	SetCell	( int x, int y, DString val );
	RECT	GetSel	( void );
	void	SetSel	( RECT RSelect );
	int		GetRowCount();
	int		GetColCount();
	void	SetRowCount(int);
	void	SetColCount(int);
	int		GetFixedCols();
	void	SetFixedCols(int);
	int		GetFixedRows();
	void	SetFixedRows(int);
	BYTE	GetFlags();
	void	SetFlags( BYTE Fl );
	int		GetRowHeight();
	void	SetRowHeight(int);
	int		GetColWidth( int index );
	void	SetColWidth( int index, int val );


private:

	static bool OnEvent( void *Sender, void *EventType, void *Class, NOTUSED );

};

typedef CGrid *Grid;

/* *********************************************************************** */

class CPlugin {

public:

	CPlugin			( HANDLE h	);
	~CPlugin		( void		);

	// Init Functions
	void RegisterName	( DString PluginName	);
	void RegisterAuthor	( DString PluginAuthor	);
	void RegisterInfos	( DString Infos			);
	
	// Retreive Functions
	HANDLE	GetHandle	( void );
	DString	CnvIDToFile	( DWORD ID );

	// I/O Functions
	bool	CreateItem	( DString ItemName );
	bool	RunPlugin	( DString Nom, DString Function, void *Param );
	void	LogText		( DString Txt );
	bool	InputText	( DString Title, DString Default, DString *out );

	bool	LoadFileQuery( DString FileMask, DString Title, DString *out );
	bool	SaveFileQuery( DString FileMask, DString Title, DString *out );
	
	
	// Objects Functions
	void	ShowPlugForm( void );
	void	HidePlugForm( void );
	void	RunPAKBrowser( DString PakName );


	// Global Events
	PLUGEVENT	Init;

	PLUGEVENTSTRING	OnPAKChange;
	PLUGEVENTSTRING	OnProjectInit;
	PLUGEVENTSTRING	OnProjectLoad;
	PLUGEVENTSTRING	OnProjectSave;

	// ItemType Events
	DString Balise;
	HBITMAP Icon;

	PLUGEVENTSTRING OnClick;
	PLUGEVENTSTRING OnRClick;
	PLUGEVENTSTRING OnDblClick;

	PLUGEVENTSTRINGEX	OnRename;
	PLUGEVENTSTRING		OnDelete;

	PLUGEVENTSTRINGEX	OnSave;
	PLUGEVENTSTRING		OnLoad;
	PLUGEVENTSTRINGEX	OnDrop;


private:

	LPActions Act;
	LPPlugInfo Inf;

	static bool OnEvent(void *EventType, void *param, void *, void * );
	static bool DragoonEntryPoint( LPPlugInfo Inf, LPActions Act );

	friend CPlugObj;
	friend CDragoonButton;
	friend CDragoonMenu;
	friend CLabel;
	friend CButton;
	friend CEdit;
	friend CCombo;
	friend CList;
	friend CGrid;

};

/* *********************************************************************** */

extern CPlugin *Plugin;

#endif
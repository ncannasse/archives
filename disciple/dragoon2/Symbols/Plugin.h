 /* *********************************************************************** */
// plugin.h - all classes
/* *********************************************************************** */
#pragma once
/* *********************************************************************** */
#include "DString.h"
/* *********************************************************************** */
typedef void * OBJHANDLE;
typedef void * NOTUSED;
/* *********************************************************************** */
#define PosPlugObj(obj,left,top,width,height) { obj->Left = left; obj->Top = top; obj->Width = width; obj->Height = height; }
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

	DWORD Reserved[24];
};

/* *********************************************************************** */

extern CPlugin *Plugin;


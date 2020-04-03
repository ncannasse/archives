#include <windows.h>
#include "pluginclass.h"


CGrid::CGrid()
{
	Plugin->Act->CreateObj( Plugin, (void *)GRID, &Obj );
	Plugin->Act->SetEventFunc( Plugin, Obj, OnEvent, this );
	Height = 300;
	Width = 100;
	Left = 0;
	Top = 0;
	OnClick = OnRClick = OnDblClick = NULL;
	OnKeyPressed = NULL;
}

CGrid::~CGrid()
{
	Plugin->Act->DeleteObj( Plugin, Obj );
}

BYTE
CGrid::GetFlags()
{
	int fl;
	Plugin->Act->SetField( NULL, Obj, &fl, (void*)F_GRIDOPT );
	return (BYTE)fl;
}

void
CGrid::SetFlags( BYTE flags )
{
	int i = flags;
	Plugin->Act->SetField( Plugin, Obj, &i, (void*)F_GRIDOPT );
}


DString 
CGrid::GetCell( int X, int Y )
{
	char buf[1024];
	int  p = Y+(X<<18);
	buf[0] = 0;
	Plugin->Act->SetField( NULL, Obj, &p, (void *)F_SETINDEX  );
	Plugin->Act->SetField( NULL, Obj, buf, (void *)F_GRIDITEM );
	return buf;
}

void
CGrid::SetCell( int X, int Y, DString val )
{
	int p = Y+(X<<18);
	Plugin->Act->SetField(Plugin, Obj, &p, (void *)F_SETINDEX );
	Plugin->Act->SetField(Plugin, Obj, val.c_str(), (void *)F_GRIDITEM );
}

int
CGrid::GetColWidth( int index )
{
	int r;
	Plugin->Act->SetField( NULL, Obj, &index, (void*)F_SETINDEX );
	Plugin->Act->SetField( NULL, Obj, &r, (void*)F_COLWIDTHS );
	return r;
}

void
CGrid::SetColWidth( int index, int r )
{
	Plugin->Act->SetField( Plugin, Obj, &index, (void*)F_SETINDEX );
	Plugin->Act->SetField( Plugin, Obj, &r, (void*)F_COLWIDTHS );
}

int
CGrid::GetRowHeight()
{
	int r;
	Plugin->Act->SetField( NULL, Obj, &r, (void*)F_ROWHEIGHT );
	return r;
}

void
CGrid::SetRowHeight( int r )
{
	Plugin->Act->SetField( Plugin, Obj, &r, (void*)F_ROWHEIGHT );
}

int
CGrid::GetRowCount()
{
	int r;
	Plugin->Act->SetField( NULL, Obj, &r, (void*)F_ROWCOUNT );
	return r;
}

void
CGrid::SetRowCount( int r )
{
	Plugin->Act->SetField( Plugin, Obj, &r, (void*)F_ROWCOUNT );
}

int
CGrid::GetColCount()
{
	int c;
	Plugin->Act->SetField( NULL, Obj, &c, (void*)F_COLCOUNT );
	return c;
}

void
CGrid::SetColCount( int c )
{
	Plugin->Act->SetField( Plugin, Obj, &c, (void*)F_COLCOUNT );
}

int
CGrid::GetFixedCols()
{
	int fc;
	Plugin->Act->SetField( NULL, Obj, &fc, (void*)F_FIXCOLS );
	return fc;
}

void
CGrid::SetFixedCols( int fc )
{
	Plugin->Act->SetField( Plugin, Obj, &fc, (void*)F_FIXCOLS );
}

int
CGrid::GetFixedRows()
{
	int fr;
	Plugin->Act->SetField( NULL, Obj, &fr, (void*)F_FIXROWS );
	return fr;
}

void
CGrid::SetFixedRows( int fr )
{
	Plugin->Act->SetField( Plugin, Obj, &fr, (void*)F_FIXROWS );
}

RECT
CGrid::GetSel()
{
	RECT r;
	Plugin->Act->SetField( NULL, Obj, &r, (void*)F_SELECTION );
	return r;
}

void
CGrid::SetSel( RECT r )
{
	Plugin->Act->SetField( Plugin, Obj, &r, (void*)F_SELECTION );
}


bool
CGrid::OnEvent( void *Sender, void *EventType, void *Class, void *p )
{
	Grid g = (Grid)Class;
	switch( (int)EventType )
	{
	case EVT_LCLICK:
		if( g->OnClick != NULL )
			return g->OnClick( g );
		break;
	case EVT_DCLICK:
		if( g->OnDblClick != NULL )
			return g->OnDblClick( g );
		break;
	case EVT_RCLICK:
		if( g->OnRClick != NULL )
			return g->OnRClick( g );
		break;
	case EVT_KPRESS:
		if( g->OnKeyPressed != NULL )
			return g->OnKeyPressed( g, p );
		break;
	default:
		break;
	}	
	return false;
}
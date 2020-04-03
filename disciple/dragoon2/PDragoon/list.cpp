#include <windows.h>
#include "pluginclass.h"


CList::CList()
{
	Plugin->Act->CreateObj( Plugin, (void *)LIST, &Obj );
	Plugin->Act->SetEventFunc( Plugin, Obj, OnEvent, this );
	Height = 30;
	Width = 100;
	Left = 0;
	Top = 0;
	OnClick = OnRClick = OnDblClick = NULL;
	OnKeyPressed = NULL;
}

CList::~CList()
{
	Plugin->Act->DeleteObj( Plugin, Obj );
}

void
CList::AddItem( DString ItemText )
{
	Plugin->Act->SetField(Plugin, Obj, ItemText.c_str(), (void *)F_ADDITEM );
}

void
CList::DelItem( int Index )
{
	Plugin->Act->SetField(Plugin, Obj, &Index, (void *)F_DELITEM );
}

DString
CList::GetItem( int Index )
{
	char buf[1024];
	buf[0] = 0;
	Plugin->Act->SetField( NULL, Obj, &Index, (void *)F_SETINDEX  );
	Plugin->Act->SetField( NULL, Obj, buf, (void *)F_ITEM );
	return buf;
}

void
CList::SetItem( int Index, DString val )
{
	Plugin->Act->SetField(Plugin, Obj, &Index, (void *)F_SETINDEX );
	Plugin->Act->SetField(Plugin, Obj, val.c_str(), (void *)F_ITEM );
}

int
CList::GetIndex()
{
	int i;
	Plugin->Act->SetField( NULL, Obj, &i, (void *)F_INDEX );
	return i;
}

void
CList::SetIndex( int Index )
{
	Plugin->Act->SetField( Plugin , Obj, &Index, (void *)F_INDEX );
}


int
CList::GetTopIndex()
{
	int i;
	Plugin->Act->SetField( NULL, Obj, &i, (void *)F_TOPINDEX );
	return i;
}

void
CList::SetTopIndex( int Index )
{
	Plugin->Act->SetField( Plugin , Obj, &Index, (void *)F_TOPINDEX );
}

int
CList::GetCount()
{
	int i;
	Plugin->Act->SetField( NULL, Obj, &i, (void *)F_ITEMCOUNT );
	return i;
}

void
CList::Clear()
{
	Plugin->Act->SetField( Plugin, Obj, NULL, (void*)F_CLEAR );
}

bool
CList::OnEvent( void *Sender, void *EventType, void *Class, void *p )
{
	List l = (List)Class;
	switch( (int)EventType )
	{
	case EVT_LCLICK:
		if( l->OnClick != NULL )
			return l->OnClick( l );
		break;
	case EVT_DCLICK:
		if( l->OnDblClick != NULL )
			return l->OnDblClick( l );
		break;
	case EVT_RCLICK:
		if( l->OnRClick != NULL )
			return l->OnRClick( l );
		break;
	case EVT_KPRESS:
		if( l->OnKeyPressed != NULL )
			return l->OnKeyPressed( l, p );
		break;
	default:
		break;
	}	
	return false;
}
#include <windows.h>
#include "pluginclass.h"


CCombo::CCombo()
{
	Plugin->Act->CreateObj( Plugin, (void *)COMBO, &Obj );
	Plugin->Act->SetEventFunc( Plugin, Obj, OnEvent, this );
	Height = 30;
	Width = 100;
	Left = 0;
	Top = 0;
	OnChange = NULL;
}

CCombo::~CCombo()
{
	Plugin->Act->DeleteObj( Plugin, Obj );
}

void
CCombo::AddItem( DString ItemText )
{
	Plugin->Act->SetField(Plugin, Obj, ItemText.c_str(), (void *)F_ADDITEM );
}

void
CCombo::DelItem( int Index )
{
	Plugin->Act->SetField(Plugin, Obj, &Index, (void *)F_DELITEM );
}

DString
CCombo::GetItem( int Index )
{
	char buf[1024];
	buf[0] = 0;
	Plugin->Act->SetField( NULL, Obj, &Index, (void *)F_SETINDEX  );
	Plugin->Act->SetField( NULL, Obj, buf, (void *)F_ITEM );
	return buf;
}

void
CCombo::SetItem( int Index, DString val )
{
	Plugin->Act->SetField(Plugin, Obj, &Index, (void *)F_SETINDEX );
	Plugin->Act->SetField(Plugin, Obj, val.c_str(), (void *)F_ITEM );
}

int
CCombo::GetIndex()
{
	int i;
	Plugin->Act->SetField( NULL, Obj, &i, (void *)F_INDEX );
	return i;
}

void
CCombo::SetIndex( int Index )
{
	Plugin->Act->SetField( Plugin , Obj, &Index, (void *)F_INDEX );
}

int
CCombo::GetCount()
{
	int i;
	Plugin->Act->SetField( NULL, Obj, &i, (void *)F_ITEMCOUNT );
	return i;
}

bool
CCombo::OnEvent( void *Sender, void *EventType, void *Class, NOTUSED )
{
	Combo c = (Combo)Class;
	switch( (int)EventType )
	{
	case EVT_CHANGE:
		if( c->OnChange != NULL )
			return c->OnChange( c );
		break;
	default:
		break;
	}	
	return false;
}
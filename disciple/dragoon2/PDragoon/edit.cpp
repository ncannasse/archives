#include <windows.h>
#include "pluginclass.h"


CEdit::CEdit()
{
	Plugin->Act->CreateObj( Plugin, (void *)EDIT, &Obj );
	Plugin->Act->SetEventFunc( Plugin, Obj, OnEvent, this );
	Height = 30;
	Width = 100;
	Left = 0;
	Top = 0;
	OnChange = NULL;
	OnKeyPress = NULL;
}

CEdit::~CEdit()
{
	Plugin->Act->DeleteObj( Plugin, Obj );
}

void
CEdit::SetText( DString Text )
{
	Plugin->Act->SetField( Plugin, Obj, Text.c_str(), (void *)F_TEXT );
}

DString
CEdit::GetText()
{
	char buf[1024];
	Plugin->Act->SetField( NULL, Obj, buf, (void *)F_TEXT );
	return buf;
}

bool
CEdit::OnEvent( void *Sender, void *EventType, void *Class, void *p )
{
	Edit e = (Edit)Class;
	switch( (int)EventType )
	{
	case EVT_CHANGE:
		if( e->OnChange != NULL )
			return e->OnChange( e );
		break;
	case EVT_KPRESS:
		if( e->OnKeyPress != NULL )
			return e->OnKeyPress( e, p );
		break;
	default:
		break;
	}	
	return false;
}
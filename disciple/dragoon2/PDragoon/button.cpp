#include <windows.h>
#include "pluginclass.h"


CButton::CButton()
{
	Plugin->Act->CreateObj( Plugin, (void *)BUTTON, &Obj );
	Plugin->Act->SetEventFunc( Plugin, Obj, OnEvent, this );
	Height = 30;
	Width = 100;
	Left = 0;
	Top = 0;
	OnClick = OnRClick = NULL;
}

CButton::~CButton()
{
	Plugin->Act->DeleteObj( Plugin, Obj );
}

void
CButton::SetCaption( DString Text )
{
	Plugin->Act->SetField( Plugin, Obj, Text.c_str(), (void *)F_CAPTION );
}

DString
CButton::GetCaption()
{
	char buf[1024];
	Plugin->Act->SetField( Plugin, Obj, buf, (void *)F_CAPTION );
	return buf;
}

bool
CButton::OnEvent( void *Sender, void *EventType, void *Class, NOTUSED )
{
	Button b = (Button)Class;
	switch( (int)EventType )
	{
	case EVT_LCLICK:
		if( b->OnClick != NULL )
			return b->OnClick( b );
		break;
	case EVT_RCLICK:
		if( b->OnRClick != NULL )
			return b->OnRClick( b );
		break;
	default:
		break;
	}	
	return false;
}
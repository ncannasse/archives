#include <windows.h>
#include "pluginclass.h"


CLabel::CLabel()
{
	Plugin->Act->CreateObj( Plugin, (void *)LABEL, &Obj );
	Plugin->Act->SetEventFunc( Plugin, Obj, OnEvent, this );
	Height = 0;
	Width = 0;
	Left = 0;
	Top = 0;
	OnLClick = OnRClick = OnDblClick = NULL;
}

CLabel::~CLabel()
{
	Plugin->Act->DeleteObj( Plugin, Obj );
}

void
CLabel::SetCaption( DString Text )
{
	Plugin->Act->SetField( Plugin, Obj, Text.c_str(), (void *)F_CAPTION );
}

DString 
CLabel::GetCaption()
{
	char buf[1024];
	Plugin->Act->SetField( NULL, Obj, buf, (void*)F_CAPTION );
	return buf;
}

bool
CLabel::OnEvent( void *Sender, void *EventType, void *Class, NOTUSED )
{
	Label l = (Label)Class;
	switch( (int)EventType )
	{
	case EVT_LCLICK:
		if( l->OnLClick != NULL )
			return l->OnLClick( l );
		break;
	case EVT_RCLICK:
		if( l->OnRClick != NULL )
			return l->OnRClick( l );
		break;
	case EVT_DCLICK:
		if( l->OnDblClick != NULL )
			return l->OnDblClick( l );
		break;
	default:
		break;
	}	
	return false;
}
#include <windows.h>
#include "pluginclass.h"

CDragoonButton::CDragoonButton()
{
	Plugin->Act->RegisterButton( Plugin, &Obj, OnEvent, this );
	OnLClick = OnRClick = NULL;
}

CDragoonButton::~CDragoonButton()
{
	Plugin->Act->DeleteObj( Plugin, Obj );
}

void
CDragoonButton::SetHint( DString h )
{
	Plugin->Act->SetField(  Plugin, Obj, h.c_str(), (void *)F_HINT );
}

DString 
CDragoonButton::GetHint()
{
	char buf[1024];
	Plugin->Act->SetField( NULL, Obj, buf,(void *)F_HINT );
	return buf;
}

void
CDragoonButton::SetIcon( HBITMAP Ico )
{
	Plugin->Act->SetField(  Plugin, Obj, Ico, (void *)F_ICON );
}

HBITMAP
CDragoonButton::GetIcon()
{
	HBITMAP i;
	Plugin->Act->SetField(  NULL, Obj, &i, (void *)F_ICON );
	return i;
}

bool
CDragoonButton::OnEvent( void *Sender, void *EventType, void *Class, NOTUSED )
{
	DragoonButton b = (DragoonButton)Class;
	switch( (int)EventType )
	{
	case EVT_LCLICK:
		if( b->OnLClick != NULL )
			return b->OnLClick( b );
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
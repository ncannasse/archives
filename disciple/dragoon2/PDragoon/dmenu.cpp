 /* *********************************************************************** */
// dragoonmenu.cpp  - adding a menu item
/* *********************************************************************** */
#include <windows.h>
#include "pluginclass.h"

CDragoonMenu::CDragoonMenu()
{
	Plugin->Act->RegisterMenu( Plugin, &Obj, OnEvent, this );
	OnClick = false;
}

CDragoonMenu::~CDragoonMenu()
{
	Plugin->Act->DeleteObj( Plugin, Obj );
}

void
CDragoonMenu::SetCaption( DString  Txt )
{
	Plugin->Act->SetField( Plugin, Obj, Txt.c_str(), (void*)F_CAPTION );
}

void
CDragoonMenu::SetEnabled( bool state )
{
	Plugin->Act->SetField( Plugin, Obj, &state, (void *)F_ENABLED );
}

bool
CDragoonMenu::GetEnabled()
{
	bool e;
	Plugin->Act->SetField( NULL, Obj, &e, (void *)F_ENABLED );
	return e;
}

void
CDragoonMenu::SetVisible( bool state )
{
	Plugin->Act->SetField( Plugin, Obj, &state, (void *)F_VISIBLE );
}

bool
CDragoonMenu::GetVisible()
{
	bool v;
	Plugin->Act->SetField( NULL, Obj, &v, (void *)F_VISIBLE );
	return v;
}

void
CDragoonMenu::SetChecked( bool state )
{
	Plugin->Act->SetField( Plugin, Obj, &state, (void *)F_CHECKED );
}

bool
CDragoonMenu::GetChecked()
{
	bool v;
	Plugin->Act->SetField( NULL, Obj, &v, (void *)F_CHECKED );
	return v;
}


bool
CDragoonMenu::OnEvent( void *Sender, void *EventType, void *Class, NOTUSED )
{
	DragoonMenu m = (DragoonMenu)Class;
	if( (int)EventType == EVT_LCLICK && m->OnClick != NULL )
			return m->OnClick( (OBJECT)m );
	return false;
}

void
CDragoonMenu::SetParent( CDragoonMenu *Par )
{
	Plugin->Act->SetField( Plugin, Obj, Par->Obj, (void *)F_PARENT );
}

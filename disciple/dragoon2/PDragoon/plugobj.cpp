#include <windows.h>
#include "pluginclass.h"

CPlugObj::CPlugObj()
{
}

CPlugObj::~CPlugObj()
{
}

void
CPlugObj::SetEnabled( bool state )
{
	Plugin->Act->SetField( Plugin, Obj, &state, (void *)F_ENABLED );
}

bool
CPlugObj::GetEnabled()
{
	bool e;
	Plugin->Act->SetField( NULL, Obj, &e, (void *)F_ENABLED );
	return e;
}

void
CPlugObj::SetVisible( bool state )
{
	Plugin->Act->SetField( Plugin, Obj, &state, (void *)F_VISIBLE );
}

bool
CPlugObj::GetVisible()
{
	bool v;
	Plugin->Act->SetField( NULL, Obj, &v, (void *)F_VISIBLE );
	return v;
}

void
CPlugObj::SetHeight( int H )
{
	Plugin->Act->SetField( Plugin, Obj, &H, (void *)F_HEIGHT );
}

int
CPlugObj::GetHeight()
{
	int H;
	Plugin->Act->SetField( NULL, Obj, &H, (void *)F_HEIGHT );
	return H;
}

void
CPlugObj::SetWidth( int W )
{
	Plugin->Act->SetField( Plugin, Obj, &W, (void *)F_WIDTH );
}

int
CPlugObj::GetWidth()
{
	int W;
	Plugin->Act->SetField( NULL, Obj, &W, (void *)F_WIDTH );
	return W;
}

void
CPlugObj::SetLeft( int L )
{
	Plugin->Act->SetField( Plugin, Obj, &L, (void *)F_LEFT );
}

int
CPlugObj::GetLeft()
{
	int L;
	Plugin->Act->SetField( NULL, Obj, &L, (void *)F_LEFT );
	return L;
}

void
CPlugObj::SetTop( int T )
{	
	Plugin->Act->SetField( Plugin, Obj, &T, (void *)F_TOP );
}

int
CPlugObj::GetTop()
{	
	int T;
	Plugin->Act->SetField( NULL, Obj, &T, (void *)F_TOP );
	return T;
}



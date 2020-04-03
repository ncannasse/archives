/* ******************************************************************** */
#include <windows.h>
#include "plugin.h"
#include "global.h"
#include "erreurs.h"
#include "resource.h"
#include "Symbols.h"
/* ******************************************************************** */

LPSymbol CurS = NULL;

/* ******************************************************************** */

bool Init( OBJECT );

bool CreateSymbol	( OBJECT );
bool DeleteSymbol	( OBJECT );
bool NewSymbol		( OBJECT );
bool LoadSymbols	( LPCSTR );
bool SaveSymbols	( LPCSTR );
bool SelectSymbol	( OBJECT );

/* ******************************************************************** */

struct TObjets {
	
	DragoonButton Main;

	List list;
	Button newbutton;
	Button deletebutton;

} Objets;

/* ******************************************************************** */

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
		Erreurs = new CErreurs;
#endif
		Plugin = new CPlugin(hModule);
		Plugin->RegisterName( NOM_COMPLET );
		Plugin->RegisterAuthor( AUTHOR );
		Plugin->RegisterInfos( HELP );
		Plugin->Init = Init;
		Symbols = new CSymbols;
		break;
	case DLL_PROCESS_DETACH:

		delete Plugin;
		delete Symbols;
#ifdef _DEBUG
		delete Erreurs;
#endif
		break;
	default:
		break;
	}
    return TRUE;
}

/* ******************************************************************** */

bool
Init( OBJECT )
{

	Plugin->OnProjectLoad = LoadSymbols;
	Plugin->OnProjectSave = SaveSymbols;

	Objets.Main = new CDragoonButton;
	Objets.Main->Icon = LoadBitmap((HINSTANCE)Plugin->GetHandle(), MAKEINTRESOURCE( IDB_BUTTON ) );
	Objets.Main->Hint = "Edit Symbols";
	Objets.Main->OnLClick = CreateSymbol;

	Objets.newbutton = new CButton;
	Objets.newbutton->Caption = "New Symbol";
	Objets.newbutton->OnClick = NewSymbol;
	PosPlugObj( Objets.newbutton, 20,350,90,20 );

	Objets.deletebutton = new CButton;
	Objets.deletebutton->OnClick = DeleteSymbol;
	Objets.deletebutton->Caption = "Delete Symbol";
	PosPlugObj( Objets.deletebutton, 220,350,90,20 );

	Objets.list = new CList;
	PosPlugObj(Objets.list, 20, 20, 290, 320 );
	Objets.list->OnClick = SelectSymbol;

	return true;
}

/* ******************************************************************** */

void
Actu()
{
	LPSymbol l = Symbols->SymbList;
	char buf[200];

	Objets.list->Clear();
	while ( l != NULL )
	{
		strcpy( buf, "G");
		_ltoa( l->Grp, buf+1, 10 );
		strcat( buf, "  -  ");
		strcat( buf, l->Symbol );
		strcat( buf, " ( ");
		_ltoa( l->ID, buf+strlen(buf), 10 );
		strcat( buf, " )");
		Objets.list->AddItem( buf );
		l = l->Suivant;
	}
}

/* ******************************************************************** */

bool
SelectSymbol( OBJECT )
{
	int i = Objets.list->ItemIndex;
	CurS = Symbols->SymbList;

	while( CurS != NULL && i>0 )
	{
		CurS = CurS->Suivant;
		i--;
	}
	return true;
}

/* ******************************************************************** */

bool
DeleteSymbol( OBJECT )
{

	if( CurS != NULL )
	{
		Symbols->DelSymbol( CurS );
		CurS = NULL;
		Actu();
	}
	return true;
}

/* ******************************************************************** */

bool
CreateSymbol( OBJECT )
{
	Plugin->ShowPlugForm();
	Actu();
	CurS = NULL;
	return false;
}

/* ******************************************************************** */

bool
NewSymbol( OBJECT )
{
	DString buf, buf2, buf3;

	if( ! Plugin->InputText("Symbol:", "", &buf ) )
		return false;
	if( ! Plugin->InputText("ID:", "", &buf2 ) )
		return false;
	if( ! Plugin->InputText("Group:", "", &buf3 ) )
		return false;
	if( atol( buf2.c_str() ) == 0 || Symbols->FindByName( buf.c_str() ) != NULL )
		return false;

	Symbols->AddSymbol( atol(buf2.c_str()), buf.c_str(), atoi(buf3.c_str()) );
	Actu();
	CurS = NULL;
	return true;
}

/* ******************************************************************** */

bool
LoadSymbols( LPCSTR nomProjet)
{
	if( Symbols->Nom == NULL )
	{
		Symbols->Nom = new char[ strlen(nomProjet)+7 ];
		strcpy( Symbols->Nom , nomProjet );
		strcpy( Symbols->Nom+strlen(Symbols->Nom)-4 , "_symbols.h" );
	}
	Symbols->Load();
	return true;
}

/* ******************************************************************** */

bool
SaveSymbols( LPCSTR )
{
	Symbols->Save();
	return true;
}

/* ******************************************************************** */

bool
ConvertSymbol( void *param, void * )
{
	LPParam p =(LPParam)param;
	LPSymbol s = Symbols->Find(p);
	if( s != NULL )
	{
		if( IsBadWritePtr( p->Symbol, strlen(s->Symbol)+1 ) )
			return false;
		strcpy( p->Symbol, s->Symbol );
		return true;
	}
	return false;
}

/* ******************************************************************** */

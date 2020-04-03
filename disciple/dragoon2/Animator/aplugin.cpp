/* ******************************************************************** */
#include <windows.h>
#include "global.h"
#include "resource.h"
#include "anims.h"
#include "interface.h"
#include "erreurs.h"
#include "dlib.h"
#include "plugin.h"

/* ******************************************************************** */

bool PluginInit( OBJECT );
bool ProjectInit( LPCSTR );
bool CreateAnm	( OBJECT );
bool LoadAnm	( LPCSTR );
bool SaveAnm	( LPCSTR, LPCSTR );
bool RenameAnm	( LPCSTR, LPCSTR );
bool DeleteAnm	( LPCSTR );
bool OnMainClick( LPCSTR );
bool OnPAKChange( LPCSTR );
bool ImportClick( OBJECT o );

/* ******************************************************************** */

char SautLigne[3] = { 13, 10, 0 };
HANDLE hInst;
CAnims Anims;
extern LPAnimList CurAnim;
extern LPAnimObj  CurObj;
extern int CurFrame;

CAnims *ImportAnims = NULL;
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
		Plugin->Init = PluginInit;
		Plugin->RegisterName( NOM_COMPLET );
		Plugin->RegisterAuthor( AUTHOR );
		Plugin->RegisterInfos( HELP );
		Plugin->Balise = "ANIMATION";
		Plugin->Icon = LoadBitmap((HINSTANCE)hModule, MAKEINTRESOURCE( IDB_ICON ) );
		DLib = new CDLib;
		hInst = hModule;
		break;
	case DLL_PROCESS_DETACH:
		delete ImportAnims;	
		delete Objets.ImportList;
		delete Objets.MainMenu;
		delete Objets.ImportMenu;
		delete Objets.anm_params[0];
		delete Objets.anm_params[1];
		delete Objets.anm_params[2];
		delete Objets.delete_frame;
		delete Objets.duplicate;
		delete Objets.label_animation;
		delete Objets.label_nom;
		delete Objets.MainButton;
		delete Objets.move_up_frame;
		delete Objets.obj_button;
		delete Objets.obj_del;
		delete Objets.obj_grid;
		delete Objets.obj_lab_debut;
		delete Objets.obj_lab_fin;
		delete Objets.obj_label;
		delete Objets.obj_list;
		delete Objets.obj_moveup;
		delete Objets.run_button;

		Anims.Clean();
		delete Anims.Nom;
		Anims.Nom = NULL;
		delete DLib;
		delete Plugin;
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
PluginInit( OBJECT )
{
	Objets.MainButton = new CDragoonButton;
	Objets.MainButton->Icon = LoadBitmap((HINSTANCE)Plugin->GetHandle(), MAKEINTRESOURCE( IDB_BUTTON ) );
	Objets.MainButton->OnLClick = CreateAnm;
	Objets.MainButton->Hint = "Create new Animation";
	
	Plugin->OnProjectLoad = OnLoadProject;
	Plugin->OnProjectSave = OnSaveProject;
	Plugin->OnPAKChange = OnPAKChange;
	Plugin->OnProjectInit = ProjectInit;

	Plugin->OnRename = RenameAnm;
	Plugin->OnDelete = DeleteAnm;
	Plugin->OnClick	 = OnMainClick;
	Plugin->OnSave   = SaveAnm;
	Plugin->OnLoad   = LoadAnm;
	Plugin->OnDrop   = OnDropPAKItem;

	CreateInterface();
	return true;
}

/* ******************************************************************** */

bool
OnPAKChange( LPCSTR pak )
{
	DLib->LoadPak( pak );
	return true;
}

/* ******************************************************************** */

bool
DeleteAnm( LPCSTR nom )
{
	if(  Anims.DeleteAnim( (char*)nom ) )
	{
		Plugin->HidePlugForm();
		return true;
	}
	return false;
}

/* ******************************************************************** */

bool
LoadAnm( LPCSTR nom )
{
	if( !Anims.Load( (char *)nom ) )
	{
		char buf[150];
		strcpy(buf,"Cannot load Anim: ");
		strcat(buf,(char *)nom );
		Plugin->LogText( buf );
		Anims.CreateEmpty((char *)nom);
	}
	return true;
}

/* ******************************************************************** */

bool
CreateAnm( OBJECT )
{
	DString buf;
	if( ! Plugin->InputText("New Animation name:", "", &buf ) )
		return false;
	if( Anims.FindByName( buf.c_str() ) != NULL )
	{
		Plugin->LogText("This name is already used");
		return false;
	}
	if( ! Plugin->CreateItem( buf.c_str() ) )
		return false;
	Anims.CreateEmpty(buf.c_str());
	return true;
}

/* ******************************************************************** */

bool
RenameAnm( LPCSTR nom1, LPCSTR nom2 )
{
	LPAnimList l = Anims.FindByName( (char*)nom1 );
	if( l == NULL )
		return false;
	
	delete l->Name;
	l->Name = new char[ strlen(nom2)+1 ];
	strcpy( l->Name, nom2 );
	
	return true;
}

/* ******************************************************************** */

bool
SaveAnm( LPCSTR nom, LPCSTR buf )
{
	strcpy((char*) buf, "ANIMATION=");
	strcat((char*) buf, (char*)nom );
	strcat((char*) buf, SautLigne );

	if( Anims.FindByName( (char*)nom ) == NULL )
		return false;
	return true;
}

/* ******************************************************************** */

bool
ProjectInit( LPCSTR Nom )
{
	delete Objets.MainMenu;
	Objets.MainMenu = new CDragoonMenu;
	Objets.MainMenu->Caption = "Animator";
	delete Objets.ImportMenu;
	Objets.ImportMenu = new CDragoonMenu;
	Objets.ImportMenu->Caption = "Import..";
	Objets.ImportMenu->Parent = Objets.MainMenu;
	Objets.ImportMenu->OnClick = ImportClick;

	delete Anims.Nom;
	Anims.Nom = new char[ strlen(Nom)+1 ];
	strcpy( Anims.Nom, Nom );
	strcpy( Anims.Nom+strlen(Nom)-4, ".anm" );
	return true;
}

/* ******************************************************************** */

bool
OnMainClick( LPCSTR nom )
{
	LPAnimList l = Anims.FindByName( (char *)nom );
	if( l == NULL )
		return false;
	CurAnim = l;
	CurObj = NULL;
	CurFrame = -1;
	Plugin->ShowPlugForm();
	Redraw();
	return true;
}

/* ******************************************************************** */

bool
ImportClick( OBJECT o )
{
	DString f;

	if( ! Objets.ImportMenu->Checked )
	{
		if( !Plugin->LoadFileQuery("*.anm", "Import from wich file ?", &f ) )
			return false;
		CurAnim = NULL;

		Objets.anm_params[0]->Visible = false;
		Objets.anm_params[1]->Visible = false;
		Objets.anm_params[2]->Visible = false;
		Objets.delete_frame->Visible = false;
		Objets.duplicate->Visible = false;
		Objets.label_animation->Visible = false;
		Objets.label_nom->Visible = false;
		Objets.move_up_frame->Visible = false;
		Objets.obj_button->Visible = false;
		Objets.obj_del->Visible = false;
		Objets.obj_grid->Visible = false;
		Objets.obj_lab_debut->Visible = false;
		Objets.obj_lab_fin->Visible = false;
		Objets.obj_label->Visible = false;
		Objets.obj_list->Visible = false;
		Objets.obj_moveup->Visible = false;
		Objets.run_button->Visible = false;

		Objets.ImportMenu->Checked = true;
		Objets.ImportList->Visible = true;

		ImportAnims = new CAnims;
		ImportAnims->Nom = new char[ f.Length() + 1 ];
		strcpy( ImportAnims->Nom, f.c_str() );
		Plugin->ShowPlugForm();
		if( ! ImportAnims->LoadAll() )
		{
			Plugin->LogText( (DString)"Cannot load : " + f );
			ImportClick(o);
			return false;
		}
		Redraw();
	}
	else
	{
		Objets.anm_params[0]->Visible = true;
		Objets.anm_params[1]->Visible = true;
		Objets.anm_params[2]->Visible = true;
		Objets.delete_frame->Visible = true;
		Objets.duplicate->Visible = true;
		Objets.label_animation->Visible = true;
		Objets.label_nom->Visible = true;
		Objets.move_up_frame->Visible = true;
		Objets.obj_button->Visible = true;
		Objets.obj_del->Visible = true;
		Objets.obj_grid->Visible = true;
		Objets.obj_lab_debut->Visible = true;
		Objets.obj_lab_fin->Visible = true;
		Objets.obj_label->Visible = true;
		Objets.obj_list->Visible = true;
		Objets.obj_moveup->Visible = true;
		Objets.run_button->Visible = true;

		Objets.ImportList->Visible = false;
		Objets.ImportMenu->Checked = false;
		delete ImportAnims;
		ImportAnims = NULL;
		if( CurAnim == NULL )
			Plugin->HidePlugForm();
		Redraw();
	}

	return true;
}

/* ******************************************************************** */

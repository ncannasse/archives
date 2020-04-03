/* ******************************************************************** */
#include <windows.h>
#include "plugin.h"
#include "global.h"
#include "erreurs.h"
#include "resource.h"
#include "data.h"
/* ******************************************************************** */

bool CreateDatas( OBJECT );

bool NewStruct( OBJECT );
bool DelStruct( OBJECT );
bool SelectStruct( OBJECT );

bool NewField( OBJECT );
bool DelField( OBJECT );
bool MoveUpField( OBJECT );
bool SelectField( OBJECT );

bool NewData( OBJECT );
bool DelData( OBJECT );
bool ChangeID( OBJECT );
bool ChangeName( OBJECT );
bool ModData( OBJECT, void * );
bool SelectSData( OBJECT );

bool SelectDField( OBJECT );

bool LoadData( LPCSTR );
bool SaveData( LPCSTR );
bool PluginInit( OBJECT );

bool ChangeType( OBJECT );

bool MainClick( OBJECT );

void RedrawAll();
/* ******************************************************************** */
LPStruct CurS = NULL;
LPSData CurD = NULL;
int SField = -1;
int SDataField = -1;
/* ******************************************************************** */

struct TObjets {

	DragoonButton Main;
	
	List str_list;
	Button str_new;
	Button str_del;
	Label str_lab;

	List str_edit;
	Button str_edit_new;
	Button str_edit_del;
	Button str_edit_moveup;
	Label str_edit_lab;

	List dat_list;
	Button dat_new;
	Button dat_del;
	Button dat_ID;
	Button dat_Name;
	Label dat_lab;

	List dat_edit;
	Label dat_edit_lab;
	Edit modif_txt;

	Combo Type;

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
		Plugin->Init = PluginInit;
		Datas = new CData;
		break;
	case DLL_PROCESS_DETACH:
		delete Objets.str_edit_moveup;
		delete Objets.dat_Name;
		delete Objets.dat_ID;
		delete Objets.dat_del;
		delete Objets.dat_edit;
		delete Objets.dat_edit_lab;
		delete Objets.dat_lab;
		delete Objets.dat_list;
		delete Objets.dat_new;
		delete Objets.Main;
		delete Objets.modif_txt;
		delete Objets.str_del;
		delete Objets.str_edit;
		delete Objets.str_edit_del;
		delete Objets.str_edit_lab;
		delete Objets.str_edit_new;
		delete Objets.str_lab;
		delete Objets.str_list;
		delete Objets.str_new;
		delete Objets.Type;
		delete Datas;
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
	Plugin->OnProjectLoad = LoadData;
	Plugin->OnProjectSave = SaveData;

	Objets.Main = new CDragoonButton;
	Objets.Main->Icon = LoadBitmap((HINSTANCE)Plugin->GetHandle(), MAKEINTRESOURCE( IDB_BUTTON ) );
	Objets.Main->OnLClick = MainClick;
	Objets.Main->Hint = "Edit Datas";

	Objets.str_new = new CButton;
	Objets.str_new->Caption = "New Struct";
	Objets.str_new->Left = 20;
	Objets.str_new->Top = 250;
	Objets.str_new->Width = 70;
	Objets.str_new->Height = 20;
	Objets.str_new->OnClick = NewStruct;

	Objets.str_del = new CButton;
	Objets.str_del->Caption = "Delete Struct";
	Objets.str_del->Left = 100;
	Objets.str_del->Top = 250;
	Objets.str_del->Width = 70;
	Objets.str_del->Height = 20;
	Objets.str_del->OnClick = DelStruct;

	Objets.str_list = new CList;
	PosPlugObj( Objets.str_list, 20,20,150,220 );
	Objets.str_list->OnClick = SelectStruct;

	Objets.str_lab = new CLabel;
	PosPlugObj( Objets.str_lab, 20, 5, 0,0 );


	Objets.str_edit_new = new CButton;
	PosPlugObj( Objets.str_edit_new, 220,250,70,20 );
	Objets.str_edit_new->Caption = "Add Field";
	Objets.str_edit_new->OnClick = NewField;

	Objets.str_edit_del = new CButton;
	Objets.str_edit_del->Caption = "Delete Field";
	Objets.str_edit_del->OnClick = DelField;
	PosPlugObj( Objets.str_edit_del, 300,250,70,20 );

	Objets.str_edit_moveup = new CButton;
	Objets.str_edit_moveup->Caption = "Move UP Field";
	Objets.str_edit_moveup->OnClick = MoveUpField;
	PosPlugObj( Objets.str_edit_moveup, 220,280,150,20 );

	Objets.str_edit = new CList;
	PosPlugObj( Objets.str_edit, 220, 20, 150, 190 );
	Objets.str_edit->OnClick = SelectField;

	Objets.str_edit_lab = new CLabel;
	PosPlugObj( Objets.str_edit_lab, 220, 5, 0,0 );

	Objets.dat_new = new CButton;
	Objets.dat_new->Caption ="New Instance";
	Objets.dat_new->OnClick = NewData;
	PosPlugObj(Objets.dat_new, 20,550,70,20 );

	Objets.dat_del = new CButton;
	Objets.dat_del->Caption = "Delete Inst.";
	Objets.dat_del->OnClick = DelData;
	PosPlugObj( Objets.dat_del, 100,550,70,20 );

	Objets.dat_ID = new CButton;
	Objets.dat_ID->Caption = "Change ID";
	Objets.dat_ID->OnClick = ChangeID;
	PosPlugObj( Objets.dat_ID, 20,580,70,20 );

	Objets.dat_Name = new CButton;
	Objets.dat_Name->Caption = "Rename";
	Objets.dat_Name->OnClick = ChangeName;
	PosPlugObj( Objets.dat_Name, 100,580,70,20 );

	
	Objets.dat_list = new CList;
	Objets.dat_list->OnClick = SelectSData;
	PosPlugObj(Objets.dat_list, 20, 320, 150, 220 );

	Objets.dat_lab = new CLabel;
	PosPlugObj( Objets.dat_lab , 20, 305, 0,0 );



	Objets.modif_txt = new CEdit;
	Objets.modif_txt->OnKeyPress = ModData;
	PosPlugObj( Objets.modif_txt, 220, 550, 150, 20 );

	Objets.dat_edit = new CList ;
	Objets.dat_edit->OnClick = SelectDField;
	PosPlugObj( Objets.dat_edit, 220, 320, 150, 220 );

	Objets.dat_edit_lab = new CLabel;
	PosPlugObj( Objets.dat_edit_lab, 220, 305, 0,0 );

	Objets.Type = new CCombo;
	PosPlugObj( Objets.Type, 220,220,150,20 );
	Objets.Type->AddItem( "Char" );
	Objets.Type->AddItem( "Short" );
	Objets.Type->AddItem( "Byte" );
	Objets.Type->AddItem( "Word" );
	Objets.Type->AddItem( "Long" );
	Objets.Type->AddItem( "String" );
	Objets.Type->ItemIndex = 1;
	Objets.Type->OnChange = ChangeType;

	return true;
}

/* ******************************************************************** */

bool
SelectStruct( OBJECT )
{
	int i = Objets.str_list->ItemIndex;
	CurS = Datas->lst;
	while( CurS != NULL && i>0 )
	{
		CurS = CurS->Suivant;
		i--;
	}
	SField = -1;
	SDataField = -1;
	CurD = NULL;
	RedrawAll();
	return true;
}

/* ******************************************************************** */

bool
SelectSData( OBJECT )
{
	int i = Objets.dat_list->ItemIndex;
	CurD = Datas->data;
	while( CurD != NULL )
	{
		if( CurD->S == CurS )
		{
			i--;
			if( i < 0 )
				break;
		}
		CurD = CurD->Suivant;
	}
	SDataField = -1;
	RedrawAll();
	return true;
}

/* ******************************************************************** */

bool
SelectField( OBJECT )
{
	if( CurS == NULL )
		return false;
	SField = Objets.str_edit->ItemIndex;
	Objets.Type->ItemIndex = CurS->Fields[ SField ]->Type;
	RedrawAll();
	return true;
}

/* ******************************************************************** */

bool
SelectDField( OBJECT )
{
	SDataField = Objets.dat_edit->ItemIndex;
	RedrawAll();
	return true;
}

/* ******************************************************************** */

bool
MainClick( OBJECT )
{
	Plugin->ShowPlugForm();
	RedrawAll();
	return true;
}


/* ******************************************************************** */

bool
ModData( OBJECT, void *Key )
{

	if( (int)Key != 13 )
		return false;

	if( SDataField == -1 || CurD == NULL  )
		return false;
	if( Objets.modif_txt->Text.Length() > 0 )
	{
		switch( CurD->S->Fields[SDataField]->Type )
		{
		case 0:
			*((char *)CurD->FVals[SDataField]) = (char)atol( Objets.modif_txt->Text.c_str() );
			break;
		case 1:
			*((short *)CurD->FVals[SDataField]) = (short)atol( Objets.modif_txt->Text.c_str() );
			break;
		case 2:
			*((BYTE *)CurD->FVals[SDataField]) = (BYTE)atol( Objets.modif_txt->Text.c_str() );
			break;
		case 3:
			*((WORD *)CurD->FVals[SDataField]) = (WORD)atol( Objets.modif_txt->Text.c_str() );
			break;
		case 4:
			*((long *)CurD->FVals[SDataField]) = (long)atol( Objets.modif_txt->Text.c_str() );
			break;
		case 5:
			*((DString *)CurD->FVals[SDataField]) = Objets.modif_txt->Text;
			break;
		}
	}

	Objets.modif_txt->Text = "";
	if( SDataField < CurD->S->NbFields-1 )
		SDataField++;
	else
		SDataField=0;

	RedrawAll();
	return true;
}

/* ******************************************************************** */

bool
NewStruct( OBJECT )
{
	DString buf;
	if( ! Plugin->InputText("Enter a struct Name:", "",&buf ) )
		return false;
	if( Datas->AddStruct(buf.c_str()) )
	{
		RedrawAll();
		return true;
	}
	return false;
}

/* ******************************************************************** */

bool
NewData( OBJECT )
{
	DString buf;
	if( CurS == NULL )
		return false;
	if( ! Plugin->InputText("Enter a data Name:","",&buf ) )
		return false;
	if( Datas->AddSData(buf.c_str(), CurS) )
	{
		RedrawAll();
		return true;
	}
	return false;
}

/* ******************************************************************** */

bool
NewField( OBJECT )
{
	DString buf;
	if( CurS == NULL )
		return false;
	if( ! Plugin->InputText("Enter a field Name:","",&buf ) )
		return false;
	if( Datas->AddField(CurS,buf.c_str(),Objets.Type->ItemIndex) )
	{
		RedrawAll();
		return true;
	}
	return false;
}

/* ******************************************************************** */

bool
DelData( OBJECT )
{
	if( CurD == NULL )
		return false;
	Datas->DelSData( CurD );
	CurD = NULL;
	SDataField = -1;
	RedrawAll();
	return true;
}

/* ******************************************************************** */

bool
DelStruct( OBJECT )
{
	if( CurS == NULL )
		return false;
	Datas->DelStruct( CurS );
	CurS = NULL;
	SField = -1;
	RedrawAll();
	return true;
}

/* ******************************************************************** */

bool
DelField( OBJECT )
{
	if( SField == -1 || CurS == NULL)
		return false;
	Datas->DelField( CurS, SField );
	if( SDataField == SField )
		SDataField = -1;
	else
	{
		if( SDataField > SField )
			SDataField--;
	}
	SField = -1;
	RedrawAll();
	return true;
}

/* ******************************************************************** */

bool
MoveUpField( OBJECT )
{
	if( SField <= 0 || CurS == NULL)
		return false;
	Datas->MoveUpField( CurS, SField );
	SField--;
	RedrawAll();
	return true;
}

/* ******************************************************************** */

bool
LoadData( LPCSTR nomProjet )
{
	if( Datas->Nom == NULL )
	{
		Datas->Nom = new char[ strlen(nomProjet)+6 ];
		strcpy( Datas->Nom , nomProjet );
		strcpy( Datas->Nom+strlen(Datas->Nom)-4 , "_data.dat" );
	}
	Datas->Load();
	return true;
}

/* ******************************************************************** */

bool
SaveData( LPCSTR )
{
	Datas->Save();
	return true;
}

/* ******************************************************************** */

bool
ChangeType( OBJECT )
{
	if( SField < 0 || CurS == NULL )
		return false;
	Datas->ChangeType( CurS, SField, Objets.Type->ItemIndex );
	RedrawAll();
	return true;
}

/* ******************************************************************** */

bool
ChangeID( OBJECT )
{
	DString id;
	if( CurD == NULL )
		return false;
	if( !Plugin->InputText("Enter ID:", (long)CurD->ID, &id ) )
		return false;
	CurD->ID = atol( id.c_str() );
	RedrawAll();
	return true;
}

/* ******************************************************************** */

bool
ChangeName( OBJECT )
{
	DString name;
	if( CurD == NULL )
		return false;
	if( !Plugin->InputText("Enter Name:", CurD->Name, &name ) )
		return false;
	if( name == "" )
		return false;
	CurD->Name = name;
	RedrawAll();
	return true;
}

/* ******************************************************************** */

void
RedrawAll()
{
	char buf[256];
	LPStruct s = Datas->lst;
	bool cache = false;
	bool vois = true;

	Objets.str_list->Clear();
	while( s != NULL )
	{
		sprintf( buf, "%ld - %s", s->ID, s->Name.c_str() );

		Objets.str_list->AddItem( buf );
		s = s->Suivant;
	}

	if( CurS != NULL )
	{
		int i;
		LPSData d = Datas->data;

		Objets.str_lab->Caption = CurS->Name;
		Objets.dat_list->Clear();
		while( d != NULL )
		{
			if( d->S == CurS )
			{
				sprintf( buf, "%ld - %s", d->ID, d->Name.c_str() );
				Objets.dat_list->AddItem( buf );
			}
			d = d->Suivant;
		}

		Objets.str_edit->Clear();
		for( i=0; i< CurS->NbFields; i++ )
		{
			switch( CurS->Fields[i]->Type )
			{
			case 0:
				sprintf(buf,"%s (Char)", CurS->Fields[i]->FieldName.c_str() );
				break;
			case 1:
				sprintf(buf,"%s (Short)", CurS->Fields[i]->FieldName.c_str() );
				break;
			case 2:
				sprintf(buf,"%s (Byte)", CurS->Fields[i]->FieldName.c_str() );
				break;
			case 3:
				sprintf(buf,"%s (Word)", CurS->Fields[i]->FieldName.c_str() );
				break;
			case 4:
				sprintf(buf,"%s (Long)", CurS->Fields[i]->FieldName.c_str() );
				break;
			case 5:
				sprintf(buf,"%s (String)", CurS->Fields[i]->FieldName.c_str() );
				break;
			}
			Objets.str_edit->AddItem(buf);
		}

		if( CurD != NULL )
		{
			Objets.dat_edit->Clear();
			for( i=0; i< CurD->S->NbFields; i++ )
			{
				switch( CurD->S->Fields[i]->Type )
				{
				case 0:
					sprintf(buf,"%s = %d", CurD->S->Fields[i]->FieldName.c_str(), *((char *)CurD->FVals[i]) );
					break;
				case 1:
					sprintf(buf,"%s = %d", CurD->S->Fields[i]->FieldName.c_str(), *((short *)CurD->FVals[i]) );
					break;
				case 2:
					sprintf(buf,"%s = %d", CurD->S->Fields[i]->FieldName.c_str(), *((BYTE *)CurD->FVals[i]) );
					break;
				case 3:
					sprintf(buf,"%s = %d", CurD->S->Fields[i]->FieldName.c_str(), *((WORD *)CurD->FVals[i]) );
					break;
				case 4:
					sprintf(buf,"%s = %ld", CurD->S->Fields[i]->FieldName.c_str(), *((long *)CurD->FVals[i]) );
					break;
				case 5:
					sprintf(buf,"%s = %s", CurD->S->Fields[i]->FieldName.c_str(), (*((DString *)CurD->FVals[i])).c_str() );
					break;
				}
				Objets.dat_edit->AddItem(buf);
			}
			Objets.dat_edit->Visible = true;
			Objets.modif_txt->Visible = true;

			Objets.dat_del->Enabled = true;
			Objets.dat_Name->Enabled  = true;
			Objets.dat_ID->Enabled  = true;
			Objets.dat_lab->Caption = CurD->Name;

			if( SDataField >= 0 )
			{
				Objets.modif_txt->Enabled = true;
				Objets.dat_edit_lab->Caption = CurD->S->Fields[ SDataField ]->FieldName;
			}
			else
			{
				Objets.modif_txt->Enabled = false;
				Objets.dat_edit_lab->Caption = "";
			}
		}
		else
		{
			Objets.dat_edit->Visible = false;
			Objets.modif_txt->Visible = false;
			Objets.dat_lab->Caption = CurS->Name;
			Objets.dat_ID->Enabled  = false;
			Objets.dat_Name->Enabled  = false;
			Objets.dat_del->Enabled = false;
			Objets.dat_lab->Caption = "";
			Objets.dat_edit_lab->Caption = "";
		}

		Objets.Type->Visible = true;
		Objets.str_edit->Visible = true;
		Objets.str_edit_new->Visible = true;
		Objets.str_edit_del->Visible = true;
		Objets.str_edit_moveup->Visible = true;

		if( SField > 0 )
			Objets.str_edit_moveup->Enabled = true;
		else
			Objets.str_edit_moveup->Enabled = false;

		if( SField >= 0 )
		{
			Objets.str_edit_del->Enabled = true;

			Objets.str_edit_lab->Caption = CurS->Fields[SField]->FieldName;
		}
		else
		{
			Objets.str_edit_del->Enabled = false;
			Objets.str_edit_lab->Caption = "";
		}

		Objets.dat_list->Visible = true;
		Objets.dat_new->Visible = true;
		Objets.dat_del->Visible = true;
		Objets.dat_ID->Visible  = true;
		Objets.dat_Name->Visible  = true;

		Objets.str_del->Enabled = true;
	}
	else
	{
		Objets.Type->Visible = false;

		Objets.str_edit->Visible = false;
		Objets.str_edit_new->Visible = false;
		Objets.str_edit_del->Visible = false;
		Objets.str_edit_moveup->Visible = false;

		Objets.dat_list->Visible = false;
		Objets.dat_new->Visible = false;
		Objets.dat_del->Visible = false;
		Objets.dat_ID->Visible  = false;
		Objets.dat_Name->Visible  = false;

		Objets.dat_edit->Visible = false;
		Objets.modif_txt->Visible = false;

		Objets.str_del->Enabled = false;

		Objets.str_edit_lab->Caption = "";
		Objets.str_lab->Caption = "";
		Objets.dat_edit_lab->Caption = "";
		Objets.dat_lab->Caption = "";

		CurD = NULL;
		SField = -1;
		SDataField = -1;
	}

}

/* ******************************************************************** */

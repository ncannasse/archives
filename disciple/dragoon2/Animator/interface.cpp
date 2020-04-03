/* ******************************************************************** */
#include "graphisme.h"
#include "interface.h"
#include "plugin.h"
#include "global.h"
#include "anims.h"
#include "erreurs.h"

/* ******************************************************************** */

LPAnimList CurAnim = NULL;
LPAnimObj  CurObj  = NULL;
int CurFrame = 0;
extern CAnims *ImportAnims;
/* ******************************************************************** */

TListeObjs Objets;

/* ******************************************************************** */

bool
RunClick( OBJECT )
{
	if( CurAnim->NbObjs == 0 )
		return false;
#ifdef _DEBUG
	Plugin->HidePlugForm();
#endif
	RunMain();
#ifdef _DEBUG
	Plugin->ShowPlugForm();
#endif
	Redraw();
	return true;
}

/* ******************************************************************** */

void
CreateInterface()
{
	int i;

	Objets.label_animation = new CLabel;
	Objets.label_animation->Left = 10;
	Objets.label_animation->Top = 15;
	Objets.label_animation->Caption = "Animation";

	Objets.label_nom = new CLabel;
	Objets.label_nom->Left = 70;
	Objets.label_nom->Top = 15;

	Objets.obj_label = new CLabel;
	Objets.obj_label->Left = 10;
	Objets.obj_label->Top = 50;
	Objets.obj_label->Caption = "Objets:";
	
	Objets.obj_list = new CList;
	Objets.obj_list->Left = 10;
	Objets.obj_list->Top = 70;
	Objets.obj_list->Width = 150;
	Objets.obj_list->Height = 300;
	Objets.obj_list->OnClick = ListObjClick;

	Objets.obj_button = new CButton;
	Objets.obj_button->Caption = "New OBJ";
	Objets.obj_button->Left = 10;
	Objets.obj_button->Top = 380;
	Objets.obj_button->Width = 150;
	Objets.obj_button->Height = 20;
	Objets.obj_button->OnClick = NewObjClick;

	Objets.obj_del = new CButton;
	Objets.obj_del->Caption = "Del OBJ";
	Objets.obj_del->Left = 10;
	Objets.obj_del->Top = 405;
	Objets.obj_del->Width = 150;
	Objets.obj_del->Height = 20;
	Objets.obj_del->OnClick = DelObjClick;

	Objets.obj_moveup = new CButton;
	Objets.obj_moveup->Caption = "Move UP";
	Objets.obj_moveup->Left = 10;
	Objets.obj_moveup->Top = 430;
	Objets.obj_moveup->Width = 150;
	Objets.obj_moveup->Height = 20;
	Objets.obj_moveup->OnClick = MoveUPClick;

	Objets.run_button = new CButton;
	Objets.run_button->Caption = "RUN Animator";
	Objets.run_button->Left = 10;
	Objets.run_button->Top = 465;
	Objets.run_button->Width = 150;
	Objets.run_button->Height = 20;
	Objets.run_button->OnClick = RunClick;

	Objets.duplicate = new CButton;
	Objets.duplicate->Caption = "Duplicate";
	Objets.duplicate->Left = 10;
	Objets.duplicate->Top = 495;
	Objets.duplicate->Width = 150;
	Objets.duplicate->Height = 20;
	Objets.duplicate->OnClick = Duplicate;

	for(i=0; i<3; i++)
	{
		Objets.anm_params[i] = new CEdit;
		Objets.anm_params[i]->Left = 170+i*150;
		Objets.anm_params[i]->Top = 15;
		Objets.anm_params[i]->Width = 100;
		Objets.anm_params[i]->Height = 20;
		Objets.anm_params[i]->OnKeyPress = ParamChange;
	}

	Objets.obj_lab_fin = new CLabel;
	Objets.obj_lab_fin->Left = 370;
	Objets.obj_lab_fin->Top = 50;


	Objets.obj_lab_debut = new CLabel;
	Objets.obj_lab_debut->Left = 170;
	Objets.obj_lab_debut->Top = 50;

	Objets.move_up_frame = new CButton;
	Objets.move_up_frame->Caption = "Move Up";
	Objets.move_up_frame->Left = 270;
	Objets.move_up_frame->Top = 380;
	Objets.move_up_frame->Width = 90;
	Objets.move_up_frame->Height = 20;
	Objets.move_up_frame->OnClick = MoveUpFrame;

	Objets.delete_frame = new CButton;
	Objets.delete_frame->Caption = "Delete";
	Objets.delete_frame->Left = 370;
	Objets.delete_frame->Top = 380;
	Objets.delete_frame->Width = 90;
	Objets.delete_frame->Height = 20;
	Objets.delete_frame->OnClick = DeleteFrame;
	
	Objets.obj_grid = new CGrid;
	Objets.obj_grid->Flags = FIXED_VLINE | FIXED_HLINE | DRAW_VLINE | DRAW_HLINE;
	Objets.obj_grid->Left = 170;
	Objets.obj_grid->Top = 70;
	Objets.obj_grid->Width = 400;
	Objets.obj_grid->Height = 300;
	Objets.obj_grid->RowHeight = 16;
	Objets.obj_grid->OnClick = GetIndex;

	Objets.ImportList = new CList;
	PosPlugObj( Objets.ImportList, 10, 10, 400, 500 );
	Objets.ImportList->OnDblClick = ImportAnim;
	Objets.ImportList->Visible = false;
}

/* ******************************************************************** */

bool
ImportAnim( OBJECT )
{
	CurAnim = ImportAnims->FindByName( Objets.ImportList->Items[ Objets.ImportList->ItemIndex ].c_str() );
	if( CurAnim != NULL )
		Duplicate(NULL); 
	return true;
}

/* ******************************************************************** */

bool
NewObjClick( OBJECT )
{
	DString buf;
	if( !Plugin->InputText( "Object Name:", "", &buf ) )
		return false;

	if( Anims.AddObj( CurAnim, buf.c_str() ) )
	{
		Objets.obj_list->AddItem( buf.c_str() );
		Plugin->RunPAKBrowser( buf.c_str() );
	}
	else
		Plugin->LogText("Object name already used");

	return true;
}

/* ******************************************************************** */

bool
DelObjClick( OBJECT )
{
	if( CurAnim != NULL && CurObj != NULL )
	{
		Objets.obj_list->DelItem( Objets.obj_list->ItemIndex );
		Objets.obj_grid->Visible = false;
		Objets.obj_lab_fin->Visible = false;
		Objets.obj_lab_debut->Visible = false;
		Objets.delete_frame->Visible = false;
		Objets.move_up_frame->Visible = false;
		Anims.DelObj( CurAnim, CurObj );
		CurObj = NULL;
		Redraw();
	}
	return true;
}

/* ******************************************************************** */

bool
MoveUPClick( OBJECT )
{
	int i;

	if( CurAnim == NULL  || CurObj == NULL )
		return false;
	
	for(i=0; i< CurAnim->NbObjs; i++ )
		if( CurAnim->Objs[i] == CurObj )
		{
			if( i == 0 )
				return false;
			CurAnim->Objs[i] = CurAnim->Objs[i-1];
			CurAnim->Objs[i-1] = CurObj;
			Redraw();
			Objets.obj_list->Clear();
			for(i=0; i< CurAnim->NbObjs; i++ )
				Objets.obj_list->AddItem( CurAnim->Objs[i]->Nom );
			return true;

		}
	return false;
}

/* ******************************************************************** */

bool
ListObjClick( OBJECT )
{
	int i = Objets.obj_list->ItemIndex;
	char buf[150];
	char nb[100];

	DWORD widths[7] = { 20,30,150,30,30,55,55 };
	LPCSTR Idents[7] = { "F","Time","Frame File Name","dx","dy","Flag","PFlag" };

	if( CurAnim == NULL || i >= CurAnim->NbObjs )
		return false;

	CurObj = CurAnim->Objs[i];
	Objets.move_up_frame->Visible = true;
	Objets.delete_frame->Visible = true;
	Objets.obj_grid->Visible = true;
	Objets.obj_lab_debut->Visible = true;
	Objets.obj_lab_fin->Visible = true;


	strcpy(buf, "Ending Code: ");
	switch( CurObj->CodeFin )
	{
	case BOUCLE:
		strcat(buf,"RESTART");
		break;
	case CHANGE_ANIM:
		strcat(buf,"NEXT ANIM is: ");
		strcat(buf,((LPAnimList)CurObj->ParamFin)->Name );
		break;
	case STOP:
		strcat(buf,"STOP");
		break;
	case WAIT_RANDOM:
		strcat(buf,"WAIT RANDOM (");
		_ltoa( CurObj->ParamFin*10, nb, 10 );
		strcat(buf, nb );
		strcat(buf, " ms)");
		break;
	case WAIT_RANDOM_GLOBAL:
		strcat(buf,"WAIT RNDGLOBAL (");
		_ltoa( CurObj->ParamFin*10, nb, 10 );
		strcat(buf, nb );
		strcat(buf, " ms)");
		break;
	case KILL:
		strcat(buf,"KILL");
		break;
	default:
		strcat(buf,"UNKNOWN");
		break;
	}
	Objets.obj_lab_fin->Caption = buf;

	strcpy(buf, "Start Code: ");
	switch( CurObj->CodeDebut )
	{
	case NORMAL:
		strcat(buf,"NORMAL");
		break;
	case PRINCIPAL:
		strcat(buf,"MAIN OBJECT");
		break;
	case WAIT_TIME:
		strcat(buf,"WAIT ");
		_ltoa( CurObj->ParamDebut*10, buf+5+12, 10 );
		strcat(buf," ms");
		break;
	case WAIT_FRAME:
		strcat(buf,"WAIT FRAME ");
		_ltoa( CurObj->ParamDebut, buf+11+12, 10 );
		break;
	case SYNCHRO:
		strcat(buf,"SYNCHRO with: ");
		strcat(buf,((LPAnimObj)CurObj->ParamDebut)->Nom );
		break;
	default:
		strcat(buf,"UNKNOWN");
		break;
	}
	Objets.obj_lab_debut->Caption = buf;

	
	Objets.obj_grid->ColCount =  7;
	Objets.obj_grid->RowCount =  CurObj->FrameCount+2;
	Objets.obj_grid->FixedCols = 0;
	Objets.obj_grid->FixedRows = 1;

	for(i=0; i<7; i++ )
		Objets.obj_grid->ColWidths[i] = widths[i];

	for( i=0; i<7; i++ )
		Objets.obj_grid->Cells[0][i] = Idents[i];

	Redraw();
	Plugin->RunPAKBrowser( CurObj->Nom );
	return true;	
}

/* ******************************************************************** */

void
Redraw()
{
	int i;
	struct {
		long  ID;
		char  buf[256];
		int   Grp;

	} CNV;

	if( CurAnim == NULL )
	{
		LPAnimList l = ImportAnims->Liste;
		Objets.ImportList->Clear();
		while( l != NULL )
		{
			Objets.ImportList->AddItem( l->Name );
			l = l->Suivant;
		}
		return;
	}

	Objets.label_nom->Caption = CurAnim->Name;

	for( i=0; i<3; i++ )
	{
		CNV.ID = CurAnim->params[i];
		CNV.Grp = i;
		if( !Plugin->RunPlugin( "symbols.dll","ConvertSymbol", &CNV ) )
			_ltoa( CurAnim->params[i], CNV.buf, 10 );
		Objets.anm_params[i]->Text = CNV.buf;
	}

	Objets.obj_list->Clear();
	for(i=0; i< CurAnim->NbObjs; i++ )
		Objets.obj_list->AddItem(CurAnim->Objs[i]->Nom );


	if( CurObj != NULL )
	{
		Objets.obj_grid->Visible =  true;
		Objets.move_up_frame->Visible = true;
		Objets.delete_frame->Visible = true;
		Objets.obj_lab_debut->Visible = true;
		Objets.obj_lab_fin->Visible = true;

		for( i=0; i< CurObj->FrameCount; i++ )
		{
			Objets.obj_grid->Cells[i+1][0] = (i+1);
			Objets.obj_grid->Cells[i+1][1] = (int)CurObj->Frames[i]->Time;
			Objets.obj_grid->Cells[i+1][2] = Plugin->CnvIDToFile( CurObj->Frames[i]->IDSprite);
			Objets.obj_grid->Cells[i+1][3] = CurObj->Frames[i]->dx;
			Objets.obj_grid->Cells[i+1][4] = CurObj->Frames[i]->dy;

			CNV.ID = CurObj->Frames[i]->Flag;
			CNV.Grp = 3;
			if( !Plugin->RunPlugin( "symbols.dll","ConvertSymbol", &CNV ) )
				_ltoa( CurObj->Frames[i]->Flag, CNV.buf, 10);
			Objets.obj_grid->Cells[i+1][5] = CNV.buf;
			CNV.ID = CurObj->Frames[i]->PFlag;
			CNV.Grp = 4;
			if( !Plugin->RunPlugin( "symbols.dll","ConvertSymbol", &CNV ) )
				_ltoa( CurObj->Frames[i]->PFlag, CNV.buf, 10);
			Objets.obj_grid->Cells[i+1][6] = CNV.buf;

		}
		for( i = 0; i< 7 ; i++ )
			Objets.obj_grid->Cells[CurObj->FrameCount + 1][i] = "";
	}
	else
	{
		Objets.obj_grid->Visible =  false;
		Objets.move_up_frame->Visible = false;
		Objets.delete_frame->Visible = false;
		Objets.obj_lab_debut->Visible = false;
		Objets.obj_lab_fin->Visible = false;	
	}
}

/* ******************************************************************** */

bool
OnDropPAKItem( LPCSTR anim, LPCSTR id )
{
	int i = (WORD)*((DWORD *)id);
	LPAnimList l = Anims.FindByName( (char*)anim );

	if( l == NULL )
	{
		Plugin->LogText("ERROR: animation not found");
		return false;
	}
	if( l != CurAnim )
	{
		CurAnim = l;
		CurObj = NULL;
	}
	if( CurObj == NULL )
	{
		Plugin->LogText("Object not seleted");
		return false;
	}
	Anims.AddFrame( CurObj, *((DWORD *)id) );
	Objets.obj_grid->RowCount=CurObj->FrameCount+2;
	Redraw();
	return true;
}

/* ******************************************************************** */

bool
OnSaveProject( LPCSTR )
{
	if( !Anims.Save() )
	{
		Plugin->LogText("Failed to save animation file" );
		return false;
	}
	return true;
}

/* ******************************************************************** */

bool
OnLoadProject( LPCSTR )
{
	FILE *f;
	char buf[256];

	if( Anims.Nom != NULL )
	{
		f = fopen( Anims.Nom, "rb" );
		if( f == NULL )
		{
			strcpy(buf,"Cannot open Animation File: ");
			strcat(buf, Anims.Nom );
			Plugin->LogText( buf);
		}
		else
		{
			fclose(f);
			Plugin->LogText("Loading Animation File [DONE]");
		}
	}
	Anims.InitDone();
	return true;
}

/* ******************************************************************** */

bool
ParamChange( OBJECT obj, void *Key )
{
	int i;
	WORD w;

	if( (int)Key != 13 )
		return false;

	for( i=0; i< 3; i++ )
		if( Objets.anm_params[i]==obj )
		{
			struct {
				long ID;
				char buf[256];
				int  group;
			} test;
			w = atoi(Objets.anm_params[i]->Text.c_str());
			CurAnim->params[i] = w;
			test.ID = w;
			test.group = i;
			if( !Plugin->RunPlugin("symbols.dll","ConvertSymbol", &test) )
				_ltoa( w, test.buf, 10 );
			((Edit)obj)->Text = test.buf;
			return true;
		}
	return false;
}

/* ******************************************************************** */

bool
GetIndex( OBJECT  )
{
	CurFrame = Objets.obj_grid->Selection.top-1;
	return true;
}

/* ******************************************************************** */

bool
MoveUpFrame( OBJECT )
{
	if( CurObj == NULL )
		return false;
	if( CurFrame > 0 && CurFrame < CurObj->FrameCount )
	{
		LPAnim a;
		a = CurObj->Frames[ CurFrame ];
		CurObj->Frames[ CurFrame ] = CurObj->Frames[ CurFrame-1 ];
		CurObj->Frames[ CurFrame-1 ] = a;
		Redraw();
		return true;
	}
	return false;
}

/* ******************************************************************** */

bool
DeleteFrame( OBJECT )
{
	if( CurObj == NULL )
		return false;
	if( CurFrame >= 0 && CurFrame < CurObj->FrameCount )
	{
		int i;
		delete CurObj->Frames[CurFrame];
		for(i=CurFrame; i< CurObj->FrameCount-1; i++ )
			CurObj->Frames[i] = CurObj->Frames[i+1];
		CurObj->FrameCount--;
		Redraw();
		return true;
	}
	return false;
}

/* ******************************************************************** */

bool
Duplicate( OBJECT )
{
	LPAnimList a;
	int i,j;
	DString buf;

	if( CurAnim == NULL )
		return false;
	if( ! Plugin->InputText("New Anim Name:", CurAnim->Name, &buf ) )
		return false;
	if( Anims.FindByName( buf.c_str() ) != NULL )
	{
		Plugin->LogText("This name is already used");
		return false;
	}

	if( Plugin->CreateItem(buf.c_str() ) )
	{
		a = Anims.CreateEmpty( buf.c_str() );
		a->params[0] = CurAnim->params[0];
		a->params[1] = CurAnim->params[1];
		a->params[2] = CurAnim->params[2];
		for( i=0; i< CurAnim->NbObjs; i++ )
		{
			Anims.AddObj( a, CurAnim->Objs[i]->Nom );
			a->Objs[i]->CodeDebut = CurAnim->Objs[i]->CodeDebut;
			a->Objs[i]->CodeFin = CurAnim->Objs[i]->CodeFin;
			a->Objs[i]->ParamDebut = CurAnim->Objs[i]->ParamDebut;
			a->Objs[i]->ParamFin = CurAnim->Objs[i]->ParamFin;


			if( a->Objs[i]->CodeDebut == SYNCHRO )
			{
				int j;
				for(j=0; j< CurAnim->NbObjs; j++ )
					if( CurAnim->Objs[i]->ParamDebut == (DWORD)CurAnim->Objs[j] )
						break;
				if( j == CurAnim->NbObjs )
				{
					a->Objs[i]->ParamDebut = 0;
					a->Objs[i]->CodeDebut = NORMAL;
				}
				else
					a->Objs[i]->ParamDebut = j;
			}
			for( j=0; j< CurAnim->Objs[i]->FrameCount; j++ )
			{
				Anims.AddFrame( a->Objs[i], CurAnim->Objs[i]->Frames[j]->IDSprite );
				memcpy(a->Objs[i]->Frames[j], CurAnim->Objs[i]->Frames[j], sizeof(TAnim) );
			}
		}
		for( i=0; i< CurAnim->NbObjs; i++ )
			if( a->Objs[i]->CodeDebut == SYNCHRO )
				a->Objs[i]->ParamDebut = (DWORD)a->Objs[ a->Objs[i]->ParamDebut ];
	}
	return true;
}

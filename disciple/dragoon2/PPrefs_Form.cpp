//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "PPrefs_Form.h"
#include "PRJ_Defs.h"
#include "Main_Form.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TPPrefForm *PPrefForm;
//---------------------------------------------------------------------------
__fastcall TPPrefForm::TPPrefForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TPPrefForm::FormClose(TObject *Sender, TCloseAction &Action)
{
     MainForm->Enabled = true;
     PPrefForm->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TPPrefForm::FormShow(TObject *Sender)
{
     PFile->Caption = Projet->PathProjet+Projet->NomProjet;
     PName->Text = Projet->Prefs.Name;
     PDesc->Text = Projet->Prefs.Describe;
     DefPath->Text = Projet->Prefs.DefPath;

     if( Projet->Prefs.Version%100 == 0 )
         PVer->Text = (AnsiString)(int)(Projet->Prefs.Version/100)+(AnsiString)".00";
     else
     {
         if( Projet->Prefs.Version%100 < 10 )
             PVer->Text = (AnsiString)(int)(Projet->Prefs.Version/100)+(AnsiString)".0"+(AnsiString)(int)(Projet->Prefs.Version%100);
         else
             PVer->Text = (AnsiString)(int)(Projet->Prefs.Version/100)+(AnsiString)"."+(AnsiString)(int)(Projet->Prefs.Version%100);
     }

     PVerInc->Checked = Projet->Prefs.VerInc;
}
//---------------------------------------------------------------------------
void __fastcall TPPrefForm::CANCELClick(TObject *Sender)
{
     Close();
}
//---------------------------------------------------------------------------
void __fastcall TPPrefForm::OKClick(TObject *Sender)
{
     AnsiString v = PVer->Text;

     Projet->PathProjet+Projet->NomProjet = PFile->Caption;
     Projet->Prefs.Name = PName->Text;
     Projet->Prefs.Describe = PDesc->Text;
     Projet->Prefs.DefPath = DefPath->Text;

     if( v.Pos(".") > 0 )
          v.Delete( v.Pos("."), 1 );
     if( v.Length() == 1 )
          v = v+"00";
     if( v.Length() == 2 )
          v = v+"0";
     if( atoi(v.c_str()) > 0 )
          Projet->Prefs.Version = atoi(v.c_str());
     Projet->Prefs.VerInc = PVerInc->Checked;

     Close();
}
//---------------------------------------------------------------------------
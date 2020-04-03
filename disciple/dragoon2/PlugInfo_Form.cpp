//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "PlugInfo_Form.h"
#include "Plugins.h"
#include "Main_Form.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TPlugInfoForm *PlugInfoForm;
//---------------------------------------------------------------------------
__fastcall TPlugInfoForm::TPlugInfoForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPlugInfoForm::FormClose(TObject *Sender, TCloseAction &Action)
{
     MainForm->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TPlugInfoForm::FormShow(TObject *Sender)
{
     LPPlugin p = Plugins->liste;
     Plugs->Clear();
     while( p != NULL )
     {
         Plugs->Items->Add( p->inf->Name );
         p = p->Suivant;
     }
     Plugs->ItemIndex = 0;
     PlugsChange( Sender );
}
//---------------------------------------------------------------------------
void __fastcall TPlugInfoForm::PlugsChange(TObject *Sender)
{
     LPPlugin p = Plugins->liste;
     AnsiString a,b;
     int i;

     for( i=0; i< Plugs->ItemIndex; i++ )
     {
          p = p->Suivant;
          if( p == NULL )
              return;
     }
     FileName->Caption = p->File;
     PName->Caption = p->inf->Name;
     Author->Caption = p->inf->Author;
     Infos->Clear();

     a = p->inf->Infos;
     while( a.Pos("\n") > 0 )
     {
         i = a.Pos("\n");
         b = a;
         b.Delete(i,b.Length());
         Infos->Lines->Add( b);
         a.Delete(1,i);
     }
     Infos->Lines->Add( a );

}
//---------------------------------------------------------------------------

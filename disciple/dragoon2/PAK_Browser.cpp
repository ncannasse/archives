//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "PAK_Browser.h"
#include "PRJ_Defs.h"
#include "Main_Form.h"
#include "Console.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TPAKBrowser *PAKBrowser;
CPak *curp;
//---------------------------------------------------------------------------
__fastcall TPAKBrowser::TPAKBrowser(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPAKBrowser::CloseClick(TObject *Sender)
{
     Visible = false;
}
//---------------------------------------------------------------------------
void *
AddToList( void *it, void * )
{
     LPPakItem i = (LPPakItem)it;
     PAKBrowser->PAKList->Items->Add( i->Nom );
     return NULL;
}
//---------------------------------------------------------------------------
void TPAKBrowser::Open( AnsiString Nom )
{
     int i;
     AnsiString n;
     Nom = Nom.LowerCase();
     for(i=0; i< Projet->PakCount; i++ )
     {
          n = Projet->Paks[i]->PakName;
          n = n.LowerCase();
          if( n == Nom )
          {
              curp = Projet->Paks[i];
              PAKList->Clear();
              curp->EnumItems( AddToList, NULL );
              PakName->Text = Nom;
              Visible = true;
              return;
          }
     }
}
//---------------------------------------------------------------------------
void __fastcall TPAKBrowser::PAKListClick(TObject *Sender)
{
     if( PAKList->Items->Count == 0 || PAKList->ItemIndex == -1 )
         return;
     LPPakItem i = curp->FindItem( PAKList->Items->Strings[ PAKList->ItemIndex ] );
     MainForm->PAKDrag = true;
     MainForm->PAKItemID = ((curp->PakNb+1)<<16) + i->ID;
     MainForm->ListeDragDrop( NULL, NULL, 0, (MainForm->Liste->ItemIndex-MainForm->Liste->TopIndex)*MainForm->Liste->ItemHeight );

}
//---------------------------------------------------------------------------
void __fastcall TPAKBrowser::PakNameChange(TObject *Sender)
{
     Open( PakName->Text );
}
//---------------------------------------------------------------------------
void __fastcall TPAKBrowser::PakNameKeyUp(TObject *Sender, WORD &Key,
        TShiftState Shift)
{
     // completition
     int p = 0;
     int sp;
     int i;
     AnsiString n = PakName->Text.LowerCase();
     AnsiString pname;

     if(Key == 8 )
            return;

     for(i=0; i< Projet->PakCount; i++ )
     {
        pname = Projet->Paks[i]->PakName.LowerCase();
        if( pname.Length() >= n.Length() && memcmp(pname.c_str(),n.c_str(),n.Length()) == 0 )
        {
            p++;
            sp = i;
        }
     }
     if( p == 1 && PakName->Text != Projet->Paks[sp]->PakName )
         PakName->Text = Projet->Paks[sp]->PakName;
        
}
//---------------------------------------------------------------------------
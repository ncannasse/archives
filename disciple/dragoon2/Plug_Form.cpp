//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Plug_Form.h"
#include "PAK_Form.h"
#include "Plugins.h"
#include "Console.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TPlugForm *PlugForm;
//---------------------------------------------------------------------------
__fastcall TPlugForm::TPlugForm(TComponent* Owner)
        : TForm(Owner)
{
     Plugin = NULL;
     Plugins = new CPlugins;
     Plugins->Charge();
}
//---------------------------------------------------------------------------
void __fastcall TPlugForm::PlugEditPressKey(TObject *Sender, WORD &Key,
        TShiftState Shift)
{
     LPPlugObj o = Plugins->GetPlugObj(NULL,Sender);
     if( o == NULL )
     {
         Log("ERROR: PlugObject is not referenced by any plugin");
         return;
     }
     o->Event( Sender, (void *)EVT_KPRESS, o->Class, (void *)Key );
}
//---------------------------------------------------------------------------
void __fastcall TPlugForm::FormShow(TObject *Sender)
{
     Plugins->AfficheObjs( Plugin );
}
//---------------------------------------------------------------------------
void __fastcall TPlugForm::PlugEditChange(TObject *Sender)
{
     LPPlugObj o = Plugins->GetPlugObj(NULL,Sender);
     if( o == NULL )
     {
         Log("ERROR: PlugObject is not referenced by any plugin");
         return;
     }
     o->Event( Sender, (void *)EVT_CHANGE, o->Class );
}
//---------------------------------------------------------------------------
void __fastcall TPlugForm::FormResize(TObject *Sender)
{
    HidePlugForm->Left = Width-30;
    HidePlugForm->Top = 0;
}
//---------------------------------------------------------------------------
void __fastcall TPlugForm::HidePlugFormClick(TObject *Sender)
{
     Visible = false;        
}
//---------------------------------------------------------------------------

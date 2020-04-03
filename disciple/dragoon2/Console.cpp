//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Console.h"
#include "Main_Form.h"
#include "Main_Defs.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TConsoleForm *ConsoleForm;
//---------------------------------------------------------------------------
__fastcall TConsoleForm::TConsoleForm(TComponent* Owner)
: TForm(Owner)
{
    Width = LogList->Width+6;
    Height = LogList->Height+25;
}
//---------------------------------------------------------------------------
void __fastcall TConsoleForm::LogListDblClick(TObject *Sender)
{
    if( LogList->ItemIndex == -1 )
        return;
    MessageBox( Handle, LogList->Items->Strings[ LogList->ItemIndex ].c_str(), "Message:", MB_OK );
}
//---------------------------------------------------------------------------
void TConsoleForm::LogAdd( AnsiString a )
{
     LogList->Items->Add(a);
     if( LogList->Items->Count > 5 )
         LogList->TopIndex = LogList->Items->Count-5;
}

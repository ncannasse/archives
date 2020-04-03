//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Input_Form.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TInputForm *InputForm;
AnsiString RTxt;
bool r;
//---------------------------------------------------------------------------
__fastcall TInputForm::TInputForm(TComponent* Owner)
: TForm(Owner)
{
}
//---------------------------------------------------------------------------
bool
TInputForm::InputLn( TForm *Form, AnsiString CaptTxt, AnsiString DefTxt, AnsiString *out )
{
    Caption = CaptTxt;
    Texte->Text = DefTxt;
    Visible = true;
    r = true;
    Form->Enabled = false;
    RTxt = "¤?¤";
    Texte->SetFocus();
    while( RTxt == "¤?¤" )
		Application->ProcessMessages();
    Form->Enabled = true;
    Visible = false;
    *out = RTxt;
    return r;
}
//---------------------------------------------------------------------------

void __fastcall TInputForm::OKClick(TObject *Sender)
{
	int i;
	for( i=1; i<= Texte->Text.Length(); i++ )
		switch( Texte->Text[i] )
	{
          case '?':
          case '\\':
          case '/':
          case ':':
          case '*':
          case '<':
          case '>':
          case '|':
          case '"':
			  MessageBox( Handle, "This text contains invalid caracters", "Error", MB_OK );
			  return;
          default:
			  break;
	}
	RTxt = Texte->Text;
}
//---------------------------------------------------------------------------
void __fastcall TInputForm::CancelClick(TObject *Sender)
{
	RTxt = "";
        r = false;
}
//---------------------------------------------------------------------------
void __fastcall TInputForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	RTxt = "";
        r = false;
}
//---------------------------------------------------------------------------
void __fastcall TInputForm::TexteKeyDown(TObject *Sender, WORD &Key,
        TShiftState Shift)
{
   if( Key == 13 )
   {
       Key == 0;
       OKClick( Sender );
   }
   if( Key == 27 )
       CancelClick( Sender );
}
//---------------------------------------------------------------------------
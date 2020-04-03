//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Load_Form.h"
#define LOAD             0
#define SAVE             1
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TLoadForm *LoadForm;
AnsiString NomFichier;
AnsiString MaskTxt;
int        Mode;

//---------------------------------------------------------------------------
__fastcall TLoadForm::TLoadForm(TComponent* Owner)
: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TLoadForm::CancelClick(TObject *Sender)
{
    NomFichier = "";
}
//---------------------------------------------------------------------------
void __fastcall TLoadForm::FileNameKeyUp(TObject *Sender, WORD &Key,
                                         TShiftState Shift)
{
    if( Key == 13 )
        ActionClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TLoadForm::ActionClick(TObject *Sender)
{
    switch( Mode )
    {
    case LOAD:
        if( Files->ItemIndex > -1 && Files->Items->Count > 0 )
            NomFichier = Files->FileName;
        break;
    case SAVE:
        AnsiString a;
        FILE *f;
        if( FileName->Text.Length() == 0 )
            break;
        a = FileName->Text.LowerCase();
        if( a.Pos(MaskTxt) <= 0 )
            FileName->Text = FileName->Text + MaskTxt;
        a = Dirs->Directory;
        if( a.Length() > 3 )
            a = a + "\\";
        a = a+FileName->Text;
        f = fopen(a.c_str(), "rb");
        if( f != NULL )
        {
            if( MessageBox(Handle, "Files already exists....\nOverwrite it ?", a.c_str(),MB_YESNO) == IDYES )
                NomFichier = a;
            fclose(f);
            break;
        }
        f = fopen(a.c_str(), "wb");
        if( f == NULL )
        {
            MessageBox(Handle,a.c_str(),"File name is not valid",MB_OK );
        }
        else
        {
            fclose(f);
            DeleteFile(a.c_str());
            NomFichier = a;
        }
        break;
    }
}
//---------------------------------------------------------------------------
AnsiString TLoadForm::Load( TForm *Form, AnsiString FileMask, AnsiString Title )
{
    Caption = Title;
    Panel->Width = Files->Left+Files->Width+10;
    Width = Panel->Width+20;
    Cancel->Left = 248;
    Action->Caption = "Load";
    FileName->Visible = false;
    Dirs->Height = 193;
    Files->Mask = FileMask;
    Visible = true;
    Form->Enabled = false;
    MaskTxt = FileMask;
    Mode = LOAD;
    NomFichier = "?";
    while( NomFichier == "?" )
        Application->ProcessMessages();
    Form->Enabled = true;
    Visible = false;
    return NomFichier;
}
//---------------------------------------------------------------------------
AnsiString TLoadForm::Save( TForm *Form, AnsiString FileMask, AnsiString Title, AnsiString ActName )
{
    Caption = Title;
    Panel->Width = FileName->Left+FileName->Width+10;
    Width = Panel->Width+20;
    Dirs->Height = 161;
    Action->Caption = ActName;
    Cancel->Left = Action->Left+Action->Width+15;
    FileName->Visible = true;
    Visible = true;
    Form->Enabled = false;
    MaskTxt = FileMask.LowerCase();
    MaskTxt.Delete(1,1);
    Mode = SAVE;
    NomFichier = "?";
    while( NomFichier == "?" )
        Application->ProcessMessages();
    Form->Enabled = true;
    Visible = false;
    return NomFichier;
}

void __fastcall TLoadForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    NomFichier = "";
}
//---------------------------------------------------------------------------
void __fastcall TLoadForm::FilesDblClick(TObject *Sender)
{
    ActionClick(Sender);   
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Main_Defs.h"
#include "Main_Form.h"
#include "Types_Form.h"
#include "Input_Form.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TTypesForm *TypesForm;
//---------------------------------------------------------------------------
__fastcall TTypesForm::TTypesForm(TComponent* Owner)
: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TTypesForm::FormShow(TObject *Sender)
{
    int i;
    Panel->Caption = CurType->Identif;
    Ident->Text = CurType->Identif;
    ID->Text = (int)CurType->ID;
    Masks->Text = CurType->Masks;
    DataPos->Text = CurType->DataPos;
    DataLen->Text = CurType->DataLen;
    Data->Clear();
    for(i=0;i<CurType->DataLen;i++)
        Data->Items->Add( (int)CurType->Data[i] );
}
//---------------------------------------------------------------------------
void __fastcall TTypesForm::AddDataClick(TObject *Sender)
{
    AnsiString nb;
    BYTE b;
    BYTE *newdata;

    if( !Input( this, "Enter a Value (0-255) or a char: ", "0", &nb ) )
        return;
    if( (b=(BYTE)atoi(nb.c_str()))>0 || nb.Length() == 1 || nb == "00" )
    {
        if( nb.Length() == 1 )
            b = (BYTE)nb[1];
        newdata = new BYTE[ CurType->DataLen + 1 ];
        memcpy( newdata, CurType->Data, CurType->DataLen );
        newdata[ CurType->DataLen ] = b;
        Data->Items->Add( (int)b );
        delete CurType->Data;
        CurType->Data = newdata;
        CurType->DataLen++;
        DataLen->Text = CurType->DataLen;
    }
    
}
//---------------------------------------------------------------------------
void __fastcall TTypesForm::DelDataClick(TObject *Sender)
{
    if( Data->Items->Count == 0 || Data->ItemIndex == -1 )
        return;
    memcpy( CurType->Data+Data->ItemIndex, CurType->Data+Data->ItemIndex+1, CurType->DataLen-1-Data->ItemIndex );
    Data->Items->Delete( Data->ItemIndex );
    CurType->DataLen--;
    DataLen->Text = CurType->DataLen;
}
//---------------------------------------------------------------------------
void __fastcall TTypesForm::OKClick(TObject *Sender)
{
    CurType->Identif = Ident->Text;
    CurType->ID = (BYTE)atoi(ID->Text.c_str());
    CurType->Masks = Masks->Text;
    CurType->DataPos = atoi(DataPos->Text.c_str());
    FormShow(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TTypesForm::DataPosChange(TObject *Sender)
{
    DataPos->Text = atoi( DataPos->Text.c_str() );
}
//---------------------------------------------------------------------------
void __fastcall TTypesForm::IDChange(TObject *Sender)
{
    ID->Text = (BYTE)atoi( ID->Text.c_str() );
    if( ID->Text == "0" )
        ID->Text = "255";
}
//---------------------------------------------------------------------------

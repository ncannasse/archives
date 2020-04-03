//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Main_Defs.h"
#include "Main_Form.h"
//---------------------------------------------------------------------------
void
EcritTxt( AnsiString a, FILE *f )
{
    fwrite( a.c_str(), 1, a.Length(), f );
}
//---------------------------------------------------------------------------
void
SautLigne( FILE *f )
{
    char c[2] = { 13, 10 };
    fwrite( c, 1, 2, f);
}
//---------------------------------------------------------------------------
void
PosSubForm( TForm *f )
{
    f->Left = MainForm->Left+MainForm->ListPanel->Left+MainForm->ListPanel->Width+3;
    f->Top = MainForm->Top + 42;
    f->Width = MainForm->Width-f->Left+MainForm->Left-2;
    f->Height = MainForm->Height-46;
}
//---------------------------------------------------------------------------



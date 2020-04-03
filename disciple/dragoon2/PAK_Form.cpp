//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "PAK_Form.h"
#include "PRJ_Defs.h"
#include "Input_Form.h"
#include "Main_Form.h"
//---------------------------------------------------------------------------
#pragma link "Grids"
#pragma resource "*.dfm"
TPAKForm *PAKForm;
//---------------------------------------------------------------------------
__fastcall TPAKForm::TPAKForm(TComponent* Owner)
: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void *ListItems( void * p, void * )
{
    LPPakItem pa = (LPPakItem )p;

    PAKForm->PAKItems->Cells[0][ PAKForm->PAKItems->RowCount-1 ] = (int)pa->ID;
    PAKForm->PAKItems->Cells[1][ PAKForm->PAKItems->RowCount-1 ] = pa->Nom + ((pa->Symbol == "")?(AnsiString)(""):(" (" + pa->Symbol+")"));
    PAKForm->PAKItems->Cells[2][ PAKForm->PAKItems->RowCount-1 ] = Projet->FTypes->GetIdent( pa->Type );
    PAKForm->PAKItems->RowCount++;
    return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::FormShow(TObject *Sender)
{
    PAKItems->RowCount = 2;
    CurPak->EnumItems( ListItems, NULL );
    while( PAKItems->RowCount < 21 )
    {
        PAKItems->Cells[0][ PAKItems->RowCount-1 ] = "";
        PAKItems->Cells[1][ PAKItems->RowCount-1 ] = "";
        PAKItems->Cells[2][ PAKItems->RowCount-1 ] = "";
        PAKItems->RowCount++;
    }
    PAKItems->Cells[0][ PAKItems->RowCount-1 ] = "";
    PAKItems->Cells[1][ PAKItems->RowCount-1 ] = "";
    PAKItems->Cells[2][ PAKItems->RowCount-1 ] = "";
    PAKItems->Cells[0][0]=" ID";
    PAKItems->Cells[1][0]=" FileName / Symbol";
    PAKItems->Cells[2][0]=" Type";
    Caption = "";
    Panel->Caption = CurPak->PakName;
    CurPak->Save();
    if( Sender != NULL )
        FilesChange( Sender );
}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::FilesChange(TObject *Sender)
{
    int i;
    AnsiString Path = Files->Directory;
    LPPakItem item;
    BYTE FType;
    LPType t;

    FILE *f;
    BYTE *Data;
    DWORD Size;

    if( Path.Length()>3 )
        Path = Path + "\\";
    FList->Clear();
    for(i=0; i< Files->Items->Count; i++ )
    {
        if( (FType = Projet->FTypes->FGetType( Path+Files->Items->Strings[i] )) != 0 )
        {
            t = Projet->FTypes->GetType(FType);
            item = CurPak->FindItem( Files->Items->Strings[i] );
            if( item == NULL )
                FList->Items->Add("N"+Files->Items->Strings[i] );
            else
            {
                f = fopen( (Path+Files->Items->Strings[i]).c_str(), "rb" );
                if( f != NULL )
                {
                    fseek( f, 0, 2 );
                    Size = ftell(f)-t->DataLen;
                    if( Size != item->Len )
                        FList->Items->Add("U"+Files->Items->Strings[i] );
                    else
                    {
                        fseek( f, 0, 0 );
                        Data = new BYTE[Size];
                        fread( Data, 1, t->DataPos, f );
                        fseek( f, t->DataPos+t->DataLen, 0 );
                        fread( Data+t->DataPos, 1, Size-t->DataPos, f );
                        if( memcmp( Data, item->Data, Size ) != 0 )
                           FList->Items->Add("U"+Files->Items->Strings[i] );
                        delete Data;
                    }
                    fclose(f);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::AddUpdateClick(TObject *Sender)
{
    int i;
    LPPakItem it;
    AnsiString fullpath, nom;

    fullpath = Files->Directory;
    if( fullpath.Length()>3 )
         fullpath = fullpath + "\\";

    for( i=0; i< FList->Items->Count; i++ )
        if( FList->Selected[i] )
        {
            nom = FList->Items->Strings[i];
            nom.Delete(1,1);
            it = CurPak->FindItem( nom );
            if( it == NULL )
                CurPak->AddItem( fullpath+nom );
            else
                CurPak->UpdateItem( it, fullpath+nom );
            FList->Items->Strings[i] = "";
        }

    for( i=0; i< FList->Items->Count; i++ )
        if( FList->Items->Strings[i] == "" )
        {
            FList->Items->Delete(i);
            i = -1;
        }

   FormShow(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::FListDrawItem(TWinControl *Control, int Index,
   TRect &Rect, TOwnerDrawState State)
{
   TListBox *ListBox = dynamic_cast<TListBox*>(Control);
   TCanvas *Canvas = ListBox->Canvas;
   AnsiString L=ListBox->Items->Strings[Index];

   Canvas->FillRect(Rect);
   switch( L[1] )
   {
   case 'N':
        if( !ListBox->Selected[Index] )
             Canvas->Font->Color = clBlack;
        break;
   case 'U':
        Canvas->Font->Color = clRed;
        break;
   }
   L.Delete(1,1);
   Canvas->TextOut(Rect.Left + 2, Rect.Top, L);

}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::FListDblClick(TObject *Sender)
{
     AddUpdateClick( Sender );
}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::FListKeyUp(TObject *Sender, WORD &Key,
        TShiftState Shift)
{
     if( Key == 13 )
         AddUpdateClick( Sender );
}
//---------------------------------------------------------------------------
void *
CountItems( void *i, void *index )
{
    if( *((int *)index) == 0 )
        return i;
    (*((int *)index))--;
    return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::OverwriteClick(TObject *Sender)
{
    if( PAKItems->Cells[0][PAKItems->Selection.Top] != "" && FList->ItemIndex > -1 && FList->Items->Count>0 )
    {
        LPPakItem it;
        AnsiString fullpath, nom;
        int index = PAKItems->Selection.Top-1;

        fullpath = Files->Directory;
        if( fullpath.Length()>3 )
            fullpath = fullpath + "\\";

        nom = FList->Items->Strings[ FList->ItemIndex ];
        nom.Delete(1,1);

        it = (LPPakItem)CurPak->EnumItems( CountItems, &index );
        if( it != NULL )
        {
            CurPak->UpdateItem( it, fullpath+nom );
            FList->Items->Delete( FList->ItemIndex );
            FormShow(NULL);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::ChangeIDClick(TObject *Sender)
{
     AnsiString a;
     LPPakItem i;
     int pos =  PAKItems->Selection.Top-1;

     i = (LPPakItem )CurPak->EnumItems( CountItems, &pos );
     if( i != NULL )
     {
         if( !Input( this, "Enter a value: ", (int)i->ID, &a ) )
             return;
         if( a != "" )
         {
             i->ID = (WORD)atoi( a.c_str() );
             PAKItems->Cells[0][PAKItems->Selection.Top] = (int)i->ID;
             CurPak->Save();
         }
     }
}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::ChangeSymbolClick(TObject *Sender)
{
     AnsiString a;
     LPPakItem i;
     int pos =  PAKItems->Selection.Top-1;

     i = (LPPakItem )CurPak->EnumItems( CountItems, &pos );
     if( i != NULL )
     {
         AnsiString isymb;
         if( ! Input( this, "Enter a symbol: ", i->Symbol, &isymb ) )
             return;
         i->Symbol = isymb;
         PAKItems->Cells[1][PAKItems->Selection.Top] = i->Nom + ((i->Symbol == "")?(AnsiString)(""):(" (" + i->Symbol+")"));
         CurPak->Save();
     }
}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::DeleteClick(TObject *Sender)
{
     AnsiString a;
     LPPakItem i;
     int pos =  PAKItems->Selection.Top-1;

     i = (LPPakItem )CurPak->EnumItems( CountItems, &pos );
     if( i != NULL )
     {
         CurPak->DeleteItem( i );          
         FormShow(Sender);
     }
}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::PAKItemsMouseDown(TObject *Sender,
        TMouseButton Button, TShiftState Shift, int X, int Y)
{
     if( Button == mbLeft )
     {
         MainForm->PAKDrag = true;
         MainForm->PAKName = CurPak->PakName;
         MainForm->PAKItemID = atoi( PAKItems->Cells[0][ PAKItems->Selection.Top ].c_str() )+((CurPak->PakNb+1)<<16);
         PAKItems->BeginDrag( false );
     }

}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::PAKItemsMouseUp(TObject *Sender, TMouseButton Button,
        TShiftState Shift, int X, int Y)
{
     if( Button == mbRight )
     {
         MainForm->PAKDrag = false;
         PAKItems->EndDrag( false );
     }

}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::PAKSaveClick(TObject *Sender)
{
     CurPak->Save();
}
//---------------------------------------------------------------------------
void __fastcall TPAKForm::ExportClick(TObject *Sender)
{
     int pos =  PAKItems->Selection.Top-1;
     LPPakItem i;
     i = (LPPakItem )CurPak->EnumItems( CountItems, &pos );
     if( i != NULL )
         CurPak->ExportItem( i );
}
//---------------------------------------------------------------------------

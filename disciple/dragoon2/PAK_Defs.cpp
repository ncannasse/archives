//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Console.h"
#include "Main_Defs.h"
#include "PAK_Defs.h"
#include "PRJ_Defs.h"
#include "Types_Defs.h"
#include "Plugins.h"
//---------------------------------------------------------------------------
CPak::CPak( AnsiString nom, AnsiString path, WORD Nb )
{
    Items = NULL;
    Count = 0;
    Folder = 0;
    PakName = nom;
    PathName = path;
    PakNb = Nb;
}
//---------------------------------------------------------------------------
CPak::~CPak()
{
    Clean();
}
//---------------------------------------------------------------------------
void
CPak::Clean()
{
    int i;
    for(i=0; i<Count; i++)
    {
        delete Items[i]->Data;
        delete Items[i];
    }
    delete Items;
    Items = NULL;
    Count = 0;
}
//---------------------------------------------------------------------------
void *
CPak::EnumItems( ENUMPROC proc, void *param )
{
    int i;
    void *r;

    for( i = 0; i< Count; i++ )
        if( (r=proc( Items[i], param ) ) != NULL )
            return r;

    return NULL;
}
//---------------------------------------------------------------------------
void
CPak::Load()
{
    FILE *f = fopen( (PathName+PakName+".pak").c_str(), "rb");
    char ctr[256];
    DWORD d;
    WORD w;
    BYTE b;
    int i;

    if( f == NULL )
    {
        Log("Cannot open PAK file : ");
        LogPlus( PakName+".pak" );
        return;
    }
    fread( ctr, 1, 3, f );
    ctr[3] = 0;
    if( strcmp(ctr,"PAK") != 0 )
    {
        Log("Invalid PAK file : ");
        LogPlus( PakName+".pak" );
        fclose(f);
        return;
    }
    fread( &PakNb, 1, 2, f );
    fread( &w, 1, 2, f );
    Count = w;
    Items = new LPPakItem[Count];

    for( i = 0; i< Count; i++ )
    {
        Items[i] = new TPakItem;
        fread( &Items[i]->ID, 1, 2, f);
        fread( &b, 1, 1, f);
        fread( ctr, 1, b, f);
        ctr[b] = 0;
        Items[i]->Symbol = ctr;
        fread( &b, 1, 1, f);
        fread( ctr, 1, b, f);
        ctr[b] = 0;
        Items[i]->Nom = ctr;
        fread( &Items[i]->Type, 1, 1, f);
        fread( &Items[i]->Len, 1, 4, f);
        Items[i]->Data = new BYTE[ Items[i]->Len ];
        fread( Items[i]->Data, 1, Items[i]->Len, f );
        fread( &d, 1, 4, f );
        if( d != 0xDAAF0BBC )
        {
            Log("Invalid PAK file : ");
            LogPlus( PakName+".pak" );
            fclose(f);
            return;
        }
    }
    fclose(f);
    Plugins->SendMainEvent( EVT_PAKCHANGE, (PathName+PakName+".pak").c_str() );
}
//---------------------------------------------------------------------------
void
CPak::SaveSymbols( FILE *f )
{
    int i,j;
    EcritTxt("// Pak symbols of: "+PakName+" ("+(AnsiString)(int)PakNb+")", f);
    SautLigne(f);
    for( i = 0; i< Count; i++ )
         if( Items[i]->Symbol != "" )
         {
             EcritTxt("#define "+Items[i]->Symbol, f);
             for( j = 0; j< 50-Items[i]->Symbol.Length(); j++)
                  EcritTxt(" ", f);
             EcritTxt( "("+(AnsiString)(int)(PakNb+1),f);
             EcritTxt("<<16)+"+(AnsiString)(int)Items[i]->ID,f);
             SautLigne(f);
         }

    SautLigne(f);
}
//---------------------------------------------------------------------------
void
CPak::Save()
{
    FILE *f = fopen( (PathName+PakName+".pak").c_str(), "wb");
    DWORD d;
    WORD w;
    BYTE b;
    int i;
    int ID;

    if( f == NULL )
    {
        Log("Cannot write to PAK file : ");
        LogPlus( PakName+".pak" );
        return;
    }
    fwrite( "PAK", 1, 3, f );
    fwrite( &PakNb, 1, 2, f );
    w = (WORD)Count;
    fwrite( &w, 1, 2, f );

    for( ID = 0; ID< 65536; ID ++ )
    {
      for( i = 0; i< Count; i++ )
        if( Items[i]->ID == ID )
        {
             fwrite( &Items[i]->ID, 1, 2, f);
             b = (BYTE)(Items[i]->Symbol.Length());
             fwrite( &b, 1, 1, f);
             fwrite( Items[i]->Symbol.c_str(),1,b,f);
             b = (BYTE)(Items[i]->Nom.Length());
             fwrite( &b, 1, 1, f);
             fwrite( Items[i]->Nom.c_str(), 1, b, f);
             fwrite( &Items[i]->Type, 1, 1, f);
             fwrite( &Items[i]->Len, 1, 4, f);
             fwrite( Items[i]->Data, 1, Items[i]->Len, f);
             d = 0xDAAF0BBC;
             fwrite( &d, 1, 4, f );
        }
    }
    fclose(f);
    Plugins->SendMainEvent( EVT_PAKCHANGE, (PathName+PakName+".pak").c_str() );
}
//---------------------------------------------------------------------------
void
CPak::AddItem( AnsiString FileName )
{
    FILE *f;
    LPType t;
    LPPakItem *NewItems,i;
    int j;
    WORD ID = 0;

    t = Projet->FTypes->FGetFullType( FileName );
    if( t == NULL )
    {
        Log("Type unkw.: "+FileName);
        return;
    }
    f = fopen( FileName.c_str(), "rb" );
    if( f == NULL )
    {
        Log("Cannot open file : "+FileName );
        return;
    }

    NewItems = new LPPakItem[ Count + 1 ];
    for(j=0; j< Count; j++ )
    {
        NewItems[j] = Items[j];
        if( Items[j]->ID > ID )
            ID = Items[j]->ID;
    }
    delete Items;
    Items = NewItems;

    fseek( f, 0, 2 );
    i = new TPakItem;
    Items[Count] = i;
    i->Nom = FileName;
    while( i->Nom.Pos("\\") > 0 )
        i->Nom.Delete(1,i->Nom.Pos("\\") );
    i->Symbol = "";
    i->ID = (WORD)(ID+1);
    i->Len = ftell( f ) - t->DataLen;
    i->Type = t->ID;
    i->Data = new BYTE[i->Len];
    fseek( f, 0,0 );
    fread( i->Data, 1, t->DataPos, f );
    fseek( f, t->DataPos+t->DataLen, 0 );
    fread( i->Data+t->DataPos, 1, i->Len-t->DataPos, f );
    fclose(f);
    Count++;
}
//---------------------------------------------------------------------------
LPPakItem
CPak::FindItem( AnsiString Nom )
{
    int i;
    for(i=0; i< Count; i++ )
        if( Items[i]->Nom == Nom )
            return Items[i];
        return NULL;
}
//---------------------------------------------------------------------------
LPPakItem
CPak::FindItem( WORD ID )
{
    int i;
    for(i=0; i< Count; i++ )
        if( Items[i]->ID == ID )
            return Items[i];
        return NULL;
}
//---------------------------------------------------------------------------
void
CPak::UpdateItem( LPPakItem i, AnsiString FileName )
{
    FILE *f;
    LPType t;

    t = Projet->FTypes->FGetFullType( FileName );
    if( t == NULL )
    {
        Log("Type unkw.: "+FileName);
        return;
    }
    f = fopen( FileName.c_str(), "rb" );
    if( f == NULL )
    {
        Log("Cannot open file : "+FileName );
        return;
    }
    fseek( f, 0, 2 );
    i->Nom = FileName;
    while( i->Nom.Pos("\\") > 0 )
        i->Nom.Delete(1,i->Nom.Pos("\\") );
    i->Len = ftell( f ) - t->DataLen;
    i->Type = t->ID;
    delete i->Data;
    i->Data = new BYTE[i->Len];
    fseek( f, 0,0 );
    fread( i->Data, 1, t->DataPos, f );
    fseek( f, t->DataPos+t->DataLen, 0 );
    fread( i->Data+t->DataPos, 1, i->Len-t->DataPos, f );
    fclose(f);
}
//---------------------------------------------------------------------------
void
CPak::DeleteItem( LPPakItem it )
{
    int i;
    for(i=0; i< Count; i++ )
    {
        if( Items[i] == it )
        {
            delete it->Data;
            delete it;
            memcpy( Items+i, Items+(i+1), (Count-i-1)*sizeof( LPPakItem ) );
            Count --;
            return;
        }
    }
}
//---------------------------------------------------------------------------
void
CPak::ExportItem( LPPakItem it )
{
    FILE *f = fopen( it->Nom.c_str(), "rb" );
    LPType t;
    
    if( f != NULL )
    {
        fclose(f);
        if( MessageBox( NULL, "Overwrite ???", "File already Exists:", MB_YESNO ) == IDNO )
            return;
    }
    t = Projet->FTypes->GetType( it->Type );
    if( t == NULL )
    {
        Log("Unknown type : "+it->Nom);
        return;
    }
    f = fopen( it->Nom.c_str(), "wb" );
    if( f == NULL )
    {
        Log("Cannot create File : "+it->Nom);
        return;
    }
    fwrite( it->Data, 1, t->DataPos, f);
    fwrite( t->Data, 1, t->DataLen, f );
    fwrite( it->Data+t->DataPos, 1, it->Len-t->DataPos, f );
    fclose(f);
    Log("Exporting : "+it->Nom);
}

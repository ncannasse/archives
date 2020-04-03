//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Console.h"
#include "Types_Defs.h"
#include "Main_Defs.h"
//---------------------------------------------------------------------------
CFTypes::CFTypes()
{
    Types = NULL;
    TCount = 0;
}
//---------------------------------------------------------------------------
CFTypes::~CFTypes()
{
    Clean();
}
//---------------------------------------------------------------------------
void
CFTypes::Clean()
{
    int i;
    for( i=0; i<TCount; i++ )
    {
        delete Types[i]->Data;
        delete Types[i];
    }
    delete Types;
    Types = NULL;
    TCount = 0;
}
//---------------------------------------------------------------------------
void *
CFTypes::EnumTypes( ENUMPROC proc, void *param )
{
    int i;
    void *r;
    for( i = 0; i< TCount; i++ )
        if( (r=proc( Types[i], param ) ) != NULL )
            return r;
        
        return NULL;
}
//---------------------------------------------------------------------------
LPType
CFTypes::NewType( AnsiString Ident )
{
    LPType *NewTypes = new LPType[ TCount + 1 ];
    int i;
    
    for( i=0; i< TCount; i++ )
        NewTypes[i] = Types[i];
    delete Types;
    Types = NewTypes;
    NewTypes[TCount] = new TType;
    NewTypes[TCount]->Identif = Ident;
    NewTypes[TCount]->Masks = "";
    NewTypes[TCount]->ID = 255;
    NewTypes[TCount]->DataPos = 0;
    NewTypes[TCount]->DataLen = 0;
    NewTypes[TCount]->Data = NULL;
    TCount++;
    return NewTypes[TCount];
}
//---------------------------------------------------------------------------
AnsiString
CFTypes::GetIdent( BYTE Type )
{
    int i;
    for( i=0; i< TCount; i++ )
        if( Types[i]->ID == Type )
            return Types[i]->Identif;
    return "UNKNOWN";
}
//---------------------------------------------------------------------------
LPType
CFTypes::GetType( BYTE Type )
{
    int i;
    for( i=0; i< TCount; i++ )
        if( Types[i]->ID == Type )
            return Types[i];
    return NULL;
}
//---------------------------------------------------------------------------
BYTE
CFTypes::FGetType( AnsiString FileName )
{
    LPType t = FGetFullType( FileName);
    if( t == NULL )
        return 0;
    return t->ID;
}
//---------------------------------------------------------------------------
LPType
CFTypes::FGetFullType( AnsiString FileName )
{
    FILE *f;
    int i,j;
    AnsiString Mask,m;
    BYTE t[1024];
    
    FileName = FileName.LowerCase();
    for( i=0; i<TCount; i++ )
    {
        Mask = Types[i]->Masks;
        while( Mask.Pos(";") > 0 )
        {
            m = Mask;
            m.Delete( m.Pos(";"), m.Length() );
            if( FileName.Pos( m ) > 0 )
                return Types[i];
            Mask.Delete(1, Mask.Pos(";") );
        }
        if( Mask != "" && FileName.Pos( Mask ) > 0 )
            return Types[i];
    }
    
    f = fopen( FileName.c_str(), "rb" );
    if( f == NULL )
    {
        Log("Cannot open file : "+FileName);
        return NULL;
    }
    for(i=0; i< TCount; i++ )
        if( Types[i]->DataLen )
        {
            fseek( f, Types[i]->DataPos, 0 );
            fread( t, 1, Types[i]->DataLen, f );
            for( j=0; j< Types[i]->DataLen; j++ )
                if( t[j] != Types[i]->Data[j] )
                    break;
                if( j == Types[i]->DataLen )
                    return Types[i];
        }
        
        fclose(f);
        return NULL;
}
//---------------------------------------------------------------------------
void
CFTypes::Delete( AnsiString Ident )
{
    int i;
    for( i=0; i< TCount; i++ )
         if( Types[i]->Identif == Ident )
             Delete( i );
}
//---------------------------------------------------------------------------
void
CFTypes::Delete( int Index )
{
    int i;
    if( Index < 0 || Index >= TCount )
        return;
    delete Types[Index]->Data;
    delete Types[Index];
    for( i=Index; i< TCount - 1; i++ )
        Types[i] = Types[i+1];
    TCount--;
}
//---------------------------------------------------------------------------
void
CFTypes::Save( FILE *f )
{
    int i,j;
    for( i=0; i< TCount; i++ )
    {
        EcritTxt( "IDENT=", f);
        EcritTxt( Types[i]->Identif,f);
        SautLigne(f);
        EcritTxt( "MASKS=", f);
        EcritTxt( Types[i]->Masks,f);
        SautLigne(f);
        EcritTxt( "ID=", f);
        EcritTxt( (int)Types[i]->ID,f);
        SautLigne(f);
        EcritTxt( "DATAPOS=", f);
        EcritTxt( Types[i]->DataPos,f);
        SautLigne(f);
        EcritTxt( "DATALEN=", f);
        EcritTxt( Types[i]->DataLen,f);
        SautLigne(f);
        EcritTxt( "DATA=", f);
        for( j=0; j< Types[i]->DataLen; j++ )
        {
            EcritTxt( (int)Types[i]->Data[j],f);
            EcritTxt( ",",f);
        }
        SautLigne(f);
        SautLigne(f);
    }
}



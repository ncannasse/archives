//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Main_Defs.h"
#include "PRJ_Defs.h"
#include "Console.h"
#include "Balises.h"
//---------------------------------------------------------------------------
CProjet *Projet = NULL;
//---------------------------------------------------------------------------
CProjet::CProjet( AnsiString nom )
{
    int i = 0;
    Paks = NULL;
    FTypes = new CFTypes;
    PakCount = 0;
    PathProjet = nom;
    NomProjet = nom;

    Prefs.Version = 100;
    Prefs.VerInc = true;
    Prefs.Describe = "";
    Prefs.Name = nom.UpperCase();

    while( PathProjet.Length() > 0 && PathProjet[ PathProjet.Length() ] != '\\'  )
    {
        i++;
        PathProjet.Delete(PathProjet.Length(),1);
    }
    NomProjet.Delete(1, nom.Length()-i );
    Dirs = new CFolders( "D<"+NomProjet+">" );
}
//---------------------------------------------------------------------------
CProjet::~CProjet()
{
    delete FTypes;
    delete Dirs;
    CleanPaks();
}
//---------------------------------------------------------------------------
void
CProjet::CleanPaks()
{
    int i;
    for( i=0; i< PakCount; i++)
        delete Paks[i];
    delete Paks;
    Paks = NULL;
    PakCount = 0;
}
//---------------------------------------------------------------------------
void *
CProjet::EnumPaks( ENUMPROC proc, void *param )
{
    int i;
    void *r;
    
    for( i = 0; i< PakCount; i++ )
        if( (r=proc( Paks[i], param ) ) != NULL )
            return r;
        
        return NULL;
}
//---------------------------------------------------------------------------
CPak *
CProjet::FindPak( AnsiString Nom )
{
    int i;
    for( i = 0; i< PakCount; i++ )
        if( Paks[i]->PakName == Nom )
            return Paks[i];

        return NULL;
}
//---------------------------------------------------------------------------
void
CProjet::NewPak( AnsiString Nom )
{
    CPak **NewPaks;
    int i;
    AnsiString Nom2;
    WORD pknb = 0;

    for( i=0; i<PakCount; i++ )
        if( Paks[i]->PakNb == pknb )
        {
            pknb++;
            i=-1;
        }

    Nom2 = Nom.LowerCase();
    for( i=0; i<PakCount; i++ )
    {
        if( Nom2 == Paks[i]->PakName.LowerCase() )
        {
            Log("This PAK is already in your list.");
            return;
        }
    }
    NewPaks = new CPak*[ PakCount + 1 ];
    for( i=0; i<PakCount; i++ )
        NewPaks[i] = Paks[i];
    NewPaks[PakCount] = new CPak( Nom, PathProjet, pknb );
    PakCount++;
    delete Paks;
    Paks = NewPaks;
}
//---------------------------------------------------------------------------
void
CProjet::LoadPak( AnsiString Nom )
{
    NewPak( Nom );
    Paks[ PakCount - 1 ]->Load();
}
//---------------------------------------------------------------------------
void
CProjet::DelPak( AnsiString Nom )
{
    int i;
    for( i=0; i< PakCount; i++)
         if( Paks[i]->PakName == Nom )
             DelPak( i );
}
//---------------------------------------------------------------------------
void
CProjet::DelPak( int Index )
{
    int i;
    if( Index < 0 || Index >= PakCount )
        return;
    DeleteFile( (Paks[Index]->PathName + Paks[Index]->PakName + ".pak" ).c_str() );
    delete Paks[Index];
    for( i=Index; i< PakCount - 1; i++ )
        Paks[i] = Paks[i+1];
    PakCount--;
}
//---------------------------------------------------------------------------
void *
FindByName( void *p, void *c )
{
   AnsiString a = *((AnsiString *)c);
   LPType t = (LPType )p;
   if( t->Identif == a )
       return p;
   return NULL;
}
//---------------------------------------------------------------------------
void
CProjet::Load()
{
    FILE *f = fopen( (PathProjet+NomProjet).c_str(), "rb" );
    AnsiString c;
    int i;
    TBalise b;
    LPFolder fsav = Dirs->GetItem(0);
    LPType t = NULL;
    AnsiString n;

    if( f == NULL )
    {
        Log("Cannot open : "+PathProjet+NomProjet);
        return;
    }
    Plugins->SendMainEvent( EVT_PRJINIT, (PathProjet+NomProjet).c_str() );
    GetNextBalise( &b, f );
    while( b.Type != BAL_EOF )
    {
        switch( b.Bloc )
        {
        case BLC_GENERAL:
             switch( b.Type )
             {
             case BAL_DEFPATH:
                  Prefs.DefPath = b.Txt;
                  break;
             case BAL_NAME:
                  Prefs.Name = b.Txt;
                  break;
             case BAL_DESCRIBE:
                  Prefs.Describe = b.Txt;
                  break;
             case BAL_VERSION:
                  Prefs.Version = atoi( b.Txt.c_str() );
                  break;
             case BAL_VERINC:
                  Prefs.VerInc = ( b.Txt == "1" );
                  break;
             default:
                  break;
             }
             break;
        case BLC_FOLDER:
             switch( b.Type )
             {
             case BAL_PLUGIN:
                  if( Plugins->RequestEvent( EVT_LOAD, b.Plugin, b.Txt.c_str(), NULL ) )
                      Dirs->AddItem( (AnsiString)b.Plugin->Pred+b.Txt, fsav );
                  break;
             case BAL_OPEN:
                  if( fsav != NULL && b.Txt == "0" )
                      fsav->open = false;
                  break;
             case BAL_NAME:
                  fsav = Dirs->GetItem( "D"+b.Txt, -1 );
                  if( fsav == NULL )
                  {
                      Dirs->AddItem( "D"+b.Txt, Dirs->GetItem(0) );
                      fsav = Dirs->GetItem( "D"+b.Txt, -1 );
                  }
                  if( fsav == NULL )
                      fsav = Dirs->GetItem(0);
                  break;
             case BAL_FOLDER:
                  Dirs->AddItem( "D"+b.Txt, fsav );
                  break;
             case BAL_PAK:
                  Dirs->AddItem( "P"+b.Txt, fsav );
                  LoadPak( b.Txt );
                  break;
             case BAL_FILETYPE:
                  Dirs->AddItem( "F"+b.Txt, fsav );
                  FTypes->NewType( b.Txt );
                  break;
             default:
                  break;
             }
             break;
        case BLC_FILETYPES:
             switch( b.Type )
             {
             case BAL_IDENT:
                  t = (LPType)FTypes->EnumTypes( FindByName, &b.Txt );
                  break;
             case BAL_MASKS:
                  if( t != NULL )
                      t->Masks = b.Txt;
                  break;
             case BAL_ID:
                  if( t != NULL )
                      t->ID = (BYTE)atoi( b.Txt.c_str() );
                  break;
             case BAL_DATAPOS:
                  if( t != NULL )
                      t->DataPos = atoi( b.Txt.c_str() );
                  break;
             case BAL_DATALEN:
                  if( t != NULL )
                  {
                      t->DataLen = atoi( b.Txt.c_str() );
                      t->Data = new BYTE[t->DataLen];
                  }
                  break;
             case BAL_DATA:
                  if( t != NULL )
                      for( i=0; i< t->DataLen; i++ )
                      {
                           c = b.Txt;
                           c.Delete( c.Pos(","), c.Length() );
                           t->Data[i] = (BYTE)atoi( c.c_str() );
                           b.Txt.Delete(1, b.Txt.Pos(",") );
                      }
                  break;
             }
             break;
        default:
             break;
        }
        GetNextBalise( &b, f );
    }
    Log( "Loading Project [DONE]");
    Plugins->SendMainEvent( EVT_PRJLOAD, (PathProjet+NomProjet).c_str() );
}
//---------------------------------------------------------------------------
void
CProjet::Save()
{
    FILE *f = fopen( (PathProjet+NomProjet).c_str(), "wb" );

    if( f == NULL )
    {
        Log("Cannot save : "+PathProjet+NomProjet);
        return;
    }
    if( Prefs.VerInc )
        Prefs.Version++;

    EcritTxt("DRAGOON_PROJECT_FILE",f);
    SautLigne(f);
    SautLigne(f);
    EcritTxt("[GENERAL]",f);
    SautLigne(f);
    EcritTxt("NAME="+Prefs.Name,f);
    SautLigne(f);
    EcritTxt("DESCRIBE="+Prefs.Describe,f);
    SautLigne(f);
    EcritTxt("VERSION=",f);
    EcritTxt(Prefs.Version,f);
    SautLigne(f);
    EcritTxt("PATH=",f);
    EcritTxt(Prefs.DefPath,f);
    SautLigne(f);
    if( Prefs.VerInc )
    {
       EcritTxt("VERINC=1",f);
       SautLigne(f);
    }
    Dirs->SaveItems( f, Dirs->GetItem(0) );
    SautLigne(f);
    EcritTxt("[FILETYPES]",f);
    SautLigne(f);
    FTypes->Save(f);
    fclose(f);

    f = fopen( (PathProjet+NomProjet+"_defs.h").c_str(), "wb" );
    EcritTxt("// PAK Symbols",f);
    SautLigne(f);
    EcritTxt("// This file was auto-generated by Dragoon (c)2000 by Warplayer",f);
    SautLigne(f);
    EcritTxt("// Cnv Macro can be defined has you want",f);
    SautLigne(f);
    SautLigne(f);
    EcritTxt("#pragma once",f);
    SautLigne(f);
    SautLigne(f);
    int i;
    for( i=0; i< PakCount; i++)
         Paks[i]->SaveSymbols(f);
    fclose(f);
    Log( "Saving Project [DONE]");
    Plugins->SendMainEvent( EVT_PRJSAVE, (PathProjet+NomProjet).c_str() );
}
//---------------------------------------------------------------------------


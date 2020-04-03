//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Balises.h"
//---------------------------------------------------------------------------
void
GetNextBalise( LPBalise b, FILE *f )
{
    AnsiString a;
    char c;

    if( fread( &c,1,1,f) != 1 )
    {
        b->Bloc = BLC_EOF;
        b->Type = BAL_EOF;
        b->Txt = "";
        return;
    }

    while( c != 13 )
    {
        a = a + c;
        if( fread( &c,1,1,f) != 1 )
            break;
    }


    if( a.Length() > 0 && a[1] == '[' )
    {
        a.Delete(1,1);
        a.Delete(a.Length(),1);
        b->Bloc = BLC_NOTHING;
        b->Type = BAL_NOTHING;
        b->Txt = "";
        if( a == "GENERAL" )
            b->Bloc = BLC_GENERAL;
        if( a == "FOLDER" )
            b->Bloc = BLC_FOLDER;
        if( a == "FILETYPES" )
            b->Bloc = BLC_FILETYPES;
    }
    else
    {
        b->Txt = a;
        a.Delete(a.Pos("="),a.Length() );
        b->Txt.Delete(1, b->Txt.Pos("=") );
        b->Type = BAL_NOTHING;

        if( a == "PATH" )
            b->Type = BAL_DEFPATH;
        if( a == "VERSION" )
            b->Type = BAL_VERSION;
        if( a == "VERINC" )
            b->Type = BAL_VERINC;
        if( a == "DESCRIBE" )
            b->Type = BAL_DESCRIBE;
        if( a == "OPEN" )
            b->Type = BAL_OPEN;
        if( a == "NAME" )
            b->Type = BAL_NAME;
        if( a == "PACK" )
            b->Type = BAL_PAK;
        if( a == "FILETYPE" )
            b->Type = BAL_FILETYPE;
        if( a == "FOLDER" )
            b->Type = BAL_FOLDER;
        if( a == "IDENT" )
            b->Type = BAL_IDENT;
        if( a == "DATALEN" )
            b->Type = BAL_DATALEN;
        if( a == "DATAPOS" )
            b->Type = BAL_DATAPOS;
        if( a == "DATA" )
            b->Type = BAL_DATA;
        if( a == "MASKS" )
            b->Type = BAL_MASKS;
        if( a == "ID" )
            b->Type = BAL_ID;
        if( b->Type == BAL_NOTHING )
        {
            b->Plugin = Plugins->GetPlugByBal( a );
            if( b != NULL )
                b->Type = BAL_PLUGIN;
        }
    }

    if( c == 13 )
    {
        if( fread( &c,1,1,f) != 1 )
            fseek(f,-1,1);
    }
}
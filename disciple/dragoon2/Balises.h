//---------------------------------------------------------------------------
#ifndef BalisesH
#define BalisesH
#include "Plugins.h"
//---------------------------------------------------------------------------
enum BLOC {
     BLC_NOTHING,
     BLC_EOF,
     BLC_GENERAL,
     BLC_FOLDER,
     BLC_FILETYPES
};
//---------------------------------------------------------------------------
enum BALISE {
     BAL_NOTHING,
     BAL_EOF,

     BAL_NAME,

     BAL_DESCRIBE,
     BAL_VERSION,
     BAL_VERINC,
     BAL_DEFPATH,

     BAL_FOLDER,
     BAL_PAK,
     BAL_FILETYPE,

     BAL_IDENT,
     BAL_MASKS,
     BAL_ID,
     BAL_DATAPOS,
     BAL_DATALEN,
     BAL_DATA,
     BAL_OPEN,

     BAL_PLUGIN
};
//---------------------------------------------------------------------------
typedef struct{
        BLOC    Bloc;
        BALISE  Type;
        AnsiString Txt;
        LPPlugin Plugin;
} TBalise, *LPBalise;

//---------------------------------------------------------------------------
void GetNextBalise( LPBalise b, FILE *f );
//---------------------------------------------------------------------------
#endif

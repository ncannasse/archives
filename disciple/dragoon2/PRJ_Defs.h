//---------------------------------------------------------------------------
#ifndef PRJ_DefsH
#define PRJ_DefsH
//---------------------------------------------------------------------------
#include "PAK_Defs.h"
#include "Types_Defs.h"
#include "Folders_Defs.h"
//---------------------------------------------------------------------------
class CProjet {
    
    void CleanPaks();

    void DelPak   ( int Index );

public:
    CPak **Paks;
    int  PakCount;

    struct {
    
           AnsiString Name;
           AnsiString Describe;
           AnsiString DefPath;
           int        Version;
           bool       VerInc;

           } Prefs;

    CFTypes *FTypes;
    CFolders *Dirs;

    AnsiString NomProjet;
    AnsiString PathProjet;

    void *EnumPaks( ENUMPROC p, void *param );
    void NewPak   ( AnsiString Nom );
    void LoadPak  ( AnsiString Nom );
    void DelPak   ( AnsiString Nom );
    CPak *FindPak( AnsiString Nom );

    void Load();
    void Save();

    CProjet( AnsiString nom );
    ~CProjet();
};
//---------------------------------------------------------------------------
extern CProjet *Projet;
//---------------------------------------------------------------------------
#endif

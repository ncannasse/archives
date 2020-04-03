//---------------------------------------------------------------------------
#ifndef Folders_DefsH
#define Folders_DefsH
//---------------------------------------------------------------------------
typedef struct TFolder {
    AnsiString Nom;
    
    bool open;
    
    int NFils;
    struct TFolder **Fils;
    struct TFolder *Pere;
    
} TFolder, *LPFolder;
//---------------------------------------------------------------------------
#include "Pak_Defs.h"
//---------------------------------------------------------------------------

class CFolders {

    LPFolder folds;

    void AfficheRec( LPFolder f, TListBox *Where, int Prof );
    LPFolder FindRec( LPFolder f, AnsiString Nom, int Prof );
    LPFolder FindRec( LPFolder f, int *Index );
    void     KillRec( LPFolder f );

public:

    void Affiche( TListBox *Where );
    
    void AddItem ( AnsiString Nom, LPFolder pere );
    void DelItem ( LPFolder f );
    void KillItem( LPFolder f );
    LPFolder CutItem ( LPFolder Item );
    void InsItem ( LPFolder Pere, LPFolder Item );
    void InsItem ( LPFolder Pere, LPFolder Item, int Pos );
    
    int      GetProf( LPFolder f );
    LPFolder GetItem( int Index );
    LPFolder GetItem( AnsiString Nom, int Prof );

    void    *EnumItems( LPFolder f, ENUMPROC proc, void *param );

    void     SaveItems( FILE *f, LPFolder fol );

    CFolders( AnsiString Nom );
    ~CFolders();
    
};

#endif

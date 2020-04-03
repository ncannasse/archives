//---------------------------------------------------------------------------
#ifndef PAK_DefsH
#define PAK_DefsH
//---------------------------------------------------------------------------
typedef void * (*ENUMPROC)( void *obj, void *cmp );
//---------------------------------------------------------------------------
typedef struct TPakItem {
    WORD ID;
    AnsiString Symbol;
    AnsiString Nom;
    BYTE  Type;
    DWORD Len;
    BYTE *Data;
} TPakItem, *LPPakItem;
//---------------------------------------------------------------------------
class CPak {
    LPPakItem *Items;
    int       Count;
    void Clean();

public:

    WORD      PakNb;
    int Folder;

    AnsiString PakName;
    AnsiString PathName;

    void Load();
    void Save();
    void SaveSymbols( FILE *f );

    void UpdateItem( LPPakItem it, AnsiString FileName );
    void ExportItem( LPPakItem it );

    void AddItem( AnsiString FileName );
    void *EnumItems( ENUMPROC p, void *param );
    LPPakItem FindItem( AnsiString Name );
    LPPakItem FindItem( WORD      ID );

    void DeleteItem( LPPakItem i );

    CPak( AnsiString nom, AnsiString path, WORD Nb );
    ~CPak();
};
//---------------------------------------------------------------------------
#endif

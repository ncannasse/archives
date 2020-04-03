//---------------------------------------------------------------------------
#ifndef Types_DefsH
#define Types_DefsH
//---------------------------------------------------------------------------
#include "PAK_Defs.h"
//---------------------------------------------------------------------------
typedef struct TType {
    AnsiString Identif;
    AnsiString Masks;
    BYTE       ID;
    int        DataPos;
    int        DataLen;
    BYTE       *Data;
} TType, *LPType;
//---------------------------------------------------------------------------
class CFTypes {
    
    LPType     *Types;
    int        TCount;
    
    void       Clean();
    void Delete( int Index );

public:

    LPType NewType( AnsiString Ident );

    void *EnumTypes( ENUMPROC proc, void *param );

    void Delete( AnsiString Ident );

    AnsiString GetIdent( BYTE Type );
    LPType     GetType ( BYTE Type );
    BYTE FGetType( AnsiString FileName );
    LPType FGetFullType( AnsiString FileName );
    
    void Save( FILE *f );
    
    CFTypes();
    ~CFTypes();
};
//---------------------------------------------------------------------------
#endif

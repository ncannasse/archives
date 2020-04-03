//---------------------------------------------------------------------------
#ifndef Types_FormH
#define Types_FormH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include "Types_Defs.h"
//---------------------------------------------------------------------------
class TTypesForm : public TForm
{
__published:    // IDE-managed Components
TGroupBox *Panel;
TButton *OK;
TLabel *Label1;
TEdit *Masks;
TEdit *ID;
TLabel *Label2;
TLabel *Label3;
TEdit *Ident;
TLabel *Label4;
TEdit *DataPos;
TEdit *DataLen;
TLabel *Label5;
TListBox *Data;
TLabel *Label6;
TButton *AddData;
TButton *DelData;
void __fastcall FormShow(TObject *Sender);

void __fastcall AddDataClick(TObject *Sender);
void __fastcall DelDataClick(TObject *Sender);
void __fastcall OKClick(TObject *Sender);
void __fastcall DataPosChange(TObject *Sender);
void __fastcall IDChange(TObject *Sender);

private:    // User declarations
public:     // User declarations
    __fastcall TTypesForm(TComponent* Owner);
    LPType     CurType;
};
//---------------------------------------------------------------------------
extern TTypesForm *TypesForm;
//---------------------------------------------------------------------------
#endif

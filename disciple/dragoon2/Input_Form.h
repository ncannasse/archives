//---------------------------------------------------------------------------
#ifndef Input_FormH
#define Input_FormH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TInputForm : public TForm
{
__published:	// IDE-managed Components
   TEdit *Texte;
   TButton *OK;
   TButton *Cancel;
   void __fastcall OKClick(TObject *Sender);
   
   void __fastcall CancelClick(TObject *Sender);
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall TexteKeyDown(TObject *Sender, WORD &Key,
        TShiftState Shift);
private:	// User declarations
public:		// User declarations
   __fastcall TInputForm(TComponent* Owner);
   bool InputLn( TForm *Form, AnsiString CaptTxt, AnsiString DefTxt, AnsiString *out );
};
//---------------------------------------------------------------------------
#define Input(x,y,z,out) InputForm->InputLn(x,y,z,out)
//---------------------------------------------------------------------------
extern TInputForm *InputForm;
//---------------------------------------------------------------------------
#endif

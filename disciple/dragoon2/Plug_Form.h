//---------------------------------------------------------------------------
#ifndef Plug_FormH
#define Plug_FormH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include "plugins.h"
//---------------------------------------------------------------------------
class TPlugForm : public TForm
{
__published:	// IDE-managed Components
        TEdit *PlugEdit;
        TComboBox *PlugCombo;
        TButton *HidePlugForm;
        void __fastcall PlugEditPressKey(TObject *Sender, WORD &Key,
        TShiftState Shift);
        void __fastcall FormShow(TObject *Sender);
        
        void __fastcall PlugEditChange(TObject *Sender);
        
        
        void __fastcall FormResize(TObject *Sender);
        void __fastcall HidePlugFormClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TPlugForm(TComponent* Owner);
        LPPlugin Plugin;
};
//---------------------------------------------------------------------------
extern TPlugForm *PlugForm;
//---------------------------------------------------------------------------
#endif

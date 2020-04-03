//---------------------------------------------------------------------------
#ifndef PlugInfo_FormH
#define PlugInfo_FormH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TPlugInfoForm : public TForm
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TComboBox *Plugs;
        TLabel *Label1;
        TLabel *FileName;
        TLabel *Label3;
        TLabel *PName;
        TLabel *Label4;
        TLabel *Author;
        TLabel *Label6;
        TMemo *Infos;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
        
        void __fastcall PlugsChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TPlugInfoForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TPlugInfoForm *PlugInfoForm;
//---------------------------------------------------------------------------
#endif

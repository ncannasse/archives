//---------------------------------------------------------------------------
#ifndef PPrefs_FormH
#define PPrefs_FormH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TPPrefForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TLabel *PFile;
        TLabel *Label3;
        TEdit *PName;
        TEdit *PDesc;
        TLabel *Label2;
        TButton *OK;
        TButton *CANCEL;
        TLabel *Label5;
        TEdit *PVer;
        TCheckBox *PVerInc;
        TEdit *DefPath;
        TLabel *Label4;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall CANCELClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TPPrefForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TPPrefForm *PPrefForm;
//---------------------------------------------------------------------------
#endif

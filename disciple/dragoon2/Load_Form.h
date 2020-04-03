//---------------------------------------------------------------------------
#ifndef urledit_loadH
#define urledit_loadH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\FileCtrl.hpp>
//---------------------------------------------------------------------------
class TLoadForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel;
   TDriveComboBox *Drives;
   TDirectoryListBox *Dirs;
   TFileListBox *Files;
   TEdit *FileName;
   TLabel *Label1;
   TButton *Action;
   TButton *Cancel;
   void __fastcall CancelClick(TObject *Sender);
   

   void __fastcall FileNameKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
   void __fastcall ActionClick(TObject *Sender);
   void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
   void __fastcall FilesDblClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TLoadForm(TComponent* Owner);
   AnsiString Load( TForm *Form, AnsiString FileMask, AnsiString Title );
   AnsiString Save( TForm *Form, AnsiString FileMask, AnsiString Title, AnsiString ActName );
};
//---------------------------------------------------------------------------
extern TLoadForm *LoadForm;
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
#ifndef PAK_BrowserH
#define PAK_BrowserH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TPAKBrowser : public TForm
{
__published:	// IDE-managed Components
        TButton *Close;
        TEdit *PakName;
        TListBox *PAKList;
        void __fastcall CloseClick(TObject *Sender);
        
        
        
        void __fastcall PAKListClick(TObject *Sender);
        
        void __fastcall PakNameChange(TObject *Sender);
        void __fastcall PakNameKeyUp(TObject *Sender, WORD &Key,
        TShiftState Shift);
private:	// User declarations
public:		// User declarations
        __fastcall TPAKBrowser(TComponent* Owner);
        void Open( AnsiString Nom );
};
//---------------------------------------------------------------------------
extern TPAKBrowser *PAKBrowser;
//---------------------------------------------------------------------------
#endif

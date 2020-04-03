//---------------------------------------------------------------------------
#ifndef ConsoleH
#define ConsoleH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
#define Log(x) ConsoleForm->LogAdd(x)
#define LogPlus(x) ConsoleForm->LogList->Items->Strings[ ConsoleForm->LogList->Items->Count - 1 ] = ConsoleForm->LogList->Items->Strings[ ConsoleForm->LogList->Items->Count - 1 ] + x
//---------------------------------------------------------------------------
class TConsoleForm : public TForm
{
__published:	// IDE-managed Components
   TListBox *LogList;
   void __fastcall LogListDblClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TConsoleForm(TComponent* Owner);
   void LogAdd( AnsiString a );
};
//---------------------------------------------------------------------------
extern TConsoleForm *ConsoleForm;
//---------------------------------------------------------------------------
#endif

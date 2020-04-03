//---------------------------------------------------------------------------
#ifndef PAK_FormH
#define PAK_FormH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include "Grids.hpp"
#include <vcl\FileCtrl.hpp>
#include "PAK_Defs.h"
//---------------------------------------------------------------------------
class TPAKForm : public TForm
{
__published:	// IDE-managed Components
   TGroupBox *Panel;
   TButton *ChangeID;
   TButton *ChangeSymbol;
   TButton *Delete;
   TStringGrid *PAKItems;
   TDriveComboBox *Drives;
   TFileListBox *Files;
   TDirectoryListBox *Dirs;
   TListBox *FList;
   TButton *AddUpdate;
   TButton *Export;
        TButton *Overwrite;
        TButton *PAKSave;
   void __fastcall FormShow(TObject *Sender);
   void __fastcall FilesChange(TObject *Sender);
   void __fastcall AddUpdateClick(TObject *Sender);

   void __fastcall FListDrawItem(TWinControl *Control, int Index, TRect &Rect,
   TOwnerDrawState State);
        void __fastcall FListDblClick(TObject *Sender);
        void __fastcall FListKeyUp(TObject *Sender, WORD &Key,
        TShiftState Shift);
        
        void __fastcall OverwriteClick(TObject *Sender);
        void __fastcall ChangeIDClick(TObject *Sender);
        void __fastcall ChangeSymbolClick(TObject *Sender);
        
        void __fastcall DeleteClick(TObject *Sender);
        
        
        
        
        void __fastcall PAKItemsMouseDown(TObject *Sender, TMouseButton Button,
        TShiftState Shift, int X, int Y);
        void __fastcall PAKItemsMouseUp(TObject *Sender, TMouseButton Button,
        TShiftState Shift, int X, int Y);
        
        void __fastcall PAKSaveClick(TObject *Sender);
        void __fastcall ExportClick(TObject *Sender);
private:	// User declarations

public:		// User declarations
   CPak     *CurPak;

   __fastcall TPAKForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TPAKForm *PAKForm;
//---------------------------------------------------------------------------
#endif

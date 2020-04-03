//---------------------------------------------------------------------------
#ifndef Main_FormH
#define Main_FormH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Menus.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Buttons.hpp>
#include "Grids.hpp"
#include <vcl\FileCtrl.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Icons;
   TPanel *ListPanel;
   TListBox *Liste;
   TSpeedButton *Exit;
   TPanel *StartPanel;
   TSpeedButton *NewProjet;
   TSpeedButton *LoadProjet;
   TPanel *NormalPanel;
   TSpeedButton *NewFilesType;
   TSpeedButton *NewPAK;
   TSpeedButton *NewFolder;
   TImageList *IconsRes;
   TPopupMenu *Menu;
   TMenuItem *Create;
   TMenuItem *Delete;
   TMenuItem *NewFolderMenu;
   TMenuItem *CreatePAKMenu;
   TLabel *Label1;
   TTimer *AutoPos;
   TMenuItem *NewFilesTypeMenu;
   TSpeedButton *SaveProject;
   TMainMenu *MainMenu;
   TMenuItem *NewOpts;
   TMenuItem *NewFolderOpt;
   TMenuItem *NewPAKOpt;
   TMenuItem *NewFTypeOpt;
   TMenuItem *ProjectOpts;
   TMenuItem *SaveProjectOpt;
   TMenuItem *ExitOpt;
   TMenuItem *Rename;
        TMenuItem *ShortKeys;
        TMenuItem *Cut;
        TMenuItem *Paste;
        TMenuItem *ExitWithoutSave;
        TMenuItem *FastLoad;
        TMenuItem *FastLoad1;
        TMenuItem *FastLoad2;
        TMenuItem *FastLoad3;
        TMenuItem *Preferences;
        TMenuItem *ProjectPrefs;
        TMenuItem *GlobalPrefs;
        TMenuItem *PlugInfo;
        TSpeedButton *PlugButton;
        TMenuItem *PlugMenu;
        TMenuItem *View;
        TMenuItem *PAKBrowserView;
   void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
   
   void __fastcall FormResize(TObject *Sender);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall ExitClick(TObject *Sender);
   void __fastcall NewProjetClick(TObject *Sender);
   void __fastcall LoadProjetClick(TObject *Sender);
   
   void __fastcall ListeMouseUp(TObject *Sender, TMouseButton Button,
   TShiftState Shift, int X, int Y);
   
   
   void __fastcall NewFolderClick(TObject *Sender);
   
   void __fastcall NewPAKClick(TObject *Sender);
   
   void __fastcall ListeDblClick(TObject *Sender);
   
   void __fastcall ListeClick(TObject *Sender);
   
   void __fastcall AutoPosTimer(TObject *Sender);
   
   void __fastcall NewFilesTypeClick(TObject *Sender);
   
   
   
   
   
   void __fastcall SaveProjectClick(TObject *Sender);
   void __fastcall DeleteClick(TObject *Sender);
   void __fastcall ListeDrawItem(TWinControl *Control, int Index, TRect &Rect,
   TOwnerDrawState State);
        void __fastcall CutClick(TObject *Sender);
        void __fastcall PasteClick(TObject *Sender);
        void __fastcall ListeDragOver(TObject *Sender, TObject *Source, int X,
        int Y, TDragState State, bool &Accept);
        void __fastcall ListeDragDrop(TObject *Sender, TObject *Source, int X,
        int Y);

        void __fastcall RenameClick(TObject *Sender);
        void __fastcall ExitWithoutSaveClick(TObject *Sender);
        void __fastcall FastLoadClick(TObject *Sender);
        void __fastcall ListeKeyUp(TObject *Sender, WORD &Key,
        TShiftState Shift);
        void __fastcall ProjectPrefsClick(TObject *Sender);
        void __fastcall PlugInfoClick(TObject *Sender);
        void __fastcall PlugButtonClick(TObject *Sender);
        void __fastcall PlugMenuClick(TObject *Sender);
        void __fastcall PAKBrowserViewClick(TObject *Sender);
        
        
        void __fastcall PlugButtonMouseDown(TObject *Sender,
        TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall PlugButtonDblClick(TObject *Sender);
        
private:	// User declarations
public:		// User declarations
   __fastcall TMainForm(TComponent* Owner);
   void       Redraw();

   bool       PAKDrag;
   AnsiString PAKName;
   DWORD      PAKItemID;
   
};
//---------------------------------------------------------------------------
extern TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif

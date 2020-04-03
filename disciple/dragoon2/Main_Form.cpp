//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "PPrefs_Form.h"
#include "Input_Form.h"
#include "Types_Form.h"
#include "Main_Form.h"
#include "Main_Defs.h"
#include "Load_Form.h"
#include "PAK_Form.h"
#include "PRJ_Defs.h"
#include "Console.h"
#include "Plugins.h"
#include "PlugInfo_Form.h"
#include "Plug_Form.h"
#include "PAK_Browser.h"
//---------------------------------------------------------------------------
AnsiString Olds[3];
//---------------------------------------------------------------------------
#pragma link "Grids"
#pragma resource "*.dfm"
TMainForm *MainForm;
AnsiString CurItem;
//---------------------------------------------------------------------------
LPFolder CutF = NULL;
LPFolder CurF = NULL;
int      IndexCount;
bool     CanExit = false;
//---------------------------------------------------------------------------
bool TestLock( AnsiString Nom )
{
     FILE *f;
     f = fopen( (Nom+".lock").c_str(), "rb" );
     if( f == NULL )
     {
         f = fopen( (Nom+".lock").c_str(), "wb" );
         fclose(f);
         return true;
     }
     fclose(f);
     if( MessageBox(MainForm->Handle, "Do you want to override the Lock ?", "WARNING ! Project Locked !!", MB_YESNO ) == IDYES )
         return true;
     return false;
}
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
   : TForm(Owner)
{
    HKEY hkm,hk,p;
    char buf[1024];
    DWORD s;

    RegOpenKey( HKEY_LOCAL_MACHINE, "Software", &hkm );
    RegOpenKey( hkm, "Dragoon", &hk );
    RegOpenKey( hk, "Projects", &p );
    s = 1024;
    if( RegQueryValueEx( p, "Name1", NULL, NULL, (BYTE *)buf, &s ) == ERROR_SUCCESS )
        Olds[0] = buf;
    s = 1024;
    if( RegQueryValueEx( p, "Name2", NULL, NULL, (BYTE *)buf, &s ) == ERROR_SUCCESS )
        Olds[1] = buf;
    s = 1024;
    if( RegQueryValueEx( p, "Name3", NULL, NULL, (BYTE *)buf, &s ) == ERROR_SUCCESS )
        Olds[2] = buf;
    RegCloseKey( p );
    RegCloseKey( hk );
    RegCloseKey( hkm );

   FastLoad1->Caption = FastLoad1->Caption + Olds[0];
   FastLoad2->Caption = FastLoad2->Caption + Olds[1];
   FastLoad3->Caption = FastLoad3->Caption + Olds[2];
   FastLoad1->Enabled = (Olds[0]!="");
   FastLoad2->Enabled = (Olds[1]!="");
   FastLoad3->Enabled = (Olds[2]!="");
}
//---------------------------------------------------------------------------
void Init()
{
   MainForm->View->Visible = true;
   MainForm->FastLoad->Visible = false;
   MainForm->ProjectOpts->Visible = true;
   MainForm->NewOpts->Visible = true;
   MainForm->ShortKeys->Visible = true;
   MainForm->Preferences->Visible = true;
   MainForm->Caption = MainForm->Caption + " - "+Projet->NomProjet;
   MainForm->StartPanel->Visible = false;
   MainForm->NormalPanel->Top = MainForm->StartPanel->Top;
   MainForm->NormalPanel->Visible = true;
   MainForm->ListPanel->Visible = true;
   Projet->Dirs->Affiche( MainForm->Liste );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose = CanExit;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject *Sender)
{
     if( Visible )
     {
         Exit->Top = Height-80;
         Liste->Height = Height-210;
     }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject *Sender)
{
     ConsoleForm->Left = Width-300;
     ConsoleForm->Top = Height-200;
     ConsoleForm->Visible = true;
     FormResize( Sender );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NewProjetClick(TObject *Sender)
{
     AnsiString Nom = LoadForm->Save( MainForm, "*.prj", "Create a new Project","Create");
     if( Nom != "" )
     {
         Projet = new CProjet( Nom );
         Plugins->SendMainEvent( EVT_PRJINIT, (Projet->PathProjet+Projet->NomProjet).c_str() );
         if( Nom != Olds[0] && Nom != Olds[1] && Nom != Olds[2] )
         {
             Olds[2] = Olds[1];
             Olds[1] = Olds[0];
             Olds[0] = Nom;
         }
         Init();
     }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LoadProjetClick(TObject *Sender)
{
     AnsiString Nom = LoadForm->Load( MainForm, "*.prj", "Load a Project File" );
     if( Nom != "" )
     {
         if( !TestLock(Nom) )
            return;
         Projet = new CProjet( Nom );
         if( Nom != Olds[0] && Nom != Olds[1] && Nom != Olds[2] )
         {
             Olds[2] = Olds[1];
             Olds[1] = Olds[0];
             Olds[0] = Nom;
         }
         Projet->Load();
         if( Projet->Prefs.DefPath != "" )
            PAKForm->Dirs->Directory = Projet->Prefs.DefPath;
         Init();
     }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ExitClick(TObject *Sender)
{
    if( LoadForm->Visible )
    {
        Log("Cannot quit while load form is visible");
        return;
    }

    if( Projet != NULL )
	{
                Projet->Save();
                DeleteFile( (Projet->PathProjet+Projet->NomProjet+".lock").c_str() );
	}
    delete Projet;
    CanExit = true;
    delete Plugins;
    Close();

    HKEY hkm,hk,p;
    RegOpenKey( HKEY_LOCAL_MACHINE, "Software", &hkm );
    RegCreateKey( hkm, "Dragoon", &hk );
    RegCreateKey( hk, "Projects", &p );
    RegSetValueEx( p, "Name1", 0, REG_SZ, (BYTE *)Olds[0].c_str(), Olds[0].Length() );
    RegSetValueEx( p, "Name2", 0, REG_SZ, (BYTE *)Olds[1].c_str(), Olds[1].Length() );
    RegSetValueEx( p, "Name3", 0, REG_SZ, (BYTE *)Olds[2].c_str(), Olds[2].Length() );
    RegCloseKey( p );
    RegCloseKey( hk );
    RegCloseKey( hkm );
}
//---------------------------------------------------------------------------
Graphics::TBitmap *GetIcon( char c, bool *erase )
{
    Graphics::TBitmap *bmp = new Graphics::TBitmap;
    *erase = true;
    switch( c )
    {
    case 'D':
         MainForm->IconsRes->GetBitmap(0, bmp );
         break;
    case 'P':
         MainForm->IconsRes->GetBitmap(1, bmp );
         break;
    case 'd':
         MainForm->IconsRes->GetBitmap(2, bmp );
         break;
    case 'F':
         MainForm->IconsRes->GetBitmap(3, bmp );
         break;
    default:
         LPPlugin p;
         p = Plugins->GetPlugByChar( c );
         if( p != NULL )
         {
             bmp->Handle = p->inf->HIcon;
             *erase = false;
         }
         break;
    }
    return bmp;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ListeMouseUp(TObject *Sender, TMouseButton Button,
   TShiftState Shift, int X, int Y)
{
   int n;
   if( Button == mbRight && Liste->ItemIndex != -1 )
   {
       n = (Y / Liste->ItemHeight) + Liste->TopIndex;
       if( n < Liste->Items->Count )
       {
           Liste->ItemIndex = n;
           Menu->Popup(X+Liste->Left+ListPanel->Left,Y+Liste->Top+ListPanel->Top+30);
           CurItem = Liste->Items->Strings[n];
       }
   }
}
//---------------------------------------------------------------------------
void TMainForm::Redraw()
{
     int i = Liste->ItemIndex, t = Liste->TopIndex;
     Liste->Clear();
     Projet->Dirs->Affiche( Liste );
     Liste->TopIndex = t;
     Liste->ItemIndex = i;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::NewFolderClick(TObject *Sender)
{
     AnsiString a;
     LPFolder f = Projet->Dirs->GetItem( Liste->ItemIndex );
     if( f == NULL )
         return;
     if( f->Nom[1]!='D' )
     {
         if( f->Pere == NULL )
             return;
         else
             f = f->Pere;
     }
     if( !Input( this, "Folder Name:","",&a ) )
         return;
     if ( a != "" )
     {
         Projet->Dirs->AddItem( "D"+a, f );
         Redraw();
     }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NewPAKClick(TObject *Sender)
{
     AnsiString a;
     LPFolder f = Projet->Dirs->GetItem( Liste->ItemIndex );
     if( f == NULL )
         return;

     if(! Input( this, "PAK Name:","", &a ) )
          return;
     if ( a != "" )
     {
         Projet->Dirs->AddItem( "P"+a, f );
         Projet->NewPak( a );
         Redraw();
     }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ListeDblClick(TObject *Sender)
{
     LPFolder f;
     if( Liste->ItemIndex == -1 )
         return;
     switch( Liste->Items->Strings[Liste->ItemIndex][2] )
     {
     case 'D':
     case 'd':
          f = Projet->Dirs->GetItem( Liste->ItemIndex );
          f->open = !f->open;
          Redraw();
          break;
     default:
          break;
     }
}
//---------------------------------------------------------------------------
void *FindType( void *t, void *nom )
{
     AnsiString N = *((AnsiString*)nom);
     LPType f = (LPType )t;
     if( f->Identif == N )
         return t;
     return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ListeClick(TObject *Sender)
{
     AnsiString n;

     PAKDrag = false;
     if( Liste->ItemIndex == -1 )
         return;
     CurF = Projet->Dirs->GetItem( Liste->ItemIndex );

     switch( Liste->Items->Strings[Liste->ItemIndex][2] )
     {
     case 'P':
          n = Liste->Items->Strings[Liste->ItemIndex];
          n.Delete(1,2);
          PAKForm->CurPak = Projet->FindPak( n );
          if( PAKForm->CurPak != NULL )
          {
              PAKForm->FormShow( Sender );
              PAKForm->Visible = true;
          }
          else
              PAKForm->Visible = true;
          TypesForm->Visible = false;
          PlugForm->Visible = false;
          break;
     case 'F':
          n = Liste->Items->Strings[Liste->ItemIndex];
          n.Delete(1,2);
          TypesForm->CurType = (LPType)Projet->FTypes->EnumTypes( FindType, &n );
          if( TypesForm->CurType != NULL )
          {
              TypesForm->FormShow( Sender );
              TypesForm->Visible = true;
          }
          else
              TypesForm->Visible = false;
          PAKForm->Visible = false;
          PlugForm->Visible = false;
          break;
     default:
          LPPlugin p;
          p = Plugins->GetPlugByChar( Liste->Items->Strings[Liste->ItemIndex][2] );
          if( p != NULL )
              Plugins->RequestEvent(EVT_LCLICK,p,Liste->Items->Strings[Liste->ItemIndex].c_str()+2,NULL);
          break;
     }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AutoPosTimer(TObject *Sender)
{
    PosSubForm( PAKForm );
    PosSubForm( TypesForm );
    PosSubForm( PlugForm );
    if( ListPanel->Visible )
    {
        ConsoleForm->Top = Liste->Top+Liste->Height+47+MainForm->Top+ListPanel->Top;
        ConsoleForm->Left = ListPanel->Left+MainForm->Left+Liste->Left+4;
        ConsoleForm->Height = 120;
        ConsoleForm->BorderStyle = bsNone;
    }
    PAKBrowser->Left = ListPanel->Left+MainForm->Left+6;
    PAKBrowser->Width = ListPanel->Width-6;
    PAKBrowser->Top = ListPanel->Top+MainForm->Top+44;
    PAKBrowser->Height = Liste->Height+35;
    PAKBrowser->PAKList->Height = PAKBrowser->Height - 50;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NewFilesTypeClick(TObject *Sender)
{
     AnsiString a;
     LPFolder f = Projet->Dirs->GetItem( Liste->ItemIndex );
     if( f == NULL )
         return;

     if( ! Input( this, "Ident:","",&a ) )
         return;
     if ( a != "" )
     {
         Projet->Dirs->AddItem( "F"+a, f );
         Projet->FTypes->NewType( a );
         Redraw();
     }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SaveProjectClick(TObject *Sender)
{
     Projet->Save();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DeleteClick(TObject *Sender)
{
     AnsiString a = Liste->Items->Strings[Liste->ItemIndex];

     switch( a[2] )
     {
     case 'D':
     case 'd':
          Projet->Dirs->DelItem( Projet->Dirs->GetItem( Liste->ItemIndex ) );
          break;
     case 'P':
          Projet->Dirs->DelItem( Projet->Dirs->GetItem( Liste->ItemIndex ) );
          a.Delete(1,2);
          Projet->DelPak( a );
          break;
     case 'F':
          Projet->Dirs->DelItem( Projet->Dirs->GetItem( Liste->ItemIndex ) );
          a.Delete(1,2);
          Projet->FTypes->Delete( a );
          break;
     default:
          LPPlugin p;
          p = Plugins->GetPlugByChar( a[2] );
          if( p != NULL && Plugins->RequestEvent(EVT_DELETE,p,a.c_str()+2,NULL) )
          {
              Projet->Dirs->DelItem( Projet->Dirs->GetItem( Liste->ItemIndex ) );
              a.Delete(1,2);
              Projet->FTypes->Delete( a );
          }
          break;
     }
     Redraw();
     ListeClick( Sender );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ListeDrawItem(TWinControl *Control, int Index,
   TRect &Rect, TOwnerDrawState State)
{
   TListBox *ListBox = (dynamic_cast<TListBox*>(Control));
   TCanvas *Canvas = ListBox->Canvas;
   Graphics::TBitmap *bmp;
   AnsiString L=ListBox->Items->Strings[Index];
   int xdec = (L[1]-'0')*15;
   bool erase;

   Canvas->FillRect(Rect);
   bmp = GetIcon(L[2], &erase );
   Canvas->Draw( Rect.Left+xdec+2, Rect.Top, bmp );
   L.Delete(1,2);
   Canvas->TextOut(Rect.Left + 22+ xdec, Rect.Top, L);
   if( erase )
      delete bmp;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CutClick(TObject *Sender)
{
   LPFolder oldC = CutF;
   CutF = Projet->Dirs->CutItem( Projet->Dirs->GetItem( Liste->ItemIndex ) );
   if( oldC != NULL )
   {
       if( CutF != NULL )
           Projet->Dirs->KillItem( oldC );
       else
           CutF = oldC;
   }
   Redraw();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PasteClick(TObject *Sender)
{
   if( CutF != NULL )
   {
       CurF = CutF;
       ListeDragDrop( NULL, NULL, -1, (Liste->ItemIndex-Liste->TopIndex)*Liste->ItemHeight );
   }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ListeDragOver(TObject *Sender, TObject *Source,
        int X, int Y, TDragState State, bool &Accept)
{
    if( Liste->TopIndex + (Y/Liste->ItemHeight) < Liste->Items->Count )
       Accept = true;
    else
       Accept = false;
}
//---------------------------------------------------------------------------
void *
GetIndex( void *p, void * c)
{
  if( p == c )
      return p;
  IndexCount++;
  return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ListeDragDrop(TObject *Sender, TObject *Source,
        int X, int Y)
{
   int DestIndex = Liste->TopIndex+(Y/Liste->ItemHeight);
   int pereIndex;
   LPFolder pere;
   LPFolder f;

   if( !PAKDrag )
   {
       pereIndex = DestIndex;
       IndexCount = DestIndex;
       pere = Projet->Dirs->GetItem( pereIndex );

       if( pere != NULL && pere->Nom[1]!='D' && pere->Nom[1]!='d' )
       {
           pere = pere->Pere;
           IndexCount = 0;
           Projet->Dirs->EnumItems( Projet->Dirs->GetItem(0),GetIndex, pere );
       }
       if( pere != NULL )
       {
           f = pere;
           while( f != NULL )
           {
              if( CurF == f )
                  return;
              f = f->Pere;
           }
           if( X != -1 )
               CutF = Projet->Dirs->CutItem( CurF );
           if( CutF != NULL  )
           {
               Projet->Dirs->InsItem( pere, CutF, DestIndex-IndexCount );
               Redraw();
               for( IndexCount = 0; IndexCount< Liste->Items->Count; IndexCount ++ )
               {
                    AnsiString a = Liste->Items->Strings[ IndexCount ];
                    a.Delete(1,1);
                    if( a == CurF->Nom )
                        break;
               }
               Liste->ItemIndex = IndexCount;
               CutF = NULL;
           }
       }
   }
   else
   {
       AnsiString a = Liste->Items->Strings[ DestIndex ];
       LPPlugin p = Plugins->GetPlugByChar( a[2] );
       if( p != NULL )
           Plugins->RequestEvent( EVT_DROP, p, a.c_str()+2, &PAKItemID ); 
   }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RenameClick(TObject *Sender)
{
     AnsiString a = Liste->Items->Strings[Liste->ItemIndex];
     LPFolder f = Projet->Dirs->GetItem( Liste->ItemIndex );

     AnsiString b,rname;
     char c;

     if( f == NULL || Liste->ItemIndex == 0 )
         return;

     b = f->Nom;
     c = b[1];
     b.Delete(1,1);
     if( ! Input(this,"Enter new Name", b, &b ) )
         return;
     if( b == "" )
         return;
     if( Projet->Dirs->GetItem( b, -1 ) != NULL )
     {
         Log("Name already used...");
         return;
     }

     switch( a[2] )
     {
     case 'D':
     case 'd':
          b = (AnsiString)c + b;
          f->Nom = b;
          break;
     case 'F':
          b = (AnsiString)c + b;
          f->Nom = b;
          break;
     case 'P':
          CPak *pak;
          rname = f->Nom;
          rname.Delete(1,1);
          pak = Projet->FindPak( rname );
          if( pak != NULL && MoveFile( (pak->PathName + pak->PakName+".pak").c_str(), (pak->PathName+b+".pak").c_str() ))
          {
              pak->PakName = b;
              b = (AnsiString)c + b;
              f->Nom = b;
          }
          break;
     default:
          LPPlugin p;
          p = Plugins->GetPlugByChar( a[2] );
          if( p != NULL && Plugins->RequestEvent(EVT_RENAME,p,f->Nom.c_str()+1,b.c_str()) )
          {
              b = (AnsiString)c + b;
              f->Nom = b;
          }
          break;
     }
     Redraw();
     ListeClick( Sender );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ExitWithoutSaveClick(TObject *Sender)
{
     delete Projet;
     Projet = NULL;
     ExitClick( Sender );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FastLoadClick(TObject *Sender)
{
     AnsiString Nom = ((TMenuItem *)Sender)->Caption;
     Nom.Delete(1,3);
     FILE *f = fopen( Nom.c_str(), "rb" );
     if( f == NULL )
     {
         ((TMenuItem *)Sender)->Caption.Delete(2,1024);
         ((TMenuItem *)Sender)->Caption = ((TMenuItem *)Sender)->Caption + "_ ";
         ((TMenuItem *)Sender)->Enabled = false;
         Log("File not found: "+Nom);
         Olds[ ((TMenuItem *)Sender)->Caption[1]-'1' ] = "";
         return;
     }
     else
         fclose(f);

     if( !TestLock(Nom) )
         return;
     Projet = new CProjet( Nom );
     Projet->Load();
     Init();
     if( Projet->Prefs.DefPath != "" )
        PAKForm->Dirs->Directory = Projet->Prefs.DefPath;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ListeKeyUp(TObject *Sender, WORD &Key,
        TShiftState Shift)
{
     if( Key == 46 )
         DeleteClick( Sender );
     if( Key == 113 )
         RenameClick( Sender );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ProjectPrefsClick(TObject *Sender)
{
     PPrefForm->Visible = true;
     MainForm->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PlugInfoClick(TObject *Sender)
{
     if( Plugins->liste  != NULL )
     {
         MainForm->Enabled = false;
         PlugInfoForm->Visible = true;
     }
     else
         Log("No plugins loaded");
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PlugMenuClick(TObject *Sender)
{
     TMenuItem *i = (TMenuItem *)Sender;
     ((TSpeedButton *)i->Tag )->Click();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PAKBrowserViewClick(TObject *Sender)
{
     PAKBrowser->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PlugButtonClick(TObject *Sender)
{
     LPPlugObj o = Plugins->GetPlugObj(NULL,Sender);
     if( o == NULL )
     {
         Log("ERROR: PlugObject is not referenced by any plugin");
         return;
     }
     o->Event( Sender, (void *)EVT_LCLICK, o->Class );
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::PlugButtonMouseDown(TObject *Sender,
        TMouseButton Button, TShiftState Shift, int X, int Y)
{
     LPPlugObj o = Plugins->GetPlugObj(NULL,Sender);
     if( o == NULL )
     {
         Log("ERROR: PlugObject is not referenced by any plugin");
         return;
     }
     if( Button == mbRight )
          o->Event( Sender, (void *)EVT_RCLICK, o->Class );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PlugButtonDblClick(TObject *Sender)
{
     LPPlugObj o = Plugins->GetPlugObj(NULL,Sender);
     if( o == NULL )
     {
         Log("ERROR: PlugObject is not referenced by any plugin");
         return;
     }
     o->Event( Sender, (void *)EVT_DCLICK, o->Class );
}
//---------------------------------------------------------------------------

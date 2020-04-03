//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include "plugins.h"
#include "Main_Form.h"
#include "PAK_Browser.h"
#include "Plug_Form.h"
#include "Console.h"
#include "Input_Form.h"
#include "PRJ_Defs.h"
#include "Load_Form.h"
#pragma hdrstop

#define SetField(field,size) {                                               \
         if( Plugin == NULL )                                                \
         {                                                                   \
             if( IsBadWritePtr(val, sizeof(size) ) )                         \
                 return false;                                               \
             *((size*)val) = o->Obj.control->field;                          \
         }                                                                   \
         else                                                                \
             o->Obj.control->field = *((size*)val);                          \
         return true;                                                        \
     }

#define SetMenuField(field,size) {                                           \
         if( Plugin == NULL )                                                \
         {                                                                   \
             if( IsBadWritePtr(val, sizeof(size) ) )                         \
                 return false;                                               \
             *((size*)val) = o->Obj.menu->field;                             \
         }                                                                   \
         else                                                                \
             o->Obj.menu->field = *((size*)val);                             \
         return true;                                                        \
     }

#define SetString(field) {                                                   \
         if( Plugin == NULL )                                                \
         {                                                                   \
             if( IsBadWritePtr(val,o->Obj.control->field.Length()+1  ) )     \
                 return false;                                               \
             strcpy( (char*)val, o->Obj.control->field.c_str());             \
         }                                                                   \
         else                                                                \
         {                                                                   \
             o->Obj.control->field = (char *)val;                            \
             o->Obj.control->field.Unique();                                 \
         }                                                                   \
         return true;                                                        \
     }

#define SetMenuString(field) {                                               \
         if( Plugin == NULL )                                                \
         {                                                                   \
             if( IsBadWritePtr(val,o->Obj.menu->field.Length()+1  ) )        \
                 return false;                                               \
             strcpy( (char*)val, o->Obj.menu->field.c_str() );               \
         }                                                                   \
         else                                                                \
         {                                                                   \
             o->Obj.menu->field = (char *)val;                               \
             o->Obj.menu->field.Unique();                                    \
         }                                                                   \
         return true;                                                        \
     }

//---------------------------------------------------------------------------
bool
CActions::SaveFile( void *FileMask, void *Title, void *buf, void * )
{
    AnsiString a = LoadForm->Save( PlugForm, (char*)FileMask, (char*)Title, "Save" );
    strcpy( (char*)buf, a.c_str() );
    return ( a != "" );
}
//---------------------------------------------------------------------------
bool
CActions::LoadFile( void *FileMask, void *Title, void *buf, void * )
{
    AnsiString a = LoadForm->Load( PlugForm, (char*)FileMask, (char*)Title );
    strcpy( (char*)buf, a.c_str() );
    return ( a != "" );
}
//---------------------------------------------------------------------------
bool
CActions::RunPlugin( void *PlugName, void *Function, void *p1, void *p2 )
{
    FARPROC FProc;
    LPPlugin p = Plugins->liste;
    AnsiString n = (char*)PlugName, nom;
    n = n.LowerCase();

    while( p != NULL )
    {
         nom = p->File;
         nom = nom.LowerCase();
         if( nom == n )
         {
             FProc = GetProcAddress( p->handle, (char *)Function);
             if( FProc == NULL )
              	return false;
             return ((PLUGPROC)FProc)( p1, p2 );
         }
         p = p->Suivant;
    }
    return false;
}
//---------------------------------------------------------------------------
bool
CActions::ObjSetField( void *Plugin, void *obj, void *val, void *Field )
{
   LPPlugObj o = Plugins->GetPlugObj( Plugin, obj );
   static int Index = -1;

   if( o == NULL )
       return false;

   switch( o->Type)
   {
   /* TControl's */
   case LABEL:
   case BUTTON:
   case SBUTTON:
   case LIST:
   case GRID:
   case EDIT:
   case COMBO:
      switch( (int)Field )
      {
      case F_TOP:
           SetField( Top, int );
      case F_LEFT:
           SetField( Left, int );
      case F_HEIGHT:
           SetField( Height, int );
      case F_WIDTH:
           SetField( Width, int );
      case F_ENABLED:
           SetField( Enabled, bool );
      case F_VISIBLE:
           SetField( Visible, bool );
      case F_CAPTION:
           if( o->Type != BUTTON && o->Type != LABEL )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, o->Obj.button->Caption.Length()+1 ) )
                   return false;
               strcpy((char*)val, o->Obj.button->Caption.c_str() );
           }
           else
           {
               o->Obj.button->Caption = (char*)val;
               o->Obj.button->Caption.Unique();
           }
           return true;
      case F_TEXT:
           if( o->Type != EDIT )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, o->Obj.edit->Text.Length()+1 ) )
                   return false;
               strcpy((char*)val, o->Obj.edit->Text.c_str() );
           }
           else
           {
               o->Obj.edit->Text = (char*)val;
               o->Obj.edit->Text.Unique();
           }
           return true;
      case F_HINT:
           SetString( Hint );
      case F_ICON:
           if( o->Type != SBUTTON )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr(val,sizeof(HBITMAP) ) )
                 return false;
               *((HBITMAP *)val) = o->Obj.speedb->Glyph->Handle;
           }
           else
           {
               o->Obj.speedb->Glyph = MainForm->SaveProject->Glyph;
               o->Obj.speedb->Glyph->Handle = (HBITMAP)val;
           }
           return true;
      case F_INDEX:
           if( o->Type != LIST && o->Type != COMBO )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, sizeof(int) ) )
                   return false;
               if( o->Type == LIST )
                  *((int *)val) = o->Obj.list->ItemIndex;
               else
                  *((int *)val) = o->Obj.combo->ItemIndex;
           }
           else
           {
               if( o->Type == LIST )
                   o->Obj.list->ItemIndex = *((int *)val);
               else
                   o->Obj.combo->ItemIndex = *((int *)val);
           }
           return true;
      case F_TOPINDEX:
           if( o->Type != LIST )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, sizeof(int) ) )
                   return false;
               *((int *)val) = o->Obj.list->TopIndex;
           }
           else
               o->Obj.list->TopIndex = *((int *)val);
           return true;
      case F_ITEMCOUNT:
           if( o->Type != LIST && o->Type != COMBO )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, sizeof(int) ) )
                   return false;
               *((int *)val) = o->Obj.combo->Items->Count;
           }
           else
               return false;
           return true;
      case F_SETINDEX:
           Index = *((int*)val);
           return true;
      case F_ITEM:
           if( o->Type != LIST && o->Type != COMBO )
               return false;
           if( Index < 0 || Index >= o->Obj.list->Items->Count )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, o->Obj.list->Items->Strings[Index].Length()+1 ) )
                   return false;
               strcpy( (char*)val,  o->Obj.list->Items->Strings[Index].c_str() );
           }
           else
           {
               o->Obj.list->Items->Strings[Index] = (char*)val;
               o->Obj.list->Items->Strings[Index].Unique();
           }
           Index = -66666;
           return true;
      case F_ADDITEM:
           if( (o->Type != LIST && o->Type != COMBO)  || Plugin == NULL )
               return false;
           o->Obj.combo->Items->Add( (char*)val );
           return true;
      case F_DELITEM:
           if( (o->Type != LIST && o->Type != COMBO)  || Plugin == NULL )
               return false;
           o->Obj.combo->Items->Delete( *((int*)val) );
           return true;
      case F_CLEAR:
           if( o->Type != LIST && o->Type != GRID )
               return false;
           if( o->Type == LIST )
               o->Obj.list->Clear();
           else
           {
               int i;
               for( i=0; i< o->Obj.grid->ColCount; i++)
                   o->Obj.grid->Cols[i]->Clear();
           }
           return true;
      case F_ROWCOUNT:
           if( o->Type != GRID )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, sizeof(int) ) )
                   return false;
               *((int*)val) = o->Obj.grid->RowCount;
           }
           else
               o->Obj.grid->RowCount = *((int*)val);
           return true;
      case F_COLCOUNT:
           if( o->Type != GRID )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, sizeof(int) ) )
                   return false;
               *((int*)val) = o->Obj.grid->ColCount;
           }
           else
               o->Obj.grid->ColCount = *((int*)val);
           return true;
      case F_FIXCOLS:
           if( o->Type != GRID )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, sizeof(int) ) )
                   return false;
               *((int*)val) = o->Obj.grid->FixedCols;
           }
           else
               o->Obj.grid->FixedCols = *((int*)val);
           return true;
      case F_ROWHEIGHT:
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, sizeof(int) ) )
                   return false;
               *((int*)val) = o->Obj.grid->DefaultRowHeight;
           }
           else
               o->Obj.grid->DefaultRowHeight = *((int*)val);
           return true;
      case F_COLWIDTHS:
           if( Index < 0 || Index >= o->Obj.grid->ColCount )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, sizeof(int) ) )
                   return false;
              *((int*)val) = o->Obj.grid->ColWidths[Index];
           }
           else
              o->Obj.grid->ColWidths[Index] = *((int*)val);
           return true;
      case F_FIXROWS:
           if( o->Type != GRID )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, sizeof(int) ) )
                   return false;
               *((int*)val) = o->Obj.grid->FixedRows;
           }
           else
               o->Obj.grid->FixedRows = *((int*)val);
           return true;
      case F_SELECTION:
           if( o->Type != GRID )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, sizeof(RECT) ) )
                   return false;
               ((RECT*)val)->top = o->Obj.grid->Selection.Top;
               ((RECT*)val)->left = o->Obj.grid->Selection.Left;
               ((RECT*)val)->bottom = o->Obj.grid->Selection.Bottom;
               ((RECT*)val)->right = o->Obj.grid->Selection.Right;
           }
           else
           {
               o->Obj.grid->Selection.Top = ((RECT*)val)->top;
               o->Obj.grid->Selection.Left = ((RECT*)val)->left;
               o->Obj.grid->Selection.Right = ((RECT*)val)->right;
               o->Obj.grid->Selection.Bottom = ((RECT*)val)->bottom;
           }
           return true;
      case F_GRIDITEM:
           if( o->Type != GRID )
               return false;
           if( Index%(1<<18) < 0 || Index%(1<<18) >= o->Obj.grid->ColCount ||
               (Index>>18) < 0 && (Index>>18) >= o->Obj.grid->RowCount )
                  return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, o->Obj.grid->Cells[Index%(1<<18)][(Index>>18)].Length()+1 ) )
                   return false;
               strcpy( (char*)val, o->Obj.grid->Cells[Index%(1<<18)][(Index>>18)].c_str() );
           }
           else
           {
               o->Obj.grid->Cells[Index%(1<<18)][(Index>>18)] = (char*)val;
               o->Obj.grid->Cells[Index%(1<<18)][(Index>>18)].Unique();
           }
           return true;
      case F_GRIDOPT:
           if( o->Type != GRID )
               return false;
           if( Plugin == NULL )
           {
               if( IsBadWritePtr( val, sizeof(int) ) )
                   return false;
               (*((int*)val)) = 0;
               if( o->Obj.grid->Options.Contains(goFixedVertLine) )
                   (*((int*)val)) += FIXED_VLINE;
               if( o->Obj.grid->Options.Contains(goFixedHorzLine) )
                   (*((int*)val)) += FIXED_HLINE;
               if( o->Obj.grid->Options.Contains(goVertLine) )
                   (*((int*)val)) += DRAW_VLINE;
               if( o->Obj.grid->Options.Contains(goHorzLine) )
                   (*((int*)val)) += DRAW_HLINE;
               if( o->Obj.grid->Options.Contains(goRangeSelect) )
                   (*((int*)val)) += RANGE_SEL;
               if( o->Obj.grid->Options.Contains(goRowSizing) )
                   (*((int*)val)) += ROW_SIZING;
               if( o->Obj.grid->Options.Contains(goColSizing) )
                   (*((int*)val)) += COL_SIZING;
               if( o->Obj.grid->Options.Contains(goEditing) )
                   (*((int*)val)) += CANEDIT;
           }
           else
           {
               o->Obj.grid->Options.Clear();
               if( (*((int*)val)) & FIXED_VLINE )
                   o->Obj.grid->Options<<goFixedVertLine;
               if( (*((int*)val)) & FIXED_HLINE )
                   o->Obj.grid->Options<<goFixedHorzLine;
               if( (*((int*)val)) & DRAW_VLINE )
                   o->Obj.grid->Options<<goVertLine;
               if( (*((int*)val)) & DRAW_HLINE )
                   o->Obj.grid->Options<<goHorzLine;
               if( (*((int*)val)) & RANGE_SEL )
                   o->Obj.grid->Options<<goRangeSelect;
               if( (*((int*)val)) & ROW_SIZING )
                   o->Obj.grid->Options<<goRowSizing;
               if( (*((int*)val)) & COL_SIZING )
                   o->Obj.grid->Options<<goColSizing;
               if( (*((int*)val)) & CANEDIT )
                   o->Obj.grid->Options<<goEditing;
           }
           return true;
      default:
           return false;
      }
   /* TMenuItem */
   case MENUIT:
      switch( (int)Field )
      {
      case F_PARENT:
           if( Plugin != NULL )
           {
               o->Obj.menu->Parent->Remove( o->Obj.menu );
               ((TMenuItem *)val)->Add(o->Obj.menu);
           }
           else
               return false;
           return true;
      case F_CHECKED:
           SetMenuField( Checked, bool );
      case F_ENABLED:
           SetMenuField( Enabled, bool );
      case F_VISIBLE:
           SetMenuField( Visible, bool );
      case F_CAPTION:
           SetMenuString( Caption );
      case F_HINT:
           SetMenuString( Hint );
      default:
           return false;
      }
   }
   return false;
}
//---------------------------------------------------------------------------
bool
CActions::CnvIDToFileName( void *ID, void *buf, NOTUSED, NOTUSED )
{
    int i;
    WORD PakID = (WORD)((*((DWORD*)ID))>>16);
    WORD ResID = (WORD)((*((DWORD*)ID))&65535);

    for(i=0; i< Projet->PakCount; i++)
    {
        if( Projet->Paks[i]->PakNb+1 == PakID )
        {
            LPPakItem it = Projet->Paks[i]->FindItem( ResID );
            if( it == NULL )
                return false;
            if( IsBadWritePtr(buf, it->Nom.Length()+1 ) )
                return false;
            strcpy((char*)buf, it->Nom.c_str() );
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool
CActions::RunPAKBrowser( void *pakname, NOTUSED, NOTUSED, NOTUSED )
{
   PAKBrowser->Visible = true;
   PAKBrowser->PakName->Text = (char*)pakname;
   return true;
}
//---------------------------------------------------------------------------
bool
CActions::RegisterButton( void *Plugin, void *obj, void *Event, void *Class )
{
   TSpeedButton *but;
   LPPlugin p = Plugins->GetPlugin( Plugin );

   if( p == NULL || IsBadWritePtr(obj,4) )
       return false;

   but = new TSpeedButton(NULL);
   but->Parent = MainForm->NormalPanel;
   but->Top = MainForm->SaveProject->Top;
   but->Left = 2;
   but->Width = 35;
   but->Height = 35;
   but->Down = false;
   but->AllowAllUp = true;
   but->Hint = "";
   but->ShowHint = true;
   but->OnClick = MainForm->PlugButtonClick;
   but->OnMouseDown = MainForm->PlugButtonMouseDown;
   MainForm->NormalPanel->Height+=44;
   MainForm->SaveProject->Top+=44;

   Plugins->AddObj( p, but, SBUTTON, (PLUGPROC)Event, Class );
   *((void **)obj) = (void *)but;
   return true;
}
//---------------------------------------------------------------------------
bool
CActions::RegisterMenu( void *Plugin, void *obj, void *Event, void *Class )
{
   TMenuItem *menu;
   LPPlugin p = Plugins->GetPlugin( Plugin );

   if( p == NULL || IsBadWritePtr(obj,4) )
       return false;

   menu = new TMenuItem(NULL);
   MainForm->MainMenu->Items->Add(menu);
   menu->OnClick = MainForm->PlugButtonClick;
   Plugins->AddObj( p, (TControl *)menu, MENUIT, (PLUGPROC)Event, Class );
   *((void **)obj) = (void *)menu;
   return true;
}
//---------------------------------------------------------------------------
bool
CActions::CreateObj( void *Plugin, void *Type, void *obj, NOTUSED )
{
   LPPlugin p = Plugins->GetPlugin( Plugin );
   if( p == NULL || IsBadWritePtr(obj,4) )
       return false;

   switch((int)Type)
   {
   case LABEL:
       TLabel *lab;
       lab = new TLabel((void*)NULL);
       lab->Parent = NULL;
       lab->OnClick = MainForm->PlugButtonClick;
       lab->OnMouseDown = MainForm->PlugButtonMouseDown;
       lab->OnDblClick = MainForm->PlugButtonDblClick;
       Plugins->AddObj( p, lab, LABEL, NULL, NULL );
       *((void **)obj)=lab;
       break;
   case BUTTON:
       TButton *but;
       but = new TButton((void*)NULL);
       but->Parent = NULL;
       but->OnClick = MainForm->PlugButtonClick;
       but->OnMouseDown = MainForm->PlugButtonMouseDown;
       Plugins->AddObj( p, but, BUTTON, NULL, NULL );
       *((void **)obj)=but;
       break;
   case LIST:
       TListBox *list;
       list = new TListBox((void*)NULL);
       list->Parent = PlugForm;
       list->OnClick = MainForm->PlugButtonClick;
       list->OnDblClick = MainForm->PlugButtonDblClick;
       list->OnMouseDown = MainForm->PlugButtonMouseDown;
       list->OnKeyDown = PlugForm->PlugEditPressKey;
       Plugins->AddObj( p, list, LIST, NULL, NULL );
       *((void **)obj)=list;
       break;
   case EDIT:
       TEdit *edit;
       edit = new TEdit((void*)NULL);
       edit->Parent = NULL;
       edit->OnChange = PlugForm->PlugEditChange;
       edit->OnKeyDown = PlugForm->PlugEditPressKey;
       Plugins->AddObj( p, edit, EDIT, NULL, NULL );
       *((void **)obj)=edit;
       break;
   case COMBO:
       TComboBox *combo;
       combo = new TComboBox((void*)NULL);
       combo->Parent = PlugForm;
       combo->Style = csDropDownList;
       combo->OnChange = PlugForm->PlugEditChange;
       Plugins->AddObj( p, combo, COMBO, NULL, NULL );
       *((void **)obj)=combo;
       break;
   case GRID:
       TStringGrid *grid;
       grid = new TStringGrid((void*)NULL);
       grid->Parent = PlugForm;
       grid->OnClick = MainForm->PlugButtonClick;
       grid->OnDblClick = MainForm->PlugButtonDblClick;
       grid->OnMouseDown = MainForm->PlugButtonMouseDown;
       grid->OnKeyDown = PlugForm->PlugEditPressKey;
       Plugins->AddObj( p, grid, GRID, NULL, NULL );
       *((void **)obj)=grid;
       break;
   default:
       return false;
   }
   return true;
}
//---------------------------------------------------------------------------
bool
CActions::SetEventFunc( void *Plugin, void *Obj, void *Event, void *Class )
{
   LPPlugin p = Plugins->GetPlugin( Plugin );
   if( p == NULL )
       return false;
   LPPlugObj o = p->objets;
   while( o != NULL )
   {
       if( o->Obj.obj == Obj )
       {
           o->Event = (PLUGPROC)Event;
           o->Class = Class;
           return true;
       }
       o = o->Suivant;
   }
   return false;
}
//---------------------------------------------------------------------------
bool
CActions::DeleteObj( void *Plugin, void *obj, NOTUSED, NOTUSED )
{
   LPPlugObj o = Plugins->GetPlugObj( Plugin, obj );
   if( o == NULL )
      return false;
   Plugins->DelObj( o );
   return true;
}
//---------------------------------------------------------------------------
void
CActions::DeleteOInst( LPPlugObj o )
{
   switch( o->Type )
   {
   case GRID:
        delete o->Obj.grid;
        break;
   case COMBO:
        delete o->Obj.combo;
        break;
   case EDIT:
        delete o->Obj.edit;
        break;
   case MENUIT:
        delete o->Obj.menu;
        break;
   case LABEL:
        delete o->Obj.label;
        break;
   case BUTTON:
        delete o->Obj.button;
        break;
   case LIST:
        delete o->Obj.list;
        break;
   case SBUTTON:
        delete o->Obj.speedb;
        break;
   default:
        delete o->Obj.obj;
        break;
   }
}
//---------------------------------------------------------------------------
bool
CActions::PlugFormVis( void *Plugin, void *state, NOTUSED, NOTUSED )
{
    LPPlugin p = Plugins->GetPlugin( Plugin );
    if( Plugin == NULL )
    {
        if( IsBadWritePtr( state, sizeof(bool) ) )
            return false;
        *((bool *)state) = PlugForm->Visible;
    }
    else
    {
        PlugForm->Plugin = p;
        PlugForm->Visible = *((bool *)state);
        if( PlugForm->Visible )
            PlugForm->FormShow( NULL );
    }

    return true;
}
//---------------------------------------------------------------------------
bool
CActions::LogTxt( void *Txt, NOTUSED, NOTUSED, NOTUSED )
{
    Log( (char*)Txt );
    return true;
}
//---------------------------------------------------------------------------
bool
CActions::InputText( void *buf, void *title, void *def, NOTUSED )
{
    AnsiString txt;
    if( !Input( PlugForm, (char*)title, (char*)def, &txt ) )
        return false;
    if( IsBadWritePtr(buf, txt.Length()+1 )  )
        return false;
    strcpy( (char*)buf, txt.c_str() );
    return true; 
}
//---------------------------------------------------------------------------
bool
CActions::CreateItem( void *Plugin, void *nom, NOTUSED, NOTUSED )
{
     AnsiString a = (char*)nom;
     LPFolder f = Projet->Dirs->GetItem( MainForm->Liste->ItemIndex );
     LPPlugin p = Plugins->GetPlugin( Plugin );

     if( p == NULL )
         return false;
     if( f == NULL )
         f = Projet->Dirs->GetItem( 0 );
     Projet->Dirs->AddItem( (AnsiString)(char)p->Pred + a, f );
     MainForm->Redraw();
     return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#ifndef PluginsH
#define PluginsH
//---------------------------------------------------------------------------
#include "pluginclass.h"
#include "PAK_Form.h"

//---------------------------------------------------------------------------
typedef struct TPlugObj {
      union
      {
           void *obj;
           TControl *control;
           TSpeedButton *speedb;
           TButton *button;
           TLabel *label;
           TListBox *list;
           TMenuItem *menu;
           TEdit *edit;
           TComboBox *combo;
           TStringGrid *grid;

      } Obj;
      BYTE Type;
      PLUGPROC Event;
      void *Class;

      struct TPlugObj *Suivant;
} TPlugObj, *LPPlugObj;
//---------------------------------------------------------------------------
typedef struct TPlugin {
      LPPlugInfo inf;
      HMODULE handle;
      AnsiString File;
      char Pred;
      LPPlugObj objets;

      struct TPlugin *Suivant;
} TPlugin, *LPPlugin;
//---------------------------------------------------------------------------
class CPlugins {

      void Clean();
      bool Charge( AnsiString Nom );
      char CurPred;
      TActions Actions;
public:

      LPPlugin   liste;

      void       SendMainEvent( DWORD EventType, void *Param );
      bool       RequestEvent( DWORD EventType, LPPlugin p, void *p1, void *p2 );

      LPPlugin   GetPlugin( void *plugin );
      LPPlugin   GetPlugByChar( char c );
      LPPlugin   GetPlugByBal( AnsiString b );
      LPPlugObj  GetPlugObj( void *plugin, void *obj );
      void       AfficheObjs( LPPlugin curP );
      void       AddObj( LPPlugin p, TControl *obj, BYTE Type, PLUGPROC Event, void *Class );
      void       DelObj( LPPlugObj o );
      void       Charge();

      CPlugins();
      ~CPlugins();
};
//---------------------------------------------------------------------------
class CActions {

public:

static bool LogTxt   ( void *Txt, NOTUSED, NOTUSED, NOTUSED );
static bool RegisterButton( void *Plugin, void *obj, void *Event, void *Class );
static bool RegisterMenu( void *Plugin, void *obj, void *Event, void *Class );

static bool CreateObj( void *Plugin, void *_Type, void *obj, NOTUSED );
static bool SetEventFunc( void *Plugin, void *Obj, void *Event, void *Class );
static bool DeleteObj( void *Plugin, void *obj, NOTUSED, NOTUSED );
static void DeleteOInst( LPPlugObj o );

static bool ObjSetField( void *Plugin, void *obj, void *val, void *Field );

static bool PlugFormVis( void *Plugin, void *state, NOTUSED, NOTUSED );
static bool RunPAKBrowser( void *pakname, NOTUSED, NOTUSED, NOTUSED );
static bool CnvIDToFileName( void *ID, void *buf, NOTUSED, NOTUSED );

static bool InputText( void *buf, void *title, void *def, NOTUSED );
static bool CreateItem( void *Plugin, void *nom, NOTUSED, NOTUSED );

static bool RunPlugin( void *PlugName, void *Function, void *p1, void *p2 );

static bool LoadFile( void *FileMask, void *Title, void *buf, NOTUSED );
static bool SaveFile( void *FileMask, void *Title, void *buf, NOTUSED );

};
//---------------------------------------------------------------------------
extern CPlugins *Plugins;
//---------------------------------------------------------------------------
#endif

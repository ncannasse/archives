//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Plugins.h"
#include "Console.h"
#include "Main_Form.h"
#include "Plug_Form.h"
#include "Input_Form.h"
#include "PRJ_Defs.h"
#include "PAK_Browser.h"
#include "PAK_Form.h"
#include "Types_Form.h"
//---------------------------------------------------------------------------
CPlugins *Plugins = NULL;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CPlugins::CPlugins()
{
    liste = NULL;
    Actions.LogTxt = CActions::LogTxt;
    Actions.CreateObj = CActions::CreateObj;
    Actions.SetEventFunc = CActions::SetEventFunc;
    Actions.DeleteObj = CActions::DeleteObj;
    Actions.RegisterButton = CActions::RegisterButton;
    Actions.RegisterMenu = CActions::RegisterMenu;
    Actions.SetField = CActions::ObjSetField;
    Actions.PlugFormVis = CActions::PlugFormVis;
    Actions.RunPAKBrowser = CActions::RunPAKBrowser;
    Actions.CnvIDToFileName = CActions::CnvIDToFileName;
    Actions.InputText = CActions::InputText;
    Actions.CreateItem = CActions::CreateItem;
    Actions.RunPlugin = CActions::RunPlugin;
    Actions.SaveFile = CActions::SaveFile;
    Actions.LoadFile = CActions::LoadFile;
    CurPred = 1;
}
//---------------------------------------------------------------------------
CPlugins::~CPlugins()
{
    Clean();
}
//---------------------------------------------------------------------------
void
CPlugins::Charge()
{
    HANDLE Rech;
    WIN32_FIND_DATA FData;

    Log("Loading Plugins...");
    if ( (Rech=FindFirstFile("*.*",&FData )) == INVALID_HANDLE_VALUE )
    	return;
    while ( FindNextFile( Rech, &FData ) )
    {
    	if( FData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY
    	    && stricmp((char *)(FData.cFileName+strlen(FData.cFileName)-4),".dll")==0)
		if( !Charge(FData.cFileName) )
                    LogPlus(" - Not a Dragoon DLL ");
    }
    FindClose(Rech);
    Log("Loading Plugins [OK]");
}
//---------------------------------------------------------------------------
bool
CPlugins::Charge( AnsiString Nom )
{
    HINSTANCE h = LoadLibrary( Nom.c_str() );
    FARPROC FProc;
    LPPlugin p;

    if( h == NULL )
    {
        Log("Cannot Load DLL: "+Nom );
    	return true;
    }
    FProc = GetProcAddress( h, (char *)1);

    if( FProc == NULL )
    {
        Log("Cannot get DLL entrypoint: "+Nom);
    	FreeLibrary( h );
    	return true;
    }

    Log("Loading: ");
    p = new TPlugin;
    p->inf = new TPlugInfo;
    memset( p->inf, 0, sizeof(TPlugInfo) );
    p->File = Nom;
    p->objets = NULL;
    p->handle = h;
    p->Suivant = liste;
    liste = p;
    if( !((PLUGPROC)FProc)( p->inf, &Actions ) )
    {
        LogPlus(" - Failed to initialize !!");
        liste = p->Suivant;
        delete p;
    	FreeLibrary( h );
    	return true;
    }

    if( p->inf->Code != CODE )
    {
        liste = p->Suivant;
        delete p;
	FreeLibrary( h );
	return false;
    }
    while( CurPred == 'd' || CurPred == 'F' || CurPred == 'D' || CurPred == 'f' )
           CurPred++;
    p->Pred = CurPred;
    LogPlus(p->inf->Name);
    CurPred++;
    return true;
}
//---------------------------------------------------------------------------
LPPlugin
CPlugins::GetPlugin( void *plugin )
{
     LPPlugin l = liste;
     if( plugin == NULL )
         return NULL;
     while( l != NULL )
     {
         if( l->inf->PluginInst == plugin )
             return l;
         l = l->Suivant;
     }
     return NULL;
}
//---------------------------------------------------------------------------
void
CPlugins::AddObj( LPPlugin p, TControl *obj, BYTE Type, PLUGPROC Event, void *Class )
{
     LPPlugObj o = new TPlugObj;
     o->Type = Type;
     o->Obj.control = obj;
     o->Event = Event;
     o->Class = Class;
     o->Suivant = p->objets;
     p->objets = o;
}
//---------------------------------------------------------------------------
void
CPlugins::DelObj( LPPlugObj o )
{
     LPPlugin p = liste;
     LPPlugObj l;
     while( p != NULL )
     {
        l = p->objets;
        if( l != NULL )
        {
            if( l == o )
            {
                p->objets = o->Suivant;
                CActions::DeleteOInst( o );
                delete o;
                return;
            }
            while( l->Suivant != NULL )
            {
                if( l->Suivant == o )
                {
                    l->Suivant = o->Suivant;
                    CActions::DeleteOInst( o );
                    delete o;
                    return;
                }
                l = l->Suivant;
            }
        }
        p = p->Suivant;
     }
}
//---------------------------------------------------------------------------
LPPlugObj
CPlugins::GetPlugObj( void *plugin, void *obj )
{
     LPPlugObj o;
     LPPlugin p;

     if( plugin == NULL )
     {
         p = liste;
         while( p != NULL )
         {
             o = p->objets;
             while( o != NULL )
             {
                 if( o->Obj.obj == obj )
                     return o;
                 o = o->Suivant;
             }
             p = p->Suivant;
         }
     }
     else
     {
         p = GetPlugin( plugin );
         if( p != NULL )
         {
             o = p->objets;
             while( o != NULL )
             {
                 if( o->Obj.obj == obj )
                     return o;
                 o = o->Suivant;
             }
         }
     }
     return NULL;
}
//---------------------------------------------------------------------------
void
CPlugins::Clean()
{
     LPPlugin l = liste;
     LPPlugObj o,oo;

     while( l != NULL )
     {
         liste = l->Suivant;
         FreeLibrary( l->handle );
         delete l->inf;
         o = l->objets;
         while( o != NULL )
         {
             oo = o->Suivant;
             CActions::DeleteOInst(o);
             delete o;
             o = oo;
         } 
         delete l;
         l = liste;
     }
}
//---------------------------------------------------------------------------
void
CPlugins::AfficheObjs( LPPlugin curP )
{
    LPPlugin l = liste;
    LPPlugObj o;
    while( l != NULL )
    {
        o = l->objets;
        while( o != NULL )
        {
            if( o->Type != SBUTTON && o->Type != MENUIT )
                o->Obj.control->Parent = (l==curP?PlugForm:NULL);
            o = o->Suivant;
        }
        l = l->Suivant;
    }
}
//---------------------------------------------------------------------------
void
CPlugins::SendMainEvent( DWORD EventType, void *Param )
{
    LPPlugin p = liste;
    while( p != NULL )
    {
        p->inf->MainEvents( (void*)EventType, Param );
        p = p->Suivant;
    }
}
//---------------------------------------------------------------------------
LPPlugin
CPlugins::GetPlugByChar( char c )
{
    LPPlugin p = liste;
    while( p != NULL )
    {
           if( p->Pred == c )
               return p;
           p = p->Suivant;
    }
    return NULL;
}
//---------------------------------------------------------------------------
LPPlugin
CPlugins::GetPlugByBal( AnsiString b )
{
    LPPlugin p = liste;
    while( p != NULL )
    {
           if( b == p->inf->RegBalise )
               return p;
           p = p->Suivant;
    }
    return NULL;
}
//---------------------------------------------------------------------------
bool
CPlugins::RequestEvent( DWORD EventType, LPPlugin p, void *p1, void *p2 )
{
    if( p == NULL )
        return false;
    return p->inf->MainEvents( (void*)EventType, p1,p2, NULL );
}
//---------------------------------------------------------------------------


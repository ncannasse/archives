#include <windows.h>
#include "pluginclass.h"

CPlugin *Plugin = NULL;

bool
CPlugin::DragoonEntryPoint( LPPlugInfo Inf, LPActions Act )
{
	if( Plugin == NULL )
		return false;
	Plugin->Act = Act;
	Plugin->Inf->HIcon = Plugin->Icon;
	Plugin->Inf->RegBalise = Plugin->Balise.c_str();
	Plugin->Inf->PluginInst = Plugin;
	
	*Inf = *Plugin->Inf;
	return (Plugin->Init == NULL)?true:Plugin->Init(NULL);
	return false;
}

CPlugin::CPlugin( HANDLE h )
{
	Inf = new TPlugInfo;
	Inf->Code = CODE;
	Inf->Name = NULL;
	Inf->Infos = NULL;
	Inf->Author = NULL;
	Inf->RegBalise = NULL;
	Inf->Handle = h;
	Inf->MainEvents = OnEvent;
	Init = NULL;
	OnPAKChange = NULL;
	OnProjectLoad = NULL;
	OnProjectSave = NULL;
	OnProjectInit = NULL;
	Icon = 0;
	OnClick = NULL;
	OnRClick = NULL;
	OnDblClick = NULL;
	OnRename = NULL;
	OnSave = NULL;
	OnDrop = NULL;
	OnLoad = NULL;
	OnDelete = NULL;
}

CPlugin::~CPlugin()
{
	delete Inf->Name;
	delete Inf->Author;
	delete Inf->Infos;
	delete Inf;
}

void
CPlugin::RegisterName( DString Name )
{
	delete Inf->Name;
	Inf->Name = new char[ Name.Length() + 1 ];
	strcpy( Inf->Name, Name.c_str() );
}

void
CPlugin::RegisterAuthor( DString Author )
{
	delete Inf->Author;
	Inf->Author = new char[ Author.Length() + 1 ];
	strcpy( Inf->Author, Author.c_str() );
}

void
CPlugin::RegisterInfos( DString Infos )
{
	delete Inf->Infos;
	Inf->Infos = new char[ Infos.Length() + 1 ];
	strcpy( Inf->Infos, Infos.c_str() );	
}

HANDLE
CPlugin::GetHandle()
{
	return Inf->Handle;
}

void
CPlugin::ShowPlugForm( void )
{
	bool vois = true;
	Plugin->Act->PlugFormVis( Plugin, &vois );
}

void
CPlugin::HidePlugForm( void )
{
	bool cache = false;
	Plugin->Act->PlugFormVis( Plugin, &cache );
}

bool
CPlugin::OnEvent(void *EventType, void *param, void *p2, void * )
{
	switch( (int)EventType )
	{
	case EVT_PRJINIT:
		if( Plugin->OnProjectInit != NULL )
			return Plugin->OnProjectInit((char*) param );
		return false;
	case EVT_PRJLOAD:
		if( Plugin->OnProjectLoad != NULL )
			return Plugin->OnProjectLoad((char*) param );
		return false;
	case EVT_PRJSAVE:
		if( Plugin->OnProjectSave != NULL )
			return Plugin->OnProjectSave((char*) param );
		return false;
	case EVT_PAKCHANGE:
		if( Plugin->OnPAKChange != NULL )
			return Plugin->OnPAKChange((char*) param );
		return false;
	case EVT_RENAME:
		if( Plugin->OnRename != NULL )
			return Plugin->OnRename((char*) param, (char*)p2 );
		return false;
	case EVT_DELETE:
		if( Plugin->OnDelete != NULL )
			return Plugin->OnDelete((char*) param );
		return false;
	case EVT_LCLICK:
		if( Plugin->OnClick != NULL )
			return Plugin->OnClick((char*) param );
		return false;
	case EVT_RCLICK:
		if( Plugin->OnRClick != NULL )
			return Plugin->OnRClick((char*) param );
		return false;
	case EVT_DCLICK:
		if( Plugin->OnDblClick != NULL )
			return Plugin->OnDblClick((char*) param );
		return false;
	case EVT_SAVE:
		if( Plugin->OnSave != NULL )
			return Plugin->OnSave((char*) param, (char*)p2 );
		return false;
	case EVT_LOAD:
		if( Plugin->OnLoad != NULL )
			return Plugin->OnLoad((char*) param );
		return false;
	case EVT_DROP:
		if( Plugin->OnDrop != NULL )
			return Plugin->OnDrop((char*) param, (char*)p2 );
		return false;
	}
	return false;
}

bool
CPlugin::RunPlugin( DString Nom, DString Function, void *Param )
{
	return Act->RunPlugin( (void *)Nom.c_str(), (void *)Function.c_str(), Param );
}

void
CPlugin::LogText( DString Txt )
{
	Act->LogTxt( Txt.c_str() );
}

void
CPlugin::RunPAKBrowser( DString PakName )
{
	Act->RunPAKBrowser( PakName.c_str() );
}

DString
CPlugin::CnvIDToFile( DWORD ID )
{
	char buf[1024];
	if( ! Act->CnvIDToFileName( &ID, buf ) )
	{
		strcpy("<UNKOWN:",buf);
		_ltoa( ID, buf+strlen(buf),10 );
		strcat(buf,">");
	}
	return buf;
}

bool
CPlugin::InputText( DString Title, DString Default, DString *out )
{
	char buf[1024];
	bool r = Act->InputText( buf, Title.c_str(), Default.c_str() );
	*out = buf;
	return r;
}

bool
CPlugin::CreateItem( DString ItemName )
{
	return Act->CreateItem( this, ItemName.c_str() );
}

bool
CPlugin::LoadFileQuery( DString FileMask, DString Title, DString *out )
{
	return Act->LoadFile( FileMask.c_str(), Title.c_str(), out->c_str() );
}
	
bool
CPlugin::SaveFileQuery( DString FileMask, DString Title, DString *out )
{
	return Act->LoadFile( FileMask.c_str(), Title.c_str(), out->c_str() );
}
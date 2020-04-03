/* ************************************************************************ */
/*																			*/
/*	M3D-SE Initialisation													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "m3d.h"
#include "m3d_context.h"
#include "m3d_api.h"
#include "m3d_sdk.h"
#include "m3d_cache.h"
#include "m3d_res.h"
#include "m3d_2d.h"
#include <crtdbg.h> // for VC++ debug messages

namespace M3D {
/* ------------------------------------------------------------------------ */

#define DEFWIDTH	1280
#define DEFHEIGHT	1024

extern void InitLib( Context *c );
extern void CloseLib( Context *c );

extern void InitCore( Context *c );
extern void CloseCore( Context *c );

extern void InitScript( Context *c );
extern void CloseScript( Context *c );

/* ------------------------------------------------------------------------ */

int no_config( const char * ) {
	return 0;
}

void SetDebugOutput( Context *c, void (*callback)(const char *) ) {
	if( c != NULL )
		c->debug_out = callback;
}

/* ------------------------------------------------------------------------ */

bool CreateDevice( Context *c, D3DDEVTYPE dev, DWORD dxflags ) {
	D3DPRESENT_PARAMETERS present; 
	D3DDISPLAYMODE mode;
	bool fullscreen = (c->get_config("FULLSCREEN") == 1);
	bool use_antialias = (c->get_config("ANTIALIAS") == 1);

	if( fullscreen ) {
		c->width = DEFWIDTH;
		c->height = DEFHEIGHT;
	}

	c->software = (dev != D3DDEVTYPE_HAL);
	c->soft_tnl = (dxflags == D3DCREATE_SOFTWARE_VERTEXPROCESSING); 
	if( c->soft_tnl )
		c->use_hwshaders = false;

	// get current display mode
	if( M3DERROR(c->object->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&mode),"GetAdapterDisplayMode") )
		return false;
	
	// check for antialiasing support
	c->aa_enabled = use_antialias && (c->object->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL , mode.Format, !fullscreen, D3DMULTISAMPLE_2_SAMPLES ) == D3D_OK);

	// setup present parameters
	ZeroMemory( &present, sizeof(present) );
	c->buffer_format = mode.Format;
	present.Windowed = !fullscreen;
	present.BackBufferWidth = c->width;
	present.BackBufferHeight = c->height;
	present.BackBufferFormat = mode.Format;
	present.BackBufferCount = 1;
	present.SwapEffect = (c->get_config("VSYNC") == 1)?D3DSWAPEFFECT_COPY_VSYNC:D3DSWAPEFFECT_DISCARD;
	present.EnableAutoDepthStencil = TRUE;
	present.AutoDepthStencilFormat = D3DFMT_D16;
	present.MultiSampleType = c->aa_enabled?D3DMULTISAMPLE_2_SAMPLES:D3DMULTISAMPLE_NONE;
	present.FullScreen_RefreshRateInHz = fullscreen?D3DPRESENT_RATE_DEFAULT:0;
	present.FullScreen_PresentationInterval = fullscreen?D3DPRESENT_INTERVAL_IMMEDIATE:0;
	present.hDeviceWindow = c->render_target;

	return (c->lasterror = c->object->CreateDevice(D3DADAPTER_DEFAULT,dev,c->render_target,dxflags | D3DCREATE_FPU_PRESERVE,&present,&c->device)) == D3D_OK;
}

/* ------------------------------------------------------------------------ */

Context *Init( InitData *idata ) {
	Context *c = (Context*)alloc_context(sizeof(Context));	
	c->debug_out = idata->debug_out;
	c->get_config = idata->get_config;	
	c->width = idata->width;
	c->height = idata->height;
	c->bgcolor = idata->bgcolor;
	c->render_target = idata->target;	
	c->time = time_init();	

	Cache::Init(c);
	InitCore(c);
	InitLib(c);
	InitScript(c);
	Resource::Init(c);
	Resource::AddRoot(c,idata->root);
	Resource::InitTextures(c);

	// Create the Main DirectX Object
	c->object = Direct3DCreate8( D3D_SDK_VERSION );
	if( c->object == NULL ) {
		M3DDEBUG("Cannot create DirectX Object");
		Close(c);
		return NULL;
	}

	// Check object capabilities for hardware shader support
	D3DCAPS8 caps;	
	c->object->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	c->use_hwshaders = (c->get_config("HWSHADERS") == 1) && (D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion)>=1);

	// Create the better D3D Device
	if( (!c->use_hwshaders || !CreateDevice(c,D3DDEVTYPE_HAL,D3DCREATE_MIXED_VERTEXPROCESSING)) &&
		(!c->use_hwshaders || !CreateDevice(c,D3DDEVTYPE_HAL,D3DCREATE_HARDWARE_VERTEXPROCESSING)) &&
		!CreateDevice(c,D3DDEVTYPE_HAL,D3DCREATE_SOFTWARE_VERTEXPROCESSING) &&
		!CreateDevice(c,D3DDEVTYPE_REF,D3DCREATE_SOFTWARE_VERTEXPROCESSING) ) {
		M3DERROR(c->lasterror,"Failed to Create Device");
		Close(c);
		return NULL;
	}

	c->device->SetRenderState( D3DRS_SHADEMODE,			D3DSHADE_GOURAUD );
	c->device->SetRenderState( D3DRS_FILLMODE,			D3DFILL_SOLID );
	c->device->SetRenderState( D3DRS_DITHERENABLE,		TRUE );
	c->device->SetRenderState( D3DRS_SPECULARENABLE,	FALSE );
	c->device->SetRenderState( D3DRS_ZENABLE,			TRUE );
	c->device->SetRenderState( D3DRS_LIGHTING,			FALSE );
	c->device->SetRenderState( D3DRS_CLIPPING,			TRUE );
	c->device->SetRenderState( D3DRS_LOCALVIEWER,		FALSE );
	c->device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	c->device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	c->device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	c->device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	if( !c->use_hwshaders )
		c->device->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING,TRUE);

	if( !Init2D(c) ) {
		Close(c);
		return NULL;
	}

	MTW *mtw_font = new MTW(MAKETAG(Font));
	mtw_font->childs->Add( new MTW(MAKETAG(FACE),8,strdup("Times New Roman")) );
	mtw_font->childs->Add( new MTW(MAKETAG(flag),0,NULL) );
	mtw_font->childs->Add( new MTW(MAKETAG(size),11) );

	c->display_group = new Group(c,0.0001);
	c->display = new Text(c->display_group);
	c->display->font = M3D::Resource::LoadFont(c,mtw_font);
	c->display->y = 20;
	c->display->x = 5;
	c->display->color = 0;
	c->display->alpha = 0.99f;
	c->display->ColorAlphaModified();
	c->display->PosRotScaleModified();
	delete mtw_font;

	c->show_fps = (c->get_config("FPS") == 1);
	return c;
}

/* ------------------------------------------------------------------------ */

void Render( Context *c ) {
/*	HWND hactive = GetForegroundWindow();
	if( !hactive || GetWindowThreadProcessId(hactive,NULL) != GetWindowThreadProcessId(render_target,NULL) ) {
		Sleep(10);
		return;
	}*/
	if( c == NULL )
		return;	

	if( c->time )
		time_update(c->time);

	if( c->device == NULL )
		return;

	int nfaces = 0;
	HRESULT h = c->device->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,c->bgcolor,1.0f,0);
	h = c->device->BeginScene();
	c->device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
	c->device->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL);
	c->device->SetRenderState( D3DRS_ALPHAREF, 0);
	nfaces += Mesh::RenderAll(c,false);
	nfaces += Render2D(c,false);
	c->device->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	c->device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	c->device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	nfaces += Mesh::RenderAll(c,true);
	nfaces += Render2D(c,true);
	c->device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	c->device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

	RECT pos = { 5,5,0,0 };
	WString to_print = c->disp_text;
	if( c->show_fps )
		to_print = wprintf("FPS = %.2f\nNFACES = %d\n\n",c->time->fps,nfaces)+to_print;
	c->display->text = to_print;
	h = c->device->EndScene();	
	h = c->device->Present(NULL,NULL,c->render_target,NULL);
	bool lost = (h == D3DERR_DEVICELOST);
	if( lost ) {
		// *TODO* Handle Lost State ( reset all allocated buffers and textures )
		M3DDEBUG("DEVICE LOST");
		c->device = NULL;
	}
}

/* ------------------------------------------------------------------------ */

void Close( Context *c ) {
	if( c == NULL )
		return;
	if( c->device ) {
		c->device->SetVertexShader(0);
		c->device->SetTexture(0,NULL);
		c->device->SetStreamSource(0,NULL,0);
		c->device->SetStreamSource(1,NULL,0);
		c->device->SetStreamSource(2,NULL,0);
	}

	if( c->display )
		delete c->display;
	if( c->display_group )
		delete c->display_group;
	c->disp_text = "";
	if( c->_2d )
		Close2D(c);
	if( c->lib )
		CloseLib(c);
	if( c->script )
		CloseScript(c);
	if( c->core )
		CloseCore(c);
	if( c->res )
		Resource::Close(c);
	if( c->cache )
		Cache::Close(c);
	if( c->textures )
		Resource::CloseTextures(c);
	if( c->device )
		c->device->Release();
	if( c->object )
		c->object->Release();
	if( c->time )
		time_close(c->time);

	free_context();
}

/* ------------------------------------------------------------------------ */

void ShareContext( Context *c ) {
	set_context(c);
}

/* ------------------------------------------------------------------------ */

void Display( Context *c, WString msg ) {
	c->disp_text += msg + "\n";
}

/* ------------------------------------------------------------------------ */

void ClearDisplay( Context *c ) {
	if( c != NULL )
		c->disp_text = "";
}

/* ------------------------------------------------------------------------ */

void DebugMessage( Context *c, WString explain, const char *file, int line ) {
	WString msg = (file?((WString)(strrchr(file,'\\')+1)+"("+itos(line)+") : "):"Unknown : ")+explain;
#ifdef _DEBUG
	_RPT0(_CRT_WARN,(msg+"\n").c_str());
#endif
	if( c->debug_out != NULL )
		c->debug_out(msg.c_str());
	else
		MessageBox(c->render_target,msg.c_str(),"M3D Debug",MB_OK | MB_ICONERROR );
}

/* ------------------------------------------------------------------------ */

namespace Cache {

	typedef CacheItem<void>* ITEM;

	int cache_compare( void *a, ITEM b ) {
		return (int)b->data-(int)a;
	}

	int cache_garbage_one( ITEM item, void * ) {
		if( item->refcount <= 0 )
			return 0;
		return -1;
	}

	int cache_garbage( ITEM item, void *list ) {
		if( item->refcount <= 0 )
			((WList<ITEM>*)list)->Add(item);
		return -1;
	}

	void DoGarbage( Context *c, ITEM item ) {
		if( !item->weakcount )
			CACHEDATA->Delete(item->data);
		else {
			CACHEDATA->Remove(item->data);
			delete item->data;
			item->data = NULL;
#ifdef _DEBUG
			WEAKCACHE->Add(item);
#endif
		}
	}

	bool Garbage( Context *c ) {
		// BETTER =>
		// Incremental Garbage by using an ManagedArray (WMArray)
		ITEM togarbage = CACHEDATA->Find(cache_garbage_one,NULL);
		if( !togarbage )
			return false;
		DoGarbage(c,togarbage);
		return true;
	}

	void GarbageAll( Context *c ) {
		bool rflag = false;
		WList<ITEM> togarbage;
		CACHEDATA->Find(cache_garbage,&togarbage);
		FOREACH(ITEM,togarbage);
			DoGarbage(c,item);
			rflag = true;
		ENDFOR;
		if( rflag )
			GarbageAll(c);
		//cachedata.Optimize()
	}

	void Init( Context *c ) {		
#		ifdef _DEBUG
		c->weak_cache = new WList<CacheItem<void>*>();
#		endif
		c->cache = new WBTree<void*,ITEM>(cache_compare);
	}

	void Close( Context *c ) {
#ifdef _DEBUG
		GarbageAll(c);
		if( CACHEDATA->Count() || WEAKCACHE->Length() )
			M3DDEBUG("Cache is not empty : "+itos(CACHEDATA->Count())+"("+itos(WEAKCACHE->Length())+") items remaining");
#endif
		CACHEDATA->Delete();
		delete CACHEDATA;
#ifdef _DEBUG
		WEAKCACHE->Delete();
		delete WEAKCACHE;
#endif
	}
};

/* ------------------------------------------------------------------------ */
}; // namespace M3D
/* ************************************************************************ */

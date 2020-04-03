/* **************************************************************************** */
/*																				*/
/*   M3D Initialisation & Main control											*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#include <m3d.h>
#include <crtdbg.h>
#include "m3d_res.h"

namespace M3D {
/* ---------------------------------------------------------------------------- */

#define INITFAILED()		{ Close(); return false; }
#define M3DDELETE(x)		if( x ) { delete x; x = NULL; }
//#define AA_ENABLED

/* ---------------------------------------------------------------------------- */

OBJECT object = NULL;
DEVICE device = NULL;
Text::TData logtext(0);

const MATRIX IDMATRIX(1,0,0,0,/**/0,1,0,0,/**/0,0,1,0,/**/0,0,0,1);
MMat transform(IDMATRIX);
MATRIX ttransform;
MMat matproj(IDMATRIX);
MMat world(IDMATRIX);
MVec4 dirlight(VECTOR4(0,-1,3,1));
MVec4 calclight(VECTOR4());

HRESULT lasterror = D3D_OK;
WString lastcmd = "";
WString logdata = "";
bool aa_enabled = false;
bool software = false;
bool soft_tnl = false;
bool fullscreen = false;
bool use_hwshaders = false;
bool vsync = false;
int width = 0 ,height = 0;

bool has_largetime = false;
FLOAT curtime = 0.0f;
FLOAT realtime = 0.0f;
FLOAT elapsed_time = 0.0f;
FLOAT frequency = 1000000.0f;
LARGE_INTEGER lasttime = { 0, 0 };
WORD fps = 0;

HWND target = NULL;
HWND target_wnd = NULL;
DWORD bgcolor = D3DCOLOR_XRGB(128,128,128);

bool show_infos = false;
bool do_shading = true;

extern int DoRender();
extern bool Shaders_Init();

/* ---------------------------------------------------------------------------- */

void Print( WString msg ) {
	logdata+="\n"+msg;	
}

/* ---------------------------------------------------------------------------- */
#if defined(EXPORTER) || !defined(M3D_EXPORTS)

void Log( WString msg ) {
	printf("%s\n",msg.c_str());
	_RPT0(_CRT_WARN,(msg+"\n").c_str());
}

#endif
/* ---------------------------------------------------------------------------- */

bool CheckResult( HRESULT h, const char *file, int line, WString msg ) {
	if( h == D3D_OK )
		return true;
	lasterror = h;
	lastcmd = (WString)file+"("+itos(line)+") : "+msg+" #"+itos(h-MAKE_D3DHRESULT(0));
	Log(lastcmd);
	return false;
}

/* ---------------------------------------------------------------------------- */

float SecTo3DTime( FLOAT t ) {
	return t;
}

/* ---------------------------------------------------------------------------- */

void InitTime() {
	has_largetime = (QueryPerformanceCounter(&lasttime) != 0);
	if( !has_largetime )
		lasttime.QuadPart = UInt32x32To64(GetTickCount(),1000);
	else {
		LARGE_INTEGER lfreq;
		QueryPerformanceFrequency(&lfreq);
		frequency = lfreq.LowPart;
	}
}

/* ---------------------------------------------------------------------------- */

void UpdateTime() {
	LARGE_INTEGER loctime;
	DWORD delta;
	if( has_largetime )
		QueryPerformanceCounter(&loctime);
	else
		loctime.QuadPart = UInt32x32To64(GetTickCount(),1000);
	if( loctime.LowPart > lasttime.LowPart ) 
		delta = loctime.LowPart-lasttime.LowPart;
	else
		delta = lasttime.LowPart-loctime.LowPart;
	elapsed_time = delta?delta/frequency:0.001/frequency;
	double dfps = (fps/100.0)*0.95+0.05/elapsed_time;	
	fps = (WORD)(dfps*100);	
	lasttime = loctime;
	realtime += elapsed_time;
	curtime = curtime * 0.95 + 0.05 * realtime;
}

/* ---------------------------------------------------------------------------- */

void
SetConfig( HWND hwnd, bool full ) {
	target = hwnd;
	target_wnd = hwnd;
	HWND parent;
	while( (parent = GetParent(target_wnd)) != NULL )
		target_wnd = parent;
	fullscreen = full;
}

/* ---------------------------------------------------------------------------- */

static bool
SetupPresent( D3DPRESENT_PARAMETERS *present ) {
	// get current display mode
	D3DDISPLAYMODE mode;
	if( M3DERROR(object->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&mode),"") )
		return false;

#ifdef AA_ENABLED
	aa_enabled = (object->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL , mode.Format, !fullscreen, D3DMULTISAMPLE_2_SAMPLES ) == D3D_OK);
#else
	aa_enabled = false;
#endif

	width = fullscreen?1024:(mode.Width>1280?1024:mode.Width);
	height = fullscreen?768:(mode.Height>1024?768:mode.Height);

	ZeroMemory( present, sizeof(*present) );
	present->Windowed   = !fullscreen;
	present->BackBufferWidth = width;
	present->BackBufferHeight = height;
	present->SwapEffect = D3DSWAPEFFECT_DISCARD;
	present->BackBufferFormat = mode.Format;
	present->EnableAutoDepthStencil = TRUE;
    present->AutoDepthStencilFormat = D3DFMT_D16;
	present->BackBufferCount = 1;
	present->MultiSampleType = aa_enabled?D3DMULTISAMPLE_2_SAMPLES:D3DMULTISAMPLE_NONE;
	return true;
}

/* ---------------------------------------------------------------------------- */

static bool
CreateDevice(D3DDEVTYPE dev,DWORD behavior) {
	software = (dev != D3DDEVTYPE_HAL);
	soft_tnl = (behavior == D3DCREATE_SOFTWARE_VERTEXPROCESSING); 

#ifdef USE_R_THREAD
	behavior |= D3DCREATE_MULTITHREADED;
#endif

	// setup options
	D3DPRESENT_PARAMETERS present; 
	if( !SetupPresent(&present) )
		return false;

	// create device
	return M3DCHECK(
		object->CreateDevice(
			D3DADAPTER_DEFAULT,
			dev,
			target,
			behavior,
			&present,
			&device));
}

/* ---------------------------------------------------------------------------- */

bool Init( bool isvsync )
{
	vsync = isvsync;
	Close();
	// create main D3D Object
	object = Direct3DCreate8( D3D_SDK_VERSION );
	if( object == NULL ) {
		M3DTRACE("Direct3DCreate8");
		INITFAILED();
	}

	D3DCAPS8 caps;	
	object->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	use_hwshaders = D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion)>=1;

	// create device
	if( (!use_hwshaders || !CreateDevice(D3DDEVTYPE_HAL,D3DCREATE_MIXED_VERTEXPROCESSING)) &&
		(!use_hwshaders || !CreateDevice(D3DDEVTYPE_HAL,D3DCREATE_HARDWARE_VERTEXPROCESSING)) &&
		!CreateDevice(D3DDEVTYPE_HAL,D3DCREATE_SOFTWARE_VERTEXPROCESSING) &&
		!CreateDevice(D3DDEVTYPE_REF,D3DCREATE_SOFTWARE_VERTEXPROCESSING) )
		INITFAILED();
	lastcmd = "";

	// init device RS
    device->SetRenderState( D3DRS_SHADEMODE,		D3DSHADE_GOURAUD );
    device->SetRenderState( D3DRS_FILLMODE,			D3DFILL_SOLID );
	device->SetRenderState( D3DRS_DITHERENABLE,		TRUE );
	device->SetRenderState( D3DRS_SPECULARENABLE,	FALSE );
	device->SetRenderState( D3DRS_ZENABLE,			TRUE );
	device->SetRenderState( D3DRS_LIGHTING,			FALSE );
	device->SetRenderState( D3DRS_CLIPPING,			TRUE );
	device->SetRenderState( D3DRS_LOCALVIEWER,		FALSE );

	device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
	device->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL);
	device->SetRenderState( D3DRS_ALPHAREF, 0);

	// init texture SS
	device->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);	
	device->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	device->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
    device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	// init projection & transformation
	Camera::Reset();	
	D3DXMatrixPerspectiveFovLH(matproj.Get(),
                           D3DX_PI / 4,
                           4.0f / 3.0f, 1.0f, 100000.0f);
	matproj.Invalidate();
	transform.Invalidate();	

	// init light
	dirlight.Set(VECTOR4(0,-1,3,1));
	vnormalize((VECTOR*)dirlight.Get(),(VECTOR*)dirlight.Get());

	// init others things...
	if( !Shaders_Init() )
		INITFAILED();	
	if( !Sprite::Init() )
		INITFAILED();
	if( !use_hwshaders )
		device->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING,TRUE);
	ResCache::Init();
	Events::Init();

	logtext = Text::Create( Text::CreateFont(19,"Courier",TRUE) );
	logtext.SetColor(0xFFFFFFFF);
	logtext.SetPos(&VECTOR(-0.99f,-0.95f,0.000001f));

	InitTime();

	return true;
}

/* ---------------------------------------------------------------------------- */

bool Render() {	
	if( !device )
		return false;

	WINDOWPLACEMENT pl;	
	GetWindowPlacement(target,&pl);
	if( pl.showCmd == SW_SHOWMINIMIZED || pl.showCmd == SW_HIDE	) {
		Sleep(100);
		return false;
	}

	GetWindowPlacement(target_wnd,&pl);
	if( pl.showCmd == SW_SHOWMINIMIZED || pl.showCmd == SW_HIDE	) {
		Sleep(100);
		return false;
	}

	HWND hactive = GetForegroundWindow();
	if( hactive && hactive != target_wnd && hactive != target ) {
		char buf[256];
		GetWindowText(hactive,buf,256);
		Sleep(100);
		return false;
	}
		

	UpdateTime();

	device->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,bgcolor,1.0f,0);
	device->BeginScene();

	long faces = DoRender();

	// FPS display
	char buf[100];
	if( fps >= 10000 )
		fps = 9999;
#ifdef _DEBUG
	char debug = 'D';
#else
	char debug = 'R';
#endif

	sprintf(buf,"%.1f #%d %c%c%s - %dK\n",(fps/10)/10.0,faces,software?'S':'H',debug,aa_enabled?"AA":"",(device->GetAvailableTextureMem()>>10));
	logtext.SetString((buf+logdata).c_str());
	logdata = "";

	device->EndScene();	
	bool lost = device->Present(NULL,NULL,target,NULL) == D3DERR_DEVICELOST;
	//****** TODO : HANDLE LOST STATE *********

	Res<void*>::Garbage();
	if( !fullscreen && vsync )
		Sleep(15);

	return true;
}


/* ---------------------------------------------------------------------------- */

void Close()
{
	if( device ) {
		device->SetTexture(0,NULL);
		device->SetStreamSource(0,NULL,0);
		device->SetStreamSource(1,NULL,0);
		device->SetStreamSource(2,NULL,0);
	}
	FX::Close();
	Sprite::Close();
	Text::Close();
	Events::Close();
	ResCache::Close();
	Shader::Clean();
	Text::Close();
	M3DRELEASE(device);
	M3DRELEASE(object);	
}

/* ---------------------------------------------------------------------------- */

}; // namespace

/* ---------------------------------------------------------------------------- */

/* **************************************************************************** */
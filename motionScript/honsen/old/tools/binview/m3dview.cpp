#include "m3dview.h"
#include "../../common/mtw.h"
#define SCRIPT_IMPORTS
#include "../../m3dse/script/script.h"
#include "animview.h"
#include "../../hsrun/hsthread.h"
#include "../../common/wlist.h"

struct Context;
extern Context *last_context;
extern void ShareContext( Context *c );

enum {MTW_MODE, ATW_MODE, HS_MODE};

M3dView::M3dView()
:m3dThread(0), lockNeeded(false), mode(HS_MODE),  
mtw(NULL), root(NULL), color(0xffffff), fps(40)
{
	av = new AnimView();
	lastWrite.dwHighDateTime = 0;
	lastWrite.dwLowDateTime = 0;
	honsen_global_init();
	firstMesh = alloc_root(1);	
	*firstMesh = NULLVAL;
}

M3dView::~M3dView() {
	free_root(firstMesh);
	close();
	honsen_global_free();
}

void
M3dView::setView(HWND d3dView)
{
	this->d3dView = d3dView;
}

void
M3dView::setAnimView(HWND animView)
{
	av->setView(animView);
}

void RenderLock( void *view ) 
{
	((M3dView*)view)->doRenderLock();
}

void
M3dView::open(const WString path) 
{
	if( mtw ){
		lock();
		av->open(path);
		unlock();
	}
	else
		av->open(path);

	WString tempFilePath = path;

	int lastPoint = 0, temp = 0;
	while((temp = tempFilePath.find(".", temp + 1)) != -1)
		lastPoint = temp;
	WString extension = tempFilePath.substr(lastPoint);

	if( extension.equals(".atw") ){
		if( mtw ){
			otype t = t_mesh;
			lock();
			av->playAnim(*firstMesh);
			unlock();
		}
		return;
	}
	
	close(false);

	mode = HS_MODE;
	MTW *tempMTW = MTW::ReadFile(tempFilePath);

	if( !(extension.equals(".mtw") && buildFromMtw(tempMTW, tempFilePath))
		&& !(extension.equals(".hs") && (root = tempMTW)))
		return;

	WIN32_FILE_ATTRIBUTE_DATA wfad;
	GetFileAttributesEx(tempFilePath, GetFileExInfoStandard, &wfad);
	lastWrite = wfad.ftLastWriteTime;
	filePath = tempFilePath;

	MTW *head = root->Child(MAKETAG(Head));
	if( !head )
		return;
	MTW *colorMtw = head->Child(MAKETAG(bgcl));
	colorMtw->data = (void *)color;
	MTW *fpsMtw = head->Child(MAKETAG(nfps));
	fpsMtw->data = (void *)fps;

	// starting virtual machine and locking during script initialisation
	MTW *cpRoot = new MTW(*root);
	Params *params = new Params;
	params->hs = cpRoot;
	params->callback = RenderLock;
	params->cbparam = this;
	params->filename = filePath;
	params->hwnd = d3dView;
	m3dThread = honsen_start_thread(params);
	
	if( mode == MTW_MODE) {
		lock();
		ShareContext( last_context );
		initFromMtw();
		av->playAnim(*firstMesh);
		unlock();
	}	
}

void
M3dView::close(bool avClose)
{
	if( avClose )
		av->close();
	if( m3dThread ){
		honsen_stop_thread(m3dThread); //delete le root
		m3dThread = NULL;
	}
	if( mtw ) {
		delete mtw;
		mtw = NULL;
	}

	RedrawWindow(d3dView, NULL, NULL, RDW_INVALIDATE);
}

void
M3dView::doRenderLock() 
{
	if( lockNeeded ) {
		renderLock = CreateSemaphore(NULL,0,1,NULL);
		ReleaseSemaphore(viewLock,1,NULL);
		WaitForSingleObject(renderLock,INFINITE);
	}
}

void
M3dView::lock() 
{
	viewLock = CreateSemaphore(NULL,0,1,NULL);
	lockNeeded = true;
	WaitForSingleObject(viewLock,INFINITE);
}

void
M3dView::unlock() 
{
	lockNeeded = false;
	ReleaseSemaphore(renderLock,1,NULL);
}

void 
M3dView::initFromMtw() 
{
	WList<MTW*> objt = mtw->Childs(MAKETAG(Objt));
	int i = 0;
	FOREACH(MTW *, objt)
		value mesh = alloc_object(&t_mesh);
		value res = alloc_resource(item);
		val_ocall1(mesh, val_id("Mesh#1"), res);
		if( i++ == 0)
			*firstMesh = mesh;
	ENDFOR;
}

long filesize(FILE *stream)
{
	long curpos, length;

	curpos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

bool
M3dView::buildFromMtw(MTW *m, WString tempFilePath) 
{
	if(m)
		mtw = m;
	else
		return false;

	mode = MTW_MODE;
	
	char path[256];
	char *fname_pos;
	GetModuleFileName(NULL,path,246);
	fname_pos = strrchr(path,'\\');
	if( fname_pos )
		strcpy(fname_pos+1,"main.hs");
	else
		strcpy(path,"main.hs");

	MTW *mtemp = MTW::ReadFile(path);
	root = new MTW(*mtemp);
	delete mtemp;

	//adding textures used by object in MTW to the ressource available
	//texture file need to be in the MTW file directory
	MTW *tex, *pict;
	WList<MTW*> objtList = mtw->Childs(MAKETAG(Objt));
	FOREACH(MTW *, objtList)
		if(tex = item->Child(MAKETAG(TEX0))){
			root->AddChild(pict = new MTW(MAKETAG(Pict)));
			
			int lastSlash = 0, temp = 0;
			while((temp = tempFilePath.find("\\", temp + 1)) != -1)
				lastSlash = temp;
			WString path = tempFilePath.substr(0, lastSlash);
			path += "\\";
			path += (char *)(tex->data);
			
			pict->AddChild(new MTW(MAKETAG(RSNM), strlen((char *)(tex->data)) + 1, tex->data));
			FILE * textureFile;
			if ( !(textureFile = fopen( path, "rb")) )
				return false;
			
			long size = filesize(textureFile);
			char *buf = new char[size];
			long sr = fread(buf, 1, size, textureFile);
			if(sr != size)
				return false;
			MTW *mtemp = new MTW(MAKETAG(DATA), sr, buf);
			pict->AddChild(new MTW(*mtemp));
			delete(mtemp);
		}
	ENDFOR;
	return true;
}

void 
M3dView::emit(unsigned int message, WPARAM wParam, WPARAM lParam) 
{
	switch(message){
		case WM_KEYUP:
		case WM_KEYDOWN:
			av->emit(message, wParam, lParam);
			break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			SetCapture(d3dView);
			break;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			ReleaseCapture();
			break;
	}
	if(m3dThread)
		PostThreadMessage( m3dThread, message, wParam, lParam );
}

void 
M3dView::animEmit(unsigned int message, WPARAM wParam, WPARAM lParam) 
{
	av->emit(message, wParam, lParam);
}

void 
M3dView::getCameraPos() 
{
	if( !mtw )
		return;
	value v = CALL_PRIM(m3dse_camera);
	cameraPos = val_ocall0(v,val_id("get_pos"));
	cameraTarget = val_ocall0(v,val_id("get_target"));
	world = val_ocall0(v,val_id("get_world"));
}
	
void 
M3dView::setCameraPos() 
{
	if( !mtw )
		return;
	value v = CALL_PRIM(m3dse_camera);
	val_ocall1(v,val_id("set_pos"),cameraPos);
	val_ocall1(v,val_id("set_target"),cameraTarget);
	val_ocall1(v,val_id("set_world"),world);
}

void
M3dView::reload() 
{
	if( mtw ){
		lock();
		av->reload();
		unlock();
	}
	else
		av->reload();

	if(lastWrite.dwHighDateTime == 0 && lastWrite.dwLowDateTime == 0)
		return;

	WIN32_FILE_ATTRIBUTE_DATA wfad;
	GetFileAttributesEx(filePath, GetFileExInfoStandard, &wfad);
	if(lastWrite.dwHighDateTime == wfad.ftLastWriteTime.dwHighDateTime 
		&& lastWrite.dwLowDateTime == wfad.ftLastWriteTime.dwLowDateTime)
		return;

	MTW *temp;
	double curFrame;
	switch(mode) {
	case MTW_MODE:
		if(m3dThread){
			getCameraPos();
			curFrame = av->getAnimFrame();
		}
		open(filePath);
		if(m3dThread){
			setCameraPos();	
			temp = av->getCurrentAnim();
			if( temp )
				av->playAnim(NULLVAL, temp);
			av->setAnimFrame(curFrame);
		}
		break;
	case HS_MODE:
		open(filePath);
		break;
	}
}

void
M3dView::setMesh(MTW *mtw) 
{
	lock();
	if( mode == MTW_MODE) {
		if( *firstMesh != NULLVAL ){
			val_ocall0(*firstMesh, val_id("remove"));
			*firstMesh = NULLVAL;
		}
		value mesh = alloc_object(&t_mesh);
		value res = alloc_resource(new MTW(*mtw));
		val_ocall1(mesh, val_id("Mesh#1"), res);
		*firstMesh = mesh;
		av->playAnim(*firstMesh);
	}
	unlock();
}

void 
M3dView::drawIcons() 
{
	PAINTSTRUCT ps;
	HDC animDC;
	animDC = BeginPaint(av->animView, &ps);
	av->drawIcons(animDC);
	EndPaint(av->animView, &ps);
}

void 
M3dView::modSpeed(double s) 
{
	av->modSpeed(s);
}

void
M3dView::changeColor(int c) 
{
	if( !( root && m3dThread) )
		return;

	MTW *head = root->Child(MAKETAG(Head));
	if( !head )
		return;
	MTW *colorMtw = head->Child(MAKETAG(bgcl));
	colorMtw->data = (void *)c;
	color = c;
	resetRoot();
}

void
M3dView::changeFps(int f)
{
	if( !( root && m3dThread) )
		return;

	MTW *head = root->Child(MAKETAG(Head));
	if( !head )
		return;
	MTW *fpsMtw = head->Child(MAKETAG(nfps));
	fpsMtw->data = (void *)f;
	fps = f;
	resetRoot();
}

void
M3dView::resetRoot()
{
	if( !( root && m3dThread) )
		return;

	getCameraPos();
	double curFrame = av->getAnimFrame();
	honsen_stop_thread(m3dThread);
		
	MTW *cpRoot = new MTW(*root);
	Params *params = new Params;
	params->hs = cpRoot;
	params->callback = RenderLock;
	params->cbparam = this;
	params->filename = filePath;
	params->hwnd = d3dView;
	m3dThread = honsen_start_thread(params);
	lock();
	if( mode == MTW_MODE) {
		ShareContext( last_context );
		initFromMtw();
		av->playAnim(*firstMesh);
		MTW *temp = av->getCurrentAnim();
		if( temp )
			av->playAnim(NULLVAL, temp);
		av->setAnimFrame(curFrame);
	}
	setCameraPos();
	unlock();
}

void 
M3dView::removeAnim()
{
	av->removeAnim();
}
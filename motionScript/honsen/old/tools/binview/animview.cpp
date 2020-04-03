#include "animview.h"
#include "../../hsrun/hsthread.h"
#include "../../common/wlist.h"

#define GET_X_LPARAM(lParam) ((int)(short)LOWORD(lParam))
#define GET_Y_LPARAM(lParam) ((int)(short)HIWORD(lParam))

AnimIcon::AnimIcon(MTW *anim)
:anim(anim)
{
	name = (char *)(anim->Child(MAKETAG(NAME))->data);
	hi= LoadIcon(NULL, IDI_APPLICATION);
}

AnimIcon::~AnimIcon()
{
	delete anim;
}

//***************************************************************

AnimView::AnimView(): animView(animView), mtw(NULL),
currentAnim(NULL), speed(1), running(true)
{	
	lastWrite.dwHighDateTime = 0;
	lastWrite.dwLowDateTime = 0;
	firstAnim = alloc_root(1);	
	*firstAnim = NULLVAL;
	firstMesh = alloc_root(1);
	*firstMesh = NULLVAL;
}

AnimView::~AnimView() 
{
	close();
	free_root(firstAnim);
}

void
AnimView::setView(HWND animView)
{
	this->animView = animView;
}

void
AnimView::open(const WString path) 
{
	WString tempFilePath = path;
	
	int lastPoint = 0, temp = 0;
	while((temp = tempFilePath.find(".", temp + 1)) != -1)
		lastPoint = temp;
	WString extension = tempFilePath.substr(lastPoint);
	if( extension.equals(".mtw") )
		return; //on garde l'anim deja presente pour l'utiliser avec le mtw
	
	//close();

	if( !extension.equals(".atw") || !(mtw = MTW::ReadFile(tempFilePath)) )
		return;
	
	WIN32_FILE_ATTRIBUTE_DATA wfad;
	GetFileAttributesEx(tempFilePath, GetFileExInfoStandard, &wfad);
	lastWrite = wfad.ftLastWriteTime;
	filePath = tempFilePath;
	
	WList<MTW*> objtList = mtw->Childs(MAKETAG(Anim));
	FOREACH(MTW *, objtList)
		animIcons.Add(new AnimIcon(item));
	ENDFOR;
	RedrawWindow(animView, NULL, NULL, RDW_INVALIDATE);
}

void
AnimView::close() 
{
	animIcons.Delete();
	currentAnim = NULL;
	removeAnim();
	*firstAnim = NULLVAL;
	mtw = NULL;
	RedrawWindow(animView, NULL, NULL, RDW_INVALIDATE);
}

BOOL CALLBACK
deleteChildWindow(HWND childWindow, LPARAM lParam)
{
	return DestroyWindow(childWindow);
}

void
AnimView::drawIcons(HDC animDC) 
{
	EnumChildWindows(animView, deleteChildWindow, NULL);
	int left = 0;
	FOREACH(AnimIcon *, animIcons)
		DrawIcon(animDC, left + 10, 0, item->hi);
		MTW *name = item->anim->Child(MAKETAG(NAME));
		CreateWindow("Static", (char *)name->data, 
                                    WS_CHILD|WS_VISIBLE|SS_CENTER, 
									left, 30, 50, 20,
									animView, 0, 
									NULL, NULL);
		left += 50;
	ENDFOR;
}

void 
AnimView::playAnim(value mesh, MTW *anim)
{
	if( mtw ) {
		if( mesh != NULLVAL ){ 
			*firstMesh = mesh;
			value anim = alloc_object(&t_anim);
			*firstAnim = val_ocall1(anim, val_id("Anim#1"), mesh);
		}
		else{
			value res = alloc_resource(new MTW(*anim));
			val_ocall1(*firstAnim, val_id("set_anim"), res);
			currentAnim = anim;
			modSpeed(speed);
			toggleRunning(false);
		}
	}
}

void
AnimView::reload()
{
	if(lastWrite.dwHighDateTime == 0 && lastWrite.dwLowDateTime == 0)
		return;

	WIN32_FILE_ATTRIBUTE_DATA wfad;
	GetFileAttributesEx(filePath, GetFileExInfoStandard, &wfad);
	if(lastWrite.dwHighDateTime == wfad.ftLastWriteTime.dwHighDateTime 
		&& lastWrite.dwLowDateTime == wfad.ftLastWriteTime.dwLowDateTime)
		return;

	open(filePath);
}

void 
AnimView::emit(unsigned int message, WPARAM wParam, LPARAM lParam)
{
	if( !mtw || *firstAnim == NULLVAL )
		return;

	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
	switch( message ) {
		case WM_LBUTTONDOWN:
			if( yPos > 0 && yPos < 48 ){
				xPos /= 50;
				if( !animIcons.Empty() ) {
					AnimIcon *icon;
					if( icon = animIcons.AtIndex(xPos) ){
						speed = 1;
						running = true;
						playAnim(NULLVAL, icon->anim);
					}
				}
			}
			break;
		case WM_KEYDOWN:
			switch( wParam ) {
			case VK_F1:
				toggleRunning();
				break;
			case VK_F2:
				modSpeed(speed = speed + 0.1);
				break;
			case VK_F3:
				modSpeed(speed = speed - 0.1);
				break;
			}
			break;
	}
}

void
AnimView::modSpeed(double s)
{
	speed = s;
	if( *firstAnim != NULLVAL ){
		val_ocall1(*firstAnim, val_id("set_speed"), alloc_float(speed));
	}
}

MTW *
AnimView::getCurrentAnim()
{
	return currentAnim;
}

void
AnimView::toggleRunning(bool toggle)
{
	if( !mtw || *firstAnim == NULLVAL )
		return;
	if( toggle )
		running = !running;
	if( running )
		val_ocall0(*firstAnim, val_id("play"));
	else
		val_ocall0(*firstAnim, val_id("stop"));
}

double
AnimView::getAnimFrame()
{
	if( !mtw || *firstAnim == NULLVAL )
		return 0;
	return val_float(val_ocall0(*firstAnim, val_id("get_frame")));
}

void
AnimView::setAnimFrame(double f)
{
	if( !mtw || *firstAnim == NULLVAL )
		return;
	val_ocall1(*firstAnim, val_id("set_frame"), alloc_float(f));
}

void
AnimView::removeAnim()
{
	if( *firstAnim != NULLVAL ){
		val_ocall0(*firstAnim, val_id("remove"));
		*firstAnim = NULLVAL;
		currentAnim = NULL;
		speed = 1;
		running = true;
		playAnim(*firstMesh);
	}
}
#pragma once
#include "../../common/mtw.h"
#include <wtypes.h>
#define SCRIPT_IMPORTS
#include "../../m3dse/script/script.h"

class AnimIcon {
public:
	HICON hi;
	WString name;
	MTW *anim;

	AnimIcon(MTW *anim);
	~AnimIcon();
};

class AnimView {
public:
	HWND animView;

	AnimView();
	~AnimView();
	void open(const WString path);
	void close();
	void drawIcons(HDC animDC);
	void playAnim(value mesh, MTW *anim = NULL);
	void reload();
	void emit(unsigned int message, WPARAM wParam, LPARAM lParam);
	void setView(HWND animView);
	void modSpeed(double s);
	MTW *getCurrentAnim();
	void toggleRunning(bool toggle = true);
	double getAnimFrame();
	void setAnimFrame(double f);
	void removeAnim();

private:
	MTW *mtw;
	MTW *currentAnim;
	FILETIME lastWrite;
	WString filePath;
	WList<AnimIcon *> animIcons;
	value *firstAnim;
	value *firstMesh;
	double speed;
	bool running;
};
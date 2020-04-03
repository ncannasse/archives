#pragma once
#include <wtypes.h>
#include "../../common/wstring.h"
#include "../../common/mtw.h"

typedef struct _value* value;

class AnimView;

class M3dView {
public:
	HWND d3dView;

	M3dView();
	~M3dView();
	void open(const WString path);
	void close(bool avClose = true);
	void doRenderLock();
	void emit(unsigned int message, WPARAM wParam, WPARAM lParam);
	void reload();
	void setView(HWND d3dView);
	void setAnimView(HWND animView);
	void setMesh(MTW *mesh);
	void drawIcons();
	void animEmit(unsigned int message, WPARAM wParam, WPARAM lParam);
	void modSpeed(double s);
	void changeColor(int c);
	void changeFps(int f);
	void removeAnim();

private:
	AnimView *av;
	DWORD m3dThread;
	MTW *root;
	MTW *mtw;
	WString filePath;
	bool lockNeeded;
	HANDLE viewLock;
	HANDLE renderLock;
	int mode;
	value cameraPos;
	value cameraTarget;
	value world;
	value *firstMesh;
	FILETIME lastWrite;
	int color;
	int fps;

	bool buildFromMtw(MTW *mtw, WString tempFilePath);
	void initFromMtw();
	void lock();
	void unlock();
	void getCameraPos();
	void setCameraPos();
	void resetRoot();
};
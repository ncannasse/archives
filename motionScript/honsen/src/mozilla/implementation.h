/* ************************************************************************ */
/*																			*/
/*	Honsen Mozilla Plugin													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include "pluginbase.h"

class MTW;
typedef struct _hscontext hscontext;

class honsenPlugin : public nsPluginInstanceBase {
public:
	honsenPlugin(NPP aInstance, const char *url );
	~honsenPlugin();
	NPBool init(NPWindow* aWindow);
	NPBool isInitialized();
	NPError DestroyStream(NPStream *stream, NPError reason );
	int32   Write(NPStream *stream, int32 offset, int32 len, void *buffer);
	NPError SetWindow(NPWindow* pNPWindow);
	void CleanBuffer();
	void shut();
	void loop();

	HANDLE lock;
	HANDLE thread;
	HANDLE wait;
	HWND wnd;
	MTW *mtw;
	hscontext *ctx;
	int lasttime;

private:
	DWORD tid;
	NPP inst;
	NPBool initflag;
	char *url;
	char *buf;
	int bufsize;
	int bufpos;
};

/* ************************************************************************ */

#pragma once

typedef struct HWND__ *HWND;
class MTW;

struct Params {
	MTW *hs;
	const char *filename;
	HWND hwnd;
	void (*callback)( void * );
	void *cbparam;
};

bool honsen_global_init();
void honsen_global_free();
DWORD honsen_start_thread( Params *params );
void honsen_stop_thread(DWORD threadID);

/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <windows.h>
typedef int bool;
#include "timer.h"

/* ------------------------------------------------------------------------ */

timer *timer_init() {
	timer *t = malloc(sizeof(timer));
	LARGE_INTEGER *lt = (LARGE_INTEGER*)&t->lasttime;
	t->has_largetime = (QueryPerformanceCounter(lt) != 0);
	if( !t->has_largetime ) {
		t->frequency = 1000000.0;
		lt->QuadPart = UInt32x32To64(GetTickCount(),1000);
	} else {
		LARGE_INTEGER lfreq;
		QueryPerformanceFrequency(&lfreq);
		t->frequency = lfreq.LowPart;
	}
	t->curtime = 0;
	t->realtime = 0;
	t->elapsed_time = 0;
	t->fps = 0;
	return t;
}

/* ------------------------------------------------------------------------ */

void timer_close( timer *t ) {
	free(t);
}

/* ------------------------------------------------------------------------ */

void timer_update( timer *t, double prec ) {
	LARGE_INTEGER *lt = (LARGE_INTEGER*)&t->lasttime;
	LARGE_INTEGER loctime;
	DWORD delta;
	if( t->has_largetime )
		QueryPerformanceCounter(&loctime);
	else
		loctime.QuadPart = UInt32x32To64(GetTickCount(),1000);
	delta = loctime.LowPart - lt->LowPart;
	t->elapsed_time = delta?( delta / t->frequency ):( 0.0001 / t->frequency );	
	t->realtime += t->elapsed_time;
	t->fps = t->fps * prec + (1 - prec) / t->elapsed_time;
	t->curtime = t->curtime * prec + (1 - prec) * t->realtime;
	*lt = loctime;
}

/* ------------------------------------------------------------------------ */

void timer_sleep( timer *t, int ms ) {
	if( ms > 0 )
		Sleep(ms);
}

/* ************************************************************************ */

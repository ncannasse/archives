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

struct _cpu_time {
	HANDLE p;
	FILETIME user;
	FILETIME kernel;
};

/* ------------------------------------------------------------------------ */

timer *time_init() {
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

void time_close( timer *t ) {
	free(t);
}

/* ------------------------------------------------------------------------ */

void time_update( timer *t ) {
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
	t->fps = t->fps * 0.95 + 0.05 / t->elapsed_time;
	if( t->realtime - t->curtime > 1.0 )
		t->curtime = t->realtime;
	else
		t->curtime = t->curtime * 0.95 + 0.05 * t->realtime;
	*lt = loctime;
}

/* ------------------------------------------------------------------------ */

void time_sleep( timer *t, int ms ) {
	if( ms > 0 )
		Sleep(ms);
}

/* ------------------------------------------------------------------------ */

cpu_time time_cpu() {
	FILETIME dummy;
	cpu_time ct = malloc(sizeof(struct _cpu_time));
	ct->p = GetCurrentProcess();
	GetProcessTimes(ct->p,&dummy,&dummy,&ct->kernel,&ct->user);
	return ct;
}

/* ------------------------------------------------------------------------ */

double time_spent( cpu_time ct ) {
	FILETIME dummy;
	__int64 old_kernel = *(__int64*)&ct->kernel, old_user = *(__int64*)&ct->user;
	__int64 delta;
	GetProcessTimes(ct->p,&dummy,&dummy,&ct->kernel,&ct->user);
	delta = *((__int64*)&ct->kernel) - old_kernel + *((__int64*)&ct->user) - old_user;
	return delta / 10000000.0; // in seconds
}

/* ------------------------------------------------------------------------ */

void time_cpu_free( cpu_time ct ) {
	free(ct);
}

/* ************************************************************************ */

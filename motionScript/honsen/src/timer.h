/* ************************************************************************ */
/*																			*/
/*	Honsen Timer															*/
/*	Nicolas Cannasse														*/
/*	(c)2004 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

typedef struct timer {
	bool has_largetime;
	double curtime;
	double realtime;
	double elapsed_time;
	double frequency;
	double fps;
	unsigned long lasttime;
	unsigned long __dummy;	
} timer;

#ifdef __cplusplus
extern "C" {
#endif

timer *timer_init();
void timer_close( timer *t);
void timer_update( timer *t, double prec );
void timer_sleep( timer *t, int millisec );

#ifdef __cplusplus
};
#endif

/* ************************************************************************ */

/* ************************************************************************ */
/*																			*/
/*	Honsen Timer															*/
/*	Nicolas Cannasse														*/
/*	(c)2004 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */

typedef struct _cpu_time *cpu_time;

typedef struct {
	bool has_largetime;
	double curtime;
	double realtime;
	double elapsed_time;
	double frequency;
	double fps;
	unsigned long lasttime;
	unsigned long __lasttime;	
} timer;

#ifdef __cplusplus
extern "C" {
#endif

timer *time_init();
void time_close( timer *t);
void time_update( timer *t );
void time_sleep( timer *t, int millisec );

cpu_time time_cpu();
double time_spent( cpu_time ct );
void time_cpu_free( cpu_time ct );

#ifdef __cplusplus
};
#endif

/* ************************************************************************ */

/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

typedef struct _window window;
typedef struct _controls controls;

void window_init();
window *window_create(const char *title, int width, int height);
window *window_create_log();
void window_log_text( window *w, const char *data );
void window_log_set( window *w, const char *data );
void window_set_title( window *w, const char *title );
bool window_loop();
void window_destroy( window *w );
void window_error( const char *msg );
void window_set_controls( window *w, controls *c );

/* ************************************************************************ */

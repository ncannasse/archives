/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

typedef struct _m3d m3d;
typedef struct _window window;
typedef struct _resources resources;
typedef struct _vect vect;

m3d *m3d_init( window *w, int width, int height, unsigned int bgcolor );
void m3d_set_resources( m3d *m, resources *res );
void m3d_render_enable( m3d *m, bool flag );
void m3d_nextframe( m3d *m );
void m3d_render( m3d *m );
void m3d_destroy( m3d *m );
void m3d_clear( m3d *m );

void m3d_add_vect( m3d *m, vect *v );
void m3d_remove_vect( m3d *m, vect *v );

/* ************************************************************************ */

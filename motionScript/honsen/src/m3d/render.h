/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#include "maths3d.h"

typedef struct _window window;
typedef struct _render render;
typedef struct _rbuffer rbuffer;
typedef struct _texture texture;

#define BLEND_ZERO			0
#define BLEND_ONE			1
#define BLEND_COLOR			2
#define BLEND_INVCOLOR		3
#define BLEND_SRCALPHA		4
#define BLEND_INVSRCALPHA	5
#define BLEND_DSTALPHA		6
#define BLEND_INVDSTALPHA	7

#define BLEND_MAX			7

typedef struct {
	rbuffer *buffer;
	matrix *matrix;
	color *color;
	texture *texture;
	bool light;
	int srcblend;
	int dstblend;
} rprops;

render *render_init( window *w );
void render_destroy( render *r );

void render_start( render *r, int fillcolor );
void render_flip( render *r );
void render_set_transform( render *r, matrix *m );
void render_set_light( render *r, vector *pos, unsigned int ambient, unsigned int diffuse, float power );
void render_buffer( render *r, rprops *p );
void render_set_alpha( render *r, bool alpha );
void render_enable( render *r, bool enable );

texture *render_texture_alloc( render *r, int twidth, int theight );
void render_texture_size( texture *t, int *width, int *height );
void render_texture_props( texture *t, float *px, float *py, float *sx, float *sy );
void render_texture_update( texture *t, void *buf, int stride );
void render_texture_lock( texture *t, void **lock, int *stride );
void render_texture_unlock( texture *t );
void render_texture_free( texture *t );

/* ************************************************************************ */

/* ************************************************************************ */
/*																			*/
/*	Honsen Vector Rendering													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

class MTW;
struct FillData;
typedef struct _vect vect;
typedef struct _vect_cache vect_cache;

vect_cache *vect_cache_init( vect_cache *c, MTW *root );
void vect_cache_free( vect_cache *c );

vect *vect_init( vect_cache *c, int width, int height );
void vect_get_size( vect *v, int *width, int *height );
void vect_nextframe( vect *v );
bool vect_render( vect *v, unsigned int bgcolor );
void *vect_buffer( vect *v );
void vect_destroy( vect *v );

void vect_vpath_resize( vect *v );
void vect_render_shape( vect *v, FillData *fill );

/* ************************************************************************ */

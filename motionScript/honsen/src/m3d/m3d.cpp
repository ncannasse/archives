/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <string.h>
#include "m3d.h"
#include "mesh.h"
#include "m3d_priv.h"
#include "rbuffer.h"
#include "../vect/vect.h"
/* ------------------------------------------------------------------------ */

_context *m3d_context = context_new();
extern void m3d_free_camera( m3d *m );
extern void m3d_free_light( m3d *m );

extern cached_texture *TextureAlloc( texture *t );
extern texture *TextureGet( cached_texture *t );

/* ------------------------------------------------------------------------ */

m3d *m3d_init( window *wnd, int width, int height, unsigned int bgcolor ) {
	m3d *m = new m3d;
	m->vcache = NULL;
	m->width = width;
	m->height = height;
	m->camera = NULL;
	m->light = NULL;
	m->bgcolor = bgcolor;
	m->window = wnd;
	m->render = render_init(wnd);
	m->resources = NULL;
	context_set_data(m3d_context,m);
	return m;
}

/* ------------------------------------------------------------------------ */

void m3d_render( m3d *m ) {
	if( m->camera != NULL ) {
		camera *c = *m->camera;
		if( c->modified ) {
			matrix mcam;
			vector dist;
			vector_sub(&dist,&c->target,&c->pos);
			vector_scale(&dist,&dist,(float)(1.0/c->zoom));
			vector_sub(&c->realpos,&c->target,&dist);
			vector_normalize(&c->worldup,&c->worldup);

			vector n;
			vector_cross(&n,&dist,&c->worldup);
			vector_cross(&c->realup,&n,&dist);
			vector_normalize(&c->realup,&c->realup);

			matrix3d_lookat(&mcam,&c->realpos,&c->target,&c->realup);
			matrix3d_multiply(&mcam,&mcam,&c->proj);
			matrix3d_multiply(&m->cur_camera,&c->world,&mcam);


			c->modified = false;
		}
	} else
		matrix3d_identity(&m->cur_camera);
	render_start(m->render,m->bgcolor);
	render_set_transform(m->render,&m->cur_camera);
	m->transform = true;
	if( m->light != NULL ) {
		light *l = *m->light;
		render_set_light(m->render,&l->pos,l->ambient,l->diffuse,l->ray);
	}
	memset(&m->stats,0,sizeof(m->stats));
	render_set_alpha(m->render,false);
	FOREACH(Vect*,m->vects);
		if( vect_render(item->v,0) )
			render_texture_update(TextureGet(item->t),vect_buffer(item->v),item->width*4);
	ENDFOR;
	FOREACH(Mesh*,m->meshes);
		item->Render(m,true,false);
	ENDFOR;
	render_set_alpha(m->render,true);
	FOREACH(Mesh*,m->meshes);
		item->Render(m,true,true);
	ENDFOR;
	render_flip(m->render);
}

/* ------------------------------------------------------------------------ */

void m3d_destroy( m3d *m ) {
	m3d_free_camera(m);
	m3d_free_light(m);
	m->vects.Delete();
	m->meshes.Delete();
	vect_cache_free(m->vcache);
	render_destroy(m->render);
	delete m;
}

/* ------------------------------------------------------------------------ */

void m3d_set_resources( m3d *m, resources *res ) {
	m->resources = res;
}

/* ------------------------------------------------------------------------ */

void m3d_nextframe( m3d *m ) {
	FOREACH(Vect*,m->vects);
		vect_nextframe(item->v);
	ENDFOR;
}

/* ------------------------------------------------------------------------ */

void m3d_render_enable( m3d *m, bool e ) {
	render_enable(m->render,e);
}

/* ------------------------------------------------------------------------ */

void m3d_clear( m3d *m ) {
	m->meshes.Delete();
}

/* ------------------------------------------------------------------------ */

void m3d_add_vect( m3d *m, vect *v ) {
	Vect *vd = new Vect;
	vd->v = v;
	vect_get_size(v,&vd->width,&vd->height);
	texture *t = render_texture_alloc(m->render,vd->width,vd->height);
	if( t == NULL ) {
		delete vd;
		return;
	}
	vd->t = TextureAlloc(t);
	m->vects.Add(vd);
}

/* ------------------------------------------------------------------------ */

void m3d_remove_vect( m3d *m, vect *v ) {
	FOREACH(Vect*,m->vects);
		if( item->v == v ) {
			Texture::Delete(item->t);
			m->vects.Delete(item);
			break;
		}
	ENDFOR;
}

/* ************************************************************************ */

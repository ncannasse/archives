/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#include "m3d.h"
#include "render.h"
#include "maths3d.h"
#include "../../common/context.h"
#include "../../common/wlist.h"

#ifndef NULL
#define NULL 0
#endif

class Mesh;
struct cached_texture;
typedef struct _vect_cache vect_cache;

struct camera {
	matrix proj;
	matrix world;
	vector pos;
	vector realpos;
	vector target;
	vector worldup;
	vector realup;
	double zoom;
	double roll;
	bool modified;
};

struct light {
	vector pos;
	unsigned int diffuse;
	unsigned int ambient;
	float ray;
};

struct Vect {
	vect *v;
	cached_texture *t;
	int width;
	int height;
};

struct _m3d {
	int width;
	int height;
	camera **camera;
	light **light;
	matrix cur_camera;
	render *render;
	window *window;
	resources *resources;
	unsigned int bgcolor;
	bool transform;
	vect_cache *vcache;
	WList<Vect*> vects;
	WList<Mesh*> meshes;
	struct {
		int nvects;
		int nmeshes;
	} stats;
};

extern _context *m3d_context;

#define M3D()	((m3d*)(context_get_data(m3d_context)))

/* ************************************************************************ */

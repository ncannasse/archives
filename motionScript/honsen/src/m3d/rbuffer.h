/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#include "render.h"
#include "maths3d.h"

typedef struct {
	float tu;
	float tv;
} tcoords;

typedef enum {
	VNORMAL,
	VCOLOR,
	VTEXTURE
} RBFORMAT;

typedef struct {
	vector v;
	vector n;
} vertex_normal;

typedef struct {
	vector v;
	vector n;
	color c;
} vertex_color;

typedef struct {
	vector v;
	vector n;
	tcoords t;
} vertex_texture;

typedef struct _rbuffer {
	RBFORMAT format;
	int maxverts;
	int nverts;
	union {
		char *buf;
		vertex_normal *vbuf;
		vertex_color *cbuf;
		vertex_texture *tbuf;
	};
	bool have_normals;
} rbuffer;

rbuffer *rbuffer_alloc( int nverts, RBFORMAT format );
void rbuffer_free( rbuffer *r );
void rbuffer_resize( rbuffer *r, int size );
void rbuffer_calc_normals( rbuffer *r );

/* ************************************************************************ */

/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <string.h>
#include <math.h>
#include "rbuffer.h"

static int format_bytes( RBFORMAT format ) {
	switch( format ) {
	case VNORMAL: return sizeof(vertex_normal);
	case VCOLOR: return sizeof(vertex_color);
	case VTEXTURE: return sizeof(vertex_texture);
	default:
		return 0;
	}
}

rbuffer *rbuffer_alloc( int nverts, RBFORMAT format ) {
	rbuffer *b = new rbuffer;
	b->format = format;
	b->nverts = 0;
	b->maxverts = nverts;
	b->buf = new char[format_bytes(format)*nverts];
	b->have_normals = false;
	return b;
}

void rbuffer_free( rbuffer *b ) {
	delete b->buf;
	delete b;
}

void rbuffer_resize( rbuffer *b, int size ) {
	int bytes = format_bytes(b->format);
	char *nbuf = new char[bytes*size];
	memcpy(nbuf,b->buf, ((size < b->nverts)?size:b->nverts) * bytes );
	delete b->buf;
	b->buf = nbuf;
	b->maxverts = size;
	if( size < b->nverts ) {
		b->nverts = size;
		b->have_normals = false;
	}
}

void rbuffer_calc_normals( rbuffer *b ) {
	int stride = format_bytes(b->format);
	vertex_normal *v1 = b->vbuf, *v2, *v3;
	int i = b->nverts / 3;
	vector d1, d2, n;
	b->have_normals = true;
	while( i-- ) {
		v2 = (vertex_normal*)((char*)v1 + stride);
		v3 = (vertex_normal*)((char*)v2 + stride);
		vector_sub(&d1,&v2->v,&v1->v);
		vector_sub(&d2,&v3->v,&v2->v);
		vector_cross(&n,&d1,&d2);
		if( fabs(n.x) < 1e-5f )
			n.x = 0;
		if( fabs(n.y) < 1e-5f )
			n.y = 0;
		if( fabs(n.z) < 1e-5f )
			n.z = 0;
		vector_normalize(&n,&n);
		v1->n = n;
		v2->n = n;
		v3->n = n;
		v1 = (vertex_normal*)((char*)v3 + stride);	
	}
}

/* ************************************************************************ */

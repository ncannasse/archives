/* ************************************************************************ */
/*																			*/
/*	Honsen Vector Rendering													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#include "vect.h"
#include "clip.h"
#include "../../common/wmem.h"
#include "../../common/warray.h"

typedef struct _VPath VPath;
typedef struct _rasterizer rasterizer;
typedef struct _mtv_cache mtv_cache;


struct _vect {
	unsigned int *buffer;
	int width;
	int height;
	vect_cache *vcache;
	WArray<Clip*> clips;
	VPath *vpath;
	int vpath_size;
	rasterizer *raster;
	bool modified;
	struct {
		int nclips;
		int nshapes;
		int nforms;
		int nsegs;
	} stats;
};

/* ************************************************************************ */

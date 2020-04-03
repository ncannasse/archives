/* ************************************************************************ */
/*																			*/
/*	Honsen Vector Rendering													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include <string.h>
#include <malloc.h>
#include "../../common/wlist.h"
#include "../../common/warray.h"

#define MAXID	0x10000
#define GRADIENT_BITS	11

typedef struct _BPath BPath;

/* ------------------------------------------------------------------------ */

enum FillStyle {
	RGB,
	LINEAR_GRADIENT,
	RADIAL_GRADIENT,
	BITMAP
};

struct FillData {
	unsigned int color;
	float width;
	matrix mat;
	unsigned int *colors;
	FillStyle style;
	int bwidth, bheight;
	~FillData() {
		delete colors;
	}
};

struct FormData {
	FillData *fill;
	int nsegs;
	BPath *path;
	~FormData() {
		free(path);
	}
};

struct ShapeData {
	WList< FormData* > forms;
	~ShapeData() {
		forms.Delete();
	}
};

/* ------------------------------------------------------------------------ */

struct FrameOp;

struct ClipData {
	WArray< WList<FrameOp*>* > frames;
	~ClipData() {
		AFOREACH(WList<FrameOp*>*,frames);
			item->Delete();
		ENDFOR;
		frames.Delete();
	}
};

/* ------------------------------------------------------------------------ */

enum FOP {
	SETMATRIX,
	REMOVE,
	PLACESHAPE,
	PLACECLIP,
	SETCOLOR
};

struct FrameOp {
	FOP op;
	int depth;
	union {
		matrix mat;
		colortrans color;
		ShapeData *shape;
		ClipData *clip;
		void *data;
	};

	FrameOp( FOP op, int depth, void *data ) : op(op), depth(depth), data(data) {
		if( op == SETMATRIX )
			memcpy(&mat,data,sizeof(matrix));
		else if( op == SETCOLOR )
			memcpy(&color,data,sizeof(colortrans));
	}
};

/* ************************************************************************ */

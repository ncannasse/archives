/* ************************************************************************ */
/*																			*/
/*	Honsen Vector Rendering													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <math.h>
#include "clip.h"
#include "clippriv.h"
#include "vectpriv.h"
#include "vecttrans.h"
/* ------------------------------------------------------------------------ */

void color_identity( colortrans c ) {
	c[0] = 1;
	c[2] = 1;
	c[4] = 1;
	c[6] = 1;
	c[1] = 0;
	c[3] = 0;
	c[5] = 0;
	c[7] = 0;
}

void color_multiply( colortrans out, colortrans a, colortrans b ) {
	out[1] = a[1] * b[0] + b[1];
	out[0] = a[0] * b[0];

	out[3] = a[3] * b[2] + b[3];
	out[2] = a[2] * b[2];

	out[5] = a[5] * b[4] + b[5];
	out[4] = a[4] * b[4];

	out[7] = a[7] * b[6] + b[7];
	out[6] = a[6] * b[6];
}

float max( float a, float b ) {
	return (a > b)?a:b;
}

float min( float a, float b ) {
	return (a < b)?a:b;
}

unsigned int color_transform( unsigned int c, colortrans t ) {
	if( t[0] == 1 && t[1] == 0 && t[2] == 1 && t[3] == 0 && t[4] == 1 && t[5] == 0 && t[6] == 1 && t[7] == 0 )
		return c;

	float r = ((c >> 16) & 0xFF) / 256.0f;
	float g = ((c >> 8) & 0xFF) / 256.0f;
	float b = (c & 0xFF) / 256.0f;
	float a = (c >> 24) / 256.0f;
	
	r = max(0, min(r * t[0] + t[1],255));
	g = max(0, min(g * t[2] + t[3],255));
	b = max(0, min(b * t[4] + t[5],255));
	a = max(0, min(a * t[6] + t[7],255));

	return ((int)(r * 256)) << 16 | ((int)(g * 256)) << 8 | ((int)(b * 256)) | ((int)(a * 256)) << 24;
}

unsigned int *gradient_transform( unsigned int *g, colortrans t, bool *free ) {
	if( t[0] == 1 && t[1] == 0 && t[2] == 1 && t[3] == 0 && t[4] == 1 && t[5] == 0 && t[6] == 1 && t[7] == 0 ) {
		*free = false;
		return g;
	}
	int size = 1 << GRADIENT_BITS;
	unsigned int *g2 = new unsigned int[size];
	int i;
	for(i=0;i<size;) {
		unsigned int c = g[i];
		float r = ((c >> 16) & 0xFF) / 256.0f;
		float gg = ((c >> 8) & 0xFF) / 256.0f;
		float b = (c & 0xFF) / 256.0f;
		float a = (c >> 24) / 256.0f;
		
		r = max(0, min(r * t[0] + t[1],255));
		gg = max(0, min(gg * t[2] + t[3],255));
		b = max(0, min(b * t[4] + t[5],255));
		a = max(0, min(a * t[6] + t[7],255));

		g2[i] = ((int)(r * 256)) << 16 | ((int)(gg * 256)) << 8 | ((int)(b * 256)) | ((int)(a * 256)) << 24;

		i++;
		while( i < size && g[i] == c ) {
			g2[i] = g2[i-1];
			i++;
		}
	}
	*free = true;
	return g2;
}

/* ------------------------------------------------------------------------ */

Element::Element( Clip *p ) {
	parent = p;
	modified = false;
	color_identity(col);
	matrix_identity(mat);
}

Element::~Element() {
}

void Element::UpdateTransforms() {
	if( parent == NULL ) {
		matrix_copy(abs_mat,mat);
		memcpy(abs_col,col,sizeof(colortrans));
	} else {
		matrix_multiply(abs_mat,mat,parent->abs_mat);
		color_multiply(abs_col,col,parent->abs_col);
	}
}

double Clip::Rotation() {
	double pi = 3.14159265358979323846;
	rotation = fmod(rotation,2 * pi);
	if( rotation > pi )
		rotation -= pi * 2;
	else if( rotation < -pi )
		rotation += pi * 2;
	return rotation;
}

double Clip::XScale() {
	double rot = Rotation();
	if( fabs(sin(rot)) < 0.5 )
		return mat[0] / cos(rot);
	else
		return -mat[1] / sin(rot);
}

double Clip::YScale() {
	double rot = Rotation();
	if( fabs(sin(rot)) < 0.5 )
		return mat[3] / cos(rot);
	else
		return mat[2] / sin(rot);
}

/* ------------------------------------------------------------------------ */

Clip::Clip( Clip *p, ClipData *d ) : Element(p), data(d) {
	ref = NULL;
	prev_frame = -1;
	frame = -1;
	speed = 1;
	visible = true;
	rotation = 0;
	NextFrame();
}

/* ------------------------------------------------------------------------ */

void
Clip::Render( vect *v ) {
	if( !visible )
		return;
	UpdateTransforms();
	v->stats.nclips++;
	AFOREACH(Element*,depths);
		if( item )
			item->Render(v);
	ENDFOR;
}

/* ------------------------------------------------------------------------ */

bool
Clip::NextFrame() {
	bool mod = modified;

	frame += speed;
	frame = fmod(frame, TotalFrames());

	AFOREACH(Element*,depths);
		if( item != NULL && item->NextFrame() )			
			mod = true;
	ENDFOR;

	int iframe = (int)frame;
	if( iframe == prev_frame )
		return mod;

	prev_frame = iframe;
	WList<FrameOp*> *l = data?data->frames[iframe]:NULL;
	if( l == NULL )
		return mod;

	mod = true;
	Element *e;
	FOREACH(FrameOp*,*l);
		switch( item->op ) {
		case SETMATRIX:
			e = depths[item->depth];
			if( e != NULL )
				matrix_copy(e->mat,item->mat);
			break;
		case REMOVE:
			delete depths[item->depth];
			depths[item->depth] = NULL;
			break;
		case PLACESHAPE:
			delete depths[item->depth];
			depths[item->depth] = (Element*)new Shape(this,item->shape);
			break;
		case PLACECLIP:
			delete depths[item->depth];
			depths[item->depth] = (Clip*)new Clip(this,item->clip);
			break;
		case SETCOLOR:
			e = depths[item->depth];
			if( e != NULL )
				memcpy(e->col,item->color,sizeof(colortrans));
			break;
		}
	ENDFOR;
	return mod;
}

/* ------------------------------------------------------------------------ */

Clip::~Clip() {
	if( ref != NULL )
		RefRelease(ref);
	depths.Delete();
}

/* ------------------------------------------------------------------------ */

int Clip::TotalFrames() {
	return data?data->frames.Length():1;
}

/* ------------------------------------------------------------------------ */

Shape::Shape( Clip *p, ShapeData *d ) : Element(p), data(d) {
}

/* ------------------------------------------------------------------------ */

Shape::~Shape() {
}

/* ------------------------------------------------------------------------ */

void Shape::Render( vect *v ) {
	UpdateTransforms();
	v->stats.nshapes++;
	FOREACH(FormData*,data->forms);
		v->stats.nforms++;
		v->stats.nsegs += item->nsegs;
		BPath *bpath2 = bpath_transform(item->path,abs_mat,item->nsegs);
		while( !bpath_segment(v->vpath,v->vpath_size,bpath2,0.5f) )
			vect_vpath_resize(v);
		FillData fd;
		bool free_gcolors = true;
		fd.colors = NULL;
		fd.style = item->fill->style;
		fd.color = color_transform(item->fill->color,abs_col);
		if( item->fill->width > 0 ) {
			double rot = atan2(-mat[1],mat[0]);
			double sr = sin(rot);
			double xs = fabs((sr > 0.5)?(-mat[1] / sr):(mat[0] / cos(rot)));
			double ys = fabs((sr > 0.5)?(mat[2] / sr):(mat[3] / cos(rot)));
			fd.width = (float)(item->fill->width * (xs > ys)?xs:ys);
		}
		else 
			fd.width = 0;
		if( item->fill->colors != NULL ) {
			fd.bwidth = item->fill->bwidth;
			fd.bheight = item->fill->bheight;
			if( fd.style == BITMAP ) {
				free_gcolors = false;
				fd.colors = item->fill->colors;
			} else
				fd.colors = gradient_transform(item->fill->colors,abs_col,&free_gcolors);
			matrix_multiply(fd.mat,item->fill->mat,abs_mat);
		}
		vect_render_shape(v,&fd);
		if( !free_gcolors )
			fd.colors = NULL;
		free(bpath2);
	ENDFOR;
}

/* ************************************************************************ */

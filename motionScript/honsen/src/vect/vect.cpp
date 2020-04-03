/* ************************************************************************ */
/*																			*/
/*	Honsen Vector Rendering													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <string.h>
#include <math.h>
#include "vect.h"
#include "vectpriv.h"
#include "vecttrans.h"
#include "clippriv.h"
#include "../../libs/agg/agg.h"

/* ------------------------------------------------------------------------ */

struct span_gradient {

	static int *radial_tbl;

	int width;
	unsigned char *buf;
	unsigned int *gcolors;
	bool radial;
	matrix imat;
	int dx,dy,dx2,dy2,x0,y0;

	#define NBITS 14
	#define RADIAL_BITS 9

	static void init_radial_tbl() {
		int w = 1 << RADIAL_BITS;
		radial_tbl = new int[w*w];
		int x,y;
		for(x=0;x<w;x++) {
			for(y=0;y<w/2;y++) {
				int dx = x - w/2;
				int dy = y - w/2;
				int k = (int)(sqrt((double)(dx*dx+dy*dy)) * (1 << (GRADIENT_BITS - RADIAL_BITS + 1)));
				if( k >= (1 << GRADIENT_BITS) )
					k = (1 << GRADIENT_BITS) - 1;
				radial_tbl[x+(y<<RADIAL_BITS)] = k;
			}
			for(y=w/2;y<w;y++)
				radial_tbl[x+(y<<RADIAL_BITS)] = radial_tbl[x+((w-1-y)<<RADIAL_BITS)];
		}
	}

	void init( vect *v, FillData *fd ) {
		gcolors = fd->colors;
		buf = (unsigned char*)v->buffer;
		width = v->width;
		matrix_inverse(imat,fd->mat);
		int w = 1 << (15 - GRADIENT_BITS);
		double fdx = (imat[0] * 20.0) / w;
		double fdy = (imat[2] * 20.0) / w;
		double fx0 = (imat[4] * 20.0 + (1 << 14)) / w;
		dx = (int)(fdx * (1 << NBITS));
		dy = (int)(fdy * (1 << NBITS));
		x0 = (int)(fx0 * (1 << NBITS));
		radial = (fd->style == RADIAL_GRADIENT);

		if( radial ) {
			double fdx2 = (imat[1] * 20.0) / w;
			double fdy2 = (imat[3] * 20.0) / w;
			double fy0 = (imat[5] * 20.0 + (1 << 14)) / w;
			dx2 = (int)(fdx2 * (1 << NBITS));
			dy2 = (int)(fdy2 * (1 << NBITS));
			y0 = (int)(fy0 * (1 << NBITS));


			if( radial_tbl == NULL )
				init_radial_tbl();
		}
	}

	int scale( int a, int b, int c ) {
		return (a > b)?a:((c < b)?c:b);
	}

    void render(unsigned char* ptr, 
                        int x,
                        unsigned count, 
                        const unsigned char* covers, 
						const agg::rgba8& _c)
    {
		int y = (((int)(ptr - buf)) >> 2) / width;
		int kx = x * dx + y * dy + x0;
		int ky;
        unsigned char* p = ptr + (x << 2);
		if( radial ) {
			ky = x * dx2 + y * dy2 + y0;
			do {
				int indx = radial_tbl[
					scale(0,(kx >> (NBITS + GRADIENT_BITS - RADIAL_BITS)),(1 << RADIAL_BITS)-1) |
					(scale(0,(ky >> (NBITS + GRADIENT_BITS - RADIAL_BITS)),(1 << RADIAL_BITS)-1) << RADIAL_BITS)
				];
				agg::rgba8 c = ((agg::rgba8*)gcolors)[indx];
				int alpha = (*covers++) * c.a;
				int r = p[0];
				int g = p[1];
				int b = p[2];
				int a = p[3];

				*p++ = (((c.r - r) * alpha) + (r << 16)) >> 16;
				*p++ = (((c.g - g) * alpha) + (g << 16)) >> 16;
				*p++ = (((c.b - b) * alpha) + (b << 16)) >> 16;
				*p++ = (((c.a - a) * alpha) + (a << 16)) >> 16;
				
				kx += dx;
				ky += dx2;
			} while(--count);
			return;
		}
        do
        {
			agg::rgba8 c = ((agg::rgba8*)gcolors)[scale(0,(kx >> NBITS),(1 << GRADIENT_BITS)-1)];
            int alpha = (*covers++) * c.a;
            int r = p[0];
            int g = p[1];
            int b = p[2];
            int a = p[3];

            *p++ = (((c.r - r) * alpha) + (r << 16)) >> 16;
            *p++ = (((c.g - g) * alpha) + (g << 16)) >> 16;
            *p++ = (((c.b - b) * alpha) + (b << 16)) >> 16;
            *p++ = (((c.a - a) * alpha) + (a << 16)) >> 16;

			kx += dx;
        }
        while(--count);
    }
};

/* ------------------------------------------------------------------------ */

struct span_bitmap {

	unsigned char *buf;
	unsigned int *bcolors;
	int width;
	int bwidth;
	int bheight;
	matrix imat;
	int dx,dy,dx2,dy2,x0,y0;
	#define NBITS 14

	void init( vect *v, FillData *fd ) {
		bcolors = fd->colors;
		buf = (unsigned char*)v->buffer;
		width = v->width;
		bwidth = fd->bwidth;
		bheight = fd->bheight;
		matrix_inverse(imat,fd->mat);
		double w = 20.0;
		double h = 20.0;
		dx = (int)(imat[0] * w * (1 << NBITS));
		dy = (int)(imat[2] * h * (1 << NBITS));
		x0 = (int)(imat[4] * w * (1 << NBITS));
		dx2 = (int)(imat[1] * w * (1 << NBITS));
		dy2 = (int)(imat[3] * h * (1 << NBITS));
		y0 = (int)(imat[5] * h * (1 << NBITS));
	}

	int scale( int a, int b, int c ) {
		return (a > b)?a:b%c;
	}

    void render(unsigned char* ptr, 
                        int x,
                        unsigned count, 
                        const unsigned char* covers, 
						const agg::rgba8& _c)
    {
		int y = (((int)(ptr - buf)) >> 2) / width;
		int kx = x * dx + y * dy + x0;
		int ky = x * dx2 + y * dy2 + y0;
        unsigned char* p = ptr + (x << 2);
		do {
			int indx = scale(0,(kx >> NBITS),bwidth) + scale(0,ky >> NBITS,bheight) * bwidth;
			agg::rgba8 c = ((agg::rgba8*)bcolors)[indx];
			int alpha = (*covers++) * c.a;
			int r = p[0];
			int g = p[1];
			int b = p[2];
			int a = p[3];

			*p++ = (((c.r - r) * alpha) + (r << 16)) >> 16;
			*p++ = (((c.g - g) * alpha) + (g << 16)) >> 16;
			*p++ = (((c.b - b) * alpha) + (b << 16)) >> 16;
			*p++ = (((c.a - a) * alpha) + (a << 16)) >> 16;
			
			kx += dx;
			ky += dx2;
		} while(--count);
	}

};

/* ------------------------------------------------------------------------ */

struct _rasterizer {
	agg::rendering_buffer rbuf;
    agg::renderer<agg::span_rgba32> ren;
	agg::renderer<span_gradient> ren_grad;
	agg::renderer<span_bitmap> ren_bitmap;
    agg::rasterizer ras;
	_rasterizer( void *buffer, int w, int h ) : rbuf((unsigned char *)buffer,w,h,w * 4), ren(rbuf), ren_grad(rbuf), ren_bitmap(rbuf) {
		ras.filling_rule(agg::fill_even_odd);
		ras.gamma(1.3);
	}
};

/* ------------------------------------------------------------------------ */

int *span_gradient::radial_tbl = NULL;

/* ------------------------------------------------------------------------ */

vect *vect_init( vect_cache *c, int width, int height ) {
	vect *v = new _vect;
	v->width = width;
	v->height = height;
	v->vcache = c;
	v->buffer = new unsigned int[width * height];
	v->vpath = NULL;
	v->vpath_size = 0;
	v->raster = new _rasterizer(v->buffer,width,height);
	return v;
}

/* ------------------------------------------------------------------------ */

void vect_get_size( vect *v, int *width, int *height ) {
	*width = v->width;
	*height = v->height;
}

/* ------------------------------------------------------------------------ */

void vect_destroy( vect *v ) {
	v->clips.Delete();
	delete v->raster;
	delete v->vpath;
	delete v->buffer;
	delete v;
}

/* ------------------------------------------------------------------------ */

void vect_nextframe( vect *v ) {
	bool mod = false;
	AFOREACH(Clip*,v->clips);
		if( item && item->NextFrame() )
			mod = true;
	ENDFOR;
	v->modified = mod;
}

/* ------------------------------------------------------------------------ */

bool vect_render( vect *v, unsigned int bgcolor ) {
	int i;
	if( !v->modified )
		return false;
	for(i=0;i<v->width * v->height;i++)
		v->buffer[i] = bgcolor;
	memset(&v->stats,0,sizeof(v->stats));
	AFOREACH(Clip*,v->clips);
		if( item )
			item->Render(v);
	ENDFOR;
	return true;
}

/* ------------------------------------------------------------------------ */

void *vect_buffer( vect *v ) {
	return v->buffer;
}

/* ------------------------------------------------------------------------ */

void vect_vpath_resize( vect *v ) {
	int size = v->vpath_size?(v->vpath_size << 1):16;
	delete v->vpath;
	v->vpath = new VPath[size];
	v->vpath_size = size;
	v->vpath->code = PATH_END;
}

/* ------------------------------------------------------------------------ */

void vect_render_shape( vect *v, FillData *fill ) {
	VPath *vec = (VPath*)v->vpath;
	if( vec == NULL )
		return;
	float px = 0, py = 0, dx, dy, d;
	if( fill->width == 0 ) {
		while( true ) {
			if( vec->code == PATH_LINETO ) {
				dx = px - vec->x;
				dy = py - vec->y;
				d = sqrt(dx*dx+dy*dy);
				px = vec->x + dy * 0.5f / d;
				py = vec->y - dx * 0.5f / d;
				v->raster->ras.line_to_d(px,py);
			} else if( vec->code == PATH_MOVETO ) {
				px = vec->x;
				py = vec->y;
				v->raster->ras.move_to_d(px,py);
			} else
				break;
			vec++;
		}
	} else {
		float w = fill->width / 2;
		while( true ) {
			if( vec->code == PATH_LINETO ) {
				dx = vec->x - px;
				dy = vec->y - py;
				d = sqrt(dx*dx+dy*dy);
				px += dy * w / d;
				py += -dx * w / d;
				v->raster->ras.line_to_d(px,py);
				px += dx + w;
				py += dy;
				v->raster->ras.line_to_d(px,py);
				px += -dy * w * 2 / d;
				py += dx * w * 2 / d;
				v->raster->ras.line_to_d(px,py);
				px -= dx;
				py -= dy;
				v->raster->ras.line_to_d(px,py);
				px += dy * w / d;
				py += -dx * w / d;
				v->raster->ras.line_to_d(px,py);
				px += dx;
				py += dy;
				v->raster->ras.move_to_d(px,py);
			} else if( vec->code == PATH_MOVETO ) {
				px = vec->x;
				py = vec->y;
				v->raster->ras.move_to_d(px,py);
			} else
				break;
			vec++;
		}
	}

	if( fill->style != RGB ) {
		if( fill->style == BITMAP ) {
			v->raster->ren_bitmap.m_span.init(v,fill);
			v->raster->ras.render(v->raster->ren_bitmap,agg::rgba8(0xFFFFFF,agg::rgba8::rgb));
		} else {
			v->raster->ren_grad.m_span.init(v,fill);
			v->raster->ras.render(v->raster->ren_grad,agg::rgba8(0xFFFFFF,agg::rgba8::rgb));
		}
	} else {
		double alpha = fill->color >> 24;
		if( fill->width == 0 )
			alpha = ceil(sqrt(alpha/255.0f)*255.0f);
		if( alpha > 255 )
			alpha = 255;
		agg::rgba8 color = agg::rgba8(fill->color&0xFF,(fill->color >> 8)&0xFF,(fill->color>>16)&0xFF,(int)alpha);
		v->raster->ras.render(v->raster->ren,color);
	}
}

/* ************************************************************************ */

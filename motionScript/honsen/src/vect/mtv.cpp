/* ************************************************************************ */
/*																			*/
/*	Honsen Vector Rendering													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "clip.h"
#include "clippriv.h"
#include "vectpriv.h"
#include "vecttrans.h"
#include "../../common/wlist.h"
#include "../../common/wmap.h"
#include "../../common/mtw.h"
#include "../../common/bits.h"

/* ------------------------------------------------------------------------ */

struct _mtv_cache {
	WArray<ClipData*> dclips;
	WArray<ShapeData*> dshapes;
	WArray<FillData*> dfills;
	WArray<MTW*> mbitmaps;
	WArray<MTW*> mfills;
	WArray<MTW*> mshapes;
	WArray<MTW*> mclips;
	~_mtv_cache() {
 		dclips.Delete();
		dshapes.Delete();
		dfills.Delete();
	}
};

/* ------------------------------------------------------------------------ */

struct _vect_cache {
	WMap<MTW*,MTW*> parents;
	WArray<mtv_cache*> cache;
	~_vect_cache() {
		cache.Delete();
	}
};

/* ------------------------------------------------------------------------ */

#define SCALE 20.0f

#define CHECK(i)	if( (i) == -1 ) { delete cdata; return NULL; }
//#undef CHECK
//#define CHECK(i)	if( (i) == -1 ) { *(char*)NULL = 0; }

/* ------------------------------------------------------------------------ */

static int ReadMatrixPart( Bits *b, int *x, int *y ) {
	int nbits = b->Read(5);
	*x = b->Read(nbits);
	*y = b->Read(nbits);
	if( nbits == -1 || *x == -1 || *y == -1 )
		return -1;
	*x = Bits::Signed(*x,nbits);
	*y = Bits::Signed(*y,nbits);
	return 1;
}


/* ------------------------------------------------------------------------ */

#define Fixed(v)	((v >> 16) + ((v & 0xFFFF) / 65536.0f))

static int ReadMatrix( Bits *b, matrix m ) {
	int x,y;
	int has_scale = b->Read(1);
	if( has_scale == -1 )
		return -1;
	if( has_scale ) {
		if( ReadMatrixPart(b,&x,&y) == -1 )
			return -1;
		m[0] = Fixed(x);
		m[3] = Fixed(y);
	} else {
		m[0] = 1;
		m[3] = 1;
	}
	int has_rotate = b->Read(1);
	if( has_rotate == -1 )
		return -1;
	if( has_rotate ) {
		if( ReadMatrixPart(b,&x,&y) == -1 )
			return -1;
		m[1] = Fixed(x);
		m[2] = Fixed(y);
	} else {
		m[1] = 0;
		m[2] = 0;
	}
	if( ReadMatrixPart(b,&x,&y) == -1 )
		return -1;	
	m[4] = x / SCALE;
	m[5] = y / SCALE;
	return 1;
}

/* ------------------------------------------------------------------------ */

static int ReadCTrans( Bits *b, colortrans c ) {
	int has_add = b->Read(1);
	int has_mult = b->Read(1);
	int nbits = b->Read(4);
	if( has_add == -1 || has_mult == -1 || nbits == -1 )
		return -1;
	if( has_add ) {
		int r = b->Read(nbits);
		int g = b->Read(nbits);
		int bb = b->Read(nbits);
		int a = b->Read(nbits);
		if( r == -1 || g == -1 || bb == -1 || a == -1 )
			return -1;
		c[1] = r / 256.0f;
		c[3] = g / 256.0f;
		c[5] = bb / 256.0f;
		c[7] = a / 256.0f;
	} else {
		c[1] = 0;
		c[3] = 0;
		c[5] = 0;
		c[7] = 0;
	}
	if( has_mult ) {
		int r = b->Read(nbits);
		int g = b->Read(nbits);
		int bb = b->Read(nbits);
		int a = b->Read(nbits);
		if( r == -1 || g == -1 || bb == -1 || a == -1 )
			return -1;
		c[0] = r / 256.0f;
		c[2] = g / 256.0f;
		c[4] = bb / 256.0f;
		c[6] = a / 256.0f;
	} else {
		c[0] = 0;
		c[2] = 0;
		c[4] = 0;
		c[6] = 0;
	}
	return 1;
}

/* ------------------------------------------------------------------------ */

static mtv_cache *MtvCache( vect *v, MTW *t ) {
	if( t == NULL )
		return NULL;
	if( t->tag == MAKETAG(Mtvd) ) {
		MTW *id = t->Child(MAKETAG(mtvi));
		if( id == NULL || id->number > MAXID )
			return NULL;
		return v->vcache->cache[id->number];
	}
	return MtvCache(v,v->vcache->parents.Find(t));
}

/* ------------------------------------------------------------------------ */

static void vect_init_data( vect_cache *c, MTW *t, MTW *parent, int mtvid ) {
	if( parent != NULL )
		c->parents.Add(t,parent);
	mtv_cache *cache = c->cache[mtvid];
	if( cache == NULL ) {
		cache = new mtv_cache;
		c->cache[mtvid] = cache;
	}
	if( t->IsGroup() ) {
		if( t->tag == MAKETAG(Mtvd) ) {
			MTW *id = t->Child(MAKETAG(mtvi));
			if( id != NULL && id->number < MAXID )
				mtvid = id->number;
			else
				return;
		}
		FOREACH(MTW*,*t->childs);
			vect_init_data(c,item,t,mtvid);
		ENDFOR;
	} else if( t->tag == MAKETAG(clid) ) {
		if( t->number < MAXID )
			cache->mclips[t->number] = parent;
	} else if( t->tag == MAKETAG(shid) ) {
		if( t->number < MAXID )
			cache->mshapes[t->number] = parent;
	} else if( t->tag == MAKETAG(fsid) ) {
		if( t->number < MAXID )
			cache->mfills[t->number] = parent;
	} else if( t->tag == MAKETAG(bmid) ) {
		if( t->number < MAXID )
			cache->mbitmaps[t->number] = parent;
	}
}

/* ------------------------------------------------------------------------ */

vect_cache *vect_cache_init( vect_cache *c, MTW *root ) {
	if( c == NULL )
		c = new vect_cache;
	if( c->parents.Exists(root) )
		return c;
	c->parents.Add(root,NULL);
	vect_init_data(c,root,NULL,0);
	c->parents.Optimize();
	return c;
}

/* ------------------------------------------------------------------------ */

void vect_cache_free( vect_cache *c ) {
	delete c;
}

/* ------------------------------------------------------------------------ */

static bool MakeBitmap( vect *v, FillData *fd, MTW *fill ) {	
	MTW *bid = fill->Child(MAKETAG(ibmp));
	if( bid == NULL )
		return false;
	mtv_cache *cache = MtvCache(v,fill);
	if( cache == NULL )
		return false;
	MTW *bmp = cache->mbitmaps[bid->number];
	if( bmp == NULL || bmp->tag != MAKETAG(Bimp) )
		return false;
	MTW *width = bmp->Child(MAKETAG(wdth));
	MTW *height = bmp->Child(MAKETAG(hght));
	MTW *data = bmp->Child(MAKETAG(DATA));
	MTW *pal = bmp->Child(MAKETAG(PALT));
	bool rgba = bmp->HasChild(MAKETAG(rGBA));
	if( width == NULL || width->number > 0x1000 || height == NULL || height->number > 0x1000 || data == NULL )
		return false;
	if( pal != NULL && pal->size != 4 * 256 )
		return false;
	int p;
	int size = width->number * height->number;
	if( (pal != NULL && data->size != size) || (pal == NULL && data->size != size * (rgba?4:3)) )
		return false;
	unsigned int *buf = new unsigned int[size];
	fd->bheight = height->number;
	fd->bwidth = width->number;
	fd->colors = buf;
	for(p=0;p<size;p++)
		if( pal ) 
			buf[p] = ((unsigned int*)pal->data)[((unsigned char*)data->data)[p]];
		else if( rgba )
			buf[p] = ((unsigned int*)data->data)[p];
		else {
			unsigned char *d = (unsigned char*)data->data + p*3;
			buf[p] = d[0] | (d[1] << 8) | (d[2] << 16) | 0xFF000000;
		}

	return true;
}

/* ------------------------------------------------------------------------ */

static unsigned int *MakeGradient( MTW *m ) {
	MTW *cols = m->Child(MAKETAG(Cols));
	MTW *vals = m->Child(MAKETAG(Vals));
	if( cols == NULL || vals == NULL )
		return NULL;
	WList<MTW*> c = cols->Childs(MAKETAG(colr));
	WList<MTW*> v = vals->Childs(MAKETAG(cval));
	if( c.Length() != v.Length() || c.Length() < 2 )
		return NULL;
	int size = 1 << GRADIENT_BITS;
	unsigned int *tbl = new unsigned int[size];
	int p = 0;
	int i;
	unsigned int col = c.First()->number;
	tbl[0] = col;
	while( !c.Empty() ) {
		int p2 = (v.Pop()->number * (size-1)) / 255;
		unsigned int c2 = c.Pop()->number;
		if( p2 < p ) {
			delete tbl;
			return NULL;
		}
		int cr = (col >> 16) & 0xFF;
		int cg = (col >> 8) & 0xFF;
		int cb = col & 0xFF;
		int ca = col >> 24;
		for(i=p+1;i<=p2;i++) {
			double f = (i - p) * 1.0 / (p2 - p);
			double r = cr + f * ((int)((c2 >> 16) & 0xFF) - cr);
			double g = cg + f * ((int)((c2 >> 8) & 0xFF) - cg);
			double b = cb + f * ((int)(c2 & 0xFF) - cb);
			double a = ca + f * ((int)(c2 >> 24) - ca);
			if( r < 0 ) r = 0; else if ( r > 255 ) r = 255;
			if( g < 0 ) g = 0; else if ( g > 255 ) g = 255;
			if( b < 0 ) b = 0; else if ( b > 255 ) b = 255;
			if( a < 0 ) a = 0; else if ( a > 255 ) a = 255;
			tbl[i] = (((int)r) << 16) | (((int)g) << 8) | ((int)b) | (((int)a) << 24);
		}
		p = p2;
		col = c2;
	}
	while( p < size - 1 )
		tbl[p++] = col;
	return tbl;
}

/* ------------------------------------------------------------------------ */

static FillData *MakeFillData( vect *v, MTW *m ) {
	if( m->tag != MAKETAG(Fill) )
		return NULL;
	MTW *id = m->Child(MAKETAG(fsid));
	if( id == NULL || id->number > MAXID )
		return NULL;
	mtv_cache *cache = MtvCache(v,m);
	if( cache == NULL )
		return NULL;
	FillData *fdata = cache->dfills[id->number];
	if( fdata != NULL )
		return fdata;
	MTW *fcolor = m->Child(MAKETAG(colr));
	MTW *mat = m->Child(MAKETAG(LMAT)); 
	bool radial = false;
	bool bitmap = false;
	if( mat == NULL ) {
		mat = m->Child(MAKETAG(RMAT));
		if( mat == NULL ) {
			mat = m->Child(MAKETAG(BMAT));
			bitmap = true;
		} else
			radial = true;
	}
	if( fcolor == NULL && mat == NULL )
		return NULL;
	MTW *fwidth = m->Child(MAKETAG(wdth));
	fdata = new FillData;
	fdata->colors = NULL;
	fdata->color = fcolor?fcolor->number:0;
	fdata->width = fwidth?(fwidth->number / SCALE):0;
	fdata->style = (mat == NULL)?RGB:(radial?RADIAL_GRADIENT:(bitmap?BITMAP:LINEAR_GRADIENT));
	if( mat != NULL ) {
		Bits b(mat->data,mat->size);
		if( ReadMatrix(&b,fdata->mat) == -1 ) {
			delete fdata;
			return NULL;
		}
		if( bitmap ) {
			if( !MakeBitmap(v,fdata,m) ) {
				delete fdata;
				return NULL;
			}
		} else {
			fdata->colors = MakeGradient(m);
			if( fdata->colors == NULL ) {
				delete fdata;
				return NULL;
			}
		}
	}
	cache->dfills[id->number] = fdata;
	return fdata;
}

/* ------------------------------------------------------------------------ */

ShapeData *Shape::MakeData( vect *v, MTW *m ) {
	if( m->tag != MAKETAG(Shap) )
		return NULL;
	MTW *id = m->Child(MAKETAG(shid));
	if( id == NULL || id->number > MAXID )
		return NULL;
	mtv_cache *cache = MtvCache(v,m);
	if( cache == NULL )
		return NULL;
	ShapeData *cdata = cache->dshapes[id->number];
	if( cdata != NULL )
		return cdata;
	cdata = new ShapeData;
	WList<MTW*> forms = m->Childs(MAKETAG(Form));
	FOREACH(MTW*,forms);
		MTW *fsnb = item->Child(MAKETAG(fsnb));
		MTW *d = item->Child(MAKETAG(DATA));
		if( fsnb != NULL )
			fsnb = cache->mfills[fsnb->number];
		if( fsnb == NULL || d == NULL ) {
			delete cdata;
			return NULL;
		}
		FillData *fsd = MakeFillData(v,fsnb);
		if( fsd == NULL ) {
			delete cdata;
			return NULL;
		}
		Bits b(d->data,d->size);
		int nsegs = b.Read(16);
		int nbits = b.Read(5);
		CHECK(nsegs);
		CHECK(nbits);

		FormData *fd = new FormData;
		fd->fill = fsd;
		fd->nsegs = nsegs;
		fd->path = (BPath*)malloc(sizeof(BPath)*(nsegs+1));
		cdata->forms.Add(fd);

		int s;
		int px = 0, py = 0;
		for(s=0;s<nsegs;s++) {
			BPath *p = &fd->path[s];
			int f = b.Read(1);
			CHECK(f);
			if( f == 1 ) {
				int cx = b.Read(nbits);
				int cy = b.Read(nbits);
				int x = b.Read(nbits);
				int y = b.Read(nbits);
				CHECK(x);
				CHECK(y);
				CHECK(cx);
				CHECK(cy);
				x = Bits::Signed(x,nbits);
				y = Bits::Signed(y,nbits);
				cx = Bits::Signed(cx,nbits);
				cy = Bits::Signed(cy,nbits);
				p->code = PATH_CURVETO;
				px += cx;
				py += cy;
				p->cx = px / SCALE;
				p->cy = py / SCALE;
				px += x;
				py += y;
				p->x = px / SCALE;
				p->y = py / SCALE;
				CHECK(s-1); // must start with moveto
				continue;
			}
			f = b.Read(1);
			CHECK(f);
			int x = b.Read(nbits);
			int y = b.Read(nbits);
			CHECK(x);
			CHECK(y);
			x = Bits::Signed(x,nbits);
			y = Bits::Signed(y,nbits);
			px += x;
			py += y;
			p->code = f?PATH_LINETO:PATH_MOVETO;
			if( f )
				CHECK(s-1); // must start with moveto
			p->x = px / SCALE;
			p->y = py / SCALE;
		}
		fd->path[s].code = PATH_END;
	ENDFOR;
	cache->dshapes[id->number] = cdata;
	return cdata;
}

/* ------------------------------------------------------------------------ */

ClipData *Clip::MakeData( vect *v, MTW *m ) {
	if( m->tag != MAKETAG(Clip) )
		return NULL;
	MTW *id = m->Child(MAKETAG(clid));
	if( id == NULL || id->number > MAXID )
		return NULL;
	mtv_cache *cache = MtvCache(v,m);
	if( cache == NULL )
		return NULL;
	ClipData *cdata = cache->dclips[id->number];
	if( cdata != NULL )
		return cdata;
	MTW *d = m->Child(MAKETAG(DATA));
	if( d == NULL )
		return NULL;
	cdata = new ClipData;
	cache->mclips[id->number] = NULL; // prevent recursion
	cdata->frames.Add( new WList<FrameOp*>() );

	Bits b(d->data,d->size);
	int nbits = b.Read(5);
	int ibits = b.Read(5);
	int curf = 0;
	CHECK(nbits);
	CHECK(ibits);

	while( true ) {
		int f = b.Read(1);
		CHECK(f);

		if( f == 1 ) {
			int depth = b.Read(nbits);
			CHECK(depth);
			matrix m;
			CHECK( ReadMatrix(&b,m) );
			cdata->frames[curf]->Add( new FrameOp(SETMATRIX,depth,&m) );
			continue;
		}
		
		f = b.Read(1);
		CHECK(f);
		if( f == 1 ) {
			cdata->frames.Add( new WList<FrameOp*>() );
			curf++;
			continue;
		}

		f = b.Read(2);
		CHECK(f);
		
		if( f == 0 )
			break;

		int depth = b.Read(nbits);
		CHECK(depth);

		if( f == 1 ) {
			int id = b.Read(ibits);
			MTW *cl = NULL;
			bool shape = false;
			if( id != -1 ) {
				cl = cache->mclips[id];
				if( cl == NULL ) {
					cl = cache->mshapes[id];
					shape = true;
				}
			}
			if( cl == NULL )
				id = -1;
			CHECK(id);
			void *data;
			if( shape )
				data = Shape::MakeData(v,cl);
			else
				data = Clip::MakeData(v,cl);
			if( data == NULL ) {
				delete cdata;
				return NULL;
			}
			cdata->frames[curf]->Add( new FrameOp(shape?PLACESHAPE:PLACECLIP,depth,data) );
			continue;
		}
		
		if( f == 2 ) {
			cdata->frames[curf]->Add( new FrameOp(REMOVE,depth,NULL) );
			continue;
		} 
		
		if( f == 3 ) {
			colortrans ctrans;
			CHECK( ReadCTrans(&b,ctrans) );
			cdata->frames[curf]->Add( new FrameOp(SETCOLOR,depth,&ctrans) );
			continue;
		}
	}
	
	cdata->frames.Delete( cdata->frames.Last() );
	if( cdata->frames.Empty() ) {
		delete cdata;
		return NULL;
	}
	cdata->frames.Optimize();
	cache->mclips[id->number] = m;
	cache->dclips[id->number] = cdata;
	return cdata;
}

/* ************************************************************************ */

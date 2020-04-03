/* ************************************************************************ */
/*																			*/
/*	M3D-SE Texture Manager													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "m3d.h"
#include "m3d_context.h"
#include "m3d_res.h"

namespace M3D {
namespace Resource {

/* ------------------------------------------------------------------------ */

struct TextureBlock {
	int start;
	int length;
	TextureBlock( int start, int length ) : start(start), length(length) { }
};

struct TextureMem {
	Context *context;
	TEXTURE t;
	TEXTURE tvideo;
	FORMAT fmt;
	int bwidth, bheight, bsize, pitch;
	WList<TextureBlock*> free_blocks;
	~TextureMem();
};

struct TextureData {
	Cached<TextureMem> m;
	int start;
	int nblocks;
	RECT r;
	TextureData( Cached<TextureMem> m, int start, int nblocks ) : m(m), start(start), nblocks(nblocks) { }
	~TextureData();
};

typedef WList<TextureBlock*>::Cel<TextureBlock*> TCel;

struct TextureContext {
	int twidth;
	int theight;
	WList<TextureMem*> tlist;
};

/* ------------------------------------------------------------------------ */

void InitTextures( Context *c ) {
	TextureContext *tc = new TextureContext;
	tc->twidth = 0;
	tc->theight = 0;
	c->textures = tc;
}

void CloseTextures( Context *c ) {
	c->textures->tlist.Delete();
	delete c->textures;
}

/* ------------------------------------------------------------------------ */

TextureMem::~TextureMem() {
	t->Release();
	if( tvideo )
		tvideo->Release();
	free_blocks.Delete();
	context->textures->tlist.Remove(this);	
}

/* ------------------------------------------------------------------------ */

Texture*
QueryTexture( Context *c, int width, int height, FORMAT fmt, POINT *p ) {
	int size = width*height;
	if( fmt == D3DFMT_UNKNOWN )
		fmt = c->buffer_format;
	else if( fmt == FORMAT_DYNAMIC(D3DFMT_UNKNOWN) )
		fmt = FORMAT_DYNAMIC(c->buffer_format);

	FOREACH(TextureMem*,c->textures->tlist);
		if( item->bheight >= height && fmt == item->fmt ) {
			int nblocks = 1+(width - 1)/item->bwidth;
			int alloc_size = nblocks * item->bsize;
			if( alloc_size / size >= 2 )
				continue;
			TextureMem *m = item;
			TCel *cur = m->free_blocks.Begin();
			TCel *prev = NULL;
			while( cur ) {
				if( cur->item->length >= nblocks ) {
					int start = cur->item->start;
					cur->item->start += nblocks;
					cur->item->length -= nblocks;
					if( cur->item->length == 0 ) {
						if( prev ) {
							prev->next = cur->next;
							delete cur->item;
							delete cur;
						} else
							m->free_blocks.Delete(cur->item);
					}
					Texture *t = new Texture();
					int x = (start % m->pitch)*m->bwidth;
					int y = (start / m->pitch)*m->bheight;
					t->t = m->t;
					t->data = new TextureData(Cache::Make(c,m),start,nblocks);
					t->pos.x = x*1.0f / c->textures->twidth;
					t->pos.y = y*1.0f / c->textures->theight;
					t->pos.sx = (width * 1.0f) / c->textures->twidth;
					t->pos.sy = (height * 1.0f) / c->textures->theight;

					t->data->r.left = x;
					t->data->r.top = y;
					t->data->r.bottom = x + height;
					t->data->r.right = y + width;

					if( p ) {
						p->x = x;
						p->y = y;
					}
					return t;
				}
				else {
					prev = cur;
					cur = cur->next;
				}
			}
		}
	ENDFOR;

	if( c->textures->twidth == 0 ) {
		D3DCAPS8 caps;
		c->device->GetDeviceCaps(&caps);
		c->textures->twidth = caps.MaxTextureWidth;
		if( c->textures->twidth > 512 )
			c->textures->twidth = 512;
		c->textures->theight = caps.MaxTextureHeight;
		if( c->textures->theight > 512 )
			c->textures->theight = 512;
	}
	
	if( width > c->textures->twidth || height > c->textures->theight )
		return NULL;

	TEXTURE t;
	FORMAT base_fmt = (FORMAT)(fmt & 0x7FFFFFFF);
	bool dynamic = (fmt & (1 << 31)) != 0;
	if( M3DERROR(c->device->CreateTexture(c->textures->twidth,c->textures->theight,1,0,base_fmt,dynamic?D3DPOOL_SYSTEMMEM:D3DPOOL_DEFAULT,&t),"CreateTexture") != D3D_OK )
		return NULL;	

	TextureMem *m = new TextureMem();
	// blocks are 8x8 aligned
	m->t = t;
	m->context = c;
	m->tvideo = NULL;
	m->bwidth = (width + 7) & ~7;
	m->bheight = (height + 7) & ~7;
	m->bsize = m->bwidth * m->bheight;
	m->pitch = c->textures->twidth / m->bwidth + 1;
	m->fmt = fmt;

	int y;
	int start = 0;
	for(y=0;y<c->textures->theight/m->bheight;y++)
		m->free_blocks.Add( new TextureBlock(y*m->pitch,m->pitch-1) );
	c->textures->tlist.AddFirst(m);
	
	if( dynamic ) {
		if( M3DERROR(c->device->CreateTexture(c->textures->twidth,c->textures->theight,1,0,base_fmt,D3DPOOL_DEFAULT,&t),"CreateVideoTexture") ) {
			delete m;
			return NULL;
		}
		m->tvideo = t;
	}

	// loop (this time should be ok !)
	return QueryTexture(c,width,height,fmt,p);
}

/* ------------------------------------------------------------------------ */

void
AddBlock( TextureMem *m, int start, int length ) {
	TCel *c = m->free_blocks.Begin();
	TCel *prev = NULL;
	while( c != NULL && c->item->start < start ) {
		prev = c;
		c = c->next;
	}

	TextureBlock *iprev,*ic;
	iprev = prev?prev->item:NULL;
	ic = c?c->item:NULL;

	if( c == NULL ) {
		if( prev != NULL && iprev->start+iprev->length == start )
			iprev->length += length;
		else
			m->free_blocks.Add( new TextureBlock(start,length) );
	} else {
		if( prev != NULL && iprev->start + iprev->length == start ) {
			iprev->length += length;
			if( ic->start == start + length ) {
				prev->next = c->next;
				iprev->length += ic->length;
				delete ic;
				delete c;
			}
		} else if( ic->start == start + length )
			ic->start -= length;
		else {
			if( prev == NULL )
				m->free_blocks.AddFirst( new TextureBlock(start,length) );
			else {
				TCel *newc = new TCel(new TextureBlock(start,length),c);
				prev->next = newc;
			}
		}
	}
}

/* ------------------------------------------------------------------------ */

Texture::~Texture() {
	if( data != NULL )
		delete data;
	else
		t->Release();
}

/* ------------------------------------------------------------------------ */

TextureData::~TextureData() {	
	Resource::AddBlock(m.Data(),start,nblocks);
}

/* ------------------------------------------------------------------------ */
};}; // namespace
/* ************************************************************************ */

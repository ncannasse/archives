/* ************************************************************************ */
/*																			*/
/*	M3D-SE Resource Loading and Cache										*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "m3d.h"
#include "m3d_res.h"
#include "m3d_2d.h"
#include "m3d_context.h"
#include "../common/mtw.h"
#include "../common/wpairbtree.h"

/* ------------------------------------------------------------------------ */

namespace M3D {

namespace Resource {

/* ------------------------------------------------------------------------ */

enum RESTYPE {
	RES_TEXTURE,
	RES_FONT,
	RES_FONT_FILE
};

struct ResItem {
	RESTYPE t;
	MTW *res;

	inline static int compare( ResItem *a, ResItem *b ) {
		if( a->res == b->res )
			return a->t - b->t;
		return (int)a->res - (int)b->res;
	}

	ResItem(RESTYPE t, MTW *res) : t(t), res(res) { }
};

struct FontFile {
	WString file;

	FontFile( WString file ) : file(file) { }
	~FontFile() {
		if( RemoveFontResource(file.c_str()) )
			DeleteFile(file.c_str());
	}

};

inline int uint_compare( unsigned int a, unsigned int b ) {
	return b - a;
}

struct ResContext {
	WList<MTW*> roots;
	WPairBTree< unsigned int, MTW* > res_ids;
	WPairBTree< ResItem* ,Weak<void>* > res_cache;
	ResContext() : res_ids(uint_compare), res_cache(ResItem::compare) {
	}
};

/* ------------------------------------------------------------------------ */

template <class T> Cached<T> LoadAny( Context *c, RESTYPE t , T *loader( Context *c, MTW * ), MTW *res ) {
	if( res == NULL )
		return Cached<T>();
	ResItem r(t,res);
	Weak<T> *w = (Weak<T>*)c->res->res_cache.Find(&r);
	if( w != NULL ) {
		if( !w->Available() )
			c->res->res_cache.Delete(&r);
		else
			return w->Make();
	}
	T *lval = loader( c, res );
	if( lval == NULL )
		return Cached<T>();
	w = new Weak<T>(Cache::MakeWeak(c,lval));
	c->res->res_cache.Add( new ResItem(r), (Weak<void>*)w );
	return w->Make();
}

/* ------------------------------------------------------------------------ */

void AddIds( Context *c, MTW *r ) {
	if( r->IsGroup() ) {
		MTW *rid = r->Child(MAKETAG(rsid));
		if( rid != NULL )
			c->res->res_ids.Add(rid->number,r);
		FOREACH(MTW*,*r->childs);
			AddIds(c,item);
		ENDFOR;
	}
}

/* ------------------------------------------------------------------------ */

void AddRoot( Context *c, MTW *r ) {
	MTW *rd = new MTW(*r);
	c->res->roots.Add(rd);
	AddIds(c,rd);
}

/* ------------------------------------------------------------------------ */

MTW *Find( Context *c, const char *name ) {
	int len = strlen(name);
	FOREACH(MTW*,c->res->roots);
		if( item->IsGroup() ) {
			MTW *tname = item->Child(MAKETAG(RSNM));
			if( tname != NULL && tname->size == len + 1 && memcmp(name,tname->data,len+1) == 0 )
				return item;	
			FOREACH(MTW*,*item->childs);
				if( item->IsGroup() ) {
					MTW *tname = item->Child(MAKETAG(RSNM));
					if( tname != NULL && tname->size == len + 1 && memcmp(name,tname->data,len+1) == 0 )
						return item;
				}
			ENDFOR;
		}
	ENDFOR;
	return NULL;
}

/* ------------------------------------------------------------------------ */

__declspec(dllexport) MTW *Find( Context *c, unsigned int id ) {
	if( c == NULL || c->res == NULL )
		return NULL;
	return c->res->res_ids.Find(id);
}

/* ------------------------------------------------------------------------ */

static Texture *load_texture( Context *c, MTW *res ) {
	if( res->tag != MAKETAG(Pict) )
		return NULL;
	MTW *data = res->Child(MAKETAG(DATA));
	bool wrap = res->HasChild(MAKETAG(wRAP));
	if( data == NULL )
		return NULL;

	TEXTURE t;
	D3DXIMAGE_INFO header;
	if( M3DERROR(D3DXCreateTextureFromFileInMemoryEx(
			c->device,
			data->data,
			data->size,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			1,
			0,
			D3DFMT_UNKNOWN,
			wrap?D3DPOOL_DEFAULT:D3DPOOL_SYSTEMMEM,
			D3DX_FILTER_NONE,
			D3DX_FILTER_LINEAR,
			0xFFFF00FF,
			&header,
			NULL,
			&t),"LoadTexture") )
		return NULL;

	D3DSURFACE_DESC sdesc;
	t->GetLevelDesc(0,&sdesc);

	Resource::Texture *tex;
	if( wrap ) {
		tex = new Resource::Texture();
		tex->data = NULL;
		tex->pos.x = 0;
		tex->pos.y = 0;
		tex->pos.sx = header.Width * 1.0f / sdesc.Width;
		tex->pos.sy = header.Height * 1.0f / sdesc.Height;
		tex->t = t;
	} else {
		POINT p;
		tex = QueryTexture(c,header.Width,header.Height,sdesc.Format,&p);
		if( tex == NULL )
			return NULL;

		SURFACE ssrc,sdst;
		RECT rdst = { p.x, p.y, p.x + header.Width, p.y + header.Height };
		RECT rsrc = { 0 , 0, header.Width, header.Height };
		t->GetSurfaceLevel(0,&ssrc);
		tex->t->GetSurfaceLevel(0,&sdst);
		if( M3DERROR(c->device->CopyRects(ssrc,&rsrc,1,sdst,&p),"CopyTexture") ) {
			sdst->Release();
			ssrc->Release();
			delete tex;
			return NULL;
		}

		ssrc->Release();
		sdst->Release();
	}

	tex->res = res;
	return tex;
}

/* ------------------------------------------------------------------------ */

static FontFile *add_new_font( Context *c, MTW *res ) {
	MTW *face = res->Child(MAKETAG(FACE));
	MTW *data = res->Child(MAKETAG(DATA));

	char tmp_dir[256];
	GetTempPath(256,tmp_dir);
	WString tmp_file = tmp_dir + (WString)"_honsen_"+(WString)(char*)face->data+".ttf";
	FILE *f = fopen(tmp_file.c_str(),"wb");
	if( f == NULL ) {
		// file already exists and is used. There is VERY HIGH possibility that its contents
		// is actually the font we want to install, then return without any error
		return new FontFile(tmp_file);
	}
	fwrite(data->data,1,data->size,f);
	fclose(f);
	int n = AddFontResource(tmp_file.c_str());
	if( n == 0 ) {
		M3DDEBUG("Failed to register font "+(WString)(char*)face->data);
		return NULL;
	}
	return new FontFile(tmp_file);
}

/* ------------------------------------------------------------------------ */

static FontRes *load_font( Context *c, MTW *res ) {
	if( res->tag != MAKETAG(Font) )
		return NULL;
	MTW *size = res->Child(MAKETAG(size));
	MTW *flags = res->Child(MAKETAG(flag));
	MTW *face = res->Child(MAKETAG(FACE));
	if( size == NULL || flags == NULL || face == NULL )
		return NULL;
	MTW *data = res->Child(MAKETAG(DATA));
	Cached<FontFile> font_file;

	if( data != NULL )
		font_file = LoadAny(c,RES_FONT_FILE,add_new_font,res);

	Font *f = Font::New(c,(const char*)face->data,size->number,flags->number);
	if( f == NULL )
		return NULL;
		
	Resource::FontRes *ft = new Resource::FontRes();
	ft->file = font_file;
	ft->f = f;
	ft->res = res;
	return ft;
}

/* ------------------------------------------------------------------------ */

FontRes::~FontRes() {
	delete f;
}

/* ------------------------------------------------------------------------ */

Cached<Texture> LoadTexture( Context *c, MTW *t ) {
	return LoadAny(c,RES_TEXTURE,load_texture,t);
}

/* ------------------------------------------------------------------------ */

Cached<FontRes> LoadFont( Context *c, MTW *f ) {
	return LoadAny(c,RES_FONT,load_font,f);
}

/* ------------------------------------------------------------------------ */

void Init( Context *c ) {
	c->res = new ResContext;
}

void Close( Context *c ) {
	c->res->roots.Delete();
	c->res->res_cache.Delete();
	delete c->res;
}

/* ------------------------------------------------------------------------ */

};}; // namespaces
/* ************************************************************************ */

/* ************************************************************************ */
/*																			*/
/*	M3D-SE Resource Loading and Cache										*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include "mdx.h"
#include "m3d_cache.h"

#define FORMAT_DYNAMIC(x)		((FORMAT)(x | (1<<31)))

class MTW;

namespace M3D {

	struct Context;
	class Font;

namespace Resource {

	struct TextureData;

	struct TPos {
		FLOAT x,y;
		FLOAT sx,sy;
	};

	struct Texture {
		TEXTURE t;
		MTW *res;
		TPos pos;
		TextureData *data;
		~Texture();
	};

	struct FontFile;

	struct FontRes {
		Font *f;
		MTW *res;
		Cached<FontFile> file;
		~FontRes();
	};

	void Init( Context *c );
	void Close( Context *c );
	void InitTextures( Context *c );
	void CloseTextures( Context *c );
	void AddRoot( Context *c, MTW *r );

	Cached<Texture> LoadTexture( Context *c, MTW *t );
	Cached<FontRes> LoadFont( Context *c, MTW *f );

	Texture *QueryTexture( Context *c, int width, int height, FORMAT fmt, POINT *p );

};

};

/* ************************************************************************ */

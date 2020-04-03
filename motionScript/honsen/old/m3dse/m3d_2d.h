/* ************************************************************************ */
/*																			*/
/*	M3D-SE	2D Classes														*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include "mdx.h"
#include "m3d_res.h"
#include "../common/wstring.h"
#include "../common/wmem.h"

namespace M3D {

	typedef struct _RenderData *RenderData;

	struct VERTEX2D {
		FLOAT x,y,z,w;
		DWORD c;
		FLOAT tu,tv;
		VERTEX2D() { }
		VERTEX2D(FLOAT x,FLOAT y,FLOAT tu,FLOAT tv,DWORD c) : x(x), y(y), tu(tu), tv(tv), c(c) { }
		VERTEX2D(double x,double y,FLOAT tu,FLOAT tv,DWORD c) : x((FLOAT)x), y((FLOAT)y), tu(tu), tv(tv), c(c) { }
	};

	struct Context;

	bool Init2D( Context *c );
	int Render2D( Context *c, bool alpha );
	void Close2D( Context *c );

	class Group;

	class Element {
		static void CleanRef( void *ref );
	protected:
		RenderData data;
	public:
		int depth;
		double x,y;
		bool visible;
		DWORD color;
		double alpha;
		Group *parent;
		Context *context;
		void *script_ref;
		Element( Context *c, Group *parent );
		void SetDepth( int depth );
		void UpdateColor();

		virtual void Render( bool alpha ) = 0;
		virtual void ColorAlphaModified();
		virtual void PosRotScaleModified();
		virtual ~Element();
	};

	class Group : public Element {
	public:
		double xscale, yscale;
		double rotation;
		double z_base;
		WBTree<int,Element*> childs;

		Group( Group *parent );
		Group( Context *c, double z_base );
		~Group();

		void AddChild( Element *e );
		void RemoveChild( Element *e );
		void SetZBase( double z_base );
		MATRIX *Matrix();

		void Render( bool alpha );
		void ColorAlphaModified();
		void PosRotScaleModified();
	};

	class Sprite : public Element {
		DWORD argb;
		bool modified;
	public:
		VERTEX2D v[4];
		double width, height;
		double tu,tv,tu2,tv2;
		Cached<Resource::Texture> texture;
		
		Sprite( Group *parent );
		~Sprite();

		void Render( bool alpha );
		void Modified();
	};

#define SKIP_CHAR	(-999)

	class Font {
	public:
		TEXTURE t;
		FLOAT coords[256*6];
		FLOAT height;
		Font( TEXTURE t );
		~Font();
		static Font *New( Context *c, const char *face, int size, int flags );
	};

	class Text : public Element {
	public:
		DWORD argb;
		WString text;
		Cached<Resource::FontRes> font;

		Text( Group *parent );
		~Text();

		void Render( bool alpha );
	};
};

/* ************************************************************************ */

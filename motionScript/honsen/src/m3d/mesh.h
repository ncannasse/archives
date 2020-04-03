/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include "../../common/wlist.h"
#include "maths3d.h"

class MTW;
struct cached_texture;
typedef struct _MeshData MeshData;
typedef struct _m3d m3d;

class Mesh {
	MeshData *data;
public:
	Mesh( Mesh *parent, MeshData *data );
	~Mesh();

	matrix pos;
	matrix abs_pos;
	color abs_color;
	unsigned int color;
	double alpha;
	bool visible;
	bool transform;
	bool sprite;
	bool light;
	int srcblend;
	int dstblend;

	void *ref;
	Mesh *parent;
	WList<Mesh*> childs;

	// builder
	void Resize( int nverts );
	bool DrawVector( float x, float y, float z );
	bool DrawTCoords( float tu, float tv );
	bool DrawColor( unsigned int color );
	void CalcNormals();

	void SetTexture( cached_texture *t );
	cached_texture *GetTexture();
	void Render( m3d *m, bool visible, bool alpha );

	static void RefRelease( void *ref );
	static MeshData *Make( m3d *m3d, MTW *m );
};

class Texture {
public:
	static void Size( cached_texture *t, int *width, int *height );
	static cached_texture *Make( m3d *m3d, MTW *t );
	static cached_texture *Copy( cached_texture *t );
	static void Delete( cached_texture *t );

};

/* ************************************************************************ */

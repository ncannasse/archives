/* ************************************************************************ */
/*																			*/
/*	Honsen Vector Rendering													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#include "vect.h"
#include "../../common/wstring.h"
#include "../../common/warray.h"

#define MAX_DEPTH 0x10000

class MTW;
class Clip;

struct FillData;
struct ShapeData;
struct ClipData;
struct TextData;

typedef double matrix[6];
typedef float colortrans[8];

class Element {
public:
	bool modified;
	Clip *parent;
	matrix mat;
	matrix abs_mat;
	colortrans col;
	colortrans abs_col;
	Element( Clip *p );
	void UpdateTransforms();
	virtual void Render( vect *v ) = 0;
	virtual bool NextFrame() { 
		bool m = modified;
		modified = false;
		return m;
	};
	virtual ~Element();
};

class Shape : public Element {
	ShapeData *data;
public:
	Shape( Clip *parent, ShapeData *d );
	~Shape();

	void Render( vect *v );
	static ShapeData *MakeData( vect *v, MTW *t );
};

class Text : public Element {
	TextData *data;
public:
	Text( Clip *parent, TextData *d );
	~Text();

	void *ref;
	void Render( vect *v );
	WString text;

	static TextData *MakeData( vect *v, MTW *t );
};

class Clip : public Element {
	ClipData *data;
	int prev_frame;
public:
	WArray<Element*> depths;
	void *ref;
	bool visible;
	double frame;
	double speed;
	double rotation;

	double Rotation();
	double XScale();
	double YScale();

	Clip( Clip *parent, ClipData *d );
	~Clip();

	void Render( vect *v );
	bool NextFrame();
	int TotalFrames();

	static void RefRelease( void *ref );
	static ClipData *MakeData( vect *v, MTW *m );
};

/* ************************************************************************ */

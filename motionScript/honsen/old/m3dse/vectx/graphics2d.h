/* ************************************************************************ */
/*																			*/
/*	M3D-SE Vector Graphics Library											*/
/*	Julien Chevreux															*/
/*	(c)2004 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include "edge.h"

typedef unsigned long DWORD;
typedef unsigned char BYTE;

typedef struct control {
	int coef;
	DWORD color;
	control(int cf, DWORD c) {
		coef = cf;
		color = c;
	}
} control;

class Graphics2D {
public:
	Graphics2D(int width, int height, int widthbits, bool useAA = false);
	~Graphics2D();
	void drawLine(FLOAT x1, FLOAT x2, FLOAT y1, FLOAT y2, DWORD color, FLOAT w);
	void drawLine(EdgeList *edges, DWORD color, FLOAT w);
	DWORD *getData();
	void fillPolygon(EdgeList *edges, DWORD color, bool shapeAA = true,
						WList<EdgeColored *> *edgesColored = NULL,
						WList<control *> *controls = NULL,
						VECTOR2 center = VECTOR2(0,0));
	void reset();
	bool toggleAA();
	bool getAAstate();

private:
	bool addLine(EdgeList *edges, FLOAT xa, FLOAT xb, FLOAT ya, FLOAT yb, 
		DWORD color, FLOAT w);
	void drawBLine(FLOAT xa, FLOAT xb, FLOAT ya, FLOAT yb, DWORD color);
	void buildActiveList(EdgeList *active, EdgeList *global, int floor);
	void updateActiveList(EdgeList *active, int floor);
	DWORD interpolate(DWORD c1, DWORD c2, FLOAT coef);
	DWORD interpolate2(DWORD c1, DWORD c2, FLOAT coef);
		
	DWORD *data;
	int width, height, widthbits;
	int minx, miny, deltax, deltay;
	BYTE colors[4096];
	bool useAA;
};

/* ************************************************************************ */

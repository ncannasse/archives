/* ************************************************************************ */
/*																			*/
/*	M3D-SE Vector Graphics Library											*/
/*	Julien Chevreux															*/
/*	(c)2004 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#include "../mdx.h"

class Edge {
public:
	Edge(VECTOR2 p1, VECTOR2 p2);
	Edge(Edge *e);
	~Edge();
	virtual void update();

	FLOAT yMin, yMax, xyMin, xyMax, slope, length;
	bool use;
};

#include "edgeList.h"

class EdgeColored : public Edge {
public:
	EdgeColored(VECTOR2 p1, VECTOR2 p2, DWORD p1c, DWORD p2c);
	EdgeColored(EdgeColored *e);
	~EdgeColored();
	virtual void update();

	DWORD minColor, maxColor;
	FLOAT coef, coefSlope;
};

void addEdgeOrdered(EdgeList *edges, Edge *e, bool useY);

/* ************************************************************************ */

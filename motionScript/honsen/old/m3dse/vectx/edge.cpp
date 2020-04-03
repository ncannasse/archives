/* ************************************************************************ */
/*																			*/
/*	M3D-SE Vector Graphics Library											*/
/*	Julien Chevreux															*/
/*	(c)2004 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "edge.h"

Edge::Edge(VECTOR2 p1, VECTOR2 p2) {
	VECTOR2 tv = p1-p2;
	length = vlength2(&tv);

	if(p1[1] < p2[1]){
		yMin = p1.y;
		yMax = p2.y;
		xyMin = p1.x;
		xyMax = p2.x;
	}
	else{
		yMin = p2.y;
		yMax = p1.y;
		xyMin = p2.x;
		xyMax = p1.x;
	}

	FLOAT t = p1.y - p2.y;
	if( t == 0 )
		use = false;
	else{
		use = true;
		slope = (p1.x - p2.x)/t;
	}
}

Edge::Edge(Edge *e) : yMin(e->yMin), yMax(e->yMax), xyMin(e->xyMin), xyMax(e->xyMax), slope(e->slope), use(e->use) {
}

Edge::~Edge() {
}

void Edge::update() {
	xyMin += slope;
}

/*----------------------------------------------------------------------------*/

EdgeColored::EdgeColored(VECTOR2 p1, VECTOR2 p2, DWORD p1c, DWORD p2c) : Edge(p1, p2), coef(0) {
	if( p1[1] < p2[1] ) {
		minColor = p1c;
		maxColor = p2c;
	}
	else{
		minColor = p2c;
		maxColor = p1c;
	}
	if( p1[1] == p2[1] ) {
		if( p1[0] < p2[0] ) {
			xyMin = p1[0];
			xyMax = p2[0];
		}
		else {
			xyMin = p2[0];
			xyMax = p1[0];
		}
	}
	coefSlope = (FLOAT)(1.0 / (yMax - yMin));
}

EdgeColored::EdgeColored(EdgeColored *e) : 
	Edge((Edge *)e),
	minColor(e->minColor),
	maxColor(e->maxColor),
	coef(0),
	coefSlope(e->coefSlope) {
}

EdgeColored::~EdgeColored() {

}

void EdgeColored::update() {
	xyMin += slope;
	coef += coefSlope;
}

/*----------------------------------------------------------------------------*/

bool cf(Edge *e1, Edge *e2) {
	return ( e2->xyMin > e1->xyMin 
			|| (e2->xyMin == e1->xyMin 
				&& e2->xyMin + e2->slope > e1->xyMin + e1->slope) );
}

bool cfy(Edge *e1, Edge *e2) {
	return ( (e2->yMin > e1->yMin)
			|| (e2->yMin == e1->yMin && e2->xyMin > e1->xyMin) 
			|| (e2->yMin == e1->yMin && e2->xyMin == e1->xyMin
				&& e2->xyMin + e2->slope > e1->xyMin + e1->slope) );
}

void addEdgeOrdered(EdgeList *edges, Edge *e, bool useY) {
	if(useY)
		edges->AddIf(e, cfy);
	else
		edges->AddIf(e, cf);
}

/* ************************************************************************ */

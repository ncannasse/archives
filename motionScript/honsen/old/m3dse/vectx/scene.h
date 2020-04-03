/* ************************************************************************ */
/*																			*/
/*	M3D-SE Vector Graphics Library											*/
/*	Julien Chevreux															*/
/*	(c)2004 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#include "edge.h"

class Graphics2D;
class Scene;

typedef float FLOAT;
typedef unsigned long DWORD;

class FillType {
public:
	FillType(DWORD c);
	~FillType();
	DWORD getColour();
	virtual void fillPolygon(Graphics2D * device, EdgeList *edgesOrdered,
								bool changed);

private:
	DWORD colour;
};

class Gradient:FillType {
	typedef struct control control;

public:
	Gradient(int type, int nb, Scene *scene); 
	~Gradient();
	void addControl(int coef, DWORD color);
	void generate();
	virtual void fillPolygon(Graphics2D * device, EdgeList *edgesOrdered, 
								bool changed);
	void transform(FLOAT &x, FLOAT &y);
	void setTranslate(VECTOR2 t);
	VECTOR2 getTranslate();
	void setScale(FLOAT f);
	FLOAT getScale();
	void setRotation(FLOAT f);
	FLOAT getRotation();

private:
	int type;
	int lastControl;
	int nb;
	control *controls;
	EdgeList edges;
	WList<control *> radialControls;
	bool alpha;
	Scene *scene;
	FLOAT rot;
	VECTOR trans;
	FLOAT scale;
	VECTOR2 center;
};

class LineStyle {
public:
	LineStyle(int f, FLOAT w);
	~LineStyle();
	int getFill();
	FLOAT getWidth();
private:
	int fill;
	FLOAT width;
};

class Segment {
public:
	Segment(VECTOR2 e);
	~Segment();
	VECTOR2 getEndPoint();
	virtual void addTo(EdgeList *edges, VECTOR2 start, 
		Scene *scene, VECTOR2 *bbdown, VECTOR2 *bbup) = 0;
	
protected:
	VECTOR2 endPoint;
};

class LineSegment:Segment {
public:
	LineSegment(VECTOR2 e);
	~LineSegment();
	virtual void addTo(EdgeList *edges, VECTOR2 start, 
		Scene *scene, VECTOR2 *bbdown, VECTOR2 *bbup);
};

class BezierSegment:Segment {
public:
	BezierSegment(VECTOR2 c1, VECTOR2 c2, VECTOR2 e);
	~BezierSegment();
	virtual void addTo(EdgeList *edges, VECTOR2 start, 
		Scene *scene, VECTOR2 *bbdown, VECTOR2 *bbup);
	
private:
	VECTOR2 c1, c2;
	VECTOR2 bezier(VECTOR2 p1, VECTOR2 p2, VECTOR2 pc, FLOAT t);
	VECTOR2 bezier(VECTOR2 p1, VECTOR2 p2, VECTOR2 pc1, 
						VECTOR2 pc2, FLOAT t);
};

class Shape {
public:
	Shape(Scene *s);
	~Shape();
	void setFillType(int f);
	void setLineStyle(int l);
	void setStart(VECTOR2 s);
	void setEnd(VECTOR2 e);
	void addSegment(Segment *s);
	void setClosed(bool b);
	bool render(Graphics2D *device, bool line);

private:
	Scene *scene;
	int fillType;
	int lineStyle;
	FillType *ft;
	LineStyle *ls;
	DWORD lc;
	WList<Segment *> segments;
	EdgeList edges, edgesOrdered;
	VECTOR2 start, end, bbdown, bbup;
	bool closed;

	bool onScreen();
};

class Scene {
public:
	Scene(int minX, int maxX, int minY, int maxY);
	~Scene();
	void addFillType(FillType *f);
	int getFillTypeNb();
	FillType * getFillType(int n);

	void addLineStyle(LineStyle *l);
	int getLineStyleNb();
	LineStyle * getLineStyle(int n);

	void addShape(Shape *s);
	int getShapeNb();
	Shape * getShape(int n);

	DWORD *render(DWORD width, DWORD height, DWORD widthbits, bool line, 
					int *cpt = NULL);
	void transform(FLOAT &x);
	void transform(FLOAT &x, FLOAT &y);
	void setTranslate(VECTOR2 t);
	VECTOR2 getTranslate();
	void setScale(FLOAT f);
	FLOAT getScale();
	void setRotation(FLOAT f);
	FLOAT getRotation();
	bool toggleAA();
	int getWidth();
	int getHeight();
	bool haveChanged();

private:
	WList<FillType *> fillTypes;
	WList<LineStyle *> lineStyles;
	WList<Shape *> shapes;
	Graphics2D *device;
	int width, height;
	int minX, maxX, minY, maxY;
	bool changed;
	FLOAT dx, dy;
	FLOAT rot;
	VECTOR trans;
	FLOAT scale;
};

void setMinMax(VECTOR2 v, VECTOR2 *bbdown, VECTOR2 *bbup);

/* ************************************************************************ */

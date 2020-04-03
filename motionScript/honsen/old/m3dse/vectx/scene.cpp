/* ************************************************************************ */
/*																			*/
/*	M3D-SE Vector Graphics Library											*/
/*	Julien Chevreux															*/
/*	(c)2004 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <math.h>
#include "scene.h"
#include "graphics2d.h"

FillType::FillType(DWORD c):colour(c){

}

FillType::~FillType(){

}

DWORD FillType::getColour() {
	return colour;
}

void FillType::fillPolygon(Graphics2D * device, EdgeList *edgesOrdered,
						   bool changed)
{
	device->fillPolygon(edgesOrdered, colour);
}

/*
#define MAX_GRAD 16384
#define GRAD 32768
*/
#define MAX_GRAD 1024
#define GRAD 2048

Gradient::Gradient(int type, int nb, Scene *scene):FillType(0), type(type),
lastControl(0), nb(nb), alpha(false), scene(scene), scale(1), rot(0)
{
	controls = (control *)malloc(nb * sizeof(control));
	trans = VECTOR(0,0,0);
	center = VECTOR2(0,0);
}

Gradient::~Gradient(){
	free(controls);
	edges.Delete();
	radialControls.Delete();
}

void Gradient::addControl(int coef, DWORD color){
	controls[lastControl++] = control(coef, color);
	if(((color>>24)&0xff) != 0xff)
		alpha = true;
}

void Gradient::generate(){
	edges.Delete();
	radialControls.Delete();
	if(nb == 0)
		return;
	
	VECTOR2 topLeft = VECTOR2(-MAX_GRAD,MAX_GRAD);
	VECTOR2 topRight = VECTOR2(MAX_GRAD, MAX_GRAD);
	VECTOR2 bottomLeft = VECTOR2(-MAX_GRAD, -MAX_GRAD);
	VECTOR2 bottomRight = VECTOR2(MAX_GRAD, -MAX_GRAD);

	transform(topLeft.x, topLeft.y);
	transform(topRight.x, topRight.y);
	transform(bottomLeft.x, bottomLeft.y);
	transform(bottomRight.x, bottomRight.y);

	scene->transform(topLeft.x, topLeft.y);
	scene->transform(topRight.x, topRight.y);
	scene->transform(bottomLeft.x, bottomLeft.y);
	scene->transform(bottomRight.x, bottomRight.y);

	if(type == 0){
		DWORD c1 = 0xff000000, c2 = 0xff000000;
		int last = 0;

		if(controls[0].coef == 0)
			addEdgeOrdered(&edges, 
						  (Edge *)new EdgeColored(topLeft, 
										bottomLeft, 
										controls[0].color, 
										controls[0].color),
						  true);
		else
			addEdgeOrdered(&edges, 
						  (Edge *)new EdgeColored(topLeft, 
										bottomLeft, 
										0xff000000, 
										0xff000000),
						  true);

		for(int i = 0; i < nb; i++){			
			if(controls[i].coef == last){
				c1 = controls[i].color;
				continue;
			}
			c2 = controls[i].color;
			FLOAT coord1 = (FLOAT)((last/255.0 * GRAD) - MAX_GRAD);
			FLOAT coord2 = (FLOAT)((controls[i].coef/255.0 * GRAD) - MAX_GRAD);
			VECTOR2 temp1 = VECTOR2(coord1, MAX_GRAD);
			VECTOR2 temp2 = VECTOR2(coord2, MAX_GRAD);
			transform(temp1.x, temp1.y);
			transform(temp2.x, temp2.y);
			scene->transform(temp1.x, temp1.y);
			scene->transform(temp2.x, temp2.y);
			addEdgeOrdered(&edges, 
						  (Edge *)new EdgeColored(temp1, 
										temp2, 
										c1, 
										c2),
						  true);
			temp1 = VECTOR2(coord1, -MAX_GRAD);
			VECTOR2 temp3 = VECTOR2(coord2, -MAX_GRAD);
			transform(temp1.x, temp1.y);
			transform(temp3.x, temp3.y);
			scene->transform(temp1.x, temp1.y);
			scene->transform(temp3.x, temp3.y);
			addEdgeOrdered(&edges, 
						  (Edge *)new EdgeColored(temp1, 
										temp3, 
										c1, 
										c2),
						  true);
			addEdgeOrdered(&edges, 
						  (Edge *)new EdgeColored(temp2, 
										temp3, 
										c2, 
										c2),
						  true);
			c1 = c2;
			last = 	controls[i].coef;
		}

		if(controls[nb-1].coef != 255){
			addEdgeOrdered(&edges, 
						  (Edge *)new EdgeColored(topRight, 
										bottomRight, 
										0xffffffff, 
										0xffffffff),
						  true);
			FLOAT coord1 = (FLOAT)((last/255.0 * GRAD) - MAX_GRAD);
			VECTOR2 temp1 = VECTOR2(coord1, MAX_GRAD);
			transform(temp1.x, temp1.y);
			scene->transform(temp1.x, temp1.y);
			addEdgeOrdered(&edges, 
						  (Edge *)new EdgeColored(temp1, 
										topRight, 
										c1, 
										0xffffffff),
						  true);
			temp1 = VECTOR2(coord1, -MAX_GRAD);
			transform(temp1.x, temp1.y);
			scene->transform(temp1.x, temp1.y);
			addEdgeOrdered(&edges, 
						  (Edge *)new EdgeColored(temp1, 
										bottomRight, 
										c1, 
										0xffffffff),
						  true);
		}
	}
	if(type == 1){
		center = (bottomRight + topLeft) / 2.0;
		for(int i = 0; i < nb; i++){
			FLOAT mult = scale;
			scene->transform(mult);
			radialControls.Add(new control((int)pow(controls[i].coef*mult, 2),
											controls[i].color));
		}
	}
}

void Gradient::fillPolygon(Graphics2D * device, EdgeList *edgesOrdered, 
						   bool changed)
{
	if((edges.Empty() && radialControls.Empty()) || changed)
		generate();
	device->fillPolygon(edgesOrdered, 
						(alpha)?0x00000000:0xff000000, 
						true, 
						(type == 0)?(WList<EdgeColored *> *)&edges:NULL, 
						(type == 0)?NULL:&radialControls,
						center);
}

void Gradient::transform(FLOAT &x, FLOAT &y){
	QUATERNION q;
	rquaternion( &q, &VECTOR(0,0,1), rot);
	MATRIX transformMatrix;
	VECTOR translation(trans.x,trans.y,0);
	mtransform( &transformMatrix, 
				NULL,
				NULL,
				&VECTOR(scale, (type == 0)?1:scale, 1),
				NULL,
				&q,
				&translation);

	VECTOR2 temp(x,y);
	vtransform2( &temp, &temp, &transformMatrix);
	x = temp.x;
	y = temp.y;
}

void Gradient::setTranslate(VECTOR2 t){
	trans.x = t.x;
	trans.y = t.y;
}

VECTOR2 Gradient::getTranslate(){
	return VECTOR2(trans.x, trans.y); 
}

void Gradient::setScale(FLOAT f){
	scale = f;
}

FLOAT Gradient::getScale(){
	return scale;
}

void Gradient::setRotation(FLOAT f){
	rot = f;
}

FLOAT Gradient::getRotation(){
	return rot;
}

/*----------------------------------------------------------------------------*/

LineStyle::LineStyle(int f, FLOAT w)
:fill(f), width(w) {

}

LineStyle::~LineStyle(){

}

int LineStyle::getFill(){
	return fill;
}

FLOAT LineStyle::getWidth(){
	return width;
}

/*----------------------------------------------------------------------------*/

Segment::Segment(VECTOR2 e):endPoint(e){

}

Segment::~Segment(){

}

VECTOR2 Segment::getEndPoint(){
	return endPoint;
}

/*----------------------------------------------------------------------------*/

LineSegment::LineSegment(VECTOR2 e):Segment(e){

}

LineSegment::~LineSegment(){

}

void LineSegment::addTo(EdgeList *edges, VECTOR2 start, 
						Scene *scene, VECTOR2 *bbdown, 
						VECTOR2 *bbup)
{
	VECTOR2 p1 = start, p2 = endPoint;
	scene->transform(p1.x, p1.y);
	scene->transform(p2.x, p2.y);
	edges->Add(new Edge(p1, p2));
	setMinMax(p1, bbdown, bbup);
	setMinMax(p2, bbdown, bbup);
}

/*----------------------------------------------------------------------------*/

BezierSegment::BezierSegment(VECTOR2 c1, VECTOR2 c2, 
							 VECTOR2 e)
							 :Segment(e), c1(c1), c2(c2)
{

}

BezierSegment::~BezierSegment()
{
	
}

VECTOR2 BezierSegment::bezier(VECTOR2 p1, VECTOR2 p2, 
								  VECTOR2 pc, FLOAT t)
{
	VECTOR2 np1;
	vlerp2(&np1, &p1, &pc, t); 
	VECTOR2 np2;
	vlerp2(&np2, &pc, &p2, t); 
	VECTOR2 res;
	return *vlerp2(&res, &np1, &np2, t);
}

VECTOR2 BezierSegment::bezier(VECTOR2 p1, VECTOR2 p2, 
								  VECTOR2 pc1, VECTOR2 pc2, 
								  FLOAT t)
{
	VECTOR2 np1;
	vlerp2(&np1, &p1, &pc1, t); 
	VECTOR2 np2;
	vlerp2(&np2, &pc2, &p2, t);
	VECTOR2 np3;
	vlerp2(&np3, &pc1, &pc2, t);
	VECTOR2 nnp1;
	vlerp2(&nnp1, &np1, &np3, t);
	VECTOR2 nnp2;
	vlerp2(&nnp2, &np3, &np2, t);
	VECTOR2 res;
	return *vlerp2(&res, &nnp1, &nnp2, t);
}

FLOAT dist(VECTOR2 s, VECTOR2 e){
	VECTOR2 t = s-e;
	return vlength2(&t);
}

void BezierSegment::addTo(EdgeList *edges, VECTOR2 start, 
						  Scene *scene, VECTOR2 *bbdown, 
						  VECTOR2 *bbup)
{
	FLOAT epsilon = (FLOAT)0.1, epsilon2 = 0;
	FLOAT temp = 0;
	VECTOR2 p1 = start, p2, end = endPoint;
	scene->transform(p1.x, p1.y);
	scene->transform(end.x, end.y);
	VECTOR2 lastVec = p1;

	VECTOR2 tc1 = c1, tc2 = c2;
	scene->transform(tc1.x, tc1.y);
	scene->transform(tc2.x, tc2.y);

	setMinMax(p1, bbdown, bbup);
	setMinMax(end, bbdown, bbup);
	setMinMax(tc1, bbdown, bbup);
	setMinMax(tc2, bbdown, bbup);

	int nb = (int)((dist(p1,tc1) + dist(tc1, tc2) + dist(tc2, end))/4.0);
	//coef à determiner...
	
	if(nb > 20)
		nb = 20;
	Edge *lastEdge = NULL;
	for ( int i=1; i<nb; i++ ){
		FLOAT t = (FLOAT)i/(FLOAT)nb;
		p2 = bezier(start, endPoint, c1, c2, t);
		scene->transform(p2.x, p2.y);
		Edge *edge = new Edge(p1, p2);

		if(lastEdge != NULL){
			if(((temp = lastEdge->slope - edge->slope) < epsilon 
				&& temp > -epsilon)
				|| dist(p1, p2) < epsilon2 
				|| dist(lastVec, p1) < epsilon2)
			{
				delete edge;
				delete lastEdge;
				edge = new Edge(lastVec, p2);
			}
			else {
				edges->Add(lastEdge);
				lastVec = p1;				
			}
		}
		lastEdge = edge;
		p1 = p2;
	}
	
	Edge *edge = new Edge(p1, end);
	if(lastEdge != NULL){
		if(((temp = lastEdge->slope - edge->slope) < epsilon && temp > -epsilon)
			|| dist(p1, end) < epsilon2 || dist(lastVec, p1) < epsilon2){
			delete edge;
			delete lastEdge;
			edges->Add(new Edge(lastVec, end));
		}
		else{
			edges->Add(lastEdge);
			edges->Add(edge);
		}
	}
	else
		edges->Add(edge);
}

/*----------------------------------------------------------------------------*/

Shape::Shape(Scene *s):scene(s), closed(true), fillType(-1), lineStyle(-1),
ls(NULL), ft(NULL), lc(0)
{
	bbdown = VECTOR2((FLOAT)s->getWidth(),(FLOAT)s->getHeight());
	bbup = VECTOR2(-1,-1);
}

Shape::~Shape(){
	edges.Delete();
}

void Shape::setFillType(int f){
	fillType = f;
	ft = scene->getFillType(fillType);
}

void Shape::setLineStyle(int l){
	lineStyle = l;
	ls = scene->getLineStyle(lineStyle);
	lc = scene->getFillType(ls->getFill())->getColour();
}

void Shape::setStart(VECTOR2 s){
	start = s;
}

void Shape::setEnd(VECTOR2 e){
	end = e;
}

void Shape::addSegment(Segment *s){
	segments.Add(s);
	end = s->getEndPoint();
	if(closed)
		start = end;
}

void Shape::setClosed(bool b){
	closed = b;
}

bool Shape::render(Graphics2D *device, bool line){
	VECTOR2 s = start;
	
	bool drawLine = (lineStyle >= 0) && line;
	bool fillShape = (fillType >= 0);
	FLOAT lineWidth;

	if(edges.Empty() || scene->haveChanged()){
		edges.Delete();

		bbdown = VECTOR2((FLOAT)scene->getWidth(),(FLOAT)scene->getHeight());
		bbup = VECTOR2(-1,-1);

		for(WList<Segment *>::Cel<Segment *> *iter = segments.Begin(); 
			iter != NULL; 
			iter = iter->next)
		{
			(iter->item)->addTo(&edges, s, scene, &bbdown, &bbup);
			s = (iter->item)->getEndPoint();
		}
	}

	if(edgesOrdered.Empty() || scene->haveChanged()){
		edgesOrdered.Clean();
		for(EdgeList::Cel<Edge *> *iter = edges.Begin(); 
			iter != NULL; 
			iter = iter->next)
		{
			Edge *e = iter->item;
			if(e->use && (int)e->yMax - (int)e->yMin != 0)
				addEdgeOrdered(&edgesOrdered, e, true);
		}
	}
	
	if(onScreen()){
		//remplissage de formes
		if(closed && fillShape){
			ft->fillPolygon(device, &edgesOrdered, scene->haveChanged());
			//device->fillPolygon(&edgesOrdered, ft->getColour());
		}

		//dessin des contours
		if(drawLine){
			lineWidth = ls->getWidth();
			scene->transform(lineWidth);
			device->drawLine(&edges, lc, lineWidth);
		}

		return true;
	}
	return false;
}

bool Shape::onScreen(){
	return bbdown.x < scene->getWidth() 
		&& bbup.x >= 0
		&& bbdown.y < scene->getHeight()
		&& bbup.y >= 0;
}

/*----------------------------------------------------------------------------*/

Scene::Scene(int minX, int maxX, int minY, int maxY):
device(NULL), width(0), height(0), changed(false), scale(1), 
rot(0), dx(0), dy(0), minX(minX), maxX(maxX), minY(minY), maxY(maxY), trans(0,0,0) {
}

Scene::~Scene(){
	if(device)
		delete device;
}

void Scene::addFillType(FillType *f){
	fillTypes.Add(f);
}

int Scene::getFillTypeNb(){
	return fillTypes.Length();
}

FillType * Scene::getFillType(int n){
	return fillTypes.AtIndex(n);
}

void Scene::addLineStyle(LineStyle *l){
	lineStyles.Add(l);
}

int Scene::getLineStyleNb(){
	return lineStyles.Length();
}

LineStyle * Scene::getLineStyle(int n){
	return lineStyles.AtIndex(n);
}

void Scene::addShape(Shape *s){
	shapes.AddFirst(s);
}

int Scene::getShapeNb(){
	return shapes.Length();
}

Shape * Scene::getShape(int n){
	return shapes.AtIndex(n);
}

void  Scene::transform(FLOAT &x){
	x = x * dx * scale;
}

void Scene::transform(FLOAT &x, FLOAT &y){
	QUATERNION q;
	rquaternion( &q, &VECTOR(0,0,1), rot);
	MATRIX transformMatrix;
	VECTOR translation = VECTOR(trans.x * width / 512, 
										  trans.y * height / 512,
										  0);
	maffine( &transformMatrix, scale,
			NULL,
			&q,
			&translation);

	x = (x - minX) * dx;
	y = (y - minY) * dy;

	x-=width>>2;
	y-=height>>2;

	VECTOR2 temp(x,y);
	vtransform2( &temp, &temp, &transformMatrix);
	
	x = temp.x + (width>>2);
	y = temp.y + (height>>2);
}

DWORD * Scene::render(DWORD width, DWORD height, DWORD widthbits, 
					  bool line, int *cpt)
{
	if(width != this->width || height != this->height || !device){
		this->width = width;
		this->height = height;
		dx = (width-1) / (FLOAT)(maxX-minX);
		dy = (height-1) / (FLOAT)(maxY-minY);
		bool useAA = false;
		if(device){
			useAA = device->getAAstate();
			delete device;
		}
		device = new Graphics2D(width, height, widthbits, useAA);
		changed = true;
	}
	else{
		device->reset();
	}
	
	for(WList<Shape *>::Cel<Shape *> *iter = shapes.Begin();
		iter != NULL;
		iter = iter->next)
	{
		if((iter->item)->render(device, line) && cpt)
			(*cpt)++;
	}

	changed = false;
	return device->getData();
}

void Scene::setTranslate(VECTOR2 t){
	trans.x = t.x;
	trans.y = t.y;
	changed = true;
}

VECTOR2 Scene::getTranslate(){
	return VECTOR2(trans.x, trans.y); 
}

void Scene::setScale(FLOAT f){
	scale = f;
	changed = true;
}

FLOAT Scene::getScale(){
	return scale;
}

void Scene::setRotation(FLOAT f){
	rot = f;
	changed = true;
}

FLOAT Scene::getRotation(){
	return rot;
}

bool Scene::toggleAA(){
	if(device)
		return device->toggleAA();
	else
		return false;
}

int Scene::getWidth(){
	return width;
}

int Scene::getHeight(){
	return height;
}

bool Scene::haveChanged(){
	return changed;
}

/*----------------------------------------------------------------------------*/

void setMinMax(VECTOR2 v, VECTOR2 *bbdown, VECTOR2 *bbup){
	if(v.x < bbdown->x)
		bbdown->x = v.x;
	if(v.x > bbup->x)
		bbup->x = v.x;
	if(v.y < bbdown->y)
		bbdown->y = v.y;
	if(v.y > bbup->y)
		bbup->y = v.y;
}

/* ************************************************************************ */

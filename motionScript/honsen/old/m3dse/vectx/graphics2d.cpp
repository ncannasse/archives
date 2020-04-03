/* ************************************************************************ */
/*																			*/
/*	M3D-SE Vector Graphics Library											*/
/*	Julien Chevreux															*/
/*	(c)2004 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "graphics2d.h"
#include <memory.h>
#include <math.h>

#define GETDATA(x,y) data[((y) << widthbits) + (x)]

void swap(int&a, int &b) {
	int tmp = a;
	a = b;
	b = tmp;
}

Graphics2D::Graphics2D(int width, int height, int widthbits, bool useAA)
:width(width), height(height), widthbits(widthbits), useAA(useAA)
{
	data = new DWORD[width * height];
	memset(data,0x00ffffff,width * height * sizeof(DWORD));
	
	for(int i=0; i < 256; i++)
		for(int j=0; j < 16; j++)
			colors[(i<<4)+j] = (BYTE)(i * ((j + 1)/17.0));
}

Graphics2D::~Graphics2D()
{
	delete data;
}

DWORD Graphics2D::interpolate(DWORD c1, DWORD c2, FLOAT coef) //c1 est dessus
{	
	FLOAT c1a = (FLOAT)(((c1>>24)&0xff)/255.0);
	DWORD c2a = (DWORD)((c2>>24)&0xff);
	DWORD c1r = (c1>>16)&0xff, c2r = (c2>>16)&0xff;
	DWORD c1g = (c1>>8)&0xff, c2g = (c2>>8)&0xff;
	DWORD c1b = (c1)&0xff, c2b = (c2)&0xff;
	
	coef *= c1a;
	if( coef < 1.0f / 17.0f )
		return c2;
	if( coef >= 1  )
		return c1;
	int c = (int)(coef*17) - 1, cm = 15 - c;
	
	DWORD r = (colors[(c1r<<4)|c] + colors[(c2r<<4)|cm]);
	DWORD g = (colors[(c1g<<4)|c] + colors[(c2g<<4)|cm]);
	DWORD b = (colors[(c1b<<4)|c] + colors[(c2b<<4)|cm]);
	DWORD a = (colors[(0xff<<4)|c] + colors[(c2a<<4)|cm]);
	return (a << 24) | (r << 16) | (g << 8) | b;
}

DWORD Graphics2D::interpolate2(DWORD c1, DWORD c2, FLOAT coef)
{	
	DWORD c1a = (c1>>24)&0xff, c2a = (c2>>24)&0xff;
	DWORD c1r = (c1>>16)&0xff, c2r = (c2>>16)&0xff;
	DWORD c1g = (c1>>8)&0xff, c2g = (c2>>8)&0xff;
	DWORD c1b = (c1)&0xff, c2b = (c2)&0xff;
	
	if( coef <= 0 )
		return c2;
	if( coef >= 1 )
		return c1;
	DWORD r = (DWORD)(c1r*coef + c2r*(1-coef));
	DWORD g = (DWORD)(c1g*coef + c2g*(1-coef));
	DWORD b = (DWORD)(c1b*coef + c2b*(1-coef));
	DWORD a = (DWORD)(c1a*coef + c2a*(1-coef));
	return (a << 24) | (r << 16) | (g << 8) | b;
}

void Graphics2D::drawBLine(FLOAT xa, FLOAT xb, FLOAT ya, FLOAT yb, DWORD color)
{
	int x1 = (int)xa, x2 = (int)xb, y1 = (int)ya, y2 = (int)yb;
	int steep = 1;
	int dx = abs(x2 - x1);
	int sx = ((x2 - x1) > 0) ? 1 : -1;
	int dy = abs(y2 - y1);
	int sy = ((y2 - y1) > 0) ? 1 : -1;

	bool lineAA = true;
	FLOAT coef = 0;
	if( dx == 0 || dy == 0 )
		lineAA = false;
	if( dx == dy )
		coef = 0;

	if( dy > dx ) {
		steep = 0;
		swap(x1, y1);
		swap(dx, dy);
		swap(sx, sy);
	}

	if( dx == 0 ) {
		DWORD *p = data+((y1 << widthbits) + x1);
		if(x1>=0 && y1>=0 && x1<width && y1<height)
			*p = interpolate(color,*p, 1);
		return;
	}

	bool nonAlpha = false;
	if( ((color>>24)&0xff) == 0xff )
		nonAlpha = true;

	int e = (dy << 1) - dx;

	if( steep ) {
		DWORD *p = data+((y1 << widthbits) + x1);
		for(int i = 0; i <= dx; i++) {
			if( x1 >= 0 && y1 >= 0 && x1 < width && y1 < height ) {
				if( useAA && dy != 0 && dx != dy )
					coef = (FLOAT)(e) / (FLOAT)(dx<<1);
				if( nonAlpha && !(useAA && lineAA) )
					*p = color;
				else
					*p = interpolate(color,*p,(coef<0)?1 + coef:1 - coef);

				if( useAA && lineAA ) {
					if( y1 - sy >= 0 && y1 - sy < height ) {
						DWORD *p2 = p - (sy << widthbits);
						*p2 = interpolate(color,*p2,(coef<0)?-coef:0);
					}
					if( y1 + sy >= 0 && y1 + sy < height ) {
						DWORD *p2 = p + (sy << widthbits);
						*p2 = interpolate(color,*p2,(coef<0)?0:coef);
					}
				}
			}
			if( e >= 0 ) {
				y1 += sy;
				p += sy << widthbits;
				e -= (dx << 1);
			}
			x1 += sx;
			p += sx;
			e += (dy << 1);
		}
	} else {	
		DWORD *p = data+((x1 << widthbits) + y1);
		for (int i = 0; i <= dx; i++) {
			if( x1>=0 && y1>=0 && x1<width && y1<height ) {
				if( useAA && dy != 0 && dx != dy )
					coef = (FLOAT)(e) / (FLOAT)(dx<<1);
				if( nonAlpha && !(useAA && lineAA) )
					*p = color;
				else
					*p = interpolate(color,*p,(coef<0)?1 + coef:1 - coef);

				if( useAA && lineAA ) {
					if( y1 - sy >= 0 && y1 - sy < height ) {
						DWORD *p2 = p - sy;
						*p2 = interpolate(color,*p2,(coef<0)?-coef:0);
					}
					if( y1 + sy >= 0 && y1 + sy < height ) {
						DWORD *p2 = p + sy;
						*p2 = interpolate(color,*p2,(coef<0)?0:coef);
					}
				}
			}
			if( e >= 0 ) {
				y1 += sy;
				p += sy;
				e -= (dx << 1);
			}
			x1 += sx;
			p += sx << widthbits;
			e += (dy << 1);
		}
	}

}

bool Graphics2D::addLine(EdgeList *edges, FLOAT xa, FLOAT xb, FLOAT ya, 
						 FLOAT yb, DWORD color, FLOAT w)
{
	if( w == 0 )
		return false;
	w *= 0.5;

	VECTOR2 p1 = VECTOR2(xa, ya), p2 = VECTOR2(xb, yb);
	Edge realLine = Edge(p1, p2);

	if(w <= 0.5 || ( w < 0.8 && (realLine.slope > 1 || realLine.slope < -1))){
		drawBLine(xa, xb, ya, yb, color);
		return false;
	}

	VECTOR2 step, stepN;
	if(realLine.use == false || (int)realLine.yMin == (int)realLine.yMax)
		step = VECTOR2(0, w);
	else {
		step = VECTOR2(-1, realLine.slope);	
		vnormalize2(&stepN, &step); 
		step = stepN * w;
	}
	VECTOR2 pau, pad, pbu, pbd;
	pau = p1 + step;
	pad = p1 - step;
	pbu = p2 + step;
	pbd = p2 - step;

	Edge *temp = new Edge(pau, pad);
	if(temp->use && (int)temp->yMax - (int)temp->yMin != 0)
		addEdgeOrdered(edges, temp, true);
	else
		delete temp;

	temp = new Edge(pbu, pbd);
	if(temp->use && (int)temp->yMax - (int)temp->yMin != 0)
		addEdgeOrdered(edges, temp, true);
	else
		delete temp;

	temp = new Edge(pau, pbu);
	if(temp->use && (int)temp->yMax - (int)temp->yMin != 0)
		addEdgeOrdered(edges, temp, true);
	else
		delete temp;

	temp = new Edge(pad, pbd);
	if(temp->use && (int)temp->yMax - (int)temp->yMin != 0)
		addEdgeOrdered(edges, temp, true);
	else
		delete temp;

	if(realLine.length < 2)
		return false;
	else
		return true;
}


void Graphics2D::drawLine(EdgeList *edges, DWORD color, FLOAT wi)
{
	FOREACH(Edge*,*edges);
		drawLine(item->xyMin, item->xyMax, item->yMin, item->yMax, color, wi);
	ENDFOR;
}


void Graphics2D::drawLine(FLOAT xa, FLOAT xb, FLOAT ya, FLOAT yb, 
						  DWORD color, FLOAT w)
{
	EdgeList edges;
	bool aa = addLine(&edges, xa, xb, ya, yb, color, w);
	fillPolygon(&edges, color, aa);
	edges.Delete();
}


DWORD *Graphics2D::getData()
{
	return data;
}

void Graphics2D::reset()
{
	memset(data,0x00ffffff,width * height * sizeof(DWORD));
}

void Graphics2D::buildActiveList(EdgeList *active, 
								 EdgeList *global, 
								 int floor)
{
	EdgeList::Cel<Edge *> *last = NULL;
	for(EdgeList::Cel<Edge *> *iter = global->Begin(); iter != NULL; ){
		Edge *temp = iter->item;
		if((int)(temp->yMin) > floor)
			break;
		if((int)(temp->yMin) == floor){
			addEdgeOrdered(active, temp, false);
			if(last == NULL){
				global->Remove(temp);//temp est le premier element de la liste
				iter = global->Begin();
			}
			else{
				last->next = iter->next;
				delete iter;
				iter = last;
				iter = iter->next;
				global->ChangeLength(-1);
				if( iter == NULL )
					global->SetQueue(last);
			}
		}
		else{
			last = iter;
			iter = iter->next;
		}
	}
}

void Graphics2D::updateActiveList(EdgeList *active, int floor)
{
	EdgeList::Cel<Edge *> *last = NULL;
	for(EdgeList::Cel<Edge *> *iter = active->Begin(); iter != NULL; ){
		Edge *temp = iter->item;
		if((int)temp->yMax == floor){
			if(last == NULL){
				active->Delete(temp);//temp est le premier element de la liste
				iter = active->Begin();
			}
			else{
				last->next = iter->next;
				delete temp;
				delete iter;
				iter = last;
				iter = iter->next;
				active->ChangeLength(-1);
				if(iter==NULL)
					active->SetQueue(last);
			}
		}
		else {
			temp->update();
			last = iter;
			iter = iter->next;
		}
	}
}

void Graphics2D::fillPolygon(EdgeList *edges, DWORD color,
							 bool shapeAA, WList<EdgeColored *> *edgesColored,
							 WList<control *> *controls, VECTOR2 center)
{
	bool radial = (controls!=NULL);
	bool linear = (edgesColored!=NULL);

	if( edges->Length() == 0 )
		return;

	//copy edges to prevent any modification on originals
	EdgeList globalEdges;
	FOREACH(Edge*,*edges);
		globalEdges.Add(new Edge(item));
	ENDFOR;

	WList<EdgeColored *> globalEdgesColored;
	if( linear ) {
		//copy linear gradient edges to prevent any modification on originals
		FOREACH(EdgeColored*,*edgesColored);
			if( item->use && (int)item->yMin != (int)item->yMax )
				globalEdgesColored.Add(new EdgeColored(item));
		ENDFOR;
	}
	
	WList<EdgeColored *> activeEdgesColored;
	int y = (int)((globalEdges.First())->yMin), yColoredMin;
	if( linear ) {
		//update linear gradient edges to shape start position
		int yStop = y;
		y = yColoredMin = (int)((globalEdgesColored.First())->yMin);
		buildActiveList((EdgeList *)&activeEdgesColored, 
						(EdgeList *)&globalEdgesColored, y); 
		while( y < (int)yStop ) {
			y++;
			updateActiveList((EdgeList *)&activeEdgesColored, y);
			buildActiveList((EdgeList *)&activeEdgesColored, 
							(EdgeList *)&globalEdgesColored, y);
		}
		y = yStop;
	}

	EdgeList activeEdges;
	buildActiveList(&activeEdges, &globalEdges, y); 
    
	bool nonAlpha = false;
	if(((color>>24)&0xff) == 0xff)
		nonAlpha = true;

	int dy2, dx2;
	DWORD *p = data + (y << widthbits);
	//main loop draw the shape line by line from up to down
	while(!activeEdges.Empty() && activeEdges.Length()%2 == 0 && y < height) {
		if( y >= 0 ) {	
			DWORD c1, c2;
			FLOAT c1x, c2x = -1;
			WList<EdgeColored *>::Cel<EdgeColored *> *iterGrad;
			if(linear && activeEdgesColored.Length() > 1){
				iterGrad = activeEdgesColored.Begin();
				c1 = interpolate2(iterGrad->item->maxColor, 
					iterGrad->item->minColor, 
					iterGrad->item->coef);
				c1x = iterGrad->item->xyMin;
				iterGrad = iterGrad->next;
				c2 = interpolate2(iterGrad->item->maxColor, 
					iterGrad->item->minColor, 
					iterGrad->item->coef);
				c2x = iterGrad->item->xyMin;
			}
			else if( radial ) {
				FLOAT d = center.y - y;
				dy2 = (int)(d*d);
			}

			//assuming good construction, edges in activeEdges are multiple of 2 here
			for(EdgeList::Cel<Edge *> *iter = activeEdges.Begin(); 
				iter != NULL; 
				iter = iter->next)
			{
				Edge *current = iter->item;
				iter = iter->next;
				Edge *next = iter->item;

				FLOAT coefxs, coefxe, coefl, coefr, coef, dxl, dxr;
				int start, stop, step1, step2;
				bool useStep1, useStep2;
				start = (int)(current->xyMin);
				stop = (int)((next)->xyMin);

				if(stop < start){
					swap(stop, start);
					Edge *temp = current;
					current = next;
					next = temp;
				}

				if(useAA && shapeAA) {									
					coefxs = (int)(current->xyMin+1) - current->xyMin;
					coefxe = (next)->xyMin - (int)((next)->xyMin);
					step1 = (int)(current->xyMin + current->slope);
					step2 = (int)(next->xyMin + (next)->slope);

					useStep1 = true;
					useStep2 = true;

					if((y+1 == (int)(current->yMax)) 
						|| (current->slope < 1 && current->slope > -1))
						useStep1 = false;
					if((y+1 == (int)(next->yMax)) 
						|| (next->slope < 1 && next->slope > -1))
						useStep2 = false;

					if(step1 < start && useStep1)
						swap(step1, start);
					if(step2 > stop && useStep2)
						swap(step2, stop);
					if(step1 > step2 && useStep1 && useStep2)
						swap(step2, step1);

					int t = 0;
					dxl = 1 / (FLOAT)(((t = (step1 - start)) == 0)?1:t+1);
					dxr =  1 / (FLOAT)(((t = (stop - step2)) == 0)?1:t+1);
					coefl = dxl;
					coefr = 1 - dxr;
				}

				bool AA = false;
				bool startCut = false, stopCut = false;
				if(stop >= height){
					stop = height - 1;
					stopCut = true;
				}
				if(start < 0){
					start = 0;
					startCut = true;
				}

				DWORD *p2 = p + start;
/// BEGIN X LOOP -------------------------------------------------------------------------
				for(int x = start; x <= stop; x++, p2++){
					/// GRADIENTS LINEAR
					if(linear && activeEdgesColored.Length() > 1 
						&& iterGrad != NULL)
					{
						while(x > c2x){
							if(iterGrad->next == NULL)
								c2x = (FLOAT)x;
							else{
								c1 = c2;
								c1x = c2x;
								iterGrad = iterGrad->next;
								c2 = interpolate2(iterGrad->item->maxColor, 
									iterGrad->item->minColor, 
									iterGrad->item->coef);
								c2x = iterGrad->item->xyMin;
							}
						}
						FLOAT dist = c2x - c1x;
						color = interpolate2(c2, c1, 
							(FLOAT)(dist == 0)?1:((x - c1x) / dist));
					}
					/// GRADIENTS RADIAL
					else if(radial){
						dx2 = (int)pow(center.x - x, 2);
						c1 = 0xff000000;
						c2 = controls->First()->color;
						c1x = 0;
						c2x = (FLOAT)controls->First()->coef;
						int dist2 = dx2 + dy2;
						for(WList<control *>::Cel<control *> *iter = controls->Begin(); 
							iter != NULL; 
							iter = iter->next)
						{
							if(iter->item->coef <= dist2) {
								c1 = c2;
								c1x = c2x;
								if(iter->next != NULL){
									c2 = iter->next->item->color;
									c2x = (FLOAT)iter->next->item->coef;
								}
							}
							else
								break;
						}
						FLOAT dist = c2x - c1x;
						FLOAT coefTemp = ((dist2 - c1x) / dist);
						color = interpolate2(c2, c1, 
							(FLOAT)(dist == 0)?1:sqrt((dist2 - c1x) / dist));
					}
					/// MAIN X LOOP
					coef = 1;
					AA = false;
					if(useAA && shapeAA){
						if(useStep1){
							if(x < step1){
								coef *= coefl;
								AA = true;
								coefl += dxl;
							}
						}
						else {
							if(x == start && !startCut){
								coef *= coefxs;
								AA = true;
							}
						}

						if(useStep2){
							if(x > step2){
								coef *= coefr;
								AA = true;
								coefr -= dxr;
							}
						}
						else{
							if(x == stop && !stopCut){
								coef *= coefxe;
								AA = true;
							}
						}
					}

					if(!AA && nonAlpha)
						*p2 = color;
					else if(coef != 0)
						*p2 = interpolate(color, *p2, coef);
				}
/// END X LOOP ---------------------------------------------------------------------------------------
			}
		}
		y++;
		p+=(1 << widthbits);
		updateActiveList(&activeEdges, y);
		buildActiveList(&activeEdges, &globalEdges, y);

		if( linear && y > yColoredMin ) {
			updateActiveList((EdgeList *)&activeEdgesColored, y);
			buildActiveList((EdgeList *)&activeEdgesColored, 
							(EdgeList *)&globalEdgesColored, y);
		}
	}
	activeEdges.Delete();
	globalEdges.Delete();

	activeEdgesColored.Delete();
	globalEdgesColored.Delete();
}

bool Graphics2D::toggleAA()
{
	useAA = !useAA;
	return useAA;
}

bool Graphics2D::getAAstate()
{ 
	return useAA;
}

/* ************************************************************************ */

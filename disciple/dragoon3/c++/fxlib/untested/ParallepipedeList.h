#pragma once 
//#include "mdx.h"

#pragma warning (disable : 4244)
extern MATRIX MID;

struct parallepipede_vertex {
	FLOAT x, y, z;
	DWORD color;
	FLOAT tu,tv;
};

#define D3DFVF_PARALLEPIPEDEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

class ParallepipedeList{
private :
	parallepipede_vertex *vertices;
	FLOAT size;
	
	RTexture *texture;
	VBUF vBuffer;
	MATRIX curview;
	MATRIX matWorld;	
	MATRIX matView;	
	BLEND asrc,adst;
	int nmax;
	int count;
protected:
	
	void setpos( VECTOR *p1, VECTOR *p2) 
	{
		VECTOR4 pos1(p1->x,p1->y,p1->z,1);
		VECTOR4 pos2(p2->x,p2->y,p2->z,1);
		VECTOR4 ptransform1;
		VECTOR4 ptransform2;
		VECTOR4 temp;
		
		
		float d = size;
		
		float alpha;
		VECTOR4 *v;
		
		
		vtransform(&ptransform1,&pos1,&curview);
		vtransform(&ptransform2,&pos2,&curview);
		
		alpha=0.0f;
		v = new VECTOR4(cos(D3DX_PI/2+alpha) * d, sin(D3DX_PI/2+alpha) * d, 0.0f,0);
		
		D3DXVec4Add(&temp, &ptransform1, v);
		vertices[0].x = temp.x;
		vertices[0].y = temp.y;
		vertices[0].z = temp.z;
		vertices[3].x = temp.x;
		vertices[3].y = temp.y;
		vertices[3].z = temp.z;
		
		
		D3DXVec4Subtract(&temp, &ptransform2, v);
		vertices[1].x = temp.x ;	
		vertices[1].y = temp.y;	
		vertices[1].z = temp.z;	
		vertices[5].x = temp.x ;	
		vertices[5].y = temp.y;	
		vertices[5].z = temp.z;	
		
		D3DXVec4Subtract(&temp, &ptransform1, v);
		vertices[2].x = temp.x;	
		vertices[2].y = temp.y;
		vertices[2].z = temp.z;
		
		D3DXVec4Add(&temp, &ptransform2, v);
		vertices[4].x = temp.x;	
		vertices[4].y = temp.y;	
		vertices[4].z = temp.z;
		vertices[0].tu = 0.0f;
		vertices[0].tv = 0.0f;
		vertices[1].tu = 1.0f;
		vertices[1].tv = 1.0f;
		vertices[2].tu = 0.0f;
		vertices[2].tv = 1.0f;
		vertices[3].tu = 0.0f;
		vertices[3].tv = 0.0f;
		vertices[4].tu = 1.0f;
		vertices[4].tv = 0.0f;
		vertices[5].tu = 1.0f;
		vertices[5].tv = 1.0f;
		
		D3DXVec4Add(&ptransform1, &ptransform1, v);
		D3DXVec4Add(&ptransform2, &ptransform2, v);
		alpha += D3DX_PI/2;
		v = new VECTOR4(cos(D3DX_PI/2+alpha) * d, sin(D3DX_PI/2+alpha) * d, 0.0f,0);
		VECTOR4 v1;
		D3DXVec4Subtract(&ptransform1, &ptransform1, v);
		D3DXVec4Subtract(&ptransform2, &ptransform2, v);
		
		D3DXVec4Add(&temp, &ptransform1, v);
		vertices[6].x = temp.x;
		vertices[6].y = temp.y;
		vertices[6].z = temp.z;
		vertices[9].x = temp.x;
		vertices[9].y = temp.y;
		vertices[9].z = temp.z;
		
		
		D3DXVec4Subtract(&temp, &ptransform2, v);
		vertices[7].x = temp.x ;	
		vertices[7].y = temp.y;	
		vertices[7].z = temp.z;	
		vertices[11].x = temp.x ;	
		vertices[11].y = temp.y;	
		vertices[11].z = temp.z;	
		
		D3DXVec4Subtract(&temp, &ptransform1, v);
		vertices[8].x = temp.x;	
		vertices[8].y = temp.y;
		vertices[8].z = temp.z;
		
		
		D3DXVec4Add(&temp, &ptransform2, v);
		vertices[10].x = temp.x;	
		vertices[10].y = temp.y;	
		vertices[10].z = temp.z;
		vertices[6].tu = 0.0f;
		vertices[6].tv = 0.0f;
		vertices[7].tu = 1.0f;
		vertices[7].tv = 1.0f;
		vertices[8].tu = 0.0f;
		vertices[8].tv = 1.0f;
		vertices[9].tu = 0.0f;
		vertices[9].tv = 0.0f;
		vertices[10].tu = 1.0f;
		vertices[10].tv = 0.0f;
		vertices[11].tu = 1.0f;
		vertices[11].tv = 1.0f;
		
		D3DXVec4Subtract(&ptransform1, &ptransform1, v);
		D3DXVec4Subtract(&ptransform2, &ptransform2, v);
		alpha += D3DX_PI/2;
		v = new VECTOR4(cos(D3DX_PI/2+alpha) * d, sin(D3DX_PI/2+alpha) * d, 0.0f,0);
		D3DXVec4Add(&ptransform1, &ptransform1, v);
		D3DXVec4Add(&ptransform2, &ptransform2, v);
		
		
		D3DXVec4Add(&temp, &ptransform1, v);
		vertices[12].x = temp.x;
		vertices[12].y = temp.y;
		vertices[12].z = temp.z;
		vertices[15].x = temp.x;
		vertices[15].y = temp.y;
		vertices[15].z = temp.z;
		
		
		D3DXVec4Subtract(&temp, &ptransform2, v);
		vertices[13].x = temp.x ;	
		vertices[13].y = temp.y;	
		vertices[13].z = temp.z;	
		vertices[17].x = temp.x ;	
		vertices[17].y = temp.y;	
		vertices[17].z = temp.z;	
		
		D3DXVec4Subtract(&temp, &ptransform1, v);
		vertices[14].x = temp.x;	
		vertices[14].y = temp.y;
		vertices[14].z = temp.z;
		
		
		D3DXVec4Add(&temp, &ptransform2, v);
		vertices[16].x = temp.x;	
		vertices[16].y = temp.y;	
		vertices[16].z = temp.z;
		vertices[12].tu = 0.0f;
		vertices[12].tv = 0.0f;
		vertices[13].tu = 1.0f;
		vertices[13].tv = 1.0f;
		vertices[14].tu = 0.0f;
		vertices[14].tv = 1.0f;
		vertices[15].tu = 0.0f;
		vertices[15].tv = 0.0f;
		vertices[16].tu = 1.0f;
		vertices[16].tv = 0.0f;
		vertices[17].tu = 1.0f;
		vertices[17].tv = 1.0f;
		
		D3DXVec4Add(&ptransform1, &ptransform1, v);
		D3DXVec4Add(&ptransform2, &ptransform2, v);
		alpha += 5*D3DX_PI/2 ;
		v = new VECTOR4(cos(D3DX_PI/2+alpha) * d, sin(D3DX_PI/2+alpha) * d, 0.0f,0);
		D3DXVec4Subtract(&ptransform1, &ptransform1, v);
		D3DXVec4Subtract(&ptransform2, &ptransform2, v);
		
		
		
		D3DXVec4Add(&temp, &ptransform1, v);
		vertices[18].x = temp.x;
		vertices[18].y = temp.y;
		vertices[18].z = temp.z;
		vertices[21].x = temp.x;
		vertices[21].y = temp.y;
		vertices[21].z = temp.z;
		
		
		D3DXVec4Subtract(&temp, &ptransform2, v);
		vertices[19].x = temp.x ;	
		vertices[19].y = temp.y;	
		vertices[19].z = temp.z;	
		vertices[23].x = temp.x ;	
		vertices[23].y = temp.y;	
		vertices[23].z = temp.z;	
		
		D3DXVec4Subtract(&temp, &ptransform1, v);
		vertices[20].x = temp.x;	
		vertices[20].y = temp.y;
		vertices[20].z = temp.z;
		
		
		D3DXVec4Add(&temp, &ptransform2, v);
		vertices[22].x = temp.x;	
		vertices[22].y = temp.y;	
		vertices[22].z = temp.z;
		vertices[18].tu = 0.0f;
		vertices[18].tv = 0.0f;
		vertices[19].tu = 1.0f;
		vertices[19].tv = 1.0f;
		vertices[20].tu = 0.0f;
		vertices[20].tv = 1.0f;
		vertices[21].tu = 0.0f;
		vertices[21].tv = 0.0f;
		vertices[22].tu = 1.0f;
		vertices[22].tv = 0.0f;
		vertices[23].tu = 1.0f;
		vertices[23].tv = 1.0f;
	}
	
	void setsize( FLOAT s)
	{
		size=s;
	}
	
	void setcolor( DWORD c ) {
		vertices[0].color = c;	
		vertices[1].color = c;	
		vertices[2].color = c;	
		vertices[3].color = c;	
		vertices[4].color = c;	
		vertices[5].color = c;	
		vertices[6].color = c;	
		vertices[7].color = c;	
		vertices[8].color = c;	
		vertices[9].color = c;	
		vertices[10].color = c;	
		vertices[11].color = c;	
		vertices[12].color = c;	
		vertices[13].color = c;	
		vertices[14].color = c;	
		vertices[15].color = c;	
		vertices[16].color = c;	
		vertices[17].color = c;	
		vertices[18].color = c;	
		vertices[19].color = c;	
		vertices[20].color = c;	
		vertices[21].color = c;	
		vertices[22].color = c;	
		vertices[23].color = c;	
	}
	
	void settextcoord( FLOAT tu1, FLOAT tv1, FLOAT tu2, FLOAT tv2 ) 
	{
		vertices[0].tu = tu1;
		vertices[0].tv = tv1;
		vertices[3].tu = tu1;
		vertices[3].tv = tv1;
		vertices[6].tu = tu1;
		vertices[6].tv = tv1;
		vertices[9].tu = tu1;
		vertices[9].tv = tv1;
		vertices[12].tu = tu1;
		vertices[12].tv = tv1;
		vertices[15].tu = tu1;
		vertices[15].tv = tv1;
		vertices[18].tu = tu1;
		vertices[18].tv = tv1;
		vertices[21].tu = tu1;
		vertices[21].tv = tv1;
		
		vertices[1].tu = tu2;
		vertices[1].tv = tv2;
		vertices[5].tu = tu2;
		vertices[5].tv = tv2;
		vertices[7].tu = tu2;
		vertices[7].tv = tv2;
		vertices[11].tu = tu2;
		vertices[11].tv = tv2;
		vertices[13].tu = tu2;
		vertices[13].tv = tv2;
		vertices[17].tu = tu2;
		vertices[17].tv = tv2;
		vertices[19].tu = tu2;
		vertices[19].tv = tv2;
		vertices[23].tu = tu2;
		vertices[23].tv = tv2;
		
		vertices[2].tu = tu1;
		vertices[2].tv = tv2;
		vertices[8].tu = tu1;
		vertices[8].tv = tv2;
		vertices[14].tu = tu1;
		vertices[14].tv = tv2;
		vertices[20].tu = tu1;
		vertices[20].tv = tv2;
		
		vertices[4].tu = tu2;
		vertices[4].tv = tv1;
		vertices[10].tu = tu2;
		vertices[10].tv = tv1;
		vertices[16].tu = tu2;
		vertices[16].tv = tv1;
		vertices[22].tu = tu2;
		vertices[22].tv = tv1;
		
	}
	
	void setalpha(BLEND src, BLEND dest)
	{
		asrc = src;
		adst = dest;
	}
	
	void NextParticle() {
		vertices+=24;
	}
	
public :
	ParallepipedeList(int nmax, char *filename) : nmax(nmax) {
		DEVICE device= get_device();
		device->CreateVertexBuffer( sizeof(parallepipede_vertex)* nmax * 24,
			D3DUSAGE_WRITEONLY,
			D3DFVF_CROIXVERTEX, 
			D3DPOOL_MANAGED,
			&vBuffer);
		
		
		device->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
		
		device->SetTextureStageState(0,D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		device->SetTextureStageState(0,D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		
		texture = filename?load_texture(filename):NULL;
		asrc=adst=D3DBLEND_ONE;
	}
	
	~ParallepipedeList() {
		free_texture(texture);
		vBuffer->Release();
	}
	
	void Begin ()
	{
		DEVICE device = get_device();
		
		device->GetTransform(D3DTS_WORLD,&matWorld);
		device->GetTransform(D3DTS_VIEW,&matView);
		
		mmult(&curview,&matWorld,&matView);
		vBuffer->Lock(0,sizeof(vertices),(BYTE**)&vertices,0);
		count = 0;
	}
	
	void Draw (VECTOR *p1,VECTOR *p2, FLOAT s, DWORD c)
	{
		if( count == nmax ) {
			MessageBox(NULL,"Too much particles","ERROR",MB_OK | MB_ICONWARNING );
			return;
		}
		
		setsize(s);
		setcolor(c);
		setpos(p1,p2);
		NextParticle();
		count++;
	}
	
	void Draw (VECTOR *p1,VECTOR *p2, FLOAT s, DWORD c, FLOAT tu1, FLOAT tv1,FLOAT tu2, FLOAT tv2)
	{
		if( count == nmax ) {
			MessageBox(NULL,"Too much particles","ERROR",MB_OK | MB_ICONWARNING );
			return;
		}
		
		setsize(s);
		setcolor(c);
		setpos(p1,p2);
		settextcoord(tu1,tv1,tu2,tv2);
		NextParticle();
		count++;
	}

	void trace(FLOAT s, DWORD c, FLOAT tu1, FLOAT tv1,FLOAT tu2, FLOAT tv2,VECTOR *p1, VECTOR *p2, ...)
	{	
		va_list argList;
		
		//Initialize variable argument list
		VECTOR *prec;
		VECTOR *i;
		prec= p1;
		va_start(argList, p1);
		while (1)
		{	
			i=va_arg( argList, VECTOR*);
			if(i==NULL)
				break;
			setsize(s);
			setcolor(c);
			setpos(prec,i);
			settextcoord(tu1,tv1,tu2,tv2);
			prec = i;
			NextParticle();
		}
		
		va_end(argList);
	}

	void trace(FLOAT s, DWORD c, VECTOR *p1, VECTOR *p2, ...)
	{	
		va_list argList;
		
		//Initialize variable argument list
		VECTOR *prec;
		VECTOR *i;
		prec= p1;
		va_start(argList, p1);
		while (1)
		{	
			i=va_arg( argList, VECTOR*);
			if(i==NULL)
				break;
			setsize(s);
			setcolor(c);
			setpos(prec,i);
			prec = i;
			NextParticle();
		}
		
		va_end(argList);
	}
	
	void End ()
	{
		DEVICE device = get_device();
		vBuffer->Unlock();
		
		device->SetTextureStageState(0,D3DTSS_COLOROP,   D3DTOP_MODULATE);
		device->SetTextureStageState(0,D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		device->SetRenderState(D3DRS_LIGHTING, FALSE);
		device->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		device->SetRenderState( D3DRS_SRCBLEND,  asrc );
		device->SetRenderState( D3DRS_DESTBLEND, adst );
		device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		device->SetTransform(D3DTS_WORLD, &MID);
		device->SetTransform(D3DTS_VIEW, &MID);
		
		if( texture != NULL )
			select_texture(texture);
		device->SetStreamSource(0,vBuffer,sizeof(parallepipede_vertex));		
		device->SetVertexShader(D3DFVF_CROIXVERTEX);
		
		device->DrawPrimitive(D3DPT_TRIANGLELIST,0,count*8);
		device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		device->SetRenderState(D3DRS_LIGHTING, TRUE);
		device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		device->SetTransform(D3DTS_WORLD, &matWorld);
		device->SetTransform(D3DTS_VIEW, &matView);
		device->SetTexture(0,NULL);
	}
	
};

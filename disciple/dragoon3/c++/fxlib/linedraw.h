struct line_vertex {
	FLOAT x, y, z;
	DWORD color;
	FLOAT tu,tv;
};

enum LINETYPE { POINTSPRITE, CROIX, PARALLEPIPEDE };

#define D3DFVF_LINEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
	
class LineDraw {
protected:
	line_vertex *vertices;
	RTexture *texture;
	VBUF vBuffer;
	MATRIX curview;
	MATRIX matWorld;
	MATRIX matView;	
	BLEND asrc,adst;
	int nmax;
	int count;

	void setpos( VECTOR *p1, VECTOR *p2, FLOAT size ) {
		VECTOR4 pos1(p1->x,p1->y,p1->z,1);
		VECTOR4 pos2(p2->x,p2->y,p2->z,1);
		VECTOR4 ptransform1;
		VECTOR4 ptransform2;
		VECTOR4 temp;		
		
		vtransform(&ptransform1,&pos1,&curview);
		vtransform(&ptransform2,&pos2,&curview);

		FLOAT alpha = atan2(ptransform2.y-ptransform1.y, ptransform2.x - ptransform1.x);
		VECTOR4 v(cos(D3DX_PI/2+alpha) * size, sin(D3DX_PI/2+alpha) * size, 0.0f,0);

		D3DXVec4Add(&temp, &ptransform1, &v);
		vertices[0].x = temp.x;
		vertices[0].y = temp.y;
		vertices[0].z = temp.z;
		vertices[3].x = temp.x;
		vertices[3].y = temp.y;
		vertices[3].z = temp.z;
		
		
		D3DXVec4Subtract(&temp, &ptransform2, &v);
		vertices[1].x = temp.x;
		vertices[1].y = temp.y;	
		vertices[1].z = temp.z;	
		vertices[5].x = temp.x;	
		vertices[5].y = temp.y;	
		vertices[5].z = temp.z;	
		
		D3DXVec4Subtract(&temp, &ptransform1, &v);
		vertices[2].x = temp.x;	
		vertices[2].y = temp.y;
		vertices[2].z = temp.z;
		
		D3DXVec4Add(&temp, &ptransform2, &v);
		vertices[4].x = temp.x;	
		vertices[4].y = temp.y;	
		vertices[4].z = temp.z;
	}
	
	void setcolor( DWORD c ) {
		vertices[0].color = c;	
		vertices[1].color = c;	
		vertices[2].color = c;	
		vertices[3].color = c;	
		vertices[4].color = c;	
		vertices[5].color = c;	
	}
	
	void settextcoord( FCOORDS *tvs ) {
		if( tvs == NULL ) tvs = &DEFCOORDS;
		vertices[0].tu = tvs->tu;
		vertices[0].tv = tvs->tv;
		vertices[1].tu = tvs->tu2;
		vertices[1].tv = tvs->tv2;
		vertices[2].tu = tvs->tu;
		vertices[2].tv = tvs->tv2;
		vertices[3].tu = tvs->tu;
		vertices[3].tv = tvs->tv;
		vertices[4].tu = tvs->tu2;
		vertices[4].tv = tvs->tv;		
		vertices[5].tu = tvs->tu2;
		vertices[5].tv = tvs->tv2;
	}
	
public :


	LineDraw(int nmax, char *filename) : nmax(nmax) {
		DEVICE device= get_device();
		device->CreateVertexBuffer( sizeof(line_vertex)* nmax * 6,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			D3DFVF_LINEVERTEX, 
			D3DPOOL_MANAGED,
			&vBuffer);
		texture = load_texture(filename);
		asrc = adst = D3DBLEND_ONE;
	}

	~LineDraw() {
		free_texture(texture);
		vBuffer->Release();
	}

	void Begin() {
		DEVICE device = get_device();
		device->GetTransform(D3DTS_WORLD,&matWorld);
		device->GetTransform(D3DTS_VIEW,&matView);
		mmult(&curview,&matWorld,&matView);
		vBuffer->Lock(0,sizeof(vertices),(BYTE**)&vertices,0);
		count = 0;
	}

	void SetAlpha(BLEND src, BLEND dest) {
		asrc = src;
		adst = dest;
	}
	
	void Draw (VECTOR *p1,VECTOR *p2, FLOAT size, DWORD color, FCOORDS *coords ) {
#		ifdef _DEBUG
		if( count == nmax ) {
			MessageBox(NULL,"Too much particles","ERROR",MB_OK | MB_ICONWARNING );
			return;
		}
#		endif
		setcolor(color);
		setpos(p1,p2,size);
		settextcoord(coords);
		vertices+=6;
		count++;
	}

	void End () {
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
		
		device->SetStreamSource(0,vBuffer,sizeof(line_vertex));		
		device->SetVertexShader(D3DFVF_LINEVERTEX);
		device->DrawPrimitive(D3DPT_TRIANGLELIST,0,count*2);
		
		device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		device->SetRenderState(D3DRS_LIGHTING, TRUE);
		device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		device->SetTransform(D3DTS_WORLD, &matWorld);
		device->SetTransform(D3DTS_VIEW, &matView);
		device->SetTexture(0,NULL);
	}

};

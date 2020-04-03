struct sprite_vertex {
	FLOAT x, y, z;
	DWORD color;
	FLOAT tu,tv;
};

#define D3DFVF_SPRITEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

class SpriteDraw {
private :
	sprite_vertex *vertices;
	VBUF vBuffer;
	MATRIX curview;
	MATRIX matWorld;
	MATRIX matView;
	BLEND asrc,adst;
	RTexture *texture;
	int nmax;
	int count;

	void setpos( VECTOR *p, FSIZE *size ) {
		VECTOR4 pos(p->x,p->y,p->z,1);
		VECTOR4 ptransform;
		vtransform(&ptransform,&pos,&curview);
		vertices[0].x = ptransform.x - size->width;
		vertices[0].y = ptransform.y + size->height;
		vertices[0].z = ptransform.z;	
		vertices[1].x = ptransform.x + size->width;
		vertices[1].y = ptransform.y + size->height;
		vertices[1].z = ptransform.z;	
		vertices[2].x = ptransform.x - size->width;
		vertices[2].y = ptransform.y - size->height;
		vertices[2].z = ptransform.z;
		vertices[3].x = ptransform.x - size->width;
		vertices[3].y = ptransform.y - size->height;
		vertices[3].z = ptransform.z;
		vertices[4].x = ptransform.x + size->width;
		vertices[4].y = ptransform.y + size->height;
		vertices[4].z = ptransform.z;	
		vertices[5].x = ptransform.x + size->width;
		vertices[5].y = ptransform.y - size->height;
		vertices[5].z = ptransform.z;
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
		vertices[1].tv = tvs->tv;
		vertices[2].tu = tvs->tu;
		vertices[2].tv = tvs->tv2;
		vertices[3].tu = tvs->tu;
		vertices[3].tv = tvs->tv2;
		vertices[4].tu = tvs->tu2;
		vertices[4].tv = tvs->tv;
		vertices[5].tu = tvs->tu2;
		vertices[5].tv = tvs->tv2;
	}

public : 

	SpriteDraw(int nmax, char *filename) : nmax(nmax) {
		DEVICE device= get_device();
		device->CreateVertexBuffer( sizeof(sprite_vertex)* nmax * 6,
									D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
									D3DFVF_SPRITEVERTEX,
									D3DPOOL_DEFAULT,
									&vBuffer);
		texture = load_texture(filename);
		asrc = adst = D3DBLEND_ONE;
	}

	~SpriteDraw() {
		free_texture(texture);
		vBuffer->Release();
	}

	void Begin () {
		DEVICE device = get_device();
		device->GetTransform(D3DTS_WORLD,&matWorld);
		device->GetTransform(D3DTS_VIEW,&matView);
		mmult(&curview,&matWorld,&matView);		
		vBuffer->Lock(0,sizeof(vertices),(BYTE**)&vertices,D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE);
		count = 0;
	}
	
	void SetAlpha(BLEND src, BLEND dest) {
		asrc = src;
		adst = dest;
	}

	void Draw (VECTOR *p, FSIZE *s, DWORD c, FCOORDS *coords) {
#		ifdef _DEBUG
		if( count == nmax ) {
			MessageBox(NULL,"Too much particles","ERROR",MB_OK | MB_ICONWARNING );
			return;
		}
#		endif
		setcolor(c);
		setpos(p,s);
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
		device->SetStreamSource(0,vBuffer,sizeof(sprite_vertex));
		device->SetVertexShader(D3DFVF_SPRITEVERTEX);
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

//Define a custom vertex for our cylinder
struct cylinder_vertex
{
	FLOAT x, y, z;		//Position of vertex in 3D space
	DWORD color;		//Color
	FLOAT tu, tv;		//Texture coordinates
};

#define CYLINDER_D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define SafeRelease(pInterface) if(pInterface != NULL) {pInterface->Release(); pInterface=NULL;}
//#define SafeDelete(pObject) if(pObject != NULL) {delete pObject; pObject=NULL;}

class CylinderList
{
private:

	VBUF vBuffer;
	RTexture *texture;
	cylinder_vertex* pVertex; 
	MATRIX curview;
	MATRIX matWorld;	
	MATRIX matView;	

	float m_rHeight;
	VECTOR m_rRadius;
	int m_nSegments; 

	DWORD m_dwNumOfVertices;	
	DWORD m_dwNumOfSidePolygons;
	int nmax;
	int count;
	BLEND asrc,adst;

	bool UpdateVertices(VECTOR center, VECTOR dir)
	{
		WORD wVertexIndex = 0;
		int nCurrentSegment;
		
		//Lock the vertex buffer
		vBuffer->Lock(0, 0, (BYTE**)&pVertex, 0);
			
		float rDeltaSegAngle = (2.0f * D3DX_PI / m_nSegments);
		float rSegmentLength = 1.0f / (float)m_nSegments;
		
		//Create the sides triangle strip
		for(nCurrentSegment = 0; nCurrentSegment <= m_nSegments; nCurrentSegment++)
		{
			/*float x0 = center.x + m_rRadius * sinf(nCurrentSegment * rDeltaSegAngle);
			float y0 = center.y ;
			float z0 = center.z + m_rRadius * cosf(nCurrentSegment * rDeltaSegAngle);

			pVertex->x = x0;
			pVertex->y = y0 + (m_rHeight / 2.0f);
			pVertex->z = z0;
			pVertex->color = 0xFFFFFFFF;
			pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
			pVertex->tv = 0.0f;
			pVertex++;
			
			pVertex->x = x0;
			pVertex->y = y0 - (m_rHeight / 2.0f);
			pVertex->z = z0;
			pVertex->color = 0xFFFFFFFF;
			pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
			pVertex->tv = 1.0f;
			pVertex++;*/

			/*float x0 = center.x + m_rRadius * sinf(nCurrentSegment * rDeltaSegAngle);
			float y0 = center.y + m_rRadius * cosf(nCurrentSegment * rDeltaSegAngle);
			float z0 = center.z ;

			pVertex->x = x0;
			pVertex->y = y0;
			pVertex->z = z0 + (m_rHeight / 2.0f);
			pVertex->color = 0xFFFFFFFF;
			pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
			pVertex->tv = 0.0f;
			pVertex++;
			
			pVertex->x = x0;
			pVertex->y = y0;
			pVertex->z = z0 - (m_rHeight / 2.0f);
			pVertex->color = 0xFFFFFFFF;
			pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
			pVertex->tv = 1.0f;
			pVertex++;*/
			/*float x0 = center.x + m_rRadius.z * cosf(nCurrentSegment * rDeltaSegAngle)* dir.y + m_rRadius.y * sinf(nCurrentSegment * rDeltaSegAngle)* dir.z ;
			float y0 = center.x + m_rRadius.x * cosf(nCurrentSegment * rDeltaSegAngle)* dir.z + m_rRadius.z * sinf(nCurrentSegment * rDeltaSegAngle)* dir.x ;
			float z0 = center.x + m_rRadius.y * cosf(nCurrentSegment * rDeltaSegAngle)* dir.x + m_rRadius.x * sinf(nCurrentSegment * rDeltaSegAngle)* dir.y ;

			pVertex->x = x0 + (m_rHeight / 2.0f) * dir.x;
			pVertex->y = y0 + (m_rHeight / 2.0f) * dir.y;
			pVertex->z = z0 + (m_rHeight / 2.0f) * dir.z;
			pVertex->color = 0xFFFFFFFF;
			pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
			pVertex->tv = 0.0f;
			pVertex++;
			
			pVertex->x = x0 - (m_rHeight / 2.0f) * dir.x;
			pVertex->y = y0 - (m_rHeight / 2.0f) * dir.y;
			pVertex->z = z0 - (m_rHeight / 2.0f) * dir.z;
			pVertex->color = 0xFFFFFFFF;
			pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
			pVertex->tv = 1.0f;
			pVertex++;*/

			float x0 = center.x + m_rRadius.x * sinf(nCurrentSegment * rDeltaSegAngle);
			float y0 = center.y ;
			float z0 = center.z + m_rRadius.z * cosf(nCurrentSegment * rDeltaSegAngle);

			pVertex->x = x0;
			pVertex->y = y0 + (m_rHeight / 2.0f);
			pVertex->z = z0;
			pVertex->color = 0xFFFFFFFF;
			pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
			pVertex->tv = 0.0f;
			pVertex++;
			
			pVertex->x = x0;
			pVertex->y = y0 - (m_rHeight / 2.0f);
			pVertex->z = z0;
			pVertex->color = 0xFFFFFFFF;
			pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
			pVertex->tv = 1.0f;
			pVertex++;
		}
		
		vBuffer->Unlock();
		
		return true;
	}

	bool UpdateVertices( VECTOR *p1, VECTOR *p2, VECTOR size) 
	{
		VECTOR4 pos1(p1->x,p1->y,p1->z,1);
		VECTOR4 pos2(p2->x,p2->y,p2->z,1);
		VECTOR4 ptransform1;
		VECTOR4 ptransform2;
		VECTOR4 temp;
		
		float alpha;
		VECTOR4 *v;
		
		
		vtransform(&ptransform1,&pos1,&curview);
		vtransform(&ptransform2,&pos2,&curview);
		alpha = atan2(p2->y-p1->y, p2->x - p1->x);
		v = new VECTOR4(cos(D3DX_PI/2+alpha) * d, sin(D3DX_PI/2+alpha) * d, 0.0f,0);

		WORD wVertexIndex = 0;
		int nCurrentSegment;
		
		//Lock the vertex buffer
		vBuffer->Lock(0, 0, (BYTE**)&pVertex, 0);
			
		float rDeltaSegAngle = (2.0f * D3DX_PI / m_nSegments);
		float rSegmentLength = 1.0f / (float)m_nSegments;

		//Create the sides triangle strip
		for(nCurrentSegment = 0; nCurrentSegment <= m_nSegments; nCurrentSegment++)
		{
			/*float x0 = ptransform1.x + size * sinf(nCurrentSegment * rDeltaSegAngle);
			float y0 = ptransform1.y ;
			float z0 = ptransform1.z + size * cosf(nCurrentSegment * rDeltaSegAngle);*/
			/*float x0 = pos1.x + size * sinf(nCurrentSegment * rDeltaSegAngle);
			float y0 = pos1.y ;
			float z0 = pos1.z + size * cosf(nCurrentSegment * rDeltaSegAngle);*/
			float x0 = pos1.x * sinf(nCurrentSegment * rDeltaSegAngle);
			float y0 = pos1.y ;
			float z0 = pos1.z * cosf(nCurrentSegment * rDeltaSegAngle);

			pVertex->x = x0;
			pVertex->y = y0;
			pVertex->z = z0;
			pVertex->color = 0xFFFFFFFF;
			pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
			pVertex->tv = 0.0f;
			pVertex++;
			
			/*x0 = ptransform2.x + size * sinf(nCurrentSegment * rDeltaSegAngle);
			y0 = ptransform2.y ;
			z0 = ptransform2.z + size * cosf(nCurrentSegment * rDeltaSegAngle);*/
			/*x0 = pos2.x + size * sinf(nCurrentSegment * rDeltaSegAngle);
			y0 = pos2.y ;
			z0 = pos2.z + size * cosf(nCurrentSegment * rDeltaSegAngle);*/
			x0 = pos2.x * sinf(nCurrentSegment * rDeltaSegAngle);
			y0 = pos2.y ;
			z0 = pos2.z * cosf(nCurrentSegment * rDeltaSegAngle);
			pVertex->x = x0;
			pVertex->y = y0;
			pVertex->z = z0;
			pVertex->color = 0xFFFFFFFF;
			pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
			pVertex->tv = 1.0f;
			pVertex++;
		}
		
		vBuffer->Unlock();
		
		return true;
		
	}

	void Cylinder(char *filename, float rHeight = 1.0, VECTOR *rRadius= new VECTOR(1,1,1), int nSegments = 20)
	{
		vBuffer = NULL;
		texture = NULL;
	
		m_rHeight = rHeight;
		m_rRadius.x = rRadius->x;
		m_rRadius.y = rRadius->y;
		m_rRadius.z = rRadius->z;
		m_nSegments = nSegments;

		//Setup counts for this object
		//m_dwNumOfVertices = ((m_nSegments + 1) * 4) + 2;	
		m_dwNumOfVertices = 2 + m_nSegments * 2  ;	
		m_dwNumOfSidePolygons = m_nSegments * 2;

		//Initialize Vertex Buffer
		DEVICE device = get_device();
		device->CreateVertexBuffer(m_dwNumOfVertices * sizeof(cylinder_vertex),
                                               0, CYLINDER_D3DFVF_CUSTOMVERTEX,
                                               D3DPOOL_DEFAULT, &vBuffer);
    
		
		texture = filename?load_texture(filename):NULL;
		//UpdateVertices(0,0);
	}
	
	DWORD Render()
	{
		DEVICE device = get_device();
		device->SetStreamSource(0, vBuffer, sizeof(cylinder_vertex));
		device->SetVertexShader(CYLINDER_D3DFVF_CUSTOMVERTEX);
		
		
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		device->SetRenderState(D3DRS_LIGHTING, FALSE);
		device->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		device->SetRenderState( D3DRS_SRCBLEND,  asrc );
		device->SetRenderState( D3DRS_DESTBLEND, adst );
		device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		if(texture != NULL)
		{
			//A texture has been set. We want our texture to be shaded based
			//on the current light levels, so used D3DTOP_MODULATE.
			select_texture(texture);
			device->SetTextureStageState(0,D3DTSS_COLOROP,   D3DTOP_MODULATE);
			device->SetTextureStageState(0,D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
			device->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
			device->SetTextureStageState(0,D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
			device->SetTextureStageState(0,D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		}
		else
		{
			//No texture has been set
		}
		
		
		//Render polygons from vertex buffer - Sides
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_dwNumOfSidePolygons);
		device->SetTexture(0, 0);
		
		//Return the number of polygons rendered
		return m_dwNumOfSidePolygons;
	}

	void CylinderRelease()
	{
		//SafeRelease(texture);
		SafeRelease(vBuffer);
	}


public:
	CylinderList(int nmax, char *filename) : nmax(nmax) {
		Cylinder(filename,200.0f,&VECTOR(100.0f,50.0f,25.0f),20);
		asrc=adst=D3DBLEND_ONE;
	}
	
	~CylinderList() {
		free_texture(texture);
		vBuffer->Release();
	}
	
	void Begin ()
	{
		DEVICE device = get_device();
		
		device->GetTransform(D3DTS_WORLD,&matWorld);
		device->GetTransform(D3DTS_VIEW,&matView);
		
		mmult(&curview,&matWorld,&matView);
		count = 0;
	}
	
	void Draw ()
	{
		if( count == nmax ) {
			MessageBox(NULL,"Too much particles","ERROR",MB_OK | MB_ICONWARNING );
			return;
		}
		VECTOR center;
		VECTOR dir;
		VECTOR pos1(30,30,30);
		VECTOR pos2(100,100,100);
		VECTOR rayon(30,30,30);
		float size = 10.0f;

		center.x= 1;
		center.y= 1;
		center.z= 1;
		dir.x=0;
		dir.y=1;
		dir.z=0;

		
		UpdateVertices(&pos1,&pos2,rayon);
		Render();
		/*center.x= 100;
		center.y= 100;
		center.z= 100;
		dir.x=10;
		dir.y=0;
		dir.z=0;

		UpdateVertices(center,dir);
		Render();*/
	}
	
	void Draw (VECTOR *p1,VECTOR *p2, FLOAT s, DWORD c, FLOAT tu1, FLOAT tv1,FLOAT tu2, FLOAT tv2)
	{
		if( count == nmax ) {
			MessageBox(NULL,"Too much particles","ERROR",MB_OK | MB_ICONWARNING );
			return;
		}
		
		/*setsize(s);
		setcolor(c);
		setpos(p1,p2);
		settextcoord(tu1,tv1,tu2,tv2);
		NextParticle();*/
		count++;
	}
	
	void End ()
	{
	}

};
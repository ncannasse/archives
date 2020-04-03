/* **************************************************************************** */
/*																				*/
/*   M3D Camera control															*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#include <m3d.h>

namespace M3D {
/* ---------------------------------------------------------------------------- */

namespace Infos {
	
	#define LINEFORMAT (D3DFVF_XYZ | D3DFVF_DIFFUSE)

	typedef struct ILINE {
		VECTOR pos;
		DWORD color;
		ILINE() { }
		ILINE( VECTOR pos ) : pos(pos), color(0xFFFF00FF) { }
		ILINE( VECTOR pos, DWORD color ) : pos(pos), color(color) { }
	} ILINE;

	struct InfoLine {
		ILINE *lines;
		int icount;
		InfoLine( int n ) : icount(n), lines(new ILINE[n]) { };
		~InfoLine() { delete lines; };
	};

	WList<InfoLine*> items;

	void DrawLine( DWORD color, VECTOR *a, VECTOR *b ) {
		InfoLine *i = new InfoLine(2);
		ILINE *data = i->lines;
		*data++ = ILINE(*a,color);
		*data++ = ILINE(*b,color);
		items.Add(i);
	}

	void DrawBox( DWORD color, VECTOR *_bmin, VECTOR *_bmax, MATRIX *m ) {
		VECTOR4 bmin(_bmin->x,_bmin->y,_bmin->z,1);
		VECTOR4 bmax(_bmax->x,_bmax->y,_bmax->z,1);
		
		vtransform(&bmin,&bmin,m);
		vtransform(&bmax,&bmax,m);

		InfoLine *i = new InfoLine(24);
		ILINE *data = i->lines;

		// min -> max (x or y or z)
		*data++ = ILINE(VECTOR(bmin.x,bmin.y,bmin.z),color);
		*data++ = ILINE(VECTOR(bmin.x,bmin.y,bmax.z),color);

		*data++ = ILINE(VECTOR(bmin.x,bmin.y,bmin.z),color);
		*data++ = ILINE(VECTOR(bmin.x,bmax.y,bmin.z),color);

		*data++ = ILINE(VECTOR(bmin.x,bmin.y,bmin.z),color);
		*data++ = ILINE(VECTOR(bmax.x,bmin.y,bmin.z),color);

		// max -> min (x or y or z )
		*data++ = ILINE(VECTOR(bmax.x,bmax.y,bmax.z),color);
		*data++ = ILINE(VECTOR(bmax.x,bmax.y,bmin.z),color);

		*data++ = ILINE(VECTOR(bmax.x,bmax.y,bmax.z),color);
		*data++ = ILINE(VECTOR(bmax.x,bmin.y,bmax.z),color);

		*data++ = ILINE(VECTOR(bmax.x,bmax.y,bmax.z),color);
		*data++ = ILINE(VECTOR(bmin.x,bmax.y,bmax.z),color);

		// remains
		*data++ = ILINE(VECTOR(bmin.x,bmin.y,bmax.z),color);
		*data++ = ILINE(VECTOR(bmin.x,bmax.y,bmax.z),color);

		*data++ = ILINE(VECTOR(bmin.x,bmin.y,bmax.z),color);
		*data++ = ILINE(VECTOR(bmax.x,bmin.y,bmax.z),color);

		*data++ = ILINE(VECTOR(bmin.x,bmax.y,bmin.z),color);
		*data++ = ILINE(VECTOR(bmax.x,bmax.y,bmin.z),color);

		*data++ = ILINE(VECTOR(bmin.x,bmax.y,bmin.z),color);
		*data++ = ILINE(VECTOR(bmin.x,bmax.y,bmax.z),color);

		*data++ = ILINE(VECTOR(bmax.x,bmin.y,bmin.z),color);
		*data++ = ILINE(VECTOR(bmax.x,bmax.y,bmin.z),color);

		*data++ = ILINE(VECTOR(bmax.x,bmin.y,bmin.z),color);
		*data++ = ILINE(VECTOR(bmax.x,bmin.y,bmax.z),color);
		
		items.Add(i);
	}

	void Render() {

		/* Done by FX 
		if( world.IsModified() ) {
			device->SetTransform(D3DTS_VIEW,Camera::view.Get());
			device->SetTransform(D3DTS_PROJECTION,matproj.Get());
			device->SetTransform(D3DTS_WORLD,transform.Get());
		}*/

		if( !items.Length() )
			return;

		device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		device->SetRenderState( D3DRS_ZENABLE, FALSE );

		device->SetVertexShader( LINEFORMAT );
		device->SetTexture(0,NULL);
		
		FOREACH(InfoLine*,items);
			device->DrawPrimitiveUP(D3DPT_LINELIST,item->icount/2,item->lines,sizeof(ILINE));
			delete item;
		ENDFOR;

		device->SetRenderState( D3DRS_ZENABLE, TRUE );
		device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		items.Clean();
	}

};

/* ---------------------------------------------------------------------------- */
}; // namespace
/* **************************************************************************** */
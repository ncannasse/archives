/* ************************************************************************ */
/*																			*/
/*	M3D-SE Resource Loading and Cache										*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "m3d.h"
#include "m3d_context.h"
#include "m3d_2d.h"

namespace M3D {

/* ------------------------------------------------------------------------ */

HBITMAP FillBitmap( int tsize, HDC dc, HFONT hf, DWORD **ret_ptr, FLOAT *ret_height, FLOAT coords[] ) {
	FLOAT fsize = (FLOAT)tsize;

	BITMAPINFO bmi;
	ZeroMemory( &bmi.bmiHeader, sizeof(BITMAPINFOHEADER) );
	bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       =  tsize;
	bmi.bmiHeader.biHeight      = -tsize;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;

	DWORD *bmp_ptr;
	HBITMAP bmp = CreateDIBSection(dc,&bmi,DIB_RGB_COLORS,(VOID**)&bmp_ptr,NULL,0);
	HGDIOBJ old_bmp = SelectObject(dc,bmp);
	HGDIOBJ old_font = SelectObject(dc,hf);
	SetMapMode(dc,MM_TEXT);
	SetTextColor(dc,RGB(255,255,255));
	SetBkColor(dc,0x00000000);
	SetBkMode(dc, TRANSPARENT);
	SetTextAlign(dc,TA_TOP);

	DWORD *bmp_tmp_ptr;
	bmi.bmiHeader.biWidth = 128;
	bmi.bmiHeader.biHeight = -100;
	HBITMAP tmp_bmp = CreateDIBSection(dc,&bmi,DIB_RGB_COLORS,(VOID**)&bmp_tmp_ptr,NULL,0);

	int x = 1;
	int y = 1;
	int xp,yp;
	unsigned char c = ' ';
	SIZE size, csize;
	RECT r = {0,0,0,0};
	GetTextExtentPoint32(dc,(const char *)&c,1,&size);
	DWORD height = size.cy;
	*ret_height = (FLOAT)height;

	for(c=0;c<0xFF;c++) {
		if( c < 32 ) {
			size.cx = 0;
			size.cy = 0;
		}
		else {
			r.left--;
			r.top--;
			r.right++;
			r.bottom++;
			SelectObject(dc,tmp_bmp);
			FillRect(dc,&r,(HBRUSH)GetStockObject(BLACK_BRUSH));
			ExtTextOut(dc,30,30,ETO_OPAQUE,NULL,(const char*)&c, 1, NULL );
			r.left = 100;
			r.top = 100;
			r.right = 0;
			r.bottom = 0;
			for(xp=0;xp<100;xp++)
				for(yp=0;yp<100;yp++) 
					if( bmp_tmp_ptr[xp|(yp<<7)] ) {
						if( r.left > xp )
							r.left = xp;
						if( r.right < xp )
							r.right = xp;
						if( r.top > yp )
							r.top = yp;
						if( r.bottom < yp )
							r.bottom = yp;
					}
			size.cx = r.right - r.left + 1;
			size.cy = r.bottom - r.top + 1;
		}

			
		if( !GetTextExtentPoint32(dc,(const char *)&c,1,&csize) ) {
			csize.cx = 0;
			csize.cy = 0;
		}

		if( c == ' ' && (size.cx <= 0 || size.cy <= 0) ) {
			size.cx = csize.cx;
			size.cy = csize.cy;
		}
	
		if( size.cx < 0 ) {
			size.cx = 0;
			r.left = 30;
		}

		if( x + size.cx + 1 > tsize ) {
			x = 1;
			y += height+1;
		}

		coords[c*6] = x/fsize;
		coords[(c*6)+1] = y/fsize;
		coords[(c*6)+2] = (x+size.cx)/fsize;
		coords[(c*6)+3] = (y+height)/fsize;
		coords[(c*6)+4] = (FLOAT)csize.cx;
		coords[(c*6)+5] = (FLOAT)(size.cx?(30 - r.left):SKIP_CHAR);

		if( size.cx > 0 ) {
			SelectObject(dc,bmp);
			ExtTextOut(dc,x+(30-r.left),y,0,NULL,(const char*)&c,1,NULL);
		}

		x += (size.cx>0)?(size.cx+1):0;
	}
	*ret_ptr = bmp_ptr;

	SelectObject(dc,old_bmp);
	SelectObject(dc,old_font);
	DeleteObject(tmp_bmp);

	return bmp;
}

/* ------------------------------------------------------------------------ */

Font::Font( TEXTURE t ) : t(t) { 
}

/* ------------------------------------------------------------------------ */

Font::~Font() {
	t->Release();
}

/* ------------------------------------------------------------------------ */

Font *
Font::New( Context *c, const char *face, int size, int flags ) {
	TEXTURE t;
	int tsize = (size<=16)?256:512;

	if( M3DERROR(c->device->CreateTexture(tsize,tsize, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &t),"Failed to allocate texture for Font") )
		return NULL;

	HDC dc = CreateCompatibleDC(NULL);
	int height = -MulDiv(size,GetDeviceCaps(dc,LOGPIXELSY),72);
	HFONT hf = CreateFontA(height,0,0,0,(flags&1)?FW_BOLD:FW_NORMAL,flags&2,FALSE,FALSE,DEFAULT_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,VARIABLE_PITCH,face);
	if( hf == NULL ) {
		t->Release();
		DeleteDC(dc);
		M3DDEBUG("Failed to create font "+(WString)face);
		return NULL;
	}

	DWORD *bmp_ptr;
	Font *f = new Font(t);
	HBITMAP bmp = FillBitmap(tsize,dc,hf,&bmp_ptr,&f->height,f->coords);
	HGDIOBJ old_bmp = SelectObject(dc,bmp);

	D3DLOCKED_RECT d3dlr;
	t->LockRect( 0, &d3dlr, 0, 0 );
	BYTE* pDstRow = (BYTE*)d3dlr.pBits;
	DWORD* pDst;
	BYTE bAlpha;
	int x,y;
	for( y=0; y < tsize; y++ ) {
		pDst = (DWORD*)pDstRow;
		for( x=0; x < tsize; x++ ) {
			bAlpha = (BYTE)(*bmp_ptr++ & 0xFF);
			if( bAlpha == 0 )
				*pDst++ = 0;
			else
				*pDst++ = (bAlpha << 24) | 0xFFFFFF;
		}
		pDstRow += d3dlr.Pitch;
	}
	t->UnlockRect(0);

	SelectObject(dc,old_bmp);	
	DeleteObject(bmp);
	DeleteObject(hf);
	DeleteDC(dc);	
	return f;
}

/* ------------------------------------------------------------------------ */
}; // namespace
/* ************************************************************************ */

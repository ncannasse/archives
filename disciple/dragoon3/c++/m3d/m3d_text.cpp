/* **************************************************************************** */
/*																				*/
/*   M3D Text display															*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#include <m3d.h>

/* ---------------------------------------------------------------------------- */

void FillTexture( TEXTURE t, int height, const char *face, bool bold, FLOAT coords[][4] ) {    
    DWORD* pBitmapBits;
    BITMAPINFO bmi;
    ZeroMemory( &bmi.bmiHeader, sizeof(BITMAPINFOHEADER) );
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  256;
    bmi.bmiHeader.biHeight      = -256;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount    = 32;

    HDC     hDC       = CreateCompatibleDC( NULL );
    HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
                                          (VOID**)&pBitmapBits, NULL, 0 );
    INT nHeight    = -MulDiv( height, GetDeviceCaps(hDC, LOGPIXELSY), 72 );
	DWORD fsize	  = bold ? FW_BOLD : FW_NORMAL;
    HFONT font    = CreateFontA( nHeight, 0, 0, 0, fsize, FALSE,
                          FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          VARIABLE_PITCH, face );

    SetMapMode( hDC, MM_TEXT );
    SelectObject( hDC, hbmBitmap );
    SelectObject( hDC, font );
    SetTextColor( hDC, RGB(255,255,255) );
    SetBkColor(   hDC, 0x00000000 );
    SetTextAlign( hDC, TA_TOP );

    DWORD x = 0;
    DWORD y = 0;
    TCHAR str[2];
    SIZE size;
	str[1] = 0;
    for( TCHAR c=32; c<127; c++ )
    {
        str[0] = c;
        GetTextExtentPoint32( hDC, str, 1, &size );

        if( (DWORD)(x+size.cx+1) > 256 )
        {
            x  = 0;
            y += size.cy+1;
        }

        ExtTextOut( hDC, x+0, y+0, ETO_OPAQUE, NULL, str, 1, NULL );

        coords[c-32][0] = x*1.0/256;
        coords[c-32][1] = y*1.0/256;
        coords[c-32][2] = (x+size.cx)*1.0/256;
        coords[c-32][3] = (y+size.cy)*1.0/256;

        x += size.cx+1;
    }

    D3DLOCKED_RECT d3dlr;
    t->LockRect( 0, &d3dlr, 0, 0 );
    BYTE* pDstRow = (BYTE*)d3dlr.pBits;
    WORD* pDst16;
    BYTE bAlpha;
    for( y=0; y < 256; y++ ) {
        pDst16 = (WORD*)pDstRow;
        for( x=0; x < 256; x++ ) {
            bAlpha = (BYTE)((pBitmapBits[256*y + x] & 0xff) >> 4);
            if (bAlpha > 0)				
                *pDst16++ = (bAlpha << 12) | 0x0fff;
            else
                *pDst16++ = 0x0000;
        }
        pDstRow += d3dlr.Pitch;
    }
    t->UnlockRect(0);
    DeleteObject( hbmBitmap );
	DeleteObject( font);
    DeleteDC( hDC );	
}

/* **************************************************************************** */

namespace M3D {
/* ---------------------------------------------------------------------------- */

	extern int width,height;

namespace Text {

	struct FONTVERTEX {
		VECTOR4 pos;
		DWORD color;
		FLOAT tu,tv;
	};

#define FONT_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define FONT_BUF_SIZE 2048

/* ---------------------------------------------------------------------------- */

	struct TText {
		WString txt;
		int txtlen;
		FLOAT width;
		DWORD color;
		VECTOR pos;
		bool center;
		Font *f;
		TText( Font *f ) : f(f), pos(0,0,0.1f), color(0xFFFFFFFF), center(false), txtlen(0) {
		}

		~TText() {
			FX::Lerp::Cancel(&color);
			FX::Lerp::Cancel(&pos);
		}
	};

/* ---------------------------------------------------------------------------- */

	static WArray<Font*> fonts;
	static WList<TText*> texts;

	struct Font {
		int fnb;
		int height;
		TEXTURE font;
		VBUF buffer;
		FLOAT coords[128-32][4];
		WArray<TText*> texts;
		
		Font( TEXTURE t, VBUF buf ) : font(t), buffer(buf), height(0.01) { }
		
		void Release() {
			if( font ) {
				font->Release();
				font = NULL;
			}
			if( buffer)  {
				buffer->Release();
				buffer = NULL;
			}
		}

		~Font() {
			Release();
			texts.Delete();
		}
	};

/* ---------------------------------------------------------------------------- */

	void add_sorted( TText *t ) {
		int p = 0;
		FLOAT z = t->pos.z;
		FOREACH(TText*,texts);
			if( item->pos.z < z ) {
				texts.Insert(t,p);
				return;
			}	
			p++;
		ENDFOR;
		texts.Add(t);	
	}

/* ---------------------------------------------------------------------------- */

	TText *get_text( int ref ) {
		Font *f = fonts[ref&0xFF];
		return f->texts[ref>>8];
	}

/* ---------------------------------------------------------------------------- */

	TData::TData( int ref ) : ref(ref) {
	}

	int
	TData::Ref() {
		return ref;
	}

	void
	TData::SetPos( VECTOR *v) {
		TText *t = get_text(ref);
		FLOAT z = t->pos.z;
		t->pos = *v;
		if( z != v->z ) {
			texts.Remove(t);
			add_sorted(t);
		}	
	}

	void
	TData::SetDest( FLOAT x, FLOAT y, FLOAT time ) {
		TText *t = get_text(ref);
		FX::Lerp::VLerp(&t->pos,&t->pos,&VECTOR(x,y,t->pos.z),time);
	}

	void
	TData::SetString( const char *str ) {
		TText *t = get_text(ref);
		t->txt = str;
		t->txtlen = t->txt.length();

		FLOAT width = 0;
		int k;
		for(k=0;k<t->txtlen;k++) {
			FLOAT *coords = t->f->coords[t->txt[k]-32];
			FLOAT tx1 = coords[0];
			FLOAT tx2 = coords[2];
			FLOAT w = (tx2-tx1) * 256;
			width += w;
		}
		t->width = width;
	}

	void
	TData::SetColor( DWORD color ) {
		TText *t = get_text(ref);
		t->color = color;
	}

	void
	TData::SetCenter( bool c ) {
		TText *t = get_text(ref);
		t->center = c;
	}

/* ---------------------------------------------------------------------------- */

	Font *CreateFont( int height, const char *face, bool bold ) {
		TEXTURE t;
		VBUF buf;
		if( !M3DCHECK(device->CreateTexture(256,256, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &t)) )
			return NULL;
		if( !M3DCHECK(device->CreateVertexBuffer(FONT_BUF_SIZE*sizeof(FONTVERTEX),D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,FONT_FVF,D3DPOOL_DEFAULT,&buf)) ) {
			t->Release();
			return NULL;
		}
		Font *font = new Font(t,buf);
		font->fnb = fonts.Length();
		FillTexture(t,height,face,bold, font->coords);
		fonts.Add(font);
		return font;
	}

	void FreeFont( Font *f ) {
		if( fonts.Length() > f->fnb )
			fonts[f->fnb] = NULL;
		delete f;
	}

/* ---------------------------------------------------------------------------- */

	TData Create( Font *f ) {		
		TText *t = new TText(f);
		// TODO : add @ an empty place
		int ref = f->fnb+(f->texts.Length()<<8);
		f->texts.Add(t);
		add_sorted(t);
		return TData(ref);
	}

	void Remove( int ref ) {
		Font *f = fonts[ref&0xFF];
		TText *t = f->texts[ref>>8];
		f->texts[ref>>8] = NULL;
		delete t;
		texts.Remove(t);
	}

/* ---------------------------------------------------------------------------- */

	int Render() {
		int tcount = 0;
		int i,j,k;
		int flen = fonts.Length();
		FONTVERTEX *ptr;

		device->SetVertexShader( FONT_FVF );
        device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        device->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
        device->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );

		for(i=0;i<flen;i++) {
			Font *f = fonts[i];
			if( !f )
				continue;
			int pos = 0;
			device->SetTexture(0,f->font);
			device->SetStreamSource(0,f->buffer,sizeof(FONTVERTEX));
			int tlen = f->texts.Length();
			f->buffer->Lock(0,0,(BYTE**)&ptr, D3DLOCK_DISCARD);
			for(j=0;j<tlen;j++) {
				TText *t = f->texts[j];
				if( !t || !t->txtlen ) 
					continue;
				int locklen = t->txtlen*6;

				if( pos+t->txtlen > FONT_BUF_SIZE ) {
		            f->buffer->Unlock();
					device->DrawPrimitive(D3DPT_TRIANGLELIST,0,pos/3);
					f->buffer->Lock( 0, 0, (BYTE**)&ptr, D3DLOCK_DISCARD);
					pos = 0;
				}

				FLOAT sx = (t->pos.x+1)*(width/2);
				FLOAT px = sx;
				FLOAT dy = f->coords[0][3]*256;
				FLOAT py = (t->pos.y+1)*(height/2);
				FLOAT z = t->pos.z;

				if( t->center )
					px -= t->width/2;

				for(k=0;k<t->txtlen;k++)
					if( t->txt[k] == '\n' ) {
						px = sx;
						py += dy;
						locklen -= 6;
					}
				else
				{


					FLOAT *coords = f->coords[t->txt[k]-32];
			        FLOAT tx1 = coords[0];
				    FLOAT ty1 = coords[1];
					FLOAT tx2 = coords[2];
					FLOAT ty2 = coords[3];
					FLOAT w = (tx2-tx1) * 256;
					FLOAT h = (ty2-ty1) * 256;

					ptr->pos = VECTOR4(px,py,z,1);
					ptr->color = t->color;
					ptr->tu = tx1;
					ptr->tv = ty2;
					ptr++;

					ptr->pos = VECTOR4(px,py-h,z,1);
					ptr->color = t->color;
					ptr->tu = tx1;
					ptr->tv = ty1;
					ptr++;

					ptr->pos = VECTOR4(px+w,py,z,1);
					ptr->color = t->color;
					ptr->tu = tx2;
					ptr->tv = ty2;
					ptr++;

					ptr->pos = VECTOR4(px+w,py,z,1);
					ptr->color = t->color;
					ptr->tu = tx2;
					ptr->tv = ty2;
					ptr++;

					ptr->pos = VECTOR4(px,py-h,z,1);
					ptr->color = t->color;
					ptr->tu = tx1;
					ptr->tv = ty1;
					ptr++;

					ptr->pos = VECTOR4(px+w,py-h,z,1);
					ptr->color = t->color;
					ptr->tu = tx2;
					ptr->tv = ty1;
					ptr++;

					px+=w;
				}
						
				tcount += locklen/3;
				pos += locklen;
			}
			if( pos > 0 )
				device->DrawPrimitive(D3DPT_TRIANGLELIST,0,pos/3);
		}
        device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		return tcount;
	}

/* ---------------------------------------------------------------------------- */

	void Close() {
		int i;
		for(i=0;i<fonts.Length();i++) {
			Font *f = fonts[i];
			if( f ) {
				f->Release();
				f->texts.Delete();
			}
		}		
		fonts.Clean();
	}

};

/* ---------------------------------------------------------------------------- */
}; // namespace
/* **************************************************************************** */

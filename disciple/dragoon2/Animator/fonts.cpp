/* ******************************************************************** */
/*	fonts.cpp															*/
/* ******************************************************************** */
#include "fonts.h"
#include "stdio.h"
#include "stdlib.h"
#include "dlib.h"
#include "resource.h"
#include "erreurs.h"

const char *ltrstxt="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!*,()+\\.-':;%<>=?\"/@#ιθ";
extern HANDLE hInst;
CFonts *Fonts = NULL;

/* ******************************************************************** */

void
CFonts::LoadFont( DWORD ID )
{
	LPSET tmp;
	LPFONT l;
	DWORD pos, pos2;
	int i,p=0,x,y,len;
	HBITMAP h;
	HDC dc;
	SIZE sz;
    HDC hdcImage = CreateCompatibleDC(NULL);
	BITMAP bm;


	h = LoadBitmap( (HINSTANCE)hInst, MAKEINTRESOURCE( ID ) );
    SelectObject (hdcImage, h);
    GetObject(h, sizeof(bm), &bm);
	
	sz.cx = 654; sz.cy = 10;
	Ecran.Nouvelle_SurfSys( &tmp, sz.cx , sz.cy );
	tmp->surf->GetDC( &dc );
	i = StretchBlt(dc, 0, 0, tmp->pitch,tmp->y,hdcImage, 0, 0,tmp->x,tmp->y, SRCCOPY);
	tmp->surf->ReleaseDC( dc );
    DeleteDC(hdcImage);
	
	if( i == 0 )
		return;

	l = new TFONT;
	l->y = (BYTE)sz.cy;
	l->data = new LPBYTE[ LTR_SIZE ];
	l->x = new BYTE [ LTR_SIZE + 1];

	for(i=0;i<LTR_SIZE; i++) 
	{
		l->data[i] = NULL;
		l->x[i] = 0;
	}
	l->x[LTR_SIZE] = 0;
 	Ecran.Lock( tmp );
	
	i=0;
	while( p<sz.cx && i<LTR_SIZE )
	{
		len=0;
		while( tmp->buf[p+len] != Ecran.Couleur[0][0][63] && p+len<sz.cx )
			len++;
		l->data[i] = new BYTE[ len*l->y ];
		l->x[i] = len;
		pos = p;
		pos2 = 0;
		for(y=0; y<l->y; y++, pos+=tmp->pitch-len )
			for(x=0; x<len; x++,pos++,pos2++)
				l->data[i][pos2] = ( tmp->buf[ pos ] == Ecran.CFond ) ? 255:Ecran.FindColor[ tmp->buf[ pos ] ].R;
		p+=len+1;
		i++;
	}

	Ecran.UnLock( tmp );
	Ecran.Free_Surf( &tmp );

	l->Suivant = NULL;
	if( fonts == NULL )
		fonts = l;
	else
	{
		LPFONT t = fonts;
		while (t->Suivant != NULL)
			t=t->Suivant;
		t->Suivant = l;
	}
}

/* ******************************************************************** */

RECT
CFonts::wrtext( LPCSTR text,LPSET set,long xpos,long ypos,WORD col, FONT f)
{
	int i=0,x,y,j;
	int x1,x2,y1,y2;
	long sxpos = xpos<0?0:xpos;
	DWORD p,p2;
	WORD *buf;
	WORD color[64];
	LPFONT l = fonts;

	if( xpos > set->x || ypos > set->y )
		return Rect(0,0,0,0);

	for(j=0;j<f; j++ )
	{
		l = l->Suivant;
		if( l == NULL )
			return Rect(0,0,0,0);
	}	

	if( ypos+l->y < 0 )
		return Rect(0,0,0,0);


	color[0]=0;
	for(j=1; j<64; j++)
		color[j]=Ecran.Couleur[ (Ecran.FindColor[col].R*j)>>6 ]
							  [ (Ecran.FindColor[col].G*j)>>6 ]
							  [ (Ecran.FindColor[col].B*j)>>6 ];

	y1 = ( ypos < 0 )?0-ypos:0;
	y2 = ( ypos+l->y > set->y )?set->y-ypos:l->y;

	Ecran.Lock( set );
	buf = set->buf+xpos+ypos*set->pitch;
		
	while (text[i])
	{

		for(j=0;j<LTR_SIZE,ltrstxt[j]!=text[i];j++)
			;
		if( j<LTR_SIZE && l->x[j] )
		{
			x1 = ( xpos < 0 )?0-xpos:0;
			x2 = ( xpos + l->x[j] > set->x )?set->x-xpos:l->x[j];

			p=x1+y1*set->pitch;p2=x1+y1*l->x[j];

			for(y=y1;y<y2;y++,p+=set->pitch-x2+x1,p2+=l->x[j]-x2+x1)
				for(x=x1;x<x2;x++,p++,p2++)
					if( l->data[j][p2] != 255 )
						buf[p]=color[l->data[j][p2]];

		}
		else
			j = LTR_SIZE-1;

		xpos+=l->x[j]+2;
		buf+=l->x[j]+2;
		if( xpos > set->x )
		{
			Ecran.UnLock( set );
			if( ypos + l->y > set->y )
				ypos = set->y-l->y;
			if( ypos < 0 )
				ypos = 0;
			return Rect( sxpos, ypos, set->x, ypos+l->y );
		}
		i++;
	}

	Ecran.UnLock( set );
	xpos -=2;
	if( ypos + l->y > set->y )
		ypos = set->y-l->y;
	if( ypos < 0 )
		ypos = 0;
	if( xpos < 0 )
		xpos = 0;
	return Rect( sxpos, ypos, xpos, ypos+l->y );
}


/* ******************************************************************** */

CFonts::CFonts()
{
	fonts = NULL;
	LTR_SIZE = (int)strlen( ltrstxt );
	LoadFont(IDB_FONT);
}


/* ******************************************************************** */

CFonts::~CFonts()
{
	LPFONT s;
	int i;

	while( fonts != NULL )
	{
		s = fonts->Suivant;
		delete fonts->x;
		for( i=0; i<LTR_SIZE; i++)
			delete fonts->data[i];
		delete fonts->data;
		delete fonts;
		fonts = s;
	}
}


/* ******************************************************************** */

void
CFonts::Affiche( LPCSTR Chaine, long x, long y, WORD Col, FONT f )
{
	Ecran.InvalidZone( wrtext( Chaine, Ecran.Get_Set( CACHEE ), x, y, Col, f) );
}

/* ******************************************************************** */

void
CFonts::Ecrit( LPCSTR Chaine, long x, long y, WORD Col, FONT f )
{
	
	Ecran.ActuZone( wrtext( Chaine, Ecran.Get_Set( DECOR ), x, y, Col, f ) );
}

/* ******************************************************************** */

void
CFonts::Print( LPCSTR Chaine, long x, long y, WORD Col, FONT f )
{
	wrtext( Chaine, Ecran.Get_Set( CACHEE ), x, y, Col, f );
}

/* ******************************************************************** */

void
CFonts::AfficheNb( long nb, long x, long y, WORD Col, FONT f )
{
	char dst[25];
	_ltoa( nb, dst, 10 );
	Ecran.InvalidZone( wrtext( dst, Ecran.Get_Set( CACHEE ), x, y, Col, f ) );
}

/* ******************************************************************** */

void
CFonts::EcritNb( long nb, long x, long y, WORD Col, FONT f )
{
	char dst[25];
	_ltoa( nb, dst, 10 );
	Ecran.ActuZone( wrtext( dst, Ecran.Get_Set( DECOR ), x, y, Col,f ) );
}

/* ******************************************************************** */

void
CFonts::PrintNb( long nb, long x, long y, WORD Col, FONT f )
{
	char dst[25];
	_ltoa( nb, dst, 10 );
	wrtext( dst, Ecran.Get_Set( CACHEE ), x, y, Col,f );
}

/* ******************************************************************** */
/* fin du fichier fonts.cpp		                                        */


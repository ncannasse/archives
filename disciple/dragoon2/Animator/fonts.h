/* ******************************************************************** */
/* fonts.h		                                                        */
/* ******************************************************************** */

#pragma once

#include "graphisme.h"

enum FONT {
	STANDARD,
	CLASSIC,
	FANTASTIC
};

typedef struct TFONT {
	LPBYTE *data;
	BYTE y;
	BYTE *x;
	struct TFONT *Suivant;
} TFONT, *LPFONT;


class CFonts {

	LPFONT fonts;
	int	   LTR_SIZE;

	RECT wrtext( LPCSTR text,LPSET set,long xpos,long ypos,WORD col, FONT f);

	public:

	void LoadFont ( DWORD ID );
	
	// affichage a l'Ecran ( temp. )
	void Affiche  ( LPCSTR Chaine, long x, long y, WORD Col, FONT f=STANDARD );
	void AfficheNb( long nb, long x, long y, WORD Col, FONT f=STANDARD );

	// ecrit dans le decor ( definitif )
	void Ecrit	  ( LPCSTR Chaine, long x, long y, WORD Col, FONT f=STANDARD );
	void EcritNb  ( long nb, long x, long y, WORD Col, FONT f=STANDARD );

	// affiche a l'Ecran sans Invalider
	void Print	  ( LPCSTR Chaine, long x, long y, WORD Col, FONT f=STANDARD );
	void PrintNb  ( long nb, long x, long y, WORD Col, FONT f=STANDARD );

	CFonts ();
	~CFonts();
};

extern CFonts *Fonts;
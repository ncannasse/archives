/* ******************************************************************** */
/* sprites.h	                                                        */
/* ******************************************************************** */
#pragma once
/* ******************************************************************** */
#include "graphisme.h"
/* ******************************************************************** */

typedef struct TSprite {
	DWORD ID;
	DWORD Tick;
	RECT Zone;
	LPSET Set;
	struct TSprite *Suivant;
} TSprite, *LPSprite;

/* ******************************************************************** */

class CSprites {

	LPSprite Liste_Sprites[256];

public:

	// fonctions de base
	void		Affiche	( long X, long Y, DWORD ID	);
	void		Print	( long X, long Y, DWORD ID	);
	void		Ecrit	( long X, long Y, DWORD ID	);
	void		Libere	( void						);
	void		Clean	( DWORD Time				);
	LPSprite	Ajoute	( DWORD ID					);

	// affichage en transparence
	void		AfficheTr	( long X, long Y, DWORD ID, int Trans );
	// affichage inversé
	void		AfficheFlip	( long X, long Y, DWORD ID	);
	// affichage avec une couleur fixe
	void		AfficheAlpha( long X, long Y, DWORD ID, WORD Col );
	// affichage avec Zoom
	void		AfficheZoom	( long X, long Y, DWORD ID, int ZFactor );
	// affichage dans une "pseudo" fenetre
	void		AfficheClip	( long X, long Y, DWORD ID, RECT r );
	void		PrintClip	( long X, long Y, DWORD ID, RECT r );

	RECT		GetZone		( DWORD ID	);

	// constructeur & destructeur
	CSprites();
	~CSprites();
};

/* ******************************************************************** */

extern CSprites *Sprites;

/* ******************************************************************** */
// fin de sprites.h
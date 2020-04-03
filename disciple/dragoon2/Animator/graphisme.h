/* ******************************************************************** */
/* graphisme.h                                                          */
/* ******************************************************************** */
#pragma once
/* ******************************************************************** */
#include <ddraw.h>
#include <stdio.h>
/* ******************************************************************** */

#define RES_X		800
#define RES_Y		600
#define NBCOULEURS	16
#define ZONE_TOUT	0xFFFFFF00

/* ******************************************************************** */

typedef void (*RefreshPROC)( void * );

enum MODE {  VIDEO, SYSTEM };
enum SURFACE {  VISIBLE, CACHEE, DECOR, SYS };

typedef struct {
	WORD *buf;
	DWORD pitch;
	LPDIRECTDRAWSURFACE surf;
	long x,y;
	MODE mode;
} SET, *LPSET;

typedef struct LOCK {
	LPDIRECTDRAWSURFACE surface;
	DDSURFACEDESC surface_desc;
	WORD *buf;
} LOCK, *LPLOCK;

typedef struct ZONES {
	RECT R;
	struct ZONES *Suivant;
} ZONES, *LPZONES;

/* ******************************************************************** */
static int GetInfo	( FILE *f, void *R	  );
static int Charge	( FILE *f, void *Set  );
/* ******************************************************************** */

class CDDBasic {
	
	LPDIRECTDRAW lpDD;

	// initialisation des couleurs
	void Init_Cols ( void );

	DWORD Ticks;
	DWORD Frames;

	// surfaces
	LPSET Visible;
	LPSET Cachee;
	LPSET Decor;
	LPSET Sys;

	// zones d'invalidation
	LPZONES Zo1,Zo2;
	LPZONES ZoTr1,ZoTr2;
	LPZONES ZoNowTr;
	LPZONES ZoSys;

public:

	DWORD CurTicks;
	HWND  HWnd;

	BYTE FPS;					// images par seconde

	// Couleurs
	WORD Couleur[64][64][64];	// tableau de conv. RGB->16bit
	struct FINDCOLOR {
		BYTE R,G,B;
	} FindColor[65536]; // tableau de conv. 16bit->RGB

	WORD CFond;					// couleur de fond

	void MsgErreur( LPCSTR Msg );

	// Gestion de DD
	void Initialise_DD	 ( HWND hWnd );						// initialise tout
	void Ferme_DD		 ( void );							// liberation DD & mem
	void Nouvelle_Surface( LPSET *un_set,DWORD X,DWORD Y );	// cree une surface Video
	void Nouvelle_SurfSys( LPSET *un_set,DWORD X,DWORD Y );	// cree une surface System
	void Free_Surf		 ( LPSET *un_set );					// libere une surface

	// Recuperation d'Informations
	LPSET Get_Set	  ( SURFACE Num );	// renvoie le SET demandé
	RECT  Get_Info    ( DWORD ID	);	// renvoie les dimentions d'une image

	// Fonctions de base
	void Affiche		 ( void );												// affiche la surface cachee courante
	void AfficheSys		 ( long X, long Y, LPSET Src, LPRECT RSrc );
	void Affiche_Sprite  ( long X, long Y, LPSET Src, RECT RSrc );				// affiche le sprite dans la surface cachee
	void Affiche_SpriteF ( long X, long Y, LPSET Src, RECT RSrc );				// affiche le sprite avec Flipping

	void Affiche_SpriteEx( LPSET Dst, long X, long Y, LPSET Src, RECT RSrc );	// affiche un sprite dans un SET
	void Copie_Zone		 ( LPSET Dst, long X, long Y, LPSET Src, RECT RSrc );	// copie une zone d'un SET a un autre
	void Charge_Ecran	 ( DWORD ID );											// charge une image dans la surface cachee courante
	void Charge_EcranEx  ( DWORD ID,LPSET un_Set,DWORD Pos_X, DWORD Pos_Y );	// charge une image dans un SET

	void InvalidAll		 ( void );		// rafraichissement général
	void ActuZone		 ( RECT R );	// invalude R pour les 2 surfaces principales
	void InvalidZone	 ( RECT R );	// invalide R pour la surface cachee courante
	void InvalidZoneTr	 ( RECT R );
	void InvalidZoneNoTr ( RECT R );

	void   DelZoneTr	 ( LPRECT R );  
	LPRECT AjouteZoneTr	 ( RECT R );	

	// Fonctions Additionnelles
	void Lock     ( LPSET set );					// Lock le SET
	void UnLock   ( LPSET set );					// DeLock le SET
	void Lock	  ( LPLOCK L  );					// Lock une surface
	void UnLock   ( LPLOCK L  );					// DeLock une surface
	void Fill	  ( LPSET Set, RECT R, WORD Col );	// Remplis une Zone
	RECT Clip	  ( RECT R, LPSET Set );			// Reduit R aux dimentions du SET
	
	void SnapShot  ( void );						// fait un snapshot en PCX
	void CleanZones( LPZONES *lz );					// vide la liste des Zones

	// Rafraichissement automatique
	RefreshPROC Rafraichir;				// fonction a appeller
	void *		RParam;					// parametre a passer
	static void Nombre ( void *Nb );	// affiche un nombre a haut a gauche

	// fonction pour les images
	int  Charge_PCX ( FILE *f,LPSET un_Set,DWORD Pos_X, DWORD Pos_Y, DWORD Taille);
	int  Charge_GIF ( FILE *f,LPSET un_Set,DWORD Pos_X, DWORD Pos_Y, DWORD Taille);	
	void Get_PCX_info( FILE *f, LPRECT R );
	void Get_GIF_info( FILE *f, LPRECT R );

	void WaitFrames  ( int FrameNumber   );
	void WaitTime    ( DWORD TimeCount   );

	// Constructeur & Destructeur
	CDDBasic ();
	~CDDBasic ();
};

/* ******************************************************************** */

extern CDDBasic Ecran;
extern RECT		RWin;

/* ******************************************************************** */

bool Intersect(RECT R1, RECT R2);

/* ******************************************************************** */

RECT Rect(long left, long top, long right, long bottom);

/* ******************************************************************** */
/* fin du fichier graphisme.h                                           */

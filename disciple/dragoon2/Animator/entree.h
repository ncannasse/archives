/* ******************************************************************** */
/* entree.h                                                             */
/* ******************************************************************** */
/* - souris                                                             */
/* - clavier                                                            */
/* ******************************************************************** */
#pragma once
/* ******************************************************************** */
#include <dinput.h>
/* ******************************************************************** */
#define BOUTON_GAUCHE 1   // Bouton gauche relach�
#define BOUTON_DROIT  2   // Bouton droit  relach�
#define BOUTON_MILIEU 4   // Bouton du milieu relach�
#define CLIQUE_LONG_G 8   // Bouton gauche avec clique long
#define CLIQUE_LONG_D 16  // Bouton droit avec clique long
#define CLIQUE_LONG_M 32  // Bouton milieu avec clique long
#define GAUCHE_ENFONCE 64 // Bouton gauche appuy�
#define DROIT_ENFONCE  128// Bouton droit  appuy�
#define MILIEU_ENFONCE 256// Bouton du milieu appuy�
#define AUTRE          512// Autres ?
/* ******************************************************************** */
#define BUFFERSIZE 32 // nb c'�l�ment de donn�e pouvant etre heberg� par 
					  // le tampon
#define Touche(x)   Ctrl->Touches[x]==2
#define KeyPress(x) Ctrl->Touches[x]
/* ******************************************************************** */
class CInterface {
	
	// Variables priv�es
	LPDIRECTINPUT lpdi; // Objet directinput comme pour directdraw
	LPDIRECTINPUTDEVICE lpdid;// Interface du peripherique
	LPDIRECTINPUTDEVICE lpdidc;// Interface du peripherique le clavier
	DIPROPDWORD dipdw;
	DIPROPDWORD dipdwc;
	DIDEVICEOBJECTDATA rgdod[BUFFERSIZE]; // Stock les donn�es lu dans le buffer
	DIDEVICEOBJECTDATA rgdodc[BUFFERSIZE]; // Stock les donn�es lu dans le buffer
	DWORD dwItems; // Nb d'element stocker
	DWORD dwItemsc; // Nb d'element stocker
	long Sensibilite_G;
	long Sensibilite_D;
	long Sensibilite_M;
	long Taux_Sensibilite;
	long Etat_Bouton_G;  // Etat du bouton gauche
	long Etat_Bouton_D;  // Etat du bouton droit
	long Etat_Bouton_M;  // Etat du bouton milieu
	long Ecart_X;// Calcule l'ecart a conserver lors du deplacement d'un
	long Ecart_Y;// controle par rapport au coordonn�es de la souris
	void Initialise_DI ( void ); // Initialise les obj directinput
	void Ferme_DI (void);	    // Rend la main a windows
	void Traite_Touches_Speciales( BYTE i ); // Touches speciales??	

public:

	long Etat_Boutons;   // Etat de tous les boutons
	long X
		,Y;          // Coordonn�es de la souris
	char Touches [ 256 ]; // Tableau contenant le "scancode" des touches

	// Fonctions de base
	void Mise_a_Jour ( void ); // Met � jour les coord
	void Affiche ( void );     // Affiche toute l'interface
	void Lire_Clavier ( void ); // Lit les donn�es du clavier
	void Actu ( void );


	char *InputLn	 ( char *defval, int MaxChar, long X, long Y, DWORD font, WORD Color );
	void CaptureChar ( char *chain, int MaxChar );
	// Constructeur
	CInterface (void);
	// Destructeur
	~CInterface (void);
};

/* ******************************************************************** */

extern CInterface* Ctrl;

/* ******************************************************************** */
/* fin du fichier entree.h                                              */
/* ******************************************************************** */
/* entree.cpp                                                           */
/* ******************************************************************** */
#include "entree.h"
#include "erreurs.h"
#include "graphisme.h"
#include "decor_fake.h"
/* ******************************************************************** */

CInterface* Ctrl = NULL;
extern HANDLE	 hInst;
extern HWND		 hWnd;
extern MSG		 msg;
LPCSTR cnv = "1234567890-=  azertyuiop()  qsdfghjklm'é  wxcvbn,./ *                789-456+1230.";

/* ******************************************************************** */

void
CInterface::Actu()
{
	Mise_a_Jour();
	Lire_Clavier();
	Affiche();
}

/* ******************************************************************** */

void
CInterface::Affiche()
{
/*	LPSET s = Ecran.Get_Set( CACHEE );
	Ecran.Lock( s );

	s->buf[ X+Y*s->pitch ] = Ecran.Couleur[63][63][63];

	Ecran.UnLock( s );
*/
}

/* ******************************************************************** */

void
CInterface::Initialise_DI () 
{
	// Cree un objet direct input changer 0x0300 en 0x0500
	if ((rDX = DirectInputCreate((HINSTANCE)hInst,0x500,&lpdi,NULL))!=DI_OK)
		if ((rDX = DirectInputCreate((HINSTANCE)hInst,0x300,&lpdi,NULL))!=DI_OK)
			DirectXFatalError("CInterface/InitDI : CreateInst",rDX);

	// Cree l'interface de la souris
	if ((rDX = lpdi->CreateDevice(GUID_SysMouse, &lpdid, NULL))!= DI_OK)
			DirectXFatalError("CInterface/InitDI : CreateMDevice",rDX);

	// Cree l'interface du clavier
	if ((rDX = lpdi->CreateDevice(GUID_SysKeyboard, &lpdidc, NULL))!= DI_OK)
			DirectXFatalError("CInterface/InitDI : CreateKDevice",rDX);

	// Définition du format de donnée de la souris
	if ((rDX = lpdid->SetDataFormat(&c_dfDIMouse))!= DI_OK)
			DirectXFatalError("CInterface/InitDI : SetMDataFormat",rDX);

	// Définition du format de donnée du clavier
	if ((rDX = lpdidc->SetDataFormat(&c_dfDIKeyboard))!= DI_OK)
			DirectXFatalError("CInterface/InitDI : SetKDataFormat",rDX);

	// Choisit le mode de non cooperation :) (rien a foutre des autres)
	if ((rDX = lpdid->SetCooperativeLevel(hWnd,DISCL_EXCLUSIVE | 
											  DISCL_FOREGROUND)) != DI_OK)
			DirectXFatalError("CInterface/InitDI : SetMCoopLevel",rDX);

	// Choisit le mode de cooperation on est obligé
	if ((rDX = lpdidc->SetCooperativeLevel(hWnd,DISCL_NONEXCLUSIVE | 
											  DISCL_FOREGROUND)) != DI_OK)
			DirectXFatalError("CInterface/InitDI : SetKCoopLevel",rDX);
	
	// Defini le tampon pour la souris
	dipdw.diph.dwSize = sizeof ( DIPROPDWORD );
	dipdw.diph.dwHeaderSize = sizeof ( DIPROPHEADER );
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = BUFFERSIZE;

	// attache le tampon à la souris
	if ((rDX =lpdid->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)) != DI_OK)
			DirectXFatalError("CInterface/InitDI : SetMProperties",rDX);

	// Defini le tampon pour le clavier
	dipdwc.diph.dwSize = sizeof ( DIPROPDWORD );
	dipdwc.diph.dwHeaderSize = sizeof ( DIPROPHEADER );
	dipdwc.diph.dwObj = 0;
	dipdwc.diph.dwHow = DIPH_DEVICE;
	dipdwc.dwData = BUFFERSIZE;

	// attache le tampon au clavier
	if ((rDX =lpdidc->SetProperty(DIPROP_BUFFERSIZE, &dipdwc.diph)) != DI_OK)
			DirectXFatalError("CInterface/InitDI : SetKProperties",rDX);

	// On acquiere le mulot pour nous
	if ((rDX = lpdid->Acquire())!= DI_OK)
			DirectXFatalError("CInterface/InitDI : AcquireM",rDX);

	// On acquiere le pad a 101 touches
	if ((rDX = lpdidc->Acquire())!= DI_OK)
			DirectXFatalError("CInterface/InitDI : AcquireK",rDX);

	ZeroMemory( Touches, 256 );
}


/* ******************************************************************** */

void
CInterface::Ferme_DI (void) 
{	
	if( lpdid )
	{
		lpdid->Unacquire();
		lpdid->Release();
		lpdid=NULL;
	}
	if( lpdidc )
	{
		lpdidc->Unacquire();
		lpdidc->Release();
		lpdidc=NULL;
	}
	if( lpdi )
	{
		lpdi->Release();
		lpdi=NULL;
	}
} // fin de Ferme_DI



/* ******************************************************************** */

CInterface::CInterface (void)
{
	X = RES_X >>1;
	Y = RES_Y >>1;

	Etat_Bouton_G = 0;
	Etat_Bouton_D = 0;
	Etat_Bouton_M = 0;
	Sensibilite_G = 1;
	Sensibilite_D = 1;
	Sensibilite_M = 1;
	Taux_Sensibilite = 2^60;
	Initialise_DI ();
}

/* ******************************************************************** */

CInterface::~CInterface (void)
{
	Ferme_DI ();
}

/* ******************************************************************** */

void
CInterface::Mise_a_Jour ()
{
	Etat_Bouton_G <<= 1;
	Etat_Bouton_D <<= 1;
	Etat_Bouton_M <<= 1;
	if (Etat_Bouton_G == Taux_Sensibilite)
	{
		Etat_Boutons = CLIQUE_LONG_G;
		Etat_Bouton_G = 0;
	}
	if (Etat_Bouton_D == Taux_Sensibilite)
	{
		Etat_Boutons = CLIQUE_LONG_D;
		Etat_Bouton_D = 0;
	}
	if (Etat_Bouton_G == Taux_Sensibilite)
	{
		Etat_Boutons = CLIQUE_LONG_M;
		Etat_Bouton_M = 0;
	}
	dwItems = BUFFERSIZE;
	if ( (rDX = lpdid->GetDeviceData (sizeof (DIDEVICEOBJECTDATA),rgdod,&dwItems,0 ) )!= DI_OK)
	{
		switch (rDX)
		{
		case DIERR_NOTACQUIRED :
		case DIERR_INPUTLOST :
			lpdid->Acquire ();
			break;
		case DI_BUFFEROVERFLOW : 
			break;
		default :
			DirectXError("CInterface/Mise a Jour", rDX );
			break;
		}
	}
	else
	{
		for (DWORD dummy = 0; dummy < dwItems; dummy++)
		{
			switch (rgdod[dummy].dwOfs)
			{
			case DIMOFS_BUTTON0 : 
				// Si le bit de poids fort de l'octet de poids faible
				// Est à un le bouton est enfoncé 
				if (rgdod[dummy].dwData & 0x80 )
				{
					Etat_Bouton_G = Sensibilite_G;
					Etat_Boutons = GAUCHE_ENFONCE;
				}
				else
				{
					// si ce n'est pas un clique long
					if ( Etat_Boutons == GAUCHE_ENFONCE )
					{
						Etat_Boutons = BOUTON_GAUCHE;
						Etat_Bouton_G = 0;
					}
					if ( Etat_Boutons == AUTRE )
						Etat_Boutons = 0;
				}
				break;
			// Si c'est le second
			case DIMOFS_BUTTON1 : 
				if (rgdod[dummy].dwData & 0x80 )
				{
					Etat_Bouton_D = Sensibilite_D;
					Etat_Boutons = DROIT_ENFONCE;
				}
				else 
				{
					// si ce n'est pas un clique long
					if ( Etat_Boutons == DROIT_ENFONCE )
					{
						Etat_Boutons = BOUTON_DROIT;
						Etat_Bouton_D = 0;
					}
				}
				break;
			// Si c'est le troisieme
			case DIMOFS_BUTTON2 :
				if (rgdod[dummy].dwData & 0x80 )
				{
					Etat_Bouton_M = Sensibilite_M;
					Etat_Boutons = MILIEU_ENFONCE;
				}
				else 
				{
					// si ce n'est pas un clique long
					if ( Etat_Boutons == MILIEU_ENFONCE )
					{
						Etat_Boutons = BOUTON_MILIEU;
						Etat_Bouton_M = 0;
					}
				}
				break;
			// Si c'est un deplacement selon l'axe des X
			case DIMOFS_X :
				X += rgdod[dummy].dwData;
				break;
			// Si c'est un deplacement selon l'axe des Y
			case DIMOFS_Y :
				Y += rgdod[dummy].dwData;
				break;
			}
		}
	}

/*	if ( X < 0 )
		X = 0;
	if ( X >= RES_X )
		X = RES_X - 1;
	if ( Y < 0 )
		Y = 0;
	if ( Y >= RES_Y )
		Y = RES_Y - 1;*/
}

/* ******************************************************************** */

void
CInterface::Lire_Clavier()
{
	dwItemsc = BUFFERSIZE;
	DWORD Temp;
	for (Temp = 0; Temp < 256; Temp++)
		switch( Touches[Temp] )
		{
		case -1:
			break;
		case 2:
			Touches[Temp] = -1;
			break;
		default:
			Touches[Temp] = 0;
			break;
	}

	if ( (rDX = lpdidc->GetDeviceData (sizeof (DIDEVICEOBJECTDATA),rgdodc,&dwItemsc,0 ) )!= DI_OK)
	{
		switch (rDX)
		{
		case DIERR_NOTACQUIRED :
		case DIERR_INPUTLOST :
			lpdidc->Acquire ();
			break;
		case DI_BUFFEROVERFLOW:
			break;
		default :
			DirectXError("CInterface/Lire_Clavier",rDX);
			break;
		}
	}
	else
	{
		for (DWORD dummy = 0; dummy < dwItemsc; dummy++)
		{
			Temp = rgdodc[dummy].dwOfs;
			if (rgdodc[dummy].dwData & 0x80)
			{
				if( Touches[Temp] == 2 )
					Touches[Temp] = -1;
				else
					Touches[Temp] = 2;
			}
			else
				Touches[Temp] = 1;
		}
	}
}

/* ******************************************************************** */

char *
CInterface::InputLn( char *defval, int MaxChar, long X, long Y, DWORD font, WORD Color )
{
	char txt[1024];
	int pos = 0;
	LPSET s;

	Ecran.Nouvelle_SurfSys( &s, RES_X, RES_Y );
	Ecran.Copie_Zone(s,0,0,Ecran.Get_Set(DECOR),Le_Decor->DecorVisible);
	Ecran.Copie_Zone(Ecran.Get_Set(DECOR),Le_Decor->DecorVisible.left,
											Le_Decor->DecorVisible.top,
											Ecran.Get_Set(CACHEE),
											Rect(0,0,RES_X,RES_Y));
	if( defval != NULL )
	{
		strcpy(txt,defval);
	    pos = strlen(txt);
		txt[pos+1] = 0;
	}
	else
	{
		txt[0] = 0;
		txt[1] = 0;
	}

    while (1)
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( !GetMessage( &msg, NULL, 0, 0 ) )
				break;
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}

		else
		{
			Lire_Clavier();
//	Fonts->Affiche(txt,X,Y,Color,(FONT)font );
			Ecran.Affiche();
			if( txt[pos] == '.' )
				txt[pos] = 0;
			else
				txt[pos] = '.';

			if( Touche( DIK_ESCAPE ) )
			{
				Ecran.Copie_Zone(Ecran.Get_Set(DECOR),Le_Decor->DecorVisible.left,
														Le_Decor->DecorVisible.top,
														s,
														Rect(0,0,RES_X,RES_Y) );
				Ecran.Free_Surf( &s );
				return NULL;
			}
			if( Touche( DIK_RETURN ) )
			{
				char *n = new char[pos+1];
				txt[pos]=0;
				strcpy( n, txt );
				Ecran.Copie_Zone(Ecran.Get_Set(DECOR),Le_Decor->DecorVisible.left,
														Le_Decor->DecorVisible.top,
														s,
														Rect(0,0,RES_X,RES_Y) );
				Ecran.Free_Surf( &s );
				return n;
			}
			
			if( Touche( DIK_BACK ) && pos > 0)
			{
				txt[pos]=0;
				pos -- ;
			}

			if( pos < MaxChar )
			{
				int k;
				for( k= 0; k< (int)strlen(cnv); k++)
					if( Touche( k + 2 ) && ( cnv[k] !=' ' || k + 2 == DIK_SPACE) )
					{
						if( (Touches[DIK_LSHIFT ] || Touches[DIK_LSHIFT]) && cnv[k] >= 'a' && cnv[k]<='z' )
						{
							txt[pos++] = cnv[k]-'a'+'A';
							txt[pos+1] = 0;
						}
						else
						{
							txt[pos++] = cnv[k];
							txt[pos+1] = 0;
						}
					}
			}
		}
    }

	Ecran.Free_Surf( &s );
	return NULL;
}

/* ******************************************************************** */

void
CInterface::CaptureChar( char *chain, int MaxChar )
{
	int pos = strlen(chain);

	if( Touche( DIK_BACK ) && pos > 0)
	{
		chain[pos-1]=0;
		pos -- ;
	}
	if( pos < MaxChar )
	{
		int k;
		for( k= 0; k< (int)strlen(cnv); k++)
			if( Touche( k + 2 ) && ( cnv[k] !=' ' || k + 2 == DIK_SPACE) )
			{
				if( (Touches[DIK_LSHIFT ] || Touches[DIK_RSHIFT]) && cnv[k] >= 'a' && cnv[k]<='z' )
					chain[pos++] = cnv[k]-'a'+'A';
				else
					chain[pos++] = cnv[k];
				chain[pos] = 0;
			}
	}
}
		
/* ******************************************************************** */
/* Fin du fichier entree.cpp                                            */
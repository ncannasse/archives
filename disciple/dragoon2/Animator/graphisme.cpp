/* ******************************************************************** */
/* graphisme.cpp                                                        */
/* ******************************************************************** */
#include "graphisme.h"
#include "Erreurs.h"
#include "decor_fake.h"
#include "global.h"
#include "entree.h"
#include "dlib.h"
#include "fonts.h"
/* ******************************************************************** */
#ifdef _DEBUG
#define DDRAW_WINDOWED_MODE
#define WAIT_SECONDS	0
#else
#define WAIT_SECONDS	3000
#endif
/* ******************************************************************** */
RECT			RWin = {0,0,RES_X,RES_Y};
CDDBasic		Ecran;
RUNFONCTION		tmp;
DWORD			StartTime;
extern MSG		msg;
/* ******************************************************************** */

void
Wait_Click()
{
	Ctrl->Mise_a_Jour();
	Ctrl->Lire_Clavier();
	if( Ctrl->Etat_Boutons || Touche(DIK_SPACE) || Touche(DIK_RETURN) 
		|| GetTickCount()-StartTime > WAIT_SECONDS )
	{
		Ctrl->Etat_Boutons = 0;
		RunFonction = tmp;
	}
}

/* ******************************************************************** */

void
CDDBasic::MsgErreur( LPCSTR Msg )
{
	tmp = RunFonction;
	StartTime = GetTickCount();
	Ctrl->Etat_Boutons = 0;
	Fill( Cachee, Rect(100,350,700,450), Couleur[0][0][20] );
	Fonts->Print( Msg, 150,370, Couleur[63][63][63],FANTASTIC );
	InvalidZone( Rect(0,350,RES_X,450) );
	Affiche();

	RunFonction = &Wait_Click;
	return;
}

/* ******************************************************************** */

void
CDDBasic::Affiche()
{
	LPZONES Z,Z2;
	long x,y;

	Frames++;
	CurTicks = GetTickCount();
	if( CurTicks-Ticks > 4000 )
	{
		FPS = (BYTE)(Frames >> 2);
		Ticks = CurTicks;
		Frames = 0;
	}
	if( Rafraichir != NULL )
		Rafraichir( RParam );

	Le_Decor->Trans->Gere( NORMAL );
#ifdef USE_TIMERS
	Timers->Gere();
#endif

#ifdef DDRAW_WINDOWED_MODE
	if( GetForegroundWindow() == HWnd )
		if((rDX = Visible->surf->BltFast(0,0,Cachee->surf,&RWin, DDBLTFAST_NOCOLORKEY  | DDBLTFAST_WAIT))!=DD_OK )
			DirectXError("CDDBasic/Affiche",rDX);
#else
	Visible->surf->Flip (NULL,DDFLIP_WAIT);
	Z = Zo1;
	Zo1 = Zo2;
	Zo2 = Z;
	Z = ZoTr1;
	ZoTr1 = ZoTr2;
	ZoTr2 = Z;
#endif

	if( (DWORD)Zo1 != ZONE_TOUT )
	{
		while( Zo1 != NULL )
		{
			Z = Zo1;
			Zo1 = Zo1->Suivant;
			y = Z->R.top;
			x = Z->R.left;
			Z->R.top+=Le_Decor->DecorVisible.top;
			Z->R.left+=Le_Decor->DecorVisible.left;
			Z->R.right+=Le_Decor->DecorVisible.left;
			Z->R.bottom+=Le_Decor->DecorVisible.top;
			Cachee->surf->BltFast(x,y,Decor->surf, &Z->R, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );
			Z2 = ZoSys;
			while( Z2 != NULL )
			{
				if( Intersect(Z2->R,Z->R) )
				{
					Sys->surf->BltFast(x,y,Decor->surf,&Z->R, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );
					break;
				}
				Z2 = Z2->Suivant;
			}
			delete Z;
		}
	}
	else
	{
		Zo1 = NULL;
		Cachee->surf->BltFast(0,0,Decor->surf,&Le_Decor->DecorVisible, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
		Z = ZoSys;
		while( Z != NULL )
		{
			y = Z->R.top;
			x = Z->R.left;
			Z->R.top+=Le_Decor->DecorVisible.top;
			Z->R.left+=Le_Decor->DecorVisible.left;
			Z->R.right+=Le_Decor->DecorVisible.left;
			Z->R.bottom+=Le_Decor->DecorVisible.top;
			Sys->surf->BltFast(x,y,Decor->surf,&Z->R, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );
			Z = Z->Suivant;
		}
	}

	while( ZoNowTr != NULL )
	{
		Z = ZoNowTr;
		ZoNowTr = ZoNowTr->Suivant;
		y = Z->R.top;
		x = Z->R.left;
		Z->R.top+=Le_Decor->DecorVisible.top;
		Z->R.left+=Le_Decor->DecorVisible.left;
		Z->R.right+=Le_Decor->DecorVisible.left;
		Z->R.bottom+=Le_Decor->DecorVisible.top;
		Sys->surf->BltFast(x,y,Decor->surf,&Z->R, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );
		delete Z;
	}

	while( ZoTr1 != NULL )
	{
		Z = ZoTr1;
		ZoTr1 = ZoTr1->Suivant;
		y = Z->R.top;
		x = Z->R.left;
		Z->R.top+=Le_Decor->DecorVisible.top;
		Z->R.left+=Le_Decor->DecorVisible.left;
		Z->R.right+=Le_Decor->DecorVisible.left;
		Z->R.bottom+=Le_Decor->DecorVisible.top;
		Cachee->surf->BltFast(x,y,Decor->surf, &Z->R, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );
		delete Z;
	}

	//---- Affiche les masks -----
/*	if( Le_Decor->Mask != NULL )
	{
		Ecran.Lock(Cachee);
		for(y=0; y<RES_Y; y++)
			for(x=0; x<RES_X; x++)
				if( Le_Decor->Mask[ x+y*Le_Decor->Mx ] )
					Cachee->buf[x+y*Cachee->pitch] = Le_Decor->Mask[ x+y*Le_Decor->Mx ];
		Ecran.UnLock( Cachee );
	}*/
	//-----------------------------

	Le_Decor->Trans->Gere( SUB );
}

/* ******************************************************************** */

void
CDDBasic::Nombre( void *Nb )
{
	Fonts->AfficheNb( (*(long *)Nb), 10,10, Ecran.CFond );
}

/* ******************************************************************** */

void
CDDBasic::SnapShot()
{
	FILE *handle = fopen("snap.pcx", "wb");
	DWORD *c,*sc,*scc;
	DWORD sign = 0x0801050A;
	BYTE col,count;
	DWORD pos=0,pos2=0;
	int i,x,y,p;

	if( handle == NULL ) return;

	c = new DWORD[RES_X*RES_Y];
	scc = c;

	Lock( Visible );


	for(y=0;y<RES_Y;y++,pos2+=(Visible->pitch-RES_X))
		for(x=0;x<RES_X;x++,pos++,pos2++)
			c[pos]=(FindColor[Visible->buf[pos2]].R<<2)+
					(FindColor[Visible->buf[pos2]].G<<10)+
					(FindColor[Visible->buf[pos2]].B<<18);

	UnLock( Visible );

	fwrite(&sign,1,4,handle);
	sign = 0;
	fwrite(&sign,1,4,handle);
	sign = (RES_X-1)+((RES_Y-1)<<16);
	fwrite(&sign,1,4,handle);
	sign = RES_X+(RES_Y<<16);
	fwrite(&sign,1,4,handle);
	
	sign = 0;
	for(i= 0; i<16; i++) fwrite(&sign,1,3,handle);
	sign = 0x0301;
	fwrite(&sign,1,2,handle);
	sign = RES_X;
    fwrite(&sign,1,2,handle);
	sign = 0x01;
	fwrite(&sign,1,1,handle);
	sign = 0;
	for(i=0; i<59; i++) fwrite(&sign,1,1,handle);


	for(y=0; y<RES_Y; y++)
	{
		sc = c;
		for(p=0; p<20; p+=8 )
		{
			c = sc;
			x = 0;
			while( x<RES_X )
			{
				col = (BYTE)((*(c++))>>p);
				count = 1;
				x++;
				while( col == (BYTE)((*(c++))>>p) && x<RES_X && count < 63 )
				{
					x++;
					count++;
				}
				c--;
				if( count == 1 &&  (col & 192 ) == 0)
				{
					fwrite(&col,1,1,handle );
				}
				else
				{
					count += 192;
					fwrite( &count,1,1,handle);
					fwrite( &col, 1,1, handle);
				}
			}
		}
	}
	delete scc;
	fclose(handle);
}


/* ******************************************************************** */

void
CDDBasic::Affiche_Sprite(long X, long Y, LPSET Src, RECT RSrc )
{
	if ( X >= Cachee->x || Y >= Cachee->y  
		|| (X+RSrc.right-RSrc.left) < 1 
		|| (Y+RSrc.bottom-RSrc.top) < 1 ) 
					return;

	if ( X+RSrc.right-RSrc.left > Cachee->x )
		RSrc.right = RSrc.left+Cachee->x-X;

	if ( Y+RSrc.bottom-RSrc.top > Cachee->y )
		RSrc.bottom = RSrc.top+Cachee->y-Y;

	if (X < 0)
	{
		RSrc.left += -X;
		X = 0;
	}

	if (Y<0)
	{
		RSrc.top += -Y;
		Y = 0;
	}

#ifdef _DEBUG
	while( (rDX = Cachee->surf->BltFast( X, Y, Src->surf , &RSrc, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT ))!= DD_OK)
	{
		switch (rDX)
		{
		case DDERR_SURFACELOST :
			if ((rDX = Cachee->surf->Restore ()) != DD_OK)
				DirectXFatalError("CDDBasic/AfficheSprite, Restore",rDX);
			continue;
		case DDERR_WASSTILLDRAWING :
			continue;
		default :
			DirectXError("CDDBasic/AfficheSprite",rDX);
			return;
		}
    }
#else
	Cachee->surf->BltFast( X, Y, Src->surf, &RSrc, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
#endif
	InvalidZone( Rect(X, Y, RSrc.right-RSrc.left+X, RSrc.bottom-RSrc.top+Y) );
	AfficheSys( X,Y,Src,&RSrc);
}

/* ******************************************************************** */

void
CDDBasic::Affiche_SpriteF(long X, long Y, LPSET Src, RECT RSrc )
{
	long x = X, y = Y,xc;
	RECT r = RSrc,col;

	if ( x >= Cachee->x || y >= Cachee->y  
		|| (x+r.right-r.left) < 1 
		|| (y+r.bottom-r.top) < 1 ) 
					return;

	if ( x+r.right-r.left > Cachee->x )
		r.right = r.left+Cachee->x-x;

	if ( y+r.bottom-r.top > Cachee->y )
		r.bottom = r.top+Cachee->y-y;

	if (x < 0)
	{
		r.left += -x;
		x = 0;
	}

	if (y<0)
	{
		r.top += -y;
		y = 0;
	}

	col.top = r.top;
	col.bottom = r.bottom;
	col.left = RSrc.right-x+X-1;
	col.right = col.left+1;

	for(xc=x; xc< x+r.right-r.left; xc++,col.left--,col.right--)
	{
		Cachee->surf->BltFast( xc,y, Src->surf, &col, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
		Sys->surf->BltFast(xc,y, Src->surf, &col, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
	}

	InvalidZone( Rect(x, y, r.right-r.left+x, r.bottom-r.top+y) );
}

/* ******************************************************************** */

void
CDDBasic::Affiche_SpriteEx(LPSET Dst, long X, long Y, LPSET Src, RECT RSrc )
{
	if ( X >= Dst->x || Y >= Dst->y  
		|| (X+RSrc.right-RSrc.left) < 1 
		|| (Y+RSrc.bottom-RSrc.top) < 1 ) 
					return;

	if ( X+RSrc.right-RSrc.left > Dst->x )
		RSrc.right = RSrc.left+Dst->x-X;

	if ( Y+RSrc.bottom-RSrc.top > Dst->y )
		RSrc.bottom = RSrc.top+Dst->y-Y;

	if (X < 0)
	{
		RSrc.left += -X;
		X = 0;
	}

	if (Y<0)
	{
		RSrc.top += -Y;
		Y = 0;
	}

	Dst->surf->BltFast( X, Y, Src->surf, &RSrc, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
}

/* ******************************************************************** */

void
CDDBasic::AfficheSys( long X, long Y, LPSET Src, LPRECT RSrc )
{
	LPZONES Z = ZoSys;

	RSrc->left+=X;
	RSrc->right+=X;
	RSrc->top+=Y;
	RSrc->bottom+=Y;

	while( Z != NULL )
	{

		if( RSrc->left < Z->R.right && RSrc->top < Z->R.bottom && RSrc->right > Z->R.left && RSrc->bottom > Z->R.top )
		{
			RSrc->left-=X;
			RSrc->right-=X;
			RSrc->top-=Y;
			RSrc->bottom-=Y;
			Sys->surf->BltFast( X, Y, Src->surf, RSrc, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
			return;
		}

		Z = Z->Suivant;
	}
}

/* ******************************************************************** */

void
CDDBasic::Copie_Zone(LPSET Dst, long X, long Y, LPSET Src, RECT RSrc )
{
	if ( X >= Dst->x || Y >= Dst->y  
		|| (X+RSrc.right-RSrc.left) < 1 
		|| (Y+RSrc.bottom-RSrc.top) < 1 ) 
					return;

	if ( X+RSrc.right-RSrc.left > Dst->x )
		RSrc.right = RSrc.left+Dst->x-X;

	if ( Y+RSrc.bottom-RSrc.top > Dst->y )
		RSrc.bottom = RSrc.top+Dst->y-Y;

	if (X < 0)
	{
		RSrc.left += -X;
		X = 0;
	}

	if (Y<0)
	{
		RSrc.top += -Y;
		Y = 0;
	}

#ifdef _DEBUG
	while( (rDX = Dst->surf->BltFast( X, Y, Src->surf , &RSrc, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT))!= DD_OK)
	{
		switch (rDX)
		{
		case DDERR_SURFACELOST :
			if ((rDX = Dst->surf->Restore ()) != DD_OK)
				DirectXFatalError("CDDBasic/CopieZone, Restore",rDX);
			continue;
		case DDERR_WASSTILLDRAWING :
			continue;
		default :
			DirectXError("CDDBasic/CopieZone",rDX);
			return;
		}
    }
#else
	Dst->surf->BltFast( X, Y, Src->surf, &RSrc, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );
#endif

}


/* ******************************************************************** */

RECT
CDDBasic::Clip( RECT R, LPSET Set )
{
	if( R.top < 0 )
		R.top = 0;
	if( R.left < 0 )
		R.left = 0;
	if( R.right > Set->x )
		R.right = Set->x;
	if( R.bottom > Set->y )
		R.bottom = Set->y;
	return R;
}

/* ******************************************************************** */

void 
CDDBasic::Nouvelle_Surface (LPSET *un_set,DWORD X,DWORD Y )
{
	*un_set= new SET;

	DDSURFACEDESC un_DDSd;
	memset (&un_DDSd,0,sizeof(un_DDSd));
	un_DDSd.dwSize = sizeof (un_DDSd);
    un_DDSd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    un_DDSd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	un_DDSd.dwWidth = X;
    un_DDSd.dwHeight = Y;

	(*un_set)->x = X;
	(*un_set)->y = Y;
	(*un_set)->mode = VIDEO;
	(*un_set)->buf = NULL;

    if ((rDX = lpDD->CreateSurface(&un_DDSd,&(*un_set)->surf,NULL)) != DD_OK)
	{
	    un_DDSd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		(*un_set)->mode = SYSTEM;
	    if ((rDX = lpDD->CreateSurface(&un_DDSd,&(*un_set)->surf,NULL)) != DD_OK)
			DirectXFatalError("CDDBasic/Nouvelle_Surface", rDX );	
	}

	LOCK L;
	L.surface = (*un_set)->surf;
	Lock( &L );
	(*un_set)->pitch = (L.surface_desc.lPitch>>1);
	UnLock(&L);

	DDCOLORKEY un_DD;
	un_DD.dwColorSpaceHighValue = CFond;
	un_DD.dwColorSpaceLowValue = CFond;
	(*un_set)->surf->SetColorKey(DDCKEY_SRCBLT, &un_DD );

}

/* ******************************************************************** */

void 
CDDBasic::Nouvelle_SurfSys (LPSET *un_set,DWORD X,DWORD Y )
{
	*un_set= new SET;

	DDSURFACEDESC un_DDSd;
	memset (&un_DDSd,0,sizeof(un_DDSd));
	un_DDSd.dwSize = sizeof (un_DDSd);
    un_DDSd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    un_DDSd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	un_DDSd.dwWidth = X;
    un_DDSd.dwHeight = Y;

	(*un_set)->x = X;
	(*un_set)->y = Y;
	(*un_set)->buf = NULL;
	(*un_set)->mode = SYSTEM;
	
	if ((rDX = lpDD->CreateSurface(&un_DDSd,&((*un_set)->surf),NULL)) != DD_OK)
		DirectXFatalError("CDDBasic/Nouvelle_SurfSys", rDX );	

	LOCK L;
	L.surface = (*un_set)->surf;
	Lock( &L );
	(*un_set)->pitch = (L.surface_desc.lPitch>>1);
	UnLock(&L);

	DDCOLORKEY un_DD;
	un_DD.dwColorSpaceHighValue = CFond;
	un_DD.dwColorSpaceLowValue = CFond;
	(*un_set)->surf->SetColorKey(DDCKEY_SRCBLT, &un_DD );

}

/* ******************************************************************** */

void
CDDBasic::Free_Surf( LPSET *un_set )
{
	if( *un_set != NULL && (*un_set)->surf != NULL )
		(*un_set)->surf->Release();
	delete (*un_set);
	(*un_set)=NULL;
}

/* ******************************************************************** */

void 
CDDBasic::Fill ( LPSET Set, RECT R, WORD Col )
{
	DDBLTFX un_ddbltfx;

	un_ddbltfx.dwSize = sizeof(un_ddbltfx);
	un_ddbltfx.dwFillColor = Col;  

	if (rDX = (Set->surf->Blt(&R, NULL, NULL,DDBLT_COLORFILL | DDBLT_WAIT, &un_ddbltfx)) != DD_OK )
		if( rDX != 1 )
			DirectXError("CDDBasic/Fill", rDX);

}

/* ******************************************************************** */


CDDBasic::CDDBasic (void) 
{
	lpDD = NULL;
	Visible = NULL;
	Cachee  = NULL;
	Decor = NULL;
	Sys = NULL;
	Ticks = 0;
	FPS = 40;
	Frames = 160;

	Rafraichir = NULL;
	RParam = NULL;

	Zo1 = (LPZONES)ZONE_TOUT;
	Zo2 = (LPZONES)ZONE_TOUT;
	ZoSys = NULL;
	ZoNowTr = NULL;
}

/* ******************************************************************** */

CDDBasic::~CDDBasic (void) 
{
}

/* ******************************************************************** */

void
CDDBasic::WaitFrames( int FrameNumber )
{
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
			Affiche();
			FrameNumber--;
			if( FrameNumber <=0 )
				return;
		}
    }
}

/* ******************************************************************** */

void
CDDBasic::WaitTime( DWORD TimeCount )
{
	DWORD T = GetTickCount();
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
			Affiche();
			if( CurTicks-T >= TimeCount )
				return;
		}
	}
}

/* ******************************************************************** */

void
CDDBasic::Initialise_DD (HWND hWnd) 
{
	LOCK L;

	HWnd = hWnd;
	if ((rDX = DirectDrawCreate (NULL,&lpDD,NULL)) != DD_OK) 
		DirectXFatalError("CDDBasic/InitDD/Create",rDX);
#ifndef DDRAW_WINDOWED_MODE

	if ((rDX = lpDD->SetCooperativeLevel (HWnd,
			DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN)) != DD_OK) 
		DirectXFatalError("CDDBasic/InitDD/SetCL",rDX);

	if ((rDX = lpDD->SetDisplayMode (RES_X,RES_Y,NBCOULEURS)) != DD_OK) 
		DirectXFatalError("CDDBasic/InitDD/SetMode",rDX);

	Visible = new SET;
	Cachee = new SET;
	DDSURFACEDESC un_DDSd;
	memset (&un_DDSd,0,sizeof(un_DDSd));
	un_DDSd.dwSize = sizeof (un_DDSd);
    un_DDSd.dwFlags= DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	un_DDSd.ddsCaps.dwCaps = DDSCAPS_COMPLEX
		| DDSCAPS_FLIP
		| DDSCAPS_PRIMARYSURFACE;
	un_DDSd.dwBackBufferCount = 1;

	if ((rDX = lpDD->CreateSurface (&un_DDSd,&(Visible->surf),NULL)) != DD_OK)
		DirectXFatalError("CDDBasic/InitDD/CreateSurf",rDX);
	
	Visible->x = RES_X;
	Visible->y = RES_Y;
	Visible->mode = VIDEO;
	Visible->buf = NULL;
	Cachee->x = RES_X;
	Cachee->y = RES_Y;
	Cachee->mode = VIDEO;
	Cachee->buf = NULL;

	DDSCAPS un_DDscaps;
	un_DDscaps.dwCaps = DDSCAPS_BACKBUFFER;
	if ((rDX = Visible->surf->GetAttachedSurface (&un_DDscaps,&(Cachee->surf))) != DD_OK) 
		DirectXFatalError("CDDBasic/InitDD/GetAttach",rDX);

	L.surface = Visible->surf;
	Lock( &L );
	Visible->pitch = (L.surface_desc.lPitch>>1);
	UnLock( &L );
	L.surface = Cachee->surf;
	Lock( &L );
	Cachee->pitch = (L.surface_desc.lPitch>>1);
	UnLock( &L );


#else

	if ((rDX = lpDD->SetCooperativeLevel (hWnd,DDSCL_NORMAL)) != DD_OK) 
		DirectXFatalError("CDDBasic/InitDD/SetCL",rDX);

	Visible = new SET;
	DDSURFACEDESC un_DDSd;
	memset (&un_DDSd,0,sizeof(un_DDSd));
	un_DDSd.dwSize = sizeof (un_DDSd);
	un_DDSd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if ((rDX = lpDD->CreateSurface (&un_DDSd,&(Visible->surf),NULL)) != DD_OK)
		DirectXFatalError("CDDBasic/InitDD/CreateSurf",rDX);
	
	Visible->x = RES_X;
	Visible->y = RES_Y;
	Visible->mode = VIDEO;
	Visible->buf = NULL;
	L.surface = Visible->surf;
	Lock( &L );
	Visible->pitch = (L.surface_desc.lPitch>>1);
	UnLock( &L );

	Nouvelle_Surface( &Cachee, RES_X, RES_Y );

#endif
	Init_Cols ();
	Nouvelle_Surface( &Decor, RES_X*2, RES_Y*2 );
	Nouvelle_SurfSys( &Sys  , RES_X, RES_Y+2   );
}

/* ******************************************************************** */

void
CDDBasic::CleanZones( LPZONES *lz )
{
	LPZONES Z = *lz ,Z2;

	if( (DWORD)Z != ZONE_TOUT )
		while( Z != NULL )
			{
				Z2 = Z->Suivant;
				delete Z;
				Z = Z2;
			}
	*lz = NULL;
}


/* ******************************************************************** */

void 
CDDBasic::Ferme_DD (void) 
{
	if (lpDD)
	{
		CleanZones( &Zo1 );
		CleanZones( &Zo2 );
		CleanZones( &ZoTr1 );
		CleanZones( &ZoTr2 );
		CleanZones( &ZoSys );
		CleanZones( &ZoNowTr );
		lpDD->RestoreDisplayMode ();
		lpDD->SetCooperativeLevel (NULL,DDSCL_NORMAL);

		Free_Surf( &Cachee );
		Free_Surf( &Decor );
		Free_Surf( &Sys );
		Free_Surf( &Visible );

		lpDD->Release ();
		lpDD = NULL;
	}

}

/* ******************************************************************** */

void CDDBasic::Init_Cols()
{
	HDC un_hdc;
    WORD pos = 0;
	BYTE Red, Green, Blue;
	int i,r,g,b;

	for( Red = 0 ; Red < 64 ; Red++)
	{
		if ( Cachee->surf->GetDC(&un_hdc) == DD_OK)
		{
			for( Green = 0; Green < 64 ; Green++)
				for( Blue = 0 ; Blue< 64 ; Blue++)
					SetPixel(un_hdc, Green, Blue, RGB( Red*4, Green*4, Blue*4 ));            
			Cachee->surf->ReleaseDC (un_hdc);
		}
		Lock( Cachee );
		for( Green = 0; Green < 64 ; Green++)
			for( Blue = 0 ; Blue< 64 ; Blue++)
				Couleur[Red][Green][Blue] = Cachee->buf[ Green + Blue*(Cachee->pitch) ]; 
 		UnLock( Cachee );
	}

	FindColor[0].R = 0;
	FindColor[0].G = 0;
	FindColor[0].B = 0;
	for(  i=1; i< 65536 ; i++ )
		FindColor[i].R = 255;
	for( r=63; r>=0; r--)
		for( g=63; g>=0; g--)
			for( b=63; b>=0; b--)
			{
				FindColor[ Couleur[r][g][b] ].R = r;
				FindColor[ Couleur[r][g][b] ].G = g;
				FindColor[ Couleur[r][g][b] ].B = b;
			}
	for(  i=1; i< 65536 ; i++ )
		if ( FindColor[i].R == 255 )
			{
				FindColor[ i ].R = FindColor[i-1].R;
				FindColor[ i ].G = FindColor[i-1].G;
				FindColor[ i ].B = FindColor[i-1].B;
			}
	CFond = Couleur[63][0][63];

}

/* ******************************************************************** */

int
Charge( FILE *f, void *Set )
{
	BYTE t;
	DWORD Taille;

	fread( &t,1,1,f );
	fread( &Taille, 1,4, f);
	switch( t )
	{
	case TYPE_PCX:
		return Ecran.Charge_PCX( f, (LPSET)Set, ((LPSET)Set)->x, ((LPSET)Set)->y,Taille );
	case TYPE_GIF:
		return Ecran.Charge_GIF( f, (LPSET)Set, ((LPSET)Set)->x, ((LPSET)Set)->y,Taille );
	default:
		return -1;
	}
	return 0;
}

/* ******************************************************************** */

void
CDDBasic::Charge_Ecran ( DWORD ID )
{
	Charge_EcranEx(ID,Decor,0,0);
	InvalidAll();
}

/* ******************************************************************** */

void
CDDBasic::Charge_EcranEx (DWORD ID,LPSET un_Set,DWORD Pos_X, DWORD Pos_Y)
{
	long X = un_Set->x ,Y = un_Set->y;
	un_Set->x = Pos_X;
	un_Set->y = Pos_Y;
	DLib->LoadTmp( ID, &Charge, (void *)un_Set );
	un_Set->x = X;
	un_Set->y = Y;
}

/* ******************************************************************** */

void
CDDBasic::Get_PCX_info( FILE *f, LPRECT R )
{	
	BYTE ch,ch2;

	fseek(f,4,1);
	fread(&ch,1,1,f);
	fread(&ch2,1,1,f);
	R->right = (ch+(ch2<<8)+1);
	
	fread(&ch,1,1,f);
	fread(&ch2,1,1,f);
	R->bottom = (ch+(ch2<<8)+1);
		
	R->top = 0;
	R->left = 0;
	fclose(f);
}

/* ******************************************************************** */

int
CDDBasic::Charge_PCX (FILE *f,LPSET un_Set,DWORD Pos_X, DWORD Pos_Y, DWORD Taille )
{
	int Dim_X,Dim_Y,XPitch;
	int Compt_X=0,Compt_Y=0;

	BYTE ch1,ch2;
	BYTE bpp;
	BYTE plan=0;
	DWORD pos=0;
	DWORD pp;   

	fread(&ch1,1,1,f);
	fread(&ch2,1,1,f);
	Dim_X=(ch1+(ch2<<8)+1);

	fread(&ch1,1,1,f);
	fread(&ch2,1,1,f);
	Dim_Y=(ch1+(ch2<<8)+1);

	fseek(f,53,1);    
	fread(&bpp,1,1,f);

	if (bpp!=1 && bpp!=3)
		return -1;

	fread(&ch1,1,1,f);
	fread(&ch2,1,1,f);
	XPitch=(ch1+(ch2<<8)+1);
	
	fseek (f,60,1);
	BYTE *Temp = new BYTE[Taille-119];
	fread ( (void *)Temp,Taille-120,1,f);
	fclose(f);

	Lock( un_Set );

	WORD *Destination = un_Set->buf;

	pp = Pos_X + Pos_Y*un_Set->pitch;

	BYTE i;
	while (Compt_Y < Dim_Y)
	{
		ch1=Temp[pos++];
		if ( (ch1 &192) >=192)
		{
			ch2 = Temp[pos++];
			i = ch1 & 63;
		}
		else
		{
			ch2 = ch1;
			i = 1;
		}
		while (i--)
		{
			switch (bpp)
			{
			case 1:
				if( Compt_X < Dim_X )
				{
					Destination [pp++] = Couleur 
						[ (Temp[Taille-120-768+ch2*3]>>2)   ]
						[ (Temp[Taille-120-768+ch2*3+1]>>2) ]
						[ (Temp[Taille-120-768+ch2*3+2]>>2) ];

				}
				if ( ++Compt_X == (XPitch-1) )
				{
					Compt_X = 0;
					pp += un_Set->pitch - Dim_X;
					Compt_Y++; 
				}
				break;
			case 3:
				if( Compt_X < Dim_X )
					switch (plan)
					{
					case 2:
						Destination [pp] = Couleur
							[Destination[pp]&255][Destination[pp]>>8][ch2>>2];
						break;
					case 1:
						Destination [pp] += (WORD)(ch2 >> 2)<<8;
						break;
					case 0:
						Destination [pp] = (WORD)(ch2 >> 2);
						break;
					}
				pp++;
				if( ++Compt_X == (XPitch-1) )
				{
					if (++plan == 3)
					{
						plan = 0; 
						pp += un_Set->pitch;
						Compt_Y++; 
					}
					pp -= XPitch-1;
					Compt_X=0;
				}
				break;
			}
		}
	}

	UnLock( un_Set );
	delete Temp;
	return 0;
}

/* ******************************************************************** */

int
GetInfo( FILE *f, void *R )
{
	BYTE Type;
	fread(&Type,1,1,f);
	switch( Type )
	{
	case TYPE_PCX:
		Ecran.Get_PCX_info( f, (LPRECT)R );
		break;
	case TYPE_GIF:
		Ecran.Get_GIF_info( f, (LPRECT)R );
		break;
	default:
		return -1;
	}
	return 0;
}

/* ******************************************************************** */

RECT
CDDBasic::Get_Info( DWORD ID )
{
	RECT R;
	DLib->LoadTmp(ID,&GetInfo,&R);
	return R;
}

/* ******************************************************************** */

void
CDDBasic::Get_GIF_info ( FILE *f, LPRECT R )
{
  	int bitspixel=0;
  	int temp=0;

  	fseek(f,9,1);
  	fread(&temp,1,1,f);
  	bitspixel = (temp & 7) + 1;
	fseek(f,(1<<bitspixel)*3+7, 1);
  	fread(&temp,1,2,f);
	R->right = temp;
  	fread(&temp,1,2,f);
	R->bottom = temp;
	R->top = 0;
	R->left = 0;
	fclose(f);
}


/* ******************************************************************** */

int
CDDBasic::Charge_GIF (FILE *f,LPSET un_Set,DWORD Pos_X, DWORD Pos_Y, DWORD Taille )
{
  	int Powers[9];
  	int Prefix[4097];
  	int Suffix[4097];
  	int OutCode[1025];
  	int MaxCodes[13];
  	int Powers2[17];
  	int CodeMask[9];
  	BYTE *Buffer;
	DWORD pp;
  	DWORD Address=0;

  	BYTE Rouge[256], Vert[256], Bleu[256];
  	int a,b,c;

  	int bitspixel=0;
  	int temp=0;
  	int Code=0;
  	int CodeSize=0;
  	int ClearCode=0;
  	int EOFCode=0;
  	int FirstFree=0;
  	int FreeCode=0;
  	int InitCodeSize=0;
  	int MaxCode=0;
  	int BitMask=0;
  	int BlockLength=0;
  	int BitsIn=0;
  	int Num=0;
  	int OutCount=0;
  	int X=0;
  	int Y=0;
  	int aa=0;
  	int TempChar=0;
  	int CurCode=0;
  	int OldCode=0;
  	int FinChar=0;
  	int InCode=0;

	WORD XStart, YStart,XEnd, YEnd, XLength, YLength;

  	for (a=0;a< 8;a++)
      		Powers[a+1] = (1<<a);
  	for (a=0;a<15;a++)
			Powers2[a] = (1<<a);
  	for (a=0;a< 12;a++)
			MaxCodes[a] = (1<<(a+2));

  	b = 1;
  	c = 2;
  	for (a=1;a< 9;a++)
	{
		CodeMask[a] = b;
		b+=c;
		c <<= 1;
	}

	Buffer = new BYTE[Taille];
	fread( Buffer, 1, Taille, f);
	Address = 0;
	
  	if (Buffer[Address++] != 'a')
		return -1;


	Address+=4;

	bitspixel = (Buffer[Address] & 7) + 1;
	Address+=3;
  	for (a=0;a< (1<<bitspixel) ;a++)
		{
			Rouge[a] = (Buffer[Address++]>>2);
			Vert[a] = (Buffer[Address++]>>2);
			Bleu[a] = (Buffer[Address++]>>2);
		};

  	if (Buffer[Address++] != 44)
		return -1;

	XStart = ((WORD*)(Buffer+Address))[0];
	Address+=2;
	YStart = ((WORD*)(Buffer+Address))[0];
	Address+=2;
	XLength = ((WORD*)(Buffer+Address))[0];
	Address+=2;
	YLength = ((WORD*)(Buffer+Address))[0];
	Address+=2;

	XEnd = XLength + XStart - 1;
  	YEnd = YLength + YStart - 1;


  	if (Buffer[Address++] > 63)
		return -1;


  	CodeSize = Buffer[Address++];
  	ClearCode = Powers2[CodeSize];
  	EOFCode = ClearCode + 1;
  	FirstFree = ClearCode + 2;
  	FreeCode = FirstFree;
  	CodeSize = CodeSize + 1;
  	InitCodeSize = CodeSize;
  	MaxCode = MaxCodes[CodeSize - 2];
  	BitMask = CodeMask[bitspixel];

	BlockLength = Buffer[Address++];
  	BlockLength++;
  	BitsIn = 8;
  	Num = 0;
  	OutCount = 0;
  	X = XStart;
  	Y = YStart;

	Lock( un_Set );
	pp=Pos_X+Pos_Y*un_Set->pitch;

	while (Code != EOFCode)
  	{
    	Code = 0;
   		for (aa= 0;aa< CodeSize;aa++)
		{
			BitsIn++;
			if (BitsIn == 9)
			{
				TempChar = Buffer[Address++];
				BitsIn = 1;
				Num++;
				if (Num == BlockLength)
				{
  					BlockLength = TempChar + 1;
	  				TempChar = Buffer[Address++];
  					Num = 1;
				}
      		}
      		if ((TempChar & Powers[BitsIn]) > 0)
	 			Code = Code + Powers2[aa];
    			
    	}
   		if (Code != EOFCode)
		{
      		if (Code == ClearCode)
			{
				CodeSize = InitCodeSize;
				MaxCode = MaxCodes[CodeSize - 2];
				FreeCode = FirstFree;
				Code = 0;
				for (aa = 0;aa< CodeSize;aa++)
				{
  					BitsIn++;
  					if (BitsIn == 9)
					{
	    				TempChar = Buffer[Address++];
    					BitsIn = 1;
   						Num++;
   						if (Num == BlockLength)
						{
	      					BlockLength = TempChar + 1;
	      					TempChar = Buffer[Address++];
   							Num = 1;
   						}
	  				}
	  				if ((TempChar & Powers[BitsIn]) > 0)
	    					Code = Code + Powers2[aa];
				}

      			CurCode = Code;
      			OldCode = Code;
      			FinChar = Code & BitMask;
				un_Set->buf[pp++] = Couleur[ Rouge[FinChar] ][ Vert[FinChar] ][ Bleu[FinChar] ];
   				X++;
   				if (X > XEnd)
				{
	      			X = XStart;
	      			Y++;
					pp+=un_Set->pitch-XLength;
      			}
    		}
    		else
			{
      			CurCode = Code;
      			InCode = Code;
      			if (Code >= FreeCode)
				{
					CurCode = OldCode;
					OutCode[OutCount] = FinChar;
					OutCount++;
      			}
     			if (CurCode > BitMask)
				{
					while (CurCode > BitMask)
					{
	  					OutCode[OutCount] = Suffix[CurCode];
	  					OutCount++;
	  					CurCode = Prefix[CurCode];
					}
      			}

   				FinChar = CurCode & BitMask;
   				OutCode[OutCount] = FinChar;
   				OutCount++;
   				for (a = (OutCount - 1); a>=0; a--)
   				{
	 				un_Set->buf[pp++] = Couleur[ Rouge[OutCode[a]] ][ Vert[OutCode[a]] ][ Bleu[OutCode[a]] ];
					X++;
					if (X > XEnd)
					{
						X = XStart;
						Y++;
						pp+=un_Set->pitch-XLength;
					}
      			}
      			OutCount = 0;
      			Prefix[FreeCode] = OldCode;
      			Suffix[FreeCode] = FinChar;
      			OldCode = InCode;
      			FreeCode++;
      			if ((FreeCode >= MaxCode) && (CodeSize < 12))
				{
					CodeSize++;
					MaxCode <<= 1;
      			}
    		}
   		}
  	}
  	fclose(f);
	delete Buffer;
	UnLock( un_Set );

	return 0;
}

/* ******************************************************************** */

void
CDDBasic::Lock( LPLOCK Lparam)
{
	Lparam->surface_desc.dwSize = sizeof( DDSURFACEDESC );

#ifdef _DEBUG
	while( (rDX = Lparam->surface->Lock(NULL,
			&Lparam->surface_desc,
			DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT ,
			NULL)) != DD_OK )
	{

		switch( rDX)
		{

			case DDERR_SURFACELOST : 
				if ((rDX = Lparam->surface->Restore ()) != DD_OK)
					DirectXFatalError("Lock, Cannot Restore",rDX );
				continue;
			case DDERR_SURFACEBUSY:
			case DDERR_WASSTILLDRAWING : 
				Lparam->buf = NULL;
				return;
			default : 
				DirectXError("Lock",rDX );
				break;
		}

	}
#else
	if( (rDX = Lparam->surface->Lock( NULL, &Lparam->surface_desc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)) != DD_OK )
		DirectXError("Lock",rDX );
#endif
	Lparam->buf= (WORD *)Lparam->surface_desc.lpSurface;
}

/* ******************************************************************** */

void
CDDBasic::UnLock( LPLOCK Lparam)
{
	if(Lparam->buf == NULL) return;
	if( (rDX=Lparam->surface->Unlock( Lparam->surface_desc.lpSurface )) != DD_OK)
		DirectXFatalError("Unlock",rDX);
	Lparam->buf = NULL;
}

/* ******************************************************************** */

void
CDDBasic::Lock( LPSET Set )
{
	DDSURFACEDESC ddsd;
	if( Set->buf != NULL )
	{
		DirectXError("Locking a locked surface :", (long)Set->surf );
		return;
	}
	ddsd.dwSize = sizeof( DDSURFACEDESC );
	if( (rDX = Set->surf->Lock( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)) != DD_OK )
		DirectXError("LockSet",rDX );
	Set->buf= (WORD *)ddsd.lpSurface;
}

/* ******************************************************************** */

void
CDDBasic::UnLock( LPSET Set )
{
	if( Set->buf == NULL)
		return;
	if( (rDX=Set->surf->Unlock( (void *)Set->buf )) != DD_OK)
		DirectXError("UnlockSet",rDX);
	Set->buf = NULL;
}


/* ******************************************************************** */

LPSET
CDDBasic::Get_Set( SURFACE Num )
{
	if( Num == CACHEE )
		return Cachee;
	if( Num == DECOR )
		return Decor;
	if( Num == VISIBLE )
		return Visible;
	if( Num == SYS )
		return Sys;
	return NULL;
}

/* ******************************************************************** */

void
CDDBasic::InvalidZone( RECT R )
{
	if( (DWORD)Zo1 == ZONE_TOUT )
		return;
	LPZONES Z = new ZONES;
	Z->R = R;
	Z->Suivant = Zo1;
	Zo1 = Z;
}

/* ******************************************************************** */

void
CDDBasic::InvalidZoneTr( RECT R )
{
	LPZONES Z = new ZONES;
	Z->R = R;
	Z->Suivant = ZoTr1;
	ZoTr1 = Z;
}

/* ******************************************************************** */

void
CDDBasic::InvalidZoneNoTr( RECT R )
{
	LPZONES Z = new ZONES;
	Z->R = R;
	Z->Suivant = ZoNowTr;
	ZoNowTr = Z;
}

/* ******************************************************************** */

LPRECT
CDDBasic::AjouteZoneTr( RECT R )
{
	LPZONES Z = new ZONES;
	Z->R = R;
	Z->Suivant = ZoSys;
	ZoSys = Z;
	return &Z->R;
}

/* ******************************************************************** */

void
CDDBasic::DelZoneTr( LPRECT R )
{
	LPZONES Z = ZoSys, o = NULL;
	while( Z != NULL )
	{
		if( &Z->R == R )
		{
			if( o == NULL )
			{
				ZoSys = ZoSys->Suivant;
				delete Z;
				return;
			}
			else
			{
				o->Suivant = Z->Suivant;
				delete Z;
				return;
			}
		}
		o = Z;
		Z = Z->Suivant;
	}
}

/* ******************************************************************** */

void
CDDBasic::ActuZone( RECT R )
{
	LPZONES Z;
	if( (DWORD)Zo1 != ZONE_TOUT )
	{
		Z = new ZONES;
		Z->R = R;
		Z->Suivant = Zo1;
		Zo1 = Z;
	}
	if( (DWORD)Zo2 != ZONE_TOUT )
	{
		Z = new ZONES;
		Z->R = R;
		Z->Suivant = Zo2;
		Zo2 = Z;
	}
}

/* ******************************************************************** */

void
CDDBasic::InvalidAll( void )
{
	LPZONES Z;
	if( (DWORD)Zo1 != ZONE_TOUT )
	{
		while ( Zo1 != NULL )
		{
			Z = Zo1->Suivant;
			delete Zo1;
			Zo1 = Z;
		}

		Zo1 = (LPZONES)ZONE_TOUT;
	}
	
	if( (DWORD)Zo2 != ZONE_TOUT )
	{
		while ( Zo2 != NULL )
		{
			Z = Zo2->Suivant;
			delete Zo2;
			Zo2 = Z;
		}
		Zo2 = (LPZONES)ZONE_TOUT;
	}
}

/* ******************************************************************** */

RECT Rect(long left, long top, long right, long bottom)
{
	RECT R;
	R.bottom = bottom;
	R.top = top;
	R.left = left;
	R.right = right;
	return R;
}

/* ******************************************************************** */

bool
Intersect(RECT R1, RECT R2)
{
	if(R1.top>=R2.top && R1.top<=R2.bottom && R1.left>=R2.left && R1.left<=R2.right) return true;
	if(R1.bottom>=R2.top && R1.bottom<=R2.bottom && R1.left>=R2.left && R1.left<=R2.right) return true;
	if(R1.bottom>=R2.top && R1.bottom<=R2.bottom && R1.right>=R2.left && R1.right<=R2.right) return true;
	if(R1.top>=R2.top && R1.top<=R2.bottom && R1.right>=R2.left && R1.right<=R2.right) return true;
	if(R2.top>=R1.top && R2.top<=R1.bottom && R2.left>=R1.left && R2.left<=R1.right) return true;
	if(R2.bottom>=R1.top && R2.bottom<=R1.bottom && R2.left>=R1.left && R2.left<=R1.right) return true;
	if(R2.bottom>=R1.top && R2.bottom<=R1.bottom && R2.right>=R1.left && R2.right<=R1.right) return true;
	if(R2.top>=R1.top && R2.top<=R1.bottom && R2.right>=R1.left && R2.right<=R1.right) return true;
	if(R1.top<=R2.top && R1.bottom>=R2.bottom && R1.left>=R2.left && R1.right<=R2.right) return true;
	if(R2.top<=R1.top && R2.bottom>=R1.bottom && R2.left>=R1.left && R2.right<=R1.right) return true;
	return false;
}

/* ******************************************************************** */
/* fin du fichier graphisme.cpp                                         */

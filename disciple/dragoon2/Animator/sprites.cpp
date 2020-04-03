/* ******************************************************************** */
/* sprites.cpp		                                                    */
/* ******************************************************************** */
#include "sprites.h"
#include "decor_fake.h"
#include "erreurs.h"
/* ******************************************************************** */

CSprites *Sprites = NULL;

/* ******************************************************************** */

CSprites::CSprites ()
{
	int i;
	for(i=0; i<256; i++)
		Liste_Sprites[i] = NULL;
}

/* ******************************************************************** */

CSprites::~CSprites ()
{
	Libere();
}


/* ******************************************************************** */

void
CSprites::Libere()
{
	LPSprite spr;
	int i;

	for( i=0; i<256; i++)
	{
		spr = Liste_Sprites[i];
		while( spr != NULL )
		{
			Liste_Sprites[i] = spr->Suivant;
			Ecran.Free_Surf( &spr->Set );
			delete spr;
			spr = Liste_Sprites[i];
		}
	}
}

/* ******************************************************************** */

void
CSprites::Clean( DWORD Time )
{
	LPSprite spr,os;
	int i;

	for( i=0; i<256; i++)
	{
		os  = NULL;
		spr = Liste_Sprites[i];
		while( spr != NULL )
		{
			if( Ecran.CurTicks - spr->Tick > Time )
			{
				if( os == NULL )
				{
					Liste_Sprites[i] = spr->Suivant;
					Ecran.Free_Surf( &spr->Set );
					delete spr;
					spr = Liste_Sprites[i];
				}
				else
				{
					os->Suivant = spr->Suivant;
					Ecran.Free_Surf( &spr->Set );
					delete spr;
					spr = os->Suivant;
				}
			}
			else
			{
				os = spr;
				spr = spr->Suivant;
			}
		}
	}
}

/* ******************************************************************** */

LPSprite
CSprites::Ajoute( DWORD ID )
{
	LPSprite spr;
	RECT	Zone;

	Clean( 60000 );

	spr = new TSprite;

    Zone = Ecran.Get_Info( ID );
	Ecran.Nouvelle_Surface( &spr->Set, Zone.right, Zone.bottom );
    Ecran.Charge_EcranEx ( ID, spr->Set, 0, 0);

	spr->ID = ID;
	spr->Zone = Zone;
	spr->Suivant = Liste_Sprites[ ID & 255 ];
	Liste_Sprites[ ID & 255 ] = spr;

	return spr;
}

/* ******************************************************************** */

RECT
CSprites::GetZone( DWORD ID	)
{
	LPSprite spr = Liste_Sprites[ ID & 255 ];

	while( spr != NULL )
	{
		if( spr->ID == ID )
			break;
		spr = spr->Suivant;
	}
	if( spr == NULL )
		spr = Ajoute( ID );
	return spr->Zone;
}

/* ******************************************************************** */

void 
CSprites::Affiche( long X, long Y, DWORD ID)
{
	LPSprite spr = Liste_Sprites[ ID & 255 ];

	while( spr != NULL )
	{
		if( spr->ID == ID )
			break;
		spr = spr->Suivant;
	}
	if( spr == NULL )
		spr = Ajoute( ID );

	spr->Tick = Ecran.CurTicks;
	Ecran.Affiche_Sprite(X-Le_Decor->Ox,Y-Le_Decor->Oy, spr->Set, spr->Zone );
};

/* ******************************************************************** */

void 
CSprites::Print( long X, long Y, DWORD ID)
{
	LPSprite spr = Liste_Sprites[ ID & 255 ];

	while( spr != NULL )
	{
		if( spr->ID == ID )
			break;
		spr = spr->Suivant;
	}
	if( spr == NULL )
		spr = Ajoute( ID );

	spr->Tick = Ecran.CurTicks;
	Ecran.Affiche_SpriteEx(Ecran.Get_Set(CACHEE),X-Le_Decor->Ox,Y-Le_Decor->Oy, spr->Set, spr->Zone );
};

/* ******************************************************************** */

void 
CSprites::AfficheFlip( long X, long Y, DWORD ID)
{
	LPSprite spr = Liste_Sprites[ ID & 255 ];

	while( spr != NULL )
	{
		if( spr->ID == ID )
			break;
		spr = spr->Suivant;
	}
	if( spr == NULL )
		spr = Ajoute( ID );

	spr->Tick = Ecran.CurTicks;
	Ecran.Affiche_SpriteF(X-Le_Decor->Ox,Y-Le_Decor->Oy, spr->Set, spr->Zone );
};

/* ******************************************************************** */

void 
CSprites::AfficheTr( long X, long Y, DWORD ID, int Trans )
{
	LPSprite spr = Liste_Sprites[ ID & 255 ];
	DWORD p,p2=0,p3,p4;
	LPSET set = Ecran.Get_Set( CACHEE ), sys = Ecran.Get_Set(SYS);
	WORD c,c2, prof;
	BYTE r,g,b;
	int x,y;

	if( Trans <= 0 || X-Le_Decor->Ox < 0 || Y-Le_Decor->Oy < 0  )
		return;
	if( Trans >= 100 )
		Trans = 100;

	while( spr != NULL )
	{
		if( spr->ID == ID )
			break;
		spr = spr->Suivant;
	}
	if( spr == NULL )
		spr = Ajoute( ID );


	spr->Tick = Ecran.CurTicks;
	
	prof=(WORD)(Y+spr->Zone.bottom-spr->Zone.top);
	Ecran.Lock( set );
	Ecran.Lock( spr->Set );
	Ecran.Lock( sys );
	p = X-Le_Decor->Ox + (Y-Le_Decor->Oy)*set->pitch;
	p3 = X-Le_Decor->Ox + (Y-Le_Decor->Oy)*sys->pitch;
	p4 = X+Y*Le_Decor->Mx;
	for(y=0; y< spr->Zone.bottom; y++, p+= set->pitch-x, p2+=spr->Set->pitch-x,p3+=sys->pitch-x, p4+=Le_Decor->Mx-x )
		for(x=0; x< spr->Zone.right; x++,p++,p2++,p3++,p4++ )
			if( Le_Decor->Mask[p4] < prof && (c=spr->Set->buf[p2]) != Ecran.CFond )
			{
				c2 = set->buf[p];
				r = ( Ecran.FindColor[c].R*Trans+Ecran.FindColor[c2].R*(100-Trans) )/100;
				g = ( Ecran.FindColor[c].G*Trans+Ecran.FindColor[c2].G*(100-Trans) )/100;
				b = ( Ecran.FindColor[c].B*Trans+Ecran.FindColor[c2].B*(100-Trans) )/100;
				set->buf[p]=Ecran.Couleur[r][g][b];
				sys->buf[p3]=Ecran.Couleur[r][g][b];
			}
	Ecran.UnLock( set );
	Ecran.UnLock( sys );
	Ecran.UnLock( spr->Set );	
	Ecran.InvalidZone( Rect(X-Le_Decor->Ox, Y-Le_Decor->Oy,X-Le_Decor->Ox+spr->Zone.right, Y-Le_Decor->Oy+spr->Zone.bottom) );
};

/* ******************************************************************** */

void 
CSprites::AfficheZoom( long X, long Y, DWORD ID, int ZFactor )
{
	LPSprite spr = Liste_Sprites[ ID & 255 ];
	DWORD p,psav;
	LPSET set = Ecran.Get_Set( CACHEE );
	WORD x,y,c;
	WORD dat[65536];
	float dx,dy;
	float rx,ry;
	float adr;

	if( ZFactor <= 0 || X-Le_Decor->Ox < 0 || Y-Le_Decor->Oy < 0  )
		return;

	while( spr != NULL )
	{
		if( spr->ID == ID )
			break;
		spr = spr->Suivant;
	}
	if( spr == NULL )
		spr = Ajoute( ID );


	if( spr->Zone.right > 256 || spr->Zone.bottom > 256 )
		return;

	spr->Tick = Ecran.CurTicks;

	p = 0;
	Ecran.Lock( spr->Set );
	for(y=0; y < spr->Set->y; y++,p+=spr->Set->pitch-x )
	{
		for(x=0; x < spr->Set->x; x++,p++ )
			dat[x+(y<<8)]=spr->Set->buf[p];
		dat[x+(y<<8)] = Ecran.CFond;
	}
	Ecran.UnLock( spr->Set );	
	
	Ecran.Lock( set );
	p = X+Y*set->pitch;
	dx = (float)(100/(ZFactor*1.0));
	dy = (float)(100/(ZFactor*1.0));
	rx = ry = adr = 0;
	while( ry < spr->Set->y )
	{
		psav = p;
		while( (BYTE)rx < spr->Set->x )
		{
			if( (c=dat[(DWORD)adr]) != Ecran.CFond )
				set->buf[p] = c;
			p++;
			rx += dx;
			adr+= dx;
		}
		adr-=rx;
		rx = 0;
		ry += dy;
		adr = (float)(((BYTE)ry)<<8);
		p = psav+set->pitch;
	}
	Ecran.UnLock( set );
	Ecran.InvalidZone( Rect(X-Le_Decor->Ox, Y-Le_Decor->Oy,X-Le_Decor->Ox+(spr->Zone.right*ZFactor)/100+1, Y-Le_Decor->Oy+(spr->Zone.bottom*ZFactor)/100+1) );
};

/* ******************************************************************** */

void 
CSprites::AfficheAlpha( long X, long Y, DWORD ID, WORD Col )
{
	LPSprite spr = Liste_Sprites[ ID & 255 ];
	DWORD p,p2=0,p3,p4;
	LPSET set = Ecran.Get_Set( CACHEE ), sys = Ecran.Get_Set(SYS);
	int x,y;

	while( spr != NULL )
	{
		if( spr->ID == ID )
			break;
		spr = spr->Suivant;
	}
	if( spr == NULL )
		spr = Ajoute( ID );


	spr->Tick = Ecran.CurTicks;
	
	Ecran.Lock( set );
	Ecran.Lock( spr->Set );
	Ecran.Lock( sys );
	p = X-Le_Decor->Ox + (Y-Le_Decor->Oy)*set->pitch;
	p3 = X-Le_Decor->Ox + (Y-Le_Decor->Oy)*sys->pitch;
	p4 = X+Y*Le_Decor->Mx;
	for(y=0; y< spr->Zone.bottom; y++, p+= set->pitch-x, p2+=spr->Set->pitch-x,p3+=sys->pitch-x, p4+=Le_Decor->Mx-x )
		for(x=0; x< spr->Zone.right; x++,p++,p2++,p3++,p4++ )
			if( spr->Set->buf[p2] != Ecran.CFond )
			{
				set->buf[p]=Col;
				sys->buf[p3]=Col;
			}
	Ecran.UnLock( set );
	Ecran.UnLock( sys );
	Ecran.UnLock( spr->Set );	
	Ecran.InvalidZone( Rect(X-Le_Decor->Ox, Y-Le_Decor->Oy,X-Le_Decor->Ox+spr->Zone.right, Y-Le_Decor->Oy+spr->Zone.bottom) );
};

/* ******************************************************************** */

void 
CSprites::AfficheClip( long X, long Y, DWORD ID, RECT r)
{
	LPSprite spr = Liste_Sprites[ ID & 255 ];
	RECT Z;

	while( spr != NULL )
	{
		if( spr->ID == ID )
			break;
		spr = spr->Suivant;
	}
	if( spr == NULL )
		spr = Ajoute( ID );

	spr->Tick = Ecran.CurTicks;
	Z = spr->Zone;

	if( X+Z.right > r.right )
		Z.right =  r.right-X;
	if( Y+Z.bottom > r.bottom )
		Z.bottom =  r.bottom-Y;
	if( X < r.left )
	{
		Z.left+=(r.left-X);
		X = r.left;
	}
	if( Y < r.top )
	{
		Z.top+=(r.top-Y);
		Y = r.top;
	}

	Ecran.Affiche_Sprite(X-Le_Decor->Ox,Y-Le_Decor->Oy, spr->Set, Z );
};

/* ******************************************************************** */

void 
CSprites::PrintClip( long X, long Y, DWORD ID, RECT r)
{
	LPSprite spr = Liste_Sprites[ ID & 255 ];
	RECT Z;

	while( spr != NULL )
	{
		if( spr->ID == ID )
			break;
		spr = spr->Suivant;
	}
	if( spr == NULL )
		spr = Ajoute( ID );

	spr->Tick = Ecran.CurTicks;
	Z = spr->Zone;

	if( X > r.right || Y > r.bottom || X+Z.right < r.left || Y+Z.bottom < r.top )
		return;
	if( X+Z.right > r.right )
		Z.right =  r.right-X;
	if( Y+Z.bottom > r.bottom )
		Z.bottom =  r.bottom-Y;
	if( X < r.left )
	{
		Z.left+=(r.left-X);
		X = r.left;
	}
	if( Y < r.top )
	{
		Z.top+=(r.top-Y);
		Y = r.top;
	}

	Ecran.Affiche_SpriteEx(Ecran.Get_Set(CACHEE),X-Le_Decor->Ox,Y-Le_Decor->Oy, spr->Set, Z );
};

/* ******************************************************************** */

void 
CSprites::Ecrit( long X, long Y, DWORD ID)
{
	LPSprite spr = Liste_Sprites[ ID & 255 ];

	while( spr != NULL )
	{
		if( spr->ID == ID )
			break;
		spr = spr->Suivant;
	}
	if( spr == NULL )
		spr = Ajoute( ID );

	spr->Tick = Ecran.CurTicks;
	Ecran.Affiche_SpriteEx(Ecran.Get_Set(DECOR),X-Le_Decor->Ox,Y-Le_Decor->Oy, spr->Set, spr->Zone );
	Ecran.ActuZone( Rect(X-Le_Decor->Ox,Y-Le_Decor->Oy,X-Le_Decor->Ox+spr->Zone.right,Y-Le_Decor->Oy+spr->Zone.bottom) );
};

/* ******************************************************************** */
// fin de sprites.cpp


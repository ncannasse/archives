/* ******************************************************************** */
#include "graphisme.h"
#include "entree.h"
#include "anims.h"
#include "dlib.h"
#include "erreurs.h"
#include "fonts.h"
#include "plugin.h"
/* ******************************************************************** */
const LPCSTR Debuts[] =	{ "NORMAL","MAIN OBJECT","WAIT TIME","WAIT FRAMES","SYNCHRO" };
const LPCSTR Fins[] =	{ "BOUCLE","NEXT ANIM","STOP","WAIT RANDOM","WAIT RNDGLOB","KILL" };
/* ******************************************************************** */

extern LPAnimList CurAnim;
extern HWND		  hWnd;
MSG			msg;
LPAnimList	PlayAnim;

enum EDITMODES {
	FRAMES,
	FLAGS,
	CODES
};

bool		StartPause;
bool		Paused, Slow;
EDITMODES	mode;
int			obj1, obj2;
int			frame;
int			ZFactor;

int			R = 0, G = 0, B = 0;


// mouse
int			MLEFT, MRIGHT, MDOWN, MUP, MMOVE;
int			LastX, LastY;

/* ******************************************************************** */

void
gere_code_left()
{
	int i;
	switch( frame )
	{
	case 0:
		CurAnim->Objs[obj1]->CodeDebut--;
		if( CurAnim->Objs[obj1]->CodeDebut >= LAST_DEBUT )
			CurAnim->Objs[obj1]->CodeDebut = LAST_DEBUT-1;
		switch( CurAnim->Objs[obj1]->CodeDebut )
		{
		case SYNCHRO:
			CurAnim->Objs[obj1]->ParamDebut = (DWORD)CurAnim->Objs[0];
			break;
		default:
			if( CurAnim->Objs[obj1]->FrameCount > 0 && CurAnim->Objs[obj1]->Frames[0]->Time == 0 )
				CurAnim->Objs[obj1]->Frames[0]->Time = 10;
			CurAnim->Objs[obj1]->ParamDebut = 0;
			break;
		}
		break;
	case 1:
		switch( CurAnim->Objs[obj1]->CodeDebut )
		{
		case SYNCHRO:
			for( i=0; i< CurAnim->NbObjs; i++)
				if( (DWORD)CurAnim->Objs[i] == CurAnim->Objs[obj1]->ParamDebut )
					break;
			if( i == 0 )
				i = CurAnim->NbObjs;
			CurAnim->Objs[obj1]->ParamDebut = (DWORD)CurAnim->Objs[i-1];
			break;
		case WAIT_TIME:
			CurAnim->Objs[obj1]->ParamDebut-=(KeyPress(DIK_LSHIFT)||KeyPress(DIK_RSHIFT))?1:10;
			break;
		default:
			CurAnim->Objs[obj1]->ParamDebut--;
			break;
		}
		break;
	case 2:
		CurAnim->Objs[obj1]->CodeFin--;
		if( CurAnim->Objs[obj1]->CodeFin >= LAST_FIN )
			CurAnim->Objs[obj1]->CodeFin = LAST_FIN-1;
		switch( CurAnim->Objs[obj1]->CodeFin )
		{
		case CHANGE_ANIM:
			CurAnim->Objs[obj1]->ParamFin = (DWORD)Anims.Liste;
			break;
		default:
			CurAnim->Objs[obj1]->ParamFin = 0;
			break;
		}
		break;
	case 3:
		switch( CurAnim->Objs[obj1]->CodeFin )
		{
		case CHANGE_ANIM:
			LPAnimList a;
			a = Anims.Liste;
			if( (DWORD)a == CurAnim->Objs[obj1]->ParamFin )
				while( a->Suivant != NULL )
					a = a->Suivant;
			else
				while( a != NULL && (DWORD)a->Suivant != CurAnim->Objs[obj1]->ParamFin )
					a = a->Suivant;
			if( a == NULL )
				a = Anims.Liste;
			CurAnim->Objs[obj1]->ParamFin = (DWORD)a;
			break;
		case WAIT_RANDOM:
		case WAIT_RANDOM_GLOBAL:
			CurAnim->Objs[obj1]->ParamFin-=(KeyPress(DIK_LSHIFT)||KeyPress(DIK_RSHIFT))?1:10;
			break;
		default:
			CurAnim->Objs[obj1]->ParamFin--;
			break;
		}
		break;
	}
}


/* ******************************************************************** */

void
gere_code_right()
{
	int i;
	switch( frame )
	{
	case 0:
		CurAnim->Objs[obj1]->CodeDebut++;
		if( CurAnim->Objs[obj1]->CodeDebut >= LAST_DEBUT )
			CurAnim->Objs[obj1]->CodeDebut = 0;
		switch( CurAnim->Objs[obj1]->CodeDebut )
		{
		case SYNCHRO:
			CurAnim->Objs[obj1]->ParamDebut = (DWORD)CurAnim->Objs[0];
			break;
		default:
			if( CurAnim->Objs[obj1]->FrameCount > 0 && CurAnim->Objs[obj1]->Frames[0]->Time == 0 )
				CurAnim->Objs[obj1]->Frames[0]->Time = 10;
			CurAnim->Objs[obj1]->ParamDebut = 0;
			break;
		}
		break;
	case 1:
		switch( CurAnim->Objs[obj1]->CodeDebut )
		{
		case SYNCHRO:
			for( i=0; i< CurAnim->NbObjs; i++)
				if( (DWORD)CurAnim->Objs[i] == CurAnim->Objs[obj1]->ParamDebut )
					break;
			if( i >= CurAnim->NbObjs-1 )
				i = -1;
			CurAnim->Objs[obj1]->ParamDebut = (DWORD)CurAnim->Objs[i+1];
			break;
		case WAIT_TIME:
			CurAnim->Objs[obj1]->ParamDebut+=(KeyPress(DIK_LSHIFT)||KeyPress(DIK_RSHIFT))?1:10;
			break;
		default:
			CurAnim->Objs[obj1]->ParamDebut++;
			break;
		}
		break;
	case 2:
		CurAnim->Objs[obj1]->CodeFin++;
		if( CurAnim->Objs[obj1]->CodeFin >= LAST_FIN )
			CurAnim->Objs[obj1]->CodeFin = 0;
		switch( CurAnim->Objs[obj1]->CodeFin )
		{
		case CHANGE_ANIM:
			CurAnim->Objs[obj1]->ParamFin = (DWORD)Anims.Liste;
			break;
		default:
			CurAnim->Objs[obj1]->ParamFin = 0;
			break;
		}
		break;
	case 3:
		switch( CurAnim->Objs[obj1]->CodeFin )
		{
		case CHANGE_ANIM:
			LPAnimList a;
			a = Anims.Liste;
			while( a != NULL && (DWORD)a != CurAnim->Objs[obj1]->ParamFin )
					a = a->Suivant;
			if( a != NULL )
				a = a->Suivant;

			if( a == NULL )
				a = Anims.Liste;
			CurAnim->Objs[obj1]->ParamFin = (DWORD)a;
			break;
		case WAIT_RANDOM:
		case WAIT_RANDOM_GLOBAL:
			CurAnim->Objs[obj1]->ParamFin+=(KeyPress(DIK_LSHIFT)||KeyPress(DIK_RSHIFT))?1:10;
			break;
		default:
			CurAnim->Objs[obj1]->ParamFin++;
			break;
		}
		break;
	}
}

/* ******************************************************************** */

void
ChangeBack( int *comp )
{
	if( KeyPress(DIK_ADD) )
		(*comp)++;
	if( KeyPress(DIK_SUBTRACT) )
		(*comp)--;
	if( (*comp)< 0 )
		(*comp) = 63;
	if( (*comp)> 63 )
		(*comp) = 0;
	Ecran.Fill(Ecran.Get_Set(DECOR),Rect(0,0,RES_X,RES_Y),Ecran.Couleur[R][G][B] );
	Fonts->EcritNb(R,780,5, Ecran.Couleur[63][63][63] );
	Fonts->EcritNb(G,780,20, Ecran.Couleur[63][63][63] );
	Fonts->EcritNb(B,780,35, Ecran.Couleur[63][63][63] );
	Ecran.InvalidAll();
}

/* ******************************************************************** */

void
RunAnimator()
{
	int i;
	char buf[256];
	WORD *col = NULL;

	StartPause = false;
	Paused = false;
	Slow = false;
	obj1 = obj2 = 0;
	frame = -1;
	ZFactor = 100;
	mode = FRAMES;

	Ecran.Fill( Ecran.Get_Set(DECOR), Rect(0,0,RES_X,RES_Y), Ecran.Couleur[R][G][B] );
	Ecran.InvalidAll();
	PlayAnim = CurAnim;
	Anims.Init();
	Ctrl->Actu();
	LastX = Ctrl->X;
	LastY = Ctrl->Y;

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
			Ctrl->Actu();
			Ecran.Affiche();
			if( Paused )
			{
				Anims.Init();
				Fonts->Affiche("PAUSED", RES_X-60,RES_Y-15, Ecran.Couleur[63][63][63] );
			}
			else
			{
				if( StartPause && CurAnim->Objs[ obj1 ]->CurFrame == frame )
				{
					Paused = true;
					StartPause = false;
				}
			}

			if( Slow )
				Fonts->Affiche("<SLOW>", RES_X-60,RES_Y-30, Ecran.Couleur[63][63][63] );

			PlayAnim = Anims.Play( PlayAnim, Slow, ZFactor );

			Ecran.Fill( Ecran.Get_Set(CACHEE), Rect(5,5,150,RES_Y-5), Ecran.Couleur[10][20][30] );
			Ecran.Fill( Ecran.Get_Set(CACHEE), Rect(165,5,315,RES_Y-5), Ecran.Couleur[5][10][15] );
			
			Fonts->Print( CurAnim->Objs[obj1]->Nom, 10,6, (frame==-1)?Ecran.Couleur[63][63][0]:Ecran.Couleur[40][40][0] );


			if( mode != CODES )
			{
				for(i=0; i< CurAnim->Objs[obj1]->FrameCount; i++ )
				{
					DString e = Plugin->CnvIDToFile( CurAnim->Objs[obj1]->Frames[i]->IDSprite );
					strcpy( buf, e.c_str() );
					buf[17] = 0;
					switch( mode )
					{
					case FRAMES:
						Fonts->Print( buf, 10,20+i*22, (frame==i)?Ecran.Couleur[63][63][0]:Ecran.Couleur[40][40][0] );
						Fonts->PrintNb((CurAnim->Objs[obj1]->Frames[i]->Time/10), 10,20+i*22+11, (frame==i)?Ecran.Couleur[63][63][63]:Ecran.Couleur[40][40][40] );
						Fonts->PrintNb(CurAnim->Objs[obj1]->Frames[i]->dx, 50,20+i*22+11, (frame==i)?Ecran.Couleur[63][63][63]:Ecran.Couleur[40][40][40] );
						Fonts->PrintNb(CurAnim->Objs[obj1]->Frames[i]->dy, 100,20+i*22+11, (frame==i)?Ecran.Couleur[63][63][63]:Ecran.Couleur[40][40][40] );
						break;
					case FLAGS:
						struct {
							long ID;
							char buf[256];
							int  Group;
						} CNV;
						Fonts->Print( buf, 10,20+i*34, (frame==i)?Ecran.Couleur[63][63][0]:Ecran.Couleur[40][40][0] );
						CNV.ID = CurAnim->Objs[obj1]->Frames[i]->Flag;
						CNV.Group = 3;
						if( !Plugin->RunPlugin( "symbols.dll","ConvertSymbol", &CNV ) )
							_ltoa( CNV.ID, CNV.buf, 10);
						Fonts->Print(CNV.buf, 10,20+i*34+11, (frame==i)?Ecran.Couleur[63][63][63]:Ecran.Couleur[40][40][40] );
						CNV.ID = CurAnim->Objs[obj1]->Frames[i]->PFlag;
						CNV.Group = 4;
						if( !Plugin->RunPlugin("symbols.dll","ConvertSymbol",&CNV ) )
							_ltoa( CNV.ID, CNV.buf, 10 );
						Fonts->Print(CNV.buf, 10,20+i*34+22, (frame==i)?Ecran.Couleur[63][63][63]:Ecran.Couleur[40][40][40] );
						break;
					}
				}
				Fonts->Affiche(">",0,10+22*CurAnim->Objs[obj1]->CurFrame+11, Ecran.Couleur[30][30][30] );
			}
			else
			{
				Fonts->Print(Debuts[CurAnim->Objs[obj1]->CodeDebut], 10,20, (frame==0)?Ecran.Couleur[63][63][63]:Ecran.Couleur[40][40][40] );
				switch( CurAnim->Objs[obj1]->CodeDebut )
				{
				case SYNCHRO:
					Fonts->Print(((LPAnimObj)CurAnim->Objs[obj1]->ParamDebut)->Nom, 10,35, (frame==1)?Ecran.Couleur[63][63][63]:Ecran.Couleur[40][40][40] );
					break;
				default:
					Fonts->PrintNb(CurAnim->Objs[obj1]->ParamDebut, 10,35,(frame==1)?Ecran.Couleur[63][63][63]:Ecran.Couleur[40][40][40] );
					break;
				}
				Fonts->Print(Fins[CurAnim->Objs[obj1]->CodeFin], 10,60, (frame==2)?Ecran.Couleur[63][63][63]:Ecran.Couleur[40][40][40]);
				switch( CurAnim->Objs[obj1]->CodeFin )
				{
				case CHANGE_ANIM:
					Fonts->Print(((LPAnimList)CurAnim->Objs[obj1]->ParamFin)->Name, 10,75, (frame==3)?Ecran.Couleur[63][63][63]:Ecran.Couleur[40][40][40] );
					break;
				default:
					Fonts->PrintNb(CurAnim->Objs[obj1]->ParamFin, 10,75, (frame==3)?Ecran.Couleur[63][63][63]:Ecran.Couleur[40][40][40] );
					break;
				}
			}


			Fonts->Affiche( CurAnim->Objs[obj2]->Nom, 170,6, Ecran.Couleur[40][40][0] );
			for(i=0; i< CurAnim->Objs[obj2]->FrameCount; i++ )
			{
				DString e = Plugin->CnvIDToFile( CurAnim->Objs[obj2]->Frames[i]->IDSprite );
				strcpy( buf, e.c_str() );
				buf[20] = 0;
				Fonts->Print( buf, 165,20+i*22, Ecran.Couleur[40][40][0] );
				Fonts->PrintNb((CurAnim->Objs[obj2]->Frames[i]->Time/10), 165,20+i*22+11, Ecran.Couleur[40][40][40] );
				Fonts->PrintNb(CurAnim->Objs[obj2]->Frames[i]->dx, 205,20+i*22+11, Ecran.Couleur[40][40][40] );
				Fonts->PrintNb(CurAnim->Objs[obj2]->Frames[i]->dy, 255,20+i*22+11, Ecran.Couleur[40][40][40] );
			}
			Fonts->Affiche(">",160,10+22*CurAnim->Objs[obj2]->CurFrame+11, Ecran.Couleur[30][30][30] );
// Ctrl+R = synchronize object
			if( Touche( DIK_R ) && ( KeyPress( DIK_LCONTROL ) || KeyPress( DIK_RCONTROL ) ) )
			{
				CurAnim->Objs[ obj1 ]->CodeDebut = SYNCHRO;
				CurAnim->Objs[ obj1 ]->ParamDebut = (DWORD)CurAnim->Objs[ obj2 ];
				for( i=0; i< CurAnim->Objs[ obj1 ]->FrameCount; i++ )
					CurAnim->Objs[ obj1 ]->Frames[i]->Time = 0;
			}
// P = pause
			if( Touche( DIK_P ) )
			{
				StartPause = false;
				Paused = !Paused;
			}
// 0 = slow
			Slow = (KeyPress( DIK_NUMPAD0 )!=0);
// F2 = set frames mode 
			if( Touche( DIK_F2 ) )
				mode = FRAMES;
// F3 = set flags mode 
			if( Touche( DIK_F3 ) )
				mode = FLAGS;
// F4 = set codes mode 
			if( Touche( DIK_F4 ) )
				mode = CODES;
// C = copie time address
			if( Touche( DIK_C ) && frame > -1)
				col = &(CurAnim->Objs[obj1]->Frames[frame]->Time);
// V = colle time
			if( Touche( DIK_V ) && frame > -1 && col != NULL )
				CurAnim->Objs[obj1]->Frames[frame]->Time = *col;
// S = reset animation
			if( Touche(DIK_S) )
			{
				PlayAnim = CurAnim;
				for(i=0; i< CurAnim->NbObjs; i++ )
				{
					CurAnim->Objs[i]->CurFrame = 0;
					CurAnim->Objs[i]->CurTime = 0;
					CurAnim->Objs[i]->CurState = 0;
				}
			}
// * = set 1x Zoom
			if( Touche( DIK_MULTIPLY ) )
				ZFactor = 100;
// . = set 2x Zoom
			if( Touche( DIK_DECIMAL ) )
				ZFactor = 200;
// R = change 'r' background
			if( KeyPress(DIK_R) )
				ChangeBack( &R );
// G = change 'g' background
			if( KeyPress(DIK_G) )
				ChangeBack( &G );
// B = change 'b' background
			if( KeyPress(DIK_B) )
				ChangeBack( &B );
// + / - = zoom
			if( !KeyPress(DIK_B) && !KeyPress(DIK_G) && !KeyPress(DIK_R) )
			{
				if( KeyPress( DIK_ADD ) )
					ZFactor+=5;
				if( KeyPress( DIK_SUBTRACT ) )
					ZFactor-=5;
			}
// Espace = toggle pause
			if( Touche( DIK_SPACE ) )
			{
				StartPause = true;
				if( Paused )
				{
					Paused = false;
					StartPause = false;
				}
			}

// Gestion des deplacements de la souris;
			MLEFT = MRIGHT = MUP = MDOWN = 0;
			while( Ctrl->X < LastX-8 )
			{
				LastX-=8;
				if( LastX < Ctrl->X )
					LastX = Ctrl->X;
				MLEFT++;
			}
			while( Ctrl->X > LastX+8 )
			{
				LastX+=8;
				if( LastX > Ctrl->X )
					LastX = Ctrl->X;
				MRIGHT++;
			}
			while( Ctrl->Y < LastY-8 )
			{
				LastY-=8;
				if( LastY < Ctrl->Y )
					LastY = Ctrl->Y;
				MUP++;
			}
			while( Ctrl->Y > LastY+8 )
			{
				LastY+=8;
				if( LastY > Ctrl->Y )
					LastY = Ctrl->Y;
				MDOWN++;
			}
			MMOVE = MLEFT | MRIGHT | MUP | MDOWN;
// Fleches 
			if( (KeyPress( DIK_LSHIFT )||KeyPress( DIK_RSHIFT )||MMOVE) && ! KeyPress( DIK_LCONTROL ) &&  ! KeyPress( DIK_RCONTROL )  )
			{
				switch( mode )
				{
				case FRAMES:
					if( Touche( DIK_LEFT ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dx--;
					if( Touche( DIK_RIGHT ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dx++;
					if( Touche( DIK_UP ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dy--;
					if( Touche( DIK_DOWN ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dy++;
					if( MLEFT && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dx-=MLEFT;
					if( MRIGHT && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dx+=MRIGHT;
					if( MUP && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dy-=MUP;
					if( MDOWN && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dy+=MDOWN;
					break;
				case FLAGS:
					if( Touche( DIK_LEFT ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->PFlag--;
					if( Touche( DIK_RIGHT ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->PFlag++;
					break;
				case CODES:
					if( Touche( DIK_LEFT ) && frame != -1 )
						gere_code_left();
					if( Touche( DIK_RIGHT ) && frame != -1 )
						gere_code_right();
					break;
				}
			}

			if( ! KeyPress( DIK_LSHIFT ) &&  ! KeyPress( DIK_RSHIFT ) && ! KeyPress( DIK_LCONTROL ) &&  ! KeyPress( DIK_RCONTROL )  )
			{
				switch( mode )
				{
				case FRAMES:
					if( KeyPress( DIK_LEFT ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->Time-=2;
					if( KeyPress( DIK_RIGHT ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->Time+=2;
					if( Touche( DIK_NUMPAD2 ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dy++;
					if( Touche( DIK_NUMPAD8 ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dy--;
					if( Touche( DIK_NUMPAD4 ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dx--;
					if( Touche( DIK_NUMPAD6 ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->dx++;						
					break;
				case FLAGS:
					if( Touche( DIK_LEFT ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->Flag--;
					if( Touche( DIK_RIGHT ) && frame != -1 )
						CurAnim->Objs[obj1]->Frames[frame]->Flag++;
					break;
				case CODES:
					if( Touche( DIK_LEFT ) && frame != -1 )
						gere_code_left();
					if( Touche( DIK_RIGHT ) && frame != -1 )
						gere_code_right();
					break;
				default:
					break;
				}
				
				if( Touche(DIK_RIGHT) && frame == -1 )
					obj1++;
				if( Touche(DIK_LEFT) && frame == -1 )
					obj1--;

				if( Touche( DIK_UP ) )
					frame--;
				if( Touche( DIK_DOWN ) )
					frame++;

				if( obj1 < 0 )
					obj1 = CurAnim->NbObjs -1;
				if( obj1 >= CurAnim->NbObjs )
					obj1 = 0;
			}

			if( !KeyPress( DIK_LSHIFT ) && !KeyPress( DIK_RSHIFT ) && (KeyPress( DIK_LCONTROL )||KeyPress( DIK_RCONTROL ))  )
			{
				if( Touche(DIK_RIGHT) )
					obj1++;
				if( Touche(DIK_LEFT) )
					obj1--;
				if( obj1 < 0 )
					obj1 = CurAnim->NbObjs -1;
				if( obj1 >= CurAnim->NbObjs )
					obj1 = 0;
			}

// Tab = switch selection
			if( Touche( DIK_TAB ) )
			{
				i = obj1;
				obj1 = obj2;
				obj2 = i;
			}

// controle
			switch( mode )
			{
			case CODES:
				if( frame > 3 )
					frame = -1;
				if( frame < -1 )
					frame = 3;
				break;
			default:
				if( frame < -1 )
					frame = CurAnim->Objs[obj1]->FrameCount-1;
				if( frame >= CurAnim->Objs[obj1]->FrameCount )
					frame = -1;
			break;
			}
			
		}
    }
}

/* ******************************************************************** */

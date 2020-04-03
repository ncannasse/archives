/* ******************************************************************** */
// types & class defs for animations
#include "graphisme.h"
#include <stdio.h>
#include "anims.h"
#include "sprites.h"
#include "erreurs.h"
#include "plugin.h"
/* ******************************************************************** */

CAnims::CAnims()
{
	Liste = NULL;
	Nom	  = NULL;
	count = 0;
}


/* ******************************************************************** */

CAnims::~CAnims()
{
	Clean();
	delete Nom;
	Nom = NULL;
}

/* ******************************************************************** */

void
CAnims::InitDone()
{
	LPAnimList l = Liste;
	int i;

	while( l != NULL )
	{
		for( i=0; i< l->NbObjs; i++ )
		{
			switch( l->Objs[i]->CodeDebut )
			{
			case SYNCHRO:
				if( (int)l->Objs[i]->ParamDebut < l->NbObjs )
					l->Objs[i]->ParamDebut = (DWORD)l->Objs[ l->Objs[i]->ParamDebut ];
				else
					l->Objs[i]->ParamDebut = (DWORD)l->Objs[0];
				break;
			default:
				break;
			}

			switch( l->Objs[i]->CodeFin )
			{
			case CHANGE_ANIM:
				LPAnimList a;
				a = FindByName( (char *)l->Objs[i]->ParamFin );
				delete ((char *)l->Objs[i]->ParamFin);
				if( a != NULL )
					l->Objs[i]->ParamFin = (DWORD)a;
				else
					l->Objs[i]->CodeFin = BOUCLE;
				break;
			default:
				break;
			}
		}
		l = l->Suivant;
	}
}

/* ******************************************************************** */

bool
CAnims::LoadAll()
{
	FILE *f;
	char buf[257];
	BYTE b = 'A',ob_count;
	DWORD d;
	int   i,j;

	if( Anims.Nom == NULL )
		return false;
	f = fopen( Anims.Nom, "rb" );
	if( f == NULL )
		return false;
	
	fread(buf,1,3,f);
	buf[3] = 0;
	if( strcmp( buf, "ANM" ) != 0 )
	{
		fclose(f);
		return false;
	}
	while( b == 'A' )
	{
		fread(&b,1,1,f);
		fread(buf, 1, b, f);
		buf[b] = 0;

		// loading anim
		{
			LPAnimList l = new TAnimList;
			l->Name = new char[b+1];
			strcpy(l->Name, buf );
			fread(&ob_count,1,1,f);
			l->NbObjs = ob_count;
			l->Objs = new LPAnimObj[ l->NbObjs ];
			fread(l->params, 1, 6, f);
			for( i=0; i< ob_count; i++ )
			{
				l->Objs[i] = new TAnimObj;
				fread( &b,1,1,f);
				l->Objs[i]->Nom = new char[b+1];
				fread( l->Objs[i]->Nom, 1, b, f);
				l->Objs[i]->Nom[b] = 0;
				fread( &l->Objs[i]->CodeFin,1,4,f);				
				switch( l->Objs[i]->CodeFin )
				{
				case CHANGE_ANIM:
					fread( &b, 1, 1, f );
					l->Objs[i]->ParamFin = (DWORD) new char[ b + 1 ];
					fread( (char *)l->Objs[i]->ParamFin, 1, b, f );
					((char *)l->Objs[i]->ParamFin)[b] = 0;
					break;
				default:
					fread( &l->Objs[i]->ParamFin,1,4,f);
					break;
				}
				fread( &l->Objs[i]->CodeDebut,1,4,f);
				fread( &l->Objs[i]->ParamDebut,1,4,f);
				fread( &d, 1, 4, f);
				l->Objs[i]->FrameCount = d;
				l->Objs[i]->Frames = new LPAnim[l->Objs[i]->FrameCount];
				for( j=0; j< l->Objs[i]->FrameCount; j++ )
				{
					l->Objs[i]->Frames[j] = new TAnim;
					fread( l->Objs[i]->Frames[j], 1, sizeof(TAnim), f );
				}
				l->Objs[i]->CurFrame = l->Objs[i]->CurTime = l->Objs[i]->CurState = 0;
				fread( &b,1,1,f);
				if( b != 'O' )
				{
					fclose(f);
					return false;
				}
			}
			l->Suivant = Liste;
			Liste = l;
		}
		fread( &b,1,1,f);
	}
	fclose(f);
	if( b != 'E' )
		return false;
	return true;
}

/* ******************************************************************** */

bool
CAnims::Load( char *AnimName )
{
	FILE *f;
	char buf[257];
	BYTE b = 'A',ob_count;
	DWORD d;
	int   i,j;

	if( Anims.Nom == NULL )
		return false;
	f = fopen( Anims.Nom, "rb" );
	if( f == NULL )
		return false;
	
	fread(buf,1,3,f);
	buf[3] = 0;
	if( strcmp( buf, "ANM" ) != 0 )
	{
		fclose(f);
		return false;
	}
	while( b == 'A' )
	{
		fread(&b,1,1,f);
		fread(buf, 1, b, f);
		buf[b] = 0;
		if( strcmp(buf, AnimName) == 0 )
		{
			LPAnimList l = new TAnimList;
			l->Name = new char[b+1];
			strcpy(l->Name, buf );
			fread(&ob_count,1,1,f);
			l->NbObjs = ob_count;
			l->Objs = new LPAnimObj[ l->NbObjs ];
			fread(l->params, 1, 6, f);
			for( i=0; i< ob_count; i++ )
			{
				l->Objs[i] = new TAnimObj;
				fread( &b,1,1,f);
				l->Objs[i]->Nom = new char[b+1];
				fread( l->Objs[i]->Nom, 1, b, f);
				l->Objs[i]->Nom[b] = 0;
				fread( &l->Objs[i]->CodeFin,1,4,f);				
				switch( l->Objs[i]->CodeFin )
				{
				case CHANGE_ANIM:
					fread( &b, 1, 1, f );
					l->Objs[i]->ParamFin = (DWORD) new char[ b + 1 ];
					fread( (char *)l->Objs[i]->ParamFin, 1, b, f );
					((char *)l->Objs[i]->ParamFin)[b] = 0;
					break;
				default:
					fread( &l->Objs[i]->ParamFin,1,4,f);
					break;
				}
				fread( &l->Objs[i]->CodeDebut,1,4,f);
				fread( &l->Objs[i]->ParamDebut,1,4,f);
				fread( &d, 1, 4, f);
				l->Objs[i]->FrameCount = d;
				l->Objs[i]->Frames = new LPAnim[l->Objs[i]->FrameCount];
				for( j=0; j< l->Objs[i]->FrameCount; j++ )
				{
					l->Objs[i]->Frames[j] = new TAnim;
					fread( l->Objs[i]->Frames[j], 1, sizeof(TAnim), f );
				}
				l->Objs[i]->CurFrame = l->Objs[i]->CurTime = l->Objs[i]->CurState = 0;
				fread( &b,1,1,f);
			}
			l->Suivant = Liste;
			Liste = l;
			fclose(f);
			count++;
			return true;
		}
		fread(&ob_count,1,1,f);
		fread(buf,1,6,f);
		for( i=0; i< ob_count; i++ )
		{
			fread( &b,1,1,f);
			fread( buf,1,b+16,f);
			fread( &d, 1, 4, f);
			for( j=0; j< (int)d; j++ )
				fread( buf, 1, sizeof(TAnim), f );
			fread( &b,1,1,f);
			if( b != 'O' )
			{
				fclose(f);
				return false;
			}
		}	
		fread( &b,1,1,f);
	}
	fclose(f);
	return false;
}

/* ******************************************************************** */

bool
CAnims::Save()
{
	LPAnimList l = Liste;
	FILE *f;
	BYTE b;
	DWORD d;
	int i,j,main;

	if( Anims.Nom == NULL )
		return false;
	if( l == NULL )
		return true;
	f = fopen( Anims.Nom, "wb" );
	if( f == NULL )
		return false;

	fwrite("ANM",1,3,f);
	while( l != NULL )
	{
		b = (BYTE)strlen( l->Name );
		fwrite( &b,1,1,f);
		fwrite( l->Name,1,b,f);
		b = (BYTE)l->NbObjs;
		fwrite( &b,1,1,f);
		fwrite( l->params,1,6,f);
		main = 0;
		for( i=0; i< l->NbObjs; i++ )
		{
			b = (BYTE)strlen( l->Objs[i]->Nom );
			fwrite( &b,1,1,f);
			fwrite( l->Objs[i]->Nom,1,b,f);
			fwrite( &l->Objs[i]->CodeFin,1,4,f);
			
			switch( l->Objs[i]->CodeFin )
			{
			case CHANGE_ANIM:
				b = strlen( ((LPAnimList)l->Objs[i]->ParamFin)->Name );
				fwrite( &b,1,1,f);
				fwrite( ((LPAnimList)l->Objs[i]->ParamFin)->Name , 1, b, f);
				break;
			default:
				fwrite( &l->Objs[i]->ParamFin,1,4,f);
				break;
			}
			fwrite( &l->Objs[i]->CodeDebut,1,4,f);
			switch( l->Objs[i]->CodeDebut )
			{
			case SYNCHRO:
				d = 0;
				for( j=0; j< l->NbObjs; j++ )
					if( (DWORD)l->Objs[j] == l->Objs[i]->ParamDebut )
					{
						d = j;
						break;
					}
				fwrite( &d,1,4,f);				
				break;
			case PRINCIPAL:
				main = 1;
			default:
				fwrite( &l->Objs[i]->ParamDebut,1,4,f);
				break;
			}
			d = (DWORD)l->Objs[i]->FrameCount;
			fwrite( &d, 1, 4, f);
			for( j=0; j< l->Objs[i]->FrameCount; j++ )
			{
				l->Objs[i]->Frames[j]->Time -= l->Objs[i]->Frames[j]->Time%10;
				fwrite( l->Objs[i]->Frames[j], 1, sizeof( TAnim ), f );
			}
			fwrite("O",1,1,f);
		}
		if( main == 0 && l->NbObjs > 0 )
		{
			char buf[250];
			strcpy(buf,"[Warning] ");
			strcat(buf, l->Name );
			strcat(buf," has no Main object");
			Plugin->LogText(buf);
		}
		if( l->Suivant != NULL )
			fwrite("A",1,1,f);
		l = l->Suivant;
	}
	fwrite("E",1,1,f);
	fclose(f);
	return true;
}

/* ******************************************************************** */

bool
CAnims::AddObj( LPAnimList a, char *ObjName )
{
	int i;
	LPAnimObj *objs;

	for( i=0; i< a->NbObjs; i++)
		if( strcmp( a->Objs[i]->Nom , ObjName ) == 0 )
			return false;

	objs = new LPAnimObj[ a->NbObjs+1 ];
	for( i=0; i< a->NbObjs; i++)
		objs[i] = a->Objs[i];

	delete a->Objs;
	a->Objs = objs;

	objs[i] = new TAnimObj;
	objs[i]->Nom = new char[strlen(ObjName)+1];
	strcpy( objs[i]->Nom, ObjName );

	objs[i]->CurFrame = 0;
	objs[i]->CurTime = 0;
	objs[i]->CurState = 0;
	objs[i]->FrameCount = 0;
	objs[i]->Frames = NULL;
	objs[i]->CodeFin = BOUCLE;
	objs[i]->ParamFin = 0;
	objs[i]->CodeDebut = (i==0)?PRINCIPAL:NORMAL;
	objs[i]->ParamDebut = 0;

	a->NbObjs++;
	return true;
}

/* ******************************************************************** */

void
CAnims::DelObj( LPAnimList a, LPAnimObj o )
{
	int i,j;
	if( a == NULL || o == NULL )
		return;

	for( i=0; i< a->NbObjs; i++ )
	{
		if( a->Objs[i]->CodeDebut == SYNCHRO && a->Objs[i]->ParamDebut == (DWORD) o )
		{
			a->Objs[i]->CodeDebut = NORMAL;
			a->Objs[i]->ParamDebut = 0;
		}
	}

	for( i=0; i< a->NbObjs; i++ )
		if( a->Objs[i] == o )
		{
			while( i < a->NbObjs-1 )
			{
				a->Objs[i] = a->Objs[i+1];
				i++;
			}
			a->NbObjs--;
			for( j=0; j< o->FrameCount; j++ )
				delete o->Frames[j];
			delete o->Frames;
			delete o->Nom;
			delete o;
			return;
		}
}

/* ******************************************************************** */

void
CAnims::AddFrame( LPAnimObj o, DWORD ID )
{
	LPAnim *Frames;
	int i;

	Frames = new LPAnim[ o->FrameCount + 1 ];
	for(i=0; i< o->FrameCount; i++ )
		Frames[i] = o->Frames[i];
	Frames[i] = new TAnim;
	Frames[i]->IDSprite = ID;
	Frames[i]->Time = 200;
	Frames[i]->dx = 0;
	Frames[i]->dy = 0;
	Frames[i]->Flag = 0;
	Frames[i]->PFlag = 0;
	
	delete o->Frames;
	o->Frames = Frames;
	o->FrameCount++;
}

/* ******************************************************************** */

void
CAnims::Detruit( LPAnimList l, bool findRefs )
{
	int i,j;
	LPAnimList li = Liste;

	if( findRefs )
		while( li != NULL )
		{
			for( i=0; i< li->NbObjs; i++ )
				if( li->Objs[i]->CodeFin == CHANGE_ANIM && li->Objs[i]->ParamFin == (DWORD)l )
					li->Objs[i]->CodeFin = BOUCLE;
			li = li->Suivant;
		}

	for(i=0; i<l->NbObjs; i++)
	{
		for(j = 0; j< l->Objs[i]->FrameCount; j++ )
			delete l->Objs[i]->Frames[j];
		delete l->Objs[i]->Frames;
		delete l->Objs[i]->Nom;
		delete l->Objs[i];
	}
	delete l->Objs;
	delete l->Name;
	delete l;
}

/* ******************************************************************** */

void
CAnims::Clean()
{
	LPAnimList l = Liste;

	while( l != NULL )
	{
		Liste = l->Suivant;
		Detruit( l, false );
		l = Liste;
	}
}

/* ******************************************************************** */

LPAnimList
CAnims::CreateEmpty( char * Name )
{
	LPAnimList l = new TAnimList;

	l->Name = new char[ strlen(Name)+1 ];
	l->NbObjs = 0;
	l->Objs = NULL;
	l->Suivant = Liste;
	l->params[0] = l->params[1] = l->params[2] = 0;

	Liste = l;
	strcpy( l->Name, Name );

	return l;
}

/* ******************************************************************** */

LPAnimList
CAnims::FindByName( char * Name )
{
	LPAnimList l = Liste;
	while( l != NULL )
	{
		if( strcmp( Name, l->Name ) == 0 )
			return l;
		l = l->Suivant;
	}
	return NULL;
}

/* ******************************************************************** */

bool
CAnims::DeleteAnim( char *Name )
{
	LPAnimList l = Liste, a;

	if( l == NULL )
		return false;

	if( strcmp( Name, l->Name ) == 0 )
	{
		a = l;
		Liste = a->Suivant;
		Detruit( a, true );
		return true;
	}

	while( l->Suivant != NULL )
	{
		if( strcmp( Name, l->Suivant->Name ) == 0 )
		{
			a = l->Suivant;
			l->Suivant = a->Suivant;
			Detruit( a, true );
			return true;
		}
		l = l->Suivant;
	}
	return false;
}

/* ******************************************************************** */

void
CAnims::Init()
{
	LastTime = Ecran.CurTicks;
}

/* ******************************************************************** */

LPAnimList
CAnims::Play( LPAnimList a, bool Slow, int Zoom )
{
	int i,j;
	DWORD Diff = Ecran.CurTicks-LastTime;
	LPAnimObj o;
	LPAnim f;
	LastTime = Ecran.CurTicks;
	if( Slow )
		Diff /= 10;

for(i=0; i< a->NbObjs; i++ )
{
	o = a->Objs[i];
	o->CurTime+=Diff;
	while(1)
	{
		switch( o->CurState )
		{
/* ******************************************************************** */
//DEBUT
		case 0:
		case 1:
			switch( o->CodeDebut )
			{
			case WAIT_TIME:
				if( o->CurTime >= (long)(o->ParamDebut*10) )
				{
					o->CurTime-=(o->ParamDebut*10);
					o->CurState = 2;
				}
				break;
			case WAIT_FRAME:
				for(j=0; j<a->NbObjs; j++ )
					if( a->Objs[j]->CodeDebut == PRINCIPAL )
						break;
				if( j == a->NbObjs )
				{
					o->CodeDebut = NORMAL;
					continue;
				}
				else
				{
					if( a->Objs[j]->CurFrame == (long)o->ParamDebut )
					{
						o->CurTime = a->Objs[j]->CurTime;
						if( j > i )
							o->CurTime+=Diff;
						o->CurState = 2;
					}
				}

				break;
			case SYNCHRO:
				o->CurFrame = ((LPAnimObj)o->ParamDebut)->CurFrame;
				o->CurTime = 0;
				if( ((LPAnimObj)o->ParamDebut)->CurState && o->CurFrame < o->FrameCount )
				{
					f = o->Frames[o->CurFrame];
					if( Zoom == 100 )
						Sprites->Affiche( (RES_X>>1)+f->dx, 120+f->dy, f->IDSprite );
					else
						Sprites->AfficheZoom( (RES_X>>1)+(f->dx*Zoom)/100, 120+(f->dy*Zoom)/100, f->IDSprite, Zoom );
				}
				break;
			default:
				o->CurState = 2;
				break;
			}
			if( o->CurState > 1 )
				continue;
			break;
/* ******************************************************************** */
// ANIM
		case 2:
			if( o->FrameCount == 0 )
				break;
			while( o->CurTime >= (long)(o->Frames[ o->CurFrame ]->Time-(o->Frames[ o->CurFrame ]->Time%10)) )
			{
				o->CurTime-=o->Frames[ o->CurFrame ]->Time-(o->Frames[ o->CurFrame ]->Time%10);
				o->CurFrame++;
				if( o->CurFrame >= o->FrameCount )
				{
					o->CurFrame = o->FrameCount-1;
					o->CurState = 3;
					break;
				}
			}
			if( o->CurState == 3 )
				continue;
			else
			{
				LPAnim f = o->Frames[o->CurFrame];
				if( Zoom == 100 )
					Sprites->Affiche( (RES_X>>1)+f->dx, 120+f->dy, f->IDSprite );
				else
					Sprites->AfficheZoom( (RES_X>>1)+(f->dx*Zoom)/100, 120+(f->dy*Zoom)/100, f->IDSprite, Zoom );

			}
			break;
/* ******************************************************************** */
// FIN
		case 3:
		case 4:
			switch( o->CodeFin )
			{
			case BOUCLE:
				o->CurFrame = 0;
				o->CurState = 0;
				break;
			case CHANGE_ANIM:
				o->CurFrame = 0;
				o->CurTime = 0;
				o->CurState = 0;
				a = (LPAnimList)o->ParamFin;
				for(j=0; j< a->NbObjs; j++)
				{
					a->Objs[j]->CurFrame = 0;
					a->Objs[j]->CurTime = 0;
					a->Objs[j]->CurState = 0;
				}
				return a;
			case WAIT_RANDOM_GLOBAL:
				if( o->CurState == 3 )
				{
					long r;
					r = rand()%((o->ParamFin*10)+1);
					for( j=0; j< a->NbObjs; j++ )
						if( a->Objs[j]->CodeFin == WAIT_RANDOM_GLOBAL )
						{
							a->Objs[j]->CurTime-=r;
							a->Objs[j]->CurState = 4;
						}
				}
				else
				{
					if( a->Objs[i]->CurTime >= 0 )
					{
						o->CurFrame = 0;
						o->CurState = 0;
					}
				}
				break;
			case WAIT_RANDOM:
				if( o->CurState == 3 )
				{
					o->CurTime-=rand()%((o->ParamFin*10)+1);
					o->CurState = 4;
				}
				else
				{
					if( a->Objs[i]->CurTime >= 0 )
					{
						o->CurFrame = 0;
						o->CurState = 0;
					}
				}
				break;
			case KILL:
			case STOP:
			default:
				break;
			}
			if( o->CurState < 3 )
				continue;
			else
			{
				f = o->Frames[o->CurFrame];
				if( Zoom == 100 )
					Sprites->Affiche( (RES_X>>1)+f->dx, 120+f->dy, f->IDSprite );
				else
					Sprites->AfficheZoom( (RES_X>>1)+(f->dx*Zoom)/100, 120+(f->dy*Zoom)/100, f->IDSprite, Zoom );

			}
			break;
/* ******************************************************************** */
		}
		break;
	}
}
	return a;
}


/* ******************************************************************** */

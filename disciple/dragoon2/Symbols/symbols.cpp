#include <windows.h>
#include <stdio.h>
#include "symbols.h"
#include "erreurs.h"
#include "plugin.h"

class CSymbols *Symbols;
char SautLigne[2] = {13,10};

CSymbols::CSymbols()
{
	Nom = NULL;
	SymbList = NULL;
}

CSymbols::~CSymbols()
{
	LPSymbol s = SymbList;
	while( s != NULL )
	{
		SymbList = s->Suivant;
		delete s;
		s = SymbList;
	}
	delete Nom;
	Nom = NULL;
}

void
CSymbols::AddSymbol( long ID, char *txt, BYTE grp ) 
{
	LPSymbol s = new TSymbol;
	s->ID = ID;
	s->Grp = grp;
	strcpy( s->Symbol, txt );
	s->Suivant = SymbList;
	SymbList = s;
}

LPSymbol
CSymbols::Find( LPParam p )
{
	LPSymbol s = SymbList;
	while( s != NULL )
	{
		if( s->ID == p->ID && s->Grp == p->Grp )
			return s;
		s = s->Suivant;
	}
	return NULL;
}

LPSymbol
CSymbols::FindByName( char *txt )
{
	LPSymbol s = SymbList;
	while( s != NULL )
	{
		if( strcmp( s->Symbol,txt) == 0 )
			return s;
		s = s->Suivant;
	}
	return NULL;
}

void
CSymbols::DelSymbol ( LPSymbol s )
{
	LPSymbol sl = SymbList;
	if( sl == NULL )
		return;
	if( sl == s )
	{
		SymbList = s->Suivant;
		delete s;
		return;
	}
	while( sl->Suivant != NULL )
	{
		if( sl->Suivant == s )
		{
			sl->Suivant = s->Suivant;
			delete s;
			return;
		}
		sl = sl->Suivant;
	}
}

void
CSymbols::Save()
{
	FILE *f = fopen(Nom, "wb" );
	LPSymbol s;
	char buf[20];
	TSymbol *Liste[256];
	TSymbol *NListe[256];
	TSymbol tmp;
	long	counts[256];
	long i,j,k;
	BYTE g;

	if( f == NULL )
		return;

	for( i=0; i<256; i++)
	{
		Liste[i] = NULL;
		counts[i] = 0;
	}

	s = SymbList;
	while( s != NULL )
	{
		g = s->Grp;
		NListe[g] = new TSymbol[counts[g]+1];
		for( i=0; i< counts[g]; i++ )
			NListe[g][i] = Liste[g][i];
		NListe[g][i] = *s;
		counts[g]++;
		delete Liste[g];
		Liste[g] = NListe[g];
		s = s->Suivant;
	}


	fwrite("// symbols definition",1,21,f);
	fwrite(SautLigne, 1,2, f);
	fwrite("#pragma once", 1, 12, f );
	fwrite(SautLigne, 1,2, f);
	fwrite(SautLigne, 1,2, f);

	for( i=255; i>=0; i-- )
	{
		if( counts[i] > 0 )
		{
			fprintf(f,"/* definitions for group %d */", i );
			fwrite( SautLigne, 1,2,f);
		}
		for( j=0; j< counts[i]; j++ )
			for( k = 0; k< counts[i]-j-1; k++ )
				if( Liste[i][k].ID < Liste[i][k+1].ID )
				{
					tmp = Liste[i][k];
					Liste[i][k] = Liste[i][k+1];
					Liste[i][k+1] = tmp;
				}

		for( j=0; j< counts[i]; j++ )
		{
			fwrite("//",1,2,f );
			fwrite( Liste[i][j].Symbol, 1, strlen(Liste[i][j].Symbol), f );
			fwrite( ": ",1,2,f);
			_ltoa( Liste[i][j].Grp, buf, 10 );
			fwrite( buf, 1, strlen(buf), f );
			fwrite( "* ",1,2,f);
			_ltoa( Liste[i][j].ID, buf, 10 );
			fwrite( buf, 1, strlen( buf ), f);
			fwrite( SautLigne, 1,2,f);

			fwrite("#define ",1,8,f);
			fwrite( Liste[i][j].Symbol,1,strlen(Liste[i][j].Symbol),f);
			DWORD d = 0x09090909;
			fwrite( &d, 1, 4, f );
			fwrite( buf, 1, strlen( buf ), f);
			fwrite( SautLigne,1,2,f);
			
		}
		if( counts[i] > 0 )
		{
			fwrite( SautLigne,1,2,f);
			fwrite( SautLigne,1,2,f);
		}
	}
	fclose(f);
}

void
CSymbols::Load()
{
	FILE *f = fopen( Nom, "rb");
	char buf[256];
	char nom[256];
	char grp[256];
	int pos;
	int step;

	if( f == NULL )
		return;
	
	pos = 0;
	step = 0;

	while( fread( buf+pos, 1,1,f ) == 1 )
	{
		switch( buf[pos] )
		{
		case ':':
			buf[pos] = 0;
			strcpy(nom,buf);
			pos = 0;
			step = 1;
			break;
		case '*':
			buf[pos] = 0;
			strcpy(grp,buf);
			pos = 0;
			if( step == 1 )
				step = 2;
			else
				step = 0;
			break;
		case 13:
			fread( buf+pos,1,1,f );
		case '/':
			buf[pos] = 0;
			if( step == 2 )
				AddSymbol( atol( buf+1 ), nom, atoi(grp+1) );
			step = 0;
			pos = 0;
			break;
		default:
			pos++;
			break;
		}
	}
	fclose(f);
}

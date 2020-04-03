/* ******************************************************************** */
/* dlib.cpp																*/
/* ******************************************************************** */
#include <stdio.h>
#include "dlib.h"
#include "erreurs.h"

/* ******************************************************************** */
CDLib *DLib = NULL;
/* ******************************************************************** */

CDLib::CDLib()
{
	PakMax = 0;
	Paks = NULL;
}

/* ******************************************************************** */

CDLib::~CDLib()
{
	int i,j;

	for( i=0; i< PakMax; i++)
	{
		if( Paks[i] != NULL )
		{
			for(j=0; j< Paks[i]->ResMax; j++ )
				delete Paks[i]->Ressources[j];
			delete Paks[i]->Ressources;
			delete Paks[i]->Name;
			delete Paks[i];
		}
	}
	delete Paks;
}

/* ******************************************************************** */

void *
CDLib::Load( DWORD ID, LOADFONCTION LFunc )
{
	FILE *f;
	void *data;
	int i;
	WORD PakNb = (WORD)(ID>>16);
	WORD ResID = (WORD)(ID&65535);

	if( PakNb < 1 )
	{
		ResNonValide("????", ID);
		return NULL;
	}
	if( PakNb > PakMax || Paks[PakNb-1] == NULL )
	{
		ResNonTrouve("Pak is not loaded", ID);
		return NULL;
	}
	for( i=0; i< Paks[PakNb-1]->ResMax; i++ )
	{
		if( ResID == Paks[PakNb-1]->Ressources[i]->ID  )
		{
			if( Paks[PakNb-1]->Ressources[i]->Loaded )
				return Paks[PakNb-1]->Ressources[i]->Data;

			f = fopen(Paks[PakNb-1]->Name,"rb");
			if( f == NULL )
			{
				FichierNonTrouve(Paks[PakNb-1]->Name);
				return NULL;
			}
			fseek( f, Paks[PakNb-1]->Ressources[i]->Pos, 0 );
			data = LFunc( f );
			if( (DWORD)data == 0xFFFFFF00 )
			{
				ResNonValide(Paks[PakNb-1]->Name,ResID);
				return NULL;
			}
			Paks[PakNb-1]->Ressources[i]->Loaded = true;
			Paks[PakNb-1]->Ressources[i]->Data = data;
			return data;
		}
	}
	ResNonValide(Paks[PakNb-1]->Name, ID);
	return NULL;
}

/* ******************************************************************** */

void *
CDLib::LoadTmp( DWORD ID, LOADFONCTION LFunc )
{
	FILE *f;
	void *data;
	WORD PakNb = (WORD)(ID>>16);
	WORD ResID = (WORD)(ID&65535);
	int i;

	if( PakNb < 1 )
	{
		ResNonValide("????", ID);
		return NULL;
	}
	if( PakNb > PakMax || Paks[PakNb-1] == NULL )
	{
		ResNonTrouve("Pak is not loaded", ID);
		return NULL;
	}
	for( i=0; i< Paks[PakNb-1]->ResMax; i++ )
	{
		if( ResID == Paks[PakNb-1]->Ressources[i]->ID  )
		{
			if( Paks[PakNb-1]->Ressources[i]->Loaded )
				return Paks[PakNb-1]->Ressources[i]->Data;

			f = fopen(Paks[PakNb-1]->Name,"rb");
			if( f == NULL )
			{
				FichierNonTrouve(Paks[PakNb-1]->Name);
				return NULL;
			}
			fseek( f, Paks[PakNb-1]->Ressources[i]->Pos, 0 );
			data = LFunc( f );
			if( (DWORD)data == 0xFFFFFF00 )
			{
				ResNonValide(Paks[PakNb-1]->Name,ResID);
				return NULL;
			}
			return data;
		}
	}
	ResNonValide(Paks[PakNb-1]->Name, ID);
	return NULL;
}

/* ******************************************************************** */

void
CDLib::LoadTmp( DWORD ID, LOADINFONCTION LFunc, void *buf )
{
	FILE *f;
	WORD PakNb = (WORD)(ID>>16);
	WORD ResID = (WORD)(ID&65535);
	int i;

	if( PakNb < 1 )
	{
		ResNonValide("????", ID);
		return;
	}
	if( PakNb > PakMax || Paks[PakNb-1] == NULL )
	{
		ResNonTrouve("Pak is not loaded", ID);
		return;
	}
	for( i=0; i< Paks[PakNb-1]->ResMax; i++ )
	{
		if( ID == Paks[PakNb-1]->Ressources[i]->ID  )
		{
			f = fopen(Paks[PakNb-1]->Name,"rb");
			if( f == NULL )
			{
				FichierNonTrouve(Paks[PakNb-1]->Name);
				return;
			}
			fseek( f, Paks[PakNb-1]->Ressources[i]->Pos, 0 );
			if( LFunc( f,buf ) == -1 )
				ResNonValide(Paks[PakNb-1]->Name,ResID);
			return;
		}
	}

	ResNonValide(Paks[PakNb-1]->Name, ID);
}

/* ******************************************************************** */

void *
CDLib::Free( DWORD ID )
{
	void *data;
	WORD PakNb = (WORD)(ID>>16);
	WORD ResID = (WORD)(ID&65535);
	int i;

	if( PakNb < 1 || PakNb > PakMax || Paks[PakNb-1] == NULL )
		return NULL;
	for( i=0; i< Paks[PakNb-1]->ResMax; i++ )
	{
		if( ResID == Paks[PakNb-1]->Ressources[i]->ID  )
		{
			Paks[PakNb-1]->Ressources[i]->Loaded = false;
			data = Paks[PakNb-1]->Ressources[i]->Data;
			Paks[PakNb-1]->Ressources[i]->Data = NULL;
			return data;
		}
	}
	return NULL;
}

/* ******************************************************************** */

bool
CDLib::LoadPak( LPCSTR Nom )
{
	char File[1024];
	FILE *f;
	TPak **NewPaks;
	char buf[4];
	WORD p;
	WORD c;
	BYTE b;
	DWORD d;
	int i;

	strcpy(File, Nom );
//	strcat(File, ".pak" );
	f = fopen( File, "rb");

	if( f == NULL )
	{
		FichierNonTrouve(File);
		return false;
	}	
	fread(buf,1,3,f);
	buf[3] = 0;
	if( strcmp(buf, "PAK" ) != 0 )
	{
		fclose(f);
		FichierNonValide(File);
		return false;
	}
	fread(&p,1,2,f);
	if( p < PakMax && Paks[p] != NULL )
	{
		if( strcmp(File,Paks[p]->Name) != 0 )
		{
			Log(File);
			Log(Paks[p]->Name);
			fclose(f);
			FatalError("DLIB/Pak overridding !!!");
			return false;
		}		
		for(i=0; i< Paks[p]->ResMax; i++ )
			delete Paks[p]->Ressources[i];
		delete Paks[p]->Ressources;
		delete Paks[p]->Name;
		delete Paks[p];
	}
	if( p >= PakMax )
	{
		NewPaks = new TPak*[p+1];
		for(i=0; i<PakMax; i++)
			NewPaks[i] = Paks[i];
		for(i=PakMax; i<p+1; i++)
			NewPaks[i] = NULL;
		delete Paks;
		Paks = NewPaks;
 		PakMax = p+1;
	}
	Paks[p] = new TPak;
	Paks[p]->Name = new char[strlen(File)+1];
	strcpy(Paks[p]->Name,File);
	fread(&c,1,2,f);
	Paks[p]->ResMax = c;
	Paks[p]->Ressources = new TPak::TRes*[c];
	for( i=0; i< Paks[p]->ResMax; i++)
		Paks[p]->Ressources[i] = NULL;

	for( i=0; i< Paks[p]->ResMax; i++)
	{
		Paks[p]->Ressources[i] = new TPak::TRes;
		fread( &c,1,2,f);
		Paks[p]->Ressources[i]->ID = c + ((p+1)<<16);
		fread(&b,1,1,f);
		fseek(f,b,1);
		fread(&b,1,1,f);
		fseek(f,b,1);
		Paks[p]->Ressources[i]->Data = NULL;
		Paks[p]->Ressources[i]->Loaded = false;
		Paks[p]->Ressources[i]->Pos = ftell(f);
		fread(&b,1,1,f);
		fread(&d,1,4,f);
		fseek(f,d,1);
		fread(&d,1,4,f);
		if( d != 0xDAAF0BBC )
		{
			fclose(f);
			FichierNonValide(File);
			return false;
		}
	}
	fclose(f);
	return true;
}

/* ******************************************************************** */
// fin de dlib.cpp
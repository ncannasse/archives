#include "erreurs.h"
#include "plugin.h"
#include "data.h"

const char SautLigne[3] = { 13, 10, 0 };

CData *Datas = NULL;

CData::CData()
{
	lst = NULL;
	data = NULL;
	Nom = NULL;
}

CData::~CData()
{
	LPStruct s;
	LPSData d;
	
	while( data != NULL )
	{
		d = data->Suivant;
		Free( data );
		data = d;
	}

	while( lst != NULL )
	{
		s = lst->Suivant;
		Free(lst);
		lst = s;
	}

	delete Nom;
}

void
CData::Free( LPStruct s )
{
	int i;
	for( i=0; i< s->NbFields; i++ )
		delete s->Fields[i];
	delete s->Fields;
	delete s;
}

void
CData::FreeData( BYTE *Val, int Type )
{
	switch( Type )
	{
	case 0:
		delete ((char*)Val);
		break;
	case 1:
		delete ((short*)Val);
		break;
	case 2:
		delete ((BYTE*)Val);
		break;
	case 3:
		delete ((WORD*)Val);
		break;
	case 4:
		delete ((long*)Val);
		break;
	case 5:
		delete ((DString*)Val);
		break;
	}
}

void
CData::Free( LPSData d )
{
	int i;
	for( i=0; i< d->S->NbFields; i++ )
		FreeData( d->FVals[i], d->S->Fields[i]->Type );
	delete d->FVals;
	delete d;
}

bool
CData::AddStruct( char *Name )
{
	LPStruct s = lst;
	DWORD ID = lst!=NULL?(lst->ID+1):0;
	if( Name == NULL )
		return false;
	while( s != NULL )
	{
		if( s->Name == Name )
			return false;
		s = s->Suivant;
	}
	s = lst;
	while( s != NULL )
	{
		if( s->ID == ID )
		{
			ID++;
			s = lst;
		}
		else
			s = s->Suivant;
	}
	s = new TStruct;
	s->Fields = NULL;
	s->ID = ID;
	s->Name = Name;
	s->NbFields = 0;
	s->Suivant = NULL;
	if( lst == NULL )
		lst = s;
	else
	{
		LPStruct s2 = lst;
		while( s2->Suivant != NULL )
			s2 = s2->Suivant;
		s2->Suivant = s;
	}
	return true;
}

void
CData::DelStruct( LPStruct s )
{
	LPStruct st = lst;
	if( lst == NULL || s == NULL )
		return;
	if( lst == s )
	{
		lst = s->Suivant;
		Free(s);
		return;
	}
	while( st->Suivant != NULL )
	{
		if( st->Suivant == s )
		{
			st->Suivant = s->Suivant;
			Free(s);
			return;
		}
		st = st->Suivant;
	}
}

bool
CData::AddField( LPStruct s, char *FName, int Type )
{
	int i;
	LPField *Fields;
	BYTE **Vals;

	if( s == NULL || FName == NULL )
		return false;
	for( i=0; i< s->NbFields; i++ )
		if( s->Fields[i]->FieldName == FName )
			return false;

	Fields = new LPField[ s->NbFields+1 ];
	for( i=0; i< s->NbFields; i++ )
		Fields[i] = s->Fields[i];

	Fields[i] = new TField;
	Fields[i]->FieldName = FName;
	Fields[i]->Type = Type;
	delete s->Fields;
	s->Fields = Fields;
	s->NbFields++;

	LPSData d = data;
	while( d != NULL )
	{
		if( d->S == s )
		{
			Vals = new BYTE*[ s->NbFields ];
			memcpy( Vals, d->FVals, 4*(s->NbFields-1) );
			
			switch( Type )
			{
			case 0:
				Vals[ s->NbFields-1 ] = (BYTE *)new char;
				*((char*)Vals[ s->NbFields-1 ]) = 0;
				break;
			case 1:
				Vals[ s->NbFields-1 ] = (BYTE *)new short;
				*((short*)Vals[ s->NbFields-1 ]) = 0;
				break;
			case 2:
				Vals[ s->NbFields-1 ] = (BYTE *)new BYTE;
				*((BYTE*)Vals[ s->NbFields-1 ]) = 0;
				break;
			case 3:
				Vals[ s->NbFields-1 ] = (BYTE *)new WORD;
				*((WORD*)Vals[ s->NbFields-1 ]) = 0;
				break;
			case 4:
				Vals[ s->NbFields-1 ] = (BYTE *)new long;
				*((long*)Vals[ s->NbFields-1 ]) = 0;
				break;
			case 5:
				Vals[ s->NbFields-1 ] = (BYTE *)new DString;
				break;
			}
			delete d->FVals;
			d->FVals = Vals;
		}
		d = d->Suivant;
	}
	return true;
}

void
CData::DelField( LPStruct s, int FIndex )
{
	int i;
	int Type;

	if( s == NULL || FIndex < 0 || FIndex >= s->NbFields )
		return;
	Type = s->Fields[FIndex]->Type;
	delete s->Fields[FIndex];
	for( i=FIndex; i< s->NbFields-1; i++)
		s->Fields[i] = s->Fields[i+1];
	s->NbFields--;

	LPSData d = data;
	while( d != NULL )
	{
		if( d->S == s )
		{
			FreeData( d->FVals[FIndex], Type );
			for(i=FIndex; i< s->NbFields;  i++ )
				d->FVals[i] = d->FVals[i+1];
		}
		d = d->Suivant;
	}
}

void
CData::MoveUpField( LPStruct s, int FIndex )
{
	LPField tmp;

	if( s == NULL || FIndex <= 0 || FIndex >= s->NbFields )
		return;

	tmp = s->Fields[FIndex];
	s->Fields[FIndex] = s->Fields[FIndex - 1];
	s->Fields[FIndex-1] = tmp;

	LPSData d = data;
	while( d != NULL )
	{
		if( d->S == s )
		{
			BYTE *vtmp;
			vtmp = d->FVals[FIndex];
			d->FVals[FIndex] = d->FVals[FIndex-1];
			d->FVals[FIndex-1] = vtmp;
		}
		d = d->Suivant;
	}
}

bool
CData::AddSData( char *Name, LPStruct s )
{
	int i;
	LPSData d = data;
	DWORD ID = 0;

	if( s == NULL || Name == NULL )
		return false;
	while( d != NULL )
	{
		if( d->Name == Name )
			return false;
		d = d->Suivant;
	}
	d = data;
	while( d != NULL )
	{
		if( d->ID == ID && d->S == s )
		{
			ID++;
			d = data;
		}
		else
			d = d->Suivant;
	}

	d = new TSData;
	d->Name = Name;
	d->S = s;
	d->ID = ID;
	d->FVals = new BYTE*[ s->NbFields ];
	for(i=0; i< s->NbFields; i++ )
		switch( s->Fields[i]->Type )
		{
		case 0:
			d->FVals[i] = (BYTE *)new char;
			*((char *)d->FVals[i]) = 0;
			break;
		case 1:
			d->FVals[i] = (BYTE *)new short;
			*((short *)d->FVals[i]) = 0;
			break;
		case 2:
			d->FVals[i] = (BYTE *)new BYTE;
			*((BYTE *)d->FVals[i]) = 0;
			break;
		case 3:
			d->FVals[i] = (BYTE *)new WORD;
			*((WORD *)d->FVals[i]) = 0;
			break;
		case 4:
			d->FVals[i] = (BYTE *)new long;
			*((long *)d->FVals[i]) = 0;
			break;
		case 5:
			d->FVals[i] = (BYTE *)new DString;
			break;
		}	
	
	d->Suivant = data;
	data = d;
	return true;
}

void
CData::DelSData( LPSData d  )
{
	LPSData ld = data;
	if( data == NULL || d == NULL )
		return;
	if( data == d )
	{
		data = d->Suivant;
		Free(d);
		return;
	}
	while( ld->Suivant != NULL )
	{
		if( ld->Suivant == d )
		{
			ld->Suivant = d->Suivant;
			Free(d);
			return;
		}
		ld = ld->Suivant;
	}
}

void
CData::ChangeType( LPStruct s, int FIndex, int Type )
{
	LPSData d = data;
	if( s->Fields[FIndex]->Type == Type || FIndex < 0 || FIndex >= s->NbFields )
		return;

	while( d != NULL )
	{
		if( d->S == s )
		{
			FreeData( d->FVals[FIndex], s->Fields[FIndex]->Type );
			switch( Type )
			{
			case 0:
				d->FVals[FIndex] = (BYTE *)new char;
				*((char *)d->FVals[FIndex]) = 0;
				break;
			case 1:
				d->FVals[FIndex] = (BYTE *)new short;
				*((short *)d->FVals[FIndex]) = 0;
				break;
			case 2:
				d->FVals[FIndex] = (BYTE *)new BYTE;
				*((BYTE *)d->FVals[FIndex]) = 0;
				break;
			case 3:
				d->FVals[FIndex] = (BYTE *)new WORD;
				*((WORD *)d->FVals[FIndex]) = 0;
				break;
			case 4:
				d->FVals[FIndex] = (BYTE *)new long;
				*((long *)d->FVals[FIndex]) = 0;
				break;
			case 5:
				d->FVals[FIndex] = (BYTE *)new DString;
				break;
			}
		}
		d = d->Suivant;
	}
	s->Fields[FIndex]->Type = Type;
}

void
CData::Save()
{
	FILE *f;
	DWORD d;
	BYTE len;
	int i;

	if( lst == NULL )
		return;

	LPStruct s = lst;
	LPSData dat;

	d = 0;
	while( s != NULL )
	{
		s = s->Suivant;
		d++;
	}

	f = fopen( Nom, "wb" );
	if( f != NULL )
	{
		LPStruct s = lst;
		fwrite("DAT",1,3,f);
		fwrite( &d, 1, 4, f );
		while( s != NULL )
		{
			len = s->Name.Length();
			d = 1+4+len;
			fwrite( &d,1,4,f );
			fwrite( &s->ID,1,4,f);
			fwrite( &len, 1, 1, f);
			fwrite( s->Name.c_str(), 1, len, f);

			d = s->NbFields;
			fwrite( &d,1,4,f );

			for( i=0; i< s->NbFields; i++ )
			{
				len = s->Fields[i]->FieldName.Length();
				d = 2 + len;
				fwrite( &d, 1,4,f);
				fwrite( &len, 1,1, f );
				fwrite( s->Fields[i]->FieldName.c_str(), 1, len, f);
				fwrite( &s->Fields[i]->Type, 1, 1, f);
			}
			
			d = 0 ;
			dat = data;
			while( dat != NULL )
			{
				if( dat->S == s )
					d++;
				dat = dat->Suivant;
			}
			fwrite( &d, 1, 4, f );
			dat = data;
			while( dat != NULL )
			{
				if( dat->S == s )
				{
					len = dat->Name.Length();
					d = len + 5;
					for( i=0; i < s->NbFields; i++ )
						switch( s->Fields[i]->Type )
						{
						case 0:
						case 2:
							d++;
							break;
						case 1:
						case 3:
							d+=2;
							break;
						case 4:
							d+=4;
							break;
						case 5:
							d+= (*((DString *)dat->FVals[i])).Length() + 1;
							break;
						}
					fwrite( &d, 1, 4, f );
					fwrite( &len, 1,1, f);
					fwrite( dat->Name.c_str(), 1, len, f );
					fwrite( &dat->ID, 1, 4, f );
					d = 0;
					for( i=0; i< s->NbFields; i++ )
					{
						switch( s->Fields[i]->Type )
						{
						case 0:
							fwrite( (char*)dat->FVals[i], 1, 1, f );
							break;
						case 1:
							fwrite( (short*)dat->FVals[i], 1, 2, f );
							break;
						case 2:
							fwrite( (BYTE*)dat->FVals[i], 1, 1, f );
							break;
						case 3:
							fwrite( (WORD*)dat->FVals[i], 1, 2, f );
							break;
						case 4:
							fwrite( (long*)dat->FVals[i], 1, 4, f );
							break;
						case 5:
							len = (*((DString*)dat->FVals[i])).Length();
							fwrite( &len, 1, 1, f);
							fwrite( (*((DString*)dat->FVals[i])).c_str(), 1, len, f );
							break;
						}
					}
				}
				dat = dat->Suivant;
			}
			s = s->Suivant;
		}
		fclose(f);
	}

//  <<GENERATION d'un .h>>

/*	strcpy( Nom+strlen(Nom)-3,"h");
	f = fopen( Nom, "wb" );

	if( f != NULL )
	{
		fprintf(f,"// data symbols definitions%s", SautLigne);
		fprintf(f,"#pragma once%s%s",SautLigne, SautLigne);
		s = lst;
		while( s != NULL )
		{
			fprintf(f,"#define %s %ld%s", s->Name.c_str(), s->ID,SautLigne );
			dat = data;
			while( dat != NULL )
			{
				if( dat->S == s )
					fprintf(f,"    #define %s %ld%s", dat->Name.c_str(), dat->ID, SautLigne );
				dat = dat->Suivant;
			}
			s = s->Suivant;
			fprintf(f,"%s", SautLigne );
		}
		fclose(f);
	}
	strcpy( Nom+strlen(Nom)-1,"dat");*/
}

void
CData::Load()
{
	int i;
	FILE *f;
	BYTE len;
	char buf[256];
	BYTE *dat,*cur;
	DWORD d, count, fcount,c;

	f = fopen( Nom, "rb" );
	if( f != NULL )
	{
		LPStruct s;
		fread(buf, 1, 3, f);
		buf[3] = 0;
		if( strcmp( buf, "DAT") != 0 )
			return;
		fread( &count, 1, 4, f );
		while( count > 0 )
		{
			fread(&d,1,4,f);
			dat = new BYTE[ d ];
			fread(dat,1,d,f);
			fread( &fcount, 1,4, f);			

			s = new TStruct;
			s->ID = *((DWORD *)dat);
			dat+=4;
			memcpy( buf, dat+1, *dat );
			buf[ *dat ] = 0;
			s->Name = buf;
			s->Fields = new LPField[fcount];
			s->NbFields = fcount;
			s->Suivant = NULL;
			if( lst == NULL )
				lst = s;
			else
			{
				LPStruct ss = lst;
				while( ss->Suivant != NULL )
					ss = ss->Suivant;
				ss->Suivant = s;
			}
			dat-=4;

			delete dat;

			for(c=0;c<fcount;c++)
			{
				s->Fields[c] = new TField;
				fread( &d, 1,4,f);
				dat = new BYTE[d];
				fread(dat,1,d,f);
				memcpy( buf, dat+1, *dat );
				buf[*dat] = 0;
				s->Fields[c]->FieldName = buf;
				s->Fields[c]->Type = *(dat+1+(*dat));
				delete dat;
			}
			
			fread( &fcount, 1,4,f );

			while( fcount > 0 )
			{
				fread(&d,1,4,f);
				dat = new BYTE[d];
				fread(dat,1,d,f);
				memcpy( buf, dat+1, *dat );
				buf[ *dat ] = 0;
				AddSData( buf, s );
				cur = dat + (*dat) + 1;
				data->ID =*((DWORD*)cur);
				cur+=4;
				for( i = 0; i< s->NbFields; i++ )
					switch( s->Fields[i]->Type )
					{
					case 0:
						*((char*)data->FVals[i]) = *((char*)cur);
						cur++;
						break;
					case 1:
						*((short*)data->FVals[i]) = *((short*)cur);
						cur+=2;
						break;
					case 2:
						*((BYTE*)data->FVals[i]) = *((BYTE*)cur);
						cur++;
						break;
					case 3:
						*((WORD*)data->FVals[i]) = *((WORD*)cur);
						cur+=2;
						break;
					case 4:
						*((long*)data->FVals[i]) = *((long*)cur);
						cur+=4;
						break;
					case 5:
						len = *((BYTE*)cur);
						cur++;
						memcpy(buf, cur, len );
						buf[len] = 0;
						*((DString*)data->FVals[i]) = buf;
						cur+=len;
						break;
					}
				delete dat;
				fcount --;
			}

			count--;
		}
		fclose(f);
	}
}
//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Folders_Defs.h"
#include "Main_Defs.h"
#include "Plugins.h"
//---------------------------------------------------------------------------
CFolders::CFolders( AnsiString Nom )
{
	folds = new TFolder;
	folds->Nom = Nom;
	folds->Fils = NULL;
	folds->NFils = 0;
	folds->Pere = NULL;
	folds->open = true;
}

CFolders::~CFolders()
{
	KillItem(folds);
}

void
CFolders::AddItem( AnsiString Nom, LPFolder pere )
{
	LPFolder *Fils;
	
	if( pere == NULL || GetItem(Nom, -1) != NULL || GetProf(pere)==9 )
		return;
	if( pere->Nom[1] != 'D' &&  pere->Nom[1] != 'd' )
	{
		pere = pere->Pere;
		if( pere == NULL )
			return;
	}
	
	Fils = new LPFolder[ pere->NFils+1 ];
	memcpy( Fils, pere->Fils, pere->NFils*sizeof(LPFolder) );
	Fils[ pere->NFils ] = new TFolder;
	Fils[ pere->NFils ]->Nom = Nom;
	Fils[ pere->NFils ]->NFils = 0;
	Fils[ pere->NFils ]->Fils = NULL;
	Fils[ pere->NFils ]->Pere = pere;
	Fils[ pere->NFils ]->open = true;
	delete pere->Fils;
	pere->Fils = Fils;
	pere->NFils++;
}

void
CFolders::Affiche( TListBox *Where )
{
	AfficheRec( folds, Where, 0 );
}

void
CFolders::AfficheRec( LPFolder f, TListBox *Where, int Prof )
{
	int i;
	if( f->open )
	{
		Where->Items->Add((AnsiString)Prof+f->Nom);
		for(i=0; i< f->NFils; i++ )
			AfficheRec( f->Fils[i], Where, Prof+1 );
	}
	else
	{
		f->Nom[1] += (char)('a'-'A');
		Where->Items->Add((AnsiString)Prof+f->Nom);
		f->Nom[1] -= (char)('a'-'A');
	}
}

LPFolder
CFolders::GetItem( int Index )
{
	return FindRec( folds, &Index );
}

LPFolder
CFolders::GetItem( AnsiString Nom, int Prof )
{
	return FindRec( folds, Nom, Prof );
}

int
CFolders::GetProf( LPFolder f )
{
	int i = 0;
	while( f->Pere != NULL )
	{
		f = f->Pere;
		i++;
	}
	return i;
}

LPFolder
CFolders::FindRec( LPFolder f, int *Index )
{
	LPFolder r;
	int i;
	
	if( *Index == 0 )
		return f;
	(*Index)--;
	
	if( f->open )
		for( i=0; i<f->NFils; i++ )
			if( (r=FindRec(f->Fils[i],Index)) != NULL )
				return r;
			return NULL;
}

LPFolder
CFolders::FindRec( LPFolder f, AnsiString Nom, int Prof )
{
	LPFolder r;
	int i;
	
	if( Prof<=0 && f->Nom == Nom )
		return f;
	
	if( Prof!=0 )
    {
		for(i=0; i<f->NFils; i++ )
			if( (r=FindRec( f->Fils[i], Nom, Prof-1)) != NULL )
				return r;
    }
	return NULL;
}

void
CFolders::DelItem( LPFolder f )
{
	LPFolder *Fils;
	int i;

	if( f != NULL )
        {
		Fils = new LPFolder[f->Pere->NFils-1+f->NFils];

		i = 0;
		while( f->Pere->Fils[i] != f )
		{
			Fils[i] = f->Pere->Fils[i];
			i++;
		}
		while( i+1 < f->Pere->NFils )
		{
			Fils[i] = f->Pere->Fils[i+1];
                        i++;
		}

		for(i=0; i< f->NFils; i++ )
                {
                         f->Fils[i]->Pere = f->Pere;
		         Fils[i+f->Pere->NFils-1] = f->Fils[i];
                }

		delete f->Pere->Fils;
		f->Pere->Fils = Fils;
		f->Pere->NFils += f->NFils-1;
		delete f->Fils;
		delete f;
    }
}

void
CFolders::KillItem( LPFolder f )
{
    if( f != NULL )
    {
	CutItem( f );
	KillRec(f);
    }
}

void
CFolders::KillRec( LPFolder f )
{
	int i;
	for( i = 0; i< f->NFils; i++ )
		KillRec( f->Fils[i] );
	delete f->Fils;
	delete f;
}

LPFolder
CFolders::CutItem( LPFolder f )
{
	int i;
	LPFolder *Fils;

	if( f == NULL || f->Pere == NULL )
		return NULL;
	Fils = new LPFolder[f->Pere->NFils-1];
	i = 0;
	while( f->Pere->Fils[i] != f )
        {
		Fils[i] = f->Pere->Fils[i];
		i++;
        }
	while( i+1 < f->Pere->NFils )
        {
		Fils[i] = f->Pere->Fils[i+1];
                i++;
        }

	delete f->Pere->Fils;
	f->Pere->Fils = Fils;
	f->Pere->NFils--;
	f->Pere = NULL;
        return f;
}

void
CFolders::InsItem( LPFolder pere, LPFolder f )
{
	if( f != NULL && pere != NULL )
		InsItem( pere, f, pere->NFils );
}

void
CFolders::InsItem( LPFolder pere, LPFolder f, int Pos )
{
	LPFolder *Fils;

	if( f != NULL && pere != NULL )
    {
		if( Pos < 1 )
			Pos = 1;
		if( Pos > pere->NFils+1 )
			Pos = pere->NFils+1;
		f->Pere = pere;
		Fils = new LPFolder[ pere->NFils + 1 ];
		memcpy( Fils, pere->Fils, (Pos-1)*sizeof( LPFolder ) );
		memcpy( Fils+Pos, pere->Fils+(Pos-1), (pere->NFils-Pos+1)*sizeof(
			LPFolder ) );
		Fils[ Pos-1 ] = f;
		delete pere->Fils;
		pere->Fils = Fils;
		pere->NFils++;
    }
}

void
CFolders::SaveItems( FILE *f, LPFolder fol )
{
    int i;
    AnsiString nom = fol->Nom;
    bool plug;
    nom.Delete(1,1);

    SautLigne(f);
    EcritTxt("[FOLDER]",f);
    SautLigne(f);
    EcritTxt("NAME="+nom,f);
    SautLigne(f);
    if( ! fol->open )
    {
        EcritTxt("OPEN=0",f);
        SautLigne(f);
    }

    for(i=0; i< fol->NFils; i++ )
    {
        plug = false;
        nom = fol->Fils[i]->Nom;
        switch( nom[1] )
        {
        case 'D':
             EcritTxt("FOLDER=",f);
             break;
        case 'P':
             EcritTxt("PACK=",f);
             break;
        case 'F':
             EcritTxt("FILETYPE=",f);
             break;
        default:
             char buf[1024];
             LPPlugin p;
             p = Plugins->GetPlugByChar( nom[1] );
             if( Plugins->RequestEvent(EVT_SAVE, p, nom.c_str()+1, buf ) )
             {
                 EcritTxt(buf,f);
                 SautLigne(f);
             }
             plug = true;
             break;
        }
        if( !plug )
        {
            nom.Delete(1,1);
            EcritTxt(nom,f);
            SautLigne(f);
        }
    }

    for(i=0; i< fol->NFils; i++ )
        if( fol->Fils[i]->Nom[1] == 'D' )
            SaveItems( f, fol->Fils[i] );
}

void *
CFolders::EnumItems( LPFolder f, ENUMPROC proc, void *param )
{
    int i;
    void *r;

    if( (r=proc( f, param ) ) != NULL )
        return r;
    if( f->open )
        for( i = 0; i< f->NFils; i++ )
             EnumItems( f->Fils[i], proc, param );
    return NULL;
}



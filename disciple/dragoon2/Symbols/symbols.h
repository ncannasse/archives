#pragma once

typedef struct TSymbol {

	long  ID;
	char  Symbol[256];
	BYTE  Grp;

	struct TSymbol *Suivant;

} TSymbol, *LPSymbol;


typedef struct TParam {

	long  ID;
	char  Symbol[256];
	int   Grp;


} *LPParam;


class CSymbols {


public:
	
	LPSymbol SymbList;

	char *Nom;

	LPSymbol Find( LPParam p );
	LPSymbol FindByName( char *Name );
	
	void	 DelSymbol ( LPSymbol s );
	void	 AddSymbol ( long ID, char *txt, BYTE grp );

	void Load();
	void Save();

	CSymbols();
	~CSymbols();
};

extern CSymbols *Symbols;
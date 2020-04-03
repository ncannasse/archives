#pragma once
#include <windows.h>

#define TYPE_PCX	75
#define	TYPE_GIF	54

typedef void *(*LOADFONCTION)( FILE * );
typedef int  (*LOADINFONCTION)( FILE *, void * );


class CDLib {

	int PakMax;

	struct TPak {
		char *Name;
		int ResMax;
		struct TRes {
			DWORD ID;
			DWORD Pos;
			bool Loaded;
			void *Data;
		} **Ressources;
	} **Paks;

public:


	void *Load		( DWORD ID, LOADFONCTION LFunc );
	void *LoadTmp	( DWORD ID, LOADFONCTION LFunc );
	
	void LoadTmp	( DWORD ID, LOADINFONCTION LFunc, void *buf );

	void *Free		( DWORD ID	);

	bool LoadPak	( LPCSTR Nom );

	CDLib();
	~CDLib();
};

extern CDLib *DLib;
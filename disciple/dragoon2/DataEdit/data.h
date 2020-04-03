#pragma once

typedef struct TField {
	
	DString FieldName;
	BYTE	Type;

} TField, *LPField;

typedef struct TStruct {

	DString Name;
	DWORD ID;

	int		NbFields;
	LPField *Fields;

	struct TStruct *Suivant;
	
} TStruct, *LPStruct;

typedef struct TSData {

	DString Name;
	LPStruct S;

	DWORD ID;
	BYTE **FVals;

	struct TSData *Suivant;

} TSData, *LPSData;

class CData {

public:

	char *Nom;

	LPStruct lst;
	LPSData  data;

	bool AddStruct	( char *Name ); // coucou je suis un commentaire
	void DelStruct	( LPStruct s );
	void Free		( LPStruct s );

	bool AddField( LPStruct s, char *FName, int Type );
	void MoveUpField( LPStruct s, int FIndex );
	void DelField( LPStruct s, int FIndex );

	bool AddSData( char *Name, LPStruct s );
	void DelSData( LPSData d  );
	void Free	 ( LPSData d  );

	void ChangeType( LPStruct s, int FIndex, int Type );
	void FreeData( BYTE *Val, int Type );

	void Load	 ();
	void Save	 ();

	CData();
	~CData();
};

extern CData *Datas;
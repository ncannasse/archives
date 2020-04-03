// types & class defs for animations
#pragma once

enum CODEDEBUT {

	NORMAL,
	PRINCIPAL,
	WAIT_TIME,
	WAIT_FRAME,
	SYNCHRO,

	LAST_DEBUT
};

enum CODEFIN {

	BOUCLE,
	CHANGE_ANIM,
	STOP,
	WAIT_RANDOM,
	WAIT_RANDOM_GLOBAL,
	KILL,

	LAST_FIN
};


typedef struct TAnim {

	DWORD	IDSprite;
	WORD	Time;

	int		dx;
	int		dy;
	WORD	Flag;
	WORD	PFlag;

} TAnim, *LPAnim;

typedef struct TAnimObj {

#ifdef ANIMS_ATOR
	char		*Nom;
	long		CurTime;
	int			CurFrame;
	int			CurState;
#endif

	int			FrameCount;
	LPAnim		*Frames;
	
	DWORD		CodeFin;
	DWORD		ParamFin;

	DWORD		CodeDebut;
	DWORD		ParamDebut;
	

} TAnimObj, *LPAnimObj;


typedef struct TAnimList {

#ifdef ANIMS_ATOR
	char	   *Name;
#endif
	int			NbObjs;
	LPAnimObj  *Objs;
	
	WORD params[3];

	struct TAnimList *Suivant;

} TAnimList, *LPAnimList;


class CAnims {

	DWORD	   LastTime;
	int		   count;

public:
	LPAnimList Liste;

	void		Init		( void	);
	void	    Clean		( void	);

#ifdef ANIMS_ATOR
	char		*Nom;
	void		Detruit		( LPAnimList l, bool FindRefs	);
	LPAnimList	CreateEmpty	( char *Name	);
	LPAnimList	FindByName	( char *Name	);
	bool		DeleteAnim	( char *Name	);
	void		DelObj		( LPAnimList a, LPAnimObj o );
	bool		AddObj		( LPAnimList a, char *ObjName );
	void		AddFrame	( LPAnimObj o,  DWORD ID	  );
	bool		Save		( void );
#endif

	void InitDone	( void );
	bool Load		( char *AnimName );
	bool LoadAll	();

	LPAnimList Play	(  LPAnimList a, bool Slow, int Zoom  );

	CAnims	();
	~CAnims	();
};

extern CAnims Anims;
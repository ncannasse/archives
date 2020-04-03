#pragma once

#define NOM_EXE	"Data Edit"

#define NOM_COMPLET "Data Editor v1.2"
#define AUTHOR		"Nicolas CANNASSE / Warplayer"
#define HELP		"Data Editor v1.2\n(c)2000 by Warplayer\n\nStandard plug-in for edition of structured datas"
			
typedef void (*RUNFONCTION)(void);

extern RUNFONCTION RunFonction;

extern void RunMain ( HWND hWnd );
extern void RunAnimator ( void	);

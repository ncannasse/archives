#pragma once

#define NOM_EXE	"Animator"

#define NOM_COMPLET "Dragoon's Animator v2.4"
#define AUTHOR		"Nicolas CANNASSE / Warplayer"
#define HELP		"Dragoon's Animator v2.4\n(c)2000 by Warplayer\n\nThis plug-in is an 2D-Sprite-Oriented-animation tool\nSymbol Groups are:\n0-2 : three params for animation\n3 : Frame Flag\n4 : Flag Param"
			
typedef void (*RUNFONCTION)(void);

extern RUNFONCTION RunFonction;

extern void RunMain ();
extern void RunAnimator ( void	);

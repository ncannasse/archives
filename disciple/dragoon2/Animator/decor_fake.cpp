/* ******************************************************************** */
#include <windows.h>
#include "graphisme.h"
#include "decor_fake.h"
#include "erreurs.h"
/* ******************************************************************** */

CDecor *Le_Decor;

/* ******************************************************************** */


CDecor::CDecor()
{
	DecorVisible = Rect(0,0,RES_X,RES_Y);
	Ox = Oy = 0;
	Mx = My = 0;
}

/* ******************************************************************** */

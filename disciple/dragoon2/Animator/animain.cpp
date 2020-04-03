/* ******************************************************************** */
/* animator.cpp															*/
/* ******************************************************************** */
#include <windows.h>
#include "global.h"
#include "graphisme.h"
#include "erreurs.h"
#include "decor_fake.h"
#include "global.h"
#include "entree.h"
#include "dlib.h"
#include "fonts.h"
#include "sprites.h"

/* ******************************************************************** */

HWND		hWnd;
extern HANDLE hInst;
RUNFONCTION RunFonction = NULL;

/* ******************************************************************** */

LRESULT CALLBACK 
Gere_Messages (HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam) 
{
	switch (iMsg)
	{
	case WM_CREATE:
		ShowCursor(FALSE);
        return TRUE; 
	case WM_KEYDOWN:
		if( wParam != VK_ESCAPE )
			break;
	case WM_CLOSE :
		DestroyWindow( hWnd );
		break;
    case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_ACTIVATEAPP:
		break;
	default:
		break;
	}
    return DefWindowProc (hwnd, iMsg, wParam, lParam); 
}

/* ******************************************************************** */

void
Initialise_Wnd (HINSTANCE hInstance, 
				int nCmdShow)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Gere_Messages;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NOM_EXE;
    wc.lpszClassName = NOM_EXE;
    RegisterClass( &wc );
	hWnd = CreateWindowEx( 0, NOM_EXE, NOM_EXE,WS_VISIBLE,0, 0, RES_X, RES_Y, NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
}
/* ******************************************************************** */

void
RunMain ( ) 
{
	Initialise_Wnd( (HINSTANCE)hInst, SW_SHOWDEFAULT );
	Ecran.Initialise_DD ( hWnd );
	Le_Decor = new CDecor;
	Fonts = new CFonts;
	Ctrl  = new CInterface;
	Sprites = new CSprites;

	RunAnimator();

	delete Sprites;
	delete Ctrl;
	delete Fonts;
	delete Le_Decor;
	Ecran.Ferme_DD ();
	ShowCursor(TRUE);
}


/* ******************************************************************** */
/* fin du fichier animator.cpp											*/

//*****************************************************************************
//*                               MTW Editor
//*                           (c)2003 Motion-Twin
//*
//*     Author   :  M.B.
//*
//*     Desc     :  main function for BinView
//*
//*     Date     :  Observations                            :   Author
//*
//*     23.05.03    Menu items
//*     20.05.03    InfoFunc
//*     16.05.03    EnumChildFunc
//*     15.05.03    Creation                                    M.B.
//*
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Includes - Standard
//

#include "time.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Includes - Project
//

//using namespace M3D;

#include "m3dview.h"
#include "BinView.h"
#include "resource.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
//
#define MAIN_CLASS_NAME "BinView"
#define VIEW_CLASS_NAME "D3DWindow"
#define ANIM_CLASS_NAME "AnimWindow"
#define BUTTON_CLASS_NAME "ButtonWindow"
#define ID_D3DWINDOW 1
#define ID_ANIMWINDOW 2
#define ID_BUTTONWINDOW 3
#define ID_ANIMBUTTON 4
#define ID_SPEEDBUTTON 5
#define ID_COLORBUTTON 6
#define ID_FPSBUTTON 7
#define ID_REMOVEBUTTON 8

HINSTANCE g_hInstance = NULL;
OPENFILENAME ofn;                       // common dialog box structure
char g_fileName[260];

static BinView* g_mtwEdit = NULL;
MTW* g_selectedTag = NULL;
M3dView *g_m3dView = NULL;

HWND buttonsWindow;
HWND hwndTrack;

//*****************************************************************************
// MENUS
//*****************************************************************************

//-----------------------------------------------------------------------------
// SetMenuItems
//
void
SetMenuItems(HWND window, DWORD state)
{
    HMENU menu = GetMenu(window);
    EnableMenuItem(menu, IDM_FILE_SAVE, state);
    EnableMenuItem(menu, IDM_FILE_SAVE_AS, state);
    EnableMenuItem(menu, IDM_FILE_CLOSE, state);

    // undo menu gets enable in more specific ways
    if(state==MF_GRAYED)
        EnableMenuItem(menu, IDM_EDIT_UNDO, state);

    EnableMenuItem(menu, IDM_EDIT_TAGINFORMATION, state);
    EnableMenuItem(menu, IDM_EDIT_DELETE, state);

    EnableMenuItem(menu, IDM_VIEW_SIZE_NONE, state);
    EnableMenuItem(menu, IDM_VIEW_SIZE_PRCT, state);
    EnableMenuItem(menu, IDM_VIEW_SIZE_BYTE, state);
    EnableMenuItem(menu, IDM_VIEW_SIZE_UNIT, state);
}


//-----------------------------------------------------------------------------
// SetMenuUndo
//
void
SetUndoMenu(HWND window, DWORD state)
{
    HMENU menu = GetMenu(window);
    EnableMenuItem(menu, IDM_EDIT_UNDO, state);
}


//-----------------------------------------------------------------------------
// SetMenuCheck
//
void
CheckViewMenu(HWND window, UINT menuItem)
{
    HMENU menu = GetMenu(window);
    CheckMenuItem(menu, IDM_VIEW_SIZE_NONE, MF_UNCHECKED);
    CheckMenuItem(menu, IDM_VIEW_SIZE_PRCT, MF_UNCHECKED);
    CheckMenuItem(menu, IDM_VIEW_SIZE_BYTE, MF_UNCHECKED);
    CheckMenuItem(menu, IDM_VIEW_SIZE_UNIT, MF_UNCHECKED);

    CheckMenuItem(menu, menuItem, MF_CHECKED);
}


//*****************************************************************************
// DIALOGS
//*****************************************************************************

//-----------------------------------------------------------------------------
// OpenBox
//
BOOL
OpenBox(HWND window)
{
	char buf[256];
	GetCurrentDirectory(256,buf);
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = window;

    ofn.lpstrFile = g_fileName;           
    ofn.nMaxFile = sizeof(g_fileName);

    ofn.lpstrFilter = "All\0*.*\0MTW Format (*.hs, *.mtw, *.atw)\0*.MTW;*.HS;*.ATW\0\0\0";
    ofn.nFilterIndex = 2;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = buf;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box
    if(GetOpenFileName(&ofn))
		if(g_mtwEdit->Open(ofn.lpstrFile)){
			g_m3dView->open(ofn.lpstrFile);
			return true;
		}

    return FALSE;
}


//-----------------------------------------------------------------------------
// SaveBox
//
BOOL
SaveBox()
{
    if(GetSaveFileName(&ofn))
    {
        return g_mtwEdit->SaveAs(ofn.lpstrFile);
    }
    return FALSE;
}


//*****************************************************************************
// CALLBACKS
//*****************************************************************************

//-----------------------------------------------------------------------------
// EnumChildFunc
//
BOOL CALLBACK
EnumChildFunc(HWND childWindow, LPARAM lParam)
{
    LPRECT rcParent = (LPRECT) lParam;
    int left = 0, width = rcParent->right/5;
	int up = 0, height = 17*rcParent->bottom/20;

    int idChild = GetWindowLong(childWindow, GWL_ID);
    if( idChild==ID_D3DWINDOW )
    {
        left = width;
        width = 4*rcParent->right/5;
    }
	else if( idChild==ID_ANIMWINDOW )
	{
		width = rcParent->right / 2;
		up = height;
		height = 3*rcParent->bottom/20;
	}
	else if( idChild==ID_BUTTONWINDOW )
	{
		left = width = rcParent->right / 2;
		up = height;
		height = 3*rcParent->bottom/20;
	}

    // Size and position the child window (treeview is 1/4 of the total size)
	if( GetParent(GetParent(childWindow)) == NULL )
		MoveWindow(childWindow, left, up, width, height, TRUE);

    return TRUE;
}


//-----------------------------------------------------------------------------
// InfoFunc
//
BOOL CALLBACK
InfoFunc(HWND dialogWindow, UINT message, WPARAM wParam, LPARAM lParam)
{
    int res, size;
    char* text;

    switch(message)
    {
        case WM_INITDIALOG:

            // initialise les labels et champs du dialog
			if( !g_selectedTag )
				return false;
            size = sizeof(g_selectedTag->ctag);
            text = new char[size+1];
            memcpy(text, (char*)g_selectedTag->ctag, size);
            text[size] = 0;
            SetDlgItemText(dialogWindow, IDD_INFO_TAG, text);
            delete text;

            SetDlgItemInt(dialogWindow, IDD_INFO_SIZE, g_selectedTag->size, FALSE);

            if( ISTEXT(g_selectedTag) )
            {
                return SetDlgItemText(dialogWindow, IDD_INFO_DATA, (char*)g_selectedTag->data);
            }
            else if(g_selectedTag->IsNumber())
            {
                return SetDlgItemInt(dialogWindow, IDD_INFO_DATA, g_selectedTag->number, FALSE);
            }
            return FALSE;

        case WM_COMMAND:

            switch(LOWORD(wParam))
            {
                case IDOK:
					if(!g_selectedTag){
						EndDialog(dialogWindow, wParam);
                        return FALSE;
					}

                    if( ISTEXT(g_selectedTag) )
                    {
                        g_mtwEdit->SetModified();
                        text = new char[256];
                        GetDlgItemText(dialogWindow, IDD_INFO_DATA, text, 256);
                        g_selectedTag->size = strlen(text)+1;
                        g_selectedTag->data = (void*)strdup(text);
                        delete text;
                        SetUndoMenu(GetParent(dialogWindow), MF_ENABLED);
                    }
                    else if(g_selectedTag->IsNumber())
                    {
                        g_mtwEdit->SetModified();
                        g_selectedTag->number = GetDlgItemInt(dialogWindow, IDD_INFO_DATA, &res, FALSE);
                        SetUndoMenu(GetParent(dialogWindow), MF_ENABLED);
                    }

                case IDCANCEL:
                    EndDialog(dialogWindow, wParam);
                    return TRUE;
            }
    }
    return FALSE;
}


//-----------------------------------------------------------------------------
// MainFunc
// Processes messages for the main window
//
LRESULT CALLBACK 
MainFunc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD box;
    RECT rcClient;

	switch(message)
	{
        case WM_CREATE:
            SetMenuItems(window, MF_GRAYED);
            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                //-------------------------------------------------------------
                // FILE MENU                       
                case IDM_FILE_OPEN:
                    if(OpenBox(window))
                        SetMenuItems(window, MF_ENABLED);
                    break;
                case IDM_FILE_SAVE:
                    g_mtwEdit->Save();
                    break;
                case IDM_FILE_SAVE_AS:
                    SaveBox();
                    break;
                case IDM_FILE_CLOSE:
					g_m3dView->close();
                    if(g_mtwEdit->Close())
                        SetMenuItems(window, MF_GRAYED);
					RedrawWindow(window, NULL, NULL, RDW_INVALIDATE);
                    break;
                case IDM_FILE_EXIT:
                    PostQuitMessage(0);
                    break;

                //-------------------------------------------------------------
                // EDIT MENU
                case IDM_EDIT_UNDO:
                    if(g_mtwEdit->Undo())
                        SetUndoMenu(window, MF_GRAYED);
                    break;
                case IDM_EDIT_TAGINFORMATION:
                    g_selectedTag = g_mtwEdit->GetSelectedTag();
                    if(!g_selectedTag) return FALSE;

                    if( ISTEXT(g_selectedTag) )
                        box = IDD_INFO;
                    else if(g_selectedTag->IsNumber())
                        box = IDD_INFO1;
                    else
                        box = IDD_INFO2;

                    if(DialogBox(g_hInstance, MAKEINTRESOURCE(box), window, 
                        (DLGPROC)InfoFunc)==IDOK)
                    {
                        g_mtwEdit->Refresh();
                    }
                    break;
                case IDM_EDIT_DELETE:
                    if(g_mtwEdit->DeleteItem())
                        SetUndoMenu(window, MF_ENABLED);
                    break;

                //-------------------------------------------------------------
                // VIEW MENU
                case IDM_VIEW_SIZE_NONE:
                    g_mtwEdit->Refresh(BinView::VIEW_SIZE_NONE);
                    CheckViewMenu(window, IDM_VIEW_SIZE_NONE);
                    break;
                case IDM_VIEW_SIZE_PRCT:
                    g_mtwEdit->Refresh(BinView::VIEW_SIZE_PRCT);
                    CheckViewMenu(window, IDM_VIEW_SIZE_PRCT);
                    break;
                case IDM_VIEW_SIZE_BYTE:
                    g_mtwEdit->Refresh(BinView::VIEW_SIZE_BYTE);
                    CheckViewMenu(window, IDM_VIEW_SIZE_BYTE);
                    break;
                case IDM_VIEW_SIZE_UNIT:
                    g_mtwEdit->Refresh(BinView::VIEW_SIZE_UNIT);
                    CheckViewMenu(window, IDM_VIEW_SIZE_UNIT);
                    break;

				//-------------------------------------------------------------
                // KEYS
				case ID_KEYS:
					DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 
								window, (DLGPROC)InfoFunc);
					break;
            }
            return 0;

		case WM_SIZE:
			GetClientRect(window, &rcClient);
			EnumChildWindows(window, EnumChildFunc, (LPARAM) &rcClient);
            return 0;

        case WM_CLOSE:
        case WM_DESTROY:
			g_m3dView->close();
			g_mtwEdit->Close();
	        PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(window, message, wParam, lParam);
    }
}


//-----------------------------------------------------------------------------
// OnMessage
// 
static LRESULT CALLBACK
ViewFunc(HWND window,UINT message,WPARAM wParam,LPARAM lParam)
{
	int width, height;
	switch(message)
    {
		case WM_MOUSEACTIVATE:
			SetFocus(g_m3dView->d3dView);	
			break;
	    case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_KILLFOCUS:
			g_m3dView->emit(message, wParam, lParam);
			break;
		case WM_SIZE:
			width = lParam & 0xffff;
			height = (lParam >> 16) & 0xffff;
			if( width > (4 * height) / 3 ){
				width = (4 * height) / 3;
			}
			else{
				height = (3 * width) / 4;
			}
			SetWindowPos(window, NULL, 0, 0, width, height, SWP_NOMOVE);
			break;
	}
	return DefWindowProc(window, message, wParam, lParam); 
}

static LRESULT CALLBACK
AnimFunc(HWND window,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message){
	case WM_PAINT:
		g_m3dView->drawIcons();
		break;
	case WM_LBUTTONDOWN:
		g_m3dView->animEmit(message, wParam, lParam);
		break;
	}
	return DefWindowProc(window, message, wParam, lParam); 
}

static LRESULT CALLBACK
ButtonFunc(HWND window,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message){
		case WM_PAINT:
			break;
		case WM_COMMAND:
			switch(HIWORD(wParam)){
				case BN_CLICKED:
					if( LOWORD(wParam) == ID_ANIMBUTTON )
						g_m3dView->animEmit(WM_KEYDOWN, VK_F1, 0);
					else if( LOWORD(wParam) == ID_REMOVEBUTTON )
						g_m3dView->removeAnim();
					break;
				case EN_CHANGE:
					if( LOWORD(wParam) == ID_COLORBUTTON ){
						char temp[7];
						temp[0] = 7;
						SendMessage((HWND)lParam, EM_GETLINE, 0, (LPARAM)temp);
						if( strlen(temp) == 6 ){
							int i;
							sscanf(temp, "%x", &i);
							g_m3dView->changeColor(i);
						}
					}
					else if( LOWORD(wParam) == ID_FPSBUTTON ){
						char temp[3];
						temp[0] = 3;
						SendMessage((HWND)lParam, EM_GETLINE, 0, (LPARAM)temp);
						if( strlen(temp) == 2 ){
							int i = atoi(temp);
							if( i < 10 )
								i = 10;
							g_m3dView->changeFps(i);
						}
					}
					break;
			}
		case WM_HSCROLL:
			if( (HWND)lParam == hwndTrack )
				g_m3dView->modSpeed((SendMessage((HWND)lParam, TBM_GETPOS, 0, 0)  - 50) / 10.0);
			break;
	}
	return DefWindowProc(window, message, wParam, lParam); 
}

//*****************************************************************************
// MAIN
//*****************************************************************************

//-----------------------------------------------------------------------------
// WinMain
//
int WINAPI 
WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR l, int)
{
    int  res = 0;

    bool fullscreen = false;
    bool aa_enabled = false;

    g_hInstance = hInstance;

    // creation de l'editeur
	g_m3dView = new M3dView();
    g_mtwEdit = new BinView(g_m3dView);

    // creation des fenetres de l'interface

    WNDCLASSEX wcMain = { sizeof(WNDCLASSEX), CS_CLASSDC, MainFunc,
							0L, 0L, GetModuleHandle(NULL), NULL, NULL, 
							(HBRUSH)(COLOR_WINDOW+1), (LPCSTR)IDM_MENU, 
							MAIN_CLASS_NAME, NULL };
    RegisterClassEx(&wcMain);

    HWND mainWindow = CreateWindowEx(0, MAIN_CLASS_NAME, MAIN_CLASS_NAME,
                                WS_OVERLAPPEDWINDOW, 100, 100, 800, 600,
                                GetDesktopWindow(), (HMENU)NULL, 
								wcMain.hInstance, NULL);
    if(!mainWindow)
        return -1;

    WNDCLASSEX wcD3D = { sizeof(WNDCLASSEX), CS_CLASSDC, ViewFunc, 0L, 0L, 
							GetModuleHandle(NULL), NULL, NULL, (HBRUSH)NULL, 
							(LPCSTR)NULL, VIEW_CLASS_NAME, NULL };
    RegisterClassEx(&wcD3D);

    HWND d3dView = CreateWindowEx(0, VIEW_CLASS_NAME, (LPSTR)NULL, 
                                    WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, 0, 0, 0,
                                    mainWindow, (HMENU)(int)(ID_D3DWINDOW), 
									wcD3D.hInstance, NULL);
    if(!d3dView)
        return -2;

    HWND treeView = CreateWindowEx(0, WC_TREEVIEW, NULL,
									WS_CHILD|WS_VISIBLE|WS_BORDER|TVS_HASLINES
									|TVS_HASBUTTONS, 
									0, 0, 0, 0, mainWindow, NULL, hInstance,
									NULL);
    if(!treeView)
        return -3;

	WNDCLASSEX wcAnim = { sizeof(WNDCLASSEX), CS_CLASSDC, AnimFunc, 0L, 0L, 
							GetModuleHandle(NULL), NULL, NULL, GetSysColorBrush(0), 
							(LPCSTR)NULL, ANIM_CLASS_NAME, NULL };
    RegisterClassEx(&wcAnim);

    HWND animView = CreateWindowEx(0, ANIM_CLASS_NAME, (LPSTR)NULL, 
                                    WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER, 
									0, 0, 0, 0,
                                    mainWindow, (HMENU)(int)(ID_ANIMWINDOW), 
									wcAnim.hInstance, NULL);
    if(!animView)
        return -4;


	//**********************************************************************

	WNDCLASSEX wcButton = { sizeof(WNDCLASSEX), CS_CLASSDC, ButtonFunc, 0L, 0L, 
							GetModuleHandle(NULL), NULL, NULL, GetSysColorBrush(0), 
							(LPCSTR)NULL, BUTTON_CLASS_NAME, NULL };
    RegisterClassEx(&wcButton);
	buttonsWindow = CreateWindowEx(0, BUTTON_CLASS_NAME, (LPSTR)NULL, 
                                    WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER, 
									0, 0, 0, 0,
                                    mainWindow, (HMENU)(int)(ID_BUTTONWINDOW), 
									wcButton.hInstance, NULL);

	HWND animButton = CreateWindow("Button", "play/stop", 
                                    WS_CHILD|WS_VISIBLE, 
									10, 5, 80, 40,
                                    buttonsWindow, (HMENU)(int)(ID_ANIMBUTTON), 
									hInstance, NULL);

	HWND removeButton = CreateWindow("Button", "remove anim", 
                                    WS_CHILD|WS_VISIBLE, 
									10, 50, 100, 30,
                                    buttonsWindow, (HMENU)(int)(ID_REMOVEBUTTON), 
									hInstance, NULL);

	CreateWindow("Static", "animation speed", 
                                    WS_CHILD|WS_VISIBLE, 
									140, 0, 160, 20,
                                    buttonsWindow, 0, 
									hInstance, NULL);

	hwndTrack = CreateWindow(TRACKBAR_CLASS, "animation speed", 
									WS_CHILD|WS_VISIBLE|TBS_ENABLESELRANGE, 
									110, 20, 200, 40,
									buttonsWindow, (HMENU)(int)(ID_SPEEDBUTTON), 
									hInstance, NULL);

	SendMessage(hwndTrack, TBM_SETRANGE, 
        (WPARAM) TRUE,                   
        (LPARAM) MAKELONG(0, 100));  
	SendMessage(hwndTrack, TBM_SETPOS, 
        (WPARAM) TRUE,                   
        (LPARAM) 60);  
	SendMessage(hwndTrack, TBM_SETPAGESIZE, 
        0, (LPARAM) 4);                  // new page size 


	CreateWindow("Static", "bg color", 
                                    WS_CHILD|WS_VISIBLE, 
									310, 0, 55, 20,
                                    buttonsWindow, 0, 
									hInstance, NULL);

	HWND hwndEdit = CreateWindow("EDIT", NULL,       
									WS_CHILD|WS_VISIBLE|ES_CENTER|ES_UPPERCASE|WS_BORDER,
									310, 20, 68, 18,  
									buttonsWindow,        
									(HMENU) ID_COLORBUTTON,   
									hInstance, NULL);       

	SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM) "ffffff"); 
	SendMessage(hwndEdit, EM_SETLIMITTEXT, (WPARAM)6, 0);

	CreateWindow("Static", "fps", 
                                    WS_CHILD|WS_VISIBLE, 
									310, 40, 20, 20,
                                    buttonsWindow, 0, 
									hInstance, NULL);

	HWND hwndEdit2 = CreateWindow("EDIT", NULL,       
									WS_CHILD|WS_VISIBLE|ES_CENTER|ES_NUMBER|WS_BORDER,
									310, 60, 20, 18,  
									buttonsWindow,        
									(HMENU) ID_FPSBUTTON,   
									hInstance, NULL);       

	SendMessage(hwndEdit2, WM_SETTEXT, 0, (LPARAM) "40"); 
	SendMessage(hwndEdit2, EM_SETLIMITTEXT, (WPARAM)2, 0);


	//**********************************************************************

    ShowWindow(mainWindow, SW_NORMAL);
    UpdateWindow(mainWindow);

	// initialisation

	g_m3dView->setAnimView(animView);
	g_m3dView->setView(d3dView);
    g_mtwEdit->SetTreeView(treeView);

    InitCommonControls();

	clock_t initial_time = clock();

	if(l && *l != '\0')
		if(g_mtwEdit->Open(l)){
			g_m3dView->open(l);
		}

    MSG msg;
	while(true)
	{
		if( clock() - initial_time > 5000 ){
			g_mtwEdit->reloadTime();
			g_m3dView->reload();
			initial_time = clock();
		}
		if(!GetMessage( &msg, NULL, 0, 0 ))
			goto end; // double break
		if( msg.message == WM_LBUTTONDBLCLK ) //intercept double click before tree view
			g_mtwEdit->GetSelectedTag();

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

end:
    SAFE_DELETE(g_mtwEdit);
	SAFE_DELETE(g_m3dView);
	UnregisterClass(BUTTON_CLASS_NAME, wcButton.hInstance);
    UnregisterClass(VIEW_CLASS_NAME, wcD3D.hInstance);
	UnregisterClass(ANIM_CLASS_NAME, wcAnim.hInstance);
    UnregisterClass(MAIN_CLASS_NAME, wcMain.hInstance);
    return res;
}


#include <windows.h>
#include <m3d.h>

static LRESULT CALLBACK
OnMessage(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	switch( iMsg )
	{
	case WM_CREATE:
		return TRUE;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}	
	return DefWindowProc (hwnd, iMsg, wParam, lParam); 
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	//WMem::Init(false);
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, OnMessage, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "M3D", NULL };
    RegisterClassEx( &wc ); 
    HWND hwnd = CreateWindow( "M3D", "M3D", WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, GetDesktopWindow(), NULL, wc.hInstance, NULL );	
	M3D::SetConfig( hwnd, false );
	if( !M3D::Init() )
		PostQuitMessage(0);
	else {
		ShowWindow(hwnd,SW_NORMAL);
		UpdateWindow(hwnd);
	}

	M3D::File fmtw("demon.mtw");
	M3D::Inst *inst = NULL;
	if( !fmtw.Import() )
		PostQuitMessage(0);
	else {
		M3D::Mesh *mesh = (M3D::Mesh*)fmtw.Get(M3D::Any::MMESH);
		mesh->Reset(false);
		inst = mesh->CreateInstance();
	}

	M3D::Camera::Zoom(0.7f);

	MSG msg;
	while( true )
	{		
		while( PeekMessage(&msg,NULL,0,0,PM_NOREMOVE) ) {
			if( !GetMessage( &msg, NULL, 0, 0 ) )
				goto end;
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
		M3D::Render();
		MATRIX r;
		mrot(&r,0,0,0.0008f);
		mmult(M3D::transform.Get(),M3D::transform.Get(),&r);
		M3D::transform.Invalidate();
	}

end:
	delete inst;
	M3D::Close();
	return 0;
}
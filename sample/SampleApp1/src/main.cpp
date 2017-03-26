

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define STRICT				
#define WIN32_LEAN_AND_MEAN	
#define WINVER        0x501	
#define _WIN32_WINNT  0x501 

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }
#define D3D_DEBUG_INFO		

#include <windows.h>
#include <Windowsx.h>

#include <crtdbg.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "resource.h"

// Live2D
#include "Live2D.h"
#include "util/UtSystem.h"
#include "Live2DModelD3D.h"

// Live2D Sample
#include "LAppRenderer.h"
#include "LAppLive2DManager.h"
#include "L2DViewMatrix.h"

using namespace live2d;
using namespace live2d::framework;


#pragma comment( lib, "d3d9.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx9d.lib" )
#else
#pragma comment( lib, "d3dx9.lib" )
#endif
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "Imm32.lib" )


#if _MSC_VER >= 1900 
#pragma comment( lib, "legacy_stdio_definitions.lib")
#endif


#define		USE_LIVE2D						1		
#define		ENABLE_CLIPPING_FEATURE			1		
#define		CHANGE_FULLSCREEN_RESOLUTION	0		
bool		g_bWindow		=  true ;				

HINSTANCE	g_hInstance		= NULL;					
HWND		g_hWindow		= NULL;					
HMENU		g_hMenu			= NULL;					

WCHAR		g_szAppTitle[]	= L"Live2D Sample / SampleApp1";
WCHAR		g_szWndClass[]	= L"L2DSAMPLE03";

RECT		g_rectWindow;							


SIZE		g_sizeWindowMode	= {  1280 , 720 };	

SIZE		g_sizeFullMode	= {  1280 , 720 };		

D3DFORMAT	g_formatFull	= D3DFMT_X8R8G8B8;		


bool		g_bActive		= false;	




LPDIRECT3D9				g_pD3D			= NULL; 
LPDIRECT3DDEVICE9		g_pD3DDevice	= NULL; 
D3DPRESENT_PARAMETERS	g_D3DPP;				
LPDIRECT3DTEXTURE9		g_pMaskTexture       = NULL; 
LPDIRECT3DSURFACE9		g_pMaskBufferSurface = NULL; 

D3DPRESENT_PARAMETERS	g_D3DPPWindow;			
D3DPRESENT_PARAMETERS	g_D3DPPFull;			

bool g_bDeviceLost = false;						




static LAppRenderer*				s_renderer;
static LAppLive2DManager*			s_live2DMgr;



LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);


void ChangeFullscreenResolution() ;



HRESULT InitApp(HINSTANCE hInst)
{
	
	g_hInstance = hInst;

	
	ImmDisableIME(-1);	

	
	WNDCLASS wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)MainWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= LoadIcon(hInst, (LPCTSTR)IDI_ICON1);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName		= g_bWindow ? MAKEINTRESOURCE(IDR_MENU1) : NULL;
	wc.lpszClassName	= g_szWndClass;

	if (!RegisterClass(&wc))
		return DXTRACE_ERR(L"InitApp", GetLastError());

	
	g_rectWindow.top	= 0;
	g_rectWindow.left	= 0;
	g_rectWindow.right	= g_sizeWindowMode.cx;
	g_rectWindow.bottom	= g_sizeWindowMode.cy;
	AdjustWindowRect(&g_rectWindow, WS_OVERLAPPEDWINDOW, TRUE);
	g_rectWindow.right	= g_rectWindow.right - g_rectWindow.left;
	g_rectWindow.bottom	= g_rectWindow.bottom - g_rectWindow.top;
	g_rectWindow.top	= 0;
	g_rectWindow.left	= 0;

	RECT rect;
	if (g_bWindow)
	{
		
		rect.top	= CW_USEDEFAULT;
		rect.left	= CW_USEDEFAULT;
		rect.right	= g_rectWindow.right;
		rect.bottom	= g_rectWindow.bottom;
	}
	else
	{
		
		rect.top	= 0;
		rect.left	= 0;
		rect.right	= g_sizeFullMode.cx;
		rect.bottom	= g_sizeFullMode.cy;

		g_hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU1));

		
		ChangeFullscreenResolution() ;
	}

	g_hWindow = CreateWindow(g_szWndClass, g_szAppTitle,
		g_bWindow ? WS_OVERLAPPEDWINDOW : WS_POPUP,
			rect.left, rect.top, rect.right, rect.bottom,
			NULL, NULL, hInst, NULL);
	if (g_hWindow == NULL)
		return DXTRACE_ERR(L"InitApp", GetLastError());

	
	ShowWindow(g_hWindow, SW_SHOWNORMAL);
	UpdateWindow(g_hWindow);

	return S_OK;
}

/************************************************************
	Setup Live2D
************************************************************/
void SetupLive2D()
{
#if USE_LIVE2D

	
	s_live2DMgr=new LAppLive2DManager();

	
	s_renderer = new LAppRenderer();
	s_renderer->setLive2DManager(s_live2DMgr);

	
	s_renderer->setDeviceSize( g_D3DPP.BackBufferWidth , g_D3DPP.BackBufferHeight ) ;

	s_live2DMgr->changeModel();
#endif
}

/************************************************************
	Cleanup Live2D
************************************************************/
void CleanupLive2D(void){
#if USE_LIVE2D
	delete s_renderer;
	delete s_live2DMgr;
#endif
}

/************************************************************
	Render Live2D
************************************************************/
VOID RenderLive2D(){
#if USE_LIVE2D
	if( ! s_live2DMgr ) return ;
		
	
	
	

	
	s_renderer->draw() ;

#endif
}

/************************************************************
	Device Lost Live2D
************************************************************/
VOID OnLostDeviceLive2D( ){
#if USE_LIVE2D
	if( s_live2DMgr ){
		s_live2DMgr->deviceLost() ;
	}
#endif
}

/************************************************************
	Device Reset Live2D
************************************************************/
VOID OnResetDeviceLive2D()
{
#if USE_LIVE2D
	if( s_live2DMgr ){
		s_live2DMgr->deviceReset() ;
	}	
#endif
}

/************************************************************
	Setup Clipping Mask
************************************************************/
HRESULT SetupClippingMask()
{
#if ENABLE_CLIPPING_FEATURE
	
	HRESULT hr = D3DXCreateTexture(g_pD3DDevice, g_D3DPP.BackBufferWidth, g_D3DPP.BackBufferHeight, 
		D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_A8, D3DPOOL_DEFAULT, &g_pMaskTexture);
	if (FAILED(hr))
	{
		return DXTRACE_ERR(L"SetupClippingMask CreateMaskTexture", hr);
	}

	hr = g_pMaskTexture->GetSurfaceLevel(0, &g_pMaskBufferSurface);
	if(FAILED(hr))
	{
		return DXTRACE_ERR(L"SetupClippingMask CreateMaskTextureSurface", hr);
	}
#endif
	return S_OK;
}

/************************************************************
	Cleanup Clipping Mask
************************************************************/
VOID CleanupClippingMask()
{
#if ENABLE_CLIPPING_FEATURE
	SAFE_RELEASE(g_pMaskBufferSurface);
	SAFE_RELEASE(g_pMaskTexture);	
#endif
}


HRESULT InitDXGraphics(void)
{
	
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
		return DXTRACE_ERR(L"InitDXGraphics Direct3DCreate9", E_FAIL);

	
	ZeroMemory(&g_D3DPPWindow, sizeof(g_D3DPPWindow));

	g_D3DPPWindow.BackBufferWidth			= 0;
	g_D3DPPWindow.BackBufferHeight			= 0;
	g_D3DPPWindow.BackBufferFormat			= D3DFMT_UNKNOWN;
	g_D3DPPWindow.BackBufferCount			= 1;
	g_D3DPPWindow.MultiSampleType			= D3DMULTISAMPLE_NONE;
	g_D3DPPWindow.MultiSampleQuality		= 0;
	g_D3DPPWindow.SwapEffect				= D3DSWAPEFFECT_DISCARD;
	g_D3DPPWindow.hDeviceWindow				= g_hWindow;
	g_D3DPPWindow.Windowed					= TRUE;
	g_D3DPPWindow.EnableAutoDepthStencil	= FALSE;
	g_D3DPPWindow.AutoDepthStencilFormat	= D3DFMT_UNKNOWN;
	g_D3DPPWindow.Flags						= 0;
	g_D3DPPWindow.FullScreen_RefreshRateInHz= 0;
	g_D3DPPWindow.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
//	g_D3DPPWindow.PresentationInterval		= D3DPRESENT_INTERVAL_ONE;

	
	ZeroMemory(&g_D3DPPFull, sizeof(g_D3DPPFull));

	g_D3DPPFull.BackBufferWidth				= g_sizeFullMode.cx;
	g_D3DPPFull.BackBufferHeight			= g_sizeFullMode.cy;
	g_D3DPPFull.BackBufferFormat			= g_formatFull;
	g_D3DPPFull.BackBufferCount				= 1;
	g_D3DPPFull.MultiSampleType				= D3DMULTISAMPLE_NONE;
	g_D3DPPFull.MultiSampleQuality			= 0;
	g_D3DPPFull.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	g_D3DPPFull.hDeviceWindow				= g_hWindow;
	g_D3DPPFull.Windowed					= FALSE;
	g_D3DPPFull.EnableAutoDepthStencil		= FALSE;
	g_D3DPPFull.AutoDepthStencilFormat		= D3DFMT_UNKNOWN;
	g_D3DPPFull.Flags						= 0;
	g_D3DPPFull.FullScreen_RefreshRateInHz	= 0;
	g_D3DPPFull.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
//	g_D3DPPFull.PresentationInterval		= D3DPRESENT_INTERVAL_ONE;

	
	if (g_bWindow)
		g_D3DPP = g_D3DPPWindow;
	else
		g_D3DPP = g_D3DPPFull;

	HRESULT hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWindow,
						D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_D3DPP, &g_pD3DDevice);
	if (FAILED(hr))
	{
		hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWindow,
						D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_D3DPP, &g_pD3DDevice);
		if (FAILED(hr))
		{
			hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, g_hWindow,
							D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_D3DPP, &g_pD3DDevice);
			if (FAILED(hr))
				return DXTRACE_ERR(L"InitDXGraphics CreateDevice", hr);
		}
	}

	SetupClippingMask();

	
	D3DVIEWPORT9 vp;
	vp.X		= 0;
	vp.Y		= 0;
	vp.Width	= g_D3DPP.BackBufferWidth;
	vp.Height	= g_D3DPP.BackBufferHeight;
	vp.MinZ		= 0.0f;
	vp.MaxZ		= 1.0f;
	hr = g_pD3DDevice->SetViewport(&vp);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDXGraphics SetViewport", hr);

	return S_OK;
}


/************************************************************
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
************************************************************/
VOID SetupMatrices()
{
   // Set up world matrix
    D3DXMATRIXA16 matWorld;
    D3DXMatrixIdentity( &matWorld );

	D3DXMATRIX Ortho2D;     
	D3DXMATRIX Identity;
    
	int w , h ;
	if( g_bWindow ){
		w = g_sizeWindowMode.cx ;
		h = g_sizeWindowMode.cy ;
	}
	else{
		w = g_sizeFullMode.cx ;
		h = g_sizeFullMode.cy ;
	}

	
	//
	
	
	
	live2d::framework::L2DViewMatrix*	viewMatrix = s_renderer->getViewMatrix() ;
	D3DXMatrixOrthoOffCenterLH(&Ortho2D
		, viewMatrix->getScreenLeft()  
		, viewMatrix->getScreenRight()  
		, viewMatrix->getScreenBottom()  
		, viewMatrix->getScreenTop() , -1.0f, 1.0f);

	D3DXMatrixIdentity(&Identity);

	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &Ortho2D);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &Identity);
	g_pD3DDevice->SetTransform(D3DTS_VIEW , &Identity);

	
	float* trGL = viewMatrix->getArray() ;
	g_pD3DDevice->MultiplyTransform(D3DTS_WORLD,(D3DXMATRIXA16*)trGL) ;

	
	//D3DVIEWPORT9 vp ;	
	//vp.X = 0 ;
	//vp.Y = 0 ;
	//vp.Width  = w ;
	//vp.Height = h ;
	//vp.MinZ = 0.0f ;
	//vp.MaxZ = 1.0f ;
	
	//g_pD3DDevice->SetViewport(&vp);

}



HRESULT Render(void)
{
	
	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 191), 1.0f, 0);

	
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{

		SetupMatrices() ;
		RenderLive2D() ;

		
		g_pD3DDevice->EndScene();
	}

	
	return g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}


HRESULT CleanupD3DObject(void)
{
	CleanupClippingMask();
	OnLostDeviceLive2D() ;

	return S_OK;
}


HRESULT ResetD3DObject(void)
{
	SetupClippingMask();
	OnResetDeviceLive2D() ;

	return S_OK;
}


HRESULT ChangeWindowSize(void)
{
	
	CleanupD3DObject();

	HRESULT hr = g_pD3DDevice->Reset(&g_D3DPP);
	if (FAILED(hr))
	{
		if (hr == D3DERR_DEVICELOST)
			g_bDeviceLost = true;
		else
			DestroyWindow(g_hWindow);
		return DXTRACE_ERR(L"ChangeWindowSize Reset", hr);
	}
	ResetD3DObject();

	g_sizeWindowMode.cx = g_D3DPP.BackBufferWidth ;
	g_sizeWindowMode.cy = g_D3DPP.BackBufferHeight ;

	
	D3DVIEWPORT9 vp;
	vp.X		= 0;
	vp.Y		= 0;
	vp.Width	= g_D3DPP.BackBufferWidth;
	vp.Height	= g_D3DPP.BackBufferHeight;
	vp.MinZ		= 0.0f;
	vp.MaxZ		= 1.0f;
	hr = g_pD3DDevice->SetViewport(&vp);
	if (FAILED(hr))
	{
		DXTRACE_ERR(L"ChangeWindowSize SetViewport", hr);
		DestroyWindow(g_hWindow);
	}

	
	s_renderer->setDeviceSize( g_D3DPP.BackBufferWidth , g_D3DPP.BackBufferHeight ) ;

	return hr;
}


void ChangeDisplayMode(void)
{
	g_bWindow = !g_bWindow;

	CleanupD3DObject();

	if (g_bWindow)
	{
		g_D3DPP = g_D3DPPWindow;
	}
	else
	{
		g_D3DPP = g_D3DPPFull;
		GetWindowRect(g_hWindow, &g_rectWindow);

		
		ChangeFullscreenResolution() ;
	}

	HRESULT hr = g_pD3DDevice->Reset(&g_D3DPP);
	if (FAILED(hr))
	{
		if (hr == D3DERR_DEVICELOST)
			g_bDeviceLost = true;
		else
			DestroyWindow(g_hWindow);
		DXTRACE_ERR(L"ChangeDisplayMode Reset", hr);
		return;
	}
	ResetD3DObject();

	if (g_bWindow)
	{
		SetWindowLong(g_hWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		if(g_hMenu != NULL)
		{
			SetMenu(g_hWindow, g_hMenu);
			g_hMenu = NULL;
		}
		SetWindowPos(g_hWindow, HWND_NOTOPMOST,
				g_rectWindow.left, g_rectWindow.top,
				g_rectWindow.right - g_rectWindow.left,
				g_rectWindow.bottom - g_rectWindow.top,
				SWP_SHOWWINDOW);
	}
	else
	{
		SetWindowLong(g_hWindow, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		if(g_hMenu == NULL)
		{
			g_hMenu = GetMenu(g_hWindow);
			SetMenu(g_hWindow, NULL);
		}
	}
	
	s_renderer->setDeviceSize( g_D3DPP.BackBufferWidth , g_D3DPP.BackBufferHeight ) ;

}



bool CleanupDXGraphics(void)
{
	SAFE_RELEASE(g_pD3DDevice);
	SAFE_RELEASE(g_pD3D);

	return true;
}


bool CleanupApp(void)
{
	
	if (g_hMenu)
		DestroyMenu(g_hMenu);

	
	UnregisterClass(g_szWndClass, g_hInstance);
	return true;
}


LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HRESULT hr = S_OK;

	switch(msg)
	{
	case WM_ACTIVATE:
		g_bActive = (LOWORD(wParam) != 0);
		break;

	case WM_DESTROY:
		
		CleanupD3DObject();

		
		CleanupLive2D() ;

		
		CleanupDXGraphics();
		
		PostQuitMessage(0);
		g_hWindow = NULL;
		return 0;

	
	case WM_SIZE:
		if (g_D3DPP.Windowed != TRUE)
			break;

		if (!g_pD3DDevice || wParam == SIZE_MINIMIZED)
			break;
		g_D3DPP.BackBufferWidth  = LOWORD(lParam);
		g_D3DPP.BackBufferHeight = HIWORD(lParam);

		if(g_bDeviceLost)
			break;
		if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
			ChangeWindowSize();
		break;

	case WM_SETCURSOR:
		if (g_D3DPP.Windowed != TRUE)
		{
			SetCursor(NULL);
			return 1;
		}
		break;

	case WM_KEYDOWN:
		
		switch(wParam)
		{
		case VK_ESCAPE:	
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case 'W':	
			ChangeDisplayMode();
			break;
		}
		break;

	case WM_RBUTTONDOWN :
		s_live2DMgr->changeModel() ;
		break;

	case WM_LBUTTONDOWN :
		if( (wParam & MK_LBUTTON) != 0 ){
			int xPos = GET_X_LPARAM(lParam); 
			int yPos = GET_Y_LPARAM(lParam); 

			s_renderer->mousePress( xPos , yPos ) ;
		}
		break;

	case WM_MOUSEMOVE :
		if( (wParam & MK_LBUTTON) != 0 ){
			int xPos = GET_X_LPARAM(lParam); 
			int yPos = GET_Y_LPARAM(lParam); 

			s_renderer->mouseDrag( xPos , yPos ) ;
		}

		break;

	case WM_MOUSEWHEEL :
		{
			
			int delta = GET_WHEEL_DELTA_WPARAM(wParam); 
			
			
			POINT cursor;			
			cursor.x = GET_X_LPARAM(lParam); 
			cursor.y = GET_Y_LPARAM(lParam);
			ScreenToClient( hWnd, &cursor);

			s_renderer->mouseWheel( delta , cursor.x , cursor.y ) ;
		}

		break;

	case WM_COMMAND:
		
		switch (LOWORD(wParam))
		{
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			return 0;
		}
		break;
	}

	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


bool AppIdle(void)
{
	if (!g_pD3D || !g_pD3DDevice)
		return false;

	if (!g_bActive)
		return true;

	
	HRESULT hr;
	if (g_bDeviceLost)
	{
		Sleep(100);	

		
		hr  = g_pD3DDevice->TestCooperativeLevel();
		if (FAILED(hr))
		{
			if (hr == D3DERR_DEVICELOST)
				return true;  

			if (hr != D3DERR_DEVICENOTRESET)
				return false; 

			CleanupD3DObject(); 
			hr = g_pD3DDevice->Reset(&g_D3DPP); 
			if (FAILED(hr))
			{
				if (hr == D3DERR_DEVICELOST)
					return true; 

				DXTRACE_ERR(L"AppIdle Reset", hr);
				return false; 
			}
		}
		
		g_bDeviceLost = false;
		ResetD3DObject();
	}

	
	hr = Render();
	if (hr == D3DERR_DEVICELOST)
		g_bDeviceLost = true;	
	else if (FAILED(hr))
		return false;

	return true;
}


int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpCmdLine, int nCmdShow)
{
	
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	
	HRESULT hr = InitApp(hInst);
	if (FAILED(hr))
	{
		DXTRACE_ERR(L"WinMain InitApp", hr);
		return 0;
	}

	
	hr = InitDXGraphics();
	if (FAILED(hr)){
		DXTRACE_ERR(L"WinMain InitDXGraphics", hr);
	}

	
	SetupLive2D() ;

	
	MSG msg;
	do
	{

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			
			if (!AppIdle())
				
				DestroyWindow(g_hWindow);
		}
	} while (msg.message != WM_QUIT);

	
	CleanupApp();
	_CrtDumpMemoryLeaks();

	DXTRACE_MSG(L"\n-- exit --\n") ;
	return (int)msg.wParam;
}


void ChangeFullscreenResolution(){
#if CHANGE_FULLSCREEN_RESOLUTION

	DEVMODE    devMode;
	
	devMode.dmSize       = sizeof(DEVMODE);
	devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;
	devMode.dmPelsWidth  = g_sizeFullMode.cx;
	devMode.dmPelsHeight = g_sizeFullMode.cy;

	ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
#endif
}

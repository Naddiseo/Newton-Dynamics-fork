/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

// NewtonDemos.cpp : Defines the entry point for the application.
//


#include <toolbox_stdafx.h>
#include "NewtonDemos.h"
#include "DemoCamera.h"
#include "sdkDemos.h"
#include "DemoEntityManager.h"

#include <CommDlg.h>

#define MAX_LOADSTRING 100

//#define DEFAULT_SCENE	0						// friction test
//#define DEFAULT_SCENE	1						// closest distance
//#define DEFAULT_SCENE	2						// Box stacks
//#define DEFAULT_SCENE	3						// simple level mesh collision
//#define DEFAULT_SCENE	4						// optimized level mesh collision
#define DEFAULT_SCENE	5						// Scene Collision


#define ALCHEMEDIA_FORMAT_DESCRIPTION	"alchemedia xml\0*.xml\0alchemedia bin\0*.new\0\0"

DemoEntityManager* mainScene = NULL;
bool autoSleepState = true;
bool debugDisplayState = false;
bool showPhysicProfiler = true;
bool showFrameRateInfo = true;
bool concurrentPhysicsUpdates = true;
bool usesSimdInstructions = false;
bool solveIslandOnSingleThread = false;
int threadCount = 1;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name


// Forward declarations of functions included in this code module:
static ATOM	MyRegisterClass(HINSTANCE hInstance);
static BOOL	InitInstance(HINSTANCE, int);
static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK	AboutProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK	OptionsProc(HWND, UINT, WPARAM, LPARAM);



void LoadScene (DemoEntityManager* const scene, char* const fileName);

static HMENU FindMenuOption (HWND mainWindow);
static LRESULT CALLBACK LoadScene (HWND mainWindow);
static LRESULT CALLBACK SaveScene (HWND mainWindow);
static LRESULT CALLBACK ToggleAutoSleep (HWND mainWindow);
static LRESULT CALLBACK ToggleDebugDisplay (HWND mainWindow);
static LRESULT CALLBACK TogglePhysicsProfiler (HWND mainWindow);
static LRESULT CALLBACK ToggleFrameRateInfoBar (HWND mainWindow);
static LRESULT CALLBACK ToggleUsesSimdSimulation (HWND mainWindow);
static LRESULT CALLBACK ToggleConcurrentPhysicsUpdates (HWND mainWindow);
static LRESULT CALLBACK ToggleSolveIslandOnSingleThread (HWND mainWindow);




int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

//	GLContext glContex;
	// load the default scene
	

	// create the scene manager
	DemoEntityManager sceneManager;
	mainScene = &sceneManager;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_NEWTONDEMOS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}


	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_NEWTONDEMOS);

	// Main message loop:
	msg.message = WM_NULL;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
	while( WM_QUIT != msg.message )
	{
		// Use PeekMessage() so we can use idle time to render the scene. 
		bool hasMessage = (PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != FALSE);
		if(hasMessage)	{
			// Translate and dispatch the message
			//if (hAccelTable == NULL || hWnd == NULL || (TranslateAccelerator( hWnd, hAccelTable, &msg) == 0)) {
			if (hAccelTable == NULL || (TranslateAccelerator(msg.hwnd, hAccelTable, &msg) == 0)) {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		} else {
			sceneManager.Render();
		}
	}

	// destroy current scene
	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_NEWTONDEMOS);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_NEWTONDEMOS;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
	  return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Initialize the open Gl context
	mainScene->GetGL().Init(hWnd);

	// Set Camera Size
	RECT rect;
	GetClientRect(hWnd, &rect);
	mainScene->GetCamera()->SetProjectionMode(rect.right, rect.bottom);


	// Load the main menu
	LoadMenuSeletions (hWnd);

	// load the first scene
	LoadDemo (mainScene, DEFAULT_SCENE);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case ID_FILE_LOADSCENE:
					LoadScene (hWnd);
					break;

				case ID_FILE_SAVESCENE:
					SaveScene (hWnd);
					break;

				case IDM_ABOUT:
					DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)AboutProc);
					break;

				case ID_OPTIONS_SELECTNUMBEROFPHYSICSTHREADS:
					DialogBox(hInst, (LPCTSTR)IDD_THREAD_COUNT_SELECTOR, hWnd, (DLGPROC)OptionsProc);
					break;

				case ID_OPTION_SHOWDEBUGDISPLAY:
					ToggleDebugDisplay (hWnd);
					break;

				case ID_OPTIONS_PHYSICSUPDATEINMAINTHREAD:
					ToggleConcurrentPhysicsUpdates(hWnd);
					break;

				case ID_OPTIONS_HIDEPHYSICSPROFILER:
					TogglePhysicsProfiler (hWnd);
					break;

				case ID_OPTIONS_HIDEFRAMERATEINFOBAR:
					ToggleFrameRateInfoBar (hWnd);
					break;

				case ID_OPTIONS_USESIMDINSTRUCTIONS:
					ToggleUsesSimdSimulation (hWnd);
					break;

				case ID_OPTIONS_SOLVEISLANDONONETHREAD:
					ToggleSolveIslandOnSingleThread (hWnd);
					break;

				case ID_OPTIONS_AUTOSLEEPDISABLE:
					ToggleAutoSleep (hWnd);
					break;

				
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;

				default:
					if ((wmId >= IDM_DEMOS) && (wmId < (IDM_DEMOS + 100))) {
						mainScene->Cleanup();
						LoadDemo (mainScene, wmId - IDM_DEMOS);
					} else {
						return DefWindowProc(hWnd, message, wParam, lParam);
					}
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			EndPaint(hWnd, &ps);
			break;

		case WM_ERASEBKGND:
			return TRUE;
			break;		
			
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_HOME:
				// Insert code here to process the HOME key
				// ...
				break;

				case VK_ESCAPE:
					PostQuitMessage(0);
					break;

				default:
				// Insert code here to process other non character keystrokes
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		
		case WM_SIZE:
		{
			if (mainScene->GetGL().GetWindow()) {
				int width = LOWORD(lParam);
				int hight = HIWORD(lParam);
				mainScene->GetCamera()->SetProjectionMode(width, hight);
			}
			break;
		}


		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}




/*
case WM_INITDIALOG:
	SetSliderRange(GetDlgItem(hDlg, IDC_GAINSLIDER), gainmin, gainmax,
		gain);
	...
case WM_HSCROLL:
	{
		HWND hwndScrollBar =3D (HWND) lParam;
		int iPos =3D (short int)HIWORD(wParam);
		if (GetDlgItem(hDlg, IDC_GAINSLIDER)=3D=3DhwndScrollBar ) {
			iPos =3D (int)SendMessage(hwndScrollBar, TBM_GETPOS, 0, 0);

			gain =3D iPos;
			sprintf_s(edtext, EDLEN, "Gain:%d", gain);
			SetDlgItemText(hDlg, IDC_GAIN, (LPCSTR)edtext);
			if (LOWORD(wParam)=3D=3DSB_THUMBPOSITION) { // thumb has been released
				// Set the gain value now
				Beep(500,200);
			}
		}
	}
	...

		A helper function:
	int SetSliderRange(HWND hwndTrack, int iMin, int iMax, int iPos) {
		SendMessage(hwndTrack, TBM_SETRANGE,
			(WPARAM) TRUE, // redraw flag
			(LPARAM) MAKELONG(iMin, iMax)); // min. & max. positions
		SendMessage(hwndTrack, TBM_SETSEL,
			(WPARAM) FALSE, // redraw flag
			(LPARAM) MAKELONG(iMin, iMax));
		SendMessage(hwndTrack, TBM_SETPAGESIZE,
			0, (LPARAM) (iMax-iMin)/10); // new page
		size
			SendMessage(hwndTrack, TBM_SETPOS,
			(WPARAM) TRUE, // redraw flag
			(LPARAM) iPos);

		SetFocus(hwndTrack);
		return 1;
	}

*/


//static LRESULT CALLBACK	AboutProc(HWND, UINT, WPARAM, LPARAM);
//static LRESULT CALLBACK	OptionsProc(HWND, UINT, WPARAM, LPARAM);


static int SetSliderRange(HWND hwndTrack, int iMin, int iMax, int iPos) 
{
	SendMessage(hwndTrack, TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG(iMin, iMax)); 
	SendMessage(hwndTrack, TBM_SETSEL,(WPARAM) FALSE, (LPARAM) MAKELONG(iMin, iMax));
	SendMessage(hwndTrack, TBM_SETPAGESIZE,	0, (LPARAM) (iMax-iMin)/8); 
	SendMessage(hwndTrack, TBM_SETPOS, (WPARAM) TRUE, (LPARAM) iPos);

	SetFocus(hwndTrack);
	return 1;
}


// Message handler for about box.
LRESULT CALLBACK OptionsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char text[256];

	switch (message)
	{
		case WM_INITDIALOG:
		{
			int maxthreads = NewtonGetMaxThreadsCount(mainScene->GetNewton());
			int pos = NewtonGetThreadsCount(mainScene->GetNewton());
			SetSliderRange(GetDlgItem(hDlg, IDC_THREAD_COUNT), 1, maxthreads, pos);
			
			sprintf(text, "Thread count = %d", pos);
			SetDlgItemText(hDlg, IDC_THREAD_COUNT_TEXT, (LPCSTR)text);
			return TRUE;
		}

		case WM_HSCROLL:
		{
			HWND hwndScrollBar = (HWND) lParam;
			if (GetDlgItem(hDlg, IDC_THREAD_COUNT) == hwndScrollBar ) {
				int newpos = (int)SendMessage(hwndScrollBar, TBM_GETPOS, 0, 0);
				sprintf(text, "Thread count = %d", newpos);
				SetDlgItemText(hDlg, IDC_THREAD_COUNT_TEXT, (LPCSTR)text);
				threadCount = newpos;

				mainScene->StopsExecution ();
				NewtonSetThreadsCount(mainScene->GetNewton(), threadCount);
				mainScene->ContinueExecution();
			}

			return TRUE;
		}

		case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
		}
	}
	return FALSE;
}

LRESULT CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}



HMENU FindMenuOption (HWND mainWindow)
{
	HMENU hMenu = GetMenu(mainWindow);  
	int count = GetMenuItemCount(hMenu);
	for (int i = 0; i < count; i ++) {
		char name[256];
		GetMenuString(hMenu, i, name, sizeof (name), MF_BYPOSITION);
		if (!stricmp (name, "Options")) {
			return GetSubMenu(hMenu, i);
		}
	}
	return NULL;
}


LRESULT CALLBACK ToggleDebugDisplay (HWND mainWindow)
{
	HMENU hMenu = GetMenu(mainWindow);  

	debugDisplayState = !debugDisplayState;
	int state = debugDisplayState ? MF_CHECKED : MF_UNCHECKED;
	CheckMenuItem (hMenu, ID_OPTION_SHOWDEBUGDISPLAY, MF_BYCOMMAND | state);

	return FALSE;
}

LRESULT CALLBACK ToggleConcurrentPhysicsUpdates (HWND mainWindow)
{
	HMENU hMenu = GetMenu(mainWindow);  

	concurrentPhysicsUpdates = !concurrentPhysicsUpdates;
	int state = concurrentPhysicsUpdates ? MF_CHECKED : MF_UNCHECKED;
	CheckMenuItem (hMenu, ID_OPTIONS_PHYSICSUPDATEINMAINTHREAD, MF_BYCOMMAND | state);

	mainScene->m_asycronousUpdate = concurrentPhysicsUpdates;

	return FALSE;
}

LRESULT CALLBACK TogglePhysicsProfiler (HWND mainWindow)
{
	HMENU hMenu = GetMenu(mainWindow);  

	showPhysicProfiler = !showPhysicProfiler;
	int state = !showPhysicProfiler ? MF_CHECKED : MF_UNCHECKED;
	CheckMenuItem (hMenu, ID_OPTIONS_HIDEPHYSICSPROFILER, MF_BYCOMMAND | state);

	return FALSE;
}

LRESULT CALLBACK ToggleFrameRateInfoBar (HWND mainWindow)
{
	HMENU hMenu = GetMenu(mainWindow);  

	showFrameRateInfo = !showFrameRateInfo;
	int state = !showFrameRateInfo ? MF_CHECKED : MF_UNCHECKED;
	CheckMenuItem (hMenu, ID_OPTIONS_HIDEFRAMERATEINFOBAR, MF_BYCOMMAND | state);

	return FALSE;
}

LRESULT CALLBACK ToggleUsesSimdSimulation (HWND mainWindow)
{
	HMENU hMenu = GetMenu(mainWindow);  

	usesSimdInstructions = !usesSimdInstructions;
	int state = usesSimdInstructions ? MF_CHECKED : MF_UNCHECKED;
	CheckMenuItem (hMenu, ID_OPTIONS_USESIMDINSTRUCTIONS, MF_BYCOMMAND | state);

	mainScene->StopsExecution ();
	if (usesSimdInstructions) {
		NewtonSetPlatformArchitecture (mainScene->GetNewton(), 3);  //best hardware (SSE at this time)
	} else {
		NewtonSetPlatformArchitecture (mainScene->GetNewton(), 0);  //x87 mode
	}
	mainScene->ContinueExecution();

	return FALSE;
}


LRESULT CALLBACK ToggleSolveIslandOnSingleThread (HWND mainWindow)
{
	HMENU hMenu = GetMenu(mainWindow);  
	solveIslandOnSingleThread = !solveIslandOnSingleThread;

	int state = solveIslandOnSingleThread ? MF_CHECKED : MF_UNCHECKED;
	CheckMenuItem (hMenu, ID_OPTIONS_SOLVEISLANDONONETHREAD, MF_BYCOMMAND | state);

	mainScene->StopsExecution ();
	NewtonSetMultiThreadSolverOnSingleIsland (mainScene->GetNewton(), solveIslandOnSingleThread ? 1 : 0);
	mainScene->ContinueExecution();

	return FALSE;
}

LRESULT CALLBACK ToggleAutoSleep (HWND mainWindow)
{
	HMENU hMenu = GetMenu(mainWindow);  
	autoSleepState = !autoSleepState;

	int state = !autoSleepState ? MF_CHECKED : MF_UNCHECKED;
	CheckMenuItem (hMenu, ID_OPTIONS_AUTOSLEEPDISABLE, MF_BYCOMMAND | state);

	mainScene->StopsExecution ();
	mainScene->SetAutoSleepState (autoSleepState);
	mainScene->ContinueExecution();

	return FALSE;
}


LRESULT CALLBACK LoadScene (HWND mainWindow)
{
	char fileNamePath[1024];
	fileNamePath[0] = 0;

	OPENFILENAME fileNameInfo;

	memset (&fileNameInfo, 0, sizeof (fileNameInfo));
	fileNameInfo.lStructSize = sizeof (fileNameInfo);
	fileNameInfo.lpstrTitle = "Load Alchemedia Scene";
	fileNameInfo.lpstrFilter = ALCHEMEDIA_FORMAT_DESCRIPTION;
	fileNameInfo.lpstrFile = fileNamePath;
	fileNameInfo.nMaxFile = sizeof (fileNamePath);
	if (GetOpenFileName(&fileNameInfo)) {
		mainScene->Cleanup();
		LoadScene (mainScene, fileNamePath);
		mainScene->SetAutoSleepState (autoSleepState);
	}
	return FALSE;
}

static LRESULT CALLBACK SaveScene (HWND mainWindow)
{
	return FALSE;
}

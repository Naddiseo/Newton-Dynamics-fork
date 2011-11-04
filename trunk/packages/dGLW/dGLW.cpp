/* Copyright (c) <2010-2011> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/


#include "dGLWstdafx.h"
#include "dGLWWidget.h"
#include "dGLW.h"
#include "dGLWDrawContext.h"


#define GLW_CLASS_INFO_NAME "dGLW"


dGLW::dGLW(void)
	:m_rootWidgets()
{
	RegisterClass();


}

dGLW::~dGLW(void)
{
}


void dGLW::AddRootWidget(dGLWWidget* const widget)
{
	m_rootWidgets.Append(widget);
}

void dGLW::RemoveRootWidget(dGLWWidget* const widget)
{
	for (dList<dGLWWidget*>::dListNode* node = m_rootWidgets.GetFirst(); node; node = node->GetNext()) {
		if (node->GetInfo() == widget) {
			m_rootWidgets.Remove(node);
			return;
		}
	}
}



void dGLW::RegisterClass()
{
	// the application instance
    m_instance = GetModuleHandle( NULL );

	// see if the application was already registered
	WNDCLASSEX winClass;
	BOOL state = GetClassInfoEx (m_instance, _T(GLW_CLASS_INFO_NAME), &winClass);

	if (!state) {
		// register the window class
		memset (&winClass, 0, sizeof (winClass));

		winClass.cbSize = sizeof(WNDCLASSEX); 

		winClass.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		winClass.lpfnWndProc	= WndProc;
		winClass.cbClsExtra		= 0;
		winClass.cbWndExtra		= 0;
		winClass.hInstance		= m_instance;
		winClass.hIcon			= LoadIcon(m_instance, _T(GLW_CLASS_INFO_NAME));
		winClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
		winClass.hbrBackground	= NULL;
		winClass.lpszMenuName	= NULL;
		winClass.lpszClassName	= _T(GLW_CLASS_INFO_NAME);
		winClass.hIconSm		= LoadIcon(m_instance, _T(GLW_CLASS_INFO_NAME));
		
		state = RegisterClassEx (&winClass);
		_ASSERTE (state);
	}

	m_winClass = winClass; 
	m_screenWidth = GetSystemMetrics( SM_CXSCREEN );
	m_screenHigh = GetSystemMetrics( SM_CYSCREEN );

}





void dGLW::Run()
{

//	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_NEWTONDEMOS);

	// Main message loop:

	MSG msg;
	msg.message = WM_NULL;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
	while( WM_QUIT != msg.message) {
		// Use PeekMessage() so we can use idle time to render the scene. 
		bool hasMessage = (PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != FALSE);
		if(hasMessage)	{
//			if (hAccelTable == NULL || (TranslateAccelerator(msg.hwnd, hAccelTable, &msg) == 0)) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		} else {
//			sceneManager.Render();
		}
		Sleep (0);
	}
}




LRESULT CALLBACK dGLW::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
//	PAINTSTRUCT ps;
//	HDC hdc;

	dGLWWidget* const widget = (dGLWWidget*) GetWindowLongPtr (hWnd, GWLP_USERDATA);
	_ASSERTE (!widget || (widget->IsType(dGLWWidget::GetRttiType()) && (widget->m_nativeHandle == hWnd)));

	switch (message) 
	{

		case WM_COMMAND:
/*
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
*/
		break;

/*
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

*/

		case WM_CREATE:
		{
			LPCREATESTRUCT const structure =  (LPCREATESTRUCT) lParam;
			dGLWWidget* const widget = (dGLWWidget*) structure->lpCreateParams;
			_ASSERTE (widget);
			_ASSERTE (widget->IsType(dGLWWidget::GetRttiType()));
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)widget);
			widget->m_nativeHandle = hWnd;
			break;
		}

		case WM_DESTROY:
		{
			if (widget) {
				dGLW* const me = widget->GetGLW();
				SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
				delete widget;

				if (!me->m_rootWidgets.GetCount()) {
					PostQuitMessage(0);
				}
			}
			break;
		}

		case WM_MOVE:
		{
			if (widget) {
				widget->OnPosition(LOWORD(lParam), HIWORD(lParam));
			}
			break;

		}
	

		case WM_SIZE:
		{
			if (widget) {
				widget->OnSize(LOWORD(lParam), HIWORD(lParam));
			}
			break;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			if (widget) {
				dGLWDrawContext drawContext;
				drawContext.m_hdc = hdc;
				drawContext.m_clientWidth = widget->m_client.m_width;
				drawContext.m_clientHeight = widget->m_client.m_height;
				widget->OnPaint(drawContext);
			}

			EndPaint(hWnd, &ps);
			break;
		}

		case WM_ERASEBKGND:
			return TRUE;
			break;		

//		case WM_SYSCOMMAND:
//			return DefWindowProc(hWnd, message, wParam, lParam);
//			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


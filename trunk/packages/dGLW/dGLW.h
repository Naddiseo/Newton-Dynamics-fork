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

#ifndef __dGLW_H_
#define __dGLW_H_

class dGLWWidget;


class dGLW
{
	public:
	dGLW(void);
	virtual ~dGLW(void);

	void Run();


	private:
	void AddRootWidget(dGLWWidget* const widget);
	void RemoveRootWidget(dGLWWidget* const widget);

	int m_screenWidth;
	int m_screenHigh;
	dList<dGLWWidget*> m_rootWidgets;

	

#ifdef _GLW_WIN32
	void RegisterClass();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HINSTANCE m_instance;
	WNDCLASSEX m_winClass;
#endif

	friend dGLWWidget; 
};


#endif
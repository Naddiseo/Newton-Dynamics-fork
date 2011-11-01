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


#ifndef __dGLWstdafx_H_
#define __dGLWstdafx_H_


#ifdef _GLW_WIN32
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <tchar.h>
#include <windows.h>
#include <crtdbg.h>
//#include <GL/glew.h>
//#include <GL/wglew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#endif

#include <dContainersStdAfx.h>

#include <dRtti.h>
#include <dList.h>
#include <dRefCounter.h>

#include <string>
using namespace std;


#ifdef _GLW_WIN32
typedef HWND dGLW_HANDLE;
#endif


struct dGLWColor
{
	unsigned char m_red; 
	unsigned char m_green; 
	unsigned char m_blue; 
	unsigned char m_alpha; 
};

struct dGLWRect
{
	int m_x;
	int m_y;
	int m_width;
	int m_height;
};



#ifndef dGLW_UNUSED_ARGUMENT
	#define dGLW_UNUSED_ARGUMENT(x) x
#endif

#endif
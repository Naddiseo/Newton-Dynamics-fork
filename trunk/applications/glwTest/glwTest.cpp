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


// glwTest.cpp : Defines the entry point for the console application.
//

#include <crtdbg.h>

#include <dGLWstdafx.h>
//#include "dGLWButton.h"
#include "dGLWFrame.h"
#include "dGLWMenuBar.h"
#include <dGLW.h>


class Demo: public dGLWFrame
{

	public:
	Demo (dGLW& glw)
		:dGLWFrame (&glw, "GLW demo")
	{
		m_menu = new dGLWMenuBar (this);
//		dGLWMenuItem* item0 = m_menu->CreateItem("xxx0");
//		dGLWMenuItem* item1 = m_menu->CreateItem("xxx1");
	}


	dGLWMenuBar* m_menu;

};

int main(int argc, char* argv[])
{
#ifdef _MSC_VER
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	dGLW_UNUSED_ARGUMENT(argc);
	dGLW_UNUSED_ARGUMENT(argv);

	dGLW glw;
	new Demo (glw);

	glw.Run();
	return 0;
}


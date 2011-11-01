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
#include "dGLWDrawContext.h"



#define GLW_CLASS_INFO_NAME "dGLWDrawContext"


dGLWDrawContext::dGLWDrawContext(void)
{
}

dGLWDrawContext::~dGLWDrawContext(void)
{
}

void dGLWDrawContext::SetBrushColor (const dGLWColor& color) const
{
	SelectObject(m_hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(m_hdc, RGB(color.m_red, color.m_green, color.m_blue));
}


void dGLWDrawContext::DrawLine (int x0, int y0, int x1, int y1) const
{
//	Graphics graphics(m_hdc);
//	Pen pen(Color(255, 0, 0, 255));
//	graphics.DrawLine(&pen, x0, y0, x1, y1);

	MoveToEx(m_hdc, x0, y0, NULL);
	LineTo(m_hdc, x1, y1);
}

void dGLWDrawContext::ClearRectangle (int x0, int y0, int x1, int y1) const
{
	Rectangle(m_hdc, x0, y0, x1, y1);
}

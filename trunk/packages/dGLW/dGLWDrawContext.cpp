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
	:m_hdc(0)
	,hPen(0)
{
	hPen = CreatePen(PS_DOT, 1, RGB(0,0,0));

	dGLWColor color;
	color.m_red = 0;
	color.m_green = 0;
	color.m_blue = 0;
	SetFontColor (color);
}

dGLWDrawContext::~dGLWDrawContext(void)
{
	DeleteObject(hPen);
}

void dGLWDrawContext::SetBrushColor (const dGLWColor& color)
{
	SelectObject(m_hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(m_hdc, RGB(color.m_red, color.m_green, color.m_blue));
}

void dGLWDrawContext::SetPenColor(const dGLWColor& color)
{
	if(hPen) {
		DeleteObject(hPen);
	}
	hPen = CreatePen(PS_DOT, 1, RGB(color.m_red, color.m_green, color.m_blue));
}


void dGLWDrawContext::DrawLine (int x0, int y0, int x1, int y1)
{
//	Graphics graphics(m_hdc);
//	Pen pen(Color(255, 0, 0, 255));
//	graphics.DrawLine(&pen, x0, y0, x1, y1);

	MoveToEx(m_hdc, x0, y0, NULL);
	LineTo(m_hdc, x1, y1);
}

void dGLWDrawContext::ClearRectangle (int x0, int y0, int x1, int y1)
{
	Rectangle(m_hdc, x0, y0, x1, y1);
}

int dGLWDrawContext::GetFontHeight() const
{
	TEXTMETRIC m;
	GetTextMetrics(m_hdc,&m);
	return m.tmHeight;
}


void dGLWDrawContext::SetFontColor(const dGLWColor& color)
{
	SetTextColor(m_hdc, RGB(color.m_red, color.m_green, color.m_blue));
}


void dGLWDrawContext::Print (int x, int y, const char* const text, ...)
{
	va_list v_args;
	char tmp[2048];

	tmp[0] = 0;
	va_start (v_args, text);     
	vsprintf(tmp, text, v_args);
	va_end (v_args);            

	SetBkMode(m_hdc, TRANSPARENT);

	HFONT hFont = (HFONT)GetStockObject(SYSTEM_FONT); 
//	HFONT hFont = (HFONT)GetStockObject(ANSI_VAR_FONT); 	

	// Select the variable stock font into the specified device context. 
	HFONT hOldFont = (HFONT)SelectObject(m_hdc, hFont);
	// Display the text string.  
	TextOut(m_hdc, x, y, tmp, strlen (tmp)); 

	// Restore the original font.        
	SelectObject(m_hdc, hOldFont); 
}

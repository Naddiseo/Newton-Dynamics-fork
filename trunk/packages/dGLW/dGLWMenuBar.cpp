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
#include "dGLWMenuBar.h"
#include "dGLWMenuItem.h"

dGLWMenuBar::dGLWMenuBar(dGLWWidget* const parent)
	:dGLWHorizontalLayout (parent->GetGLW(), parent, m_child)
	,m_height(30)
{
	dGLWColor color;
	color.m_red = 241;
	color.m_green = 244;
	color.m_blue = 251;
	color.m_alpha = 255;
	SetBackgroundColor(color);


/*
	BOOL GetTextMetrics(
		__in   HDC hdc,
		__out  LPTEXTMETRIC lptm
		);

*/

	Update();

	int width;
	int height;
	PredictNewSize(width, height);
	SetSize(width, height);
	OnSize(width, height);

	m_parent->PredictNewSize(width, height);
	m_parent->OnSize(width, height);
}

dGLWMenuBar::~dGLWMenuBar(void)
{
}

void dGLWMenuBar::PredictNewSize(int& width, int& height) const
{
	dGLWRect rect (m_parent->GetClientRect());

	width = rect.m_width;
	height = m_height;
}

void dGLWMenuBar::SetSize(int width, int height)
{
	m_rect.m_x = 0;
	m_rect.m_y = 0;
	dGLWHorizontalLayout::SetSize(width, height);
}

void dGLWMenuBar::OnSize(int width, int height)
{
	dGLWHorizontalLayout::OnSize(width, height);
}

dGLWMenuItem* dGLWMenuBar::CreateItem(const string& title)
{
	return new dGLWMenuItem (this, title) ;
}


void dGLWMenuBar::OnPaint(dGLWDrawContext& gdc)
{	

	dGLWHorizontalLayout::OnPaint(gdc);

	m_height = gdc.GetFontHeight() + 8;
//	dGLWColor color;
//	color.m_red = 0;
//	color.m_green = 0;
//	color.m_blue = 0;
//	color.m_alpha = 0;
//	gdc.SetFontColor(color, m_bkColor);
	gdc.Print (10, 4, "File");
}


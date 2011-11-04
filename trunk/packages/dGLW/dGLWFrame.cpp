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
#include "dGLWFrame.h"

dGLWFrame::dGLWFrame(dGLW* const glw, const string& title)
	:dGLWDocker (glw, NULL, m_frame) 
	,m_title(title)
{
	SetWindowText (m_nativeHandle, m_title.c_str());
	SetSize(m_rect.m_width, m_rect.m_height);
}

dGLWFrame::dGLWFrame(dGLWWidget* const parent, const string& title)
	:dGLWDocker (parent->GetGLW(), parent, m_frame)
	,m_title(title)
{
	SetWindowText (m_nativeHandle, m_title.c_str());
}


dGLWFrame::~dGLWFrame(void)
{
}

void dGLWFrame::OnSize(int width, int height)
{
	dGLWDocker::OnSize(width, height);

	for (dList<dGLWWidget*>::dListNode* node = m_children.GetFirst(); node; node = node->GetNext()) {
		dGLWWidget* const widget = node->GetInfo();
		if (widget->GetWidndowType() == m_child) {
			int childWidth;
			int childHeight;
			widget->PredictNewSize(childWidth, childHeight);
			widget->SetSize(childWidth, childHeight);
		}
	}
	dGLWDocker::OnSize(width, height);
}

void dGLWFrame::OnPaint(dGLWDrawContext& gdc)
{
	dGLWDocker::OnPaint(gdc);
//	dGLW_UNUSED_ARGUMENT(gdc);
//	gdc.SetBrushColor (m_bkColor);
//	gdc.ClearRectangle (0, 0, m_client.m_width, m_client.m_height);
//	gdc.DrawLine( 100, 100, 200, 200);
}
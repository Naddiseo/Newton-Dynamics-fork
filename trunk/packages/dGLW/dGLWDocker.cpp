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
#include "dGLWDocker.h"
#include "dGLWMenuBar.h"

dGLWDocker::dGLWDocker(dGLW* const glw, dGLWWidget* const parent, dFrameType type)
	:dGLWWidget (glw, parent, type)
{
	SetSize(m_rect.m_width, m_rect.m_height);
}

dGLWDocker::~dGLWDocker(void)
{
}


void dGLWDocker::OnSize(int width, int height)
{
	dGLWWidget::OnSize(width, height);

	dGLWMenuBar* menu = NULL;
	for (dList<dGLWWidget*>::dListNode* node = m_children.GetFirst(); node; node = node->GetNext()) {
		dGLWWidget* const widget = node->GetInfo();
		if (widget->IsType(dGLWMenuBar::GetRttiType())) {
			menu = (dGLWMenuBar*) widget;
		}
	}

	OnPosition(m_rect.m_x, m_rect.m_y);
	if (menu) {
		dGLWRect rect (menu->GetRect());
		m_client.m_y += rect.m_height;
		m_client.m_height -= rect.m_height;
	}
}


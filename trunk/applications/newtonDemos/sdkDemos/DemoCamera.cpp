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



// RenderPrimitive.cpp: implementation of the RenderPrimitive class.
//
//////////////////////////////////////////////////////////////////////
#include <toolbox_stdafx.h>
#include "DemoCamera.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


DemoCamera::DemoCamera()
	:DemoEntity (NULL) 
{
	m_fov = 60.0f * 3.1416f / 180.0f;
	m_frontPlane = 0.1f;
	m_backPlane = 1000.0f;
}


void DemoCamera::Render(dFloat timeStep) const
{
}
	
DemoCamera::~DemoCamera()
{
}

void DemoCamera::SetProjectionMode(int width, int height)
{
	glViewport(0, 0, (GLint) width, (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(m_fov * 180.0f / 3.1416f, GLfloat (width) /GLfloat(height), m_frontPlane, m_backPlane);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//dMatrix matrix;
	//GLint viewport[4]; 
	//glGetIntegerv(GL_VIEWPORT, viewport); 
	//glGetFloat(GL_PROJECTION_MATRIX, &matrix[0][0]);
	//glGetFloat(GL_PROJECTION_MATRIX, &matrix[0][0]);
}

void DemoCamera::Update()
{
	dVector pointOfInterest (m_matrix.m_posit + m_matrix.m_front);

	gluLookAt(m_matrix.m_posit.m_x, m_matrix.m_posit.m_y, m_matrix.m_posit.m_z, 
		      pointOfInterest.m_x, pointOfInterest.m_y, pointOfInterest.m_z, 
			  m_matrix.m_up.m_x, m_matrix.m_up.m_y, m_matrix.m_up.m_z);	
}



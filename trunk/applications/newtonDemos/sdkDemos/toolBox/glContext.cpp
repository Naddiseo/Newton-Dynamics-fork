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


#include "toolbox_stdafx.h"
#include "glcontext.h"
#include "TargaToOpenGl.h"


GLContext::GLContext()
{
	m_wnd = NULL;
	m_hDC = NULL;
	m_hRC = NULL;
	m_fonts = 0;
}


GLContext::~GLContext()
{
#ifndef _MSC_VER
	UnloadTexture (m_fonts);
#else 
	glDeleteLists(m_fonts, 96);	
#endif

	if (m_hRC) {
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext (m_hRC);
	}

	if (m_wnd && m_hDC) {
		ReleaseDC (m_wnd, m_hDC);
	}
}


void GLContext::Init(HWND hWnd)
{
	// remember the window handle (HWND)
	m_wnd = hWnd;

	// get the device context (DC)
	m_hDC = GetDC (m_wnd);

	// set the pixel format for the DC
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |	PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int format = ChoosePixelFormat(m_hDC, &pfd );
	SetPixelFormat (m_hDC, format, &pfd );


	// obtain detailed information about  
	// the device context's first pixel format  
	PIXELFORMATDESCRIPTOR pfd1;
	DescribePixelFormat(m_hDC, format, sizeof(PIXELFORMATDESCRIPTOR), &pfd1);

	// create the render context (RC)
	m_hRC = wglCreateContext(m_hDC);

	// make it the current render context
	wglMakeCurrent (m_hDC, m_hRC);


	// Load the Font Texture
#ifndef _MSC_VER
	m_fonts = LoadTexture ("Arial.tga");
#else 
	m_fonts = BuildFont();
#endif
}


#ifndef _MSC_VER
static dFloat gAerialFontUV[128][4] = 
{
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 0.019531f, 0.074219f},
	{0.023438f, 0.000000f, 0.046875f, 0.074219f},
	{0.050781f, 0.000000f, 0.082031f, 0.074219f},
	{0.085938f, 0.000000f, 0.121094f, 0.074219f},
	{0.125000f, 0.000000f, 0.160156f, 0.074219f},
	{0.164063f, 0.000000f, 0.226563f, 0.074219f},
	{0.230469f, 0.000000f, 0.277344f, 0.074219f},
	{0.281250f, 0.000000f, 0.296875f, 0.074219f},
	{0.300781f, 0.000000f, 0.324219f, 0.074219f},
	{0.328125f, 0.000000f, 0.351563f, 0.074219f},
	{0.355469f, 0.000000f, 0.382813f, 0.074219f},
	{0.386719f, 0.000000f, 0.425781f, 0.074219f},
	{0.429688f, 0.000000f, 0.449219f, 0.074219f},
	{0.453125f, 0.000000f, 0.476563f, 0.074219f},
	{0.480469f, 0.000000f, 0.500000f, 0.074219f},
	{0.503906f, 0.000000f, 0.523438f, 0.074219f},
	{0.527344f, 0.000000f, 0.562500f, 0.074219f},
	{0.566406f, 0.000000f, 0.601563f, 0.074219f},
	{0.605469f, 0.000000f, 0.640625f, 0.074219f},
	{0.644531f, 0.000000f, 0.679688f, 0.074219f},
	{0.683594f, 0.000000f, 0.718750f, 0.074219f},
	{0.722656f, 0.000000f, 0.757813f, 0.074219f},
	{0.761719f, 0.000000f, 0.796875f, 0.074219f},
	{0.800781f, 0.000000f, 0.835938f, 0.074219f},
	{0.839844f, 0.000000f, 0.875000f, 0.074219f},
	{0.878906f, 0.000000f, 0.914063f, 0.074219f},
	{0.917969f, 0.000000f, 0.941406f, 0.074219f},
	{0.945313f, 0.000000f, 0.968750f, 0.074219f},
	{0.000000f, 0.078125f, 0.039063f, 0.152344f},
	{0.042969f, 0.078125f, 0.082031f, 0.152344f},
	{0.085938f, 0.078125f, 0.125000f, 0.152344f},
	{0.128906f, 0.078125f, 0.167969f, 0.152344f},
	{0.171875f, 0.078125f, 0.238281f, 0.152344f},
	{0.242188f, 0.078125f, 0.285156f, 0.152344f},
	{0.289063f, 0.078125f, 0.335938f, 0.152344f},
	{0.339844f, 0.078125f, 0.386719f, 0.152344f},
	{0.390625f, 0.078125f, 0.437500f, 0.152344f},
	{0.441406f, 0.078125f, 0.484375f, 0.152344f},
	{0.488281f, 0.078125f, 0.527344f, 0.152344f},
	{0.531250f, 0.078125f, 0.582031f, 0.152344f},
	{0.585938f, 0.078125f, 0.632813f, 0.152344f},
	{0.636719f, 0.078125f, 0.652344f, 0.152344f},
	{0.656250f, 0.078125f, 0.691406f, 0.152344f},
	{0.695313f, 0.078125f, 0.742188f, 0.152344f},
	{0.746094f, 0.078125f, 0.785156f, 0.152344f},
	{0.789063f, 0.078125f, 0.839844f, 0.152344f},
	{0.843750f, 0.078125f, 0.890625f, 0.152344f},
	{0.894531f, 0.078125f, 0.945313f, 0.152344f},
	{0.949219f, 0.078125f, 0.992188f, 0.152344f},
	{0.000000f, 0.156250f, 0.050781f, 0.230469f},
	{0.054688f, 0.156250f, 0.101563f, 0.230469f},
	{0.105469f, 0.156250f, 0.148438f, 0.230469f},
	{0.152344f, 0.156250f, 0.191406f, 0.230469f},
	{0.195313f, 0.156250f, 0.242188f, 0.230469f},
	{0.246094f, 0.156250f, 0.289063f, 0.230469f},
	{0.292969f, 0.156250f, 0.359375f, 0.230469f},
	{0.363281f, 0.156250f, 0.406250f, 0.230469f},
	{0.410156f, 0.156250f, 0.449219f, 0.230469f},
	{0.453125f, 0.156250f, 0.488281f, 0.230469f},
	{0.492188f, 0.156250f, 0.515625f, 0.230469f},
	{0.519531f, 0.156250f, 0.539063f, 0.230469f},
	{0.542969f, 0.156250f, 0.566406f, 0.230469f},
	{0.570313f, 0.156250f, 0.609375f, 0.230469f},
	{0.613281f, 0.156250f, 0.648438f, 0.230469f},
	{0.652344f, 0.156250f, 0.675781f, 0.230469f},
	{0.679688f, 0.156250f, 0.714844f, 0.230469f},
	{0.718750f, 0.156250f, 0.757813f, 0.230469f},
	{0.761719f, 0.156250f, 0.796875f, 0.230469f},
	{0.800781f, 0.156250f, 0.839844f, 0.230469f},
	{0.843750f, 0.156250f, 0.878906f, 0.230469f},
	{0.882813f, 0.156250f, 0.902344f, 0.230469f},
	{0.906250f, 0.156250f, 0.945313f, 0.230469f},
	{0.949219f, 0.156250f, 0.988281f, 0.230469f},
	{0.000000f, 0.234375f, 0.015625f, 0.308594f},
	{0.019531f, 0.234375f, 0.035156f, 0.308594f},
	{0.039063f, 0.234375f, 0.074219f, 0.308594f},
	{0.078125f, 0.234375f, 0.093750f, 0.308594f},
	{0.097656f, 0.234375f, 0.152344f, 0.308594f},
	{0.156250f, 0.234375f, 0.195313f, 0.308594f},
	{0.199219f, 0.234375f, 0.238281f, 0.308594f},
	{0.242188f, 0.234375f, 0.281250f, 0.308594f},
	{0.285156f, 0.234375f, 0.324219f, 0.308594f},
	{0.328125f, 0.234375f, 0.355469f, 0.308594f},
	{0.359375f, 0.234375f, 0.394531f, 0.308594f},
	{0.398438f, 0.234375f, 0.421875f, 0.308594f},
	{0.425781f, 0.234375f, 0.464844f, 0.308594f},
	{0.468750f, 0.234375f, 0.503906f, 0.308594f},
	{0.507813f, 0.234375f, 0.558594f, 0.308594f},
	{0.562500f, 0.234375f, 0.597656f, 0.308594f},
	{0.601563f, 0.234375f, 0.636719f, 0.308594f},
	{0.640625f, 0.234375f, 0.675781f, 0.308594f},
	{0.679688f, 0.234375f, 0.707031f, 0.308594f},
	{0.710938f, 0.234375f, 0.726563f, 0.308594f},
	{0.730469f, 0.234375f, 0.757813f, 0.308594f},
	{0.761719f, 0.234375f, 0.800781f, 0.308594f}
};											   

void GLContext::Print (const dVector& color, dFloat x, dFloat y, const char *fmt, ... ) const
{
	// enter 2d display mode 
	int len;
	int witdh;
	int height;
	dFloat sx;
	dFloat sy;
	va_list argptr;
	char string[1024];

	// Note, there may be other things you need to change,
	//depending on how you have your OpenGL state set up.
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glColor3f(color.m_x, color.m_y, color.m_z);
	// This allows alpha blending of 2D textures with the scene 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);


	GLint viewport[4]; 
	//Retrieves the viewport and stores it in the variable
	glGetIntegerv(GL_VIEWPORT, viewport); 
	witdh = viewport[2];
	height = viewport[3];

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)witdh, height, 0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	// draw the text on the screen
	glBindTexture(GL_TEXTURE_2D, gAerialFontImage);

	va_start (argptr, fmt);
	vsprintf (string, fmt, argptr);
	va_end( argptr );
	len = (int) strlen (string );

	glBegin(GL_QUADS);

	sx = x + 0.5f;
	sy = y + 0.5f;
	for (int i = 0; i < len; i++) {
		dFloat w;
		dFloat h;
		dFloat tx1;
		dFloat ty1;
		dFloat tx2;
		dFloat ty2;
		unsigned char c;

		c = string[i];
		tx1 = gAerialFontUV[c][0];
		ty1 = gAerialFontUV[c][1];
		tx2 = gAerialFontUV[c][2];
		ty2 = gAerialFontUV[c][3];
		w = (tx2-tx1) * 256.0f;
		h = (ty2-ty1) * 256.0f;

		glTexCoord2f (tx1, 1.0f-ty1); glVertex2f(sx + 0, sy + 0);
		glTexCoord2f (tx2, 1.0f-ty1); glVertex2f(sx + w, sy + 0);
		glTexCoord2f (tx2, 1.0f-ty2); glVertex2f(sx + w, sy + h);
		glTexCoord2f (tx1, 1.0f-ty2); glVertex2f(sx + 0, sy + h);
		sx += w;
	}
	glEnd();

	// exit 2d display mode
	glDisable(GL_BLEND);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();

	//	setup_opengl (screen->w, screen->h);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

#else 


void GLContext::Print (const dVector& color, dFloat x, dFloat y, const char *fmt, ... ) const
{
	glColor3f(color.m_x, color.m_y, color.m_z);

	GLint viewport[4]; 
	//Retrieves the viewport and stores it in the variable
	glGetIntegerv(GL_VIEWPORT, viewport); 
	int witdh = viewport[2];
	int height = viewport[3];

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)witdh, height, 0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2f(x, y + 16);

	va_list argptr;
	char string[1024];

	va_start (argptr, fmt);
	vsprintf (string, fmt, argptr);
	va_end( argptr );


	glPushAttrib(GL_LIST_BIT);
	glListBase(m_fonts - 32);	
	int lenght = (int) strlen (string);
	glCallLists (lenght, GL_UNSIGNED_BYTE, string);	
	glPopAttrib();				


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}


GLuint GLContext::BuildFont()
{
	HFONT font = CreateFont(
		20,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,                   // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));              // lpszFacename


	// only 96 usable ascii for performance in some low end system	
	GLuint displayLists = glGenLists(96);	

	// remember the old font
	HFONT oldfont = (HFONT)SelectObject  (m_hDC, font);	
	wglUseFontBitmaps(m_hDC, 32, 96, displayLists);				
	SelectObject(m_hDC, oldfont);						
	DeleteObject(font);									

	return displayLists;
}

#endif
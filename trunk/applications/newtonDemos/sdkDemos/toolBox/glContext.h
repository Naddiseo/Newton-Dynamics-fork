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

#ifndef __GL_CONTEXT_H__
#define __GL_CONTEXT_H__

class GLContext
{
	public:
	GLContext();
	~GLContext();
	void Init(HWND hWnd);
	void Print (const dVector& color, dFloat x, dFloat y, const char *fmt, ... ) const;

	HWND GetWindow() const { return m_wnd;}
	HDC GetHDC() const { return m_hDC;}

	private:
	GLuint BuildFont();

	HWND m_wnd;
	HDC m_hDC;
	HGLRC m_hRC;
	GLuint m_fonts;
//	GLuint gAerialFontImage;
};

#endif
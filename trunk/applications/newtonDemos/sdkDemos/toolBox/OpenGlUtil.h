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
	


// OpenGlUtil.h: interface for the OpenGlUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLUTIL_H__INCLUDED_)
#define AFX_OPENGLUTIL_H__INCLUDED_

#include <toolbox_stdafx.h>


//#define RECORD_LOG
//#define READ_LOG






struct dMOUSE_POINT
{
	int x;
	int y;
};


#define dRAND_MAX		0x0fffffff

unsigned dRand ();
dMatrix GetModelViewMatrix ();

void HandleResize(int width, int height);
int dGetKeyState(int key);
void GetCursorPos(dMOUSE_POINT& point);
dVector ScreenToWorld (const dVector& screen);
dVector WorldToScreen (const dVector& world);
void ShowMousePicking (const dVector& p0, const dVector& p1); 

void KeyUp(int key);
void KeyDown (int key);



#endif 


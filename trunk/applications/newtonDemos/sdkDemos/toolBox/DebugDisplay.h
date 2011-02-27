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


#ifndef AFX_DEBUG_DISPLAY_H____INCLUDED_
#define AFX_DEBUG_DISPLAY_H____INCLUDED_

#include <toolbox_stdafx.h>
#include <dVector.h>
#include <dMatrix.h>

void RenderContactPoints (NewtonWorld* const world); 
void DebugRenderWorldCollision (const NewtonWorld* world);

#endif

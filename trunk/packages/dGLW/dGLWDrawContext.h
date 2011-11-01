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

#ifndef __dGLWDrawContext_H_
#define __dGLWDrawContext_H_

class dGLW;

class dGLWDrawContext
{


	public:
	dGLWDrawContext(void);
	~dGLWDrawContext(void);

	void SetBrushColor (const dGLWColor& color) const;
	void ClearRectangle (int x0, int y0, int x1, int y1) const;
	void DrawLine (int x0, int y0, int x1, int y1) const;


	private:
	int m_clientWidth;
	int m_clientHeight;

#ifdef _GLW_WIN32
	HDC m_hdc;
#endif
	friend dGLW;
};



#endif
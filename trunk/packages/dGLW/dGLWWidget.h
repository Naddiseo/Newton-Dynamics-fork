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

#ifndef __dGLWWidget_H_
#define __dGLWWidget_H_

class dGLW;


class dGLWWidget
{
	public: 
	enum dFrameType
	{
		m_child,
		m_frame,
	};

	dGLW* GetGLW() const;
	void Create (dFrameType type);
	void AddChild(dGLWWidget* const child);
	void RemoveChild(dGLWWidget* const child);

	protected:
	dGLWWidget(dGLW* const glw, dGLWWidget* const parent, dFrameType type);
	virtual ~dGLWWidget(void);


	dGLW* m_glw;
	dGLWWidget* m_parent;
	dGLW_HANDLE m_nativeHandle;
	dList<dGLWWidget*> m_children;

	friend dGLW;
	dRttiRootClassSupportDeclare(dGLWWidget);
};


#endif
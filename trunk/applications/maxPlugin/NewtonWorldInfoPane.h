/* Copyright (c) <2003-2011> <Julio Jerez, Newton Game Dynamics>
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


#ifndef __NEWTON_WORLD_INFO_PANE_H__
#define __NEWTON_WORLD_INFO_PANE_H__


class NewtonWorldInfoPane
{
	public:
	NewtonWorldInfoPane(void);
	~NewtonWorldInfoPane(void);

	void Init(HWND hWnd);
	void Destroy(HWND hWnd);

	void StartUpdates ();
	void StopUpdates ();
	void Update ();

	

	
	static INT_PTR CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND m_hWnd;
	bool m_updateState;
//	float m_animationFPS;
	ICustEdit* m_gravity[3];
};


#endif
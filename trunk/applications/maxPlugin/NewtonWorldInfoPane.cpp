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

#include "Common.h"
#include "PhysicsPluginDesc.h"
#include "NewtonWorldInfoPane.h"




NewtonWorldInfoPane::NewtonWorldInfoPane(void)
	:m_hWnd (0)
	,m_updateState(false)
{
}

NewtonWorldInfoPane::~NewtonWorldInfoPane(void)
{
}

void NewtonWorldInfoPane::Init(HWND hWnd)
{
	m_hWnd = hWnd;

	HWND gravity_x = GetDlgItem(hWnd, IDC_GRAVITY_X);
	HWND gravity_y = GetDlgItem(hWnd, IDC_GRAVITY_Y);
	HWND gravity_z = GetDlgItem(hWnd, IDC_GRAVITY_Z);

	m_gravity[0] = GetICustEdit(gravity_x);
	m_gravity[1] = GetICustEdit(gravity_y);
	m_gravity[2] = GetICustEdit(gravity_z);

	PhysicsPluginClassDesc* const plugin = (PhysicsPluginClassDesc*) PhysicsPluginClassDesc::GetControlDesc();
	dVector gravity = plugin->m_systemMatrixInv.RotateVector(plugin->m_gravity.Scale (1.0f / float (GetMasterScale(UNITS_METERS))));

	m_gravity[0]->SetText(gravity.m_x);
	m_gravity[1]->SetText(gravity.m_y);
	m_gravity[2]->SetText(gravity.m_z);
}

void NewtonWorldInfoPane::Destroy(HWND hWnd)
{
	ReleaseICustEdit (m_gravity[0]);
	ReleaseICustEdit (m_gravity[1]);
	ReleaseICustEdit (m_gravity[2]);
}



INT_PTR CALLBACK NewtonWorldInfoPane::DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PhysicsPluginObject* const obj = (PhysicsPluginObject *)GetWindowLong (hWnd, GWL_USERDATA);

	switch (msg) 
	{
		case WM_INITDIALOG:
		{
			PhysicsPluginObject* const obj = (PhysicsPluginObject *)lParam;
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)obj);

			obj->NewtonWorldInfoPane::Init(hWnd);
			break;
		}

		case WM_DESTROY:
		{
			obj->NewtonWorldInfoPane::Destroy(hWnd);
			break;
		}

		case WM_ENABLE:
		{
			EnableWindow(obj->m_worldPaneHandle, (BOOL) wParam);
			break;
		}

		case WM_TIMER:
		{
			obj->Update();
			break;
		}


		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_PREVIEW_WORLD:
				{
					if (IsDlgButtonChecked(hWnd, IDC_PREVIEW_WORLD) == BST_CHECKED) {
						obj->StartUpdates();
					} else {
						obj->StopUpdates();
					}
					break;
				}

				case IDC_STEP_WORLD:
				{
					if (!obj->m_updateState) {
						obj->SaveState ();
						obj->Update();
					}
					break;
				}

				case IDC_GRAVITY_X:
				case IDC_GRAVITY_Y:
				case IDC_GRAVITY_Z:
				{
					PhysicsPluginClassDesc* const plugin = (PhysicsPluginClassDesc*) PhysicsPluginClassDesc::GetControlDesc();
					dVector gravity (obj->m_gravity[0]->GetFloat(), obj->m_gravity[1]->GetFloat(), obj->m_gravity[2]->GetFloat(), 0.0f);
					plugin->m_gravity = plugin->m_systemMatrix.RotateVector(gravity.Scale(float (GetMasterScale(UNITS_METERS))));
					break;
				}

			}

			break;
		}

		default:
		return FALSE;
	}
	return TRUE;
}


void NewtonWorldInfoPane::StartUpdates ()
{
	PhysicsPluginObject& me = *(PhysicsPluginObject *)this;
	m_updateState = true;

	int ticksPerFrame = GetTicksPerFrame();
	float timestep = float (ticksPerFrame) / float (4800.0f);

	me.SaveState ();
	SetTimer(me.m_hWnd, IDC_UPDATE_WORLD, int (timestep * 1000.f), NULL);
}

void NewtonWorldInfoPane::StopUpdates ()
{
	PhysicsPluginObject& me = *(PhysicsPluginObject *)this;

	m_updateState = false;
	KillTimer(me.m_hWnd, IDC_UPDATE_WORLD);
}


void NewtonWorldInfoPane::Update()
{
	PhysicsPluginObject& me = *(PhysicsPluginObject *)this;

	int ticks = GetTicksPerFrame();
	Interval range (me.m_ip->GetAnimRange());
	TimeValue time (me.m_ip->GetTime());
	time += ticks;
	if (time > range.End()) {
		time = range.Start() + time % (range.End() - range.Start());
	}
	int ticksPerFrame = GetTicksPerFrame();
	float timestep = float (ticksPerFrame) / float (4800.0f);

	int count = int (dFloor (timestep / (1.0f / 120.0f))) + 1;
	timestep = timestep / float (count);
	for (int i = 0; i < count; i ++) {
		NewtonUpdate(me.m_newton, timestep);
	}
	
	me.SetTransforms (time);

	me.m_ip->SetTime (time);  
}


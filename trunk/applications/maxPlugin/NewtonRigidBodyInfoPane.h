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


#ifndef __NEWTON_RIGIDBODY_INFO_PANE_H__
#define __NEWTON_RIGIDBODY_INFO_PANE_H__


class RidBodyData;

class NewtonRigidBodyInfoPane
{
	public:

	enum CollisionShape
	{
		m_box = 0,
		m_sphere,
		m_cylinder,
		m_convexHull,
		m_collisionTree,
	};
	
	NewtonRigidBodyInfoPane(void);
	~NewtonRigidBodyInfoPane(void);

	void Init(HWND hWnd);
	void Destroy(HWND hWnd);

	void SelectionHasChanged();

	void SelectRigiBodies();
	void AttachSelectedRigiBodies();
	void DetachSelectedRigiBodies();

	bool AttachRigidBodyToNode(INode* const node);
	void RemoveRigidBodyFromNode(INode* const node);
	void CloneRigidBody(INode* const orignal, INode* const clone);
	
	void SetCollisionData (HWND listControl, int shapeId, char* const label) const;

	void SetSelectionMass (dFloat mass);
	void SetTransforms (TimeValue tick) const;

	void SaveState ();
	void AddNodeToWorld (INode* const node);
	void RemoveNodeFromWorld (INode* const node);

	static Class_ID GetRigidBodyID ();
	static INT_PTR CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static void ApplyGravityForce (const NewtonBody* const body, dFloat timestep, int threadIndex);
	

	
	ICustEdit* m_massEdit;
	ISpinnerControl* m_massSpinner;
	CollisionShape m_curCollsionShape;

	
	
};


#endif
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


#ifndef __RIGIDBODID_WORLD_DESC_H__
#define __RIGIDBODID_WORLD_DESC_H__

#include "RigidBodyUIPane.h"

class RigidBodyController;

class RigidBodyData
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

	RigidBodyData()
	{
		memset (this, 0, sizeof (RigidBodyData));
		m_oldControlerID = Class_ID (PRS_CONTROL_CLASS_ID, 0);
	}	

	Point3 m_position;
	Point3 m_basePosition;
	Class_ID m_oldControlerID;

	CollisionShape m_collisionShape;
	int m_hideGizmos;
	dFloat m_mass;
	dVector m_inertia; 
	dVector m_origin; 
	NewtonBody* m_body;
};


class RigidBodyWorldDesc: public ClassDesc2 
{
	public:

	class SavedData
	{
		public:
		RigidBodyData m_bodyData;
		Matrix3 m_matrix;
	};

	RigidBodyWorldDesc ();
	~RigidBodyWorldDesc ();
	int IsPublic();
	void* Create(BOOL loading = FALSE);
	const TCHAR* ClassName();
	SClass_ID SuperClassID();
	Class_ID ClassID();
	const TCHAR* Category();
	const TCHAR* InternalName();
	HINSTANCE HInstance();
	void ResetClassParams (BOOL fileReset); 

	virtual BOOL NeedsToSave();
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);
	

//	void AttachRigiBodyController (INode* const node, bool createBody);
//	void DetachRigiBodyController (INode* const node, bool deleteBody);

	
	RigidBodyController* GetRigidBodyControl(INode* const node) const;
	
	static ClassDesc* GetDescriptor();
//	static void OnPreCloneNode(void* param, NotifyInfo* info);
//	static void OnPostCloneNode(void* param, NotifyInfo* info);

	float m_minFps;
	dVector m_gravity;
	dMatrix m_systemMatrix;
	dMatrix m_systemMatrixInv;
	NewtonWorld* m_newton;
	dTree<SavedData, INode*> m_savedCloneList;
};


class RigidBodyWorld: public UtilityObj, public RigidBodyUIPane
{
	public:
	RigidBodyWorld();
	~RigidBodyWorld();		

	SClass_ID SuperClassID();
	Class_ID ClassID();

	void InitUI(HWND hWnd);
	void DestroyUI(HWND hWnd);


	virtual void DeleteThis (); 

	virtual void BeginEditParams (Interface* ip, IUtil* iu); 
	virtual void EndEditParams (Interface* ip, IUtil* iu); 
	virtual void SelectionSetChanged (Interface *ip, IUtil *iu); 

	static INT_PTR CALLBACK Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	

	void UpdateViewPorts ();






/*
	virtual void SelectionSetChanged (Interface *ip, IUtil *iu); 
	virtual void SetStartupParam (MSTR param); 
	void GetNodeList (dList<INode*>& list);
	static void OnPostLoadScene(void *param, NotifyInfo *info);
	static void OnPreLoadScene(void *param, NotifyInfo *info);
	IUtil* m_iu;
	Interface *m_ip;
	NewtonWorld* m_newton;
	HWND m_worldPaneHandle;
	HWND m_rigidBodyPaneHandle;
	bool m_selectionActive;
	dList<INode*> m_currentSelection;
*/

	void AttachRigiBodyController (INode* const node);
	void DetachRigiBodyController (INode* const node);

	
	bool m_selectionChange;
	HWND m_newtonBodyUI;
	HWND m_newtonWorldUI;

	ICustEdit* m_minFps;
	ICustEdit* m_gravity[3];
	
};

#endif


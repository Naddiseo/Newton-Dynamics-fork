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


#ifndef __NEWTONWORLD_DESC_H__
#define __NEWTONWORLD_DESC_H__

#if 0
#include "NewtonWorldInfoPane.h"
#include "NewtonRigidBodyInfoPane.h"

class PhysicsPluginObject;

class PhysicsPluginClassDesc: public ClassDesc2 
{
	public:
	PhysicsPluginClassDesc ();
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


	static ClassDesc* GetDescriptor();

	PhysicsPluginObject* m_object;
	float m_minFps;
	dVector m_gravity;
	dMatrix m_systemMatrix;
	dMatrix m_systemMatrixInv;
};



class PhysicsPluginObject
	:public UtilityObj
	,public NewtonWorldInfoPane
	,public NewtonRigidBodyInfoPane
{
	public:
	//Constructor/Destructor
	PhysicsPluginObject();
	~PhysicsPluginObject();		


	virtual void  BeginEditParams (Interface *ip, IUtil *iu); 
	virtual void  EndEditParams (Interface *ip, IUtil *iu); 

	virtual void  DeleteThis (); 
	virtual void  SelectionSetChanged (Interface *ip, IUtil *iu); 
	virtual void  SetStartupParam (MSTR param); 

	void GetNodeList (dList<INode*>& list);

	static void OnCloneNode(void *param, NotifyInfo *info);
	static void OnPostLoadScene(void *param, NotifyInfo *info);
	static void OnPreLoadScene(void *param, NotifyInfo *info);

	SClass_ID SuperClassID();
	Class_ID ClassID();


	IUtil* m_iu;
	Interface *m_ip;
	NewtonWorld* m_newton;
	HWND m_worldPaneHandle;
	HWND m_rigidBodyPaneHandle;
	bool m_selectionActive;
	dList<INode*> m_currentSelection;
};
#endif

#endif


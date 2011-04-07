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

#define PHYSICS_PLUGIN_CLASS_ID	Class_ID(0x6185a57, 0x3a1f2f69)


PhysicsPluginClassDesc::PhysicsPluginClassDesc ()
	:ClassDesc2()
	,m_minFps (120.0f)
	,m_gravity(0.0f, -9.8f, 0.0f, 0.0f)
	,m_systemMatrix (dVector (0.0f, 0.0f, 1.0f, 0.0f), dVector (1.0f, 0.0f, 0.0f, 0.0f), dVector (0.0f, 1.0f, 0.0f, 0.0f), dVector (0.0f, 0.0f, 0.0f, 1.0f))
	,m_systemMatrixInv (m_systemMatrix.Inverse())
{
}


int PhysicsPluginClassDesc::IsPublic() 
{ 
	return TRUE; 
}

void* PhysicsPluginClassDesc::Create(BOOL loading) 
{ 
	m_object = new PhysicsPluginObject;
	return m_object;
}


const TCHAR* PhysicsPluginClassDesc::ClassName() 
{ 
	return GetString(IDS_CLASS_NAME); 
}

SClass_ID PhysicsPluginClassDesc::SuperClassID() 
{ 
	return UTILITY_CLASS_ID; 
}

Class_ID PhysicsPluginClassDesc::ClassID() 
{ 
	return PHYSICS_PLUGIN_CLASS_ID; 
}

const TCHAR* PhysicsPluginClassDesc::Category() 
{ 
	return GetString(IDS_CATEGORY); 
}

const TCHAR* PhysicsPluginClassDesc::InternalName() 
{ 
	return _T("PhysicsDesc"); 
}	

HINSTANCE PhysicsPluginClassDesc::HInstance() 
{ 
	return hInstance; 
}				

void PhysicsPluginClassDesc::ResetClassParams (BOOL fileReset)
{
	ClassDesc2::ResetClassParams (fileReset);
}

ClassDesc* PhysicsPluginClassDesc::GetControlDesc()
{ 
	static PhysicsPluginClassDesc desc;
	return &desc; 
}

BOOL PhysicsPluginClassDesc::NeedsToSave() 
{
	return TRUE; 
}

IOResult PhysicsPluginClassDesc::Load(ILoad* iload)
{
	IOResult ret = ClassDesc2::Load(iload);

	ULONG retVal;

	iload->OpenChunk();
	iload->Read(&m_gravity, sizeof (m_gravity), &retVal);
	iload->Read(&m_minFps, sizeof (m_minFps), &retVal);
	iload->CloseChunk();

	return ret;

}

IOResult PhysicsPluginClassDesc::Save(ISave* isave)
{
	IOResult ret = ClassDesc2::Save(isave);

	ULONG retVal;

	isave->BeginChunk(USHORT (ClassID().PartB()));
	isave->Write(&m_gravity, sizeof (m_gravity), &retVal);
	isave->Write(&m_minFps, sizeof (m_minFps), &retVal);
	isave->EndChunk();

	return ret;
}



PhysicsPluginObject::PhysicsPluginObject()
	:UtilityObj()
	,NewtonWorldInfoPane()
	,NewtonRigidBodyInfoPane()
	,m_iu (NULL)
	,m_ip (NULL)
	,m_newton(NULL)
	,m_worldPaneHandle(0)
	,m_rigidBodyPaneHandle(0)
	,m_selectionActive(true)
	,m_currentSelection()
{
	_ASSERTE (!m_newton);
	m_newton = NewtonCreate();

	// Register the callback
	RegisterNotification(RemoveAllRigidBodies, this, NOTIFY_FILE_PRE_OPEN);
	RegisterNotification(AddAllRidigBodies, this, NOTIFY_FILE_POST_OPEN);
}

PhysicsPluginObject::~PhysicsPluginObject()
{
}


void PhysicsPluginObject::DeleteThis ()
{
	_ASSERTE (m_newton);
	NewtonDestroy (m_newton);

	// When done, unregister the callback
	UnRegisterNotification(RemoveAllRigidBodies, this, NOTIFY_FILE_PRE_OPEN);
	UnRegisterNotification(AddAllRidigBodies, this, NOTIFY_FILE_POST_OPEN);
}


SClass_ID PhysicsPluginObject::SuperClassID()
{
	return PhysicsPluginClassDesc::GetControlDesc()->SuperClassID();
}

Class_ID PhysicsPluginObject::ClassID()
{
	return PhysicsPluginClassDesc::GetControlDesc()->ClassID();
}


void  PhysicsPluginObject::BeginEditParams (Interface *ip, IUtil *iu)
{
	m_iu = iu;
	m_ip = ip;
	m_worldPaneHandle = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_NEWTON_WORLD_PANE), NewtonWorldInfoPane::DialogProc, _T("Newton Alchemedia"), LPARAM (this));
	m_rigidBodyPaneHandle = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_NEWTON_BODY_PANE), NewtonRigidBodyInfoPane::DialogProc, _T("RigidBodies properties"), LPARAM (this));

	SelectionSetChanged (ip, iu);
}

void  PhysicsPluginObject::EndEditParams (Interface *ip, IUtil *iu)
{
	m_iu = NULL;
	m_ip = NULL;

	ip->DeleteRollupPage(m_worldPaneHandle);
	ip->DeleteRollupPage(m_rigidBodyPaneHandle);

	m_worldPaneHandle = NULL;
	m_rigidBodyPaneHandle = NULL;
}


void PhysicsPluginObject::SelectionSetChanged (Interface *ip, IUtil *iu)
{
	if (m_selectionActive) {
		_ASSERTE (m_ip == ip);
		m_currentSelection.RemoveAll();
		int selectionCount = m_ip->GetSelNodeCount();
		for (int i = 0; i < selectionCount; i ++) {
			INode* const node = m_ip->GetSelNode(i);
			m_currentSelection.Append(node);
		}

		NewtonRigidBodyInfoPane::SelectionHasChanged();
	}
}

void PhysicsPluginObject::GetNodeList(dList<INode*>& list)
{
	list.RemoveAll();
	INode* stack[4096];
	
	stack[0] = m_ip->GetRootNode();
	int stackIndex = 1;
	while (stackIndex) {
		stackIndex --;
		INode* const node = stack[stackIndex];
		list.Append(node);
		for (int i = 0; i < node->NumberOfChildren(); i ++) {
			stack[stackIndex] = node->GetChildNode(i);
			stackIndex ++;
			_ASSERTE (stackIndex * sizeof (INode*) < sizeof (stack));	
		}
	}
}


void PhysicsPluginObject::SetStartupParam (MSTR param)
{
	_ASSERTE (0);
}




//Declare the callback function  
void PhysicsPluginObject::RemoveAllRigidBodies(void* param, NotifyInfo* info)
{
	// Get the nodes being deleted
	PhysicsPluginObject* const me = (PhysicsPluginObject*)param;

	dList<INode*> list;
	me->GetNodeList (list);
	for (dList<INode*>::dListNode* ptr = list.GetFirst(); ptr; ptr = ptr->GetNext()) {
		INode* node = ptr->GetInfo();
		me->RemoveRigidBodyFromNode(node);
	}
}

void PhysicsPluginObject::AddAllRidigBodies(void* param, NotifyInfo* info)
{
	// Get the nodes being deleted
	PhysicsPluginObject* const me = (PhysicsPluginObject*)param;

	dList<INode*> list;
	me->GetNodeList (list);
	for (dList<INode*>::dListNode* ptr = list.GetFirst(); ptr; ptr = ptr->GetNext()) {
		INode* node = ptr->GetInfo();
		me->AddNodeToWorld(node);
	}
}






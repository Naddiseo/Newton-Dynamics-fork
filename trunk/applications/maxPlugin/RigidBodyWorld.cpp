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
#include "RigidBodyWorld.h"
#include "RigidBodyPositionController.h"
#include "RigidBodyRotationController.h"


#define RIGIDBOGY_WORLD_CLASS_ID Class_ID(0x6185a57, 0x3a1f2f69)



RigidBodyWorldDesc::RigidBodyWorldDesc ()
	:ClassDesc2()
//	,m_minFps (120.0f)
	,m_gravity(0.0f, -9.8f, 0.0f, 0.0f)
	,m_systemMatrix (dVector (0.0f, 0.0f, 1.0f, 0.0f), dVector (1.0f, 0.0f, 0.0f, 0.0f), dVector (0.0f, 1.0f, 0.0f, 0.0f), dVector (0.0f, 0.0f, 0.0f, 1.0f))
	,m_systemMatrixInv (m_systemMatrix.Inverse())
{
	m_newton = NewtonCreate();


	RegisterNotification(OnPreCloneNode, this, NOTIFY_PRE_NODES_CLONED);
	RegisterNotification(OnPostCloneNode, this, NOTIFY_POST_NODES_CLONED);
}

RigidBodyWorldDesc::~RigidBodyWorldDesc ()
{
	_ASSERTE (m_newton);
	NewtonDestroy (m_newton);
}

int RigidBodyWorldDesc::IsPublic() 
{ 
	return TRUE; 
}

void* RigidBodyWorldDesc::Create(BOOL loading) 
{ 
	return new RigidBodyWorld;
}


const TCHAR* RigidBodyWorldDesc::ClassName() 
{ 
//	return GetString(IDS_CLASS_NAME); 
	return _T("Newton"); 
}

SClass_ID RigidBodyWorldDesc::SuperClassID() 
{ 
	return UTILITY_CLASS_ID; 
}

Class_ID RigidBodyWorldDesc::ClassID() 
{ 
	return RIGIDBOGY_WORLD_CLASS_ID; 
}

const TCHAR* RigidBodyWorldDesc::Category() 
{ 
	return GetString(IDS_CATEGORY); 
}

const TCHAR* RigidBodyWorldDesc::InternalName() 
{ 
	return _T("NewtonPlugin"); 
}	

HINSTANCE RigidBodyWorldDesc::HInstance() 
{ 
	_ASSERTE (0);
	return hInstance; 
}				

void RigidBodyWorldDesc::ResetClassParams (BOOL fileReset)
{
	_ASSERTE (0);
	ClassDesc2::ResetClassParams (fileReset);
}

ClassDesc* RigidBodyWorldDesc::GetDescriptor()
{ 
	static RigidBodyWorldDesc desc;
	return &desc; 
}

BOOL RigidBodyWorldDesc::NeedsToSave() 
{
	_ASSERTE (0);
	return TRUE; 
}

IOResult RigidBodyWorldDesc::Load(ILoad* iload)
{
	IOResult ret = ClassDesc2::Load(iload);

	_ASSERTE (0);
/*
	ULONG retVal;
	iload->OpenChunk();
	iload->Read(&m_gravity, sizeof (m_gravity), &retVal);
	iload->Read(&m_minFps, sizeof (m_minFps), &retVal);
	iload->CloseChunk();
*/
	return ret;
}

IOResult RigidBodyWorldDesc::Save(ISave* isave)
{
	IOResult ret = ClassDesc2::Save(isave);

	_ASSERTE (0);
/*
	ULONG retVal;
	isave->BeginChunk(USHORT (ClassID().PartB()));
	isave->Write(&m_gravity, sizeof (m_gravity), &retVal);
	isave->Write(&m_minFps, sizeof (m_minFps), &retVal);
	isave->EndChunk();
*/
	return ret;
}


RigidBodyPositionController* RigidBodyWorldDesc::GetRigidBodyControl(INode* const node) const
{
	Control* const control = node->GetTMController();
	if (control) {
		RigidBodyPositionControllerDesc& rigidBodyPositionControllerDesc = *(RigidBodyPositionControllerDesc*)RigidBodyPositionControllerDesc::GetDescriptor();
		RigidBodyPositionController* const positController = (RigidBodyPositionController*) control->GetPositionController();
		return (positController->ClassID() == rigidBodyPositionControllerDesc.ClassID()) ? positController : NULL;
	}

	return NULL;
}




void RigidBodyWorldDesc::AttachRigiBodyController (INode* const node, bool createBody)
{
	Control* const control = node->GetTMController();

	RigidBodyPositionControllerDesc& rigidBodyPositionControllerDesc = *(RigidBodyPositionControllerDesc*)RigidBodyPositionControllerDesc::GetDescriptor();
	RigidBodyRotationControllerDesc& rigidBodyRotationControllerDesc = *(RigidBodyRotationControllerDesc*)RigidBodyRotationControllerDesc::GetDescriptor();

	Control* const positController = control->GetPositionController();
	Control* const rotationController = control->GetRotationController();
	_ASSERTE (positController);
	_ASSERTE (rotationController);

	if (positController->ClassID() != rigidBodyPositionControllerDesc.ClassID()) {
		Matrix3 matrix (node->GetNodeTM (GetCOREInterface()->GetTime()));		

		RigidBodyPositionController* const rigidBodyPositionController = (RigidBodyPositionController*) rigidBodyPositionControllerDesc.Create(positController->ClassID());
		RigidBodyRotationController* const rigidBodyRotationController = (RigidBodyRotationController*) rigidBodyRotationControllerDesc.Create(rotationController->ClassID());

		control->SetPositionController(rigidBodyPositionController);
		control->SetRotationController(rigidBodyRotationController);

		node->SetNodeTM(GetCOREInterface()->GetTime(), matrix);

		if (createBody) {
			rigidBodyPositionController->AddRigidBody(node);
		}
	}
}


void RigidBodyWorldDesc::DetachRigiBodyController (INode* const node, bool deleteBody)
{
	Control* const control = node->GetTMController();

	RigidBodyPositionControllerDesc& rigidBodyPositionControllerDesc = *(RigidBodyPositionControllerDesc*)RigidBodyPositionControllerDesc::GetDescriptor();
	RigidBodyRotationControllerDesc& rigidBodyRotationControllerDesc = *(RigidBodyRotationControllerDesc*)RigidBodyRotationControllerDesc::GetDescriptor();

	RigidBodyPositionController* const positController = (RigidBodyPositionController*) control->GetPositionController();
	RigidBodyRotationController* const rotationController = (RigidBodyRotationController*) control->GetRotationController();
	_ASSERTE (positController);
	_ASSERTE (rotationController);
	if (positController->ClassID() == rigidBodyPositionControllerDesc.ClassID()) {
		if (deleteBody) {
			positController->RemoveRigidBody(node);
		}

		Matrix3 matrix (node->GetNodeTM (GetCOREInterface()->GetTime()));		

		Control* const regularPositionController = (Control*) CreateInstance (CTRL_POSITION_CLASS_ID, positController->m_oldControlerID);
		Control* const regularRotationController = (Control*) CreateInstance (CTRL_ROTATION_CLASS_ID, rotationController->m_oldControlerID);

		control->SetPositionController(regularPositionController);
		control->SetRotationController(regularRotationController);

		node->SetNodeTM(GetCOREInterface()->GetTime(), matrix);
	}
}

void RigidBodyWorldDesc::OnPreCloneNode(void* param, NotifyInfo* info)
{
	RigidBodyWorldDesc* const me = (RigidBodyWorldDesc*) param;
	const INodeTab& origNodes = *(INodeTab*) info->callParam;

	RigidBodyPositionControllerDesc& rigidBodyPositionControllerDesc = *(RigidBodyPositionControllerDesc*)RigidBodyPositionControllerDesc::GetDescriptor();
	RigidBodyRotationControllerDesc& rigidBodyRotationControllerDesc = *(RigidBodyRotationControllerDesc*)RigidBodyRotationControllerDesc::GetDescriptor();

	me->m_savedCloneList.RemoveAll();
	for (int i = 0; i < origNodes.Count(); i ++) {
		INode* const node = origNodes[i];

		Control* const control = node->GetTMController();
		if (control) {
			RigidBodyPositionController* const positController = (RigidBodyPositionController*) control->GetPositionController();
			RigidBodyRotationController* const rotationController = (RigidBodyRotationController*) control->GetRotationController();
			if (positController && rotationController) {
				if (positController->ClassID() == rigidBodyPositionControllerDesc.ClassID()) {
					SavedData data;
					data.m_bodyData = *positController;

					data.m_matrix = node->GetNodeTM(GetCOREInterface()->GetTime());
					me->m_savedCloneList.Insert(data, node);
					me->DetachRigiBodyController (node, false);
				}
			}
		}
	}
}

void RigidBodyWorldDesc::OnPostCloneNode(void* param, NotifyInfo* info)
{
	RigidBodyWorldDesc* const me = (RigidBodyWorldDesc*) param;

	struct CloneData
	{ 
		INodeTab* origNodes; 
		INodeTab* clonedNodes; 
		CloneType cloneType;
	} ;

	CloneData* const data = (CloneData*)info->callParam;
//	me->StopUpdates();
	
	const INodeTab& origNodes = *data->origNodes; 
	const INodeTab& clonedNodes = *data->clonedNodes; 
	_ASSERTE (origNodes.Count() == clonedNodes.Count());

	TimeValue t (GetCOREInterface()->GetTime());
	for (int i = 0; i < origNodes.Count(); i ++) {
		INode* const node = origNodes[i];
		dTree<SavedData, INode*>::dTreeNode* const nodeData = me->m_savedCloneList.Find(node);
		if (nodeData) {

			me->AttachRigiBodyController (node, false);

			Control* const control = node->GetTMController();
			_ASSERTE (control);

			RigidBodyPositionController* const positController = (RigidBodyPositionController*) control->GetPositionController();
			_ASSERTE (positController);

			RigidBodyData* const data = positController;
			memcpy (data, &nodeData->GetInfo().m_bodyData, sizeof (RigidBodyData));
			node->SetNodeTM(t, nodeData->GetInfo().m_matrix);
		}
	}


	RigidBodyPositionControllerDesc& rigidBodyPositionControllerDesc = *(RigidBodyPositionControllerDesc*)RigidBodyPositionControllerDesc::GetDescriptor();
	RigidBodyRotationControllerDesc& rigidBodyRotationControllerDesc = *(RigidBodyRotationControllerDesc*)RigidBodyRotationControllerDesc::GetDescriptor();
	for (int i = 0; i < origNodes.Count(); i ++) {
		INode* const origNode = origNodes[i];
		Control* const origControl = origNode->GetTMController();
		if (origControl) {

			RigidBodyPositionController* const origPositController = (RigidBodyPositionController*) origControl->GetPositionController();
			if (origPositController) {
				if (origPositController->ClassID() == rigidBodyPositionControllerDesc.ClassID()) {
					INode* const cloneNode = clonedNodes[i];

					me->AttachRigiBodyController (cloneNode, false);

					Control* const cloneControl = cloneNode->GetTMController();
					RigidBodyPositionController* const clonePositController = (RigidBodyPositionController*) cloneControl->GetPositionController();

					RigidBodyData* const orgData = origPositController;
					RigidBodyData* const cloneData = clonePositController;
					memcpy (cloneData, orgData, sizeof (RigidBodyData));

					NewtonCollision* collision = NewtonBodyGetCollision(orgData->m_body);
					dMatrix matrix;
					NewtonBodyGetMatrix(orgData->m_body, &matrix[0][0]);
					cloneData->m_body = NewtonCreateBody (me->m_newton, collision, &matrix[0][0]);

					NewtonBodySetUserData(cloneData->m_body, cloneNode);

					float mass;
					float Ixx;
					float Iyy;
					float Izz;
					NewtonBodyGetMassMatrix(orgData->m_body, &mass, &Ixx, &Iyy, &Izz);
					NewtonBodySetMassMatrix(cloneData->m_body, mass, Ixx, Iyy, Izz);

					NewtonBodySetMaterialGroupID(cloneData->m_body, NewtonBodyGetMaterialGroupID(orgData->m_body));
					NewtonBodySetForceAndTorqueCallback(cloneData->m_body, NewtonBodyGetForceAndTorqueCallback(orgData->m_body));

					dVector com;
					NewtonBodyGetCentreOfMass(orgData->m_body, &com[0]);
					NewtonBodySetCentreOfMass(cloneData->m_body, &com[0]);
			
					dVector veloc;
					NewtonBodyGetVelocity(orgData->m_body, &veloc[0]);
					NewtonBodySetVelocity(cloneData->m_body, &veloc[0]);

					dVector omega;
					NewtonBodyGetOmega(orgData->m_body, &omega[0]);
					NewtonBodySetOmega(cloneData->m_body, &omega[0]);
				}
			}
		}
	}

	me->m_savedCloneList.RemoveAll();
}






RigidBodyWorld::RigidBodyWorld()
	:UtilityObj()
//	,NewtonWorldInfoPane()
//	,NewtonRigidBodyInfoPane()
//	,m_iu (NULL)
//	,m_ip (NULL)
//	,m_newton(NULL)
//	,m_worldPaneHandle(0)
//	,m_rigidBodyPaneHandle(0)
//	,m_selectionActive(true)
//	,m_currentSelection()
{
//	_ASSERTE (!m_newton);
//	m_newton = NewtonCreate();

	// Register the callback
//	RegisterNotification(OnCloneNode, this, NOTIFY_POST_NODES_CLONED);
//	RegisterNotification(OnPreLoadScene, this, NOTIFY_FILE_PRE_OPEN);
//	RegisterNotification(OnPostLoadScene, this, NOTIFY_FILE_POST_OPEN);
}

RigidBodyWorld::~RigidBodyWorld()
{
}


void RigidBodyWorld::DeleteThis ()
{
//	_ASSERTE (m_newton);
//	NewtonDestroy (m_newton);

	// When done, unregister the callback
//	UnRegisterNotification(OnCloneNode, this, NOTIFY_POST_NODES_CLONED);
//	UnRegisterNotification(OnPreLoadScene, this, NOTIFY_FILE_PRE_OPEN);
//	UnRegisterNotification(OnPostLoadScene, this, NOTIFY_FILE_POST_OPEN);
}



void RigidBodyWorld::BeginEditParams (Interface *ip, IUtil *iu)
{
	_ASSERTE (ip == GetCOREInterface());
	m_newtonWorldUI = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_NEWTON_WORLD_PANE), NetwonWorldUIProc, _T("Newton World"), LPARAM (this));
	m_newtonBodyUI = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_NEWTON_BODY_PANE), NetwonBodyUIProc, _T("RigidBodies properties"), LPARAM (this));
//	SelectionSetChanged (ip, iu);
}

void RigidBodyWorld::EndEditParams (Interface *ip, IUtil *iu)
{
//	m_iu = NULL;
	_ASSERTE (ip == GetCOREInterface());
	ip->DeleteRollupPage(m_newtonWorldUI);
	ip->DeleteRollupPage(m_newtonBodyUI);
}



INT_PTR CALLBACK RigidBodyWorld::NetwonBodyUIProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RigidBodyWorld* const world = (RigidBodyWorld *)GetWindowLong (hWnd, GWL_USERDATA);
	RigidBodyWorldDesc* const desc = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();

	switch (msg) 
	{
		case WM_INITDIALOG:
		{
			RigidBodyWorld* const world = (RigidBodyWorld *)lParam;
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)world);
			//			obj->NewtonWorldInfoPane::Init(hWnd);
			break;
		}
		/*
		case WM_DESTROY:
		{
		//			obj->NewtonWorldInfoPane::Destroy(hWnd);
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
		*/

		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_HIDE_GIZMO:
				{
					Interface* const ip = GetCOREInterface();
					int selectionCount = ip->GetSelNodeCount();
					for (int i = 0; i < selectionCount; i ++) {
						INode* const node = ip->GetSelNode(i);
						RigidBodyPositionController* const bodyInfo = desc->GetRigidBodyControl(node);
						if (bodyInfo) {
							bodyInfo->m_hideGizmos = (IsDlgButtonChecked(hWnd, IDC_HIDE_GIZMO) == BST_CHECKED) ? TRUE : FALSE;
						}
					}

					world->UpdateViewPorts();
					break;
				}
/*
				case IDC_DELETE_RIGIDBODY:
				{
					world->RemoveRigidBodies ();
					break;
				}

				
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
				PhysicsPluginClassDesc* const plugin = (PhysicsPluginClassDesc*) PhysicsPluginClassDesc::GetDescriptor();
				dVector gravity (obj->m_gravity[0]->GetFloat(), obj->m_gravity[1]->GetFloat(), obj->m_gravity[2]->GetFloat(), 0.0f);
				plugin->m_gravity = plugin->m_systemMatrix.RotateVector(gravity.Scale(float (GetMasterScale(UNITS_METERS))));
				break;
				}
				*/
			}

			break;
		}

		default:
		return FALSE;
	}
	return TRUE;
}


INT_PTR CALLBACK RigidBodyWorld::NetwonWorldUIProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RigidBodyWorld* const world = (RigidBodyWorld *)GetWindowLong (hWnd, GWL_USERDATA);
	RigidBodyWorldDesc* const desc = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();

	switch (msg) 
	{
		case WM_INITDIALOG:
		{
			RigidBodyWorld* const world = (RigidBodyWorld *)lParam;
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)world);
			break;
		}
/*
		case WM_DESTROY:
		{
//			obj->NewtonWorldInfoPane::Destroy(hWnd);
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
*/

		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_MAKE_RIGIDBODY:
				{
					world->MakeRigidBodies ();
					break;
				}

				case IDC_DELETE_RIGIDBODY:
				{
					world->RemoveRigidBodies ();
					break;
				}

				case IDC_SHOW_GIZMOS:
				{
					for (NewtonBody* body = NewtonWorldGetFirstBody(desc->m_newton); body; body = NewtonWorldGetNextBody(desc->m_newton, body)) {
						INode* const node = (INode*)NewtonBodyGetUserData(body);
						RigidBodyPositionController* const bodyInfo = (RigidBodyPositionController*)desc->GetRigidBodyControl(node);
						_ASSERTE (bodyInfo);
						bodyInfo->m_hideGizmos = FALSE;
					}
					world->UpdateViewPorts();
					break;
				}

				case IDC_HIDE_GIZMOS:
				{
					for (NewtonBody* body = NewtonWorldGetFirstBody(desc->m_newton); body; body = NewtonWorldGetNextBody(desc->m_newton, body)) {
						INode* const node = (INode*)NewtonBodyGetUserData(body);
						RigidBodyPositionController* const bodyInfo = (RigidBodyPositionController*)desc->GetRigidBodyControl(node);
						_ASSERTE (bodyInfo);
						bodyInfo->m_hideGizmos = TRUE;
					}
					world->UpdateViewPorts();
					break;
				}

				case IDC_SELECT_ALL:
				{
					Interface* const ip = GetCOREInterface();
					ip->ClearNodeSelection(FALSE);
					for (NewtonBody* body = NewtonWorldGetFirstBody(desc->m_newton); body; body = NewtonWorldGetNextBody(desc->m_newton, body)) {
						INode* const node = (INode*)NewtonBodyGetUserData(body);
						ip->SelectNode(node, 0); 
					}
					world->UpdateViewPorts();
					break;
				}
				

				case IDC_REMOVE_ALL:
				{
					Interface* const ip = GetCOREInterface();
					ip->ClearNodeSelection(FALSE);
					for (NewtonBody* body = NewtonWorldGetFirstBody(desc->m_newton); body; ) {
						INode* const node = (INode*)NewtonBodyGetUserData(body);
						body = NewtonWorldGetNextBody(desc->m_newton, body);
						desc->DetachRigiBodyController (node, true);
					}
					world->UpdateViewPorts();
					break;
				}


/*
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
					PhysicsPluginClassDesc* const plugin = (PhysicsPluginClassDesc*) PhysicsPluginClassDesc::GetDescriptor();
					dVector gravity (obj->m_gravity[0]->GetFloat(), obj->m_gravity[1]->GetFloat(), obj->m_gravity[2]->GetFloat(), 0.0f);
					plugin->m_gravity = plugin->m_systemMatrix.RotateVector(gravity.Scale(float (GetMasterScale(UNITS_METERS))));
					break;
				}
*/
			}

			break;
		}

		default:
		return FALSE;
	}
	return TRUE;
}


void RigidBodyWorld::MakeRigidBodies ()
{
	RigidBodyWorldDesc* const plugin = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();
	Interface* const ip = GetCOREInterface();
	int selectionCount = ip->GetSelNodeCount();
	for (int i = 0; i < selectionCount; i ++) {
		INode* const node = ip->GetSelNode(i);
		Object* const obj = node->GetObjOrWSMRef();
		if (obj) {
			switch(obj->SuperClassID()) 
			{
				case GEOMOBJECT_CLASS_ID: 
				{
					plugin->AttachRigiBodyController (node, true);
					break;
				}

				default:;
				{
					_ASSERTE (0);
				}
			}
		}
	}

	UpdateViewPorts();
}


void RigidBodyWorld::RemoveRigidBodies ()
{
	RigidBodyWorldDesc* const plugin = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();
	Interface* const ip = GetCOREInterface();
	int selectionCount = ip->GetSelNodeCount();
	for (int i = 0; i < selectionCount; i ++) {
		INode* const node = ip->GetSelNode(i);
		Object* const obj = node->GetObjOrWSMRef();
		if (obj) {
			switch(obj->SuperClassID()) 
			{
				case GEOMOBJECT_CLASS_ID: 
				{
					plugin->DetachRigiBodyController (node, true);
					break;
				}

				default:;
				{
					_ASSERTE (0);
				}
			}
		}
	}
	UpdateViewPorts ();
}


void RigidBodyWorld::UpdateViewPorts ()
{
	//GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	GetCOREInterface()->ForceCompleteRedraw(); 
}

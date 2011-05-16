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
#include "RigidBodyController.h"


#define RIGIDBOGY_WORLD_CLASS_ID Class_ID(0x6185a57, 0x3a1f2f69)



RigidBodyWorldDesc::RigidBodyWorldDesc ()
	:ClassDesc2()
	,m_minFps (120.0f)
	,m_gravity(0.0f, -9.8f, 0.0f, 0.0f)
	,m_systemMatrix (dVector (0.0f, 0.0f, 1.0f, 0.0f), dVector (1.0f, 0.0f, 0.0f, 0.0f), dVector (0.0f, 1.0f, 0.0f, 0.0f), dVector (0.0f, 0.0f, 0.0f, 1.0f))
	,m_systemMatrixInv (m_systemMatrix.Inverse())
{
//	RegisterNotification(OnPreCloneNode, this, NOTIFY_PRE_NODES_CLONED);
//	RegisterNotification(OnPostCloneNode, this, NOTIFY_POST_NODES_CLONED);

//	RegisterNotification(OnPreLoadScene, this, NOTIFY_FILE_PRE_OPEN);
//	RegisterNotification(OnPostLoadScene, this, NOTIFY_FILE_POST_OPEN);

	m_newton = NewtonCreate();
}

RigidBodyWorldDesc::~RigidBodyWorldDesc ()
{
	_ASSERTE (m_newton);
	NewtonDestroy (m_newton);

//	UnRegisterNotification(OnPreCloneNode, this, NOTIFY_PRE_NODES_CLONED);
//	UnRegisterNotification(OnPostCloneNode, this, NOTIFY_POST_NODES_CLONED);

//	UnRegisterNotification(OnPreLoadScene, this, NOTIFY_FILE_PRE_OPEN);
//	UnRegisterNotification(OnPostLoadScene, this, NOTIFY_FILE_POST_OPEN);
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
	return TRUE; 
}

IOResult RigidBodyWorldDesc::Load(ILoad* iload)
{
	IOResult ret = ClassDesc2::Load(iload);

	ULONG retVal;
	iload->OpenChunk();
	iload->Read(&m_gravity, sizeof (m_gravity), &retVal);
	iload->Read(&m_minFps, sizeof (m_minFps), &retVal);
	iload->CloseChunk();

	return ret;
}

IOResult RigidBodyWorldDesc::Save(ISave* isave)
{
	IOResult ret = ClassDesc2::Save(isave);

	ULONG retVal;
	isave->BeginChunk(USHORT (ClassID().PartB()));
	isave->Write(&m_gravity, sizeof (m_gravity), &retVal);
	isave->Write(&m_minFps, sizeof (m_minFps), &retVal);
	isave->EndChunk();

	return ret;
}


RigidBodyController* RigidBodyWorldDesc::GetRigidBodyControl(INode* const node) const
{
	_ASSERTE (0);
	return NULL;
/*
	Control* const control = node->GetTMController();
	if (control) {
		RigidBodyControllerDesc& RigidBodyControllerDesc = *(RigidBodyControllerDesc*)RigidBodyControllerDesc::GetDescriptor();
		RigidBodyController* const positController = (RigidBodyController*) control->GetPositionController();
		if (positController && (positController->ClassID() == RigidBodyControllerDesc.ClassID())) {
			return positController;
		}
	}
*/
	return NULL;
}


/*
void RigidBodyWorldDesc::OnPreCloneNode(void* param, NotifyInfo* info)
{
	RigidBodyWorldDesc* const me = (RigidBodyWorldDesc*) param;
	const INodeTab& origNodes = *(INodeTab*) info->callParam;

	RigidBodyControllerDesc& RigidBodyControllerDesc = *(RigidBodyControllerDesc*)RigidBodyControllerDesc::GetDescriptor();
	RigidBodyRotationControllerDesc& rigidBodyRotationControllerDesc = *(RigidBodyRotationControllerDesc*)RigidBodyRotationControllerDesc::GetDescriptor();

	me->m_savedCloneList.RemoveAll();
	TimeValue t (GetCOREInterface()->GetTime());
	for (int i = 0; i < origNodes.Count(); i ++) {
		INode* const node = origNodes[i];

		Control* const control = node->GetTMController();
		if (control) {
			RigidBodyController* const positController = (RigidBodyController*) control->GetPositionController();
			RigidBodyRotationController* const rotationController = (RigidBodyRotationController*) control->GetRotationController();
			if (positController && rotationController) {
				if (positController->ClassID() == RigidBodyControllerDesc.ClassID()) {
					SavedData data;
					data.m_bodyData = *positController;

					data.m_matrix = node->GetNodeTM(t);
					me->m_savedCloneList.Insert(data, node);

					Control* const regularPositionController = (Control*) CreateInstance (CTRL_POSITION_CLASS_ID, positController->m_oldControlerID);
					Control* const regularRotationController = (Control*) CreateInstance (CTRL_ROTATION_CLASS_ID, rotationController->m_oldControlerID);

					control->SetPositionController(regularPositionController);
					control->SetRotationController(regularRotationController);

					node->SetNodeTM(t, data.m_matrix);
				}
			}
		}
	}
}

void RigidBodyWorldDesc::OnPostCloneNode(void* param, NotifyInfo* info)
{

	struct CloneData
	{ 
		INodeTab* origNodes; 
		INodeTab* clonedNodes; 
		CloneType cloneType;
	} ;

	RigidBodyWorldDesc* const me = (RigidBodyWorldDesc*) param;
	CloneData* const data = (CloneData*)info->callParam;
//	me->StopUpdates();
	
	const INodeTab& origNodes = *data->origNodes; 
	const INodeTab& clonedNodes = *data->clonedNodes; 
	_ASSERTE (origNodes.Count() == clonedNodes.Count());

	RigidBodyControllerDesc& RigidBodyControllerDesc = *(RigidBodyControllerDesc*)RigidBodyControllerDesc::GetDescriptor();
	RigidBodyRotationControllerDesc& rigidBodyRotationControllerDesc = *(RigidBodyRotationControllerDesc*)RigidBodyRotationControllerDesc::GetDescriptor();

	TimeValue t (GetCOREInterface()->GetTime());
	for (int i = 0; i < origNodes.Count(); i ++) {
		INode* const node = origNodes[i];
		dTree<SavedData, INode*>::dTreeNode* const nodeData = me->m_savedCloneList.Find(node);
		if (nodeData) {
			SavedData& data = nodeData->GetInfo();

			Control* const control = node->GetTMController();
			_ASSERTE (control);

			Control* const positController = control->GetPositionController();
			Control* const rotationController = control->GetRotationController();

			_ASSERTE (positController);
			_ASSERTE (rotationController);
			_ASSERTE (positController->ClassID() != RigidBodyControllerDesc.ClassID()) ;

			RigidBodyController* const RigidBodyController = (RigidBodyController*) RigidBodyControllerDesc.Create(positController->ClassID());
			RigidBodyRotationController* const rigidBodyRotationController = (RigidBodyRotationController*) rigidBodyRotationControllerDesc.Create(rotationController->ClassID());

			control->SetPositionController(RigidBodyController);
			control->SetRotationController(rigidBodyRotationController);

			RigidBodyData* const nodeData = RigidBodyController;
			memcpy (nodeData, &data.m_bodyData, sizeof (RigidBodyData));

			node->SetNodeTM(t, data.m_matrix);
		}
	}


	for (int i = 0; i < origNodes.Count(); i ++) {
		INode* const origNode = origNodes[i];

		RigidBodyData* const origData = me->GetRigidBodyControl(origNode);
		if (origData) {
			INode* const cloneNode = clonedNodes[i];
			Control* const cloneControl = cloneNode->GetTMController();
			Control* const oldPositControl = cloneControl->GetPositionController();
			Control* const oldRotationControl = cloneControl->GetRotationController();

			Matrix3 cloneMatrix (cloneNode->GetNodeTM(t));
			
			RigidBodyController* const clonePositionController = (RigidBodyController*) RigidBodyControllerDesc.Create(oldPositControl->ClassID());
			RigidBodyRotationController* const cloneRotationController = (RigidBodyRotationController*) rigidBodyRotationControllerDesc.Create(oldRotationControl->ClassID());

			cloneControl->SetPositionController(clonePositionController);
			cloneControl->SetRotationController(cloneRotationController);

			RigidBodyData* const cloneData = clonePositionController;
			memcpy (cloneData, origData, sizeof (RigidBodyData));

			NewtonCollision* const collision = NewtonBodyGetCollision(origData->m_body);

			dMatrix matrix;
			NewtonBodyGetMatrix(origData->m_body, &matrix[0][0]);
			cloneData->m_body = NewtonCreateBody (me->m_newton, collision, &matrix[0][0]);

			NewtonBodySetUserData(cloneData->m_body, cloneNode);

			float mass;
			float Ixx;
			float Iyy;
			float Izz;
			NewtonBodyGetMassMatrix(origData->m_body, &mass, &Ixx, &Iyy, &Izz);
			NewtonBodySetMassMatrix(cloneData->m_body, mass, Ixx, Iyy, Izz);

			NewtonBodySetMaterialGroupID(cloneData->m_body, NewtonBodyGetMaterialGroupID(origData->m_body));
			NewtonBodySetForceAndTorqueCallback(cloneData->m_body, NewtonBodyGetForceAndTorqueCallback(origData->m_body));

			dVector com;
			NewtonBodyGetCentreOfMass(origData->m_body, &com[0]);
			NewtonBodySetCentreOfMass(cloneData->m_body, &com[0]);
			
			dVector veloc;
			NewtonBodyGetVelocity(origData->m_body, &veloc[0]);
			NewtonBodySetVelocity(cloneData->m_body, &veloc[0]);

			dVector omega;
			NewtonBodyGetOmega(origData->m_body, &omega[0]);
			NewtonBodySetOmega(cloneData->m_body, &omega[0]);

			cloneNode->SetNodeTM(t, cloneMatrix);

			_ASSERTE (me->GetRigidBodyControl (cloneNode));

		}
	}

	me->m_savedCloneList.RemoveAll();
}
*/





RigidBodyWorld::RigidBodyWorld()
	:UtilityObj()
	,m_selectionChange(true)
	,m_newtonBodyUI(NULL)
	,m_newtonWorldUI(NULL)
{
}

RigidBodyWorld::~RigidBodyWorld()
{
}

SClass_ID RigidBodyWorld::SuperClassID()
{
	return RigidBodyWorldDesc::GetDescriptor()->SuperClassID();
}

Class_ID RigidBodyWorld::ClassID()
{
	return RigidBodyWorldDesc::GetDescriptor()->ClassID();
}


void RigidBodyWorld::DeleteThis ()
{
}


void RigidBodyWorld::InitUI(HWND hWnd)
{
	RigidBodyWorldDesc* const desc = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();

	HWND minFps = GetDlgItem(hWnd, IDC_MINUMIN_SIMULATION_RATE);
	HWND gravity_x = GetDlgItem(hWnd, IDC_GRAVITY_X);
	HWND gravity_y = GetDlgItem(hWnd, IDC_GRAVITY_Y);
	HWND gravity_z = GetDlgItem(hWnd, IDC_GRAVITY_Z);

	m_minFps = GetICustEdit(minFps);
	m_gravity[0] = GetICustEdit(gravity_x);
	m_gravity[1] = GetICustEdit(gravity_y);
	m_gravity[2] = GetICustEdit(gravity_z);


	float scale = 1.0f / float (GetMasterScale(UNITS_METERS));
	dVector gravity = desc->m_systemMatrixInv.RotateVector(desc->m_gravity.Scale (scale));

	m_minFps->SetText(desc->m_minFps);
	m_gravity[0]->SetText(gravity.m_x, 1);
	m_gravity[1]->SetText(gravity.m_y, 1);
	m_gravity[2]->SetText(gravity.m_z, 1);
}

void RigidBodyWorld::DestroyUI(HWND hWnd)
{
	ReleaseICustEdit (m_minFps);
	ReleaseICustEdit (m_gravity[0]);
	ReleaseICustEdit (m_gravity[1]);
	ReleaseICustEdit (m_gravity[2]);
}


void RigidBodyWorld::BeginEditParams (Interface *ip, IUtil *iu)
{
	_ASSERTE (ip == GetCOREInterface());
	m_newtonWorldUI = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_NEWTON_WORLD_PANE), RigidBodyWorld::Proc, _T("Newton World"), LPARAM (this));
	m_newtonBodyUI = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_NEWTON_BODY_PANE), RigidBodyUIPane::Proc, _T("RigidBodies properties"), LPARAM (this));
	SelectionSetChanged (ip, iu);
}

void RigidBodyWorld::EndEditParams (Interface *ip, IUtil *iu)
{
//	m_iu = NULL;
	_ASSERTE (ip == GetCOREInterface());
	ip->DeleteRollupPage(m_newtonWorldUI);
	ip->DeleteRollupPage(m_newtonBodyUI);
}



INT_PTR CALLBACK RigidBodyWorld::Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RigidBodyWorld* const world = (RigidBodyWorld *)GetWindowLong (hWnd, GWLP_USERDATA);
	RigidBodyWorldDesc* const desc = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();

	switch (msg) 
	{
		case WM_INITDIALOG:
		{
			RigidBodyWorld* const world = (RigidBodyWorld *)lParam;
			SetWindowLong(hWnd, GWLP_USERDATA, (LONG)world);
			world->RigidBodyWorld::InitUI(hWnd);
			break;
		}

		case WM_DESTROY:
		{
			world->RigidBodyWorld::DestroyUI(hWnd);
			break;
		}

		case WM_ENABLE:
		{
			//EnableWindow(obj->m_worldPaneHandle, (BOOL) wParam);
			break;
		}

		case WM_TIMER:
		{
			_ASSERTE (0);
			//obj->Update();
			break;
		}


		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_MAKE_RIGIDBODY:
				{
					Interface* const ip = GetCOREInterface();
					int selectionCount = ip->GetSelNodeCount();
					for (int i = 0; i < selectionCount; i ++) {
						INode* const node = ip->GetSelNode(i);
						Object* const obj = node->GetObjOrWSMRef();
						if (obj) {
							world->AttachRigiBodyController (node);
						}
					}
					world->UpdateViewPorts();
					break;
				}

				case IDC_DELETE_RIGIDBODY:
				{
					Interface* const ip = GetCOREInterface();
					int selectionCount = ip->GetSelNodeCount();
					for (int i = 0; i < selectionCount; i ++) {
						INode* const node = ip->GetSelNode(i);
						world->DetachRigiBodyController (node);
					}
					world->UpdateViewPorts ();

					break;
				}

				case IDC_SHOW_GIZMOS:
				{
					for (NewtonBody* body = NewtonWorldGetFirstBody(desc->m_newton); body; body = NewtonWorldGetNextBody(desc->m_newton, body)) {
						INode* const node = (INode*)NewtonBodyGetUserData(body);
						RigidBodyController* const bodyInfo = (RigidBodyController*)desc->GetRigidBodyControl(node);
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
						RigidBodyController* const bodyInfo = (RigidBodyController*)desc->GetRigidBodyControl(node);
						_ASSERTE (bodyInfo);
						bodyInfo->m_hideGizmos = TRUE;
					}
					world->UpdateViewPorts();
					break;
				}

				case IDC_SELECT_ALL:
				{
					world->m_selectionChange = false;
					Interface* const ip = GetCOREInterface();
					ip->ClearNodeSelection(FALSE);
					for (NewtonBody* body = NewtonWorldGetFirstBody(desc->m_newton); body; body = NewtonWorldGetNextBody(desc->m_newton, body)) {
						INode* const node = (INode*)NewtonBodyGetUserData(body);
						ip->SelectNode(node, 0); 
					}
					world->m_selectionChange = true;
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
						world->DetachRigiBodyController (node);
					}
					world->UpdateViewPorts();
					break;
				}

				case IDC_MINUMIN_SIMULATION_RATE:
				{
					desc->m_minFps = world->m_minFps->GetFloat();
					break;
				}

				case IDC_GRAVITY_X:
				case IDC_GRAVITY_Y:
				case IDC_GRAVITY_Z:
				{
					dVector gravity (world->m_gravity[0]->GetFloat(), world->m_gravity[1]->GetFloat(), world->m_gravity[2]->GetFloat(), 0.0f);
					//world->m_gravity[0]->SetText(gravity.m_x, 1);
					//world->m_gravity[1]->SetText(gravity.m_y, 1);
					//world->m_gravity[2]->SetText(gravity.m_z, 1);
					desc->m_gravity = desc->m_systemMatrix.RotateVector(gravity.Scale(float (GetMasterScale(UNITS_METERS))));
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

*/
			}

			break;
		}

		default:
		return FALSE;
	}
	return TRUE;
}


void RigidBodyWorld::SelectionSetChanged (Interface *ip, IUtil *iu)
{
return ;
	if (m_selectionChange) {
		RigidBodyUIPane::SelectionSetChanged();
/*
		m_currentSelection.RemoveAll();
		int selectionCount = m_ip->GetSelNodeCount();
		for (int i = 0; i < selectionCount; i ++) {
			INode* const node = m_ip->GetSelNode(i);
			m_currentSelection.Append(node);
		}

		NewtonRigidBodyInfoPane::SelectionHasChanged();
*/
	}
}




void RigidBodyWorld::AttachRigiBodyController (INode* const node)
{
	RigidBodyControllerDesc& desc = *(RigidBodyControllerDesc*)RigidBodyControllerDesc::GetDescriptor();
	Control* const control = node->GetTMController();
	if (control->ClassID() != desc.ClassID()) {
		Matrix3 matrix (node->GetNodeTM (GetCOREInterface()->GetTime()));		

		Interval thisValidity (FOREVER);
		//desc.GetValidity (node, GetCOREInterface()->GetTime(), thisValidity);


		RigidBodyData data;
		data.m_oldControlerID = control->ClassID();

#if 1
		// this create the matrix controller but for some reason that I cannot explain I can no move the body with the navigation 
		RigidBodyController* const rigidBodyController = (RigidBodyController*)CreateInstance(desc.SuperClassID(), desc.ClassID());
//		RigidBodyController* const rigidBodyController = (RigidBodyController*)CreateInstance(CTRL_MATRIX3_CLASS_ID, RIGIDBODY_CONTROLLER_ID);
		_ASSERTE (rigidBodyController);
		rigidBodyController->PostInit (data, node);
#else
		// this creates a proper CTRL_MATRIX3_CLASS_ID and everything work as it should, therefore I most be missing something in my implementation
		// but I do not know what, please help 
		RigidBodyController* const rigidBodyController = (RigidBodyController*)CreateInstance(CTRL_MATRIX3_CLASS_ID, Class_ID(PRS_CONTROL_CLASS_ID, 0));
		_ASSERTE (rigidBodyController);
#endif

		node->SetTMController (rigidBodyController);
		_ASSERTE (node->GetTMController());
		_ASSERTE (node->GetTMController() == rigidBodyController);


        BOOL updateObjTM = FALSE;
        node->SetAFlag(A_INODE_IN_UPDATE_TM); //flag to fix #592326. exposeTM's check this for loops.

//		Interval tmValid() const { return tmvi; }
//        if (!node->tmValid().InInterval(GetCOREInterface()->GetTime()))  {
	          // This is the case before initial pose is introduced.
              // Add in TM controller's relative TM
              rigidBodyController->GetValue(GetCOREInterface()->GetTime(), &matrix, thisValidity, CTRL_RELATIVE);
              //tmValid = thisValidity

Matrix3 matrix1 (node->GetNodeTM (GetCOREInterface()->GetTime(), &FOREVER));		

		node->SetNodeTM(GetCOREInterface()->GetTime(), matrix);
		
	}
}


void RigidBodyWorld::DetachRigiBodyController (INode* const node)
{
	RigidBodyControllerDesc& desc = *(RigidBodyControllerDesc*)RigidBodyControllerDesc::GetDescriptor();
	RigidBodyController* const rigidBodyController = (RigidBodyController*) node->GetTMController();
	if (rigidBodyController->ClassID() == desc.ClassID()) {

		Matrix3 matrix (node->GetNodeTM (GetCOREInterface()->GetTime()));		
		Control* const control = (Control*) CreateInstance (desc.SuperClassID(), rigidBodyController->m_oldControlerID);
		_ASSERTE (control);

		node->SetTMController (control);
		_ASSERTE (node->GetTMController());
		_ASSERTE (node->GetTMController() == control);
		node->SetNodeTM(GetCOREInterface()->GetTime(), matrix);
	}
}


void RigidBodyWorld::UpdateViewPorts ()
{
	//GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	GetCOREInterface()->ForceCompleteRedraw(); 
}

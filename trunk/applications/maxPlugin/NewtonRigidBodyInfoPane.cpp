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
#include "NewtonRigidBodyInfoPane.h"
#include "NewtonRigidBodyControllers.h"

#define RIGID_BODY_INFO_ID Class_ID(0x2dd440ce, 0x3a326f7c)





class RidBodyData
{
	public:
	float m_mass;
	float m_Ixx;
	float m_Iyy;
	float m_Izz;
	NewtonRigidBodyInfoPane::CollisionShape m_collisionShape;

	NewtonBody* m_body;
};


class UpdateUndoRedo: public RestoreObj
{
	public:
	UpdateUndoRedo (INode* const node, TimeValue time)
		:RestoreObj(), m_time(time), m_node (node)
	{
		m_matrix = m_node->GetNodeTM(time);
	}

	virtual void Restore(int isUndo)
	{
		m_node->SetNodeTM(m_time, m_matrix);	
	}

	virtual void Redo()
	{
		m_node->SetNodeTM(m_time, m_matrix);	
	}

	virtual int Size()
	{
		return sizeof (*this);
	}

	TimeValue m_time;
	INode* m_node;
	Matrix3 m_matrix;
};





NewtonRigidBodyInfoPane::NewtonRigidBodyInfoPane(void)
	:m_massEdit(NULL)
	,m_massSpinner(NULL)
	,m_curCollsionShape(m_convexHull)
{
}

NewtonRigidBodyInfoPane::~NewtonRigidBodyInfoPane(void)
{
}

void NewtonRigidBodyInfoPane::Init(HWND hWnd)
{
	HWND massControlHandle = GetDlgItem(hWnd, IDC_RIGID_BODY_MASS);
	HWND massSpinControlHandle = GetDlgItem(hWnd, IDC_RIGID_BODY_MASS_PINNER);
	HWND collisionShapeHandle = GetDlgItem(hWnd, IDC_RIGID_BODY_COLLISION);

	m_massEdit = GetICustEdit(massControlHandle);
	m_massSpinner = GetISpinner (massSpinControlHandle);
	m_massSpinner->LinkToEdit (massControlHandle, EDITTYPE_POS_FLOAT);
	m_massSpinner->SetScale (0.1f);
	m_massSpinner->SetLimits(0.0f, 1000.0f);

	SetCollisionData (collisionShapeHandle, m_box, "box");
	SetCollisionData (collisionShapeHandle, m_sphere, "sphere");
	SetCollisionData (collisionShapeHandle, m_cylinder, "cylinder");
	SetCollisionData (collisionShapeHandle, m_convexHull, "convex hull");
	SetCollisionData (collisionShapeHandle, m_collisionTree, "collision tree");

	//SendMessage (collisionShapeHandle, CB_SELECTSTRING, 0, LPARAM ("convex hull"));
	SendMessage (collisionShapeHandle, CB_SELECTSTRING, 0, LPARAM ("box"));
	m_curCollsionShape = m_box;
	


	PhysicsPluginObject& me = *(PhysicsPluginObject*)this;
	dList<INode*> nodeList;
	me.GetNodeList (nodeList);
	for (dList<INode*>::dListNode* ptr = nodeList.GetFirst(); ptr; ptr = ptr->GetNext()) {
		INode* const node = ptr->GetInfo();
		AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
		if (rigidBodyChunk) {
			RidBodyData* const bodyData = (RidBodyData*) rigidBodyChunk->data;
			bodyData->m_body = NULL;
			AddNodeToWorld (node);
		}
	}
}

void NewtonRigidBodyInfoPane::Destroy(HWND hWnd)
{
	PhysicsPluginObject& me = *(PhysicsPluginObject*)this;
	dList<INode*> nodeList;
	me.GetNodeList (nodeList);
	for (dList<INode*>::dListNode* ptr = nodeList.GetFirst(); ptr; ptr = ptr->GetNext()) {
		INode* const node = ptr->GetInfo();
		AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
		if (rigidBodyChunk) {
			RemoveNodeFromWorld (node);
		}
	}

	ReleaseICustEdit (m_massEdit);
	ReleaseISpinner (m_massSpinner);
}



void NewtonRigidBodyInfoPane::AddNodeToWorld (INode* const node)
{
	AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
	if (rigidBodyChunk) {
		RidBodyData* const bodyData = (RidBodyData*) rigidBodyChunk->data;
		_ASSERTE (bodyData);
		//_ASSERTE (!bodyData->m_body);

		PhysicsPluginObject& me = *(PhysicsPluginObject*)this;
		PhysicsPluginClassDesc* const plugin = (PhysicsPluginClassDesc*) PhysicsPluginClassDesc::GetControlDesc();

		float scale = float (GetMasterScale(UNITS_METERS));
		
		ObjectState os (node->EvalWorldState(0)); 

		_ASSERTE (os.obj);

		Box3 box;
		os.obj->GetLocalBoundBox(0, node, NULL, box); 

		Point3 center (box.Center());
		Point3 boxSize (box.Width());

		dMatrix GeoMatrix (GetMatrixFromMaxMatrix (node->GetObjectTM(0)));
		dMatrix nodeMatrix (GetMatrixFromMaxMatrix (node->GetNodeTM(0)));

		dMatrix offset (GeoMatrix * nodeMatrix.Inverse4x4());

		offset.m_posit.m_x += center.x;
		offset.m_posit.m_y += center.y;
		offset.m_posit.m_z += center.z;
		offset = plugin->m_systemMatrixInv * offset * plugin->m_systemMatrix;

		offset.m_posit = offset.m_posit.Scale (scale);

		dVector size (plugin->m_systemMatrix.RotateVector(dVector(boxSize.x, boxSize.y, boxSize.z, 0.0f)));
		size = size.Scale (scale);

		NewtonCollision* collision = NULL;
		switch (bodyData->m_collisionShape) 
		{
			case m_box:
			{
				collision = NewtonCreateBox(me.m_newton, size.m_x, size.m_y, size.m_z, 0, &offset[0][0]);
				break;
			}
			case m_convexHull:
			{
				_ASSERTE (0);
				break;
			}
				
			default:;
			{
				_ASSERTE (0);
			}
		}

		
		nodeMatrix = plugin->m_systemMatrixInv * nodeMatrix * plugin->m_systemMatrix;
		nodeMatrix.m_posit = nodeMatrix.m_posit.Scale (scale);

		NewtonBody* const rigidBody = NewtonCreateBody(me.m_newton, collision, &nodeMatrix[0][0]);
		NewtonBodySetUserData(rigidBody, node);

		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		bodyData->m_Ixx = inertia.m_x;
		bodyData->m_Iyy = inertia.m_y;
		bodyData->m_Izz = inertia.m_z;
		NewtonBodySetCentreOfMass(rigidBody, &origin[0]);
		NewtonBodySetMassMatrix(rigidBody, bodyData->m_mass, bodyData->m_mass * inertia.m_x, bodyData->m_mass * inertia.m_y, bodyData->m_mass * inertia.m_z);

		NewtonBodySetForceAndTorqueCallback(rigidBody, ApplyGravityForce);

		bodyData->m_body = rigidBody;
		NewtonReleaseCollision(me.m_newton, collision);
	}
}



void NewtonRigidBodyInfoPane::RemoveNodeFromWorld (INode* const node)
{
	AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
	_ASSERTE (rigidBodyChunk);

	RidBodyData* const bodyData = (RidBodyData*) rigidBodyChunk->data;
	_ASSERTE (bodyData);
	_ASSERTE (bodyData->m_body);
	
	PhysicsPluginObject& me = *(PhysicsPluginObject*)this;
	NewtonDestroyBody(me.m_newton, bodyData->m_body);

	bodyData->m_body = NULL;
}



void NewtonRigidBodyInfoPane::SaveState ()
{
	PhysicsPluginObject& me = *(PhysicsPluginObject*)this;

	TimeValue time = me.m_ip->GetTime();
	PhysicsPluginClassDesc* const plugin = (PhysicsPluginClassDesc*) PhysicsPluginClassDesc::GetControlDesc();
	float scale = float (GetMasterScale(UNITS_METERS));

	theHold.Begin();
	for (NewtonBody* body = NewtonWorldGetFirstBody (me.m_newton); body; body = NewtonWorldGetNextBody(me.m_newton, body)) {
		INode* const node = (INode*) NewtonBodyGetUserData(body);

		if (theHold.Holding()) {
			theHold.Put(new UpdateUndoRedo (node, time));
		}

		dMatrix matrix (GetMatrixFromMaxMatrix (node->GetNodeTM(time)));
		matrix = plugin->m_systemMatrixInv * matrix * plugin->m_systemMatrix;
		matrix.m_posit = matrix.m_posit.Scale (scale);

		NewtonBodySetMatrix (body, &matrix[0][0]);

	}

	theHold.Accept(_T("Newon Update"));
}




void NewtonRigidBodyInfoPane::SetSelectionMass (dFloat mass)
{
	PhysicsPluginObject& me = *(PhysicsPluginObject*)this;
	for (dList<INode*>::dListNode* ptr = me.m_currentSelection.GetFirst(); ptr; ptr = ptr->GetNext()) {
		INode* const node = ptr->GetInfo();
		AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
		if (rigidBodyChunk) {
			RidBodyData* const body = (RidBodyData*) rigidBodyChunk->data;
			body->m_mass = mass;

			RidBodyData* const bodyData = (RidBodyData*) rigidBodyChunk->data;
			_ASSERTE (bodyData);
			if (bodyData->m_body) {
				NewtonCollisionInfoRecord info;
				NewtonCollision* const collision = NewtonBodyGetCollision(bodyData->m_body);
				NewtonCollisionGetInfo (collision, &info);

				switch (info.m_collisionType)
				{
					case SERIALIZE_ID_BOX:
					case SERIALIZE_ID_CONE:
					case SERIALIZE_ID_SPHERE:
					case SERIALIZE_ID_CAPSULE:
					case SERIALIZE_ID_CYLINDER:
					case SERIALIZE_ID_COMPOUND:
					case SERIALIZE_ID_CONVEXHULL:
					case SERIALIZE_ID_CONVEXMODIFIER:
					case SERIALIZE_ID_CHAMFERCYLINDER:
					{
						dVector origin;
						dVector inertia;
						NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	
						bodyData->m_Ixx = inertia.m_x;
						bodyData->m_Iyy = inertia.m_y;
						bodyData->m_Izz = inertia.m_z;

						NewtonBodySetCentreOfMass(bodyData->m_body, &origin[0]);
						NewtonBodySetMassMatrix(bodyData->m_body, bodyData->m_mass, bodyData->m_mass * inertia.m_x, bodyData->m_mass * inertia.m_y, bodyData->m_mass * inertia.m_z);
					}
				}
			}
		}
	}
}



void NewtonRigidBodyInfoPane::ApplyGravityForce (const NewtonBody* const body, dFloat timestep, int threadIndex)
{
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);

	PhysicsPluginClassDesc* const plugin = (PhysicsPluginClassDesc*) PhysicsPluginClassDesc::GetControlDesc();
	dVector gravity (plugin->m_gravity.Scale (mass));
	NewtonBodySetForce (body, &gravity.m_x);
}


void NewtonRigidBodyInfoPane::SetTransforms (TimeValue tick) const
{
	PhysicsPluginObject& me = *(PhysicsPluginObject*)this;
	PhysicsPluginClassDesc* const plugin = (PhysicsPluginClassDesc*) PhysicsPluginClassDesc::GetControlDesc();

	float scale = 1.0f / float (GetMasterScale(UNITS_METERS));	
	for (NewtonBody* body = NewtonWorldGetFirstBody (me.m_newton); body; body = NewtonWorldGetNextBody(me.m_newton, body)) {
		INode* const node = (INode*) NewtonBodyGetUserData(body);

		AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
		_ASSERTE (rigidBodyChunk);

		RidBodyData* const bodyData = (RidBodyData*) rigidBodyChunk->data;
		_ASSERTE (bodyData);
		_ASSERTE (bodyData->m_body == body);

		dMatrix matrix;
		NewtonBodyGetMatrix(body, &matrix[0][0]);
		matrix.m_posit = matrix.m_posit.Scale (scale);
		matrix = plugin->m_systemMatrix * matrix * plugin->m_systemMatrixInv;

		Matrix3 maxMatrix;
		maxMatrix.SetRow (0, *((Point3*) &matrix[0]));
		maxMatrix.SetRow (1, *((Point3*) &matrix[1]));
		maxMatrix.SetRow (2, *((Point3*) &matrix[2]));
		maxMatrix.SetRow (3, *((Point3*) &matrix[3]));

		node->SetNodeTM(tick, maxMatrix);
	}
}



void NewtonRigidBodyInfoPane::SetCollisionData (HWND listControl, int shapeId, char* const label) const
{
	SendMessage(listControl, CB_ADDSTRING, 0, LPARAM (label));
	SendMessage (listControl, CB_SELECTSTRING, 0, LPARAM (label));
	int index = SendMessage(listControl, CB_GETCURSEL, 0, 0);  
	SendMessage(listControl, CB_SETITEMDATA, index, shapeId);  
}

Class_ID NewtonRigidBodyInfoPane::GetRigidBodyID ()
{
	return RIGID_BODY_INFO_ID;
}

void NewtonRigidBodyInfoPane::SelectionHasChanged()
{
	PhysicsPluginObject& me = *(PhysicsPluginObject*)this;

	bool activateControls = false;
	if (me.m_currentSelection.GetCount()) {
		activateControls = true;
		for (dList<INode*>::dListNode* ptr = me.m_currentSelection.GetFirst(); ptr; ptr = ptr->GetNext()) {
			INode* const node = ptr->GetInfo();

			AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
			if (!rigidBodyChunk) {
				activateControls = false;
				break;
			}
		}
	}
	
	if (activateControls) {
		INode* const node = me.m_currentSelection.GetFirst()->GetInfo();

		AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
		if (rigidBodyChunk) {
			RidBodyData* const body = (RidBodyData*) rigidBodyChunk->data;
			m_massEdit->SetText (body->m_mass);

			HWND collisionShapeHandle = GetDlgItem(me.m_hWnd, IDC_RIGID_BODY_COLLISION);
			int count = SendMessage(collisionShapeHandle, CB_GETCOUNT, 0, 0);  
			for (int i = 0; i < count; i ++) {
				CollisionShape shape = CollisionShape(SendMessage(collisionShapeHandle, CB_GETITEMDATA, i, 0));  
				if (shape == body->m_collisionShape) {
					char label[256];
					SendMessage(collisionShapeHandle, CB_GETLBTEXT, i, (LPARAM)&label[0]);
					SendMessage (collisionShapeHandle, CB_SELECTSTRING, 0, (LPARAM)&label[0]);
					break;
				}
			}
		} 
	}
}

void NewtonRigidBodyInfoPane::SelectRigiBodies()
{
	PhysicsPluginObject& me = *(PhysicsPluginObject*)this;

	me.m_selectionActive = false;
	me.m_ip->ClearNodeSelection();

	dList<INode*> nodeList;
	me.GetNodeList (nodeList);

	for (dList<INode*>::dListNode* ptr = nodeList.GetFirst(); ptr; ptr = ptr->GetNext()) {
		INode* const node = ptr->GetInfo();
		AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
		if (rigidBodyChunk) {
			me.m_ip->SelectNode(node, 0); 
		}
	}

	me.m_selectionActive = true;
	me.SelectionSetChanged (me.m_ip, me.m_iu);
}

bool NewtonRigidBodyInfoPane::AttachRigidBodyToNode(INode* const node)
{
	Object* const obj = node->GetObjOrWSMRef();
	if (obj) {
		switch(obj->SuperClassID()) 
		{
			case GEOMOBJECT_CLASS_ID: 
			{
				AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
				if (!rigidBodyChunk) {
					RidBodyData* const rigidBody = (RidBodyData*) MAX_malloc (sizeof (RidBodyData));
					memset (rigidBody, 0, sizeof (RidBodyData));

					rigidBody->m_mass = m_massEdit->GetFloat();
					rigidBody->m_collisionShape = m_curCollsionShape;

					node->AddAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0, sizeof (RidBodyData), rigidBody);
					AddNodeToWorld (node);
				}
				return true;
				break;
			}

			default:;
			{
				_ASSERTE (0);
			}
		}
	}

	return false;
}

void NewtonRigidBodyInfoPane::CloneRigidBody(INode* const orignal, INode* const clone)
{
	AppDataChunk* const sourceBody = orignal->GetAppDataChunk(GetRigidBodyID(), orignal->SuperClassID(), 0); 
	if (sourceBody) {
		_ASSERTE (!clone->GetAppDataChunk(GetRigidBodyID(), clone->SuperClassID(), 0));
		
		RidBodyData* const rigidBody = (RidBodyData*) MAX_malloc (sizeof (RidBodyData));
		memcpy (rigidBody, (RidBodyData*) sourceBody->data, sizeof (RidBodyData));
		rigidBody->m_body = NULL;
		clone->AddAppDataChunk(GetRigidBodyID(), clone->SuperClassID(), 0, sizeof (RidBodyData), rigidBody);

		AddNodeToWorld (clone);
	}
}

void NewtonRigidBodyInfoPane::AttachSelectedRigiBodies()
{
	PhysicsPluginObject& me = *(PhysicsPluginObject*)this;
	for (dList<INode*>::dListNode* nodeSel = me.m_currentSelection.GetFirst(); nodeSel; nodeSel = nodeSel->GetNext()) {
		INode* const node = nodeSel->GetInfo();
		AttachRigidBodyToNode(node);
	}
	SelectionHasChanged();
}

void NewtonRigidBodyInfoPane::RemoveRigidBodyFromNode(INode* const node)
{
	Object* const obj = node->GetObjOrWSMRef();
	if (obj) {
		switch(obj->SuperClassID()) 
		{
			case GEOMOBJECT_CLASS_ID: 
			{
				AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
				if (rigidBodyChunk) {
					RemoveNodeFromWorld(node);
					node->RemoveAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0);
				}
				break;
			}

			default:;
			{
				_ASSERTE (0);
			}
		}
	}
}

void NewtonRigidBodyInfoPane::DetachSelectedRigiBodies()
{
	PhysicsPluginObject& me = *(PhysicsPluginObject*)this;
	for (dList<INode*>::dListNode* nodeSel = me.m_currentSelection.GetFirst(); nodeSel; nodeSel = nodeSel->GetNext()) {
		INode* const node = nodeSel->GetInfo();
		RemoveRigidBodyFromNode(node);
	}
	SelectionHasChanged();
}


INT_PTR CALLBACK NewtonRigidBodyInfoPane::DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	PhysicsPluginObject* const obj = (PhysicsPluginObject *)GetWindowLong (hWnd, GWL_USERDATA);

	switch (msg) 
	{
		case WM_INITDIALOG:
		{
			PhysicsPluginObject* const obj = (PhysicsPluginObject *)lParam;
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)obj);

			obj->NewtonRigidBodyInfoPane::Init(hWnd);
			break;
		}

		case WM_DESTROY:
		{
			
			obj->NewtonRigidBodyInfoPane::Destroy(hWnd);
			break;
		}

		case WM_ENABLE:
		{
			EnableWindow(obj->m_rigidBodyPaneHandle, (BOOL) wParam);
			break;
		}

		case CC_SPINNER_CHANGE:
		{
			NewtonRigidBodyInfoPane& me = *obj;
			switch (LOWORD(wParam))
			{
				case IDC_RIGID_BODY_MASS_PINNER:
				{
					obj->SetSelectionMass (me.m_massEdit->GetFloat());
					break;
				}
			}
			return TRUE;
		}


		case WM_COMMAND:
		{
			NewtonRigidBodyInfoPane& me = *obj;
			switch (LOWORD(wParam)) 
			{
				case IDC_ATTACH_RIGIDBODY:
				{
					me.AttachSelectedRigiBodies();										
					break;
				}

				case IDC_DETACH_RIGIDBODY:
				{
					me.DetachSelectedRigiBodies();										
					break;
				}

				case IDC_SELECT_RIGIDBODIES:
				{
					me.SelectRigiBodies();										
					break;
				}

				case IDC_RIGID_BODY_COLLISION:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE) {
						HWND collisionShapeHandle = GetDlgItem(hWnd, IDC_RIGID_BODY_COLLISION);

						int index = CollisionShape(SendMessage(collisionShapeHandle, CB_GETCURSEL, 0, 0));  
						me.m_curCollsionShape = CollisionShape(SendMessage(collisionShapeHandle, CB_GETITEMDATA, index, 0));  
							for (dList<INode*>::dListNode* ptr = obj->m_currentSelection.GetFirst(); ptr; ptr = ptr->GetNext()) {
								INode* const node = ptr->GetInfo();
								AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
								if (rigidBodyChunk) {
									RidBodyData* const body = (RidBodyData*) rigidBodyChunk->data;
									body->m_collisionShape = me.m_curCollsionShape;
								}
							}

					}
					break;
				}

				case IDC_RIGID_BODY_MASS:
				{
					obj->SetSelectionMass (me.m_massEdit->GetFloat());
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



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

#define RIGIDBODY_POSITION_CONTROLLER_ID Class_ID(0x6e6c6a1b, 0x6c7d3fb9)


int RigidBodyPositionControllerDesc::IsPublic() 
{
	return 1;
}

void* RigidBodyPositionControllerDesc::Create(BOOL loading) 
{
	return new RigidBodyPositionController(Class_ID (EXPR_POS_CONTROL_CLASS_ID, 0));
}

void* RigidBodyPositionControllerDesc::Create(Class_ID oldID)
{
	return new RigidBodyPositionController(oldID);
}

const TCHAR* RigidBodyPositionControllerDesc::ClassName() 
{
	return _T("RigidBody Position Controller");
}

SClass_ID RigidBodyPositionControllerDesc::SuperClassID() 
{
	return CTRL_POSITION_CLASS_ID;
}

Class_ID RigidBodyPositionControllerDesc::ClassID() 
{
	return RIGIDBODY_POSITION_CONTROLLER_ID;
}

const TCHAR* RigidBodyPositionControllerDesc::Category() 
{
	return _T("");
}

ClassDesc* RigidBodyPositionControllerDesc::GetDescriptor() 
{
	static RigidBodyPositionControllerDesc controllerDesc;
	return &controllerDesc;
}





RigidBodyPositionController::RigidBodyPositionController(Class_ID oldClassId)
	:Control(), RigidBodyData()
{
	m_body = NULL;
	m_oldControlerID = oldClassId;
}

RigidBodyPositionController::RigidBodyPositionController(const RigidBodyPositionController& clone)
	:Control(), RigidBodyData(clone)
{
	m_body = NULL;
}

RigidBodyPositionController::~RigidBodyPositionController()
{
	_ASSERTE (m_body == NULL);
}

Class_ID RigidBodyPositionController::ClassID()
{
	return RigidBodyPositionControllerDesc::GetDescriptor()->ClassID();
}

SClass_ID RigidBodyPositionController::SuperClassID()
{
	return RigidBodyPositionControllerDesc::GetDescriptor()->SuperClassID(); 
}

void RigidBodyPositionController::Copy(Control *from)
{
	_ASSERTE (0);
}


IOResult RigidBodyPositionController::Load(ILoad *iload)
{
	IOResult ret = Control::Load(iload);

	ULONG nread;
	iload->OpenChunk();

	iload->Read(&m_position, sizeof (m_position), &nread);
	iload->Read(&m_basePosition, sizeof (m_basePosition), &nread);
	iload->Read(&m_oldControlerID, sizeof (m_oldControlerID), &nread);

	iload->CloseChunk();

	return ret;
}

IOResult RigidBodyPositionController::Save(ISave *isave)
{
	IOResult ret = Control::Save(isave);

	ULONG nwrit;
	Class_ID id (RigidBodyPositionController::ClassID());
	isave->BeginChunk(USHORT (id.PartB()));

	isave->Write(&m_position, sizeof (m_position), &nwrit);
	isave->Write(&m_basePosition, sizeof (m_basePosition), &nwrit);
	isave->Write(&m_oldControlerID, sizeof (m_oldControlerID), &nwrit);

	isave->EndChunk();

	return ret;
}


RefResult RigidBodyPositionController::NotifyRefChanged(Interval,RefTargetHandle,PartID &,RefMessage)
{
	_ASSERTE (0);
	return REF_SUCCEED;
}

void RigidBodyPositionController::MouseCycleStarted(TimeValue t)
{
	m_basePosition += m_position;
	m_position = Point3(0.0f, 0.0f, 0.0f);
	Control::MouseCycleStarted (t);
}

void RigidBodyPositionController::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
{
	Matrix3& matrix = *(Matrix3*)val;		
	if (method == CTRL_ABSOLUTE) {
		_ASSERTE (0);
		//matrix.PreTranslate (m_position + m_basePosition);
	} else {
		matrix.PreTranslate (m_position + m_basePosition);
	}
}

void RigidBodyPositionController::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
{
	Point3& posit = *(Point3*)val;		
	if (method == CTRL_ABSOLUTE) {
		m_basePosition = posit;
		m_position = Point3(0.0f, 0.0f, 0.0f);
	} else {
		m_position = posit;
	}
}
	

void RigidBodyPositionController::ApplyGravityForce (const NewtonBody* const body, dFloat timestep, int threadIndex)
{
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);

	RigidBodyWorldDesc* const plugin = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();
	dVector gravity (plugin->m_gravity.Scale (mass));
	NewtonBodySetForce (body, &gravity.m_x);
}


void RigidBodyPositionController::RemoveRigidBody(INode* const myNode)
{
	RigidBodyWorldDesc* const plugin = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();
	_ASSERTE (m_body);
	NewtonDestroyBody(plugin->m_newton, m_body);
	plugin->m_newton = NULL;
}

void RigidBodyPositionController::AddRigidBody(INode* const myNode)
{
	RigidBodyWorldDesc* const plugin = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();

	float scale = float (GetMasterScale(UNITS_METERS));
	ObjectState os (myNode->EvalWorldState(0)); 
	_ASSERTE (os.obj);

	Box3 box;
	os.obj->GetLocalBoundBox(0, myNode, NULL, box); 

	Point3 center (box.Center());
	Point3 boxSize (box.Width());

	dMatrix GeoMatrix (GetMatrixFromMaxMatrix (myNode->GetObjectTM(GetCOREInterface()->GetTime())));
	dMatrix nodeMatrix (GetMatrixFromMaxMatrix (myNode->GetNodeTM(GetCOREInterface()->GetTime())));

	dMatrix offset (GeoMatrix * nodeMatrix.Inverse4x4());

	offset.m_posit.m_x += center.x;
	offset.m_posit.m_y += center.y;
	offset.m_posit.m_z += center.z;
	offset = plugin->m_systemMatrixInv * offset * plugin->m_systemMatrix;

	offset.m_posit = offset.m_posit.Scale (scale);

	dVector size (plugin->m_systemMatrix.RotateVector(dVector(boxSize.x, boxSize.y, boxSize.z, 0.0f)));
	size = size.Scale (scale);

	NewtonCollision* collision = NULL;
	switch (m_collisionShape) 
	{
		case m_box:
		{
			collision = NewtonCreateBox(plugin->m_newton, size.m_x, size.m_y, size.m_z, 0, &offset[0][0]);
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

	m_body = NewtonCreateBody(plugin->m_newton, collision, &nodeMatrix[0][0]);
	NewtonBodySetUserData(m_body, myNode);

	NewtonConvexCollisionCalculateInertialMatrix (collision, &m_inertia[0], &m_origin[0]);	

	NewtonBodySetCentreOfMass(m_body, &m_origin[0]);
	NewtonBodySetMassMatrix(m_body, m_mass, m_mass * m_inertia.m_x, m_mass * m_inertia.m_y, m_mass * m_inertia.m_z);

	NewtonBodySetForceAndTorqueCallback(m_body, ApplyGravityForce);

	NewtonReleaseCollision(plugin->m_newton, collision);
}

struct RendefGizmoInfo
{
	dMatrix m_matrix;
	GraphicsWindow* m_graphicWindow;
	RigidBodyPositionController* m_me;
};

void RigidBodyPositionController::RenderGizmo (void* const userData, int vertexCount, const dFloat* const faceArray, int faceId)
{
	RendefGizmoInfo* const info = (RendefGizmoInfo*) userData;
	GraphicsWindow* const gw = info->m_graphicWindow;

	Point3 point3[256];
	info->m_matrix.TransformTriplex (&point3[0].x, sizeof (Point3), faceArray, 3 * sizeof (dFloat), vertexCount);
	gw->polyline (vertexCount, point3, NULL, NULL, TRUE, NULL);
}

int RigidBodyPositionController::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags)
{
	RigidBodyWorldDesc* const plugin = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();
	RigidBodyPositionController* const bodyInfo = plugin->GetRigidBodyControl(inode);

	if (bodyInfo && !bodyInfo->m_hideGizmos) {
		_ASSERTE (m_body);
		GraphicsWindow* const gw = vpt->getGW();
		float scale = float (GetMasterScale(UNITS_METERS));

		dMatrix matrix (GetMatrixFromMaxMatrix (inode->GetNodeTM(t)));
		matrix = plugin->m_systemMatrixInv * matrix * plugin->m_systemMatrix;
		matrix.m_posit = matrix.m_posit.Scale (scale);

		// this does not handle scale yet, 
		// remember to extra scale for the matrix, 
		// the simplest way is removing the scale controller form the PRS controller
		NewtonBodySetMatrix (m_body, &matrix[0][0]);

		gw->setColor (LINE_COLOR, Point3 (1.0f, 0.5f, 0.0f));


		RendefGizmoInfo info;
		dMatrix scaleMtarix (GetIdentityMatrix());
		scaleMtarix[0][0] = 1.0f / scale;
		scaleMtarix[1][1] = 1.0f / scale;
		scaleMtarix[2][2] = 1.0f / scale;

		info.m_me = this;
		info.m_matrix = scaleMtarix * plugin->m_systemMatrixInv;
		info.m_graphicWindow = gw;

		gw->setTransform(Matrix3(TRUE)); 
		NewtonCollision* const collision = NewtonBodyGetCollision(m_body);
		NewtonCollisionForEachPolygonDo (collision, &matrix[0][0], RenderGizmo, &info);

		return 1;
	} else {
		return 0;
	}
}


void RigidBodyPositionController::PostCloneNode()
{
	_ASSERTE (0);
}

RefTargetHandle RigidBodyPositionController::Clone(RemapDir& remap)
{
	RigidBodyPositionController* ctrl = new RigidBodyPositionController(*this);
	CloneControl(ctrl, remap);
	return ctrl;
}

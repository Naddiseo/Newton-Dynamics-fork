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
#include "RigidBodyRotationController.h"


#define RIGIDBODY_ROTATION_CONTROLLER_ID Class_ID(0x30a55fdc, 0x422f5500)



int RigidBodyRotationControllerDesc::IsPublic() 
{
	return 1;
}

void* RigidBodyRotationControllerDesc::Create(BOOL loading) 
{
	return new RigidBodyRotationController(Class_ID(EULER_CONTROL_CLASS_ID, 0));
}

void* RigidBodyRotationControllerDesc::Create(Class_ID oldID) 
{
	return new RigidBodyRotationController(oldID);
}


const TCHAR* RigidBodyRotationControllerDesc::ClassName() 
{
	return _T("RigidBody Rotation Controller");
}

SClass_ID RigidBodyRotationControllerDesc::SuperClassID() 
{
	return CTRL_ROTATION_CLASS_ID;
}

Class_ID RigidBodyRotationControllerDesc::ClassID() 
{
	return RIGIDBODY_ROTATION_CONTROLLER_ID;
}

const TCHAR* RigidBodyRotationControllerDesc::Category() 
{
	return _T("");
}

ClassDesc* RigidBodyRotationControllerDesc::GetDescriptor() 
{
	static RigidBodyRotationControllerDesc controllerDesc;
	return &controllerDesc;
}





RigidBodyRotationController::RigidBodyRotationController(Class_ID oldID)
	:Control()
	,m_rotation ()
	,m_baseRotation()
	,m_oldControlerID(oldID)
{
}

Class_ID RigidBodyRotationController::ClassID()
{
	return RigidBodyRotationControllerDesc::GetDescriptor()->ClassID();
}

SClass_ID RigidBodyRotationController::SuperClassID()
{
	return RigidBodyRotationControllerDesc::GetDescriptor()->SuperClassID(); 
}

void RigidBodyRotationController::Copy(Control *from)
{
	_ASSERTE (0);
}

IOResult RigidBodyRotationController::Load(ILoad *iload)
{
	IOResult ret = Control::Load(iload);

	ULONG nread;
	Class_ID id (RigidBodyRotationController::ClassID());
	iload->OpenChunk();

	iload->Read(&m_rotation, sizeof (m_rotation), &nread);
	iload->Read(&m_baseRotation, sizeof (m_baseRotation), &nread);
	iload->Read(&m_oldControlerID, sizeof (m_oldControlerID), &nread);

	iload->CloseChunk();

	return ret;
}

IOResult RigidBodyRotationController::Save(ISave *isave)
{
	IOResult ret = Control::Save(isave);

	ULONG nwrit;
	Class_ID id (RigidBodyRotationController::ClassID());
	isave->BeginChunk(USHORT (id.PartB()));

	isave->Write(&m_rotation, sizeof (m_rotation), &nwrit);
	isave->Write(&m_baseRotation, sizeof (m_baseRotation), &nwrit);
	isave->Write(&m_oldControlerID, sizeof (m_oldControlerID), &nwrit);

	isave->EndChunk();
	return ret;
}


RefResult RigidBodyRotationController::NotifyRefChanged(Interval,RefTargetHandle,PartID &,RefMessage)
{
	_ASSERTE (0);
	return REF_FAIL;
}

void RigidBodyRotationController::MouseCycleStarted(TimeValue t)
{
	m_baseRotation *= m_rotation;
	m_rotation = Quat();
	Control::MouseCycleStarted (t);
}

void RigidBodyRotationController::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
{
	Matrix3& matrix = *(Matrix3*)val;		
	if (method == CTRL_ABSOLUTE) {
		_ASSERTE (0);
		//matrix.PreTranslate (m_position + m_basePosition);
	} else {
		Matrix3 rot;
		rot.SetRotate(m_baseRotation * m_rotation);
		matrix = rot * matrix;
	}
}

void RigidBodyRotationController::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
{
	if (method == CTRL_ABSOLUTE) {
		Quat& rotation = *(Quat*)val;		
		m_rotation = Quat();
		m_baseRotation = rotation;
	} else {
		AngAxis& axisAngle = *(AngAxis*)val;		
		Quat rotation (axisAngle);
		m_rotation = rotation;
	}
}



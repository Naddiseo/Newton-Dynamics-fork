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
#include "NewtonRigidBodyControllers.h"

/*
#define NEWTON_POSITION_CONTROL_ID Class_ID(0x6e6c6a1b, 0x6c7d3fb9)
#define NEWTON_ROTATION_CONTROL_ID Class_ID(0x30a55fdc, 0x422f5500)



int RigidBodyPositionControlDesc::IsPublic() 
{
	return 1;
}

void* RigidBodyPositionControlDesc::Create(BOOL loading) 
{
	return new RigidBodyPositionControl();
}

void* RigidBodyPositionControlDesc::Create(Class_ID oldID) 
{
	return new RigidBodyPositionControl(oldID);
}


const TCHAR* RigidBodyPositionControlDesc::ClassName() 
{
	return _T("Newton RigidBody Position Control");
}

SClass_ID RigidBodyPositionControlDesc::SuperClassID() 
{
	return CTRL_POSITION_CLASS_ID;
}

Class_ID RigidBodyPositionControlDesc::ClassID() 
{
	return NEWTON_POSITION_CONTROL_ID;
}

const TCHAR* RigidBodyPositionControlDesc::Category() 
{
	return _T("");
}

ClassDesc* RigidBodyPositionControlDesc::GetControlDesc() 
{
	static RigidBodyPositionControlDesc controllerDesc;
	return &controllerDesc;
}



int RigidBodyRotationControlDesc::IsPublic() 
{
	return 1;
}

void* RigidBodyRotationControlDesc::Create(BOOL loading) 
{
	return new RigidBodyRotationControl();
}

void* RigidBodyRotationControlDesc::Create(Class_ID oldID) 
{
	return new RigidBodyRotationControl(oldID);
}


const TCHAR* RigidBodyRotationControlDesc::ClassName() 
{
	return _T("Newton RigidBody Rotation Control");
}

SClass_ID RigidBodyRotationControlDesc::SuperClassID() 
{
	return CTRL_ROTATION_CLASS_ID;
}

Class_ID RigidBodyRotationControlDesc::ClassID() 
{
	return NEWTON_ROTATION_CONTROL_ID;
}

const TCHAR* RigidBodyRotationControlDesc::Category() 
{
	return _T("");
}

ClassDesc* RigidBodyRotationControlDesc::GetControlDesc() 
{
	static RigidBodyRotationControlDesc controllerDesc;
	return &controllerDesc;
}




RigidBodyPositionControl::RigidBodyPositionControl(Class_ID oldID)
	:Control()
	,m_position(0.0f, 0.0f, 0.0f)
	,m_basePosition(0.0f, 0.0f, 0.0f)
	,m_oldControlerID(oldID)
{
}

Class_ID RigidBodyPositionControl::ClassID()
{
	return RigidBodyPositionControlDesc::GetControlDesc()->ClassID();
}

SClass_ID RigidBodyPositionControl::SuperClassID()
{
	return RigidBodyPositionControlDesc::GetControlDesc()->SuperClassID(); 
}

void RigidBodyPositionControl::Copy(Control *from)
{
	_ASSERTE (0);
}

IOResult RigidBodyPositionControl::Load(ILoad *iload)
{
	IOResult ret = Control::Load(iload);

	ULONG nread;
//	Class_ID id (RigidBodyPositionControl::ClassID());
	iload->OpenChunk();

	iload->Read(&m_position, sizeof (m_position), &nread);
	iload->Read(&m_basePosition, sizeof (m_basePosition), &nread);
	iload->Read(&m_oldControlerID, sizeof (m_oldControlerID), &nread);

	iload->CloseChunk();

	return ret;
}

IOResult RigidBodyPositionControl::Save(ISave *isave)
{
	IOResult ret = Control::Save(isave);

	ULONG nwrit;
	Class_ID id (RigidBodyPositionControl::ClassID());
	isave->BeginChunk(USHORT (id.PartB()));

	isave->Write(&m_position, sizeof (m_position), &nwrit);
	isave->Write(&m_basePosition, sizeof (m_basePosition), &nwrit);
	isave->Write(&m_oldControlerID, sizeof (m_oldControlerID), &nwrit);

	isave->EndChunk();

	return ret;
}


RefResult RigidBodyPositionControl::NotifyRefChanged(Interval,RefTargetHandle,PartID &,RefMessage)
{
	_ASSERTE (0);
	return REF_FAIL;
}

void RigidBodyPositionControl::MouseCycleStarted(TimeValue t)
{
	m_basePosition += m_position;
	m_position = Point3(0.0f, 0.0f, 0.0f);
	Control::MouseCycleStarted (t);
}

void RigidBodyPositionControl::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
{
	Matrix3& matrix = *(Matrix3*)val;		
	if (method == CTRL_ABSOLUTE) {
		_ASSERTE (0);
		//matrix.PreTranslate (m_position + m_basePosition);
	} else {
		matrix.PreTranslate (m_position + m_basePosition);
	}
}

void RigidBodyPositionControl::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
{
	Point3& posit = *(Point3*)val;		
	if (method == CTRL_ABSOLUTE) {
		m_basePosition = posit;
		m_position = Point3(0.0f, 0.0f, 0.0f);
	} else {
		m_position = posit;
	}
}
	




RigidBodyRotationControl::RigidBodyRotationControl(Class_ID oldID)
	:Control()
	,m_rotation ()
	,m_baseRotation()
	,m_oldControlerID(oldID)
{
}

Class_ID RigidBodyRotationControl::ClassID()
{
	return RigidBodyRotationControlDesc::GetControlDesc()->ClassID();
}

SClass_ID RigidBodyRotationControl::SuperClassID()
{
	return RigidBodyRotationControlDesc::GetControlDesc()->SuperClassID(); 
}

void RigidBodyRotationControl::Copy(Control *from)
{
	_ASSERTE (0);
}

IOResult RigidBodyRotationControl::Load(ILoad *iload)
{
	IOResult ret = Control::Load(iload);

	ULONG nread;
	Class_ID id (RigidBodyRotationControl::ClassID());
	iload->OpenChunk();

	iload->Read(&m_rotation, sizeof (m_rotation), &nread);
	iload->Read(&m_baseRotation, sizeof (m_baseRotation), &nread);
	iload->Read(&m_oldControlerID, sizeof (m_oldControlerID), &nread);

	iload->CloseChunk();

	return ret;
}

IOResult RigidBodyRotationControl::Save(ISave *isave)
{
	IOResult ret = Control::Save(isave);

	ULONG nwrit;
	Class_ID id (RigidBodyRotationControl::ClassID());
	isave->BeginChunk(USHORT (id.PartB()));

	isave->Write(&m_rotation, sizeof (m_rotation), &nwrit);
	isave->Write(&m_baseRotation, sizeof (m_baseRotation), &nwrit);
	isave->Write(&m_oldControlerID, sizeof (m_oldControlerID), &nwrit);

	isave->EndChunk();
	return ret;
}


RefResult RigidBodyRotationControl::NotifyRefChanged(Interval,RefTargetHandle,PartID &,RefMessage)
{
	_ASSERTE (0);
	return REF_FAIL;
}

void RigidBodyRotationControl::MouseCycleStarted(TimeValue t)
{
	m_baseRotation *= m_rotation;
	m_rotation = Quat();
	Control::MouseCycleStarted (t);
}

void RigidBodyRotationControl::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
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

void RigidBodyRotationControl::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
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
*/


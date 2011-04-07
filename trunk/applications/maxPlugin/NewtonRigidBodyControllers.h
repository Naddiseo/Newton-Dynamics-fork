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


#ifndef __NEWTON_RIGIDBODY_CONTROLLERS_H__
#define __NEWTON_RIGIDBODY_CONTROLLERS_H__

/*

class RigidBodyPositionControlDesc: public ClassDesc2 
{
	public:
	int  IsPublic();
	void* Create(BOOL loading);
	void* Create(Class_ID oldID);
	const TCHAR* ClassName();
	SClass_ID SuperClassID();
	Class_ID ClassID();
	const TCHAR* Category();
	static ClassDesc* GetControlDesc();
};

class RigidBodyRotationControlDesc: public ClassDesc2 
{
public:
	int  IsPublic();
	void* Create(BOOL loading);
	void* Create(Class_ID oldID);
	const TCHAR* ClassName();
	SClass_ID SuperClassID();
	Class_ID ClassID();
	const TCHAR* Category();

	static ClassDesc* GetControlDesc();
};


class RigidBodyPositionControl: public Control
{
	public:
	RigidBodyPositionControl(Class_ID oldID = Class_ID(0x118f7e02,0xffee238a));
	
	protected:
	virtual Class_ID ClassID();
	virtual SClass_ID SuperClassID();
	virtual void Copy(Control *from);
	virtual RefResult NotifyRefChanged(Interval,RefTargetHandle,PartID &,RefMessage);
	virtual void MouseCycleStarted(TimeValue t);
	virtual void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method=CTRL_ABSOLUTE);
	virtual void SetValue(TimeValue t, void *val, int commit=1, GetSetMethod method=CTRL_ABSOLUTE);

	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	Point3 m_position;
	Point3 m_basePosition;
	Class_ID m_oldControlerID;

	friend class NewtonRigidBodyInfoPane;
};


class RigidBodyRotationControl: public Control
{
public:
	RigidBodyRotationControl(Class_ID oldID = Class_ID(EULER_CONTROL_CLASS_ID, 0));

protected:
	virtual Class_ID ClassID();
	virtual SClass_ID SuperClassID();
	virtual void Copy(Control *from);
	virtual RefResult NotifyRefChanged(Interval,RefTargetHandle,PartID &,RefMessage);
	virtual void MouseCycleStarted(TimeValue t);
	virtual void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method=CTRL_ABSOLUTE);
	virtual void SetValue(TimeValue t, void *val, int commit=1, GetSetMethod method=CTRL_ABSOLUTE);

	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	Quat m_rotation;
	Quat m_baseRotation;
	Class_ID m_oldControlerID;

	friend class NewtonRigidBodyInfoPane;
};
*/


#endif
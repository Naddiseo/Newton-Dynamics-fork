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


#ifndef __RIGIDBODY_CONTROLLER_H__
#define __RIGIDBODY_CONTROLLER_H__



class RigidBodyControllerDesc: public ClassDesc2 
{
	public:
	int  IsPublic();
	void* Create(BOOL loading);
	const TCHAR* ClassName();
	SClass_ID SuperClassID();
	Class_ID ClassID();
	const TCHAR* Category();
	static ClassDesc* GetDescriptor();
};

class RigidBodyController: public Control, public RigidBodyData
{
	public:
	RigidBodyController(const RigidBodyController& clone);
	RigidBodyController();

	void PostInit (const RigidBodyData& data, INode* const myNode);

	~RigidBodyController();

	virtual Class_ID ClassID();
	virtual SClass_ID SuperClassID();
	virtual void Copy(Control *from);
	virtual RefResult NotifyRefChanged(Interval,RefTargetHandle,PartID &,RefMessage);
	virtual void MouseCycleStarted(TimeValue t);
	virtual void GetValue(TimeValue t, void* val, Interval &valid, GetSetMethod method=CTRL_ABSOLUTE);
	virtual void SetValue(TimeValue t, void* val, int commit=1, GetSetMethod method=CTRL_ABSOLUTE);

	virtual int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);


	virtual int NumSubs();
	virtual Animatable* SubAnim(int i);
	virtual BOOL IsKeyAtTime(TimeValue t,DWORD flags);
	virtual int NumRefs();    
	virtual RefTargetHandle GetReference(int i);
	virtual void SetReference(int i, RefTargetHandle rtarg);
	virtual int RemapRefOnLoad(int iref);


	
	virtual IOResult Load(ILoad* iload);
	virtual IOResult Save(ISave* isave);

	virtual Control* GetPositionController();
	virtual Control* GetRotationController();
	virtual Control* GetScaleController();

	virtual BOOL SetPositionController(Control* control);
	virtual BOOL SetRotationController(Control* control);
	virtual BOOL SetScaleController(Control* control);


	virtual void PostCloneNode();
	RefTargetHandle Clone(RemapDir& remap=DefaultRemapDir());
	

	static void ApplyGravityForce (const NewtonBody* const body, dFloat timestep, int threadIndex);
	static void RenderGizmo (void* const userData, int vertexCount, const dFloat* const faceArray, int faceId);

	void AddRigidBody(INode* const myNode);
	void RemoveRigidBody(INode* const myNode);

	Control* m_scaleControl;
	Control* m_positionControl;
	Control* m_rotationControl;
};


#endif
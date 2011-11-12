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

#ifndef _DG_DEFORMABLE_BODY_H_
#define _DG_DEFORMABLE_BODY_H_

#include "dgPhysicsStdafx.h"
#include "dgBody.h"


class dgDeformableBody: public dgBody
{
	public:
	dgDeformableBody();
	virtual ~dgDeformableBody();
/*
	virtual void AddForce (const dgVector& force);
	virtual void AddTorque (const dgVector& torque);
	virtual void AttachCollision (dgCollision* collision);

	virtual void SetGroupID (dgUnsigned32 id);
	virtual void SetMatrix(const dgMatrix& matrix);
	virtual void SetMatrixIgnoreSleep(const dgMatrix& matrix);
	virtual void SetUserData (void* const userData);
	virtual void SetForce (const dgVector& force);
	virtual void SetTorque (const dgVector& torque);
	virtual void SetOmega (const dgVector& omega);
	virtual void SetVelocity (const dgVector& velocity);
	virtual void SetLinearDamping (dgFloat32 linearDamp);
	virtual void SetAngularDamping (const dgVector& angularDamp);
	virtual void SetCentreOfMass (const dgVector& com);
	virtual void SetAparentMassMatrix (const dgVector& massMatrix);
	virtual void SetMassMatrix (dgFloat32 mass, dgFloat32 Ix, dgFloat32 Iy, dgFloat32 Iz);
//	void SetGyroscopicTorqueMode (bool mode);
	virtual void SetCollisionWithLinkedBodies (bool state);
//	void SetFreezeTreshhold (dgFloat32 freezeAccel2, dgFloat32 freezeAlpha2, dgFloat32 freezeSpeed2, dgFloat32 freezeOmega2);
//	void SetAutoactiveNotify (OnActivation activate);


	virtual void SetContinuesCollisionMode (bool mode);
	virtual void SetDestructorCallback (OnBodyDestroy destructor);
	virtual void SetMatrixUpdateCallback (OnMatrixUpdateCallback callback);
	virtual void SetExtForceAndTorqueCallback (OnApplyExtForceAndTorque callback);

	virtual OnMatrixUpdateCallback GetMatrixUpdateCallback ();
	virtual OnApplyExtForceAndTorque GetExtForceAndTorqueCallback () const;

	virtual dgConstraint* GetFirstJoint() const;
	virtual dgConstraint* GetNextJoint(dgConstraint* joint) const;

	virtual dgConstraint* GetFirstContact() const;
	virtual dgConstraint* GetNextContact(dgConstraint* joint) const;


	virtual void* GetUserData() const;
	virtual dgWorld* GetWorld() const;
	virtual const dgVector& GetMass() const;
	virtual const dgVector& GetInvMass() const;
	virtual const dgVector& GetAparentMass() const;
	 
	const dgVector& GetOmega() const;
	const dgVector& GetVelocity() const;
	const dgVector& GetForce() const;
	const dgVector& GetTorque() const;
	const dgVector& GetNetForce() const;
	const dgVector& GetNetTorque() const;

	dgCollision* GetCollision () const;
	dgUnsigned32 GetGroupID () const;
	const dgMatrix& GetMatrix() const;
	const dgVector& GetPosition() const;
	const dgQuaternion& GetRotation() const;

	dgFloat32 GetLinearDamping () const;
	dgVector GetAngularDamping () const;
	dgVector GetCentreOfMass () const;
	bool IsInEquelibrium () const;
	void GetAABB (dgVector &p0, dgVector &p1) const;	

	bool GetSleepState () const;
	bool GetAutoSleep () const;
	void SetAutoSleep (bool state);

	bool GetFreeze () const;
	void SetFreeze (bool state);

	void Freeze ();
	void Unfreeze ();

	dgInt32 GetUniqueID () const;

	bool GetCollisionWithLinkedBodies () const;
	bool GetContinuesCollisionMode () const;

	virtual void AddBuoyancyForce (dgFloat32 fluidDensity, dgFloat32 fluidLinearViscousity, dgFloat32 fluidAngularViscousity, 
						   const dgVector& gravityVector, GetBuoyancyPlane buoyancyPlane, void* const context);

	virtual dgVector CalculateInverseDynamicForce (const dgVector& desiredVeloc, dgFloat32 timestep) const;


	virtual dgFloat32 RayCast (const dgLineBox& line, OnRayCastAction filter, OnRayPrecastAction preFilter, void* const userData, dgFloat32 minT) const;
	virtual dgFloat32 RayCastSimd (const dgLineBox& line, OnRayCastAction filter, OnRayPrecastAction preFilter, void* const userData, dgFloat32 minT) const;
//	dgFloat32 RayCastSimd (const dgVector& globalP0, const dgVector& globalP1, 
//					   OnRayCastAction filter, OnRayPrecastAction preFilter, void* userData, dgFloat32 minT) const;

	virtual void CalcInvInertiaMatrix ();
	virtual void CalcInvInertiaMatrixSimd ();
	const dgMatrix& GetCollisionMatrix () const;

	dgBodyMasterList::dgListNode* GetMasterList() const;

	void InvalidateCache ();


	private:
	virtual void SetMatrixOriginAndRotation(const dgMatrix& matrix);
	virtual void CalculateContinueVelocity (dgFloat32 timestep, dgVector& veloc, dgVector& omega) const;
	virtual void CalculateContinueVelocitySimd (dgFloat32 timestep,	dgVector& veloc, dgVector& omega) const;
	virtual dgVector GetTrajectory (const dgVector& veloc, const dgVector& omega) const;
	virtual void IntegrateVelocity (dgFloat32 timestep);
	virtual void UpdateMatrix (dgFloat32 timestep, dgInt32 threadIndex);
	virtual void UpdateCollisionMatrix (dgFloat32 timestep, dgInt32 threadIndex);
	virtual void UpdateCollisionMatrixSimd (dgFloat32 timestep, dgInt32 threadIndex);
	virtual void ApplyExtenalForces (dgFloat32 timestep, dgInt32 threadIndex);
	virtual void AddImpulse (const dgVector& pointVeloc, const dgVector& pointPosit);
	virtual void ApplyImpulseArray (dgInt32 count, dgInt32 strideInBytes, const dgFloat32* const impulseArray, const dgFloat32* const pointArray);
//	void AddGyroscopicTorque();
	virtual void AddDamingAcceleration();
	
	virtual dgMatrix CalculateInertiaMatrix () const;
	virtual dgMatrix CalculateInvInertiaMatrix () const;
*/

};



#endif 


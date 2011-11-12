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

#include "dgPhysicsStdafx.h"
#include "dgBody.h"
#include "dgWorld.h"
#include "dgContact.h"
#include "dgWorldDynamicUpdate.h"

#define REST_RELATIVE_VELOCITY		dgFloat32 (1.0e-3f)
#define MAX_DYNAMIC_FRICTION_SPEED	dgFloat32 (0.3f)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

dgContactMaterial::dgContactMaterial()
{
	//	_ASSERTE ((sizeof (dgContactMaterial) & 15) == 0);
	_ASSERTE ((((dgUnsigned64) this) & 15) == 0);
	m_point.m_x = dgFloat32 (0.0f);
	m_point.m_y = dgFloat32 (0.0f);
	m_point.m_z = dgFloat32 (0.0f);
	m_softness = dgFloat32 (0.1f);
	m_restitution = dgFloat32 (0.4f);

	m_staticFriction0 = dgFloat32 (0.9f);
	m_staticFriction1 = dgFloat32 (0.9f);
	m_dynamicFriction0 = dgFloat32 (0.5f);
	m_dynamicFriction1 = dgFloat32 (0.5f);
	m_dir0_Force = dgFloat32 (0.0f);
	m_dir1_Force = dgFloat32 (0.0f);
	m_normal_Force = dgFloat32 (0.0f);
	m_penetrationPadding = dgFloat32 (0.0f);
//m_penetrationPadding = 0.5f;

//	m_collisionEnable = true;
//	m_friction0Enable = true;
//	m_friction1Enable = true;
//	m_collisionContinueCollisionEnable = true;
	m_flags = m_collisionEnable__ | m_friction0Enable__ | m_friction1Enable__ | m_collisionContinueCollisionEnable__;
}



dgContact::dgContact(dgWorld* world)
	:dgConstraint(), dgList<dgContactMaterial>(world->GetAllocator())
{
	_ASSERTE ((((dgUnsigned64) this) & 15) == 0);

	dgActiveContacts &activeContacts = *world;

	m_contactNode = activeContacts.Append (this);

	m_maxDOF = 3;
	m_broadphaseLru = 0;
	m_enableCollision = true;
	m_constId = dgContactConstraintId;
	m_world = world;
	m_myCacheMaterial = NULL;
}

inline dgContact::~dgContact()
{
	dgActiveContacts &activeContacts = *m_world;
	dgList<dgContactMaterial>::RemoveAll();
	activeContacts.Remove (m_contactNode);
}

void dgContact::GetInfo (dgConstraintInfo* const info) const
{
	memset (info, 0, sizeof (dgConstraintInfo));
	InitInfo (info);
	info->m_collideCollisionOn = GetCount();
	strcpy (info->m_discriptionType, "contact");
}

void dgContact::CalculatePointDerivative (dgInt32 index, dgContraintDescritor& desc, const dgVector& dir, const dgPointParam& param) const
{
	_ASSERTE (m_body0);
	_ASSERTE (m_body1);

	dgVector r0CrossDir (param.m_r0 * dir);
	dgJacobian &jacobian0 = desc.m_jacobian[index].m_jacobian_IM0; 
	jacobian0.m_linear[0] = dir.m_x;
	jacobian0.m_linear[1] = dir.m_y;
	jacobian0.m_linear[2] = dir.m_z;
	jacobian0.m_linear[3] = dgFloat32 (0.0f);
	jacobian0.m_angular[0] = r0CrossDir.m_x;
	jacobian0.m_angular[1] = r0CrossDir.m_y;
	jacobian0.m_angular[2] = r0CrossDir.m_z;
	jacobian0.m_angular[3] = dgFloat32 (0.0f);


	dgVector r1CrossDir (dir * param.m_r1);
	dgJacobian &jacobian1 = desc.m_jacobian[index].m_jacobian_IM1; 
	jacobian1.m_linear[0] = -dir.m_x;
	jacobian1.m_linear[1] = -dir.m_y;
	jacobian1.m_linear[2] = -dir.m_z;
	jacobian1.m_linear[3] =  dgFloat32 (0.0f);
	jacobian1.m_angular[0] = r1CrossDir.m_x;
	jacobian1.m_angular[1] = r1CrossDir.m_y;
	jacobian1.m_angular[2] = r1CrossDir.m_z;
	jacobian1.m_angular[3] = dgFloat32 (0.0f);
}

dgUnsigned32 dgContact::JacobianDerivative (dgContraintDescritor& params)
{
	dgInt32 frictionIndex = 0;
	if (m_maxDOF) {
		dgInt32 i = 0;
		frictionIndex = GetCount();
		for (dgList<dgContactMaterial>::dgListNode* node = GetFirst(); node; node = node->GetNext()) {
			const dgContactMaterial& contact = node->GetInfo(); 
			JacobianContactDerivative (params, contact, i, frictionIndex);
			i ++;
		}
	}

	return dgUnsigned32 (frictionIndex);
}



void dgContact::JacobianContactDerivative (dgContraintDescritor& params, const dgContactMaterial& contact, dgInt32 normalIndex, dgInt32& frictionIndex) 
{
	dgPointParam pointData;

	InitPointParam (pointData, dgFloat32 (1.0f), contact.m_point, contact.m_point);
	CalculatePointDerivative (normalIndex, params, contact.m_normal, pointData); 

	dgVector velocError (pointData.m_veloc1 - pointData.m_veloc0);
	dgFloat32 restitution	= contact.m_restitution;

	dgFloat32 relVelocErr = velocError % contact.m_normal;

	dgFloat32 penetration = GetMin (contact.m_penetration, dgFloat32(0.5f));
	dgFloat32 penetrationStiffness = dgFloat32 (50.0f) * contact.m_softness;
	dgFloat32 penetrationVeloc = penetration * penetrationStiffness;
	_ASSERTE (dgAbsf (penetrationVeloc - dgFloat32 (50.0f) * contact.m_softness * GetMin (contact.m_penetration, dgFloat32(0.5f))) < dgFloat32 (1.0e-6f));
	if (relVelocErr > REST_RELATIVE_VELOCITY) {
		relVelocErr *= (restitution + dgFloat32 (1.0f));
	}

	params.m_restitution[normalIndex] = restitution;
	params.m_penetration[normalIndex] = penetration;
	params.m_penetrationStiffness[normalIndex] = penetrationStiffness;
	params.m_forceBounds[normalIndex].m_low = dgFloat32 (0.0f);
	params.m_forceBounds[normalIndex].m_normalIndex = DG_NORMAL_CONSTRAINT;
	params.m_forceBounds[normalIndex].m_jointForce = (dgFloat32*)&contact.m_normal_Force;
	params.m_jointStiffness[normalIndex] = dgFloat32 (1.0f);
	params.m_isMotor[normalIndex] = 0;


	params.m_jointAccel[normalIndex] = GetMax (dgFloat32 (-4.0f), relVelocErr + penetrationVeloc) * params.m_invTimestep;
//	params.m_jointAccel[normalIndex] = (penetrationVeloc + relVelocErr) * params.m_invTimestep;
	if (contact.m_flags & dgContactMaterial::m_overrideNormalAccel__) {
		params.m_jointAccel[normalIndex] += contact.m_normal_Force;
	}

	// first dir friction force
	if (contact.m_flags & dgContactMaterial::m_friction0Enable__) {
		dgInt32 jacobIndex = frictionIndex;
		frictionIndex += 1;
		CalculatePointDerivative (jacobIndex, params, contact.m_dir0, pointData); 
		relVelocErr = velocError % contact.m_dir0;
		params.m_forceBounds[jacobIndex].m_normalIndex = normalIndex;
		params.m_jointStiffness[jacobIndex] = dgFloat32 (1.0f);

		params.m_restitution[jacobIndex] = dgFloat32 (0.0f);
		params.m_penetration[jacobIndex] = dgFloat32 (0.0f);
		params.m_penetrationStiffness[jacobIndex] = dgFloat32 (0.0f);
//		if (contact.m_override0Accel) {
		if (contact.m_flags & dgContactMaterial::m_override0Accel__) {
			params.m_jointAccel[jacobIndex] = contact.m_dir0_Force;
			params.m_isMotor[jacobIndex] = 1;
		} else {
			params.m_jointAccel[jacobIndex] = relVelocErr * params.m_invTimestep;
			params.m_isMotor[jacobIndex] = 0;
		}
		if (dgAbsf (relVelocErr) > MAX_DYNAMIC_FRICTION_SPEED) {
			params.m_forceBounds[jacobIndex].m_low = -contact.m_dynamicFriction0;
			params.m_forceBounds[jacobIndex].m_upper = contact.m_dynamicFriction0;
		} else {
			params.m_forceBounds[jacobIndex].m_low = -contact.m_staticFriction0;
			params.m_forceBounds[jacobIndex].m_upper = contact.m_staticFriction0;
		}
		params.m_forceBounds[jacobIndex].m_jointForce = (dgFloat32*)&contact.m_dir0_Force;
	}

//	if (contact.m_friction1Enable) {
	if (contact.m_flags & dgContactMaterial::m_friction1Enable__) {
		dgInt32 jacobIndex = frictionIndex;
		frictionIndex += 1;
		CalculatePointDerivative (jacobIndex, params, contact.m_dir1, pointData); 
		relVelocErr = velocError % contact.m_dir1;
		params.m_forceBounds[jacobIndex].m_normalIndex = normalIndex;
		params.m_jointStiffness[jacobIndex] = dgFloat32 (1.0f);

		params.m_restitution[jacobIndex] = dgFloat32 (0.0f);
		params.m_penetration[jacobIndex] = dgFloat32 (0.0f);
		params.m_penetrationStiffness[jacobIndex] = dgFloat32 (0.0f);
//		if (contact.m_override1Accel) {
		if (contact.m_flags & dgContactMaterial::m_override1Accel__) {
			_ASSERTE (0);
			params.m_jointAccel[jacobIndex] = contact.m_dir1_Force;
			params.m_isMotor[jacobIndex] = 1;
		} else {
			params.m_jointAccel[jacobIndex] = relVelocErr * params.m_invTimestep;
			params.m_isMotor[jacobIndex] = 0;
		}
		if (dgAbsf (relVelocErr) > MAX_DYNAMIC_FRICTION_SPEED) {
			params.m_forceBounds[jacobIndex].m_low = - contact.m_dynamicFriction1;
			params.m_forceBounds[jacobIndex].m_upper = contact.m_dynamicFriction1;
		} else {
			params.m_forceBounds[jacobIndex].m_low = - contact.m_staticFriction1;
			params.m_forceBounds[jacobIndex].m_upper = contact.m_staticFriction1;
		}
		params.m_forceBounds[jacobIndex].m_jointForce = (dgFloat32*)&contact.m_dir1_Force;
	}

//dgTrace (("p(%f %f %f)\n", params.m_jointAccel[normalIndex], params.m_jointAccel[normalIndex + 1], params.m_jointAccel[normalIndex + 2]));

}



void dgContact::JointAccelerationsSimd(dgJointAccelerationDecriptor* const params)
{
	simd_128 zero (dgFloat32 (0.0f));
	simd_128 four (dgFloat32 (4.0f));
	simd_128 negOne (dgFloat32 (-1.0f));
	simd_128 tol002 (dgFloat32 (1.0e-2f));
	simd_128 timeStep (params->m_timeStep);
	simd_128 invTimeStep (params->m_invTimeStep);

	simd_128 bodyVeloc0 ((simd_128&) m_body0->m_veloc);
	simd_128 bodyOmega0 ((simd_128&) m_body0->m_omega);
	simd_128 bodyVeloc1 ((simd_128&) m_body1->m_veloc);
	simd_128 bodyOmega1 ((simd_128&) m_body1->m_omega);

	dgInt32 count = params->m_rowsCount;
	dgJacobianMatrixElement* const rowMatrix = params->m_rowMatrix;

	for (dgInt32 k = 0; k < count; k ++) {

		if (!rowMatrix[k].m_accelIsMotor) {
			dgJacobianMatrixElement* const row = &rowMatrix[k];
			simd_128 relVeloc ((simd_128&)row->m_Jt.m_jacobian_IM0.m_linear * bodyVeloc0 +
							   (simd_128&)row->m_Jt.m_jacobian_IM0.m_angular * bodyOmega0 + 
							   (simd_128&)row->m_Jt.m_jacobian_IM1.m_linear * bodyVeloc1 +
							   (simd_128&)row->m_Jt.m_jacobian_IM1.m_angular * bodyOmega1);

			_ASSERTE (relVeloc.m_type.m128_f32[3] == dgFloat32 (0.0f));
			relVeloc = relVeloc.AddHorizontal();

			simd_128 relAccel (row->m_deltaAccel);
			if (row->m_normalForceIndex < 0) {

				simd_128 restitution ((simd_128 (row->m_restitution) & (relVeloc <= zero)) - negOne);
				simd_128 penetration (row->m_penetration);

				simd_128 penetrationMask (penetration > tol002);
				simd_128 velocMask (penetrationMask & (relVeloc > zero));
				penetration = zero.GetMax(penetration - ((relVeloc * timeStep) & velocMask));
				simd_128 penetrationVeloc ((penetration * simd_128 (row->m_penetrationStiffness)) & penetrationMask);
				penetration.StoreScalar(&row->m_penetration);
				relVeloc = four.GetMin (relVeloc * restitution - penetrationVeloc);
			}
			simd_128 a (relAccel - relVeloc * invTimeStep);
			a.StoreScalar(&row->m_coordenateAccel);
		}
	}
}


void dgContact::JointAccelerations(dgJointAccelerationDecriptor* const params)
{
	dgJacobianMatrixElement* const rowMatrix = params->m_rowMatrix;
//	const dgJacobianPair* const Jt = params.m_Jt;
	const dgVector& bodyVeloc0 = m_body0->m_veloc;
	const dgVector& bodyOmega0 = m_body0->m_omega;
	const dgVector& bodyVeloc1 = m_body1->m_veloc;
	const dgVector& bodyOmega1 = m_body1->m_omega;

	dgInt32 count = params->m_rowsCount;
	dgFloat32 timestep = params->m_timeStep;
	dgFloat32 invTimestep = params->m_invTimeStep;
	for (dgInt32 k = 0; k < count; k ++) {
		//if (!params.m_accelIsMotor[k]) {
		if (!rowMatrix[k].m_accelIsMotor) {
			dgJacobianMatrixElement* const row = &rowMatrix[k];

			dgVector relVeloc (row->m_Jt.m_jacobian_IM0.m_linear.CompProduct(bodyVeloc0));
			relVeloc += row->m_Jt.m_jacobian_IM0.m_angular.CompProduct(bodyOmega0);
			relVeloc += row->m_Jt.m_jacobian_IM1.m_linear.CompProduct(bodyVeloc1);
			relVeloc += row->m_Jt.m_jacobian_IM1.m_angular.CompProduct(bodyOmega1);
			dgFloat32 vRel = relVeloc.m_x + relVeloc.m_y + relVeloc.m_z;
			//dgFloat32 aRel = row->m_externAccelaration;
			dgFloat32 aRel = row->m_deltaAccel;

			if (row->m_normalForceIndex < 0) {
				dgFloat32 restitution = dgFloat32 (1.0f);
				if (vRel <= dgFloat32 (0.0f)) {
					restitution += row->m_restitution;
				}

				dgFloat32 penetrationVeloc = dgFloat32 (0.0f);
				if (row->m_penetration > dgFloat32 (1.0e-2f)) {
					if (vRel > dgFloat32 (0.0f)) {
						dgFloat32 penetrationCorrection = vRel * timestep;
						_ASSERTE (penetrationCorrection >= dgFloat32 (0.0f));
						row->m_penetration = GetMax (dgFloat32 (0.0f), row->m_penetration - penetrationCorrection);
					}
					penetrationVeloc = -(row->m_penetration * row->m_penetrationStiffness);
				}

				vRel *= restitution;
				vRel = GetMin (dgFloat32 (4.0f), vRel + penetrationVeloc);
			}
			row->m_coordenateAccel = (aRel - vRel * invTimestep);
		}
	}
}

void dgContact::JointVelocityCorrection(dgJointAccelerationDecriptor* const params)
{
	_ASSERTE (0);
}


dgCollidingPairCollector::dgCollidingPairCollector ()
{
	m_lock = 0;
	m_count = 0;
	m_maxSize = 0;
//	m_world = NULL;
//	m_pairs = NULL;
	m_sentinel = NULL;
}

dgCollidingPairCollector::~dgCollidingPairCollector ()
{
}

void dgCollidingPairCollector::Init ()
{
	dgWorld* const world = (dgWorld*) this;
//	m_maxSize = world->m_pairMemoryBufferSizeInBytes / sizeof (dgPair);
//	m_maxSize = world->m_pairMemoryBuffer.GetElementsCapacity() / sizeof (dgPair);
	m_pairs = (dgPair*) &world->m_pairMemoryBuffer[0];
	m_lock = 0;
	m_count = 0;
}

//void dgCollidingPairCollector::AddPair (dgBody* const bodyA, dgBody* const bodyB, dgInt32 threadIndex)
void dgCollidingPairCollector::AddPair (dgBody* const bodyPtr0, dgBody* const bodyPtr1, dgInt32 threadIndex)
{
	_ASSERTE (bodyPtr0 != m_sentinel);
	_ASSERTE (bodyPtr1 != m_sentinel);

//	if ((body0 != m_sentinel) && (body1 != m_sentinel)) {

		dgWorld* const world = (dgWorld*) this;
		if (bodyPtr0->GetSleepState() & bodyPtr1->GetSleepState()) {
			dgContact* contact = NULL;
			if (bodyPtr0->m_invMass.m_w != dgFloat32 (0.0f)) {
				for (dgBodyMasterListRow::dgListNode* link = world->FindConstraintLink (bodyPtr0, bodyPtr1); link; link = world->FindConstraintLinkNext (link, bodyPtr1)) {
					dgConstraint* const constraint = link->GetInfo().m_joint;
					if (constraint->GetId() == dgContactConstraintId) {
						contact = (dgContact*)constraint;
						break;
					}
				}
			} else if (bodyPtr1->m_invMass.m_w != dgFloat32 (0.0f)) {
				_ASSERTE (bodyPtr1->m_invMass.m_w != dgFloat32 (0.0f));
				for (dgBodyMasterListRow::dgListNode* link = world->FindConstraintLink (bodyPtr1, bodyPtr0); link; link = world->FindConstraintLinkNext (link, bodyPtr0)) {
					dgConstraint* const constraint = link->GetInfo().m_joint;
					if (constraint->GetId() == dgContactConstraintId) {
						contact = (dgContact*)constraint;
						break;
					} 
				}
			} else {
				return;
			}

			if (contact) {
				_ASSERTE (contact->GetId() == dgContactConstraintId);
				contact->m_broadphaseLru = world->m_broadPhaseLru;
			}
			
		} else {
			dgBody* tmpbody0 (bodyPtr0);
			dgBody* tmpbody1 (bodyPtr1);
			if (tmpbody0->m_uniqueID > tmpbody1->m_uniqueID) {
				Swap(tmpbody0, tmpbody1);
			}
			dgBody* const body0 (tmpbody0);
			dgBody* const body1 (tmpbody1);

			_ASSERTE (body0->GetWorld());
			_ASSERTE (body1->GetWorld());
			_ASSERTE (body0->GetWorld() == world);
			_ASSERTE (body1->GetWorld() == world);

			dgContact* contact = NULL;
			if (body0->m_invMass.m_w != dgFloat32 (0.0f)) {
				for (dgBodyMasterListRow::dgListNode* link = world->FindConstraintLink (body0, body1); link; link = world->FindConstraintLinkNext (link, body1)) {
					dgConstraint* const constraint = link->GetInfo().m_joint;
					if (constraint->GetId() == dgContactConstraintId) {
						contact = (dgContact*)constraint;
					} else {
						if (!constraint->IsCollidable()) {
							return;
						} 
					}
				}
			} else if (body1->m_invMass.m_w != dgFloat32 (0.0f)) {
				_ASSERTE (body1->m_invMass.m_w != dgFloat32 (0.0f));
				for (dgBodyMasterListRow::dgListNode* link = world->FindConstraintLink (body1, body0); link; link = world->FindConstraintLinkNext (link, body0)) {
					dgConstraint* const constraint = link->GetInfo().m_joint;
					if (constraint->GetId() == dgContactConstraintId) {
						contact = (dgContact*)constraint;
					} else {
						if (!constraint->IsCollidable()) {
							return;
						}
					}
				}
			} else {
				return;
			}

			if (!(body0->m_collideWithLinkedBodies & body1->m_collideWithLinkedBodies)) {
				if (world->AreBodyConnectedByJoints (body0, body1)) {
					return;
				}
			}

		
			_ASSERTE (!contact || contact->GetId() == dgContactConstraintId);

			dgUnsigned32 group0_ID = dgUnsigned32 (body0->m_bodyGroupId);
			dgUnsigned32 group1_ID = dgUnsigned32 (body1->m_bodyGroupId);
			if (group1_ID < group0_ID) {
				Swap (group0_ID, group1_ID);
			}

			dgUnsigned32 key = (group1_ID << 16) + group0_ID;
			const dgBodyMaterialList& materialList = *world;  

			const dgContactMaterial* const material = &materialList.Find (key)->GetInfo();

			if (material->m_flags & dgContactMaterial::m_collisionEnable__) {

				dgInt32 processContacts = 1;
				if (material->m_aabbOverlap) {
					processContacts = material->m_aabbOverlap (*material, *body0, *body1, threadIndex);
				}
				if (processContacts) {

					_ASSERTE (!body0->m_collision->IsType (dgCollision::dgCollisionNull_RTTI));
					_ASSERTE (!body1->m_collision->IsType (dgCollision::dgCollisionNull_RTTI));

					world->GetIndirectLock (&m_lock);

					if (world->m_pairMemoryBuffer.ExpandCapacityIfNeessesary (m_count, sizeof (dgPair))) {
						m_maxSize = dgInt32 (world->m_pairMemoryBuffer.GetBytesCapacity() / sizeof (dgPair));
						m_pairs = (dgPair*) &world->m_pairMemoryBuffer[0];
					}

					dgPair* const pair = &m_pairs[m_count];
					m_count ++;
					pair->m_body0 = body0;
					pair->m_body1 = body1;
					pair->m_material = material; 
					pair->m_contact = contact;
					pair->m_contactCount = 0;
					pair->m_isTrigger = 0;

					world->ReleaseIndirectLock(&m_lock);
			}
		}
	}
//	}
}







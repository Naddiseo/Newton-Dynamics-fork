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
#include "dgConstraint.h"
#include "dgWorldDynamicUpdate.h"
//#include "dgBroadPhaseCollision.h"


void dgWorldDynamicUpdate::CalculateIslandReactionForces (const dgIsland* const island, dgFloat32 timestep, dgInt32 threadID) const
{
/*
	const dgIsland* const m_islandArray = m_dynamics->m_islandArray;
//	dgContactPoint* const contactBuffer = m_world->m_contactBuffer;

	dgInt32 step = m_threads;
	dgInt32 count = m_count;
	if (m_useSimd) {
		for (dgInt32 i = 0; i < count; i += step) {
			const dgIsland& island = m_islandArray [i + m_threadIndex];
			if (!island.m_isContinueCollision) {
				m_dynamics->BuildJacobianMatrixSimd (island, m_threadIndex, m_timestep);
				m_system->CalculateReactionsForcesSimd (m_solverMode, DG_SOLVER_MAX_ERROR);
				m_dynamics->IntegrateArray (&m_system->m_bodyArray[1], m_system->m_bodyCount - 1, DG_SOLVER_MAX_ERROR, m_timestep, m_threadIndex, true);
			} else {
				dgBodyInfo* const bodyArray = &m_dynamics->m_bodyArray[island.m_bodyStart];
				dgJointInfo* const constraintArray = &m_dynamics->m_constraintArray[island.m_jointStart];

				dgFloat32 dist = dgFloat32 (0.0f);
				dgFloat32 maxSpeed2 = dgFloat32 (0.0f);
				for (dgInt32 k = 1; k < island.m_bodyCount; k ++) {
					dgVector veloc;
					dgVector omega;

					dgBody* const body = bodyArray[k].m_body;
					_ASSERTE (body->m_mass.m_w > dgFloat32  (0.0f));
					body->CalculateContinueVelocity (m_timestep, veloc, omega);

					dgFloat32 mag2 = veloc % veloc;
					if (mag2 > maxSpeed2) {
						maxSpeed2 = mag2;
						dist = body->m_collision->GetBoxMinRadius();
					}
				}

				_ASSERTE (dist > dgFloat32 (0.0f));
				dgInt32 steps = dgInt32 (dgFloat32 (4.0f) * dgSqrt (maxSpeed2) * m_timestep / dist) + 1;
				dgFloat32 timestep = m_timestep / dgFloat32 (steps);

				if (steps > 8) {
					steps = 8;
				}

				for (dgInt32 j = 0; j < steps - 1; j ++) {
					m_dynamics->BuildJacobianMatrixSimd (island, m_threadIndex, timestep);
					m_system->CalculateReactionsForcesSimd (m_solverMode, DG_SOLVER_MAX_ERROR);
					m_dynamics->IntegrateArray (&m_system->m_bodyArray[1], m_system->m_bodyCount - 1, DG_SOLVER_MAX_ERROR, timestep, m_threadIndex, false);

					for (dgInt32 k = 1; k < island.m_bodyCount; k ++) {
						dgBody* const body = bodyArray[k].m_body;
						body->m_accel = body->m_prevExternalForce;
						body->m_alpha = body->m_prevExternalTorque;
					}

					for (dgInt32 k = 0; k < island.m_jointCount; k ++) {
						dgConstraint* const constraint = constraintArray[k].m_joint;
						if (constraint->GetId() == dgContactConstraintId) {
							dgContact* const contact = (dgContact*) constraint;
							const dgContactMaterial* const material = contact->m_myCacheMaterial;
							if (material->m_flags & dgContactMaterial::m_collisionEnable__) {
								if (material->m_aabbOverlap) {
									_ASSERTE (contact->m_body0);
									_ASSERTE (contact->m_body1);
									material->m_aabbOverlap (*material, *contact->m_body0, *contact->m_body1, m_threadIndex);
								}
								_ASSERTE (0);
								dgCollidingPairCollector::dgPair pair;
								pair.m_body0 = contact->m_body0;
								pair.m_body1 = contact->m_body1;
								pair.m_contact = contact;
								pair.m_material = material;
								pair.m_contactBuffer = contactBuffer;

								m_world->CalculateContactsSimd(&pair, timestep, m_threadIndex);
								//_ASSERTE (contact->m_maxDOF == 0);

								_ASSERTE (pair.m_contact);
								if (pair.m_contactCount) {
									_ASSERTE (pair.m_contactCount <= (DG_CONSTRAINT_MAX_ROWS / 3));
									m_world->ProcessContacts (&pair, timestep, m_threadIndex);
								} else if (!pair.m_contactBuffer) {
									m_world->ProcessCachedContacts (pair.m_contact, pair.m_material, timestep, m_threadIndex);
								}
							}
						}
					}
				}
				m_dynamics->BuildJacobianMatrixSimd (island, m_threadIndex, timestep);
				m_system->CalculateReactionsForcesSimd (m_solverMode, DG_SOLVER_MAX_ERROR);
				m_dynamics->IntegrateArray (&m_system->m_bodyArray[1], m_system->m_bodyCount - 1, DG_SOLVER_MAX_ERROR, timestep, m_threadIndex, true);
			}
		}
	} else {
		for (dgInt32 i = 0; i < count; i += step) {
			const dgIsland& island = m_islandArray [i + m_threadIndex];

			if (!island.m_isContinueCollision) {
				m_dynamics->BuildJacobianMatrix (island, m_threadIndex, m_timestep);
				m_system->CalculateReactionsForces (m_solverMode, DG_SOLVER_MAX_ERROR);
				m_dynamics->IntegrateArray (&m_system->m_bodyArray[1], m_system->m_bodyCount - 1, DG_SOLVER_MAX_ERROR, m_timestep, m_threadIndex, true);
			} else {
				dgBodyInfo* const bodyArray = &m_dynamics->m_bodyArray[island.m_bodyStart];
				dgJointInfo* const constraintArray = &m_dynamics->m_constraintArray[island.m_jointStart];

				dgFloat32 dist = dgFloat32 (0.0f);
				dgFloat32 maxSpeed2 = dgFloat32 (0.0f);
				for (dgInt32 k = 1; k < island.m_bodyCount; k ++) {
					dgVector veloc;
					dgVector omega;

					dgBody* const body = bodyArray[k].m_body;
					_ASSERTE (body->m_mass.m_w > dgFloat32  (0.0f));
					body->CalculateContinueVelocity (m_timestep, veloc, omega);

					dgFloat32 mag2 = veloc % veloc;
					if (mag2 > maxSpeed2) {
						maxSpeed2 = mag2;
						dist = body->m_collision->GetBoxMinRadius();
					}
				}

				_ASSERTE (dist > dgFloat32 (0.0f));
				dgInt32 steps = dgInt32 (dgFloat32 (4.0f) * dgSqrt (maxSpeed2) * m_timestep / dist) + 1;
				dgFloat32 timestep = m_timestep / dgFloat32 (steps);

				if (steps > 8) {
					steps = 8;
				}

				for (dgInt32 j = 0; j < steps - 1; j ++) {
					m_dynamics->BuildJacobianMatrix (island, m_threadIndex, timestep);
					m_system->CalculateReactionsForces (m_solverMode, DG_SOLVER_MAX_ERROR);
					m_dynamics->IntegrateArray (&m_system->m_bodyArray[1], m_system->m_bodyCount - 1, DG_SOLVER_MAX_ERROR, timestep, m_threadIndex, false);

					for (dgInt32 k = 1; k < island.m_bodyCount; k ++) {
						dgBody* const body = bodyArray[k].m_body;
						body->m_accel = body->m_prevExternalForce;
						body->m_alpha = body->m_prevExternalTorque;
					}

					for (dgInt32 k = 0; k < island.m_jointCount; k ++) {
						dgConstraint* const constraint = constraintArray[k].m_joint;
						if (constraint->GetId() == dgContactConstraintId) {
							dgContact* const contact = (dgContact*) constraint;
							const dgContactMaterial* const material = contact->m_myCacheMaterial;
							if (material->m_flags & dgContactMaterial::m_collisionEnable__) {
								if (material->m_aabbOverlap) {
									_ASSERTE (contact->m_body0);
									_ASSERTE (contact->m_body1);
									material->m_aabbOverlap (*material, *contact->m_body0, *contact->m_body1, m_threadIndex);
								}
								_ASSERTE (0);
								dgCollidingPairCollector::dgPair pair;
								pair.m_body0 = contact->m_body0;
								pair.m_body1 = contact->m_body1;
								pair.m_contact = contact;
								pair.m_material = material;
								pair.m_contactBuffer = contactBuffer;
								m_world->CalculateContacts (&pair, timestep, m_threadIndex);

								_ASSERTE (pair.m_contact);

								if (pair.m_contactCount) {
									_ASSERTE (pair.m_contactCount <= (DG_CONSTRAINT_MAX_ROWS / 3));
									m_world->ProcessContacts (&pair, timestep, m_threadIndex);
								} else if (!pair.m_contactBuffer) {
									m_world->ProcessCachedContacts (pair.m_contact, pair.m_material, timestep, m_threadIndex);
								}
							}
						}
					}
				}
				m_dynamics->BuildJacobianMatrix (island, m_threadIndex, timestep);
				m_system->CalculateReactionsForces (m_solverMode, DG_SOLVER_MAX_ERROR);
				m_dynamics->IntegrateArray (&m_system->m_bodyArray[1], m_system->m_bodyCount - 1, DG_SOLVER_MAX_ERROR, timestep, m_threadIndex, true);
			}
		}
	}
	//	m_world->m_perfomanceCounters[m_threadIndex][m_dynamicsSolverSpanningTreeTicks] += (m_world->m_getPerformanceCount() - ticks);
*/


	if (!island->m_isContinueCollision) {
		dgInt32 rowBase = BuildJacobianMatrix (island, threadID, timestep);
		CalculateReactionsForces (island, rowBase, threadID, timestep, DG_SOLVER_MAX_ERROR);
		IntegrateArray (island, DG_SOLVER_MAX_ERROR, timestep, threadID, true); 

	} else {
		_ASSERTE (0);
	/*
		dgBodyInfo* const bodyArray = &m_dynamics->m_bodyArray[island.m_bodyStart];
		dgJointInfo* const constraintArray = &m_dynamics->m_constraintArray[island.m_jointStart];

		dgFloat32 dist = dgFloat32 (0.0f);
		dgFloat32 maxSpeed2 = dgFloat32 (0.0f);
		for (dgInt32 k = 1; k < island.m_bodyCount; k ++) {
			dgVector veloc;
			dgVector omega;

			dgBody* const body = bodyArray[k].m_body;
			_ASSERTE (body->m_mass.m_w > dgFloat32  (0.0f));
			body->CalculateContinueVelocity (m_timestep, veloc, omega);

			dgFloat32 mag2 = veloc % veloc;
			if (mag2 > maxSpeed2) {
				maxSpeed2 = mag2;
				dist = body->m_collision->GetBoxMinRadius();
			}
		}

		_ASSERTE (dist > dgFloat32 (0.0f));
		dgInt32 steps = dgInt32 (dgFloat32 (4.0f) * dgSqrt (maxSpeed2) * m_timestep / dist) + 1;
		dgFloat32 timestep = m_timestep / dgFloat32 (steps);

		if (steps > 8) {
			steps = 8;
		}

		for (dgInt32 j = 0; j < steps - 1; j ++) {
			m_dynamics->BuildJacobianMatrix (island, m_threadIndex, timestep);
			m_system->CalculateReactionsForces (m_solverMode, DG_SOLVER_MAX_ERROR);
			m_dynamics->IntegrateArray (&m_system->m_bodyArray[1], m_system->m_bodyCount - 1, DG_SOLVER_MAX_ERROR, timestep, m_threadIndex, false);

			for (dgInt32 k = 1; k < island.m_bodyCount; k ++) {
				dgBody* const body = bodyArray[k].m_body;
				body->m_accel = body->m_prevExternalForce;
				body->m_alpha = body->m_prevExternalTorque;
			}

			for (dgInt32 k = 0; k < island.m_jointCount; k ++) {
				dgConstraint* const constraint = constraintArray[k].m_joint;
				if (constraint->GetId() == dgContactConstraintId) {
					dgContact* const contact = (dgContact*) constraint;
					const dgContactMaterial* const material = contact->m_myCacheMaterial;
					if (material->m_flags & dgContactMaterial::m_collisionEnable__) {
						if (material->m_aabbOverlap) {
							_ASSERTE (contact->m_body0);
							_ASSERTE (contact->m_body1);
							material->m_aabbOverlap (*material, *contact->m_body0, *contact->m_body1, m_threadIndex);
						}
						_ASSERTE (0);
						dgCollidingPairCollector::dgPair pair;
						pair.m_body0 = contact->m_body0;
						pair.m_body1 = contact->m_body1;
						pair.m_contact = contact;
						pair.m_material = material;
						pair.m_contactBuffer = contactBuffer;
						m_world->CalculateContacts (&pair, timestep, m_threadIndex);

						_ASSERTE (pair.m_contact);

						if (pair.m_contactCount) {
							_ASSERTE (pair.m_contactCount <= (DG_CONSTRAINT_MAX_ROWS / 3));
							m_world->ProcessContacts (&pair, timestep, m_threadIndex);
						} else if (!pair.m_contactBuffer) {
							m_world->ProcessCachedContacts (pair.m_contact, pair.m_material, timestep, m_threadIndex);
						}
					}
				}
			}
		}
		m_dynamics->BuildJacobianMatrix (island, m_threadIndex, timestep);
		m_system->CalculateReactionsForces (m_solverMode, DG_SOLVER_MAX_ERROR);
		m_dynamics->IntegrateArray (&m_system->m_bodyArray[1], m_system->m_bodyCount - 1, DG_SOLVER_MAX_ERROR, timestep, m_threadIndex, true);
	*/
	}
}



dgInt32 dgWorldDynamicUpdate::BuildJacobianMatrix (const dgIsland* const island, dgInt32 threadIndex, dgFloat32 timestep) const 
{
	_ASSERTE (island->m_bodyCount >= 2);
	//_ASSERTE (island.m_jointCount >= 1);

	dgWorld* const world = (dgWorld*) this;

	dgInt32 bodyCount = island->m_bodyCount;
	dgBodyInfo* const bodyArrayPtr = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	dgBodyInfo* const bodyArray = &bodyArrayPtr[island->m_bodyStart];

	_ASSERTE ((bodyArray[0].m_body->m_accel % bodyArray[0].m_body->m_accel) == dgFloat32 (0.0f));
	_ASSERTE ((bodyArray[0].m_body->m_alpha % bodyArray[0].m_body->m_alpha) == dgFloat32 (0.0f));

	for (dgInt32 i = 1; i < bodyCount; i ++) {
		dgBody* const body = bodyArray[i].m_body;
		_ASSERTE (body->m_invMass.m_w > dgFloat32 (0.0f));
		body->AddDamingAcceleration();
		body->CalcInvInertiaMatrix ();
	}

	dgInt32 jointCount = island->m_jointCount;
	dgJointInfo* const constraintArrayPtr = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJointInfo* const constraintArray = &constraintArrayPtr[island->m_jointStart];

	dgInt32 rowCount = 0;
	dgInt32 rowBase = dgAtomicAdd(&m_rowCountAtomicIndex, island->m_rowsCount);
	if (island->m_hasUnilateralJoints) {
		rowCount = GetJacobianDerivatives (island, threadIndex, false, rowBase, rowCount, timestep);
	}
	rowCount = GetJacobianDerivatives (island, threadIndex, true, rowBase, rowCount, timestep);
	_ASSERTE (rowCount <= island->m_rowsCount);


	//dgFloat32* const force = &m_solverMemory.m_force[rowBase];
	//const dgJacobianPair* const Jt = &m_solverMemory.m_Jt[rowBase];
	//dgJacobianPair* const JMinv = &m_solverMemory.m_JMinv[rowBase];
	//dgFloat32* const diagDamp = &m_solverMemory.m_diagDamp[rowBase];
	//dgFloat32* const extAccel = &m_solverMemory.m_deltaAccel[rowBase];
	//dgFloat32* const invDJMinvJt = &m_solverMemory.m_invDJMinvJt[rowBase];
	//dgFloat32* const coordenateAccel = &m_solverMemory.m_coordenateAccel[rowBase];
	//dgFloat32** const jointForceFeeback = &m_solverMemory.m_jointFeebackForce[rowBase];
	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowBase];

	for (dgInt32 k = 0; k < jointCount; k ++) {
		const dgJointInfo* const jointInfo = &constraintArray[k];
		dgInt32 index = jointInfo->m_autoPairstart;
		dgInt32 count = jointInfo->m_autoPaircount;
		dgInt32 m0 = jointInfo->m_m0;
		dgInt32 m1 = jointInfo->m_m1;

		_ASSERTE (m0 >= 0);
		_ASSERTE (m0 < bodyCount);
		dgBody* const body0 = bodyArray[m0].m_body;
		dgFloat32 invMass0 = body0->m_invMass[3];
		const dgMatrix& invInertia0 = body0->m_invWorldInertiaMatrix;

		_ASSERTE (m1 >= 0);
		_ASSERTE (m1 < bodyCount);
		dgBody* const body1 = bodyArray[m1].m_body;
		dgFloat32 invMass1 = body1->m_invMass[3];
		const dgMatrix& invInertia1 = body1->m_invWorldInertiaMatrix;

		for (dgInt32 i = 0; i < count; i ++) {
			dgJacobianMatrixElement* const row = &matrixRow[index];

			row->m_JMinv.m_jacobian_IM0.m_linear = row->m_Jt.m_jacobian_IM0.m_linear.Scale (invMass0);
			row->m_JMinv.m_jacobian_IM0.m_angular = invInertia0.UnrotateVector (row->m_Jt.m_jacobian_IM0.m_angular);
			dgVector tmpDiag (row->m_JMinv.m_jacobian_IM0.m_linear.CompProduct(row->m_Jt.m_jacobian_IM0.m_linear));
			tmpDiag += row->m_JMinv.m_jacobian_IM0.m_angular.CompProduct(row->m_Jt.m_jacobian_IM0.m_angular);
			dgVector tmpAccel (row->m_JMinv.m_jacobian_IM0.m_linear.CompProduct(body0->m_accel));
			tmpAccel += row->m_JMinv.m_jacobian_IM0.m_angular.CompProduct(body0->m_alpha);

			row->m_JMinv.m_jacobian_IM1.m_linear = row->m_Jt.m_jacobian_IM1.m_linear.Scale (invMass1);
			row->m_JMinv.m_jacobian_IM1.m_angular = invInertia1.UnrotateVector (row->m_Jt.m_jacobian_IM1.m_angular);
			tmpDiag += row->m_JMinv.m_jacobian_IM1.m_linear.CompProduct(row->m_Jt.m_jacobian_IM1.m_linear);
			tmpDiag += row->m_JMinv.m_jacobian_IM1.m_angular.CompProduct(row->m_Jt.m_jacobian_IM1.m_angular);
			tmpAccel += row->m_JMinv.m_jacobian_IM1.m_linear.CompProduct(body1->m_accel);
			tmpAccel += row->m_JMinv.m_jacobian_IM1.m_angular.CompProduct(body1->m_alpha);

			dgFloat32 extenalAcceleration = -(tmpAccel.m_x + tmpAccel.m_y + tmpAccel.m_z);
			row->m_deltaAccel = extenalAcceleration;
			row->m_coordenateAccel += extenalAcceleration;
			row->m_force = row->m_jointFeebackForce[0];

			//force[index] = 0.0f;
			_ASSERTE (row->m_diagDamp >= dgFloat32(0.1f));
			_ASSERTE (row->m_diagDamp <= dgFloat32(100.0f));
			dgFloat32 stiffness = DG_PSD_DAMP_TOL * row->m_diagDamp;

			dgFloat32 diag = (tmpDiag.m_x + tmpDiag.m_y + tmpDiag.m_z);
			_ASSERTE (diag > dgFloat32 (0.0f));
			row->m_diagDamp = diag * stiffness;

			diag *= (dgFloat32(1.0f) + stiffness);
			row->m_invDJMinvJt = dgFloat32(1.0f) / diag;
			index ++;
		}
	}

	return rowBase;

}


void dgWorldDynamicUpdate::CalculateReactionsForces(const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const
{
	if (island->m_jointCount == 0) {
		ApplyExternalForcesAndAcceleration (island, rowStart, threadIndex, timestep, 0.0f);
		return;
	}

	if (island->m_jointCount == 1) {
		CalculateSimpleBodyReactionsForces (island, rowStart, threadIndex, timestep, maxAccNorm);
		ApplyExternalForcesAndAcceleration (island, rowStart, threadIndex, timestep, maxAccNorm * dgFloat32 (0.001f));
		return;
	}

	dgWorld* const world = (dgWorld*) this;
	if (world->m_solverMode) {
		CalculateForcesGameMode (island, rowStart, threadIndex, timestep, maxAccNorm);
	} else {
		CalculateForcesSimulationMode (island, rowStart, threadIndex, timestep, maxAccNorm);
	}
}





void dgWorldDynamicUpdate::ApplyExternalForcesAndAcceleration(const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const
{
//	dgFloat32* const force = &m_solverMemory.m_force[rowStart];
//	const dgJacobianPair* const Jt = &m_solverMemory.m_Jt[rowStart];
//	dgFloat32** const jointForceFeeback = &m_solverMemory.m_jointFeebackForce[rowStart];
	dgJacobian* const internalForces = &m_solverMemory.m_internalForces[island->m_bodyStart];

	dgVector zero (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgInt32 bodyCount = island->m_bodyCount;
	for (dgInt32 i = 0; i < bodyCount; i ++) {
		internalForces[i].m_linear = zero;
		internalForces[i].m_angular = zero;
	}

	dgInt32 hasJointFeeback = 0;
	dgInt32 jointCount = island->m_jointCount;
	dgWorld* const world = (dgWorld*) this;
	dgJointInfo* const constraintArrayPtr = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJointInfo* const constraintArray = &constraintArrayPtr[island->m_jointStart];

	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowStart];
	for (dgInt32 i = 0; i < jointCount; i ++) {
		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPaircount;

		dgInt32 m0 = constraintArray[i].m_m0;
		dgInt32 m1 = constraintArray[i].m_m1;

		dgJacobian y0;
		dgJacobian y1;
		y0.m_linear = zero;
		y0.m_angular = zero;
		y1.m_linear = zero;
		y1.m_angular = zero;

		for (dgInt32 j = 0; j < count; j ++) { 
			//dgInt32 index = j + first;
			dgJacobianMatrixElement* const row = &matrixRow[j + first];
			dgFloat32 val = row->m_force; 

			_ASSERTE (dgCheckFloat(val));
			row->m_jointFeebackForce[0] = val;

			y0.m_linear += row->m_Jt.m_jacobian_IM0.m_linear.Scale (val);
			y0.m_angular += row->m_Jt.m_jacobian_IM0.m_angular.Scale (val);
			y1.m_linear += row->m_Jt.m_jacobian_IM1.m_linear.Scale (val);
			y1.m_angular += row->m_Jt.m_jacobian_IM1.m_angular.Scale (val);
		}

		//if (constraintArray[i].m_joint->GetId() == dgContactConstraintId) {
		//		m_world->AddToBreakQueue ((dgContact*)constraintArray[i].m_joint, maxForce);
		//}
		hasJointFeeback |= (constraintArray[i].m_joint->m_updaFeedbackCallback ? 1 : 0);

		internalForces[m0].m_linear += y0.m_linear;
		internalForces[m0].m_angular += y0.m_angular;
		internalForces[m1].m_linear += y1.m_linear;
		internalForces[m1].m_angular += y1.m_angular;
	}

	dgFloat32 accelTol2 = maxAccNorm * maxAccNorm;
	dgBodyInfo* const bodyArrayPtr = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	dgBodyInfo* const bodyArray = &bodyArrayPtr[island->m_bodyStart];
	for (dgInt32 i = 1; i < bodyCount; i ++) {
		dgBody* const body = bodyArray[i].m_body;
		body->m_accel += internalForces[i].m_linear;
		body->m_alpha += internalForces[i].m_angular;

		dgVector accel (body->m_accel.Scale (body->m_invMass.m_w));
		dgVector alpha (body->m_invWorldInertiaMatrix.RotateVector (body->m_alpha));
		dgFloat32 error = accel % accel;
		if (error < accelTol2) {
			accel = zero;
			body->m_accel = zero;
		}

		error = alpha % alpha;
		if (error < accelTol2) {
			alpha = zero;
			body->m_alpha = zero;
		}

		body->m_netForce = body->m_accel;
		body->m_netTorque = body->m_alpha;

		body->m_veloc += accel.Scale(timestep);
		body->m_omega += alpha.Scale(timestep);
	}

	if (hasJointFeeback) {
		for (dgInt32 i = 0; i < jointCount; i ++) {
			if (constraintArray[i].m_joint->m_updaFeedbackCallback) {
				constraintArray[i].m_joint->m_updaFeedbackCallback (*constraintArray[i].m_joint, timestep, threadIndex);
			}
		}
	}

}


void dgWorldDynamicUpdate::CalculateSimpleBodyReactionsForces (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const
{
	dgFloat32 accel[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 activeRow[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 lowBound[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 highBound[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 deltaForce[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 deltaAccel[DG_CONSTRAINT_MAX_ROWS];


	dgWorld* const world = (dgWorld*) this;
	dgJointInfo* const constraintArrayPtr = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJointInfo* const constraintArray = &constraintArrayPtr[island->m_jointStart];
	dgInt32 count = constraintArray[0].m_autoPaircount;
	_ASSERTE (constraintArray[0].m_autoPairstart == 0);

	dgInt32 maxPasses = count;
	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowStart];
	for (dgInt32 i = 0; i < count; i ++) {
		dgJacobianMatrixElement* const row = &matrixRow[i];

		dgInt32 k = row->m_normalForceIndex;
		_ASSERTE (((k <0) && (matrixRow[k].m_force == dgFloat32 (1.0f))) || ((k >= 0) && (matrixRow[k].m_force >= dgFloat32 (0.0f))));
		dgFloat32 val = matrixRow[k].m_force;
		lowBound[i] = val * row->m_lowerBoundFrictionCoefficent;
		highBound[i] = val * row->m_upperBoundFrictionCoefficent;

		activeRow[i] = dgFloat32 (1.0f);
		if (row->m_force < lowBound[i]) {
			maxPasses --;
			row->m_force = lowBound[i];
			activeRow[i] = dgFloat32 (0.0f);
		} else if (row->m_force > highBound[i]) {
			maxPasses --;
			row->m_force = highBound[i];
			activeRow[i] = dgFloat32 (0.0f);
		}
	}

	dgJacobian y0;
	dgJacobian y1;
	dgVector zero (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	y0.m_linear = zero;
	y0.m_angular = zero;
	y1.m_linear = zero;
	y1.m_angular = zero;
	for (dgInt32 i = 0; i < count; i ++) {
		dgJacobianMatrixElement* const row = &matrixRow[i];
		dgFloat32 val = row->m_force; 
		y0.m_linear += row->m_Jt.m_jacobian_IM0.m_linear.Scale (val);
		y0.m_angular += row->m_Jt.m_jacobian_IM0.m_angular.Scale (val);
		y1.m_linear += row->m_Jt.m_jacobian_IM1.m_linear.Scale (val);
		y1.m_angular += row->m_Jt.m_jacobian_IM1.m_angular.Scale (val);
	}

	dgFloat32 akNum = dgFloat32 (0.0f);
	dgFloat32 accNorm = dgFloat32(0.0f);
	for (dgInt32 i = 0; i < count; i ++) {
		dgJacobianMatrixElement* const row = &matrixRow[i];

		dgVector acc (row->m_JMinv.m_jacobian_IM0.m_linear.CompProduct(y0.m_linear));
		acc += row->m_JMinv.m_jacobian_IM0.m_angular.CompProduct (y0.m_angular);
		acc += row->m_JMinv.m_jacobian_IM1.m_linear.CompProduct (y1.m_linear);
		acc += row->m_JMinv.m_jacobian_IM1.m_angular.CompProduct (y1.m_angular);
		accel[i] = row->m_coordenateAccel - acc.m_x - acc.m_y - acc.m_z - row->m_force * row->m_diagDamp;

		deltaForce[i] = accel[i] * row->m_invDJMinvJt * activeRow[i];
		akNum += accel[i] * deltaForce[i];
		accNorm = GetMax (dgAbsf (accel[i] * activeRow[i]), accNorm);
	}


	
	for (dgInt32 i = 0; (i < maxPasses) && (accNorm > maxAccNorm); i ++) {
		y0.m_linear = zero;
		y0.m_angular = zero;
		y1.m_linear = zero;
		y1.m_angular = zero;
		for (dgInt32 k = 0; k < count; k ++) {
			dgJacobianMatrixElement* const row = &matrixRow[k];
			dgFloat32 val = deltaForce[k]; 
			y0.m_linear += row->m_Jt.m_jacobian_IM0.m_linear.Scale (val);
			y0.m_angular += row->m_Jt.m_jacobian_IM0.m_angular.Scale (val);
			y1.m_linear += row->m_Jt.m_jacobian_IM1.m_linear.Scale (val);
			y1.m_angular += row->m_Jt.m_jacobian_IM1.m_angular.Scale (val);
		}

		dgFloat32 akDen = dgFloat32 (0.0f);
		for (dgInt32 k = 0; k < count; k ++) {
			dgJacobianMatrixElement* const row = &matrixRow[k];
			dgVector acc (row->m_JMinv.m_jacobian_IM0.m_linear.CompProduct(y0.m_linear));
			acc += row->m_JMinv.m_jacobian_IM0.m_angular.CompProduct(y0.m_angular);
			acc += row->m_JMinv.m_jacobian_IM1.m_linear.CompProduct(y1.m_linear);
			acc += row->m_JMinv.m_jacobian_IM1.m_angular.CompProduct(y1.m_angular);

			//m_accel[i0] = m_coordenateAccel[i0] - acc.m_x - acc.m_y - acc.m_z - m_force[i0] * m_diagDamp[i0];
			deltaAccel[k] = acc.m_x + acc.m_y + acc.m_z + deltaForce[k] * row->m_diagDamp;
			akDen += deltaAccel[k] * deltaForce[k];
		}


		_ASSERTE (akDen > dgFloat32 (0.0f));
		akDen = GetMax (akDen, dgFloat32(1.0e-16f));
		_ASSERTE (dgAbsf (akDen) >= dgFloat32(1.0e-16f));
		dgFloat32 ak = akNum / akDen;

		dgInt32 clampedForceIndex = -1;
		dgFloat32 clampedForceIndexValue = dgFloat32(0.0f);
		for (dgInt32 k = 0; k < count; k ++) {
			if (activeRow[k]) {
				dgJacobianMatrixElement* const row = &matrixRow[k];
				dgFloat32 val = row->m_force + ak * deltaForce[k];
				if (deltaForce[k] < dgFloat32 (-1.0e-16f)) {
					if (val < lowBound[k]) {
						ak = GetMax ((lowBound[k] - row->m_force) / deltaForce[k], dgFloat32 (0.0f));
						clampedForceIndex = k;
						clampedForceIndexValue = lowBound[k];
						if (ak < dgFloat32 (1.0e-8f)) {
							ak = dgFloat32 (0.0f);
							break;
						}
					}
				} else if (deltaForce[k] > dgFloat32 (1.0e-16f)) {
					if (val >= highBound[k]) {
						ak = GetMax ((highBound[k] - row->m_force) / deltaForce[k], dgFloat32 (0.0f));;
						clampedForceIndex = k;
						clampedForceIndexValue = highBound[k];
						if (ak < dgFloat32 (1.0e-8f)) {
							ak = dgFloat32 (0.0f);
							break;
						}
					}
				}
			}
		}

		if (ak == dgFloat32 (0.0f) && (clampedForceIndex != -1)) {
			_ASSERTE (clampedForceIndex !=-1);
			akNum = dgFloat32 (0.0f);
			accNorm = dgFloat32(0.0f);

			activeRow[clampedForceIndex] = dgFloat32 (0.0f);
			deltaForce[clampedForceIndex] = dgFloat32 (0.0f);
			matrixRow[clampedForceIndex].m_force = clampedForceIndexValue;
			for (dgInt32 k = 0; k < count; k ++) {
				if (activeRow[k]) {
					dgJacobianMatrixElement* const row = &matrixRow[k];
					dgFloat32 val = lowBound[k] - row->m_force;
					if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (accel[k] < dgFloat32 (0.0f))) {
						row->m_force = lowBound[k];
						activeRow[k] = dgFloat32 (0.0f);
						deltaForce[k] = dgFloat32 (0.0f); 

					} else {
						val = highBound[k] - row->m_force;
						if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (accel[k] > dgFloat32 (0.0f))) {
							row->m_force = highBound[k];
							activeRow[k] = dgFloat32 (0.0f);
							deltaForce[k] = dgFloat32 (0.0f); 
						} else {
							_ASSERTE (activeRow[k] > dgFloat32 (0.0f));
							deltaForce[k] = accel[k] * row->m_invDJMinvJt;
							akNum += accel[k] * deltaForce[k];
							accNorm = GetMax (dgAbsf (accel[k]), accNorm);
						}
					}
				}
			}


			i = -1;
			maxPasses = GetMax (maxPasses - 1, 1); 

		} else if (clampedForceIndex >= 0) {
			akNum = dgFloat32(0.0f);
			accNorm = dgFloat32(0.0f);
			activeRow[clampedForceIndex] = dgFloat32 (0.0f);
			for (dgInt32 k = 0; k < count; k ++) {
				dgJacobianMatrixElement* const row = &matrixRow[k];
				row->m_force += ak * deltaForce[k];
				accel[k] -= ak * deltaAccel[k];
				accNorm = GetMax (dgAbsf (accel[k] * activeRow[k]), accNorm);
				_ASSERTE (dgCheckFloat(row->m_force));
				_ASSERTE (dgCheckFloat(accel[k]));

				deltaForce[k] = accel[k] * row->m_invDJMinvJt * activeRow[k];
				akNum += deltaForce[k] * accel[k];
			}
			matrixRow[clampedForceIndex].m_force = clampedForceIndexValue;

			i = -1;
			maxPasses = GetMax (maxPasses - 1, 1); 

		} else {
			accNorm = dgFloat32(0.0f);
			for (dgInt32 k = 0; k < count; k ++) {
				dgJacobianMatrixElement* const row = &matrixRow[k];
				row->m_force += ak * deltaForce[k];
				accel[k] -= ak * deltaAccel[k];
				accNorm = GetMax (dgAbsf (accel[k] * activeRow[k]), accNorm);
				_ASSERTE (dgCheckFloat(row->m_force));
				_ASSERTE (dgCheckFloat(accel[k]));
			}

			if (accNorm > maxAccNorm) {

				akDen = akNum;
				akNum = dgFloat32(0.0f);
				for (dgInt32 k = 0; k < count; k ++) {
					deltaAccel[k] = accel[k] * matrixRow[k].m_invDJMinvJt * activeRow[k];
					akNum += accel[k] * deltaAccel[k];
				}

				_ASSERTE (akDen > dgFloat32(0.0f));
				akDen = GetMax (akDen, dgFloat32 (1.0e-17f));
				ak = dgFloat32 (akNum / akDen);
				for (dgInt32 k = 0; k < count; k ++) {
					deltaForce[k] = deltaAccel[k] + ak * deltaForce[k];
				}
			}
		}
	}
}


void dgWorldDynamicUpdate::CalculateForcesGameMode (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestepSrc, dgFloat32 maxAccNorm) const
{
	dgJacobian* const internalVeloc = &m_solverMemory.m_internalVeloc[island->m_bodyStart];
	dgJacobian* const internalForces = &m_solverMemory.m_internalForces[island->m_bodyStart];
	dgVector zero(dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));

	//	sleepCount = 0;
	dgWorld* const world = (dgWorld*) this;
	dgInt32 bodyCount = island->m_bodyCount;
	dgBodyInfo* const bodyArrayPtr = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	dgBodyInfo* const bodyArray = &bodyArrayPtr[island->m_bodyStart];
	for (dgInt32 i = 1; i < bodyCount; i ++) {
		dgBody* const body = bodyArray[i].m_body;

		body->m_netForce = body->m_veloc;
		body->m_netTorque = body->m_omega;
		internalVeloc[i].m_linear = zero;
		internalVeloc[i].m_angular = zero;
		internalForces[i].m_linear = zero;
		internalForces[i].m_angular = zero;
	}

	internalVeloc[0].m_linear = zero;
	internalVeloc[0].m_angular = zero;
	internalForces[0].m_linear = zero;
	internalForces[0].m_angular = zero;

	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowStart];

	dgInt32 jointCount = island->m_jointCount;
	dgJointInfo* const constraintArrayPtr = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJointInfo* const constraintArray = &constraintArrayPtr[island->m_jointStart];
	for (dgInt32 i = 0; i < jointCount; i ++) {
		dgJacobian y0;
		dgJacobian y1;
		y0.m_linear = zero;
		y0.m_angular = zero;
		y1.m_linear = zero;
		y1.m_angular = zero;
		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPaircount;
		for (dgInt32 j = 0; j < count; j ++) { 
			dgJacobianMatrixElement* const row = &matrixRow[j + first];
			dgFloat32 val = row->m_force; 
			_ASSERTE (dgCheckFloat(val));
			y0.m_linear += row->m_Jt.m_jacobian_IM0.m_linear.Scale (val);
			y0.m_angular += row->m_Jt.m_jacobian_IM0.m_angular.Scale (val);
			y1.m_linear += row->m_Jt.m_jacobian_IM1.m_linear.Scale (val);
			y1.m_angular += row->m_Jt.m_jacobian_IM1.m_angular.Scale (val);
		}
		dgInt32 m0 = constraintArray[i].m_m0;
		dgInt32 m1 = constraintArray[i].m_m1;
		internalForces[m0].m_linear += y0.m_linear;
		internalForces[m0].m_angular += y0.m_angular;
		internalForces[m1].m_linear += y1.m_linear;
		internalForces[m1].m_angular += y1.m_angular;
	}

	dgFloat32 invTimestepSrc = dgFloat32 (1.0f) / timestepSrc;
	dgFloat32 invStep = (dgFloat32 (1.0f) / dgFloat32 (LINEAR_SOLVER_SUB_STEPS));
	dgFloat32 timestep =  timestepSrc * invStep;
	dgFloat32 invTimestep = invTimestepSrc * dgFloat32 (LINEAR_SOLVER_SUB_STEPS);
	_ASSERTE (bodyArray[0].m_body == world->m_sentionelBody);

	dgFloat32 firstPassCoef = dgFloat32 (0.0f);
	dgInt32 maxPasses = dgInt32 (world->m_solverMode + DG_BASE_ITERATION_COUNT);
	for (dgInt32 step = 0; step < LINEAR_SOLVER_SUB_STEPS; step ++) {
		dgJointAccelerationDecriptor joindDesc;
		joindDesc.m_timeStep = timestep;
		joindDesc.m_invTimeStep = invTimestep;
		joindDesc.m_firstPassCoefFlag = firstPassCoef;
		for (dgInt32 curJoint = 0; curJoint < jointCount; curJoint ++) {
			//dgInt32 index;
			//index = constraintArray[curJoint].m_autoPairstart;
			//joindDesc.m_Jt = &row->m_Jt;
			//joindDesc.m_penetration = &row->m_penetration;
			//joindDesc.m_restitution = &row->m_restitution;
			//joindDesc.m_externAccelaration = &row->m_deltaAccel;
			//joindDesc.m_coordenateAccel = &row->m_coordenateAccel;
			//joindDesc.m_accelIsMotor = &row->m_accelIsMotor;
			//joindDesc.m_normalForceIndex = &row->m_normalForceIndex;
			//joindDesc.m_penetrationStiffness = &row->m_penetrationStiffness;
			joindDesc.m_rowsCount = constraintArray[curJoint].m_autoPaircount;
			joindDesc.m_rowMatrix = &matrixRow[constraintArray[curJoint].m_autoPairstart];
			constraintArray[curJoint].m_joint->JointAccelerations (&joindDesc);
		}
		firstPassCoef = dgFloat32 (1.0f);

		dgFloat32 accNorm = maxAccNorm * dgFloat32 (2.0f);
		for (dgInt32 passes = 0; (passes < maxPasses) && (accNorm > maxAccNorm); passes ++) {
			accNorm = dgFloat32 (0.0f);
			for (dgInt32 curJoint = 0; curJoint < jointCount; curJoint ++) {
				dgInt32 index = constraintArray[curJoint].m_autoPairstart;
				dgInt32 rowsCount = constraintArray[curJoint].m_autoPaircount;
				dgInt32 m0 = constraintArray[curJoint].m_m0;
				dgInt32 m1 = constraintArray[curJoint].m_m1;

				dgVector linearM0 (internalForces[m0].m_linear);
				dgVector angularM0 (internalForces[m0].m_angular);
				dgVector linearM1 (internalForces[m1].m_linear);
				dgVector angularM1 (internalForces[m1].m_angular);

				for (dgInt32 k = 0; k < rowsCount; k ++) {
					dgJacobianMatrixElement* const row = &matrixRow[index];

					dgVector acc (row->m_JMinv.m_jacobian_IM0.m_linear.CompProduct(linearM0));
					acc += row->m_JMinv.m_jacobian_IM0.m_angular.CompProduct (angularM0);
					acc += row->m_JMinv.m_jacobian_IM1.m_linear.CompProduct (linearM1);
					acc += row->m_JMinv.m_jacobian_IM1.m_angular.CompProduct (angularM1);

					dgFloat32 a = row->m_coordenateAccel - acc.m_x - acc.m_y - acc.m_z - row->m_force * row->m_diagDamp;
					dgFloat32 f = row->m_force + row->m_invDJMinvJt * a;

					dgInt32 frictionIndex = row->m_normalForceIndex;
					_ASSERTE (((frictionIndex < 0) && (matrixRow[frictionIndex].m_force == dgFloat32 (1.0f))) || ((frictionIndex >= 0) && (matrixRow[frictionIndex].m_force >= dgFloat32 (0.0f))));

					dgFloat32 frictionNormal = matrixRow[frictionIndex].m_force;
					dgFloat32 lowerFrictionForce = frictionNormal * row->m_lowerBoundFrictionCoefficent;
					dgFloat32 upperFrictionForce = frictionNormal * row->m_upperBoundFrictionCoefficent;

					if (f > upperFrictionForce) {
						a = dgFloat32 (0.0f);
						f = upperFrictionForce;
					} else if (f < lowerFrictionForce) {
						a = dgFloat32 (0.0f);
						f = lowerFrictionForce;
					}

					accNorm = GetMax (accNorm, dgAbsf (a));
					dgFloat32 prevValue = f - row->m_force;
					row->m_force = f;

					linearM0 += row->m_Jt.m_jacobian_IM0.m_linear.Scale (prevValue);
					angularM0 += row->m_Jt.m_jacobian_IM0.m_angular.Scale (prevValue);
					linearM1 += row->m_Jt.m_jacobian_IM1.m_linear.Scale (prevValue);
					angularM1 += row->m_Jt.m_jacobian_IM1.m_angular.Scale (prevValue);
					index ++;
				}
				internalForces[m0].m_linear = linearM0;
				internalForces[m0].m_angular = angularM0;
				internalForces[m1].m_linear = linearM1;
				internalForces[m1].m_angular = angularM1;
			}
//accNorm  = 1.0f;
		}

		for (dgInt32 i = 1; i < bodyCount; i ++) {
			dgBody* const body = bodyArray[i].m_body;
			dgVector force (body->m_accel + internalForces[i].m_linear);
			dgVector torque (body->m_alpha + internalForces[i].m_angular);

			dgVector accel (force.Scale (body->m_invMass.m_w));
			dgVector alpha (body->m_invWorldInertiaMatrix.RotateVector (torque));
			body->m_veloc += accel.Scale(timestep);
			body->m_omega += alpha.Scale(timestep);
			internalVeloc[i].m_linear += body->m_veloc;
			internalVeloc[i].m_angular += body->m_omega;
		}
	}

	dgInt32 hasJointFeeback = 0;
	for (dgInt32 i = 0; i < jointCount; i ++) {
		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPaircount;

		for (dgInt32 j = 0; j < count; j ++) { 
			//dgInt32 index = j + first;
			dgJacobianMatrixElement* const row = &matrixRow[j + first];
			dgFloat32 val = row->m_force; 
			//maxForce = GetMax (dgAbsf (val), maxForce);
			_ASSERTE (dgCheckFloat(val));
			row->m_jointFeebackForce[0] = val;
		}
		//if (constraintArray[i].m_joint->GetId() == dgContactConstraintId) {
		//	m_world->AddToBreakQueue ((dgContact*)constraintArray[i].m_joint, maxForce);
		//}

		hasJointFeeback |= (constraintArray[i].m_joint->m_updaFeedbackCallback ? 1 : 0);
	}


	dgFloat32 maxAccNorm2 = maxAccNorm * maxAccNorm;
	for (dgInt32 i = 1; i < bodyCount; i ++) {
		dgBody* const body = bodyArray[i].m_body;

#ifdef DG_WIGHT_FINAL_RK4_DERIVATIVES
		body->m_veloc = internalVeloc[i].m_linear.Scale(invStep);
		body->m_omega = internalVeloc[i].m_angular.Scale(invStep);
#endif

		dgVector accel = (body->m_veloc - body->m_netForce).Scale (invTimestepSrc);
		dgVector alpha = (body->m_omega - body->m_netTorque).Scale (invTimestepSrc);
		if ((accel % accel) < maxAccNorm2) {
			accel = zero;
		}

		if ((alpha % alpha) < maxAccNorm2) {
			alpha = zero;
		}

		body->m_accel = accel;
		body->m_alpha = alpha;
		body->m_netForce = accel.Scale (body->m_mass[3]);

		alpha = body->m_matrix.UnrotateVector(alpha);
		body->m_netTorque = body->m_matrix.RotateVector (alpha.CompProduct(body->m_mass));
	}

	if (hasJointFeeback) {
		for (dgInt32 i = 0; i < jointCount; i ++) {
			if (constraintArray[i].m_joint->m_updaFeedbackCallback) {
				constraintArray[i].m_joint->m_updaFeedbackCallback (*constraintArray[i].m_joint, timestepSrc, threadIndex);
			}
		}
	}
}


dgFloat32 dgWorldDynamicUpdate::CalculateJointForces (const dgIsland* const island, dgInt32 rowStart, dgInt32 joint, dgFloat32* const forceStep, dgFloat32 maxAccNorm) const
{
	dgFloat32 deltaAccel[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 deltaForce[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 activeRow[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 lowBound[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 highBound[DG_CONSTRAINT_MAX_ROWS];

	dgWorld* const world = (dgWorld*) this;
	dgJointInfo* const constraintArrayPtr = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJointInfo* const constraintArray = &constraintArrayPtr[island->m_jointStart];

	const dgJointInfo* const jointInfo = &constraintArray[joint];
	dgInt32 first = jointInfo->m_autoPairstart;
	dgInt32 count = jointInfo->m_autoPaircount;

	dgInt32 maxPasses = count;
	dgFloat32 akNum = dgFloat32 (0.0f);
	dgFloat32 accNorm = dgFloat32(0.0f);

	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowStart];
	for (dgInt32 j = 0; j < count; j ++) {
		dgJacobianMatrixElement* const row = &matrixRow[first + j];
		dgInt32 k = row->m_normalForceIndex;

		//_ASSERTE (((k <0) && (force[k] == dgFloat32 (1.0f))) || ((k >= 0) && (force[k] >= dgFloat32 (0.0f))));
		_ASSERTE (((k <0) && (matrixRow[k].m_force == dgFloat32 (1.0f))) || ((k >= 0) && (matrixRow[k].m_force >= dgFloat32 (0.0f))));
		dgFloat32 val = matrixRow[k].m_force;
		lowBound[j] = val * row->m_lowerBoundFrictionCoefficent;
		highBound[j] = val * row->m_upperBoundFrictionCoefficent;

		activeRow[j] = dgFloat32 (1.0f);
		forceStep[j] = row->m_force;
		if (row->m_force < lowBound[j]) {
			maxPasses --;
			row->m_force = lowBound[j];
			activeRow[j] = dgFloat32 (0.0f);
		} else if (row->m_force > highBound[j]) {
			maxPasses --;
			row->m_force = highBound[j];
			activeRow[j] = dgFloat32 (0.0f);
		}

		deltaForce[j] = row->m_accel * row->m_invDJMinvJt * activeRow[j];
		akNum += row->m_accel * deltaForce[j];
		accNorm = GetMax (dgAbsf (row->m_accel * activeRow[j]), accNorm);
	}

	dgFloat32 retAccel = accNorm;
	dgFloat32 clampedForceIndexValue = dgFloat32(0.0f);
	dgVector zero (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	for (dgInt32 i = 0; (i < maxPasses) && (accNorm >  maxAccNorm); i ++) {
		dgJacobian y0;
		dgJacobian y1;
		y0.m_linear = zero;
		y0.m_angular = zero;
		y1.m_linear = zero;
		y1.m_angular = zero;

		for (dgInt32 j = 0; j < count; j ++) {
			dgJacobianMatrixElement* const row = &matrixRow[first + j];
			dgFloat32 val = deltaForce[j]; 
			y0.m_linear += row->m_Jt.m_jacobian_IM0.m_linear.Scale (val);
			y0.m_angular += row->m_Jt.m_jacobian_IM0.m_angular.Scale (val);
			y1.m_linear += row->m_Jt.m_jacobian_IM1.m_linear.Scale (val);
			y1.m_angular += row->m_Jt.m_jacobian_IM1.m_angular.Scale (val);

		}

		dgFloat32 akDen = dgFloat32 (0.0f);
		for (dgInt32 j = 0; j < count; j ++) {
			dgJacobianMatrixElement* const row = &matrixRow[first + j];
			dgVector acc (row->m_JMinv.m_jacobian_IM0.m_linear.CompProduct(y0.m_linear));
			acc += row->m_JMinv.m_jacobian_IM0.m_angular.CompProduct(y0.m_angular);
			acc += row->m_JMinv.m_jacobian_IM1.m_linear.CompProduct(y1.m_linear);
			acc += row->m_JMinv.m_jacobian_IM1.m_angular.CompProduct(y1.m_angular);

			deltaAccel[j] = acc.m_x + acc.m_y + acc.m_z + deltaForce[j] * row->m_diagDamp;
			akDen += deltaAccel[j] * deltaForce[j];
		}
		_ASSERTE (akDen > dgFloat32 (0.0f));
		akDen = GetMax (akDen, dgFloat32(1.0e-16f));
		_ASSERTE (dgAbsf (akDen) >= dgFloat32(1.0e-16f));
		dgFloat32 ak = akNum / akDen;

		dgInt32 clampedForceIndex = -1;
		for (dgInt32 j = 0; j < count; j ++) {
			if (activeRow[j]) {
				dgJacobianMatrixElement* const row = &matrixRow[first + j];
				if (deltaForce[j] < dgFloat32 (-1.0e-16f)) {
					dgFloat32 val = row->m_force + ak * deltaForce[j];
					if (val < lowBound[j]) {
						ak = GetMax ((lowBound[j] - row->m_force) / deltaForce[j], dgFloat32 (0.0f));
						clampedForceIndex = j;
						clampedForceIndexValue = lowBound[j];
						if (ak < dgFloat32 (1.0e-8f)) {
							ak = dgFloat32 (0.0f);
							break;
						}
					}
				} else if (deltaForce[j] > dgFloat32 (1.0e-16f)) {
					dgFloat32 val = row->m_force + ak * deltaForce[j];
					if (val > highBound[j]) {
						ak = GetMax ((highBound[j] - row->m_force) / deltaForce[j], dgFloat32 (0.0f));
						clampedForceIndex = j;
						clampedForceIndexValue = highBound[j];
						if (ak < dgFloat32 (1.0e-8f)) {
							ak = dgFloat32 (0.0f);
							break;
						}
					}
				}
			}
		}

		if (ak == dgFloat32 (0.0f) && (clampedForceIndex != -1)) {
			_ASSERTE (clampedForceIndex !=-1);
			akNum = dgFloat32 (0.0f);
			accNorm = dgFloat32(0.0f);

			activeRow[clampedForceIndex] = dgFloat32 (0.0f);
			deltaForce[clampedForceIndex] = dgFloat32 (0.0f);
			matrixRow[clampedForceIndex].m_force = clampedForceIndexValue;
			for (dgInt32 j = 0; j < count; j ++) {
				if (activeRow[j]) {
					dgJacobianMatrixElement* const row = &matrixRow[first + j];
					dgFloat32 val = lowBound[j] - row->m_force;
					if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (row->m_accel < dgFloat32 (0.0f))) {
						row->m_force = lowBound[j];
						activeRow[j] = dgFloat32 (0.0f);
						deltaForce[j] = dgFloat32 (0.0f); 

					} else {
						val = highBound[j] - row->m_force;
						if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (row->m_accel > dgFloat32 (0.0f))) {
							row->m_force = highBound[j];
							activeRow[j] = dgFloat32 (0.0f);
							deltaForce[j] = dgFloat32 (0.0f); 
						} else {
							_ASSERTE (activeRow[j] > dgFloat32 (0.0f));
							deltaForce[j] = row->m_accel * row->m_invDJMinvJt;
							akNum += row->m_accel * deltaForce[j];
							accNorm = GetMax (dgAbsf (row->m_accel), accNorm);
						}
					}
				}
			}

			_ASSERTE (activeRow[clampedForceIndex] == dgFloat32 (0.0f));
			i = -1;
			maxPasses = GetMax (maxPasses - 1, 1); 

		} else if (clampedForceIndex >= 0) {
			akNum = dgFloat32(0.0f);
			accNorm = dgFloat32(0.0f);
			activeRow[clampedForceIndex] = dgFloat32 (0.0f);
			for (dgInt32 j = 0; j < count; j ++) {
				dgJacobianMatrixElement* const row = &matrixRow[first + j];
				row->m_force += ak * deltaForce[j];
				row->m_accel -= ak * deltaAccel[j];
				accNorm = GetMax (dgAbsf (row->m_accel * activeRow[j]), accNorm);
				_ASSERTE (dgCheckFloat(row->m_force));
				_ASSERTE (dgCheckFloat(row->m_accel));

				deltaForce[j] = row->m_accel * row->m_invDJMinvJt * activeRow[j];
				akNum += deltaForce[j] * row->m_accel;
			}
			//force[clampedForceIndex + first] = clampedForceIndexValue;
			matrixRow[clampedForceIndex + first].m_force = clampedForceIndexValue;

			i = -1;
			maxPasses = GetMax (maxPasses - 1, 1); 

		} else {
			accNorm = dgFloat32(0.0f);
			for (dgInt32 j = 0; j < count; j ++) {
				dgJacobianMatrixElement* const row = &matrixRow[first + j];
				row->m_force += ak * deltaForce[j];
				row->m_accel -= ak * deltaAccel[j];
				accNorm = GetMax (dgAbsf (row->m_accel * activeRow[j]), accNorm);
				_ASSERTE (dgCheckFloat(row->m_force));
				_ASSERTE (dgCheckFloat(row->m_accel));
			}

			if (accNorm > maxAccNorm) {
				akDen = akNum;
				akNum = dgFloat32(0.0f);
				for (dgInt32 j = 0; j < count; j ++) {
					dgJacobianMatrixElement* const row = &matrixRow[first + j];
					deltaAccel[j] = row->m_accel * row->m_invDJMinvJt * activeRow[j];
					akNum += row->m_accel * deltaAccel[j];
				}

				_ASSERTE (akDen > dgFloat32(0.0f));
				akDen = GetMax (akDen, dgFloat32 (1.0e-17f));
				ak = dgFloat32 (akNum / akDen);
				for (dgInt32 j = 0; j < count; j ++) {
					deltaForce[j] = deltaAccel[j] + ak * deltaForce[j];
				}
			}
		}
	}

	for (dgInt32 j = 0; j < count; j ++) {
		dgJacobianMatrixElement* const row = &matrixRow[first + j];
		forceStep[j] = row->m_force - forceStep[j];
	}
	return retAccel;
}


void dgWorldDynamicUpdate::CalculateForcesSimulationMode (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const
{
	dgFloat32 forceStep[DG_CONSTRAINT_MAX_ROWS];	

	dgWorld* const world = (dgWorld*) this;
	dgJacobian* const internalForces = &m_solverMemory.m_internalForces[island->m_bodyStart];

	dgInt32 bodyCount = island->m_bodyCount;
	dgInt32 jointCount = island->m_jointCount;

	// initialize the intermediate force accumulation to zero 
	dgVector zero(dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	for (dgInt32 i = 0; i < bodyCount; i ++) {
		internalForces[i].m_linear = zero;
		internalForces[i].m_angular = zero;
	}

	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowStart];
	dgJointInfo* const constraintArrayPtr = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJointInfo* const constraintArray = &constraintArrayPtr[island->m_jointStart];
	for (dgInt32 i = 0; i < jointCount; i ++) {
		dgJacobian y0;
		dgJacobian y1;
		y0.m_linear = zero;
		y0.m_angular = zero;
		y1.m_linear = zero;
		y1.m_angular = zero;

		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPairActiveCount;
		for (dgInt32 j = 0; j < count; j ++) {
			dgJacobianMatrixElement* const row = &matrixRow[j + first];
			dgFloat32 val = row->m_force; 
			_ASSERTE (dgCheckFloat(val));
			y0.m_linear += row->m_Jt.m_jacobian_IM0.m_linear.Scale (val);
			y0.m_angular += row->m_Jt.m_jacobian_IM0.m_angular.Scale (val);
			y1.m_linear += row->m_Jt.m_jacobian_IM1.m_linear.Scale (val);
			y1.m_angular += row->m_Jt.m_jacobian_IM1.m_angular.Scale (val);
		}

		dgInt32 m0 = constraintArray[i].m_m0;
		dgInt32 m1 = constraintArray[i].m_m1;
		internalForces[m0].m_linear += y0.m_linear;
		internalForces[m0].m_angular += y0.m_angular;
		internalForces[m1].m_linear += y1.m_linear;
		internalForces[m1].m_angular += y1.m_angular;
	}

	for (dgInt32 i = 0; i < DG_CONSTRAINT_MAX_ROWS; i ++) {
		forceStep[i] = dgFloat32 (0.0f);
	}


	dgInt32 maxPasses = 4;
	dgInt32 prevJoint = 0;
	dgFloat32 accNorm = maxAccNorm * dgFloat32 (2.0f);
	for (dgInt32 passes = 0; (passes < maxPasses) && (accNorm > maxAccNorm); passes ++) {
		accNorm = dgFloat32 (0.0f);
		for (dgInt32 currJoint = 0; currJoint < jointCount; currJoint ++) {
			dgJacobian y0;
			dgJacobian y1;
			y0.m_linear = zero;
			y0.m_angular = zero;
			y1.m_linear = zero;
			y1.m_angular = zero;

			dgInt32 first = constraintArray[prevJoint].m_autoPairstart;
			dgInt32 rowsCount = constraintArray[prevJoint].m_autoPaircount;
			for (dgInt32 i = 0; i < rowsCount; i ++) {
				dgFloat32 deltaForce = forceStep[i]; 
				dgJacobianMatrixElement* const row = &matrixRow[i + first];

				y0.m_linear += row->m_Jt.m_jacobian_IM0.m_linear.Scale (deltaForce);
				y0.m_angular += row->m_Jt.m_jacobian_IM0.m_angular.Scale (deltaForce);
				y1.m_linear += row->m_Jt.m_jacobian_IM1.m_linear.Scale (deltaForce);
				y1.m_angular += row->m_Jt.m_jacobian_IM1.m_angular.Scale (deltaForce);
			}
			dgInt32 m0 = constraintArray[prevJoint].m_m0;
			dgInt32 m1 = constraintArray[prevJoint].m_m1;
			internalForces[m0].m_linear += y0.m_linear;
			internalForces[m0].m_angular += y0.m_angular;
			internalForces[m1].m_linear += y1.m_linear;
			internalForces[m1].m_angular += y1.m_angular;

			first = constraintArray[currJoint].m_autoPairstart;
			rowsCount = constraintArray[currJoint].m_autoPaircount;
			m0 = constraintArray[currJoint].m_m0;
			m1 = constraintArray[currJoint].m_m1;
			y0 = internalForces[m0];
			y1 = internalForces[m1];

			for (dgInt32 i = 0; i < rowsCount; i ++) {
				dgJacobianMatrixElement* const row = &matrixRow[i + first];
				dgVector acc (row->m_JMinv.m_jacobian_IM0.m_linear.CompProduct(y0.m_linear));
				acc += row->m_JMinv.m_jacobian_IM0.m_angular.CompProduct (y0.m_angular);
				acc += row->m_JMinv.m_jacobian_IM1.m_linear.CompProduct (y1.m_linear);
				acc += row->m_JMinv.m_jacobian_IM1.m_angular.CompProduct (y1.m_angular);
				row->m_accel = row->m_coordenateAccel - acc.m_x - acc.m_y - acc.m_z - row->m_force * row->m_diagDamp;
			}

			dgFloat32 jointAccel = CalculateJointForces (island, rowStart, currJoint, forceStep, maxAccNorm);
			accNorm = GetMax(accNorm, jointAccel);
			prevJoint = currJoint;
		}
	}

	for (dgInt32 i = 0; i < jointCount; i ++) {
		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPaircount;
		constraintArray[i].m_autoPaircount = count;
		for (dgInt32 k = 0; k < count; k ++) {
			dgJacobianMatrixElement* const row = &matrixRow[k + first];
			dgInt32 j = row->m_normalForceIndex;
			//_ASSERTE (((j < 0) && (force[j] == dgFloat32 (1.0f))) || ((j >= 0) && (force[j] >= dgFloat32 (0.0f))));
			_ASSERTE (((j <0) && (matrixRow[j].m_force == dgFloat32 (1.0f))) || ((j >= 0) && (matrixRow[j].m_force >= dgFloat32 (0.0f))));
			dgFloat32 val = matrixRow[j].m_force;
			row->m_lowerBoundFrictionCoefficent *= val;
			row->m_upperBoundFrictionCoefficent *= val;
			row->m_force = ClampValue(row->m_force, row->m_lowerBoundFrictionCoefficent, row->m_upperBoundFrictionCoefficent);
		}
	}

	for (dgInt32 i = 0; i < bodyCount; i ++) {
		internalForces[i].m_linear[0] = dgFloat32(0.0f);
		internalForces[i].m_linear[1] = dgFloat32(0.0f);
		internalForces[i].m_linear[2] = dgFloat32(0.0f);
		internalForces[i].m_linear[3] = dgFloat32(0.0f);
		internalForces[i].m_angular[0] = dgFloat32(0.0f);
		internalForces[i].m_angular[1] = dgFloat32(0.0f);
		internalForces[i].m_angular[2] = dgFloat32(0.0f);
		internalForces[i].m_angular[3] = dgFloat32(0.0f);
	}

	for (dgInt32 i = 0; i < jointCount; i ++) {
		dgJacobian y0;
		dgJacobian y1;
		y0.m_linear = zero;
		y0.m_angular = zero;
		y1.m_linear = zero;
		y1.m_angular = zero;
		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPairActiveCount;
		for (dgInt32 j = 0; j < count; j ++) {
			dgJacobianMatrixElement* const row = &matrixRow[j + first];
			dgFloat32 val = row->m_force; 
			y0.m_linear += row->m_Jt.m_jacobian_IM0.m_linear.Scale (val);
			y0.m_angular += row->m_Jt.m_jacobian_IM0.m_angular.Scale (val);
			y1.m_linear += row->m_Jt.m_jacobian_IM1.m_linear.Scale (val);
			y1.m_angular += row->m_Jt.m_jacobian_IM1.m_angular.Scale (val);

		}
		dgInt32 m0 = constraintArray[i].m_m0;
		dgInt32 m1 = constraintArray[i].m_m1;
		internalForces[m0].m_linear += y0.m_linear;
		internalForces[m0].m_angular += y0.m_angular;
		internalForces[m1].m_linear += y1.m_linear;
		internalForces[m1].m_angular += y1.m_angular;
	}


	dgInt32 forceRows = 0;
	dgFloat32 akNum = dgFloat32 (0.0f);
	accNorm = dgFloat32(0.0f);
	for (dgInt32 i = 0; i < jointCount; i ++) {
		bool isClamped[DG_CONSTRAINT_MAX_ROWS];
		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPairActiveCount;
		dgInt32 m0 = constraintArray[i].m_m0;
		dgInt32 m1 = constraintArray[i].m_m1;
		const dgJacobian& y0 = internalForces[m0];
		const dgJacobian& y1 = internalForces[m1];

		for (dgInt32 j = 0; j < count; j ++) {
			dgJacobianMatrixElement* const row = &matrixRow[j + first];
			dgVector tmpAccel (row->m_JMinv.m_jacobian_IM0.m_linear.CompProduct(y0.m_linear));
			tmpAccel += row->m_JMinv.m_jacobian_IM0.m_angular.CompProduct (y0.m_angular);
			tmpAccel += row->m_JMinv.m_jacobian_IM1.m_linear.CompProduct (y1.m_linear);
			tmpAccel += row->m_JMinv.m_jacobian_IM1.m_angular.CompProduct (y1.m_angular);
			row->m_accel = row->m_coordenateAccel - tmpAccel.m_x - tmpAccel.m_y - tmpAccel.m_z - row->m_force * row->m_diagDamp;
		}

		dgInt32 activeCount = 0;
		for (dgInt32 j = 0; j < count; j ++) {
			dgJacobianMatrixElement* const row = &matrixRow[j + first];
			dgFloat32 val = row->m_lowerBoundFrictionCoefficent - row->m_force;
			if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (row->m_accel < dgFloat32 (0.0f))) {
				row->m_force = row->m_lowerBoundFrictionCoefficent;
				isClamped[j] = true;
			} else {
				val = row->m_upperBoundFrictionCoefficent - row->m_force;
				if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (row->m_accel > dgFloat32 (0.0f))) {
					row->m_force = row->m_upperBoundFrictionCoefficent;
					isClamped[j] = true;
				} else {
					forceRows ++;
					activeCount ++;
					row->m_deltaForce = row->m_accel * row->m_invDJMinvJt;
					akNum += row->m_accel * row->m_deltaForce;
					accNorm = GetMax (dgAbsf (row->m_accel), accNorm);
					isClamped[j] = false;
				}
			}
		}

		if (activeCount < count) {
			dgInt32 i0 = 0;
			dgInt32 i1 = count - 1;
			constraintArray[i].m_autoPairActiveCount = activeCount;
			do { 
				while ((i0 <= i1) && !isClamped[i0]) i0 ++;
				while ((i0 <= i1) && isClamped[i1]) i1 --;
				if (i0 < i1) {
					//SwapRows (first + i0, first + i1);
					Swap (matrixRow[first + i0], matrixRow[first + i1]);
					i0 ++;
					i1 --;
				}
			} while (i0 < i1); 
		}
	}


	maxPasses = forceRows;
	dgInt32 totalPassesCount = 0;
	for (dgInt32 passes = 0; (passes < maxPasses) && (accNorm > maxAccNorm); passes ++) {
		for (dgInt32 i = 0; i < bodyCount; i ++) {
			internalForces[i].m_linear[0] = dgFloat32(0.0f);
			internalForces[i].m_linear[1] = dgFloat32(0.0f);
			internalForces[i].m_linear[2] = dgFloat32(0.0f);
			internalForces[i].m_linear[3] = dgFloat32(0.0f);
			internalForces[i].m_angular[0] = dgFloat32(0.0f);
			internalForces[i].m_angular[1] = dgFloat32(0.0f);
			internalForces[i].m_angular[2] = dgFloat32(0.0f);
			internalForces[i].m_angular[3] = dgFloat32(0.0f);
		}

		for (dgInt32 i = 0; i < jointCount; i ++) {
			dgJacobian y0;
			dgJacobian y1;

			y0.m_linear = zero;
			y0.m_angular = zero;
			y1.m_linear = zero;
			y1.m_angular = zero;
			dgInt32 first = constraintArray[i].m_autoPairstart;
			dgInt32 count = constraintArray[i].m_autoPairActiveCount;

			for (dgInt32 j = 0; j < count; j ++) {
				dgJacobianMatrixElement* const row = &matrixRow[j + first];
				dgFloat32 val = row->m_deltaForce;
				y0.m_linear += row->m_Jt.m_jacobian_IM0.m_linear.Scale (val);
				y0.m_angular += row->m_Jt.m_jacobian_IM0.m_angular.Scale (val);
				y1.m_linear += row->m_Jt.m_jacobian_IM1.m_linear.Scale (val);
				y1.m_angular += row->m_Jt.m_jacobian_IM1.m_angular.Scale (val);
			}
			dgInt32 m0 = constraintArray[i].m_m0;
			dgInt32 m1 = constraintArray[i].m_m1;
			internalForces[m0].m_linear += y0.m_linear;
			internalForces[m0].m_angular += y0.m_angular;
			internalForces[m1].m_linear += y1.m_linear;
			internalForces[m1].m_angular += y1.m_angular;
		}


		dgFloat32 akDen = dgFloat32 (0.0f);
		for (dgInt32 i = 0; i < jointCount; i ++) {
			dgInt32 first = constraintArray[i].m_autoPairstart;
			dgInt32 count = constraintArray[i].m_autoPairActiveCount;
			dgInt32 m0 = constraintArray[i].m_m0;
			dgInt32 m1 = constraintArray[i].m_m1;
			const dgJacobian& y0 = internalForces[m0];
			const dgJacobian& y1 = internalForces[m1];
			for (dgInt32 j = 0; j < count; j ++) {
				dgJacobianMatrixElement* const row = &matrixRow[j + first];
				dgVector tmpAccel (row->m_JMinv.m_jacobian_IM0.m_linear.CompProduct(y0.m_linear));
				tmpAccel += row->m_JMinv.m_jacobian_IM0.m_angular.CompProduct (y0.m_angular);
				tmpAccel += row->m_JMinv.m_jacobian_IM1.m_linear.CompProduct (y1.m_linear);
				tmpAccel += row->m_JMinv.m_jacobian_IM1.m_angular.CompProduct (y1.m_angular);

				row->m_deltaAccel = tmpAccel.m_x + tmpAccel.m_y + tmpAccel.m_z + row->m_deltaForce * row->m_diagDamp;
				akDen += row->m_deltaAccel * row->m_deltaForce;
			}
		}

		_ASSERTE (akDen > dgFloat32 (0.0f));
		akDen = GetMax (akDen, dgFloat32(1.0e-16f));
		_ASSERTE (dgAbsf (akDen) >= dgFloat32(1.0e-16f));
		dgFloat32 ak = akNum / akDen;
		dgInt32 clampedForceIndex = -1;
		dgInt32 clampedForceJoint = -1;
		dgFloat32 clampedForceIndexValue = dgFloat32 (0.0f);

		for (dgInt32 i = 0; i < jointCount; i ++) {
			if (ak > dgFloat32 (1.0e-8f)) {
				dgInt32 first = constraintArray[i].m_autoPairstart;
				dgInt32 count = constraintArray[i].m_autoPairActiveCount;
				for (dgInt32 j = 0; j < count; j ++) {
					dgJacobianMatrixElement* const row = &matrixRow[j + first];
					dgFloat32 val = row->m_force + ak * row->m_deltaForce;
					if (row->m_deltaForce < dgFloat32 (-1.0e-16f)) {
						if (val < row->m_lowerBoundFrictionCoefficent) {
							ak = GetMax ((row->m_lowerBoundFrictionCoefficent - row->m_force) / row->m_deltaForce, dgFloat32 (0.0f));
							_ASSERTE (ak >= dgFloat32 (0.0f));
							clampedForceIndex = j;
							clampedForceJoint = i;
							clampedForceIndexValue = row->m_lowerBoundFrictionCoefficent;
						}
					} else if (row->m_deltaForce > dgFloat32 (1.0e-16f)) {
						if (val > row->m_upperBoundFrictionCoefficent) {
							ak = GetMax ((row->m_upperBoundFrictionCoefficent - row->m_force) / row->m_deltaForce, dgFloat32 (0.0f));
							_ASSERTE (ak >= dgFloat32 (0.0f));
							clampedForceIndex = j;
							clampedForceJoint = i;
							clampedForceIndexValue = row->m_upperBoundFrictionCoefficent;
						}
					}
				}
			}
		}

		if (clampedForceIndex >= 0) {
			bool isClamped[DG_CONSTRAINT_MAX_ROWS];
			for (dgInt32 i = 0; i < jointCount; i ++) {
				dgInt32 first = constraintArray[i].m_autoPairstart;
				dgInt32 count = constraintArray[i].m_autoPairActiveCount;
				for (dgInt32 j = 0; j < count; j ++) {
					dgJacobianMatrixElement* const row = &matrixRow[j + first];
					row->m_force += ak * row->m_deltaForce;
					row->m_accel -= ak * row->m_deltaAccel;
				}
			}

			dgInt32 first = constraintArray[clampedForceJoint].m_autoPairstart;
			dgInt32 count = constraintArray[clampedForceJoint].m_autoPairActiveCount;
			count --;
			//force[first + clampedForceIndex] = clampedForceIndexValue;
			matrixRow[first + clampedForceIndex].m_force = clampedForceIndexValue;
			if (clampedForceIndex != count) {
				//SwapRows (first + clampedForceIndex, first + count);
				Swap (matrixRow[first + clampedForceIndex], matrixRow[first + count]);
			}

			dgInt32 activeCount = count;
			for (dgInt32 i = 0; i < count; i ++) {
				//dgInt32 index;
				//dgFloat32 val;
				//index = first + i;
				dgJacobianMatrixElement* const row = &matrixRow[first + i];
				isClamped[i] = false;
				dgFloat32 val = row->m_lowerBoundFrictionCoefficent - row->m_force;
				if ((val > dgFloat32 (-1.0e-5f)) && (row->m_accel < dgFloat32 (0.0f))) {
					activeCount --;
					isClamped[i] = true;
				} else {
					val = row->m_upperBoundFrictionCoefficent - row->m_force;
					if ((val < dgFloat32 (1.0e-5f)) && (row->m_accel > dgFloat32 (0.0f))) {
						activeCount --;
						isClamped[i] = true;
					}
				}
			}

			if (activeCount < count) {
				dgInt32 i0 = 0;
				dgInt32 i1 = count - 1;
				do { 
					while ((i0 <= i1) && !isClamped[i0]) i0 ++;
					while ((i0 <= i1) && isClamped[i1]) i1 --;
					if (i0 < i1) {
						//SwapRows (first + i0, first + i1);
						Swap (matrixRow[first + i0], matrixRow[first + i1]);
						i0 ++;
						i1 --;
					}
				} while (i0 < i1); 
			}
			constraintArray[clampedForceJoint].m_autoPairActiveCount = activeCount;

			forceRows = 0;
			akNum = dgFloat32 (0.0f);
			accNorm = dgFloat32(0.0f);
			for (dgInt32 i = 0; i < jointCount; i ++) {
				dgInt32 first = constraintArray[i].m_autoPairstart;
				dgInt32 count = constraintArray[i].m_autoPairActiveCount;
				forceRows += count;

				for (dgInt32 j = 0; j < count; j ++) {
					//index = first + j;
					dgJacobianMatrixElement* const row = &matrixRow[first + j];
					_ASSERTE ((i != clampedForceJoint) || !((dgAbsf (row->m_lowerBoundFrictionCoefficent - row->m_force) < dgFloat32 (1.0e-5f)) && (row->m_accel < dgFloat32 (0.0f))));
					_ASSERTE ((i != clampedForceJoint) || !((dgAbsf (row->m_upperBoundFrictionCoefficent - row->m_force) < dgFloat32 (1.0e-5f)) && (row->m_accel > dgFloat32 (0.0f))));
					row->m_deltaForce = row->m_accel * row->m_invDJMinvJt;
					akNum += row->m_deltaForce * row->m_accel;
					accNorm = GetMax (dgAbsf (row->m_accel), accNorm);
					_ASSERTE (dgCheckFloat(row->m_deltaForce));
				}
			}

			_ASSERTE (akNum >= dgFloat32 (0.0f));
			passes = -1;
			maxPasses = forceRows;

		} else {

			accNorm = dgFloat32(0.0f);
			for (dgInt32 i = 0; i < jointCount; i ++) {
				dgInt32 first = constraintArray[i].m_autoPairstart;
				dgInt32 count = constraintArray[i].m_autoPairActiveCount;
				for (dgInt32 j = 0; j < count; j ++) {
					//index = j + first;
					dgJacobianMatrixElement* const row = &matrixRow[first + j];
					row->m_force += ak * row->m_deltaForce;
					row->m_accel -= ak * row->m_deltaAccel;
					accNorm = GetMax (dgAbsf (row->m_accel), accNorm);
				}
			}

			if (accNorm > maxAccNorm) {
				akDen = akNum;
				akNum = dgFloat32(0.0f);
				for (dgInt32 i = 0; i < jointCount; i ++) {
					dgInt32 first = constraintArray[i].m_autoPairstart;
					dgInt32 count = constraintArray[i].m_autoPairActiveCount;
					for (dgInt32 j = 0; j < count; j ++) {
						//index = j + first;
						dgJacobianMatrixElement* const row = &matrixRow[first + j];
						row->m_deltaAccel = row->m_accel * row->m_invDJMinvJt;
						akNum += row->m_accel * row->m_deltaAccel;
					}
				}

				_ASSERTE (akNum >= dgFloat32 (0.0f));
				_ASSERTE (akDen > dgFloat32(0.0f));
				akDen = GetMax (akDen, dgFloat32 (1.0e-17f));
				ak = dgFloat32 (akNum / akDen);
				for (dgInt32 i = 0; i < jointCount; i ++) {
					dgInt32 first = constraintArray[i].m_autoPairstart;
					dgInt32 count = constraintArray[i].m_autoPairActiveCount;
					for (dgInt32 j = 0; j < count; j ++) {
						//index = j + first;
						dgJacobianMatrixElement* const row = &matrixRow[first + j];
						row->m_deltaForce = row->m_deltaAccel + ak * row->m_deltaForce;
					}
				}
			}
		}

		totalPassesCount ++;
	}


	//ApplyExternalForcesAndAcceleration (maxAccNorm);
	ApplyExternalForcesAndAcceleration (island, rowStart, threadIndex, timestep, maxAccNorm);
}

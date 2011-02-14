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


#define DG_MAX_PASSES	64

class dgCalculateForceLocks
{
	public:
	dgInt32 m_jointAccelerationSync;
	dgInt32 m_bodyVelocityAtomicSync;
	dgInt32 m_bodyVelocityAtomicIndex;
	dgInt32 m_jointAccelerationAtomicIndex;
	dgInt32 m_JointForceSync[DG_MAX_PASSES];
	dgInt32 m_JointForceAtomicIndex[DG_MAX_PASSES];
};


class dgPalellelSolveSyncData
{
	public:

	dgPalellelSolveSyncData()
	{
		memset (this, 0, sizeof (dgPalellelSolveSyncData));
	}

	dgInt32 m_bodyCount;
	dgInt32 m_jointCount;

	dgInt32 m_initBodiesSync;
	dgInt32 m_initBodiesCounter;

	dgInt32 m_initJacobianMatrixSync;
	dgInt32 m_initJacobianMatrixCounter;
	dgInt32 m_jacobianMatrixRowIndexCounter;

	dgInt32 m_initIntenalForceAccumulatorSync;
	dgInt32 m_initIntenalForceAccumulatorCounter;

	dgInt32 m_saveForcefeebackSync;
	dgInt32 m_saveForcefeebackCounter;

	dgInt32 m_updateForcefeebackSync;
	dgInt32 m_updateForcefeebackCounter;

	dgInt32 m_updateBodyVelocitySync;
	dgInt32 m_updateBodyVelocityCounter;


	dgInt32 m_subStepLocks;
	dgInt32 m_calculateInternalForcesSync;

	dgFloat32 m_timestep;

	dgWorld* m_world;
	dgInt32* m_bodyInfoMap;
	dgInt32* m_jointInfoMap;

	dgFloat32 m_accNorm[DG_MAX_THREADS_HIVE_COUNT];
	dgInt32 m_hasJointFeeback[DG_MAX_THREADS_HIVE_COUNT];
	dgCalculateForceLocks m_calculateForcesLock[LINEAR_SOLVER_SUB_STEPS];
};

void dgWorldDynamicUpdate::CalculateReactionForcesParallel (const dgIsland* const islandArray, dgInt32 islandsCount, dgFloat32 timestep) const
{
//	dgInt32 rowBase = BuildJacobianMatrix (island, threadID, timestep);
//	CalculateReactionsForces (island, rowBase, threadID, timestep, DG_SOLVER_MAX_ERROR);
//	IntegrateArray (island, DG_SOLVER_MAX_ERROR, timestep, threadID, true); 

	dgWorld* const world = (dgWorld*) this;

	world->m_pairMemoryBuffer.ExpandCapacityIfNeessesary ((m_bodies + m_joints + 1024), sizeof (dgInt32));

	dgInt32* const bodyInfoMap = (dgInt32*) &world->m_pairMemoryBuffer[0];
	dgInt32* const jointInfoMap = &bodyInfoMap[(m_bodies + 15) & (-16)];


	dgJacobian* const internalVeloc = &world->m_solverMemory.m_internalVeloc[0];
	dgJacobian* const internalForces = &world->m_solverMemory.m_internalForces[0];
	dgVector zero(dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	internalVeloc[0].m_linear = zero;
	internalVeloc[0].m_angular = zero;
	internalForces[0].m_linear = zero;
	internalForces[0].m_angular = zero;

	dgInt32 bodyCount = 0;
	dgBodyInfo* const bodyArray = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	for (dgInt32 i = 0; i < islandsCount; i ++) {
		dgInt32 count = islandArray[i].m_bodyCount;
		dgInt32 bodyStart = islandArray[i].m_bodyStart;
		for (dgInt32 j = 1; j < count; j ++) {
			dgInt32 index = bodyStart + j;
			bodyInfoMap[bodyCount] = index;
			dgBody* const body = bodyArray[index].m_body;
			body->m_index = index;
			_ASSERTE (bodyCount <= m_bodies);
			bodyCount ++;
		}
	}

	dgInt32 jointsCount = 0;
	dgInt32 maxRowCount = islandsCount ? islandArray[0].m_jointCount : 0;	
	dgJointInfo* const constraintArray = (dgJointInfo*) &world->m_jointsMemory[0];
	for (dgInt32 jointIndex = 0; jointIndex < maxRowCount; jointIndex ++) {
		for (dgInt32 i = 0; (i < islandsCount) && (jointIndex < islandArray[i].m_jointCount); i ++) {
			dgInt32 index = islandArray[i].m_jointStart + jointIndex;
			jointInfoMap[jointsCount] = index;
			dgConstraint* const joint = constraintArray[index].m_joint;
			joint->m_index = dgUnsigned32 (jointsCount);
			jointsCount ++;
			_ASSERTE (jointsCount <= m_joints);
		}
	} 

	_ASSERTE (0);
/*
	dgPalellelSolveSyncData sincksPoints;
	void* userParamArray[DG_MAX_THREADS_HIVE_PARAMETERS];
	userParamArray[0] = &sincksPoints;

	sincksPoints.m_bodyCount = bodyCount;
	sincksPoints.m_jointCount = jointsCount;
	sincksPoints.m_world = world;
	sincksPoints.m_timestep = timestep;
	sincksPoints.m_bodyInfoMap = bodyInfoMap;
	sincksPoints.m_jointInfoMap = jointInfoMap;

	dgInt32 threadCounts = world->GetThreadCount();	
	for (dgInt32 i = 0; i < threadCounts; i ++) {
		world->QueueJob (ParallelSolverDriver, &userParamArray[0], 1);
	}
	world->SynchronizationBarrier();
*/
}


void dgWorldDynamicUpdate::ParallelSolverDriver (void** const userParamArray, dgInt32 threadID)
{
	dgPalellelSolveSyncData* const data = (dgPalellelSolveSyncData*) userParamArray[0];

	dgWorld* const world = data->m_world;
	world->InitilizeBodyArrayParallel (data, threadID);
	world->BuildJacobianMatrixParallel (data, threadID);
	world->SolverInitInternalForcesParallel (data, threadID);
	world->CalculateForcesGameModeParallel (data, threadID);
}


void dgWorldDynamicUpdate::GetJacobianDerivativesParallel (dgJointInfo* const jointInfo, dgInt32 threadIndex, bool bitMode, dgInt32 rowBase, dgFloat32 timestep) const
{
	dgWorld* const world = (dgWorld*) this;

	dgContraintDescritor constraintParams;
	constraintParams.m_world = world; 
	constraintParams.m_threadIndex = threadIndex;
	constraintParams.m_timestep = timestep;
	constraintParams.m_invTimestep = dgFloat32 (1.0f / timestep);

	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowBase];
	dgConstraint* const constraint = jointInfo->m_joint;

	dgInt32 dof = dgInt32 (constraint->m_maxDOF);
	_ASSERTE (dof <= DG_CONSTRAINT_MAX_ROWS);
	for (dgInt32 i = 0; i < dof; i ++) {
		constraintParams.m_forceBounds[i].m_low = DG_MIN_BOUND;
		constraintParams.m_forceBounds[i].m_upper = DG_MAX_BOUND;
		constraintParams.m_forceBounds[i].m_jointForce = NULL;
		constraintParams.m_forceBounds[i].m_normalIndex = DG_BILATERAL_CONSTRAINT;
	}

	_ASSERTE (constraint->m_body0);
	_ASSERTE (constraint->m_body1);

	constraint->m_body0->m_inCallback = true;
	constraint->m_body1->m_inCallback = true;

	dof = dgInt32 (constraint->JacobianDerivative (constraintParams)); 

	constraint->m_body0->m_inCallback = false;
	constraint->m_body1->m_inCallback = false;

	dgInt32 m0 = (constraint->m_body0->m_invMass.m_w != dgFloat32(0.0f)) ? constraint->m_body0->m_index: 0;
	dgInt32 m1 = (constraint->m_body1->m_invMass.m_w != dgFloat32(0.0f)) ? constraint->m_body1->m_index: 0;

	//_ASSERTE (constraint->m_index == dgUnsigned32(j));
	//constraint->m_index = j;
	jointInfo->m_autoPairstart = rowBase;
	jointInfo->m_autoPaircount = dof;
	jointInfo->m_autoPairActiveCount = dof;
	jointInfo->m_m0 = m0;
	jointInfo->m_m1 = m1;

//	dgInt32 fistForceOffset = -rowBase;
	for (dgInt32 i = 0; i < dof; i ++) {
		dgJacobianMatrixElement* const row = &matrixRow[i];
		_ASSERTE (constraintParams.m_forceBounds[i].m_jointForce);
		row->m_Jt = constraintParams.m_jacobian[i]; 

		_ASSERTE (constraintParams.m_jointStiffness[i] >= dgFloat32(0.1f));
		_ASSERTE (constraintParams.m_jointStiffness[i] <= dgFloat32(100.0f));

		row->m_diagDamp = constraintParams.m_jointStiffness[i];
		row->m_coordenateAccel = constraintParams.m_jointAccel[i];
		row->m_accelIsMotor = constraintParams.m_isMotor[i];
		row->m_restitution = constraintParams.m_restitution[i];
		row->m_penetration = constraintParams.m_penetration[i];
		row->m_penetrationStiffness = constraintParams.m_penetrationStiffness[i];
		row->m_lowerBoundFrictionCoefficent = constraintParams.m_forceBounds[i].m_low;
		row->m_upperBoundFrictionCoefficent = constraintParams.m_forceBounds[i].m_upper;
		row->m_jointFeebackForce = constraintParams.m_forceBounds[i].m_jointForce;

		dgInt32 index = constraintParams.m_forceBounds[i].m_normalIndex ;
		//normalForceIndex[rowCount] = constraintParams.m_forceBounds[i].m_normalIndex + ((constraintParams.m_forceBounds[i].m_normalIndex >=0) ? (rowCount - i) : fistForceOffset);
		//row->m_normalForceIndex = index + ((index >=0) ? (rowCount - i) : fistForceOffset);
		row->m_normalForceIndex = (index >=0) ? (index + rowBase) : index;
//		rowCount ++;
	}

	//#ifdef _DEBUG
#if 0
	for (dgInt32 i = 0; i < ((rowCount + 3) & 0xfffc) - rowCount ; i ++) {
		matrixRow[rowCount + i].m_diagDamp = dgFloat32 (0.0f);
		matrixRow[rowCount + i].m_coordenateAccel = dgFloat32 (0.0f);
		matrixRow[rowCount + i].m_restitution = dgFloat32 (0.0f);
		matrixRow[rowCount + i].m_penetration = dgFloat32 (0.0f);
		matrixRow[rowCount + i].m_penetrationStiffness = dgFloat32 (0.0f);
		matrixRow[rowCount + i].m_lowerBoundFrictionCoefficent = dgFloat32 (0.0f);
		matrixRow[rowCount + i].m_upperBoundFrictionCoefficent = dgFloat32 (0.0f);
		matrixRow[rowCount + i].m_jointFeebackForce = 0;
		matrixRow[rowCount + i].m_normalForceIndex = 0;
	}
#endif
//	rowCount = (rowCount & (DG_SIMD_WORD_SIZE - 1)) ? ((rowCount & (-DG_SIMD_WORD_SIZE)) + DG_SIMD_WORD_SIZE) : rowCount;
//	_ASSERTE ((rowCount & (DG_SIMD_WORD_SIZE - 1)) == 0);
//	return rowCount;
}



void dgWorldDynamicUpdate::InitilizeBodyArrayParallel (dgPalellelSolveSyncData* const syncData, dgInt32 threadIndex) const 
{
	dgWorld* const world = (dgWorld*)this;
	dgInt32* const atomicIndex = &syncData->m_initBodiesCounter; 
	const dgInt32* const bodyInfoIndexArray = syncData->m_bodyInfoMap;
	dgBodyInfo* const bodyArray = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	dgJacobian* const internalVeloc = &world->m_solverMemory.m_internalVeloc[0];
	dgJacobian* const internalForces = &world->m_solverMemory.m_internalForces[0];
	dgVector zero(dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	
	for (dgInt32 i = dgAtomicAdd(atomicIndex, 1); i < syncData->m_bodyCount;  i = dgAtomicAdd(atomicIndex, 1)) {
		_ASSERTE ((bodyArray[0].m_body->m_accel % bodyArray[0].m_body->m_accel) == dgFloat32 (0.0f));
		_ASSERTE ((bodyArray[0].m_body->m_alpha % bodyArray[0].m_body->m_alpha) == dgFloat32 (0.0f));

		dgInt32 index = bodyInfoIndexArray[i];
		_ASSERTE (index);
		dgBody* const body = bodyArray[index].m_body;
		_ASSERTE (body->m_invMass.m_w > dgFloat32 (0.0f));
		body->AddDamingAcceleration();
		body->CalcInvInertiaMatrix ();

		body->m_netForce = body->m_veloc;
		body->m_netTorque = body->m_omega;

		internalVeloc[index].m_linear = zero;
		internalVeloc[index].m_angular = zero;
		internalForces[index].m_linear = zero;
		internalForces[index].m_angular = zero;
	}
	world->SyncThreads(&syncData->m_initBodiesSync);
}

void dgWorldDynamicUpdate::BuildJacobianMatrixParallel (dgPalellelSolveSyncData* const syncData, dgInt32 threadIndex) const 
{
	dgWorld* const world = (dgWorld*) this;
	const dgInt32* const jointInfoIndexArray = syncData->m_jointInfoMap;
	dgInt32* const atomicIndex = &syncData->m_initJacobianMatrixCounter;
	dgBodyInfo* const bodyArray = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	dgJointInfo* const constraintArray = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[0];

	for (dgInt32 i = dgAtomicAdd(atomicIndex, 1); i < syncData->m_jointCount;  i = dgAtomicAdd(atomicIndex, 1)) {

		dgInt32 jointIndex = jointInfoIndexArray[i];
		dgJointInfo* const jointInfo = &constraintArray[jointIndex];
		_ASSERTE (dgInt32 (jointInfo->m_joint->m_index) == i);

		dgInt32 rowBase = dgAtomicAdd(&syncData->m_jacobianMatrixRowIndexCounter, jointInfo->m_autoPaircount);
		//	if (island->m_hasUnilateralJoints) {
		//		rowCount = GetJacobianDerivatives (island, threadIndex, false, rowBase, rowCount, timestep);
		//	}
		//	rowCount = GetJacobianDerivatives (island, threadIndex, true, rowBase, rowCount, timestep);
		GetJacobianDerivativesParallel (jointInfo, threadIndex, false, rowBase, syncData->m_timestep);

		dgInt32 index = jointInfo->m_autoPairstart;
		dgInt32 count = jointInfo->m_autoPaircount;
		dgInt32 m0 = jointInfo->m_m0;
		dgInt32 m1 = jointInfo->m_m1;

		//_ASSERTE (m0 >= 0);
		//_ASSERTE (m0 < bodyCount);
		dgBody* const body0 = bodyArray[m0].m_body;
		dgFloat32 invMass0 = body0->m_invMass[3];
		const dgMatrix& invInertia0 = body0->m_invWorldInertiaMatrix;

		//_ASSERTE (m1 >= 0);
		//_ASSERTE (m1 < bodyCount);
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
			//row->m_extAccel = extenalAcceleration;
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
			//solverMemory.m_diagJMinvJt[index] = diag;
			row->m_invDJMinvJt = dgFloat32(1.0f) / diag;

			index ++;
		}
	}
	world->SyncThreads(&syncData->m_initJacobianMatrixSync);
}


void dgWorldDynamicUpdate::SolverInitInternalForcesParallel (dgPalellelSolveSyncData* const syncData, dgInt32 threadIndex) const 
{
	dgWorld* const world = (dgWorld*) this;
	dgInt32* const treadLocks = &m_solverMemory.m_treadLocks[0];
	const dgInt32* const jointInfoIndexArray = syncData->m_jointInfoMap;
	dgJacobian* const internalForces = &m_solverMemory.m_internalForces[0];
	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[0];
	dgInt32* const atomicIndex = &syncData->m_initIntenalForceAccumulatorCounter;
	dgJointInfo* const constraintArray = (dgJointInfo*) &world->m_jointsMemory[0];
	
	dgVector zero(dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	for (dgInt32 i = dgAtomicAdd(atomicIndex, 1); i < syncData->m_jointCount; i = dgAtomicAdd(atomicIndex, 1)) {

		dgJacobian y0;
		dgJacobian y1;

		y0.m_linear = zero;
		y0.m_angular = zero;
		y1.m_linear = zero;
		y1.m_angular = zero;

		dgInt32 jointIndex = jointInfoIndexArray[i];
		dgJointInfo* const jointInfo = &constraintArray[jointIndex];
		_ASSERTE (dgInt32 (jointInfo->m_joint->m_index) == i);

		dgInt32 first = jointInfo->m_autoPairstart;
		dgInt32 count = jointInfo->m_autoPaircount;
		for (dgInt32 j = 0; j < count; j ++) { 
			dgJacobianMatrixElement* const row = &matrixRow[j + first];

			dgFloat32 val = row->m_force; 
			_ASSERTE (dgCheckFloat(val));
			y0.m_linear += row->m_Jt.m_jacobian_IM0.m_linear.Scale (val);
			y0.m_angular += row->m_Jt.m_jacobian_IM0.m_angular.Scale (val);
			y1.m_linear += row->m_Jt.m_jacobian_IM1.m_linear.Scale (val);
			y1.m_angular += row->m_Jt.m_jacobian_IM1.m_angular.Scale (val);
		}

		dgInt32 m0 = jointInfo->m_m0;
		dgInt32 m1 = jointInfo->m_m1;
		world->GetIndirectLock (&treadLocks[m0]);
			internalForces[m0].m_linear += y0.m_linear;
			internalForces[m0].m_angular += y0.m_angular;
		world->ReleaseIndirectLock(&treadLocks[m0]);

		world->GetIndirectLock (&treadLocks[m1]);
			internalForces[m1].m_linear += y1.m_linear;
			internalForces[m1].m_angular += y1.m_angular;
		world->ReleaseIndirectLock(&treadLocks[m1]);
	}

	world->SyncThreads(&syncData->m_initIntenalForceAccumulatorSync);
}


void dgWorldDynamicUpdate::CalculateForcesGameModeParallel (dgPalellelSolveSyncData* const syncData, dgInt32 threadIndex) const
{
	dgWorld* const world = (dgWorld*) this;
	dgInt32* const treadLocks = &m_solverMemory.m_treadLocks[0];
	const dgInt32* const bodyInfoIndexArray = syncData->m_bodyInfoMap;
	const dgInt32* const jointInfoIndexArray = syncData->m_jointInfoMap;
	dgBodyInfo* const bodyArray = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	
	dgJointInfo* const constraintArray = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[0];

	dgJacobian* const internalVeloc = &m_solverMemory.m_internalVeloc[0];
	dgJacobian* const internalForces = &m_solverMemory.m_internalForces[0];

	dgFloat32 invTimestepSrc = dgFloat32 (1.0f) / syncData->m_timestep;
	dgFloat32 invStep = (dgFloat32 (1.0f) / dgFloat32 (LINEAR_SOLVER_SUB_STEPS));
	dgFloat32 timestep =  syncData->m_timestep * invStep;
	dgFloat32 invTimestep = invTimestepSrc * dgFloat32 (LINEAR_SOLVER_SUB_STEPS);
	_ASSERTE (syncData->m_bodyCount ? (bodyArray[0].m_body == world->m_sentionelBody) : !syncData->m_bodyCount);

	dgInt32 maxPasses = dgInt32 (world->m_solverMode + DG_BASE_ITERATION_COUNT);
	if (maxPasses > DG_MAX_PASSES) {
		maxPasses = DG_MAX_PASSES;
	}

	dgFloat32 firstPassCoef = dgFloat32 (0.0f);
	for (dgInt32 step = 0; step < LINEAR_SOLVER_SUB_STEPS; step ++) {

		dgJointAccelerationDecriptor joindDesc;
		joindDesc.m_timeStep = timestep;
		joindDesc.m_invTimeStep = invTimestep;
		joindDesc.m_firstPassCoefFlag = firstPassCoef;

		dgInt32* const jointAtomicIndex = &syncData->m_calculateForcesLock[step].m_jointAccelerationAtomicIndex;
		for (dgInt32 i = dgAtomicAdd(jointAtomicIndex, 1); i < syncData->m_jointCount;  i = dgAtomicAdd(jointAtomicIndex, 1)) {
			dgInt32 curJoint = jointInfoIndexArray[i];
			joindDesc.m_rowsCount = constraintArray[curJoint].m_autoPaircount;
			joindDesc.m_rowMatrix = &matrixRow[constraintArray[curJoint].m_autoPairstart];
			constraintArray[curJoint].m_joint->JointAccelerations (&joindDesc);
		}
		world->SyncThreads(&syncData->m_calculateForcesLock[step].m_jointAccelerationSync);
		firstPassCoef = dgFloat32 (1.0f);


		dgFloat32 accNorm = DG_SOLVER_MAX_ERROR * dgFloat32 (2.0f);
		for (dgInt32 passes = 0; (passes < maxPasses) && (accNorm > DG_SOLVER_MAX_ERROR); passes ++) {
			accNorm = dgFloat32 (0.0f);
			dgInt32* const atomicIndex = &syncData->m_calculateForcesLock[step].m_JointForceAtomicIndex[passes];
			for (dgInt32 i = dgAtomicAdd(atomicIndex, 1); i < syncData->m_jointCount; i = dgAtomicAdd(atomicIndex, 1)) {

				dgInt32 curJoint = jointInfoIndexArray[i];
				dgInt32 m0 = constraintArray[curJoint].m_m0;
				dgInt32 m1 = constraintArray[curJoint].m_m1;
				dgInt32 index = constraintArray[curJoint].m_autoPairstart;
				dgInt32 rowsCount = constraintArray[curJoint].m_autoPaircount;

				world->GetIndirectLock (&syncData->m_subStepLocks);
					while (m0 && treadLocks[m0]) {
						//dgThreadYield();
					}
					while (m1 && treadLocks[m1]) {
						//dgThreadYield();
					}
					treadLocks[m0] = 1;
					treadLocks[m1] = 1;
				world->ReleaseIndirectLock(&syncData->m_subStepLocks);
				
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

				treadLocks[m0] = 0;
				treadLocks[m1] = 0;
			}
			syncData->m_accNorm[threadIndex] = accNorm;
			world->SyncThreads(&syncData->m_calculateForcesLock[step].m_JointForceSync[passes]);

			accNorm = dgFloat32 (0.0f);
			for (dgInt32 i = 0; i < DG_MAX_THREADS_HIVE_COUNT; i ++) {
				accNorm = GetMax (accNorm, dgAbsf (syncData->m_accNorm[i]));
			}
//accNorm  = 1.0f;
		}

		dgInt32* const bodyAtomicIndex = &syncData->m_calculateForcesLock[step].m_bodyVelocityAtomicIndex;
		for (dgInt32 i = dgAtomicAdd(bodyAtomicIndex, 1); i < syncData->m_bodyCount;  i = dgAtomicAdd(bodyAtomicIndex, 1)) {
			dgInt32 index = bodyInfoIndexArray[i];
			_ASSERTE (index);
			dgBody* const body = bodyArray[index].m_body;
			_ASSERTE (body->m_index == index);
			dgVector force (body->m_accel + internalForces[index].m_linear);
			dgVector torque (body->m_alpha + internalForces[index].m_angular);

			dgVector accel (force.Scale (body->m_invMass.m_w));
			dgVector alpha (body->m_invWorldInertiaMatrix.RotateVector (torque));
			body->m_veloc += accel.Scale(timestep);
			body->m_omega += alpha.Scale(timestep);
			internalVeloc[index].m_linear += body->m_veloc;
			internalVeloc[index].m_angular += body->m_omega;
		}
		world->SyncThreads(&syncData->m_calculateForcesLock[step].m_bodyVelocityAtomicSync);
	}

	dgInt32 hasJointFeeback = 0;
	dgInt32* const saveForcefeebackAtomicIndex = &syncData->m_saveForcefeebackCounter;
	for (dgInt32 i = dgAtomicAdd(saveForcefeebackAtomicIndex, 1); i < syncData->m_jointCount; i = dgAtomicAdd(saveForcefeebackAtomicIndex, 1)) {
		dgInt32 curJoint = jointInfoIndexArray[i];
		dgInt32 first = constraintArray[curJoint].m_autoPairstart;
		dgInt32 count = constraintArray[curJoint].m_autoPaircount;

		for (dgInt32 j = 0; j < count; j ++) { 
			dgJacobianMatrixElement* const row = &matrixRow[j + first];
			dgFloat32 val = row->m_force; 
			_ASSERTE (dgCheckFloat(val));
			row->m_jointFeebackForce[0] = val;
		}
		hasJointFeeback |= (constraintArray[i].m_joint->m_updaFeedbackCallback ? 1 : 0);
	}
	syncData->m_hasJointFeeback[threadIndex] = hasJointFeeback;
	world->SyncThreads(&syncData->m_saveForcefeebackSync);

	hasJointFeeback = 0;
	for (dgInt32 i = 0; i < DG_MAX_THREADS_HIVE_COUNT; i ++) {
		hasJointFeeback |= syncData->m_hasJointFeeback[i];
	}

//	dgFloat32 maxAccNorm2 = maxAccNorm * maxAccNorm;
	dgFloat32 maxAccNorm2 = DG_SOLVER_MAX_ERROR * DG_SOLVER_MAX_ERROR;
	dgVector zero(dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));

	dgInt32* const updateBodyVelocityCounter = &syncData->m_updateBodyVelocityCounter;
	for (dgInt32 i = dgAtomicAdd(updateBodyVelocityCounter, 1); i < syncData->m_bodyCount; i = dgAtomicAdd(updateBodyVelocityCounter, 1)) {
		dgInt32 index = bodyInfoIndexArray[i];
		_ASSERTE (index);
		dgBody* const body = bodyArray[index].m_body;

		body->m_veloc = internalVeloc[index].m_linear.Scale(invStep);
		body->m_omega = internalVeloc[index].m_angular.Scale(invStep);

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
	world->SyncThreads(&syncData->m_updateBodyVelocitySync);

	if (hasJointFeeback) {
		dgInt32* const m_updateForcefeebackCounter = &syncData->m_updateForcefeebackCounter;
		for (dgInt32 i = dgAtomicAdd(m_updateForcefeebackCounter, 1); i < syncData->m_jointCount; i = dgAtomicAdd(m_updateForcefeebackCounter, 1)) {
			dgInt32 curJoint = jointInfoIndexArray[i];
			if (constraintArray[curJoint].m_joint->m_updaFeedbackCallback) {
				constraintArray[curJoint].m_joint->m_updaFeedbackCallback (*constraintArray[curJoint].m_joint, syncData->m_timestep, threadIndex);
			}
		}
		world->SyncThreads(&syncData->m_updateForcefeebackSync);
	}
}

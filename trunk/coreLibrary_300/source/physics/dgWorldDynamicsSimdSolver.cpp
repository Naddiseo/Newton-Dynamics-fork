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

#ifdef DG_BUILD_SIMD_CODE

void dgWorldDynamicUpdate::CalculateIslandReactionForcesSimd (const dgIsland* const island, dgFloat32 timestep, dgInt32 threadID) const
{
	if (!island->m_isContinueCollision) {
		dgInt32 rowBase = BuildJacobianMatrixSimd (island, threadID, timestep);
		CalculateReactionsForcesSimd (island, rowBase, threadID, timestep, DG_SOLVER_MAX_ERROR);
		IntegrateArray (island, DG_SOLVER_MAX_ERROR, timestep, threadID, true); 

	} else {
		_ASSERTE (0);
	}
}

dgInt32 dgWorldDynamicUpdate::BuildJacobianMatrixSimd (const dgIsland* const island, dgInt32 threadIndex, dgFloat32 timestep) const
{
	_ASSERTE (island->m_bodyCount >= 2);
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
		body->CalcInvInertiaMatrixSimd ();
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

//	dgFloat32* const force = &m_solverMemory.m_force[rowBase];
//	const dgJacobianPair* const Jt = &m_solverMemory.m_Jt[rowBase];
//	dgJacobianPair* const JMinv = &m_solverMemory.m_JMinv[rowBase];
//	dgFloat32* const diagDamp = &m_solverMemory.m_diagDamp[rowBase];
//	dgFloat32* const extAccel = &m_solverMemory.m_deltaAccel[rowBase];
//	dgFloat32* const invDJMinvJt = &m_solverMemory.m_invDJMinvJt[rowBase];
//	dgFloat32* const coordenateAccel = &m_solverMemory.m_coordenateAccel[rowBase];
//	dgFloat32** const jointForceFeeback = &m_solverMemory.m_jointFeebackForce[rowBase];

	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowBase];

	simd_type zero = simd_set1 (dgFloat32 (0.0f));
	for (dgInt32 k = 0; k < jointCount; k ++) {
		const dgJointInfo* const jointInfo = &constraintArray[k];
		dgInt32 index = jointInfo->m_autoPairstart;
		dgInt32 count = jointInfo->m_autoPaircount;
		dgInt32 m0 = jointInfo->m_m0;
		dgInt32 m1 = jointInfo->m_m1;

		_ASSERTE (m0 >= 0);
		_ASSERTE (m0 < bodyCount);
		dgBody* const body0 = bodyArray[m0].m_body;
		//dgFloat32 invMass0 = body0->m_invMass[3];
		simd_type invMass0 = simd_set1 (body0->m_invMass[3]);
		const dgMatrix& invInertia0 = body0->m_invWorldInertiaMatrix;

		_ASSERTE (m1 >= 0);
		_ASSERTE (m1 < bodyCount);
		dgBody* const body1 = bodyArray[m1].m_body;
		//dgFloat32 invMass1 = body1->m_invMass[3];
		simd_type invMass1 = simd_set1 (body1->m_invMass[3]);
		const dgMatrix& invInertia1 = body1->m_invWorldInertiaMatrix;

		for (dgInt32 i = 0; i < count; i ++) {
			//JMinv[index].m_jacobian_IM0.m_linear = Jt[index].m_jacobian_IM0.m_linear.Scale (invMass0);
			//JMinv[index].m_jacobian_IM0.m_angular = invInertia0.UnrotateVector (Jt[index].m_jacobian_IM0.m_angular);
			//dgVector tmpDiag (JMinv[index].m_jacobian_IM0.m_linear.CompProduct(Jt[index].m_jacobian_IM0.m_linear));
			//tmpDiag += JMinv[index].m_jacobian_IM0.m_angular.CompProduct(Jt[index].m_jacobian_IM0.m_angular);
			//dgVector tmpAccel (JMinv[index].m_jacobian_IM0.m_linear.CompProduct(body0->m_accel));
			//tmpAccel += JMinv[index].m_jacobian_IM0.m_angular.CompProduct(body0->m_alpha);

			dgJacobianMatrixElement* const row = &matrixRow[index];
			
			((simd_type&)row->m_JMinv.m_jacobian_IM0.m_linear) = simd_mul_v ((simd_type&)row->m_Jt.m_jacobian_IM0.m_linear, invMass0);
			simd_type tmp0 = (simd_type&)row->m_Jt.m_jacobian_IM0.m_angular;
			simd_type tmp1 = simd_mul_v ((simd_type&)invInertia0.m_front, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 0, 0, 0)));
			tmp1 = simd_mul_add_v (tmp1, (simd_type&)invInertia0.m_up, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 1, 1, 1)));
			((simd_type&)row->m_JMinv.m_jacobian_IM0.m_angular) = simd_mul_add_v (tmp1, (simd_type&)invInertia0.m_right, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 2, 2, 2)));
			simd_type tmpDiag = simd_mul_v ((simd_type&)row->m_JMinv.m_jacobian_IM0.m_linear, (simd_type&)row->m_Jt.m_jacobian_IM0.m_linear);
			tmpDiag = simd_mul_add_v (tmpDiag, (simd_type&)row->m_JMinv.m_jacobian_IM0.m_angular, (simd_type&)row->m_Jt.m_jacobian_IM0.m_angular);
			simd_type tmpAccel = simd_mul_v ((simd_type&)row->m_JMinv.m_jacobian_IM0.m_linear, (simd_type&)body0->m_accel);
			tmpAccel = simd_mul_add_v (tmpAccel, (simd_type&)row->m_JMinv.m_jacobian_IM0.m_angular, (simd_type&)body0->m_alpha);


			//JMinv[index].m_jacobian_IM1.m_linear = Jt[index].m_jacobian_IM1.m_linear.Scale (invMass1);
			//JMinv[index].m_jacobian_IM1.m_angular = invInertia1.UnrotateVector (Jt[index].m_jacobian_IM1.m_angular);
			//tmpDiag += JMinv[index].m_jacobian_IM1.m_linear.CompProduct(Jt[index].m_jacobian_IM1.m_linear);
			//tmpDiag += JMinv[index].m_jacobian_IM1.m_angular.CompProduct(Jt[index].m_jacobian_IM1.m_angular);
			//tmpAccel += JMinv[index].m_jacobian_IM1.m_linear.CompProduct(body1->m_accel);
			//tmpAccel += JMinv[index].m_jacobian_IM1.m_angular.CompProduct(body1->m_alpha);
			
			((simd_type&)row->m_JMinv.m_jacobian_IM1.m_linear) = simd_mul_v ((simd_type&)row->m_Jt.m_jacobian_IM1.m_linear, invMass1);
			tmp0 = (simd_type&)row->m_Jt.m_jacobian_IM1.m_angular;
			tmp1 = simd_mul_v ((simd_type&)invInertia1.m_front, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 0, 0, 0)));
			tmp1 = simd_mul_add_v (tmp1, (simd_type&)invInertia1.m_up, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 1, 1, 1)));
			((simd_type&)row->m_JMinv.m_jacobian_IM1.m_angular) = simd_mul_add_v (tmp1, (simd_type&)invInertia1.m_right, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 2, 2, 2)));
			tmpDiag = simd_mul_add_v (tmpDiag, (simd_type&)row->m_JMinv.m_jacobian_IM1.m_linear, (simd_type&)row->m_Jt.m_jacobian_IM1.m_linear);
			tmpDiag = simd_mul_add_v (tmpDiag, (simd_type&)row->m_JMinv.m_jacobian_IM1.m_angular, (simd_type&)row->m_Jt.m_jacobian_IM1.m_angular);
			tmpAccel = simd_mul_add_v (tmpAccel, (simd_type&)row->m_JMinv.m_jacobian_IM1.m_linear, (simd_type&)body1->m_accel);
			tmpAccel = simd_mul_add_v (tmpAccel, (simd_type&)row->m_JMinv.m_jacobian_IM1.m_angular, (simd_type&)body1->m_alpha);

			//dgFloat32 extenalAcceleration = -(tmpAccel.m_x + tmpAccel.m_y + tmpAccel.m_z);
			//extAccel[index] = extenalAcceleration;
			//coordenateAccel[index] += extenalAcceleration;

			_ASSERTE (tmpAccel.m128_f32[3] == dgFloat32 (0.0f));
			tmpAccel = simd_add_v (tmpAccel, simd_move_hl_v(tmpAccel, tmpAccel));
			tmpAccel = simd_sub_s (zero, simd_add_s(tmpAccel, simd_permut_v (tmpAccel, tmpAccel, PURMUT_MASK(3, 3, 3, 1))));
			//simd_store_s (tmpAccel, &extAccel[index]);
			simd_store_s (tmpAccel, &row->m_deltaAccel);
			simd_store_s (simd_add_s(simd_load_s(row->m_coordenateAccel), tmpAccel), &row->m_coordenateAccel);

			row->m_force = row->m_jointFeebackForce[0];
			//simd_store_s (simd_load_s (row->m_jointFeebackForce[0]), &row->m_force);

			_ASSERTE (row->m_diagDamp >= dgFloat32(0.1f));
			_ASSERTE (row->m_diagDamp <= dgFloat32(100.0f));
			dgFloat32 stiffness = DG_PSD_DAMP_TOL * row->m_diagDamp;

			//dgFloat32 diag = (tmpDiag.m_x + tmpDiag.m_y + tmpDiag.m_z);
			tmpDiag = simd_add_v (tmpDiag, simd_move_hl_v(tmpDiag, tmpDiag));
			dgFloat32 diag;
			simd_store_s (simd_add_s(tmpDiag, simd_permut_v (tmpDiag, tmpDiag, PURMUT_MASK(3, 3, 3, 1))), &diag);
			_ASSERTE (diag > dgFloat32 (0.0f));
			row->m_diagDamp = diag * stiffness;

			diag *= (dgFloat32(1.0f) + stiffness);
			//solverMemory.m_diagJMinvJt[index] = diag;
			row->m_invDJMinvJt = dgFloat32(1.0f) / diag;

			index ++;
		}
	}

	return rowBase;
}




void dgWorldDynamicUpdate::CalculateReactionsForcesSimd (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const
{
	if (island->m_jointCount == 0) {
		ApplyExternalForcesAndAccelerationSimd (island, rowStart, threadIndex, timestep, 0.0f);
		return;
	}

	if (island->m_jointCount == 1) {
		CalculateSimpleBodyReactionsForcesSimd (island, rowStart, threadIndex, timestep, maxAccNorm);
		ApplyExternalForcesAndAccelerationSimd (island, rowStart, threadIndex, timestep, maxAccNorm * dgFloat32 (0.001f));
		return;
	}

	dgWorld* const world = (dgWorld*) this;
	if (world->m_solverMode) {
		CalculateForcesGameModeSimd (island, rowStart, threadIndex, timestep, maxAccNorm);
	} else {
		_ASSERTE (0);
//		CalculateForcesSimulationMode (tolerance);
	}
}



void dgWorldDynamicUpdate::ApplyExternalForcesAndAccelerationSimd (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestepSrc, dgFloat32 maxAccNorm) const
{
	dgJacobian* const internalForces = &m_solverMemory.m_internalForces[island->m_bodyStart];

//	dgVector zero (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
//	dgInt32 bodyCount = island->m_bodyCount;
//	for (dgInt32 i = 0; i < bodyCount; i ++) {
//		internalForces[i].m_linear = zero;
//		internalForces[i].m_angular = zero;
//	}
	simd_type zero = simd_set1(dgFloat32 (0.0f));
	dgInt32 bodyCount = island->m_bodyCount;
	for (dgInt32 i = 0; i < bodyCount; i ++) {
		(simd_type&)internalForces[i].m_linear = zero;
		(simd_type&)internalForces[i].m_angular = zero;
	}


	dgInt32 hasJointFeeback = 0;

	dgInt32 jointCount = island->m_jointCount;
	dgWorld* const world = (dgWorld*) this;
	dgJointInfo* const constraintArrayPtr = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJointInfo* const constraintArray = &constraintArrayPtr[island->m_jointStart];

//	dgBodyInfo* const bodyArrayPtr = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
//	dgBodyInfo* const bodyArray = &bodyArrayPtr[island->m_bodyStart];
//	dgFloat32* const force = &m_solverMemory.m_force[rowStart];
//	const dgJacobianPair* const Jt = &m_solverMemory.m_Jt[rowStart];
//	dgFloat32** const jointForceFeeback = &m_solverMemory.m_jointFeebackForce[rowStart];

	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowStart];

	simd_type timestep = simd_set1(timestepSrc);
	simd_type toleranceSimd = simd_set1(maxAccNorm);
	toleranceSimd = simd_mul_s (toleranceSimd, toleranceSimd);
	for (dgInt32 i = 0; i < jointCount; i ++) {
		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPaircount;
		dgInt32 m0 = constraintArray[i].m_m0;
		dgInt32 m1 = constraintArray[i].m_m1;

//		dgJacobian y0;
//		dgJacobian y1;
//		y0.m_linear = zero;
//		y0.m_angular = zero;
//		y1.m_linear = zero;
//		y1.m_angular = zero;
		simd_type y0_linear = zero;
		simd_type y0_angular = zero;
		simd_type y1_linear = zero;
		simd_type y1_angular = zero;
		for (dgInt32 j = 0; j < count; j ++) { 
			//dgInt32 index = j + first;
			//dgFloat32 val = force[index]; 
			//_ASSERTE (dgCheckFloat(val));
			//jointForceFeeback[index][0] = val;
			dgJacobianMatrixElement* const row = &matrixRow[j + first];

			_ASSERTE (dgCheckFloat(row->m_force));
			simd_type val = simd_set1 (row->m_force);
			simd_store_s (val, &row->m_jointFeebackForce[0]);

			//y0.m_linear += Jt[index].m_jacobian_IM0.m_linear.Scale (val);
			//y0.m_angular += Jt[index].m_jacobian_IM0.m_angular.Scale (val);
			//y1.m_linear += Jt[index].m_jacobian_IM1.m_linear.Scale (val);
			//y1.m_angular += Jt[index].m_jacobian_IM1.m_angular.Scale (val);
			y0_linear = simd_mul_add_v (y0_linear, (simd_type&)row->m_Jt.m_jacobian_IM0.m_linear, val);
			y0_angular = simd_mul_add_v (y0_angular, (simd_type&)row->m_Jt.m_jacobian_IM0.m_angular, val);
			y1_linear = simd_mul_add_v (y1_linear, (simd_type&)row->m_Jt.m_jacobian_IM1.m_linear, val);
			y1_angular = simd_mul_add_v (y1_angular, (simd_type&)row->m_Jt.m_jacobian_IM1.m_angular, val);
		}

		//if (constraintArray[i].m_joint->GetId() == dgContactConstraintId) {
		//		m_world->AddToBreakQueue ((dgContact*)constraintArray[i].m_joint, maxForce);
		//}
		hasJointFeeback |= (constraintArray[i].m_joint->m_updaFeedbackCallback ? 1 : 0);

		//internalForces[m0].m_linear += y0.m_linear;
		//internalForces[m0].m_angular += y0.m_angular;
		//internalForces[m1].m_linear += y1.m_linear;
		//internalForces[m1].m_angular += y1.m_angular;
		(simd_type&)internalForces[m0].m_linear = simd_add_v ((simd_type&)internalForces[m0].m_linear, y0_linear);
		(simd_type&)internalForces[m0].m_angular = simd_add_v ((simd_type&)internalForces[m0].m_angular, y0_angular);
		(simd_type&)internalForces[m1].m_linear = simd_add_v ((simd_type&)internalForces[m1].m_linear, y1_linear);
		(simd_type&)internalForces[m1].m_angular = simd_add_v ((simd_type&)internalForces[m1].m_angular, y1_angular);
	}

//	dgFloat32 accelTol2 = maxAccNorm * maxAccNorm;
	dgBodyInfo* const bodyArrayPtr = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	dgBodyInfo* const bodyArray = &bodyArrayPtr[island->m_bodyStart];
	for (dgInt32 i = 1; i < bodyCount; i ++) {
		dgBody* const body = bodyArray[i].m_body;
		//body->m_accel += internalForces[i].m_linear;
		//body->m_alpha += internalForces[i].m_angular;
		(simd_type&)body->m_accel = simd_add_v ((simd_type&)internalForces[i].m_linear, (simd_type&)body->m_accel);
		(simd_type&)body->m_alpha = simd_add_v ((simd_type&)internalForces[i].m_angular, (simd_type&)body->m_alpha);

		//dgVector accel (body->m_accel.Scale (body->m_invMass.m_w));
		//dgVector alpha (body->m_invWorldInertiaMatrix.RotateVector (body->m_alpha));
		simd_type accel = simd_mul_v ((simd_type&)body->m_accel, simd_set1 (body->m_invMass.m_w));
		simd_type alpha = simd_mul_add_v (simd_mul_add_v (simd_mul_v ((simd_type&)body->m_invWorldInertiaMatrix[0], simd_set1 (body->m_alpha.m_x)), 
																	  (simd_type&)body->m_invWorldInertiaMatrix[1], simd_set1 (body->m_alpha.m_y)), 
																	  (simd_type&)body->m_invWorldInertiaMatrix[2], simd_set1 (body->m_alpha.m_z));
		//dgFloat32 error = accel % accel;
		//if (error < accelTol2) {
		//	accel = zero;
		//	body->m_accel = zero;
		//}
		simd_type tmp = simd_mul_v (accel, accel);
		tmp = simd_add_v (tmp, simd_move_hl_v (tmp, tmp));
		tmp = simd_add_s (tmp, simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 1)));
		tmp = simd_cmplt_s (tmp, toleranceSimd);
		tmp = simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 0));
		accel = simd_andnot_v (accel, tmp);
		(simd_type&)body->m_accel = simd_andnot_v ((simd_type&)body->m_accel, tmp);

		//error = alpha % alpha;
		//if (error < accelTol2) {
		//	alpha = zero;
		//	body->m_alpha = zero;
		//}
		tmp = simd_mul_v (alpha, alpha);
		tmp = simd_add_v (tmp, simd_move_hl_v (tmp, tmp));
		tmp = simd_add_s (tmp, simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 1)));
		tmp = simd_cmplt_s (tmp, toleranceSimd);
		tmp = simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 0));
		alpha = simd_andnot_v (alpha, tmp);
		(simd_type&)body->m_alpha = simd_andnot_v ((simd_type&)body->m_alpha, tmp);

		//body->m_netForce = body->m_accel;
		//body->m_netTorque = body->m_alpha;
		//body->m_veloc += accel.Scale(timestep);
		//body->m_omega += alpha.Scale(timestep);

		(simd_type&)body->m_netForce = (simd_type&)body->m_accel;
		(simd_type&)body->m_netTorque = (simd_type&)body->m_alpha;
		(simd_type&)body->m_veloc = simd_mul_add_v ((simd_type&)body->m_veloc, accel, timestep);
		(simd_type&)body->m_omega = simd_mul_add_v ((simd_type&)body->m_omega, alpha, timestep);
	}

	if (hasJointFeeback) {
		for (dgInt32 i = 0; i < jointCount; i ++) {
			if (constraintArray[i].m_joint->m_updaFeedbackCallback) {
				constraintArray[i].m_joint->m_updaFeedbackCallback (*constraintArray[i].m_joint, timestepSrc, threadIndex);
			}
		}
	}
}


void dgWorldDynamicUpdate::CalculateSimpleBodyReactionsForcesSimd (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const
{
/*
	dgFloatSign tmpIndex;
	tmpIndex.m_integer.m_iVal = 0x7fffffff;
	simd_type signMask = simd_set1(tmpIndex.m_fVal);
	simd_type one = simd_set1(dgFloat32 (1.0f));
	simd_type zero = simd_set1(dgFloat32 (0.0f));
	simd_type tol_pos_1eNeg16 = simd_set1 (dgFloat32 ( 1.0e-16f));



//	dgFloat32 clampedForceIndexValue = dgFloat32(0.0f);
//	dgFloat32 akNum;
	dgFloat32 accNorm;
//	simd_store_s (tmp0, &akNum);
	simd_store_s (accNormTmp, &accNorm);
	for (dgInt32 i = 0; (i < maxPasses) && (accNorm > maxAccNorm); i ++) {

		
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
			dgFloat32 akNum1 = dgFloat32 (0.0f);
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
							akNum1 += accel[k] * deltaForce[k];
							accNorm = GetMax (dgAbsf (accel[k]), accNorm);
						}
					}
				}
			}

			akNum = simd_set1 (akNum1);
			

			i = -1;
			maxPasses = GetMax (maxPasses - 1, 1); 

		} else if (clampedForceIndex >= 0) {
			dgFloat32 akNum1 = dgFloat32(0.0f);
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
				akNum1 += deltaForce[k] * accel[k];
			}
			matrixRow[clampedForceIndex].m_force = clampedForceIndexValue;

			akNum = simd_set1 (akNum1);
			

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
				dgFloat32 akDen;
				simd_store_s (akNum, &akDen);

				dgFloat32 akNum1 = dgFloat32(0.0f);
				for (dgInt32 k = 0; k < count; k ++) {
					deltaAccel[k] = accel[k] * matrixRow[k].m_invDJMinvJt * activeRow[k];
					akNum1 += accel[k] * deltaAccel[k];
				}

				_ASSERTE (akDen > dgFloat32(0.0f));
				akDen = GetMax (akDen, dgFloat32 (1.0e-17f));
				dgFloat32 ak = dgFloat32 (akNum1 / akDen);
				for (dgInt32 k = 0; k < count; k ++) {
					deltaForce[k] = deltaAccel[k] + ak * deltaForce[k];
				}
			}
		}
	}
*/


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

	simd_128 one(dgFloat32 (1.0f));
	simd_128 passesCount (dgFloat32 (0.0f));
	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowStart];
	for (dgInt32 i = 0; i < count; i ++) {
		dgJacobianMatrixElement* const row = &matrixRow[i];

		dgInt32 k = row->m_normalForceIndex;
		_ASSERTE (((k <0) && (matrixRow[k].m_force == dgFloat32 (1.0f))) || ((k >= 0) && (matrixRow[k].m_force >= dgFloat32 (0.0f))));
		simd_128 val (matrixRow[k].m_force);
		simd_128 low (val * simd_128 (row->m_lowerBoundFrictionCoefficent));
		simd_128 high (val * simd_128 (row->m_upperBoundFrictionCoefficent));
		low.StoreScalar(&lowBound[i]);
		high.StoreScalar(&highBound[i]);

		simd_128 force (row->m_force);
		simd_128 test ((force < low) | (force > high));
		simd_128 unit (one.AndNot(test));
		unit.StoreScalar(&activeRow[i]);
		passesCount = passesCount + unit;
		high.GetMin(low.GetMax(force)).StoreScalar(&row->m_force);
	}
	dgInt32 maxPasses = passesCount.GetInt();

	simd_128 zero(dgFloat32 (0.0f));
	simd_128 y0_linear (zero);
	simd_128 y0_angular (zero);
	simd_128 y1_linear (zero);
	simd_128 y1_angular (zero);
	for (dgInt32 i = 0; i < count; i ++) {
		dgJacobianMatrixElement* const row = &matrixRow[i];
		simd_128 val (row->m_force);
		y0_linear = y0_linear + ((simd_128&)row->m_Jt.m_jacobian_IM0.m_linear) * val;
		y0_angular = y0_angular + ((simd_128&)row->m_Jt.m_jacobian_IM0.m_angular) * val;
		y1_linear = y1_linear + ((simd_128&)row->m_Jt.m_jacobian_IM1.m_linear) * val;
		y1_angular = y1_angular + ((simd_128&)row->m_Jt.m_jacobian_IM1.m_angular) * val;
	}

	simd_128 akNumSimd (zero);
	simd_128 accNormSimd(zero);
	for (dgInt32 i = 0; i < count; i ++) {
		dgJacobianMatrixElement* const row = &matrixRow[i];
		simd_128 acc ((simd_128&)row->m_JMinv.m_jacobian_IM0.m_linear * y0_linear +
					  (simd_128&)row->m_JMinv.m_jacobian_IM0.m_angular * y0_angular +
					  (simd_128&)row->m_JMinv.m_jacobian_IM1.m_linear * y1_linear +
					  (simd_128&)row->m_JMinv.m_jacobian_IM1.m_angular * y1_angular);
		_ASSERTE (acc.m_type.m128_f32[3] == dgFloat32 (0.0f));

		simd_128 acceleration (simd_128 (row->m_coordenateAccel) - acc.AddHorizontal() - simd_128(row->m_force) * simd_128(row->m_diagDamp));
		acceleration.StoreScalar(&accel[i]);
		simd_128 dForce (acceleration * simd_128(row->m_invDJMinvJt) * simd_128 (activeRow[i]));
		dForce.StoreScalar(&deltaForce[i]);
		akNumSimd = akNumSimd + acceleration * dForce;
		accNormSimd = accNormSimd.GetMax(acceleration.Abs() * simd_128 (activeRow[i]));
	}

	dgFloat32 akNum;
	dgFloat32 accNorm;
	akNumSimd.StoreScalar(&akNum);
	accNormSimd.StoreScalar(&accNorm);
	
	simd_128 tol_Pos1eNeg8 (dgFloat32 ( 1.0e-8f));
	simd_128 tol_Pos1eNeg16 (dgFloat32 ( 1.0e-16f));
	simd_128 tol_Neg1eNeg16 (dgFloat32 ( 1.0e-16f));
	for (dgInt32 i = 0; (i < maxPasses) && (accNorm > maxAccNorm); i ++) {

		simd_128 y0_linear (zero);
		simd_128 y0_angular (zero);
		simd_128 y1_linear (zero);
		simd_128 y1_angular (zero);
		for (dgInt32 k = 0; k < count; k ++) {
			dgJacobianMatrixElement* const row = &matrixRow[k];
			simd_128 val (deltaForce[k]);
			y0_linear = y0_linear + ((simd_128&)row->m_Jt.m_jacobian_IM0.m_linear) * val;
			y0_angular = y0_angular + ((simd_128&)row->m_Jt.m_jacobian_IM0.m_angular) * val;
			y1_linear = y1_linear + ((simd_128&)row->m_Jt.m_jacobian_IM1.m_linear) * val;
			y1_angular = y1_angular + ((simd_128&)row->m_Jt.m_jacobian_IM1.m_angular) * val;
		}


		simd_128 akDen(dgFloat32 (0.0f));
		for (dgInt32 k = 0; k < count; k ++) {
			dgJacobianMatrixElement* const row = &matrixRow[k];
			simd_128 acc ((simd_128&)row->m_JMinv.m_jacobian_IM0.m_linear * y0_linear +
						  (simd_128&)row->m_JMinv.m_jacobian_IM0.m_angular * y0_angular +
						  (simd_128&)row->m_JMinv.m_jacobian_IM1.m_linear * y1_linear +
						  (simd_128&)row->m_JMinv.m_jacobian_IM1.m_angular * y1_angular);

			simd_128 dForce (deltaForce[k]);
			simd_128 dAccel (acc.AddHorizontal() + dForce * simd_128(row->m_diagDamp));
			dAccel.StoreScalar(&deltaAccel[k]);
			akDen = akDen + dAccel * dForce;
		}

		_ASSERTE (akDen.m_type.m128_f32[0] > dgFloat32 (0.0f));
		_ASSERTE (akDen.m_type.m128_f32[1] > dgFloat32 (0.0f));
		_ASSERTE (akDen.m_type.m128_f32[2] > dgFloat32 (0.0f));
		_ASSERTE (akDen.m_type.m128_f32[3] > dgFloat32 (0.0f));
		akDen = akDen.GetMax (tol_Pos1eNeg16);
		simd_128 akSimd (akNumSimd / akDen);

		dgInt32 clampedForceIndex = -1;
		simd_128 clampedForceIndexValueSimd (dgFloat32(0.0f));
		for (dgInt32 k = 0; k < count; k ++) {
			if (activeRow[k]) {
				dgJacobianMatrixElement* const row = &matrixRow[k];
				simd_128 force (row->m_force);
				simd_128 dForce (deltaForce[k]);
				simd_128 val__ (force + akSimd * dForce);
				if ((dForce < tol_Neg1eNeg16).GetInt()) {
					simd_128 low(lowBound[k])	;
					if ((val__ < low).GetInt()) {
						akSimd = zero.GetMax ((low - force) / dForce);
						clampedForceIndex = k;
						clampedForceIndexValueSimd = low;
						simd_128 test (akSimd < tol_Pos1eNeg8);
						akSimd = akSimd.GetMax(zero);
						if (test.GetInt()) {
							break;
						}
					}
				} else if ((dForce > tol_Pos1eNeg16).GetInt()) {
					simd_128 high(highBound[k])	;
					if ((val__ > high).GetInt()) {
						akSimd = zero.GetMax ((high - force) / dForce);
						clampedForceIndex = k;
						clampedForceIndexValueSimd = high;
						simd_128 test (akSimd < tol_Pos1eNeg8);
						akSimd = akSimd.GetMax(zero);
						if (test.GetInt()) {
							break;
						}
					}
				}
			}
		}


		dgFloat32 ak;
		akSimd.StoreScalar(&ak);
		dgFloat32 clampedForceIndexValue;
		clampedForceIndexValueSimd.StoreScalar(&clampedForceIndexValue);

		accNormSimd = zero;
		if (ak == dgFloat32 (0.0f) && (clampedForceIndex != -1)) {
			_ASSERTE (clampedForceIndex !=-1);
			akNum = dgFloat32 (0.0f);

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
							//accNorm = GetMax (dgAbsf (accel[k]), accNorm);
							accNormSimd = accNormSimd.GetMax (simd_128(accel[k]).Abs());
						}
					}
				}
			}


			i = -1;
			maxPasses = GetMax (maxPasses - 1, 1); 

		} else if (clampedForceIndex >= 0) {
			akNum = dgFloat32(0.0f);
			activeRow[clampedForceIndex] = dgFloat32 (0.0f);
			for (dgInt32 k = 0; k < count; k ++) {
				dgJacobianMatrixElement* const row = &matrixRow[k];
				row->m_force += ak * deltaForce[k];
				accel[k] -= ak * deltaAccel[k];
				//accNorm = GetMax (dgAbsf (accel[k] * activeRow[k]), accNorm);
				accNormSimd = accNormSimd.GetMax(simd_128(accel[k]).Abs() * simd_128(activeRow[k]));
				_ASSERTE (dgCheckFloat(row->m_force));
				_ASSERTE (dgCheckFloat(accel[k]));

				deltaForce[k] = accel[k] * row->m_invDJMinvJt * activeRow[k];
				akNum += deltaForce[k] * accel[k];
			}
			matrixRow[clampedForceIndex].m_force = clampedForceIndexValue;

			i = -1;
			maxPasses = GetMax (maxPasses - 1, 1); 

		} else {
			//accNorm = dgFloat32(0.0f);
			for (dgInt32 k = 0; k < count; k ++) {
				dgJacobianMatrixElement* const row = &matrixRow[k];
				row->m_force += ak * deltaForce[k];
				accel[k] -= ak * deltaAccel[k];
				//accNorm = GetMax (dgAbsf (accel[k] * activeRow[k]), accNorm);
				accNormSimd = accNormSimd.GetMax(simd_128(accel[k]).Abs() * simd_128(activeRow[k]));
				_ASSERTE (dgCheckFloat(row->m_force));
				_ASSERTE (dgCheckFloat(accel[k]));
			}

			if (accNorm > maxAccNorm) {
				dgFloat32 akDen = akNum;
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
		akNumSimd = simd_128 (akNum);
		accNormSimd.StoreScalar(&accNorm);
	}
}


void dgWorldDynamicUpdate::CalculateForcesGameModeSimd (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestepSrc, dgFloat32 maxAccNorm) const
{
	dgJacobian* const internalVeloc = &m_solverMemory.m_internalVeloc[island->m_bodyStart];
	dgJacobian* const internalForces = &m_solverMemory.m_internalForces[island->m_bodyStart];

//	dgVector zero(dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	simd_128 zero (dgFloat32 (0.0f));

	//	sleepCount = 0;
	dgInt32 bodyCount = island->m_bodyCount;
	dgWorld* const world = (dgWorld*) this;
	dgBodyInfo* const bodyArrayPtr = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	dgBodyInfo* const bodyArray = &bodyArrayPtr[island->m_bodyStart];
	for (dgInt32 i = 1; i < bodyCount; i ++) {

		dgBody* const body = bodyArray[i].m_body;
		(simd_128&) internalVeloc[i].m_linear = zero;
		(simd_128&) internalVeloc[i].m_angular = zero;
		(simd_128&) internalForces[i].m_linear = zero;
		(simd_128&) internalForces[i].m_angular = zero;
		(simd_128&) body->m_netForce = (simd_128&) body->m_veloc;
		(simd_128&) body->m_netTorque = (simd_128&) body->m_omega;
	}
	(simd_128&) internalVeloc[0].m_linear = zero;
	(simd_128&) internalVeloc[0].m_angular = zero;
	(simd_128&) internalForces[0].m_linear = zero;
	(simd_128&) internalForces[0].m_angular = zero;

	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowStart];

	dgInt32 jointCount = island->m_jointCount;
	dgJointInfo* const constraintArrayPtr = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJointInfo* const constraintArray = &constraintArrayPtr[island->m_jointStart];
	for (dgInt32 i = 0; i < jointCount; i ++) {
		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPaircount;

		dgInt32 m0 = constraintArray[i].m_m0;
		dgInt32 m1 = constraintArray[i].m_m1;

		//dgJacobian y0;
		//dgJacobian y1;
		simd_128 y0_linear = zero;
		simd_128 y0_angular = zero;
		simd_128 y1_linear = zero;
		simd_128 y1_angular = zero;
		for (dgInt32 j = 0; j < count; j ++) { 
			dgJacobianMatrixElement* const row = &matrixRow[j + first];
			_ASSERTE (dgCheckFloat(row->m_force));
			simd_128 val (row->m_force);
			y0_linear  = y0_linear + (simd_128&)row->m_Jt.m_jacobian_IM0.m_linear * val;
			y0_angular = y0_angular + (simd_128&) row->m_Jt.m_jacobian_IM0.m_angular * val;
			y1_linear  = y1_linear +  (simd_128&) row->m_Jt.m_jacobian_IM1.m_linear * val;
			y1_angular = y1_angular + (simd_128&) row->m_Jt.m_jacobian_IM1.m_angular * val;
		}
		internalForces[m0].m_linear = (simd_128&) internalForces[m0].m_linear + y0_linear;
		internalForces[m0].m_angular = (simd_128&) internalForces[m0].m_angular +  y0_angular;
		internalForces[m1].m_linear = (simd_128&) internalForces[m1].m_linear +  y1_linear;
		internalForces[m1].m_angular = (simd_128&)internalForces[m1].m_angular +  y1_angular;
	}


	dgFloat32 invTimestepSrc = dgFloat32 (1.0f) / timestepSrc;
	dgFloat32 invStep = (dgFloat32 (1.0f) / dgFloat32 (LINEAR_SOLVER_SUB_STEPS));
	dgFloat32 timestep =  timestepSrc * invStep;
	dgFloat32 invTimestep = invTimestepSrc * dgFloat32 (LINEAR_SOLVER_SUB_STEPS);
	_ASSERTE (bodyArray[0].m_body == world->m_sentionelBody);

//	dgFloatSign tmpIndex;
//	tmpIndex.m_integer.m_iVal = 0x7fffffff;
//	simd_type signMask = simd_set1(tmpIndex.m_fVal);
	simd_128 signMask (0x7fffffff);
	simd_128 timeStepSimd (timestep);
	dgFloat32 firstPassCoef = dgFloat32 (0.0f);
	dgInt32 maxPasses = dgInt32 (world->m_solverMode + DG_BASE_ITERATION_COUNT);

	for (dgInt32 step = 0; step < LINEAR_SOLVER_SUB_STEPS; step ++) {
		dgJointAccelerationDecriptor joindDesc;
		joindDesc.m_timeStep = timestep;
		joindDesc.m_invTimeStep = invTimestep;
		joindDesc.m_firstPassCoefFlag = firstPassCoef;

		for (dgInt32 curJoint = 0; curJoint < jointCount; curJoint ++) {
			joindDesc.m_rowsCount = constraintArray[curJoint].m_autoPaircount;
			joindDesc.m_rowMatrix = &matrixRow[constraintArray[curJoint].m_autoPairstart];
			constraintArray[curJoint].m_joint->JointAccelerationsSimd (&joindDesc);
		}

		firstPassCoef = dgFloat32 (1.0f);
		dgFloat32 accNorm = maxAccNorm * dgFloat32 (2.0f);
		for (dgInt32 passes = 0; (passes < maxPasses) && (accNorm > maxAccNorm); passes ++) {
			simd_128 accNormSimd = zero;

			for (dgInt32 curJoint = 0; curJoint < jointCount; curJoint ++) {
				dgInt32 index = constraintArray[curJoint].m_autoPairstart;
				dgInt32 rowsCount = constraintArray[curJoint].m_autoPaircount;
				dgInt32 m0 = constraintArray[curJoint].m_m0;
				dgInt32 m1 = constraintArray[curJoint].m_m1;

				simd_128 linearM0  = (simd_128&)internalForces[m0].m_linear;
				simd_128 angularM0 = (simd_128&)internalForces[m0].m_angular;
				simd_128 linearM1  = (simd_128&)internalForces[m1].m_linear;
				simd_128 angularM1 = (simd_128&)internalForces[m1].m_angular;

				for (dgInt32 k = 0; k < rowsCount; k ++) {
					dgJacobianMatrixElement* const row = &matrixRow[index];
					simd_128 a ((simd_128&)row->m_JMinv.m_jacobian_IM0.m_linear * linearM0 +
								(simd_128&)row->m_JMinv.m_jacobian_IM0.m_angular * angularM0 +
								(simd_128&)row->m_JMinv.m_jacobian_IM1.m_linear * linearM1 +
								(simd_128&)row->m_JMinv.m_jacobian_IM1.m_angular * angularM1);

					_ASSERTE (a.m_type.m128_f32[3] == dgFloat32 (0.0f));
					//dgFloat32 a = coordenateAccel[index] - acc.m_x - acc.m_y - acc.m_z - force[index] * diagDamp[index];
					//a = simd_add_v (a, simd_move_hl_v(a, a));
					//a = simd_add_s (a, simd_permut_v (a, a, PURMUT_MASK(3, 3, 3, 1)));
					//simd_type force = simd_load_s(row->m_force);
					//a = simd_sub_s(simd_load_s(row->m_coordenateAccel), simd_mul_add_s(a, force, simd_load_s(row->m_diagDamp)));
					simd_128 force (row->m_force);
					a = simd_128(row->m_coordenateAccel) - a.AddHorizontal() - force * simd_128(row->m_diagDamp);
					
					//dgFloat32 f = force[index] + invDJMinvJt[index] * a;
					//simd_type f = simd_mul_add_s (force, simd_load_s(row->m_invDJMinvJt), a);
					simd_128 f (force + a * simd_128(row->m_invDJMinvJt));

					dgInt32 frictionIndex = row->m_normalForceIndex;
					_ASSERTE (((frictionIndex < 0) && (matrixRow[frictionIndex].m_force == dgFloat32 (1.0f))) || ((frictionIndex >= 0) && (matrixRow[frictionIndex].m_force >= dgFloat32 (0.0f))));

					//dgFloat32 frictionNormal = force[frictionIndex];
					//dgFloat32 lowerFrictionForce = frictionNormal * lowerFrictionCoef[index];
					//dgFloat32 upperFrictionForce = frictionNormal * upperFrictionCoef[index];
					//simd_type frictionNormal = simd_load_s(matrixRow[frictionIndex].m_force);
					//simd_type lowerFrictionForce = simd_mul_s (frictionNormal, simd_load_s(row->m_lowerBoundFrictionCoefficent));
					//simd_type upperFrictionForce = simd_mul_s (frictionNormal, simd_load_s(row->m_upperBoundFrictionCoefficent));

					simd_128 frictionNormal (matrixRow[frictionIndex].m_force);
					simd_128 lowerFrictionForce (frictionNormal * simd_128(row->m_lowerBoundFrictionCoefficent));
					simd_128 upperFrictionForce (frictionNormal * simd_128(row->m_upperBoundFrictionCoefficent));


					//if (f > upperFrictionForce) {
					//	a = dgFloat32 (0.0f);
					//	f = upperFrictionForce;
					//} else if (f < lowerFrictionForce) {
					//	a = dgFloat32 (0.0f);
					//	f = lowerFrictionForce;
					//}
//					f = simd_min_s (simd_max_s (f, lowerFrictionForce), upperFrictionForce);
//					a = simd_andnot_v (a, simd_or_v (simd_cmplt_s (f, lowerFrictionForce), simd_cmpgt_s (f, upperFrictionForce)));

					f = f.GetMax(lowerFrictionForce).GetMin(upperFrictionForce);

//					a = simd_andnot_v (a, simd_or_v (simd_cmplt_s (f, lowerFrictionForce), simd_cmpgt_s (f, upperFrictionForce)));

/*
					//accNorm = GetMax (accNorm, dgAbsf (a));
					accNormSimd = simd_max_s (accNormSimd, simd_and_v (a, signMask));

					//dgFloat32 prevValue = f - force[index];
					simd_type prevValue = simd_sub_s (f, force);
					prevValue = simd_permut_v (prevValue, prevValue, PURMUT_MASK(0, 0, 0, 0));

					//force[index] = f;
					simd_store_s (f, &row->m_force);

					//linearM0 += Jt[index].m_jacobian_IM0.m_linear.Scale (prevValue);
					//angularM0 += Jt[index].m_jacobian_IM0.m_angular.Scale (prevValue);
					//linearM1 += Jt[index].m_jacobian_IM1.m_linear.Scale (prevValue);
					//angularM1 += Jt[index].m_jacobian_IM1.m_angular.Scale (prevValue);
					linearM0 = simd_mul_add_v (linearM0, (simd_type&) row->m_Jt.m_jacobian_IM0.m_linear, prevValue);
					angularM0 = simd_mul_add_v (angularM0,(simd_type&) row->m_Jt.m_jacobian_IM0.m_angular, prevValue);
					linearM1 = simd_mul_add_v (linearM1, (simd_type&) row->m_Jt.m_jacobian_IM1.m_linear, prevValue);
					angularM1 = simd_mul_add_v (angularM1,(simd_type&) row->m_Jt.m_jacobian_IM1.m_angular, prevValue);
					index ++;
*/
				}
				internalForces[m0].m_linear = linearM0;
				internalForces[m0].m_angular = angularM0;
				internalForces[m1].m_linear = linearM1;
				internalForces[m1].m_angular = angularM1;
			}
			accNormSimd.StoreScalar(&accNorm);
		}
/*
		for (dgInt32 i = 1; i < bodyCount; i ++) {
			dgBody* const body = bodyArray[i].m_body;

			//dgVector force (body->m_accel + internalForces[i].m_linear);
			//dgVector torque (body->m_alpha + internalForces[i].m_angular);
			simd_type force = simd_add_v ((simd_type&) body->m_accel, (simd_type&)internalForces[i].m_linear);
			simd_type torque = simd_add_v ((simd_type&) body->m_alpha, (simd_type&)internalForces[i].m_angular);

			//dgVector accel (force.Scale (body->m_invMass.m_w));
			simd_type accel = simd_mul_v (force, simd_set1 (body->m_invMass.m_w));

			//dgVector alpha (body->m_invWorldInertiaMatrix.RotateVector (torque));
			simd_type alpha = simd_mul_add_v (simd_mul_add_v (simd_mul_v ((simd_type&)body->m_invWorldInertiaMatrix[0], simd_permut_v (torque, torque, PURMUT_MASK(0, 0, 0, 0))), 
											                              (simd_type&)body->m_invWorldInertiaMatrix[1], simd_permut_v (torque, torque, PURMUT_MASK(1, 1, 1, 1))), 
																	      (simd_type&)body->m_invWorldInertiaMatrix[2], simd_permut_v (torque, torque, PURMUT_MASK(2, 2, 2, 2)));

			//body->m_veloc += accel.Scale(timestep);
			//body->m_omega += alpha.Scale(timestep);
			(simd_type&) body->m_veloc = simd_mul_add_v ((simd_type&) body->m_veloc, accel, timeStepSimd);
			(simd_type&) body->m_omega = simd_mul_add_v ((simd_type&) body->m_omega, alpha, timeStepSimd);

			//internalVeloc[i].m_linear += body->m_veloc;
			//internalVeloc[i].m_angular += body->m_omega;
			(simd_type&)internalVeloc[i].m_linear = simd_add_v ((simd_type&)internalVeloc[i].m_linear, (simd_type&) body->m_veloc);
			(simd_type&)internalVeloc[i].m_angular = simd_add_v ((simd_type&)internalVeloc[i].m_angular, (simd_type&) body->m_omega);
		}
*/
	}

/*
	dgInt32 hasJointFeeback = 0;
	for (dgInt32 i = 0; i < jointCount; i ++) {
		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPaircount;

		for (dgInt32 j = 0; j < count; j ++) { 
			//dgInt32 index = j + first;
			//dgFloat32 val = force[index]; 
			dgJacobianMatrixElement* const row = &matrixRow[j + first];
			dgFloat32 val = row->m_force; 
			//maxForce = GetMax (dgAbsf (val), maxForce);
			_ASSERTE (dgCheckFloat(val));
			//jointForceFeeback[index][0] = val;
			row->m_jointFeebackForce[0] = val;
		}
		//if (constraintArray[i].m_joint->GetId() == dgContactConstraintId) {
		//	m_world->AddToBreakQueue ((dgContact*)constraintArray[i].m_joint, maxForce);
		//}

		hasJointFeeback |= (constraintArray[i].m_joint->m_updaFeedbackCallback ? 1 : 0);
	}

//	dgFloat32 maxAccNorm2 = maxAccNorm * maxAccNorm;
	simd_type invStepSimd = simd_set1 (invStep);	
	simd_type invTimeStepSimd = simd_set1 (invTimestepSrc);	
	simd_type accelerationTolerance = simd_set1 (maxAccNorm);	
	accelerationTolerance = simd_mul_s (accelerationTolerance, accelerationTolerance);
	for (dgInt32 i = 1; i < bodyCount; i ++) {
		dgBody* const body = bodyArray[i].m_body;

		//body->m_veloc = internalVeloc[i].m_linear.Scale(invStep);
		//body->m_omega = internalVeloc[i].m_angular.Scale(invStep);
		(simd_type&) body->m_veloc = simd_mul_v ((simd_type&) internalVeloc[i].m_linear, invStepSimd);
		(simd_type&) body->m_omega = simd_mul_v ((simd_type&) internalVeloc[i].m_angular, invStepSimd);

		//dgVector accel = (body->m_veloc - body->m_netForce).Scale (invTimestep);
		//dgVector alpha = (body->m_omega - body->m_netTorque).Scale (invTimestep);
		simd_type accel = simd_mul_v (simd_sub_v ((simd_type&) body->m_veloc, (simd_type&) body->m_netForce), invTimeStepSimd);
		simd_type alpha = simd_mul_v (simd_sub_v ((simd_type&) body->m_omega, (simd_type&) body->m_netTorque), invTimeStepSimd);

		//if ((accel % accel) < maxAccNorm2) {
		//	accel = zero;
		//}
		//if ((alpha % alpha) < maxAccNorm2) {
		//	alpha = zero;
		//}
		//body->m_accel = accel;
		//body->m_alpha = alpha;
		//body->m_netForce = accel.Scale (body->m_mass[3]);

		_ASSERTE (accel.m128_f32[3] == dgFloat32 (0.0f));
		simd_type tmp = simd_mul_v (accel, accel);
		tmp = simd_add_v (tmp, simd_move_hl_v (tmp, tmp));
		tmp = simd_add_s (tmp, simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 1)));
		tmp = simd_cmplt_s (tmp, accelerationTolerance);
		(simd_type&)body->m_accel = simd_andnot_v (accel, simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 0)));
		(simd_type&)body->m_netForce = simd_mul_v ((simd_type&)body->m_accel, simd_set1 (body->m_mass[3]));

		_ASSERTE (alpha.m128_f32[3] == dgFloat32 (0.0f));
		tmp = simd_mul_v (alpha, alpha);
		tmp = simd_add_v (tmp, simd_move_hl_v (tmp, tmp));
		tmp = simd_add_s (tmp, simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 1)));
		tmp = simd_cmplt_s (tmp, accelerationTolerance);
		(simd_type&)body->m_alpha = simd_andnot_v (alpha, simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 0)));

		//alpha = body->m_matrix.UnrotateVector(alpha);
		alpha = simd_mul_v ((simd_type&)body->m_matrix[0], (simd_type&)body->m_alpha);
		alpha = simd_add_v (alpha, simd_move_hl_v (alpha, alpha));
		alpha = simd_add_s (alpha, simd_permut_v (alpha, alpha, PURMUT_MASK(0, 0, 0, 1)));
		tmp = simd_mul_v ((simd_type&)body->m_matrix[1], (simd_type&)body->m_alpha);
		tmp = simd_add_v (tmp, simd_move_hl_v (tmp, tmp));
		tmp = simd_add_s (tmp, simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 1)));
		alpha = simd_pack_lo_v (alpha, tmp);

		tmp = simd_mul_v ((simd_type&)body->m_matrix[2], (simd_type&)body->m_alpha);
		tmp = simd_add_v (tmp, simd_move_hl_v (tmp, tmp));
		tmp = simd_add_s (tmp, simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 1)));
		alpha = simd_permut_v (alpha, tmp, PURMUT_MASK(3, 0, 1, 0));

		//body->m_netTorque = body->m_matrix.RotateVector (alpha.CompProduct(body->m_mass));
		alpha = simd_mul_v (alpha, (simd_type&)body->m_mass);
		(simd_type&)body->m_netTorque = simd_mul_add_v (simd_mul_add_v (simd_mul_v ((simd_type&)body->m_matrix[0], simd_permut_v (alpha, alpha, PURMUT_MASK(0, 0, 0, 0))), 
																				    (simd_type&)body->m_matrix[1], simd_permut_v (alpha, alpha, PURMUT_MASK(1, 1, 1, 1))), 
																					(simd_type&)body->m_matrix[2], simd_permut_v (alpha, alpha, PURMUT_MASK(2, 2, 2, 2)));
	}

	if (hasJointFeeback) {
		for (dgInt32 i = 0; i < jointCount; i ++) {
			if (constraintArray[i].m_joint->m_updaFeedbackCallback) {
				constraintArray[i].m_joint->m_updaFeedbackCallback (*constraintArray[i].m_joint, timestepSrc, threadIndex);
			}
		}
	}
*/
}

#endif

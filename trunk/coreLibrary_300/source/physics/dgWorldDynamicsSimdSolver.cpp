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


	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowBase];

	simd_128 one (dgFloat32 (1.0f));
	simd_128 zero (dgFloat32 (0.0f));
	simd_128 diagDampConst (DG_PSD_DAMP_TOL);
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
		simd_128 invMass0 (body0->m_invMass[3]);
//		const dgMatrix& invInertia0 = body0->m_invWorldInertiaMatrix;
		dgMatrix invInertiaTrans0;
		Transpose4x4Simd_128 ((simd_128&)invInertiaTrans0[0], (simd_128&)invInertiaTrans0[1], (simd_128&)invInertiaTrans0[2], (simd_128&)invInertiaTrans0[3], 
							  (simd_128&)body0->m_invWorldInertiaMatrix[0], (simd_128&)body0->m_invWorldInertiaMatrix[1], 
							  (simd_128&)body0->m_invWorldInertiaMatrix[2], (simd_128&)body0->m_invWorldInertiaMatrix[3]);

		_ASSERTE (m1 >= 0);
		_ASSERTE (m1 < bodyCount);
		dgBody* const body1 = bodyArray[m1].m_body;
		//dgFloat32 invMass1 = body1->m_invMass[3];
		simd_128 invMass1 (body1->m_invMass[3]);
		//const dgMatrix& invInertia1 = body1->m_invWorldInertiaMatrix;
		dgMatrix invInertiaTrans1;
		Transpose4x4Simd_128 ((simd_128&)invInertiaTrans1[0], (simd_128&)invInertiaTrans1[1], (simd_128&)invInertiaTrans1[2], (simd_128&)invInertiaTrans1[3], 
							  (simd_128&)body1->m_invWorldInertiaMatrix[0], (simd_128&)body1->m_invWorldInertiaMatrix[1], 
				              (simd_128&)body1->m_invWorldInertiaMatrix[2], (simd_128&)body1->m_invWorldInertiaMatrix[3]);

		for (dgInt32 i = 0; i < count; i ++) {

			dgJacobianMatrixElement* const row = &matrixRow[index];

			row->m_JMinv.m_jacobian_IM0.m_linear = (simd_128&)row->m_Jt.m_jacobian_IM0.m_linear * invMass0;
			//row->m_JMinv.m_jacobian_IM0.m_angular = invInertia0.UnrotateVectorSimd((simd_128&)row->m_Jt.m_jacobian_IM0.m_angular);
			row->m_JMinv.m_jacobian_IM0.m_angular = invInertiaTrans0.RotateVectorSimd((simd_128&)row->m_Jt.m_jacobian_IM0.m_angular);
			simd_128 tmpDiag ((simd_128&)row->m_JMinv.m_jacobian_IM0.m_linear * (simd_128&)row->m_Jt.m_jacobian_IM0.m_linear + 
							  (simd_128&)row->m_JMinv.m_jacobian_IM0.m_angular * (simd_128&)row->m_Jt.m_jacobian_IM0.m_angular);
			simd_128 tmpAccel ((simd_128&)row->m_JMinv.m_jacobian_IM0.m_linear * (simd_128&)body0->m_accel +
						       (simd_128&)row->m_JMinv.m_jacobian_IM0.m_angular * (simd_128&)body0->m_alpha);
							  
			row->m_JMinv.m_jacobian_IM1.m_linear = (simd_128&)row->m_Jt.m_jacobian_IM1.m_linear * invMass1;
			//row->m_JMinv.m_jacobian_IM1.m_angular = invInertia1.UnrotateVectorSimd((simd_128&)row->m_Jt.m_jacobian_IM1.m_angular);
			row->m_JMinv.m_jacobian_IM1.m_angular = invInertiaTrans1.RotateVectorSimd((simd_128&)row->m_Jt.m_jacobian_IM1.m_angular);
			tmpDiag = tmpDiag + (simd_128&)row->m_JMinv.m_jacobian_IM1.m_linear * (simd_128&)row->m_Jt.m_jacobian_IM1.m_linear + 
								(simd_128&)row->m_JMinv.m_jacobian_IM1.m_angular * (simd_128&)row->m_Jt.m_jacobian_IM1.m_angular;
			tmpAccel = tmpAccel + (simd_128&)row->m_JMinv.m_jacobian_IM1.m_linear * (simd_128&)body1->m_accel +
								  (simd_128&)row->m_JMinv.m_jacobian_IM1.m_angular * (simd_128&)body1->m_alpha;


			//dgFloat32 extenalAcceleration = -(tmpAccel.m_x + tmpAccel.m_y + tmpAccel.m_z);
			_ASSERTE (tmpAccel.m_type.m128_f32[3] == dgFloat32 (0.0f));
			tmpAccel = zero - tmpAccel.AddHorizontal();

			//row->m_deltaAccel = extenalAcceleration;
			//row->m_coordenateAccel += extenalAcceleration;
			tmpAccel.StoreScalar(&row->m_deltaAccel);
			tmpAccel = tmpAccel + simd_128 (row->m_coordenateAccel);
			tmpAccel.StoreScalar(&row->m_coordenateAccel);

			//row->m_force = row->m_jointFeebackForce[0];
			row->m_force = row->m_jointFeebackForce[0];
		
			_ASSERTE (row->m_diagDamp >= dgFloat32(0.1f));
			_ASSERTE (row->m_diagDamp <= dgFloat32(100.0f));
			_ASSERTE (tmpDiag.m_type.m128_f32[3] == dgFloat32 (0.0f));

			//dgFloat32 stiffness = DG_PSD_DAMP_TOL * row->m_diagDamp;
			//dgFloat32 diag = (tmpDiag.m_x + tmpDiag.m_y + tmpDiag.m_z);
			//row->m_diagDamp = diag * stiffness;
			simd_128 stiffness (simd_128(row->m_diagDamp) * diagDampConst);
			tmpDiag = tmpDiag.AddHorizontal();
			simd_128 diagDamp (tmpDiag * stiffness);
			diagDamp.StoreScalar (&row->m_diagDamp);

			//diag *= (dgFloat32(1.0f) + stiffness);
			//row->m_invDJMinvJt = dgFloat32(1.0f) / diag;
			simd_128 invDiagDamp (one / (tmpDiag * (one + stiffness)));
			invDiagDamp.StoreScalar(&row->m_invDJMinvJt);

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

	simd_128 zero (dgFloat32 (0.0f));
	dgInt32 bodyCount = island->m_bodyCount;
	for (dgInt32 i = 0; i < bodyCount; i ++) {
		(simd_128&) internalForces[i].m_linear = zero;
		(simd_128&) internalForces[i].m_angular = zero;
	}

	dgInt32 hasJointFeeback = 0;
	dgInt32 jointCount = island->m_jointCount;
	dgWorld* const world = (dgWorld*) this;
	dgJointInfo* const constraintArrayPtr = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJointInfo* const constraintArray = &constraintArrayPtr[island->m_jointStart];
	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowStart];

	simd_128 timestep (timestepSrc);
	for (dgInt32 i = 0; i < jointCount; i ++) {
		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPaircount;
		dgInt32 m0 = constraintArray[i].m_m0;
		dgInt32 m1 = constraintArray[i].m_m1;

		simd_128 y0_linear (zero);
		simd_128 y0_angular (zero);
		simd_128 y1_linear (zero);
		simd_128 y1_angular (zero);
		for (dgInt32 j = 0; j < count; j ++) { 
			dgJacobianMatrixElement* const row = &matrixRow[j + first];

			_ASSERTE (dgCheckFloat(row->m_force));
			simd_128 val (row->m_force);
			val.StoreScalar(&row->m_jointFeebackForce[0]);
			y0_linear = y0_linear + (simd_128&)row->m_Jt.m_jacobian_IM0.m_linear * val;
			y0_angular = y0_angular + (simd_128&)row->m_Jt.m_jacobian_IM0.m_angular * val;
			y1_linear = y1_linear + (simd_128&)row->m_Jt.m_jacobian_IM1.m_linear * val;
			y1_angular = y1_angular + (simd_128&)row->m_Jt.m_jacobian_IM1.m_angular * val;
		}
		hasJointFeeback |= (constraintArray[i].m_joint->m_updaFeedbackCallback ? 1 : 0);
		(simd_128&)internalForces[m0].m_linear = (simd_128&)internalForces[m0].m_linear + y0_linear;
		(simd_128&)internalForces[m0].m_angular = (simd_128&)internalForces[m0].m_angular + y0_angular;
		(simd_128&)internalForces[m1].m_linear = (simd_128&)internalForces[m1].m_linear + y1_linear;
		(simd_128&)internalForces[m1].m_angular = (simd_128&)internalForces[m1].m_angular + y1_angular;
	}

	simd_128 toleranceSimd (simd_128 (maxAccNorm) * simd_128 (maxAccNorm));
	dgBodyInfo* const bodyArrayPtr = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	dgBodyInfo* const bodyArray = &bodyArrayPtr[island->m_bodyStart];
	for (dgInt32 i = 1; i < bodyCount; i ++) {
		dgBody* const body = bodyArray[i].m_body;
		(simd_128&)body->m_accel = (simd_128&)body->m_accel + (simd_128&)internalForces[i].m_linear;
		(simd_128&)body->m_alpha = (simd_128&)body->m_alpha + (simd_128&)internalForces[i].m_angular;

		simd_128 accel = (simd_128&)body->m_accel * simd_128(body->m_invMass.m_w);
		simd_128 alpha (body->m_invWorldInertiaMatrix.RotateVectorSimd(body->m_alpha));

		simd_128 alphaTest ((accel * accel) > toleranceSimd);
		alphaTest = alphaTest | alphaTest.MoveHighToLow(alphaTest);
		alphaTest = alphaTest | alphaTest.PackLow(alphaTest);
		(simd_128&)body->m_accel = accel & (alphaTest | alphaTest.MoveLowToHigh(alphaTest));

		alphaTest = (alpha * alpha) > toleranceSimd;
		alphaTest = alphaTest | alphaTest.MoveHighToLow(alphaTest);
		alphaTest = alphaTest | alphaTest.PackLow(alphaTest);
		(simd_128&)body->m_alpha = alpha & (alphaTest | alphaTest.MoveLowToHigh(alphaTest));

		(simd_128&)body->m_netForce = (simd_128&)body->m_accel;
		(simd_128&)body->m_netTorque = (simd_128&)body->m_alpha;
		(simd_128&)body->m_veloc = (simd_128&)body->m_veloc +  accel * timestep;
		(simd_128&)body->m_omega = (simd_128&)body->m_omega + alpha * timestep;
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

	simd_128 zero (dgFloat32 (0.0f));

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
		(simd_128&)internalForces[m0].m_linear = (simd_128&) internalForces[m0].m_linear + y0_linear;
		(simd_128&)internalForces[m0].m_angular = (simd_128&) internalForces[m0].m_angular +  y0_angular;
		(simd_128&)internalForces[m1].m_linear = (simd_128&) internalForces[m1].m_linear +  y1_linear;
		(simd_128&)internalForces[m1].m_angular = (simd_128&)internalForces[m1].m_angular +  y1_angular;
	}


	dgFloat32 invTimestepSrc = dgFloat32 (1.0f) / timestepSrc;
	dgFloat32 invStep = (dgFloat32 (1.0f) / dgFloat32 (LINEAR_SOLVER_SUB_STEPS));
	dgFloat32 timestep =  timestepSrc * invStep;
	dgFloat32 invTimestep = invTimestepSrc * dgFloat32 (LINEAR_SOLVER_SUB_STEPS);
	_ASSERTE (bodyArray[0].m_body == world->m_sentionelBody);

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
					simd_128 force (row->m_force);
					a = simd_128(row->m_coordenateAccel) - a.AddHorizontal() - force * simd_128(row->m_diagDamp);
					
					simd_128 f (force + a * simd_128(row->m_invDJMinvJt));

					dgInt32 frictionIndex = row->m_normalForceIndex;
					_ASSERTE (((frictionIndex < 0) && (matrixRow[frictionIndex].m_force == dgFloat32 (1.0f))) || ((frictionIndex >= 0) && (matrixRow[frictionIndex].m_force >= dgFloat32 (0.0f))));
					simd_128 frictionNormal (matrixRow[frictionIndex].m_force);
					simd_128 lowerFrictionForce (frictionNormal * simd_128(row->m_lowerBoundFrictionCoefficent));
					simd_128 upperFrictionForce (frictionNormal * simd_128(row->m_upperBoundFrictionCoefficent));

					f = f.GetMax(lowerFrictionForce).GetMin(upperFrictionForce);
					a = a.AndNot((f > upperFrictionForce) | (f < upperFrictionForce));

					accNormSimd = accNormSimd.GetMax(a & signMask);

					simd_128 prevValue (f - force);
					_ASSERTE (prevValue.m_type.m128_f32[0] == prevValue.m_type.m128_f32[1]);
					_ASSERTE (prevValue.m_type.m128_f32[0] == prevValue.m_type.m128_f32[2]);
					_ASSERTE (prevValue.m_type.m128_f32[0] == prevValue.m_type.m128_f32[3]);

					f.StoreScalar(&row->m_force);

					linearM0 = linearM0 + (simd_128&) row->m_Jt.m_jacobian_IM0.m_linear * prevValue;
					angularM0 = angularM0 + (simd_128&) row->m_Jt.m_jacobian_IM0.m_angular * prevValue;
					linearM1 = linearM1 + (simd_128&) row->m_Jt.m_jacobian_IM1.m_linear * prevValue;
					angularM1 = angularM1 + (simd_128&) row->m_Jt.m_jacobian_IM1.m_angular * prevValue;
					index ++;
				}
				(simd_128&)internalForces[m0].m_linear = linearM0;
				(simd_128&)internalForces[m0].m_angular = angularM0;
				(simd_128&)internalForces[m1].m_linear = linearM1;
				(simd_128&)internalForces[m1].m_angular = angularM1;
			}
			accNormSimd.StoreScalar(&accNorm);
		}

		for (dgInt32 i = 1; i < bodyCount; i ++) {
			dgBody* const body = bodyArray[i].m_body;
			simd_128 force ((simd_128&)body->m_accel + (simd_128&)internalForces[i].m_linear);
			simd_128 torque((simd_128&)body->m_alpha + (simd_128&)internalForces[i].m_angular);
			simd_128 accel (force * simd_128(body->m_invMass.m_w));
			simd_128 alpha (body->m_invWorldInertiaMatrix.RotateVectorSimd(torque));
			(simd_128&)body->m_veloc = (simd_128&)body->m_veloc + accel * timeStepSimd;
			(simd_128&)body->m_omega = (simd_128&)body->m_omega + alpha * timeStepSimd;
			(simd_128&)internalVeloc[i].m_linear = (simd_128&)internalVeloc[i].m_linear + (simd_128&) body->m_veloc;
			(simd_128&)internalVeloc[i].m_angular = (simd_128&)internalVeloc[i].m_angular + (simd_128&) body->m_omega;
		}
	}

	dgInt32 hasJointFeeback = 0;
	for (dgInt32 i = 0; i < jointCount; i ++) {
		dgInt32 first = constraintArray[i].m_autoPairstart;
		dgInt32 count = constraintArray[i].m_autoPaircount;

		for (dgInt32 j = 0; j < count; j ++) { 
			dgJacobianMatrixElement* const row = &matrixRow[j + first];
			dgFloat32 val = row->m_force; 
			_ASSERTE (dgCheckFloat(val));
			row->m_jointFeebackForce[0] = val;
		}
		hasJointFeeback |= (constraintArray[i].m_joint->m_updaFeedbackCallback ? 1 : 0);
	}

	simd_128 invStepSimd (invStep);	
	simd_128 invTimeStepSimd (invTimestepSrc);	
	simd_128 accelerationTolerance (maxAccNorm, maxAccNorm, maxAccNorm, dgFloat32 (0.0f));	

	for (dgInt32 i = 1; i < bodyCount; i ++) {
		dgBody* const body = bodyArray[i].m_body;

		(simd_128&)body->m_veloc = (simd_128&) internalVeloc[i].m_linear * invStepSimd;
		(simd_128&)body->m_omega = (simd_128&) internalVeloc[i].m_angular * invStepSimd;

		simd_128 accel (((simd_128&) body->m_veloc - (simd_128&) body->m_netForce) * invTimeStepSimd);
		simd_128 alpha (((simd_128&) body->m_omega - (simd_128&) body->m_netTorque) * invTimeStepSimd);

		_ASSERTE (accel.m_type.m128_f32[3] == dgFloat32 (0.0f));
		_ASSERTE (alpha.m_type.m128_f32[3] == dgFloat32 (0.0f));

		simd_128 alphaTest ((accel * accel) > accelerationTolerance);
		alphaTest = alphaTest | alphaTest.MoveHighToLow(alphaTest);
		alphaTest = alphaTest | alphaTest.PackLow(alphaTest);
		(simd_128&)body->m_accel = accel & (alphaTest | alphaTest.MoveLowToHigh(alphaTest));

		alphaTest = (alpha * alpha) > accelerationTolerance;
		alphaTest = alphaTest | alphaTest.MoveHighToLow(alphaTest);
		alphaTest = alphaTest | alphaTest.PackLow(alphaTest);
		(simd_128&)body->m_alpha = alpha & (alphaTest | alphaTest.MoveLowToHigh(alphaTest));

		(simd_128&)body->m_netForce = accel * simd_128 (body->m_mass[3]);

		alpha = body->m_matrix.UnrotateVectorSimd(alpha);
		_ASSERTE (alpha.m_type.m128_f32[3] == dgFloat32 (0.0f));
		(simd_128&)body->m_netTorque = body->m_matrix.RotateVectorSimd(alpha * (simd_type&)body->m_mass);
	}

	if (hasJointFeeback) {
		for (dgInt32 i = 0; i < jointCount; i ++) {
			if (constraintArray[i].m_joint->m_updaFeedbackCallback) {
				constraintArray[i].m_joint->m_updaFeedbackCallback (*constraintArray[i].m_joint, timestepSrc, threadIndex);
			}
		}
	}
}



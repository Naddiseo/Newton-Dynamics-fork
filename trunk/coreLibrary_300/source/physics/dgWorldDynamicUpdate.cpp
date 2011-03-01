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
#include "dgBroadPhaseCollision.h"



#define DG_PARALLEL_JOINT_COUNT				64


class dgWorldDynamicUpdateSyncDescriptor
{
	public:
	dgWorldDynamicUpdateSyncDescriptor()
	{
		memset (this, 0, sizeof (dgWorldDynamicUpdateSyncDescriptor));
	}

//	dgInt32 m_lock;
	dgFloat32 m_timestep;
	dgWorld* m_world;
	dgInt32 m_atomicCounter;


};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

dgBody* dgWorld::GetIslandBody (const void* const islandPtr, dgInt32 index) const
{
	const dgIslandCallbackStruct* const island = (dgIslandCallbackStruct*)islandPtr;

	char* const ptr = &((char*)island->m_bodyArray)[island->m_strideInByte * index];
	dgBody** const bodyPtr = (dgBody**)ptr;
	return (index < island->m_count) ? ((index >= 0) ? *bodyPtr : NULL) : NULL;
}


dgWorldDynamicUpdate::dgWorldDynamicUpdate()
{
	m_bodies = 0;;
	m_joints = 0;
	m_islands = 0;
	m_markLru = 0;
}


void dgWorldDynamicUpdate::UpdateDynamics(dgFloat32 timestep)
{
	dgWorld* const world = (dgWorld*) this;
	dgUnsigned32 updateTime = world->m_getPerformanceCount();

	m_bodies = 0;
	m_joints = 0;
	m_islands = 0;
	m_markLru = 0;

	world->m_dynamicsLru = world->m_dynamicsLru + 2;
	m_markLru = world->m_dynamicsLru;
	dgUnsigned32 lru = m_markLru - 1;

	dgBodyMasterList& me = *world;


	world->m_pairMemoryBuffer.ExpandCapacityIfNeessesary (4 * me.GetCount(), sizeof (dgBody*));
	


	_ASSERTE (me.GetFirst()->GetInfo().GetBody() == world->m_sentionelBody);

	world->m_sentionelBody->m_index = 0; 
	world->m_sentionelBody->m_dynamicsLru = m_markLru;

	for (dgBodyMasterList::dgListNode* node = me.GetLast(); node; node = node->GetPrev()) {
		const dgBodyMasterListRow& graphNode = node->GetInfo();
		dgBody *const body = graphNode.GetBody();	
		if (body->m_invMass.m_w == dgFloat32(0.0f)) {
#ifdef _DEBUG
			for (; node; node = node->GetPrev()) {
				_ASSERTE (node->GetInfo().GetBody()->m_invMass.m_w == dgFloat32(0.0f));
			}
#endif
			break;
		}

		if (body->m_dynamicsLru < lru) {
			if (!(body->m_freeze | body->m_spawnnedFromCallback | body->m_sleeping | !body->m_isInWorld)) {
				SpanningTree (body);
			}
		}
		body->m_spawnnedFromCallback = false;
	}

	dgIsland* const islands = (dgIsland*) &world->m_islandMemory[0];
	dgSort (islands, m_islands, CompareIslands); 

	dgInt32 maxRowCount = 0;
	for (dgInt32 i = 0; i < m_islands; i ++) {
		maxRowCount += islands[i].m_rowsCount;
	}
	m_solverMemory.Init (world, maxRowCount, m_bodies);

	dgUnsigned32 dynamicsTime = world->m_getPerformanceCount();
	world->m_perfomanceCounters[m_dynamicsBuildSpanningTreeTicks] = dynamicsTime - updateTime;

	m_rowCountAtomicIndex = 0;

	if (world->m_singleIslandMultithreading) {
//if (1) {
		_ASSERTE (0);
/*
dgWorldDynamicUpdateSyncDescriptor descriptor;

//	dgInt32 atomicCounter = 0;
//	userParamArray[0] = this;
//	userParamArray[1] = *((void**) &timestep);
//	userParamArray[2] = &atomicCounter;
//	m_rowCountAtomicIndex = 0;
dgInt32 threadCounts = world->GetThreadCount();	

descriptor.m_world = world;
descriptor.m_timestep = timestep;


		dgInt32 unilarealJointsCount = 0;
		for (dgInt32 i = 0; (i < m_islands) && islands[i].m_hasUnilateralJoints; i ++) {
			unilarealJointsCount ++;
		}

		if (unilarealJointsCount) {
			dgInt32 islandCount = m_islands;
			m_islands = unilarealJointsCount;
			for (dgInt32 i = 0; i < threadCounts; i ++) {
				world->QueueJob (CalculateIslandReactionForces, &userParamArray[0], 3);
			}
			world->SynchronizationBarrier();
			m_islands = islandCount;
		}
		CalculateReactionForcesParallel (&islands[unilarealJointsCount], m_islands - unilarealJointsCount, timestep);
*/			
	} else {
		dgWorldDynamicUpdateSyncDescriptor descriptor;
		//	dgInt32 atomicCounter = 0;
		//	userParamArray[0] = this;
		//	userParamArray[1] = *((void**) &timestep);
		//	userParamArray[2] = &atomicCounter;
		
		dgInt32 threadCounts = world->GetThreadCount();	

		descriptor.m_world = world;
		descriptor.m_timestep = timestep;
		for (dgInt32 i = 0; i < threadCounts; i ++) {
			world->QueueJob (CalculateIslandReactionForces, &descriptor);
		}
		world->SynchronizationBarrier();
	}

	dgUnsigned32 ticks = world->m_getPerformanceCount();
	world->m_perfomanceCounters[m_dynamicsSolveSpanningTreeTicks] = ticks - dynamicsTime;
	world->m_perfomanceCounters[m_dynamicsTicks] = ticks - updateTime;
}


void dgJacobianMemory::Init (dgWorld* const world, dgInt32 rowsCount, dgInt32 bodyCount)
{
	dgInt32 stride = sizeof (dgJacobianMatrixElement);
	if (world->m_solverMatrixMemory.ExpandCapacityIfNeessesary (rowsCount + 4, stride)) {
		m_memory = (dgJacobianMatrixElement*) &world->m_solverMatrixMemory[0];
		memset (m_memory, 0, 4 * sizeof (dgJacobianMatrixElement));
		m_memory[0].m_force = dgFloat32 (1.0f);
		m_memory[1].m_force = dgFloat32 (1.0f);
		m_memory[2].m_force = dgFloat32 (1.0f);
		m_memory[3].m_force = dgFloat32 (1.0f);
		m_memory += 4;
	}


	stride = CalculateIntenalMemorySize();
	if (world->m_internalForcesMemory.ExpandCapacityIfNeessesary (bodyCount, stride)) {
		dgInt32 newCount = ((world->m_internalForcesMemory.GetBytesCapacity() - 16)/ stride) & (-8);
		_ASSERTE (bodyCount <= newCount);
		dgInt8* memory = (dgInt8*) &world->m_internalForcesMemory[0];

		m_internalForces = (dgJacobian*) memory;
		m_internalVeloc = (dgJacobian*) &m_internalForces[newCount];
		m_treadLocks = (dgInt32*) &m_internalVeloc[newCount];
		memset (m_treadLocks, 0, newCount * sizeof (dgInt32));

		_ASSERTE ((dgUnsigned64(m_internalForces) & 0x0f) == 0);
		_ASSERTE ((dgUnsigned64(m_internalVeloc) & 0x0f) == 0);
	}
}


dgInt32 dgWorldDynamicUpdate::CompareIslands (const dgIsland* const islandA, const dgIsland* const islandB, void* notUsed)
{
	dgInt32 countA = islandA->m_jointCount + (islandA->m_hasUnilateralJoints << 28);
	dgInt32 countB = islandB->m_jointCount + (islandB->m_hasUnilateralJoints << 28);

	if (countA < countB) {
		return 1;
	}
	if (countA > countB) {
		return -1;
	}
	return 0;
}



void dgWorldDynamicUpdate::SpanningTree (dgBody* const body)
{
	dgInt32 bodyCount = 0;
	dgInt32 jointCount = 0;
	dgInt32 staticCount = 0;
	dgUnsigned32 lruMark = m_markLru - 1;
	dgInt32 isInWorld = 0;
	dgInt32 isInEquilibrium = 1;
	dgInt32 hasUnilateralJoints = 0;
	dgInt32 isContinueCollisionIsland = 0;
	dgBody* heaviestBody = NULL;
	dgFloat32 haviestMass = dgFloat32 (0.0f);

	dgWorld* const world = (dgWorld*) this;
	dgQueue<dgBody*> queue ((dgBody**) &world->m_pairMemoryBuffer[0], dgInt32 ((world->m_pairMemoryBuffer.GetBytesCapacity()>>1)/sizeof (void*)));
	
	dgBody** const staticPool = &queue.m_pool[queue.m_mod];

	body->m_dynamicsLru = lruMark;

	queue.Insert (body);
	while (!queue.IsEmpty()) {
		dgInt32 count = queue.m_firstIndex - queue.m_lastIndex;
		if (count < 0) {
			_ASSERTE (0);
			count += queue.m_mod;
		}

		dgInt32 index = queue.m_lastIndex;
		queue.Reset ();
		for (dgInt32 j = 0; j < count; j ++) {

			dgBody* const srcBody = queue.m_pool[index];
			_ASSERTE (srcBody);
			_ASSERTE (srcBody->m_invMass.m_w > dgFloat32 (0.0f));
			_ASSERTE (srcBody->m_dynamicsLru == lruMark);
			_ASSERTE (srcBody->m_masterNode);

			dgInt32 bodyIndex = m_bodies + bodyCount;
			world->m_bodiesMemory.ExpandCapacityIfNeessesary(bodyIndex, sizeof (dgBodyInfo));

			dgBodyInfo* const bodyArray = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
			bodyArray[bodyIndex].m_body = srcBody;


			isInWorld |= srcBody->m_isInWorld;
			isInEquilibrium &= srcBody->m_equilibrium;
			isInEquilibrium &= srcBody->m_autoSleep;

// hack to test stability
//isInEquilibrium = 0;

			srcBody->m_sleeping = false;
			isContinueCollisionIsland |= srcBody->m_solverInContinueCollision;

			if (srcBody->m_mass.m_w > haviestMass) {
				haviestMass = srcBody->m_mass.m_w;
				heaviestBody = srcBody;
			}

			bodyCount ++;
			for (dgBodyMasterListRow::dgListNode* jointNode = srcBody->m_masterNode->GetInfo().GetFirst(); jointNode; jointNode = jointNode->GetNext()) {

				dgBodyMasterListCell* const cell = &jointNode->GetInfo();
				dgBody* const body = cell->m_bodyNode;

				if (body->m_dynamicsLru < lruMark) {
					body->m_dynamicsLru = lruMark;
					if (body->m_invMass.m_w > dgFloat32 (0.0f)) { 
						queue.Insert (body);
					} else {
						dgInt32 duplicateBody = 0;
						for (; duplicateBody < staticCount; duplicateBody ++) {
							if (staticPool[duplicateBody] == srcBody) {
								break;
							}
						}
						if (duplicateBody == staticCount) {
							staticPool[staticCount] = srcBody;
							staticCount ++;
							_ASSERTE (srcBody->m_invMass.m_w > dgFloat32 (0.0f));
						}

						dgConstraint* const constraint = cell->m_joint;
						_ASSERTE (dgInt32 (constraint->m_dynamicsLru) != m_markLru);

						dgInt32 jointIndex = m_joints + jointCount; 

						world->m_jointsMemory.ExpandCapacityIfNeessesary(jointIndex, sizeof (dgJointInfo));

						if (constraint->m_isUnilateral)	{
							hasUnilateralJoints = 1;
							_ASSERTE ((constraint->m_body0 == world->m_sentionelBody) || (constraint->m_body1 == world->m_sentionelBody));
						}			

						constraint->m_index = dgUnsigned32 (jointCount);
						dgJointInfo* const constraintArray = (dgJointInfo*) &world->m_jointsMemory[0];
						constraintArray[jointIndex].m_joint = constraint;
						jointCount ++;

						_ASSERTE (constraint->m_body0);
						_ASSERTE (constraint->m_body1);
					}

//				} else if (cell->m_bodyNode->m_invMass.m_w == dgFloat32 (0.0f)) { 
				} else if (body->m_invMass.m_w == dgFloat32 (0.0f)) { 
					dgInt32 duplicateBody = 0;
					for (; duplicateBody < staticCount; duplicateBody ++) {
						if (staticPool[duplicateBody] == srcBody) {
							break;
						}
					}
					if (duplicateBody == staticCount) {
						staticPool[staticCount] = srcBody;
						staticCount ++;
						_ASSERTE (srcBody->m_invMass.m_w > dgFloat32 (0.0f));
					}


					dgConstraint* const constraint = cell->m_joint;
					_ASSERTE (dgInt32 (constraint->m_dynamicsLru) != m_markLru);

					dgInt32 jointIndex = m_joints + jointCount; 
					world->m_jointsMemory.ExpandCapacityIfNeessesary(jointIndex, sizeof (dgJointInfo));

					if (constraint->m_isUnilateral)	{
						hasUnilateralJoints = 1;
						_ASSERTE ((constraint->m_body0 == world->m_sentionelBody) || (constraint->m_body1 == world->m_sentionelBody));
					}			

					constraint->m_index = dgUnsigned32 (jointCount);
					dgJointInfo* const constraintArray = (dgJointInfo*) &world->m_jointsMemory[0];
					constraintArray[jointIndex].m_joint = constraint;
					jointCount ++;

					_ASSERTE (constraint->m_body0);
					_ASSERTE (constraint->m_body1);
				}
			}

			index ++;
			if (index >= queue.m_mod) {
				_ASSERTE (0);
				index = 0;
			}
		}
	}


	dgBodyInfo* const bodyArray = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	dgJointInfo* const constraintArray = (dgJointInfo*) &world->m_jointsMemory[0];

	if (isInEquilibrium | !isInWorld) {
		for (dgInt32 i = 0; i < bodyCount; i ++) {
			dgBody* const body = bodyArray[m_bodies + i].m_body;
			body->m_dynamicsLru = m_markLru;
			body->m_sleeping = true;
		}
	} else {
		if (world->m_islandUpdate) {
			dgIslandCallbackStruct record;
			record.m_world = world;
			record.m_count = bodyCount;
			record.m_strideInByte = sizeof (dgBodyInfo);
			record.m_bodyArray = &bodyArray[m_bodies].m_body;
			if (!world->m_islandUpdate (world, &record, bodyCount)) {
				for (dgInt32 i = 0; i < bodyCount; i ++) {
					dgBody* const body = bodyArray[m_bodies + i].m_body;
					body->m_dynamicsLru = m_markLru;
				}
				return;
			}
		}

		dgInt32 rowsCount = 0;
		if (staticCount) {
			queue.Reset ();
			for (dgInt32 i = 0; i < staticCount; i ++) {
				dgBody* const body = staticPool[i];
				body->m_dynamicsLru = m_markLru;
				queue.Insert (body);
				_ASSERTE (dgInt32 (body->m_dynamicsLru) == m_markLru);
			}

			for (dgInt32 i = 0; i < jointCount; i ++) {
				dgConstraint* const constraint = constraintArray[m_joints + i].m_joint;
				constraint->m_dynamicsLru = m_markLru;
				dgInt32 rows = dgInt32 ((constraint->m_maxDOF & (DG_SIMD_WORD_SIZE - 1)) ? ((constraint->m_maxDOF & (-DG_SIMD_WORD_SIZE)) + DG_SIMD_WORD_SIZE) : constraint->m_maxDOF);
				rowsCount += rows;
				constraintArray[m_joints + i].m_autoPaircount = rows;
			}
		} else {
			_ASSERTE (heaviestBody);
			queue.Insert (heaviestBody);
			//body->m_dynamicsLru = m_markLru;
			heaviestBody->m_dynamicsLru = m_markLru;
		}
		BuildIsland (queue, jointCount, rowsCount, hasUnilateralJoints, isContinueCollisionIsland);
	}
}


void dgWorldDynamicUpdate::BuildIsland (dgQueue<dgBody*>& queue, dgInt32 jointCount, dgInt32 rowsCount, dgInt32 hasUnilateralJoints, dgInt32 isContinueCollisionIsland)
{

	dgInt32 bodyCount = 1;
	dgUnsigned32 lruMark = m_markLru;

	dgWorld* const world = (dgWorld*) this;
	world->m_bodiesMemory.ExpandCapacityIfNeessesary(m_bodies, sizeof (dgBodyInfo));

	dgBodyInfo* const bodyArray = (dgBodyInfo*) &world->m_bodiesMemory[0]; 

	bodyArray[m_bodies].m_body = world->m_sentionelBody;
	_ASSERTE (world->m_sentionelBody->m_index == 0); 
	_ASSERTE (dgInt32 (world->m_sentionelBody->m_dynamicsLru) == m_markLru); 

	while (!queue.IsEmpty()) {

		dgInt32 count = queue.m_firstIndex - queue.m_lastIndex;
		if (count < 0) {
			_ASSERTE (0);
			count += queue.m_mod;
		}

		dgInt32 index = queue.m_lastIndex;
		queue.Reset ();

		for (dgInt32 j = 0; j < count; j ++) {

			dgBody* const body = queue.m_pool[index];
			_ASSERTE (body);
			_ASSERTE (body->m_dynamicsLru == lruMark);
			_ASSERTE (body->m_masterNode);

			if (body->m_invMass.m_w > dgFloat32 (0.0f)) { 
				dgInt32 bodyIndex = m_bodies + bodyCount;
				world->m_bodiesMemory.ExpandCapacityIfNeessesary(bodyIndex, sizeof (dgBodyInfo));

				dgBodyInfo* const bodyArray = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
				body->m_index = bodyCount; 
				bodyArray[bodyIndex].m_body = body;

				bodyCount ++;
			}


			for (dgBodyMasterListRow::dgListNode* jointNode = body->m_masterNode->GetInfo().GetFirst(); jointNode; jointNode = jointNode->GetNext()) {
				dgBodyMasterListCell* const cell = &jointNode->GetInfo();

				dgBody* const body = cell->m_bodyNode;
				dgConstraint* const constraint = cell->m_joint;

				if (constraint->m_dynamicsLru != lruMark) {

					constraint->m_dynamicsLru = lruMark;

					dgInt32 jointIndex = m_joints + jointCount; 
					world->m_jointsMemory.ExpandCapacityIfNeessesary(jointIndex, sizeof (dgJointInfo));

					if (constraint->m_isUnilateral)	{
						hasUnilateralJoints = 1;
						_ASSERTE ((constraint->m_body0 == world->m_sentionelBody) || (constraint->m_body1 == world->m_sentionelBody));
					}			

					constraint->m_index = dgUnsigned32 (jointCount);
					dgJointInfo* const constraintArray = (dgJointInfo*) &world->m_jointsMemory[0];
					constraintArray[jointIndex].m_joint = constraint;

					//rowsCount += (constraint->m_maxDOF & (DG_SIMD_WORD_SIZE - 1)) ? ((constraint->m_maxDOF & (-DG_SIMD_WORD_SIZE)) + DG_SIMD_WORD_SIZE) : constraint->m_maxDOF;
					dgInt32 rows = dgInt32 ((constraint->m_maxDOF & (DG_SIMD_WORD_SIZE - 1)) ? ((constraint->m_maxDOF & (-DG_SIMD_WORD_SIZE)) + DG_SIMD_WORD_SIZE) : constraint->m_maxDOF);
					rowsCount += rows;
					constraintArray[jointIndex].m_autoPaircount = rows;

					jointCount ++;

					_ASSERTE (constraint->m_body0);
					_ASSERTE (constraint->m_body1);
				}

				if (body->m_dynamicsLru != lruMark) {
					if (body->m_invMass.m_w > dgFloat32 (0.0f)) { 
						queue.Insert (body);
						body->m_dynamicsLru = lruMark;
					}
				}
			}

			index ++;
			if (index >= queue.m_mod) {
				_ASSERTE (0);
				index = 0;
			}
		}
	}


	if (bodyCount > 1) {
		world->m_islandMemory.ExpandCapacityIfNeessesary (m_islands, sizeof (dgIsland));

		dgIsland* const islandArray = (dgIsland*) &world->m_islandMemory[0];

#ifdef _DEBUG
		islandArray[m_islands].m_islandId = m_islands;
#endif

		islandArray[m_islands].m_bodyStart = m_bodies;
		islandArray[m_islands].m_jointStart = m_joints;
		islandArray[m_islands].m_bodyCount = bodyCount;
		islandArray[m_islands].m_jointCount = jointCount;
		islandArray[m_islands].m_rowsCount = rowsCount;
		islandArray[m_islands].m_hasUnilateralJoints = hasUnilateralJoints;
		islandArray[m_islands].m_isContinueCollision = isContinueCollisionIsland;

		m_islands ++;
		m_bodies += bodyCount;
		m_joints += jointCount;
	}
}

void dgWorldDynamicUpdate::CalculateIslandReactionForces (void* const context, dgInt32 threadID)
{
	dgWorldDynamicUpdateSyncDescriptor* const descriptor = (dgWorldDynamicUpdateSyncDescriptor*) context;

	dgFloat32 timestep = descriptor->m_timestep;
	dgWorld* const world = (dgWorld*) descriptor->m_world;
	dgInt32 count = world->m_islands;
	dgIsland* const islands = (dgIsland*) &world->m_islandMemory[0];

	if (world->m_cpu == dgSimdPresent) {
		for (dgInt32 i = dgAtomicAdd(&descriptor->m_atomicCounter, 1); i < count; i = dgAtomicAdd(&descriptor->m_atomicCounter, 1)) {
			dgIsland* const island = &islands[i]; 
			world->CalculateIslandReactionForcesSimd (island, timestep, threadID);
		}
	} else {
		for (dgInt32 i = dgAtomicAdd(&descriptor->m_atomicCounter, 1); i < count; i = dgAtomicAdd(&descriptor->m_atomicCounter, 1)) {
			dgIsland* const island = &islands[i]; 
			world->CalculateIslandReactionForces (island, timestep, threadID);
		}
	}
}


dgInt32 dgWorldDynamicUpdate::GetJacobianDerivatives (const dgIsland* const island, dgInt32 threadIndex, bool bitMode, dgInt32 rowBase, dgInt32 rowCount, dgFloat32 timestep) const
{
	dgContraintDescritor constraintParams;

	dgWorld* const world = (dgWorld*) this;
	constraintParams.m_world = world; 
	constraintParams.m_threadIndex = threadIndex;
	constraintParams.m_timestep = timestep;
	constraintParams.m_invTimestep = dgFloat32 (1.0f / timestep);

	dgJointInfo* const constraintArrayPtr = (dgJointInfo*) &world->m_jointsMemory[0];
	dgJointInfo* const constraintArray = &constraintArrayPtr[island->m_jointStart];

//	dgJacobianPair* const Jt = &m_solverMemory.m_Jt[rowBase];
//	dgFloat32* const diagDamp = &m_solverMemory.m_diagDamp[rowBase];
//	dgFloat32* const restitution = &m_solverMemory.m_restitution[rowBase];
//	dgFloat32* const coordenateAccel = &m_solverMemory.m_coordenateAccel[rowBase];
//	dgInt32* const accelIsMotor = &m_solverMemory.m_accelIsMotor[rowBase];
//	dgFloat32* const penetration = &m_solverMemory.m_penetration[rowBase];
//	dgFloat32* const penetrationStiffness = &m_solverMemory.m_penetrationStiffness[rowBase];
//	dgFloat32* const lowerBoundFrictionCoefficent = &m_solverMemory.m_lowerBoundFrictionCoefficent[rowBase];
//	dgFloat32* const upperBoundFrictionCoefficent = &m_solverMemory.m_upperBoundFrictionCoefficent[rowBase];
//	dgFloat32** const jointFeebackForce = &m_solverMemory.m_jointFeebackForce[rowBase];
//	dgInt32* const normalForceIndex = &m_solverMemory.m_normalForceIndex[rowBase];

	dgJacobianMatrixElement* const matrixRow = &m_solverMemory.m_memory[rowBase];
	dgInt32 jointCount = island->m_jointCount;
	for (dgInt32 j = 0; j < jointCount; j ++) {
		dgJointInfo* const jointInfo = &constraintArray[j];
		dgConstraint* const constraint = jointInfo->m_joint;;
		if (constraint->m_isUnilateral ^ bitMode) {

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

			_ASSERTE (constraint->m_index == dgUnsigned32(j));
			//constraint->m_index = j;
			jointInfo->m_autoPairstart = rowCount;
			jointInfo->m_autoPaircount = dof;
			jointInfo->m_autoPairActiveCount = dof;
			jointInfo->m_m0 = m0;
			jointInfo->m_m1 = m1;

			dgInt32 fistForceOffset = -rowBase;
			for (dgInt32 i = 0; i < dof; i ++) {

				dgJacobianMatrixElement* const row = &matrixRow[rowCount];
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
				row->m_normalForceIndex = index + ((index >=0) ? (rowCount - i) : fistForceOffset);
				rowCount ++;
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
			rowCount = (rowCount & (DG_SIMD_WORD_SIZE - 1)) ? ((rowCount & (-DG_SIMD_WORD_SIZE)) + DG_SIMD_WORD_SIZE) : rowCount;
			_ASSERTE ((rowCount & (DG_SIMD_WORD_SIZE - 1)) == 0);
		}
	}
	return rowCount;

}


void dgWorldDynamicUpdate::IntegrateArray (const dgIsland* const island, dgFloat32 accelTolerance, dgFloat32 timestep, dgInt32 threadIndex, bool update) const
{
	bool isAutoSleep = true;
	bool stackSleeping = true;
	dgInt32 sleepCounter = 10000;

	dgWorld* const world = (dgWorld*) this;

	dgFloat32 speedFreeze = world->m_freezeSpeed2;
	dgFloat32 accelFreeze = world->m_freezeAccel2;
	dgFloat32 maxAccel = dgFloat32 (0.0f);
	dgFloat32 maxAlpha = dgFloat32 (0.0f);
	dgFloat32 maxSpeed = dgFloat32 (0.0f);
	dgFloat32 maxOmega = dgFloat32 (0.0f);

	dgVector zero (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));

	dgFloat32 forceDamp = DG_FREEZZING_VELOCITY_DRAG;

	dgBodyInfo* const bodyArrayPtr = (dgBodyInfo*) &world->m_bodiesMemory[0]; 
	dgBodyInfo* const bodyArray = &bodyArrayPtr[island->m_bodyStart + 1]; 
	dgInt32 count = island->m_bodyCount - 1;
	if (count <= 2) {
		bool autosleep = bodyArray[0].m_body->m_autoSleep;
		if (count == 2) {
			autosleep &= bodyArray[1].m_body->m_autoSleep;
		}
		if (!autosleep) {
			forceDamp = dgFloat32 (0.9999f);
		}
	}


	for (dgInt32 i = 0; i < count; i ++) {
		dgBody* const body = bodyArray[i].m_body;

		if (body->m_invMass.m_w && body->m_isInWorld) {
			body->IntegrateVelocity(timestep);

			dgFloat32 accel2 = body->m_accel % body->m_accel;
			dgFloat32 alpha2 = body->m_alpha % body->m_alpha;
			dgFloat32 speed2 = body->m_veloc % body->m_veloc;
			dgFloat32 omega2 = body->m_omega % body->m_omega;

			maxAccel = GetMax (maxAccel, accel2);
			maxAlpha = GetMax (maxAlpha, alpha2);
			maxSpeed = GetMax (maxSpeed, speed2);
			maxOmega = GetMax (maxOmega, omega2);

			bool equilibrium = (accel2 < accelFreeze) && (alpha2 < accelFreeze) && (speed2 < speedFreeze) && (omega2 < speedFreeze);
			if (equilibrium) {
				body->m_veloc = body->m_veloc.Scale (forceDamp);
				body->m_omega = body->m_omega.Scale (forceDamp);
			}
			body->m_equilibrium = dgUnsigned32 (equilibrium);
			stackSleeping &= equilibrium;
			isAutoSleep &= body->m_autoSleep;

			sleepCounter = GetMin (sleepCounter, body->m_sleepingCounter);
		}
	}


	if (update) {
		for (dgInt32 i = 0; i < count; i ++) {
			dgBody* const body = bodyArray[i].m_body;
			if (body->m_invMass.m_w && body->m_isInWorld) {
				body->UpdateMatrix (timestep, threadIndex);
			}
		}

		if (isAutoSleep) {
			if (stackSleeping) {
				for (dgInt32 i = 0; i < count; i ++) {
					dgBody* const body = bodyArray[i].m_body;
					body->m_netForce = zero;
					body->m_netTorque = zero;
					body->m_veloc = zero;
					body->m_omega = zero;
				}
			} else {
				if ((maxAccel > world->m_sleepTable[DG_SLEEP_ENTRIES - 1].m_maxAccel) ||
					(maxAlpha > world->m_sleepTable[DG_SLEEP_ENTRIES - 1].m_maxAlpha) ||
					(maxSpeed > world->m_sleepTable[DG_SLEEP_ENTRIES - 1].m_maxVeloc) ||
					(maxOmega > world->m_sleepTable[DG_SLEEP_ENTRIES - 1].m_maxOmega)) {
						for (dgInt32 i = 0; i < count; i ++) {
							dgBody* const body = bodyArray[i].m_body;
							body->m_sleepingCounter = 0;
						}
				} else {
					//dgInt32 sleepEntriesCount;
					//sleepEntriesCount = (count > 1) ? DG_SLEEP_ENTRIES : DG_SLEEP_ENTRIES - 1;
					dgInt32 index = 0;
					for (dgInt32 i = 0; i < DG_SLEEP_ENTRIES; i ++) {
						if ((maxAccel <= world->m_sleepTable[i].m_maxAccel) &&
							(maxAlpha <= world->m_sleepTable[i].m_maxAlpha) &&
							(maxSpeed <= world->m_sleepTable[i].m_maxVeloc) &&
							(maxOmega <= world->m_sleepTable[i].m_maxOmega)) {
								index = i;
								break;
						}
					}

					dgInt32 timeScaleSleepCount = dgInt32 (dgFloat32 (60.0f) * sleepCounter * timestep);
					if (timeScaleSleepCount > world->m_sleepTable[index].m_steps) {
						for (dgInt32 i = 0; i < count; i ++) {
							dgBody* const body = bodyArray[i].m_body;
							body->m_netForce = zero;
							body->m_netTorque = zero;
							body->m_veloc = zero;
							body->m_omega = zero;
							body->m_equilibrium = true;
						}
					} else {
						sleepCounter ++;
						for (dgInt32 i = 0; i < count; i ++) {
							dgBody* const body = bodyArray[i].m_body;
							body->m_sleepingCounter = sleepCounter;
						}
					}
				}
			}
		}
	}
}



#if 0
void dgJacobianMemory::SwapRowsSimd (dgInt32 i, dgInt32 j) const
{
#ifdef DG_BUILD_SIMD_CODE
	_ASSERTE (i != j);
	//Swap (m_Jt[i], m_Jt[j]);
	dgJacobianPair* const ptr0 = m_Jt;
	simd_type tmp0 = (simd_type&)ptr0[i].m_jacobian_IM0.m_linear;
	simd_type tmp1 = (simd_type&)ptr0[i].m_jacobian_IM0.m_angular;
	simd_type tmp2 = (simd_type&)ptr0[i].m_jacobian_IM1.m_linear;
	simd_type tmp3 = (simd_type&)ptr0[i].m_jacobian_IM1.m_angular;
	simd_type tmp4 = (simd_type&)ptr0[j].m_jacobian_IM0.m_linear;
	simd_type tmp5 = (simd_type&)ptr0[j].m_jacobian_IM0.m_angular;
	simd_type tmp6 = (simd_type&)ptr0[j].m_jacobian_IM1.m_linear;
	simd_type tmp7 = (simd_type&)ptr0[j].m_jacobian_IM1.m_angular;

	(simd_type&)ptr0[j].m_jacobian_IM0.m_linear = tmp0;
	(simd_type&)ptr0[j].m_jacobian_IM0.m_angular = tmp1;
	(simd_type&)ptr0[j].m_jacobian_IM1.m_linear = tmp2;
	(simd_type&)ptr0[j].m_jacobian_IM1.m_angular = tmp3;
	(simd_type&)ptr0[i].m_jacobian_IM0.m_linear = tmp4;
	(simd_type&)ptr0[i].m_jacobian_IM0.m_angular = tmp5;
	(simd_type&)ptr0[i].m_jacobian_IM1.m_linear = tmp6;
	(simd_type&)ptr0[i].m_jacobian_IM1.m_angular = tmp7;

	//Swap (m_JMinv[i], m_JMinv[j]);
	dgJacobianPair* const ptr1 = m_JMinv;
	tmp0 = (simd_type&)ptr1[i].m_jacobian_IM0.m_linear;
	tmp1 = (simd_type&)ptr1[i].m_jacobian_IM0.m_angular;
	tmp2 = (simd_type&)ptr1[i].m_jacobian_IM1.m_linear;
	tmp3 = (simd_type&)ptr1[i].m_jacobian_IM1.m_angular;
	tmp4 = (simd_type&)ptr1[j].m_jacobian_IM0.m_linear;
	tmp5 = (simd_type&)ptr1[j].m_jacobian_IM0.m_angular;
	tmp6 = (simd_type&)ptr1[j].m_jacobian_IM1.m_linear;
	tmp7 = (simd_type&)ptr1[j].m_jacobian_IM1.m_angular;

	(simd_type&)ptr1[j].m_jacobian_IM0.m_linear = tmp0;
	(simd_type&)ptr1[j].m_jacobian_IM0.m_angular = tmp1;
	(simd_type&)ptr1[j].m_jacobian_IM1.m_linear = tmp2;
	(simd_type&)ptr1[j].m_jacobian_IM1.m_angular = tmp3;
	(simd_type&)ptr1[i].m_jacobian_IM0.m_linear = tmp4;
	(simd_type&)ptr1[i].m_jacobian_IM0.m_angular = tmp5;
	(simd_type&)ptr1[i].m_jacobian_IM1.m_linear = tmp6;
	(simd_type&)ptr1[i].m_jacobian_IM1.m_angular = tmp7;

	Swap (m_diagDamp[i], m_diagDamp[j]); 
	Swap (m_invDJMinvJt[i], m_invDJMinvJt[j]);
	//	Swap (m_jacobianIndexArray[i], m_jacobianIndexArray[j]); 
	Swap (m_normalForceIndex[i], m_normalForceIndex[j]);
	//	Swap (m_lowerBoundFrictionForce[i], m_lowerBoundFrictionForce[j]);
	//	Swap (m_upperBoundFrictionForce[i], m_upperBoundFrictionForce[j]);
	Swap (m_lowerBoundFrictionCoefficent[i], m_lowerBoundFrictionCoefficent[j]);
	Swap (m_upperBoundFrictionCoefficent[i], m_upperBoundFrictionCoefficent[j]);
	Swap (m_jointFeebackForce[i], m_jointFeebackForce[j]);
	Swap (m_coordenateAccel[i], m_coordenateAccel[j]);
	Swap (m_force[i], m_force[j]);
	Swap (m_accel[i], m_accel[j]);
	//	Swap (m_forceStep[i], m_forceStep[j]);
	Swap (m_deltaAccel[i], m_deltaAccel[j]);
	Swap (m_deltaForce[i], m_deltaForce[j]);

#else

#endif
}


void dgJacobianMemory::SwapRows (dgInt32 i, dgInt32 j) const
{
	_ASSERTE (i != j);
#define SwapMacro(a,b) Swap(a,b)

	SwapMacro (m_Jt[i], m_Jt[j]);
	SwapMacro (m_JMinv[i], m_JMinv[j]);

	SwapMacro (m_diagDamp[i], m_diagDamp[j]); 
	SwapMacro (m_invDJMinvJt[i], m_invDJMinvJt[j]);
	//SwapMacro (m_diagJMinvJt[i], m_diagJMinvJt[j]); 
	//SwapMacro (m_frictionThreshold[i], m_frictionThreshold[j]);
	//SwapMacro (m_jacobianIndexArray[i], m_jacobianIndexArray[j]); 
	//SwapMacro (m_bilateralForceBounds[i], m_bilateralForceBounds[j]);

	SwapMacro (m_normalForceIndex[i], m_normalForceIndex[j]);
	//	SwapMacro (m_lowerBoundFrictionForce[i], m_lowerBoundFrictionForce[j]);
	//	SwapMacro (m_upperBoundFrictionForce[i], m_upperBoundFrictionForce[j]);
	SwapMacro (m_lowerBoundFrictionCoefficent[i], m_lowerBoundFrictionCoefficent[j]);
	SwapMacro (m_upperBoundFrictionCoefficent[i], m_upperBoundFrictionCoefficent[j]);
	SwapMacro (m_jointFeebackForce[i], m_jointFeebackForce[j]);

	SwapMacro (m_coordenateAccel[i], m_coordenateAccel[j]);

	SwapMacro (m_force[i], m_force[j]);
	SwapMacro (m_accel[i], m_accel[j]);
	//	SwapMacro (m_forceStep[i], m_forceStep[j]);
	SwapMacro (m_deltaAccel[i], m_deltaAccel[j]);
	SwapMacro (m_deltaForce[i], m_deltaForce[j]);
}

void dgJacobianMemory::CalculateForcesSimulationMode (dgFloat32 maxAccNorm) const
{
	dgInt32 passes;
	dgInt32 prevJoint;
	dgInt32 maxPasses;
	dgInt32 forceRows;
	dgInt32 totalPassesCount;
	dgFloat32 akNum;
	dgFloat32 accNorm;
	dgFloat32* const force = m_force;
	dgFloat32* const accel = m_accel;
	const dgJacobianPair* const Jt = m_Jt;
	dgFloat32* const deltaAccel = m_deltaAccel;
	dgFloat32* const deltaForce = m_deltaForce;
	const dgJacobianPair* const JMinv = m_JMinv;
	const dgFloat32* const diagDamp = m_diagDamp;
	const dgFloat32* const invDJMinvJt = m_invDJMinvJt;

	dgJacobian* const internalForces = m_internalForces;
	dgInt32* const normalForceIndex = m_normalForceIndex;
	dgJointInfo* const constraintArray = m_constraintArray;
	const dgFloat32* const coordenateAccel = m_coordenateAccel;
	dgFloat32* const lowerForceBound = m_lowerBoundFrictionCoefficent;
	dgFloat32* const upperForceBound = m_upperBoundFrictionCoefficent;
	dgFloat32 forceStep[DG_CONSTRAINT_MAX_ROWS];	

	dgVector zero(dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));

	// initialize the intermediate force accumulation to zero 
	for (dgInt32 i = 0; i < m_bodyCount; i ++) {
		internalForces[i].m_linear = zero;
		internalForces[i].m_angular = zero;
	}

	for (dgInt32 i = 0; i < m_jointCount; i ++) {
		dgInt32 m0;
		dgInt32 m1;
		dgInt32 index;
		dgInt32 first;
		dgInt32 count;
		dgFloat32 val;
		dgJacobian y0;
		dgJacobian y1;

		first = constraintArray[i].m_autoPairstart;
		count = constraintArray[i].m_autoPairActiveCount;
		m0 = constraintArray[i].m_m0;
		m1 = constraintArray[i].m_m1;

		y0.m_linear = zero;
		y0.m_angular = zero;
		y1.m_linear = zero;
		y1.m_angular = zero;
		for (dgInt32 j = 0; j < count; j ++) {
			index = first + j;
			val = force[index]; 
			y0.m_linear += Jt[index].m_jacobian_IM0.m_linear.Scale (val);
			y0.m_angular += Jt[index].m_jacobian_IM0.m_angular.Scale (val);
			y1.m_linear += Jt[index].m_jacobian_IM1.m_linear.Scale (val);
			y1.m_angular += Jt[index].m_jacobian_IM1.m_angular.Scale (val);
		}
		internalForces[m0].m_linear += y0.m_linear;
		internalForces[m0].m_angular += y0.m_angular;
		internalForces[m1].m_linear += y1.m_linear;
		internalForces[m1].m_angular += y1.m_angular;
	}

	for (dgInt32 i = 0; i < DG_CONSTRAINT_MAX_ROWS; i ++) {
		forceStep[i] = dgFloat32 (0.0f);
	}

	maxPasses = 4;
	prevJoint = 0;
	accNorm = maxAccNorm * dgFloat32 (2.0f);
	for (passes = 0; (passes < maxPasses) && (accNorm > maxAccNorm); passes ++) {
		accNorm = dgFloat32 (0.0f);
		for (dgInt32 currJoint = 0; currJoint < m_jointCount; currJoint ++) {
			dgInt32 m0;
			dgInt32 m1;
			dgInt32 index;
			dgInt32 rowsCount;
			dgFloat32 jointAccel;
			dgJacobian y0;
			dgJacobian y1;

			index = constraintArray[prevJoint].m_autoPairstart;
			rowsCount = constraintArray[prevJoint].m_autoPaircount;
			m0 = constraintArray[prevJoint].m_m0;
			m1 = constraintArray[prevJoint].m_m1;

			y0.m_linear = zero;
			y0.m_angular = zero;
			y1.m_linear = zero;
			y1.m_angular = zero;
			for (dgInt32 i = 0; i < rowsCount; i ++) {
				dgFloat32 deltaForce;
				deltaForce = forceStep[i]; 
				y0.m_linear += Jt[index].m_jacobian_IM0.m_linear.Scale (deltaForce);
				y0.m_angular += Jt[index].m_jacobian_IM0.m_angular.Scale (deltaForce);
				y1.m_linear += Jt[index].m_jacobian_IM1.m_linear.Scale (deltaForce);
				y1.m_angular += Jt[index].m_jacobian_IM1.m_angular.Scale (deltaForce);
				index ++;
			}
			internalForces[m0].m_linear += y0.m_linear;
			internalForces[m0].m_angular += y0.m_angular;
			internalForces[m1].m_linear += y1.m_linear;
			internalForces[m1].m_angular += y1.m_angular;

			index = constraintArray[currJoint].m_autoPairstart;
			rowsCount = constraintArray[currJoint].m_autoPaircount;
			m0 = constraintArray[currJoint].m_m0;
			m1 = constraintArray[currJoint].m_m1;
			y0 = internalForces[m0];
			y1 = internalForces[m1];
			for (dgInt32 i = 0; i < rowsCount; i ++) {
				dgVector acc (JMinv[index].m_jacobian_IM0.m_linear.CompProduct(y0.m_linear));
				acc += JMinv[index].m_jacobian_IM0.m_angular.CompProduct (y0.m_angular);
				acc += JMinv[index].m_jacobian_IM1.m_linear.CompProduct (y1.m_linear);
				acc += JMinv[index].m_jacobian_IM1.m_angular.CompProduct (y1.m_angular);

				accel[i] = coordenateAccel[index] - acc.m_x - acc.m_y - acc.m_z - force[index] * diagDamp[index];
				index ++;
			}

			jointAccel = CalculateJointForces (currJoint, forceStep, maxAccNorm);
			accNorm = GetMax(accNorm, jointAccel);
			prevJoint = currJoint;
		}
	}


	for (dgInt32 i = 0; i < m_jointCount; i ++) {
		dgInt32 j;
		dgInt32 k;
		dgInt32 first;
		dgInt32 count;
		dgInt32 index;
		dgFloat32 val;
		first = constraintArray[i].m_autoPairstart;
		count = constraintArray[i].m_autoPaircount;
		constraintArray[i].m_autoPaircount = count;
		for (k = 0; k < count; k ++) {
			index = first + k;
			j = normalForceIndex[index];
			_ASSERTE (((j < 0) && (force[j] == dgFloat32 (1.0f))) || ((j >= 0) && (force[j] >= dgFloat32 (0.0f))));
			val = force[j];
			lowerForceBound[index] *= val;
			upperForceBound[index] *= val;

			val = force[index];
			force[index] = ClampValue(val, lowerForceBound[index], upperForceBound[index]);
		}
	}


	for (dgInt32 i = 0; i < m_bodyCount; i ++) {
		internalForces[i].m_linear[0] = dgFloat32(0.0f);
		internalForces[i].m_linear[1] = dgFloat32(0.0f);
		internalForces[i].m_linear[2] = dgFloat32(0.0f);
		internalForces[i].m_linear[3] = dgFloat32(0.0f);
		internalForces[i].m_angular[0] = dgFloat32(0.0f);
		internalForces[i].m_angular[1] = dgFloat32(0.0f);
		internalForces[i].m_angular[2] = dgFloat32(0.0f);
		internalForces[i].m_angular[3] = dgFloat32(0.0f);
	}

	for (dgInt32 i = 0; i < m_jointCount; i ++) {
		dgInt32 j;
		dgInt32 m0;
		dgInt32 m1;
		dgInt32 first;
		dgInt32 count;
		dgInt32 index;
		dgJacobian y0;
		dgJacobian y1;

		first = constraintArray[i].m_autoPairstart;
		count = constraintArray[i].m_autoPairActiveCount;
		m0 = constraintArray[i].m_m0;
		m1 = constraintArray[i].m_m1;
		y0.m_linear = zero;
		y0.m_angular = zero;
		y1.m_linear = zero;
		y1.m_angular = zero;
		for (j = 0; j < count; j ++) {
			dgFloat32 val;
			index = j + first;
			val = force[index]; 
			y0.m_linear += Jt[index].m_jacobian_IM0.m_linear.Scale (val);
			y0.m_angular += Jt[index].m_jacobian_IM0.m_angular.Scale (val);
			y1.m_linear += Jt[index].m_jacobian_IM1.m_linear.Scale (val);
			y1.m_angular += Jt[index].m_jacobian_IM1.m_angular.Scale (val);
		}
		internalForces[m0].m_linear += y0.m_linear;
		internalForces[m0].m_angular += y0.m_angular;
		internalForces[m1].m_linear += y1.m_linear;
		internalForces[m1].m_angular += y1.m_angular;
	}


	forceRows = 0;
	akNum = dgFloat32 (0.0f);
	accNorm = dgFloat32(0.0f);
	for (dgInt32 i = 0; i < m_jointCount; i ++) {
		dgInt32 m0;
		dgInt32 m1;
		dgInt32 first;
		dgInt32 count;
		dgInt32 index;
		dgInt32 activeCount;
		bool isClamped[DG_CONSTRAINT_MAX_ROWS];

		first = constraintArray[i].m_autoPairstart;
		count = constraintArray[i].m_autoPairActiveCount;
		m0 = constraintArray[i].m_m0;
		m1 = constraintArray[i].m_m1;

		const dgJacobian& y0 = internalForces[m0];
		const dgJacobian& y1 = internalForces[m1];
		for (dgInt32 j = 0; j < count; j ++) {
			index = j + first;
			dgVector tmpAccel (JMinv[index].m_jacobian_IM0.m_linear.CompProduct(y0.m_linear));
			tmpAccel += JMinv[index].m_jacobian_IM0.m_angular.CompProduct(y0.m_angular);
			tmpAccel += JMinv[index].m_jacobian_IM1.m_linear.CompProduct(y1.m_linear);
			tmpAccel += JMinv[index].m_jacobian_IM1.m_angular.CompProduct(y1.m_angular);
			accel[index] = coordenateAccel[index] - (tmpAccel.m_x + tmpAccel.m_y + tmpAccel.m_z + force[index] * diagDamp[index]);
		}

		activeCount = 0;
		for (dgInt32 j = 0; j < count; j ++) {
			dgFloat32 val;
			index = j + first;
			val = lowerForceBound[index] - force[index];
			if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (accel[index] < dgFloat32 (0.0f))) {
				force[index] = lowerForceBound[index];
				isClamped[j] = true;
			} else {
				val = upperForceBound[index] - force[index];
				if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (accel[index] > dgFloat32 (0.0f))) {
					force[index] = upperForceBound[index];
					isClamped[j] = true;
				} else {
					forceRows ++;
					activeCount ++;
					deltaForce[index] = accel[index] * invDJMinvJt[index];
					akNum += accel[index] * deltaForce[index];
					accNorm = GetMax (dgAbsf (accel[index]), accNorm);
					isClamped[j] = false;
				}
			}
		}

		if (activeCount < count) {
			dgInt32 i0;
			dgInt32 i1;

			i0 = 0;
			i1 = count - 1;
			constraintArray[i].m_autoPairActiveCount = activeCount;
			do { 
				while ((i0 <= i1) && !isClamped[i0]) i0 ++;
				while ((i0 <= i1) && isClamped[i1]) i1 --;
				if (i0 < i1) {
					SwapRows (first + i0, first + i1);
					i0 ++;
					i1 --;
				}
			} while (i0 < i1); 
		}
	}

	maxPasses = forceRows;
	totalPassesCount = 0;
	for (passes = 0; (passes < maxPasses) && (accNorm > maxAccNorm); passes ++) {
		dgInt32 clampedForceIndex;
		dgInt32 clampedForceJoint; 
		dgFloat32 ak;
		dgFloat32 akDen;
		dgFloat32 clampedForceIndexValue;

		for (dgInt32 i = 0; i < m_bodyCount; i ++) {
			internalForces[i].m_linear[0] = dgFloat32(0.0f);
			internalForces[i].m_linear[1] = dgFloat32(0.0f);
			internalForces[i].m_linear[2] = dgFloat32(0.0f);
			internalForces[i].m_linear[3] = dgFloat32(0.0f);
			internalForces[i].m_angular[0] = dgFloat32(0.0f);
			internalForces[i].m_angular[1] = dgFloat32(0.0f);
			internalForces[i].m_angular[2] = dgFloat32(0.0f);
			internalForces[i].m_angular[3] = dgFloat32(0.0f);
		}

		for (dgInt32 i = 0; i < m_jointCount; i ++) {
			dgInt32 j;
			dgInt32 m0;
			dgInt32 m1;
			dgInt32 first;
			dgInt32 count;
			dgInt32 index;
			dgFloat32 ak;
			dgJacobian y0;
			dgJacobian y1;

			first = constraintArray[i].m_autoPairstart;
			count = constraintArray[i].m_autoPairActiveCount;
			m0 = constraintArray[i].m_m0;
			m1 = constraintArray[i].m_m1;
			y0.m_linear = zero;
			y0.m_angular = zero;
			y1.m_linear = zero;
			y1.m_angular = zero;
			for (j = 0; j < count; j ++) {
				index = j + first;
				ak = deltaForce[index]; 
				y0.m_linear += Jt[index].m_jacobian_IM0.m_linear.Scale (ak);
				y0.m_angular += Jt[index].m_jacobian_IM0.m_angular.Scale (ak);
				y1.m_linear += Jt[index].m_jacobian_IM1.m_linear.Scale (ak);
				y1.m_angular += Jt[index].m_jacobian_IM1.m_angular.Scale (ak);
			}
			internalForces[m0].m_linear += y0.m_linear;
			internalForces[m0].m_angular += y0.m_angular;
			internalForces[m1].m_linear += y1.m_linear;
			internalForces[m1].m_angular += y1.m_angular;
		}


		akDen = dgFloat32 (0.0f);
		for (dgInt32 i = 0; i < m_jointCount; i ++) {
			dgInt32 j;
			dgInt32 m0;
			dgInt32 m1;
			dgInt32 first;
			dgInt32 count;
			dgInt32 index;
			first = constraintArray[i].m_autoPairstart;
			count = constraintArray[i].m_autoPairActiveCount;
			m0 = constraintArray[i].m_m0;
			m1 = constraintArray[i].m_m1;
			const dgJacobian& y0 = internalForces[m0];
			const dgJacobian& y1 = internalForces[m1];
			for (j = 0; j < count; j ++) {
				index = j + first;
				dgVector tmpAccel (JMinv[index].m_jacobian_IM0.m_linear.CompProduct(y0.m_linear));
				tmpAccel += JMinv[index].m_jacobian_IM0.m_angular.CompProduct(y0.m_angular);
				tmpAccel += JMinv[index].m_jacobian_IM1.m_linear.CompProduct(y1.m_linear);
				tmpAccel += JMinv[index].m_jacobian_IM1.m_angular.CompProduct(y1.m_angular);
				deltaAccel[index] = tmpAccel.m_x + tmpAccel.m_y + tmpAccel.m_z + deltaForce[index] * diagDamp[index];
				akDen += deltaAccel[index] * deltaForce[index];
			}
		}


		_ASSERTE (akDen > dgFloat32 (0.0f));
		akDen = GetMax (akDen, dgFloat32(1.0e-16f));
		_ASSERTE (dgAbsf (akDen) >= dgFloat32(1.0e-16f));
		ak = akNum / akDen;
		clampedForceIndex = -1;
		clampedForceJoint = -1;
		clampedForceIndexValue = dgFloat32 (0.0f);
		for (dgInt32 i = 0; i < m_jointCount; i ++) {
			dgInt32 j;
			dgInt32 first;
			dgInt32 count;
			dgInt32 index;
			dgFloat32 val;
			if (ak > dgFloat32 (1.0e-8f)) {
				first = constraintArray[i].m_autoPairstart;
				count = constraintArray[i].m_autoPairActiveCount;
				for (j = 0; j < count; j ++) {
					index = j + first;
					val = force[index] + ak * deltaForce[index];
					if (deltaForce[index] < dgFloat32 (-1.0e-16f)) {
						//if (val < bilateralForceBounds[index].m_low) {
						if (val < lowerForceBound[index]) {
							ak = GetMax ((lowerForceBound[index] - force[index]) / deltaForce[index], dgFloat32 (0.0f));
							_ASSERTE (ak >= dgFloat32 (0.0f));
							clampedForceIndex = j;
							clampedForceJoint = i;
							//clampedForceIndexValue = bilateralForceBounds[index].m_low;
							clampedForceIndexValue = lowerForceBound[index];
						}
					} else if (deltaForce[index] > dgFloat32 (1.0e-16f)) {
						//if (val > bilateralForceBounds[index].m_upper) {
						if (val > upperForceBound[index]) {
							ak = GetMax ((upperForceBound[index] - force[index]) / deltaForce[index], dgFloat32 (0.0f));
							_ASSERTE (ak >= dgFloat32 (0.0f));
							clampedForceIndex = j;
							clampedForceJoint = i;
							clampedForceIndexValue = upperForceBound[index];
						}
					}
				}
			}
		}


		if (clampedForceIndex >= 0) {
			dgInt32 first;
			dgInt32 count;
			dgInt32 activeCount;
			bool isClamped[DG_CONSTRAINT_MAX_ROWS];

			for (dgInt32 i = 0; i < m_jointCount; i ++) {
				dgInt32 j;
				dgInt32 first;
				dgInt32 count;
				dgInt32 index;
				first = constraintArray[i].m_autoPairstart;
				count = constraintArray[i].m_autoPairActiveCount;
				for (j = 0; j < count; j ++) {
					index = j + first;
					force[index] += ak * deltaForce[index];
					accel[index] -= ak * deltaAccel[index];
				}
			}


			first = constraintArray[clampedForceJoint].m_autoPairstart;
			count = constraintArray[clampedForceJoint].m_autoPairActiveCount;
			count --;
			force[first + clampedForceIndex] = clampedForceIndexValue;
			if (clampedForceIndex != count) {
				SwapRows (first + clampedForceIndex, first + count);
			}

			activeCount = count;
			for (dgInt32 i = 0; i < count; i ++) {
				dgInt32 index;
				dgFloat32 val;
				index = first + i;
				isClamped[i] = false;
				val = lowerForceBound[index] - force[index];
				if ((val > dgFloat32 (-1.0e-5f)) && (accel[index] < dgFloat32 (0.0f))) {
					activeCount --;
					isClamped[i] = true;
				} else {
					val = upperForceBound[index] - force[index];
					if ((val < dgFloat32 (1.0e-5f)) && (accel[index] > dgFloat32 (0.0f))) {
						activeCount --;
						isClamped[i] = true;
					}
				}
			}

			if (activeCount < count) {
				dgInt32 i0;
				dgInt32 i1;
				i0 = 0;
				i1 = count - 1;
				do { 
					while ((i0 <= i1) && !isClamped[i0]) i0 ++;
					while ((i0 <= i1) && isClamped[i1]) i1 --;
					if (i0 < i1) {
						SwapRows (first + i0, first + i1);
						//Swap (isClamped[i0], isClamped[i1]);
						//Swap (permutationIndex[i0], permutationIndex[i1]);
						i0 ++;
						i1 --;
					}
				} while (i0 < i1); 
			}
			constraintArray[clampedForceJoint].m_autoPairActiveCount = activeCount;

			forceRows = 0;
			akNum = dgFloat32 (0.0f);
			accNorm = dgFloat32(0.0f);
			for (dgInt32 i = 0; i < m_jointCount; i ++) {
				dgInt32 j;
				dgInt32 first;
				dgInt32 count;
				dgInt32 index;
				first = constraintArray[i].m_autoPairstart;
				count = constraintArray[i].m_autoPairActiveCount;
				forceRows += count;
				for (j = 0; j < count; j ++) {
					index = first + j;
					_ASSERTE ((i != clampedForceJoint) || !((dgAbsf (lowerForceBound[index] - force[index]) < dgFloat32 (1.0e-5f)) && (accel[index] < dgFloat32 (0.0f))));
					_ASSERTE ((i != clampedForceJoint) || !((dgAbsf (upperForceBound[index] - force[index]) < dgFloat32 (1.0e-5f)) && (accel[index] > dgFloat32 (0.0f))));
					deltaForce[index] = accel[index] * invDJMinvJt[index];
					akNum += deltaForce[index] * accel[index];
					accNorm = GetMax (dgAbsf (accel[index]), accNorm);
					_ASSERTE (dgCheckFloat(deltaForce[index]));
				}
			}

			_ASSERTE (akNum >= dgFloat32 (0.0f));
			passes = -1;
			maxPasses = forceRows;


		} else {
			accNorm = dgFloat32(0.0f);
			for (dgInt32 i = 0; i < m_jointCount; i ++) {
				dgInt32 j;
				dgInt32 first;
				dgInt32 count;
				dgInt32 index;
				first = constraintArray[i].m_autoPairstart;
				count = constraintArray[i].m_autoPairActiveCount;
				for (j = 0; j < count; j ++) {
					index = j + first;
					force[index] += ak * deltaForce[index];
					accel[index] -= ak * deltaAccel[index];
					accNorm = GetMax (dgAbsf (accel[index]), accNorm);
				}
			}

			if (accNorm > maxAccNorm) {
				akDen = akNum;
				akNum = dgFloat32(0.0f);
				for (dgInt32 i = 0; i < m_jointCount; i ++) {
					dgInt32 j;
					dgInt32 first;
					dgInt32 count;
					dgInt32 index;
					first = constraintArray[i].m_autoPairstart;
					count = constraintArray[i].m_autoPairActiveCount;
					for (j = 0; j < count; j ++) {
						index = j + first;
						deltaAccel[index] = accel[index] * invDJMinvJt[index];
						akNum += accel[index] * deltaAccel[index];
					}
				}

				_ASSERTE (akNum >= dgFloat32 (0.0f));
				_ASSERTE (akDen > dgFloat32(0.0f));
				akDen = GetMax (akDen, dgFloat32 (1.0e-17f));
				ak = dgFloat32 (akNum / akDen);
				for (dgInt32 i = 0; i < m_jointCount; i ++) {
					dgInt32 j;
					dgInt32 first;
					dgInt32 count;
					dgInt32 index;
					first = constraintArray[i].m_autoPairstart;
					count = constraintArray[i].m_autoPairActiveCount;
					for (j = 0; j < count; j ++) {
						index = j + first;
						deltaForce[index] = deltaAccel[index] + ak * deltaForce[index];
					}
				}
			}
		}
		totalPassesCount ++;
	}
	ApplyExternalForcesAndAcceleration (maxAccNorm);
}


dgFloat32 dgJacobianMemory::CalculateJointForcesSimd (dgInt32 joint, dgFloat32* forceStep, dgFloat32 maxAccNorm) const
{

#ifdef DG_BUILD_SIMD_CODE

	dgInt32 m0;
	dgInt32 m1;
	dgInt32 first;
	dgInt32 count;
	dgInt32 roundCount;
	dgInt32 maxPasses;
	dgInt32 clampedForceIndex;
	dgFloat32 ak;
	dgFloat32 akNum;
	dgFloat32 retAccel;
	dgFloatSign tmpIndex;
	//dgFloat32 akDen;
	//dgFloat32 force;
	dgFloat32 accNorm;
	//dgFloat32 retAccNorm;
	dgFloat32 clampedForceIndexValue;
	//dgJacobian y0;
	//dgJacobian y1;
	//	simd_type tmp0;
	//	simd_type tmp1;
	//	simd_type tmp2;
	//	simd_type tmp3;

	simd_type akNumSimd;
	simd_type accNormSimd;
	simd_type maxPassesSimd;
	simd_type y0_linear;
	simd_type y0_angular;
	simd_type y1_linear;
	simd_type y1_angular;
	simd_type one;
	simd_type zero;
	simd_type signMask;
	simd_type tol_pos_1eNeg5;
	simd_type tol_pos_1eNeg8;
	simd_type tol_neg_1eNeg16;
	simd_type tol_pos_1eNeg16;
	simd_type deltaAccelPtr[DG_CONSTRAINT_MAX_ROWS / DG_SIMD_WORD_SIZE];
	simd_type deltaForcePtr[DG_CONSTRAINT_MAX_ROWS / DG_SIMD_WORD_SIZE];
	simd_type activeRowPtr[DG_CONSTRAINT_MAX_ROWS / DG_SIMD_WORD_SIZE];
	simd_type lowBoundPtr[DG_CONSTRAINT_MAX_ROWS / DG_SIMD_WORD_SIZE];
	simd_type highBoundPtr[DG_CONSTRAINT_MAX_ROWS / DG_SIMD_WORD_SIZE];


	dgFloat32* const deltaAccel = (dgFloat32*)deltaAccelPtr;
	dgFloat32* const deltaForce =  (dgFloat32*)deltaForcePtr;
	dgFloat32* const activeRow = (dgFloat32*)activeRowPtr;
	dgFloat32* const lowBound = (dgFloat32*)lowBoundPtr;
	dgFloat32* const highBound =  (dgFloat32*)highBoundPtr;

	dgFloat32* const accel = m_accel;
	dgFloat32* const force = m_force;
	const dgJacobianPair* const Jt = m_Jt;
	const dgJacobianPair* const JMinv = m_JMinv;
	const dgFloat32* const diagDamp = m_diagDamp;
	const dgFloat32* const invDJMinvJt = m_invDJMinvJt;
	//	const dgFloat32* const lowerFrictionForce = m_lowerBoundFrictionCoefficent;
	//	const dgFloat32* const upperFrictionForce = m_upperBoundFrictionCoefficent;
	const dgFloat32* const lowerFrictionCoef = m_lowerBoundFrictionCoefficent;
	const dgFloat32* const upperFrictionCoef = m_upperBoundFrictionCoefficent;

	const dgInt32* const normalForceIndex = m_normalForceIndex;
	const dgJointInfo* const constraintArray = m_constraintArray;
	//	const dgJacobianIndex* const jacobianIndexArray = m_jacobianIndexArray;

	//	count = 0x7fffffff;
	//	signMask = simd_set1((dgFloat32&) count);
	tmpIndex.m_integer.m_iVal = 0x7fffffff;
	signMask = simd_set1(tmpIndex.m_fVal);

	one = simd_set1(dgFloat32 (1.0f));
	zero = simd_set1(dgFloat32 (0.0f));
	tol_pos_1eNeg8 = simd_set1 (dgFloat32 (1.0e-8f));
	tol_pos_1eNeg5 = simd_set1 (dgFloat32 (1.0e-5f));
	tol_pos_1eNeg16 = simd_set1 (dgFloat32 ( 1.0e-16f));
	tol_neg_1eNeg16 = simd_set1 (dgFloat32 (-1.0e-16f));

	first = constraintArray[joint].m_autoPairstart;
	count = constraintArray[joint].m_autoPaircount;
	m0 = constraintArray[joint].m_m0;
	m1 = constraintArray[joint].m_m1;


	roundCount = count & (-DG_SIMD_WORD_SIZE);
	if (roundCount != count) {
		roundCount += 4;

		for (dgInt32 j = count; j < roundCount; j ++) {
			dgInt32 i;
			i = first + j; 
			//force[i] = dgFloat32 (0.0f);
			//accel[j] -= dgFloat32 (0.0f);;
			//activeRow[j] = dgFloat32 (0.0f);
			//deltaAccel[j] = dgFloat32 (0.0f);
			//deltaForce[j] = dgFloat32 (0.0f);
			m_normalForceIndex[i] = -1;
			simd_store_s (zero, &force[i]);
			simd_store_s (zero, &accel[j]);
			simd_store_s (zero, &activeRow[j]);
			simd_store_s (zero, &deltaAccel[j]);
			simd_store_s (zero, &deltaForce[j]);
			simd_store_s (one, &m_lowerBoundFrictionCoefficent[i]);
			simd_store_s (zero, &m_upperBoundFrictionCoefficent[i]);
		}
	}

	//akNum = dgFloat32 (0.0f);
	//accNorm = dgFloat32(0.0f);
	//maxPasses = count;
	akNumSimd = zero;
	accNormSimd = zero;
	maxPassesSimd = zero;
	//	tmp3 = simd_set1(dgFloat32 (1.0f));

	//for (j = 0; j < count; j ++) {
	for (dgInt32 j = 0; j < roundCount; j += DG_SIMD_WORD_SIZE) {
		dgInt32 i;
		//dgInt32 k0;
		//dgInt32 k1;
		//dgInt32 k2;
		//dgInt32 k3;
		//simd_type index_k;
		//simd_type accel_j;
		simd_type force_k;
		//simd_type force_i;
		//simd_type lowBound_j;
		//simd_type higntBound_j;
		//simd_type deltaforce_j;
		simd_type lowHighBound_test;

		i = first + j; 
		//k = bilateralForceBounds[i].m_normalIndex;
		//k0 = normalForceIndex[i + 0];
		//k1 = normalForceIndex[i + 1];
		//k2 = normalForceIndex[i + 2];
		//k3 = normalForceIndex[i + 3];

		//val = (k >= 0) ? force[k] : dgFloat32 (1.0f);
		//val = force[k];
		//index_k = simd_move_lh_v (simd_pack_lo_v (simd_load_is (zero, k0), simd_load_is (zero, k1)), simd_pack_lo_v (simd_load_is (zero, k2), simd_load_is (zero, k3)));
		//index_k = simd_cmpge_v (index_k, zero);
		force_k = simd_move_lh_v (simd_pack_lo_v (simd_load_s(force[normalForceIndex[i + 0]]), simd_load_s(force[normalForceIndex[i + 1]])), simd_pack_lo_v (simd_load_s(force[normalForceIndex[i + 2]]), simd_load_s(force[normalForceIndex[i + 3]])));
		//force_k = simd_or_v (simd_and_v(force_k, index_k), simd_andnot_v (one, index_k));

		//lowBound[j] = val * bilateralForceBounds[i].m_low;
		//highBound[j] = val * bilateralForceBounds[i].m_upper;
		(simd_type&)lowBound[j] = simd_mul_v (force_k, (simd_type&)lowerFrictionCoef[i]);
		(simd_type&)highBound[j] = simd_mul_v (force_k, (simd_type&)upperFrictionCoef[i]);


		//activeRow[j] = dgFloat32 (1.0f);
		//forceStep[j] = m_force[i];
		//if (force[i] < lowBound[j]) {
		//	maxPasses --;
		//	force[i] = lowBound[j];
		//	activeRow[j] = dgFloat32 (0.0f);
		//} else if (force[i] > highBound[j]) {
		//	maxPasses --;
		//	force[i] = highBound[j];
		//	activeRow[j] = dgFloat32 (0.0f);
		//}

		(simd_type&)forceStep[j] = (simd_type&) force[i];
		//		lowBound_test = simd_cmplt_v (force_i, lowBound_j);
		//		higntBound_test = simd_cmpgt_v (force_i, higntBound_j);
		lowHighBound_test = simd_or_v (simd_cmplt_v ((simd_type&) force[i], (simd_type&)lowBound[j]), simd_cmpgt_v ((simd_type&) force[i], (simd_type&)highBound[j]));
		(simd_type&)activeRow[j] = simd_andnot_v (one, lowHighBound_test);
		maxPassesSimd = simd_add_v (maxPassesSimd, (simd_type&)activeRow[j]);

		//force_k = simd_or_v (simd_and_v (lowBound_j, lowBound_test), simd_and_v (higntBound_j, higntBound_test));
		//(simd_type&)force[i] = simd_mul_v (activeRow_j, simd_or_v (simd_and_v (force_k, lowHighBound_test), simd_andnot_v (force_i, lowHighBound_test)));
		(simd_type&) force[i] = simd_min_v ((simd_type&)highBound[j], simd_max_v((simd_type&) force[i], (simd_type&)lowBound[j]));


		//deltaForce[j] = accel[j] * invDJMinvJt[i] * activeRow[j];
		(simd_type&)deltaForce[j] = simd_mul_v ((simd_type&)accel[j] , simd_mul_v ((simd_type&)invDJMinvJt[i], (simd_type&)activeRow[j]));

		//akNum += accel[j] * deltaForce[j];
		akNumSimd = simd_mul_add_v (akNumSimd, (simd_type&)accel[j], (simd_type&)deltaForce[j]);

		//accNorm = GetMax (dgAbsf (accel[j] * activeRow[j]), accNorm);
		accNormSimd = simd_max_v (accNormSimd, simd_and_v (simd_mul_v ((simd_type&)accel[j], (simd_type&)activeRow[j]), signMask));
	}
	akNumSimd = simd_add_v (akNumSimd, simd_move_hl_v(akNumSimd, akNumSimd));
	simd_store_s (simd_add_s (akNumSimd, simd_permut_v (akNumSimd, akNumSimd, PURMUT_MASK (0, 0, 0, 1))), &akNum);

	accNormSimd = simd_max_v (accNormSimd, simd_move_hl_v(accNormSimd, accNormSimd));
	simd_store_s (simd_max_s (accNormSimd, simd_permut_v (accNormSimd, accNormSimd, PURMUT_MASK (0, 0, 0, 1))), &accNorm);

	maxPassesSimd = simd_add_v (maxPassesSimd, simd_move_hl_v (maxPassesSimd, maxPassesSimd));
	maxPasses = simd_store_is (simd_add_s (maxPassesSimd, simd_permut_v (maxPassesSimd, maxPassesSimd, PURMUT_MASK (0, 0, 0, 1))));


	retAccel = accNorm;
	clampedForceIndexValue = dgFloat32(0.0f);
	for (dgInt32 i = 0; (i < maxPasses) && (accNorm > maxAccNorm); i ++) {
		simd_type akSimd;
		simd_type akDenSimd;

		//y0.m_linear = zero;
		//y0.m_angular = zero;
		//y1.m_linear = zero;
		//y1.m_angular = zero;
		y0_linear = zero;
		y0_angular = zero;
		y1_linear = zero;
		y1_angular = zero;
		for (dgInt32 j = 0; j < count; j ++) {
			dgInt32 k;
			simd_type tmp1;
			k = j + first;
			//ak = deltaForce[j]; 
			tmp1 = simd_set1(deltaForce[j]);
			//y0.m_linear += m_Jt[k].m_jacobian_IM0.m_linear.Scale (ak);
			//y0.m_angular += m_Jt[k].m_jacobian_IM0.m_angular.Scale (ak);
			//y1.m_linear += m_Jt[k].m_jacobian_IM1.m_linear.Scale (ak);
			//y1.m_angular += m_Jt[k].m_jacobian_IM1.m_angular.Scale (ak);

			y0_linear  = simd_mul_add_v (y0_linear, (simd_type&) Jt[k].m_jacobian_IM0.m_linear, tmp1);
			y0_angular = simd_mul_add_v (y0_angular,(simd_type&) Jt[k].m_jacobian_IM0.m_angular, tmp1);
			y1_linear  = simd_mul_add_v (y1_linear, (simd_type&) Jt[k].m_jacobian_IM1.m_linear, tmp1);
			y1_angular = simd_mul_add_v (y1_angular,(simd_type&) Jt[k].m_jacobian_IM1.m_angular, tmp1);
		}

		//akDen = dgFloat32 (0.0f);
		akDenSimd = zero;
		for (dgInt32 j = 0; j < count; j ++) {
			dgInt32 k;
			simd_type tmp1;
			k = j + first;

			//dgVector acc (m_JMinv[k].m_jacobian_IM0.m_linear.CompProduct(y0.m_linear));
			//acc += m_JMinv[k].m_jacobian_IM0.m_angular.CompProduct(y0.m_angular);
			//acc += m_JMinv[k].m_jacobian_IM1.m_linear.CompProduct(y1.m_linear);
			//acc += m_JMinv[k].m_jacobian_IM1.m_angular.CompProduct(y1.m_angular);
			tmp1 = simd_mul_v (          (simd_type&)JMinv[k].m_jacobian_IM0.m_linear, y0_linear);
			tmp1 = simd_mul_add_v (tmp1, (simd_type&)JMinv[k].m_jacobian_IM0.m_angular, y0_angular);
			tmp1 = simd_mul_add_v (tmp1, (simd_type&)JMinv[k].m_jacobian_IM1.m_linear, y1_linear);
			tmp1 = simd_mul_add_v (tmp1, (simd_type&)JMinv[k].m_jacobian_IM1.m_angular, y1_angular);

			//deltaAccel[j] = acc.m_x + acc.m_y + acc.m_z + deltaForce[j] * m_diagDamp[k];
			tmp1 = simd_add_v (tmp1, simd_move_hl_v(tmp1, tmp1));
			tmp1 = simd_add_s(tmp1, simd_permut_v (tmp1, tmp1, PURMUT_MASK(3, 3, 3, 1)));
			tmp1 = simd_mul_add_s(tmp1, simd_load_s(deltaForce[j]), simd_load_s(diagDamp[k]));
			simd_store_s (tmp1, &deltaAccel[j]);

			//akDen += deltaAccel[j] * deltaForce[j];
			akDenSimd = simd_mul_add_s (akDenSimd, tmp1, simd_load_s(deltaForce[j])); 
		}

		//_ASSERTE (akDen > dgFloat32 (0.0f));
		//akDen = GetMax (akDen, dgFloat32(1.0e-16f));
		//_ASSERTE (dgAbsf (akDen) >= dgFloat32(1.0e-16f));
		//ak = akNum / akDen;
		akSimd = simd_div_s(simd_load_s(akNum), simd_max_s (akDenSimd, tol_pos_1eNeg16));

		//		simd_store_s (tmp0, &ak);
		//		clampedForceIndex = -1;
		simd_type min_index;
		simd_type minClampIndex;
		simd_type min_index_step;
		simd_type campedIndexValue;

		campedIndexValue = zero;
		minClampIndex = simd_set1 (dgFloat32 (-1.0f));
		min_index_step = simd_set1 (dgFloat32 (4.0f));
		akSimd = simd_permut_v (akSimd, akSimd, PURMUT_MASK (0, 0, 0, 0));
		min_index = simd_set (dgFloat32(0.0f), dgFloat32(1.0f), dgFloat32(2.0f), dgFloat32(3.0f));

		//		for (j = 0; j < roundCount; j ++) {
		for (dgInt32 j = 0; j < roundCount; j += DG_SIMD_WORD_SIZE) {
			//			if (activeRow[j]) {
			//			dgFloat32 val;
			//			k = j + first;
			//			if (deltaForce[j] < dgFloat32 (-1.0e-16f)) {
			//			val = force[k] + ak * deltaForce[j];
			//			if (val < lowBound[j]) {
			//			ak = (lowBound[j] - force[k]) / deltaForce[j];
			//			clampedForceIndex = j;
			//			clampedForceIndexValue = lowBound[j];
			//			if (ak < dgFloat32 (1.0e-8f)) {
			//			ak = dgFloat32 (0.0f);
			//			break;
			//			}
			//			}
			//			} else if (deltaForce[j] > dgFloat32 (1.0e-16f)) {
			//			val = force[k] + ak * deltaForce[j];
			//			if (val > highBound[j]) {
			//			ak = (highBound[j] - force[k]) / deltaForce[j];
			//			clampedForceIndex = j;
			//			clampedForceIndexValue = highBound[j];
			//			if (ak < dgFloat32 (1.0e-8f)) {
			//			ak = dgFloat32 (0.0f);
			//			break;
			//			}
			//			}
			//			}
			//			}
			//			
			//bool test;
			//bool negTest;
			//bool posTest;
			//bool negValTest;
			//bool posValTest;
			//bool negDeltaForceTest;
			//bool posDeltaForceTest;
			//dgFloat32 val;
			//dgFloat32 num;
			//dgFloat32 den;

			dgInt32 k;
			simd_type val;
			simd_type num;
			simd_type den;
			simd_type test;
			simd_type negTest;
			simd_type posTest;
			simd_type negValTest;
			simd_type posValTest;
			simd_type negDeltaForceTest;
			simd_type posDeltaForceTest;

			// Make sure AK is not negative
			k = j + first;
			//val = force[k] + ak * deltaForce[j];
			val = simd_mul_add_v ((simd_type&)force[k], akSimd, (simd_type&)deltaForce[j]);

			//negValTest = val < lowBound[j];
			negValTest = simd_cmplt_v (val, (simd_type&)lowBound[j]);

			//posValTest = val > highBound[j];
			posValTest = simd_cmpgt_v (val, (simd_type&)highBound[j]);

			//negDeltaForceTest = deltaForce[j] < dgFloat32 (-1.0e-16f);
			negDeltaForceTest = simd_cmplt_v ((simd_type&)deltaForce[j], tol_neg_1eNeg16);

			//posDeltaForceTest = deltaForce[j] > dgFloat32 ( 1.0e-16f);
			posDeltaForceTest = simd_cmpgt_v ((simd_type&)deltaForce[j], tol_pos_1eNeg16);

			//negTest = negValTest & negDeltaForceTest;
			negTest = simd_and_v (negValTest,  negDeltaForceTest);

			//posTest = posValTest & posDeltaForceTest;
			posTest = simd_and_v (posValTest, posDeltaForceTest);

			//test = negTest | posTest;
			test = simd_or_v (negTest, posTest);

			//num = negTest ? lowBound[j] : (posTest ? highBound[j] : force[k]);
			num = simd_or_v (simd_and_v ((simd_type&)lowBound[j], negTest), simd_and_v ((simd_type&)highBound[j], posTest)); 
			num = simd_or_v (simd_and_v (num, test), simd_andnot_v ((simd_type&)force[k], test)); 

			//den = test ? deltaForce[j] : dgFloat32 (1.0f);
			den = simd_or_v (simd_and_v ((simd_type&)deltaForce[j], test), simd_andnot_v (one, test)); 

			//test = test & (activeRow[j] > dgFloat32 (0.0f));
			test = simd_and_v (test, simd_cmpgt_v ((simd_type&)activeRow[j], zero));

			//_ASSERTE (dgAbsf (den) > 1.0e-16f);
			//ak = test ? (num - force[k]) / den : ak;
			akSimd = simd_or_v (simd_div_v (simd_sub_v (num, (simd_type&)force[k]), den), simd_andnot_v (akSimd, test)); 

			//ak = (ak < dgFloat32 (1.0e-8f)) ? dgFloat32 (0.0f) : ak;
			akSimd = simd_and_v(akSimd, simd_cmpgt_v (akSimd, tol_pos_1eNeg8)); 

			//clampedForceIndex = test ? j : clampedForceIndex;
			minClampIndex = simd_or_v (simd_and_v (min_index, test), simd_andnot_v (minClampIndex, test));
			min_index = simd_add_v (min_index, min_index_step);

			//clampedForceIndexValue = test ? num : clampedForceIndexValue;
			campedIndexValue = simd_or_v (simd_and_v (num, test), simd_andnot_v (campedIndexValue, test));
		}

		akDenSimd = simd_move_hl_v(akSimd, akSimd);
		maxPassesSimd = simd_cmplt_v (akSimd, akDenSimd);
		akSimd = simd_min_v (akSimd, akDenSimd);
		minClampIndex = simd_or_v (simd_and_v (minClampIndex, maxPassesSimd), simd_andnot_v (simd_move_hl_v(minClampIndex, minClampIndex), maxPassesSimd));
		campedIndexValue = simd_or_v (simd_and_v (campedIndexValue, maxPassesSimd), simd_andnot_v (simd_move_hl_v(campedIndexValue, campedIndexValue), maxPassesSimd));

		akDenSimd = simd_permut_v (akSimd, akSimd, PURMUT_MASK (0, 0, 0, 1));
		maxPassesSimd = simd_cmplt_s (akSimd, akDenSimd);
		akSimd = simd_min_s (akSimd, akDenSimd);
		minClampIndex = simd_or_v (simd_and_v (minClampIndex, maxPassesSimd), simd_andnot_v (simd_permut_v (minClampIndex, minClampIndex, PURMUT_MASK (0, 0, 0, 1)), maxPassesSimd));
		campedIndexValue = simd_or_v (simd_and_v (campedIndexValue, maxPassesSimd), simd_andnot_v (simd_permut_v (campedIndexValue, campedIndexValue, PURMUT_MASK (0, 0, 0, 1)), maxPassesSimd));

		//		tmp2 = zero;
		simd_store_s (akSimd, &ak);
		clampedForceIndex = simd_store_is (minClampIndex);
		simd_store_s (campedIndexValue, &clampedForceIndexValue);
		if (ak == dgFloat32 (0.0f) && (clampedForceIndex != -1)) {
			_ASSERTE (clampedForceIndex !=-1);

			//akNum = dgFloat32 (0.0f);
			//accNorm = dgFloat32(0.0f);
			akNumSimd = zero;
			accNormSimd = zero;
			maxPassesSimd = zero;

			activeRow[clampedForceIndex] = dgFloat32 (0.0f);
			deltaForce[clampedForceIndex] = dgFloat32 (0.0f);
			force[clampedForceIndex + first] = clampedForceIndexValue;

			//for (j = 0; j < count; j ++) {
			for (dgInt32 j = 0; j < roundCount; j += DG_SIMD_WORD_SIZE) {
				//for (j = 0; j < roundCount; j ++) {
				//				if (((dgFloat32*)activeRow)[j]) {
				//				bool test0;
				//				bool test1;
				//				k = j + first;
				//				val = ((dgFloat32*)lowBound)[j] - force[k];
				//				if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (accel[j] < dgFloat32 (0.0f))) {
				//				force[k] = lowBound[j];
				//				activeRow[j] = dgFloat32 (0.0f);
				//				deltaForce[j] = dgFloat32 (0.0f); 
				//
				//				} else {
				//				val = highBound[j] - force[k];
				//				if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (accel[j] > dgFloat32 (0.0f))) {
				//				force[k] = highBound[j];
				//				activeRow[j] = dgFloat32 (0.0f);
				//				deltaForce[j] = dgFloat32 (0.0f); 
				//				} else {
				//				_ASSERTE (activeRow[j] > dgFloat32 (0.0f));
				//				deltaForce[j] = accel[j] * invDJMinvJt[k];
				//				akNum += accel[j] * deltaForce[j];
				//				accNorm = GetMax (dgAbsf (accel[j]), accNorm);
				//				}
				//				}
				//				}

				dgInt32 k;
				simd_type val_k;
				simd_type test_0;
				simd_type test_1;
				//simd_type test_2;
				simd_type accel_k;
				simd_type force_k;

				k = j + first;
				accel_k = (simd_type&) accel[j];
				force_k = (simd_type&) force[k];
				//val = dgAbsf (lowBound[j] - force[k]);
				val_k = simd_and_v (simd_sub_v ((simd_type&)lowBound[j], force_k), signMask);

				//test0 = (val < dgFloat32 (1.0e-5f)) & (accel[j] < dgFloat32 (0.0f));
				test_0 = simd_and_v (simd_cmplt_v (val_k, tol_pos_1eNeg5), simd_cmplt_v (accel_k, zero)); 

				//val = dgAbsf (highBound[j] - force[k]);
				val_k = simd_and_v (simd_sub_v ((simd_type&)highBound[j], force_k), signMask);

				//test1 = (val < dgFloat32 (1.0e-5f)) & (accel[j] > dgFloat32 (0.0f));
				test_1 = simd_and_v (simd_cmplt_v (val_k, tol_pos_1eNeg5), simd_cmpgt_v (accel_k, zero)); 

				//force[k] = test0 ? lowBound[j] : (test1 ? highBound[j] : force[k]);
				//val_k = simd_or_v (simd_and_v ((simd_type&)lowBound[j], test_0), simd_and_v ((simd_type&)highBound[j], test_1));
				//(simd_type&) force[k] = simd_or_v (simd_and_v (val_k, test_2) , simd_andnot_v (force_k, test_2));
				(simd_type&) force[k] = simd_min_v ((simd_type&) highBound[j], simd_max_v((simd_type&) force[k], (simd_type&)lowBound[j]));

				//activeRow[j] *= (test0 | test1) ? dgFloat32 (0.0f) : dgFloat32 (1.0f);
				//test_2 = simd_or_v (test_0, test_1);
				(simd_type&) activeRow[j] = simd_mul_v ((simd_type&) activeRow[j], simd_andnot_v (one, simd_or_v (test_0, test_1)));

				//deltaForce[j] = accel[j] * invDJMinvJt[k] * activeRow[j];
				(simd_type&)deltaForce[j] = simd_mul_v (accel_k, simd_mul_v ((simd_type&) invDJMinvJt[k], (simd_type&) activeRow[j]));

				//akNum += accel[j] * deltaForce[j];
				akNumSimd = simd_mul_add_v(akNumSimd, (simd_type&)deltaForce[j], accel_k);

				//accNorm = GetMax (dgAbsf (accel[j] * activeRow[j]), accNorm);
				accNormSimd = simd_max_v(accNormSimd, simd_and_v(simd_mul_v (accel_k, (simd_type&)activeRow[j]), signMask));

				// masPases += 1;
				maxPassesSimd = simd_add_v (maxPassesSimd, (simd_type&)activeRow[j]);
			}

			_ASSERTE (activeRow[clampedForceIndex] == dgFloat32 (0.0f));

			akNumSimd = simd_add_v (akNumSimd, simd_move_hl_v (akNumSimd, akNumSimd));
			akNumSimd = simd_add_s (akNumSimd, simd_permut_v (akNumSimd, akNumSimd, PURMUT_MASK (0, 0, 0, 1)));
			simd_store_s (akNumSimd, &akNum);

			accNormSimd = simd_max_v (accNormSimd, simd_move_hl_v (accNormSimd, accNormSimd));
			accNormSimd = simd_max_s (accNormSimd, simd_permut_v (accNormSimd, accNormSimd, PURMUT_MASK (0, 0, 0, 1)));
			simd_store_s (accNormSimd, &accNorm);

			i = -1;
			//maxPasses = GetMax (maxPasses - 1, 1); 
			maxPassesSimd = simd_add_v (maxPassesSimd, simd_move_hl_v (maxPassesSimd, maxPassesSimd));
			maxPasses = simd_store_is (simd_add_s (maxPassesSimd, simd_permut_v (maxPassesSimd, maxPassesSimd, PURMUT_MASK (0, 0, 0, 1))));

		} else if (clampedForceIndex >= 0) {
			//akNum = dgFloat32(0.0f);
			//accNorm = dgFloat32(0.0f);

			akNumSimd = zero;
			accNormSimd = zero;
			//			tmp2 = zero;
			akSimd = simd_permut_v (akSimd, akSimd, PURMUT_MASK(0, 0, 0, 0));
			activeRow[clampedForceIndex] = dgFloat32 (0.0f);

			//for (j = 0; j < count; j ++) {
			for (dgInt32 j = 0; j < roundCount; j += DG_SIMD_WORD_SIZE) {
				dgInt32 k;
				k = j + first;
				//m_force[k] += ak * deltaForce[j];
				//m_accel[j] -= ak * deltaAccel[j];
				(simd_type&)force[k] = simd_mul_add_v((simd_type&)force[k], akSimd, (simd_type&)deltaForce[j]);
				(simd_type&)accel[j] = simd_mul_sub_v((simd_type&)accel[j], akSimd, (simd_type&)deltaAccel[j]);

				//accNorm = GetMax (dgAbsf (m_accel[j] * activeRow[j]), accNorm);
				accNormSimd = simd_max_v(accNormSimd, simd_and_v(simd_mul_v ((simd_type&)accel[j], (simd_type&)activeRow[j]), signMask));
				//_ASSERTE (dgCheckFloat(m_force[k]));
				//_ASSERTE (dgCheckFloat(m_accel[j]));

				//deltaForce[j] = m_accel[j] * m_invDJMinvJt[k] * activeRow[j];
				(simd_type&)deltaForce[j] = simd_mul_v((simd_type&)accel[j], simd_mul_v((simd_type&)invDJMinvJt[k], (simd_type&)activeRow[j]));

				//akNum += deltaForce[j] * m_accel[j];
				akNumSimd = simd_mul_add_v(akNumSimd, (simd_type&)deltaForce[j], (simd_type&)accel[j]);
			}

			akNumSimd = simd_add_v (akNumSimd, simd_move_hl_v(akNumSimd, akNumSimd));
			akNumSimd = simd_add_s (akNumSimd, simd_permut_v (akNumSimd, akNumSimd, PURMUT_MASK (0, 0, 0, 1)));
			simd_store_s (akNumSimd, &akNum);

			accNormSimd = simd_max_v (accNormSimd, simd_move_hl_v(accNormSimd, accNormSimd));
			accNormSimd = simd_max_s (accNormSimd, simd_permut_v (accNormSimd, accNormSimd, PURMUT_MASK (0, 0, 0, 1)));
			simd_store_s (accNormSimd, &accNorm);

			force[clampedForceIndex + first] = clampedForceIndexValue;

			i = -1;
			maxPasses = GetMax (maxPasses - 1, 1); 

		} else {
			//accNorm = dgFloat32(0.0f);
			//			tmp2 = zero;
			accNormSimd = zero;
			akSimd = simd_permut_v (akSimd, akSimd, PURMUT_MASK(0, 0, 0, 0));
			//for (j = 0; j < count; j ++) {
			for (dgInt32 j = 0; j < roundCount; j += DG_SIMD_WORD_SIZE) {
				dgInt32 k;
				k = j + first;
				//m_force[k] += ak * deltaForce[j];
				//m_accel[j] -= ak * deltaAccel[j];
				(simd_type&)force[k] = simd_mul_add_v((simd_type&)force[k], akSimd, (simd_type&)deltaForce[j]);
				(simd_type&)accel[j] = simd_mul_sub_v((simd_type&)accel[j], akSimd, (simd_type&)deltaAccel[j]);

				//accNorm = GetMax (dgAbsf (m_accel[j] * activeRow[j]), accNorm);
				accNormSimd = simd_max_v(accNormSimd, simd_and_v(simd_mul_v ((simd_type&)accel[j], (simd_type&)activeRow[j]), signMask));
				//_ASSERTE (dgCheckFloat(m_force[k]));
				//_ASSERTE (dgCheckFloat(m_accel[j]));
			}
			accNormSimd = simd_max_v (accNormSimd, simd_move_hl_v(accNormSimd, accNormSimd));
			accNormSimd = simd_max_s (accNormSimd, simd_permut_v (accNormSimd, accNormSimd, PURMUT_MASK (0, 0, 0, 1)));
			simd_store_s (accNormSimd, &accNorm);
			if (accNorm > maxAccNorm) {

				//akDen = akNum;
				//akNum = dgFloat32(0.0f);
				akDenSimd = simd_set1(akNum);
				akNumSimd = zero;
				//for (j = 0; j < count; j ++) {
				for (dgInt32 j = 0; j < roundCount; j += DG_SIMD_WORD_SIZE) {
					dgInt32 k;
					k = j + first;
					//deltaAccel[j] = m_accel[j] * m_invDJMinvJt[k] * activeRow[j];
					(simd_type&)deltaAccel[j] = simd_mul_v((simd_type&)accel[j], simd_mul_v((simd_type&)invDJMinvJt[k], (simd_type&)activeRow[j]));
					//akNum += m_accel[j] * deltaAccel[j];
					akNumSimd = simd_mul_add_v(akNumSimd, (simd_type&)accel[j], (simd_type&)deltaAccel[j]);
				}

				akNumSimd = simd_add_v (akNumSimd, simd_move_hl_v(akNumSimd, akNumSimd));
				akNumSimd = simd_add_s (akNumSimd, simd_permut_v (akNumSimd, akNumSimd, PURMUT_MASK (0, 0, 0, 1)));
				simd_store_s (akNumSimd, &akNum);

				//_ASSERTE (akDen > dgFloat32(0.0f));
				//akDen = GetMax (akDen, dgFloat32 (1.0e-17f));
				akDenSimd = simd_max_s (akDenSimd, simd_set1(dgFloat32 (1.0e-17f)));
				//ak = dgFloat32 (akNum / akDen);
				akSimd = simd_div_s(akSimd, akDenSimd);
				akSimd = simd_permut_v (akSimd, akSimd, PURMUT_MASK(0, 0, 0, 0));

				//for (j = 0; j < count; j ++) {
				for (dgInt32 j = 0; j < roundCount; j += DG_SIMD_WORD_SIZE) {
					//deltaForce[j] = deltaAccel[j] + ak * deltaForce[j];
					(simd_type&)deltaForce[j] = simd_mul_add_v((simd_type&)deltaAccel[j], akSimd, (simd_type&)deltaForce[j]);
				}
			}
		}
	}

	//	for (j = 0; j < count; j ++) {
	for (dgInt32 j = 0; j < roundCount; j += DG_SIMD_WORD_SIZE) {
		//forceStep[j] = force[j + first] - forceStep[j];
		(simd_type&)forceStep[j] = simd_sub_v ((simd_type&)force[j + first], (simd_type&)forceStep[j]);
	}

	//	tmp0 = simd_set1(DG_SSOR_FACTOR);
	//	for (j = 0; j < roundCount; j += DG_SIMD_WORD_SIZE) {
	//		tmp1 = simd_mul_add_v((simd_type&)forceStep[j], simd_sub_v ((simd_type&)force[j + first], (simd_type&)forceStep[j]), tmp0);
	//		(simd_type&)force[j + first] = simd_min_v ((simd_type&) highBound[j], simd_max_v(tmp1, (simd_type&)lowBound[j]));
	//		(simd_type&)forceStep[j] = simd_sub_v ((simd_type&)force[j + first], (simd_type&)forceStep[j]);
	//	}

	return retAccel;


#else
	return dgFloat32 (0.0f);
#endif
}



dgFloat32 dgJacobianMemory::CalculateJointForces (dgInt32 joint, dgFloat32* forceStep, dgFloat32 maxAccNorm) const
{
	dgInt32 m0;
	dgInt32 m1;
	dgInt32 first;
	dgInt32 count;
	dgInt32 maxPasses;
	dgInt32 clampedForceIndex;
	dgFloat32 ak;
	dgFloat32 val;
	dgFloat32 akNum;
	dgFloat32 akDen;
	dgFloat32 accNorm;
	dgFloat32 retAccel;
	dgFloat32 clampedForceIndexValue;
	dgJacobian y0;
	dgJacobian y1;
	dgFloat32 deltaAccel[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 deltaForce[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 activeRow[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 lowBound[DG_CONSTRAINT_MAX_ROWS];
	dgFloat32 highBound[DG_CONSTRAINT_MAX_ROWS];

	dgFloat32* const accel = m_accel;
	dgFloat32* const force = m_force;
	const dgJacobianPair* const Jt = m_Jt;
	const dgJacobianPair* const JMinv = m_JMinv;
	const dgFloat32* const diagDamp = m_diagDamp;
	const dgFloat32* const invDJMinvJt = m_invDJMinvJt;
	const dgInt32* const normalForceIndex = m_normalForceIndex;
	const dgJointInfo* const constraintArray = m_constraintArray;
	const dgFloat32* const lowerFriction = m_lowerBoundFrictionCoefficent;
	const dgFloat32* const upperFriction = m_upperBoundFrictionCoefficent;
	//dgBilateralBounds* const bilateralForceBounds = m_bilateralForceBounds;
	//const dgJacobianIndex* const jacobianIndexArray = m_jacobianIndexArray;


	first = constraintArray[joint].m_autoPairstart;
	count = constraintArray[joint].m_autoPaircount;

	akNum = dgFloat32 (0.0f);
	accNorm = dgFloat32(0.0f);
	maxPasses = count;

	for (dgInt32 j = 0; j < count; j ++) {
		dgInt32 i;
		dgInt32 k;
		i = first + j; 
		//k = bilateralForceBounds[i].m_normalIndex;
		k = normalForceIndex[i];

		//		val = (k >= 0) ? force[k] : dgFloat32 (1.0f);
		_ASSERTE (((k <0) && (force[k] == dgFloat32 (1.0f))) || ((k >= 0) && (force[k] >= dgFloat32 (0.0f))));
		val = force[k];
		lowBound[j] = val * lowerFriction[i];
		highBound[j] = val * upperFriction[i];

		activeRow[j] = dgFloat32 (1.0f);
		forceStep[j] = force[i];
		if (force[i] < lowBound[j]) {
			maxPasses --;
			force[i] = lowBound[j];
			activeRow[j] = dgFloat32 (0.0f);
		} else if (force[i] > highBound[j]) {
			maxPasses --;
			force[i] = highBound[j];
			activeRow[j] = dgFloat32 (0.0f);
		}

		deltaForce[j] = accel[j] * invDJMinvJt[i] * activeRow[j];
		akNum += accel[j] * deltaForce[j];
		accNorm = GetMax (dgAbsf (accel[j] * activeRow[j]), accNorm);
	}

	retAccel = accNorm;
	m0 = constraintArray[joint].m_m0;
	m1 = constraintArray[joint].m_m1;
	clampedForceIndexValue = dgFloat32(0.0f);

	dgVector zero (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	for (dgInt32 i = 0; (i < maxPasses) && (accNorm >  maxAccNorm); i ++) {
		y0.m_linear = zero;
		y0.m_angular = zero;
		y1.m_linear = zero;
		y1.m_angular = zero;
		for (dgInt32 j = 0; j < count; j ++) {
			dgInt32 k;
			k = j + first;
			ak = deltaForce[j]; 

			y0.m_linear += Jt[k].m_jacobian_IM0.m_linear.Scale (ak);
			y0.m_angular += Jt[k].m_jacobian_IM0.m_angular.Scale (ak);
			y1.m_linear += Jt[k].m_jacobian_IM1.m_linear.Scale (ak);
			y1.m_angular += Jt[k].m_jacobian_IM1.m_angular.Scale (ak);
		}

		akDen = dgFloat32 (0.0f);
		for (dgInt32 j = 0; j < count; j ++) {
			dgInt32 k;
			k = j + first;

			dgVector acc (JMinv[k].m_jacobian_IM0.m_linear.CompProduct(y0.m_linear));
			acc += JMinv[k].m_jacobian_IM0.m_angular.CompProduct(y0.m_angular);
			acc += JMinv[k].m_jacobian_IM1.m_linear.CompProduct(y1.m_linear);
			acc += JMinv[k].m_jacobian_IM1.m_angular.CompProduct(y1.m_angular);
			deltaAccel[j] = acc.m_x + acc.m_y + acc.m_z + deltaForce[j] * diagDamp[k];
			akDen += deltaAccel[j] * deltaForce[j];
		}


		_ASSERTE (akDen > dgFloat32 (0.0f));
		akDen = GetMax (akDen, dgFloat32(1.0e-16f));
		_ASSERTE (dgAbsf (akDen) >= dgFloat32(1.0e-16f));
		ak = akNum / akDen;

		clampedForceIndex = -1;
		for (dgInt32 j = 0; j < count; j ++) {
			if (activeRow[j]) {
				dgInt32 k;
				k = j + first;
				if (deltaForce[j] < dgFloat32 (-1.0e-16f)) {
					val = force[k] + ak * deltaForce[j];
					if (val < lowBound[j]) {
						ak = GetMax ((lowBound[j] - force[k]) / deltaForce[j], dgFloat32 (0.0f));
						clampedForceIndex = j;
						clampedForceIndexValue = lowBound[j];
						if (ak < dgFloat32 (1.0e-8f)) {
							ak = dgFloat32 (0.0f);
							break;
						}
					}
				} else if (deltaForce[j] > dgFloat32 (1.0e-16f)) {
					val = force[k] + ak * deltaForce[j];
					if (val > highBound[j]) {
						ak = GetMax ((highBound[j] - force[k]) / deltaForce[j], dgFloat32 (0.0f));
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

		//		if (ak == dgFloat32 (0.0f)) {
		if (ak == dgFloat32 (0.0f) && (clampedForceIndex != -1)) {

			_ASSERTE (clampedForceIndex !=-1);
			akNum = dgFloat32 (0.0f);
			accNorm = dgFloat32(0.0f);

			activeRow[clampedForceIndex] = dgFloat32 (0.0f);
			deltaForce[clampedForceIndex] = dgFloat32 (0.0f);
			force[clampedForceIndex + first] = clampedForceIndexValue;
			for (dgInt32 j = 0; j < count; j ++) {
				if (activeRow[j]) {
					dgInt32 k;
					k = j + first;
					val = lowBound[j] - force[k];
					if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (accel[j] < dgFloat32 (0.0f))) {
						force[k] = lowBound[j];
						activeRow[j] = dgFloat32 (0.0f);
						deltaForce[j] = dgFloat32 (0.0f); 

					} else {
						val = highBound[j] - force[k];
						if ((dgAbsf (val) < dgFloat32 (1.0e-5f)) && (accel[j] > dgFloat32 (0.0f))) {
							force[k] = highBound[j];
							activeRow[j] = dgFloat32 (0.0f);
							deltaForce[j] = dgFloat32 (0.0f); 
						} else {
							_ASSERTE (activeRow[j] > dgFloat32 (0.0f));
							deltaForce[j] = accel[j] * invDJMinvJt[k];
							akNum += accel[j] * deltaForce[j];
							accNorm = GetMax (dgAbsf (accel[j]), accNorm);
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
				dgInt32 k;
				k = j + first;
				force[k] += ak * deltaForce[j];
				accel[j] -= ak * deltaAccel[j];
				accNorm = GetMax (dgAbsf (accel[j] * activeRow[j]), accNorm);
				_ASSERTE (dgCheckFloat(force[k]));
				_ASSERTE (dgCheckFloat(accel[j]));

				deltaForce[j] = accel[j] * invDJMinvJt[k] * activeRow[j];
				akNum += deltaForce[j] * accel[j];
			}
			force[clampedForceIndex + first] = clampedForceIndexValue;

			i = -1;
			maxPasses = GetMax (maxPasses - 1, 1); 

		} else {
			accNorm = dgFloat32(0.0f);
			for (dgInt32 j = 0; j < count; j ++) {
				dgInt32 k;
				k = j + first;
				force[k] += ak * deltaForce[j];
				accel[j] -= ak * deltaAccel[j];
				accNorm = GetMax (dgAbsf (accel[j] * activeRow[j]), accNorm);
				_ASSERTE (dgCheckFloat(force[k]));
				_ASSERTE (dgCheckFloat(accel[j]));
			}

			if (accNorm > maxAccNorm) {

				akDen = akNum;
				akNum = dgFloat32(0.0f);
				for (dgInt32 j = 0; j < count; j ++) {
					dgInt32 k;
					k = j + first;
					deltaAccel[j] = accel[j] * invDJMinvJt[k] * activeRow[j];
					akNum += accel[j] * deltaAccel[j];
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
		forceStep[j] = force[j + first] - forceStep[j];
	}
	return retAccel;
}











////////////////////////////////////////////////////////////////////////////////////////
//
//  Parallel solver
//
////////////////////////////////////////////////////////////////////////////////////////


void dgParallelSolverBodyInertia::ThreadExecute()
{
	//	dgUnsigned32 ticks;
	//	ticks = m_world->m_getPerformanceCount();

	if (m_useSimd) {
		for (dgInt32 j = 0; j < m_count; j ++) {
			dgBody* body;
			body = m_bodyArray[j].m_body;
			_ASSERTE (body->m_invMass.m_w > dgFloat32 (0.0f));
			body->AddDamingAcceleration();
			body->CalcInvInertiaMatrixSimd();
		}
	} else {
		for (dgInt32 j = 0; j < m_count; j ++) {
			dgBody* body;
			body = m_bodyArray[j].m_body;
			_ASSERTE (body->m_invMass.m_w > dgFloat32 (0.0f));
			body->AddDamingAcceleration();
			body->CalcInvInertiaMatrix ();
		}
	}

	//	m_world->m_perfomanceCounters[m_threadIndex][m_dynamicsSolveSpanningTreeTicks] += (m_world->m_getPerformanceCount() - ticks);
}


void dgParallelSolverBuildJacobianRows::ThreadExecute()
{
	//	dgUnsigned32 ticks;
	//	ticks = m_world->m_getPerformanceCount();

	dgFloat32* const force = m_force;
	const dgJacobianPair* const Jt = m_Jt;
	dgJacobianPair* const JMinv = m_JMinv;
	dgFloat32* const diagDamp = m_diagDamp;
	dgFloat32* const extAccel = m_deltaAccel;
	dgFloat32* const invDJMinvJt = m_invDJMinvJt;
	dgFloat32* const coordenateAccel = m_coordenateAccel;
	dgFloat32** const jointForceFeeback = m_jointFeebackForce;
	//	dgInt32* const accelIsMotor = solverMemory.m_accelIsMotor;

	dgBodyInfo* const bodyArray = m_bodyArray;
	dgJointInfo* const constraintArray = m_constraintArray;

	if (m_useSimd) {
#ifdef DG_BUILD_SIMD_CODE
		simd_type zero;
		zero = simd_set1 (dgFloat32 (0.0f));
		for (dgInt32 k = 0; k < m_count; k ++) {
			dgInt32 m0;
			dgInt32 m1;
			dgInt32 index;
			dgInt32 count;
			dgFloat32 diag;
			dgFloat32 stiffness;
			simd_type tmp0;
			simd_type tmp1;
			simd_type invMass0;
			simd_type invMass1;
			simd_type tmpDiag;
			simd_type tmpAccel;
			dgBody* body0;
			dgBody* body1;

			index = constraintArray[k].m_autoPairstart;
			count = constraintArray[k].m_autoPaircount;
			m0 = constraintArray[k].m_m0;
			m1 = constraintArray[k].m_m1;
			_ASSERTE (m0 >= 0);
			_ASSERTE (m0 < m_bodyCount);
			_ASSERTE (m1 >= 0);
			_ASSERTE (m1 < m_bodyCount);

			body0 = bodyArray[m0].m_body;
			//invMass0 = body0->m_invMass[3];
			invMass0 = simd_set1 (body0->m_invMass[3]);
			_ASSERTE ((dgUnsigned64 (&body0->m_invWorldInertiaMatrix) & 0x0f) == 0);
			const dgMatrix& invInertia0 = body0->m_invWorldInertiaMatrix;

			body1 = bodyArray[m1].m_body;
			//invMass1 = body1->m_invMass[3];
			invMass1 = simd_set1 (body1->m_invMass[3]);
			_ASSERTE ((dgUnsigned64 (&body1->m_invWorldInertiaMatrix) & 0x0f) == 0);
			const dgMatrix& invInertia1 = body1->m_invWorldInertiaMatrix;
			for (dgInt32 i = 0; i < count; i ++) {
				//JMinv[index].m_jacobian_IM0.m_linear = Jt[index].m_jacobian_IM0.m_linear.Scale (invMass0);
				//JMinv[index].m_jacobian_IM0.m_angular = invInertia0.UnrotateVector (Jt[index].m_jacobian_IM0.m_angular);
				//dgVector tmpDiag (JMinv[index].m_jacobian_IM0.m_linear.CompProduct(Jt[index].m_jacobian_IM0.m_linear));
				//tmpDiag += JMinv[index].m_jacobian_IM0.m_angular.CompProduct(Jt[index].m_jacobian_IM0.m_angular);
				//dgVector tmpAccel (JMinv[index].m_jacobian_IM0.m_linear.CompProduct(body0->m_accel));
				//tmpAccel += JMinv[index].m_jacobian_IM0.m_angular.CompProduct(body0->m_alpha);

				((simd_type&)JMinv[index].m_jacobian_IM0.m_linear) = simd_mul_v ((simd_type&)Jt[index].m_jacobian_IM0.m_linear, invMass0);
				tmp0 = (simd_type&)Jt[index].m_jacobian_IM0.m_angular;
				tmp1 = simd_mul_v ((simd_type&)invInertia0.m_front, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 0, 0, 0)));
				tmp1 = simd_mul_add_v (tmp1, (simd_type&)invInertia0.m_up, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 1, 1, 1)));
				((simd_type&)JMinv[index].m_jacobian_IM0.m_angular) = simd_mul_add_v (tmp1, (simd_type&)invInertia0.m_right, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 2, 2, 2)));
				tmpDiag = simd_mul_v ((simd_type&)JMinv[index].m_jacobian_IM0.m_linear, (simd_type&)Jt[index].m_jacobian_IM0.m_linear);
				tmpDiag = simd_mul_add_v (tmpDiag, (simd_type&)JMinv[index].m_jacobian_IM0.m_angular, (simd_type&)Jt[index].m_jacobian_IM0.m_angular);
				tmpAccel = simd_mul_v ((simd_type&)JMinv[index].m_jacobian_IM0.m_linear, (simd_type&)body0->m_accel);
				tmpAccel = simd_mul_add_v (tmpAccel, (simd_type&)JMinv[index].m_jacobian_IM0.m_angular, (simd_type&)body0->m_alpha);

				//JMinv[index].m_jacobian_IM1.m_linear = Jt[index].m_jacobian_IM1.m_linear.Scale (invMass1);
				//JMinv[index].m_jacobian_IM1.m_angular = invInertia1.UnrotateVector (Jt[index].m_jacobian_IM1.m_angular);
				//tmpDiag += JMinv[index].m_jacobian_IM1.m_linear.CompProduct(Jt[index].m_jacobian_IM1.m_linear);
				//tmpDiag += JMinv[index].m_jacobian_IM1.m_angular.CompProduct(Jt[index].m_jacobian_IM1.m_angular);
				//tmpAccel += JMinv[index].m_jacobian_IM1.m_linear.CompProduct(body1->m_accel);
				//tmpAccel += JMinv[index].m_jacobian_IM1.m_angular.CompProduct(body1->m_alpha);

				((simd_type&)JMinv[index].m_jacobian_IM1.m_linear) = simd_mul_v ((simd_type&)Jt[index].m_jacobian_IM1.m_linear, invMass1);
				tmp0 = (simd_type&)Jt[index].m_jacobian_IM1.m_angular;
				tmp1 = simd_mul_v ((simd_type&)invInertia1.m_front, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 0, 0, 0)));
				tmp1 = simd_mul_add_v (tmp1, (simd_type&)invInertia1.m_up, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 1, 1, 1)));
				((simd_type&)JMinv[index].m_jacobian_IM1.m_angular) = simd_mul_add_v (tmp1, (simd_type&)invInertia1.m_right, simd_permut_v (tmp0, tmp0, PURMUT_MASK(3, 2, 2, 2)));
				tmpDiag = simd_mul_add_v (tmpDiag, (simd_type&)JMinv[index].m_jacobian_IM1.m_linear, (simd_type&)Jt[index].m_jacobian_IM1.m_linear);
				tmpDiag = simd_mul_add_v (tmpDiag, (simd_type&)JMinv[index].m_jacobian_IM1.m_angular, (simd_type&)Jt[index].m_jacobian_IM1.m_angular);
				tmpAccel = simd_mul_add_v (tmpAccel, (simd_type&)JMinv[index].m_jacobian_IM1.m_linear, (simd_type&)body1->m_accel);
				tmpAccel = simd_mul_add_v (tmpAccel, (simd_type&)JMinv[index].m_jacobian_IM1.m_angular, (simd_type&)body1->m_alpha);

				//coordenateAccel[index] -= (tmpAccel.m_x + tmpAccel.m_y + tmpAccel.m_z);
				tmpAccel = simd_add_v (tmpAccel, simd_move_hl_v(tmpAccel, tmpAccel));
				tmpAccel = simd_sub_s (zero, simd_add_s(tmpAccel, simd_permut_v (tmpAccel, tmpAccel, PURMUT_MASK(3, 3, 3, 1))));

				simd_store_s (tmpAccel, &extAccel[index]);
				simd_store_s (simd_add_s(simd_load_s(coordenateAccel[index]), tmpAccel), &coordenateAccel[index]);

				//force[index] = bilateralForceBounds[index].m_jointForce[0];
				force[index] = jointForceFeeback[index][0];

				_ASSERTE (diagDamp[index] >= dgFloat32(0.1f));
				_ASSERTE (diagDamp[index] <= dgFloat32(100.0f));
				stiffness = DG_PSD_DAMP_TOL * diagDamp[index];

				//diag = (tmpDiag.m_x + tmpDiag.m_y + tmpDiag.m_z);
				tmpDiag = simd_add_v (tmpDiag, simd_move_hl_v(tmpDiag, tmpDiag));
				simd_store_s (simd_add_s(tmpDiag, simd_permut_v (tmpDiag, tmpDiag, PURMUT_MASK(3, 3, 3, 1))), &diag);
				_ASSERTE (diag > dgFloat32 (0.0f));
				diagDamp[index] = diag * stiffness;

				diag *= (dgFloat32(1.0f) + stiffness);
				invDJMinvJt[index] = dgFloat32(1.0f) / diag;

				index ++;
			}
		}
#endif
	} else {
		for (dgInt32 k = 0; k < m_count; k ++) {
			dgInt32 m0;
			dgInt32 m1;
			dgInt32 index;
			dgInt32 count;
			dgFloat32 invMass0;
			dgFloat32 invMass1;
			dgBody* body0;
			dgBody* body1;

			index = constraintArray[k].m_autoPairstart;
			count = constraintArray[k].m_autoPaircount;
			m0 = constraintArray[k].m_m0;
			m1 = constraintArray[k].m_m1;

			_ASSERTE (m0 >= 0);
			_ASSERTE (m0 < m_bodyCount);
			body0 = bodyArray[m0].m_body;
			invMass0 = body0->m_invMass[3];
			const dgMatrix& invInertia0 = body0->m_invWorldInertiaMatrix;

			_ASSERTE (m1 >= 0);
			_ASSERTE (m1 < m_bodyCount);
			body1 = bodyArray[m1].m_body;
			invMass1 = body1->m_invMass[3];
			const dgMatrix& invInertia1 = body1->m_invWorldInertiaMatrix;

			for (dgInt32 i = 0; i < count; i ++) {
				dgFloat32 diag;
				dgFloat32 stiffness;
				dgFloat32 extenalAcceleration;

				JMinv[index].m_jacobian_IM0.m_linear = Jt[index].m_jacobian_IM0.m_linear.Scale (invMass0);
				JMinv[index].m_jacobian_IM0.m_angular = invInertia0.UnrotateVector (Jt[index].m_jacobian_IM0.m_angular);
				dgVector tmpDiag (JMinv[index].m_jacobian_IM0.m_linear.CompProduct(Jt[index].m_jacobian_IM0.m_linear));
				tmpDiag += JMinv[index].m_jacobian_IM0.m_angular.CompProduct(Jt[index].m_jacobian_IM0.m_angular);
				dgVector tmpAccel (JMinv[index].m_jacobian_IM0.m_linear.CompProduct(body0->m_accel));
				tmpAccel += JMinv[index].m_jacobian_IM0.m_angular.CompProduct(body0->m_alpha);

				JMinv[index].m_jacobian_IM1.m_linear = Jt[index].m_jacobian_IM1.m_linear.Scale (invMass1);
				JMinv[index].m_jacobian_IM1.m_angular = invInertia1.UnrotateVector (Jt[index].m_jacobian_IM1.m_angular);
				tmpDiag += JMinv[index].m_jacobian_IM1.m_linear.CompProduct(Jt[index].m_jacobian_IM1.m_linear);
				tmpDiag += JMinv[index].m_jacobian_IM1.m_angular.CompProduct(Jt[index].m_jacobian_IM1.m_angular);
				tmpAccel += JMinv[index].m_jacobian_IM1.m_linear.CompProduct(body1->m_accel);
				tmpAccel += JMinv[index].m_jacobian_IM1.m_angular.CompProduct(body1->m_alpha);

				extenalAcceleration = -(tmpAccel.m_x + tmpAccel.m_y + tmpAccel.m_z);
				extAccel[index] = extenalAcceleration;
				coordenateAccel[index] += extenalAcceleration;
				force[index] = jointForceFeeback[index][0];

				_ASSERTE (diagDamp[index] >= dgFloat32(0.1f));
				_ASSERTE (diagDamp[index] <= dgFloat32(100.0f));
				stiffness = DG_PSD_DAMP_TOL * diagDamp[index];

				diag = (tmpDiag.m_x + tmpDiag.m_y + tmpDiag.m_z);
				_ASSERTE (diag > dgFloat32 (0.0f));
				diagDamp[index] = diag * stiffness;

				diag *= (dgFloat32(1.0f) + stiffness);
				//solverMemory.m_diagJMinvJt[index] = diag;
				invDJMinvJt[index] = dgFloat32(1.0f) / diag;
				index ++;
			}
		}
	}

	//	m_world->m_perfomanceCounters[m_threadIndex][m_dynamicsSolveSpanningTreeTicks] += (m_world->m_getPerformanceCount() - ticks);
}


void dgParallelSolverBuildJacobianMatrix::ThreadExecute()
{
	dgInt32 j;
	dgInt32 count;
	dgInt32 rowCount;
	dgInt32 jointSolved;
	//	dgUnsigned32 ticks;
	dgContraintDescritor constraintParams;
	dgJointInfo* const constraintArray = m_constraintArray;

	//	ticks = m_world->m_getPerformanceCount();

	constraintParams.m_world = m_world; 
	constraintParams.m_threadIndex = m_threadIndex;
	constraintParams.m_timestep = m_timestep;
	constraintParams.m_invTimestep = m_invTimestep;


	dgJacobianMemory& solverMemory = *m_solverMemory;	

	count = m_count;
	jointSolved = m_jointSolved;
	for (j = 0; j < count; j ++) {
		dgConstraint* constraint;
		constraint = constraintArray[j].m_joint;

		if (constraint->m_isUnilateral ^ m_bitMode) {

			dgInt32 m0;
			dgInt32 m1;
			dgInt32 dof;
			dof = constraint->m_maxDOF;
			jointSolved ++;

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


			dof = constraint->JacobianDerivative (constraintParams); 

			constraint->m_body0->m_inCallback = false;
			constraint->m_body1->m_inCallback = false;

			m0 = (constraint->m_body0->m_invMass.m_w != dgFloat32(0.0f)) ? constraint->m_body0->m_index: 0;
			m1 = (constraint->m_body1->m_invMass.m_w != dgFloat32(0.0f)) ? constraint->m_body1->m_index: 0;

			m_world->dgGetUserLock();
			rowCount = m_rowsCount[0];
			m_rowsCount[0] = rowCount + ((dof & (DG_SIMD_WORD_SIZE - 1)) ? ((dof & (-DG_SIMD_WORD_SIZE)) + DG_SIMD_WORD_SIZE) : dof);
			m_world->dgReleasedUserLock();

			constraintArray[j].m_autoPairstart = rowCount;
			constraintArray[j].m_autoPaircount = dgInt32 (dof);
			constraintArray[j].m_autoPairActiveCount = dgInt32 (dof);
			constraintArray[j].m_m0 = m0;
			constraintArray[j].m_m1 = m1;

			for (dgInt32 i = 0; i < dof; i ++) {
				_ASSERTE (constraintParams.m_forceBounds[i].m_jointForce);
				solverMemory.m_Jt[rowCount] = constraintParams.m_jacobian[i]; 

				_ASSERTE (constraintParams.m_jointStiffness[i] >= dgFloat32(0.1f));
				_ASSERTE (constraintParams.m_jointStiffness[i] <= dgFloat32(100.0f));
				solverMemory.m_diagDamp[rowCount] = constraintParams.m_jointStiffness[i];
				solverMemory.m_coordenateAccel[rowCount] = constraintParams.m_jointAccel[i];
				solverMemory.m_accelIsMotor[rowCount] = constraintParams.m_isMotor[i];
				solverMemory.m_restitution[rowCount] = constraintParams.m_restitution[i];
				solverMemory.m_penetration[rowCount] = constraintParams.m_penetration[i];
				solverMemory.m_penetrationStiffness[rowCount] = constraintParams.m_penetrationStiffness[i];

				solverMemory.m_lowerBoundFrictionCoefficent[rowCount] = constraintParams.m_forceBounds[i].m_low;
				solverMemory.m_upperBoundFrictionCoefficent[rowCount] = constraintParams.m_forceBounds[i].m_upper;
				solverMemory.m_jointFeebackForce[rowCount] = constraintParams.m_forceBounds[i].m_jointForce;
				solverMemory.m_normalForceIndex[rowCount] = constraintParams.m_forceBounds[i].m_normalIndex + ((constraintParams.m_forceBounds[i].m_normalIndex >=0) ? (rowCount - i) : 0);

				rowCount ++;
			}

#ifdef _DEBUG
			for (dgInt32 i = 0; i < ((rowCount + 3) & 0xfffc) - rowCount ; i ++) {
				solverMemory.m_diagDamp[rowCount + i] = dgFloat32 (0.0f);
				solverMemory.m_coordenateAccel[rowCount + i] = dgFloat32 (0.0f);
				solverMemory.m_restitution[rowCount + i] = dgFloat32 (0.0f);
				solverMemory.m_penetration[rowCount + i] = dgFloat32 (0.0f);
				solverMemory.m_penetrationStiffness[rowCount + i] = dgFloat32 (0.0f);

				solverMemory.m_lowerBoundFrictionCoefficent[rowCount + i] = dgFloat32 (0.0f);
				solverMemory.m_upperBoundFrictionCoefficent[rowCount + i] = dgFloat32 (0.0f);
				solverMemory.m_jointFeebackForce[rowCount + i] = 0;
				solverMemory.m_normalForceIndex[rowCount + i] = 0;
			}
#endif

			//			rowCount = (rowCount & (DG_SIMD_WORD_SIZE - 1)) ? ((rowCount & (-DG_SIMD_WORD_SIZE)) + DG_SIMD_WORD_SIZE) : rowCount;
			//			_ASSERTE ((rowCount & (DG_SIMD_WORD_SIZE - 1)) == 0);
		}
	}

	m_jointSolved = jointSolved;

	//	m_world->m_perfomanceCounters[m_threadIndex][m_dynamicsSolveSpanningTreeTicks] += (m_world->m_getPerformanceCount() - ticks);
}



dgInt32 dgWorldDynamicUpdate::GetJacobianDerivativesParallel (const dgIsland& island, bool bitMode, dgInt32 rowCount, dgFloat32 timestep)
{
	dgInt32 acc;
	dgInt32 jointCount;
	dgInt32 threadCounts;
	dgInt32 chunkSizes[DG_MAX_THREADS_HIVE_COUNT];

	dgJointInfo* constraintArray = &m_constraintArray[island.m_jointStart];

	acc = 0;
	threadCounts = m_world->m_numberOfTheads;
	jointCount = island.m_jointCount;
	m_world->m_threadsManager.CalculateChunkSizes(jointCount, chunkSizes);
	for (dgInt32 threadIndex = 0; threadIndex < threadCounts; threadIndex ++) {
		m_parallelSolverBuildJacobianMatrix[threadIndex].m_bitMode = bitMode;
		m_parallelSolverBuildJacobianMatrix[threadIndex].m_world = m_world;
		m_parallelSolverBuildJacobianMatrix[threadIndex].m_jointSolved = 0;
		m_parallelSolverBuildJacobianMatrix[threadIndex].m_count = chunkSizes[threadIndex];
		m_parallelSolverBuildJacobianMatrix[threadIndex].m_timestep = timestep;
		m_parallelSolverBuildJacobianMatrix[threadIndex].m_invTimestep = dgFloat32 (1.0f / timestep);
		m_parallelSolverBuildJacobianMatrix[threadIndex].m_threadIndex = threadIndex;
		m_parallelSolverBuildJacobianMatrix[threadIndex].m_rowsCount = &rowCount;
		m_parallelSolverBuildJacobianMatrix[threadIndex].m_constraintArray = &constraintArray[acc];
		m_parallelSolverBuildJacobianMatrix[threadIndex].m_solverMemory = &m_solverMemory[0];
		m_world->m_threadsManager.SubmitJob(&m_parallelSolverBuildJacobianMatrix[threadIndex]);
		acc += chunkSizes[threadIndex];
	}
	m_world->m_threadsManager.SynchronizationBarrier ();

	return rowCount;
}



void dgWorldDynamicUpdate::BuildJacobianMatrixParallel (const dgIsland& island, dgFloat32 timestep, dgInt32 archModel)
{
	dgInt32 threads;
	dgInt32 rowCount;
	dgInt32 bodyCount;
	dgInt32 jointCount;

	dgVector zeroVector (dgFloat32(0.0f), dgFloat32(0.0f), dgFloat32(0.0f), dgFloat32(0.0f));

	_ASSERTE (island.m_bodyCount >= 2);
	//_ASSERTE (island.m_jointCount >= 1);

	threads = m_world->m_numberOfTheads;
	dgJacobianMemory& solverMemory = m_solverMemory[0];

	bodyCount = island.m_bodyCount;
	dgBodyInfo* const bodyArray = &m_bodyArray[island.m_bodyStart];

	bodyArray[0].m_body = m_world->GetSentinelBody();
	_ASSERTE ((bodyArray[0].m_body->m_accel % bodyArray[0].m_body->m_accel) == dgFloat32 (0.0f));
	_ASSERTE ((bodyArray[0].m_body->m_alpha % bodyArray[0].m_body->m_alpha) == dgFloat32 (0.0f));

	dgInt32 acc;
	acc = 0;
	dgInt32 chunkSizes[DG_MAX_THREADS_HIVE_COUNT];
	m_world->m_threadsManager.CalculateChunkSizes(bodyCount - 1, chunkSizes);
	for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
		m_parallelBodyInertiaMatrix[threadIndex].m_useSimd = archModel;
		m_parallelBodyInertiaMatrix[threadIndex].m_bodyArray = &bodyArray[acc + 1];
		m_parallelBodyInertiaMatrix[threadIndex].m_threadIndex = threadIndex;
		m_parallelBodyInertiaMatrix[threadIndex].m_world = m_world;
		m_parallelBodyInertiaMatrix[threadIndex].m_count = chunkSizes[threadIndex];
		m_world->m_threadsManager.SubmitJob(&m_parallelBodyInertiaMatrix[threadIndex]);
		acc += chunkSizes[threadIndex];
	}
	m_world->m_threadsManager.SynchronizationBarrier ();


	while (bodyCount > solverMemory.m_maxBodiesCount) {
		m_world->dgGetUserLock();
		ReallocIntenalForcesMemory (bodyCount, 0);
		m_world->dgReleasedUserLock();
	}

	jointCount = island.m_jointCount;
	dgJointInfo* const constraintArray = &m_constraintArray[island.m_jointStart];
	solverMemory.m_constraintArray = constraintArray;


	dgInt32 maxRowCount = 0;
	for (dgInt32 j = 0; j < jointCount; j ++) {
		dgInt32 dof;
		dgConstraint* constraint;
		constraint = constraintArray[j].m_joint;;
		dof = (constraint->m_maxDOF & (DG_SIMD_WORD_SIZE - 1)) ? ((constraint->m_maxDOF & (-DG_SIMD_WORD_SIZE)) + DG_SIMD_WORD_SIZE) : constraint->m_maxDOF;
		maxRowCount += dof;
	}

	while (maxRowCount > solverMemory.m_maxJacobiansCount) {
		m_world->dgGetUserLock();
		ReallocJacobiansMemory (solverMemory.m_maxJacobiansCount * 2, 0);
		m_world->dgReleasedUserLock();
	}

	rowCount = 0;
	if (island.m_hasUnilateralJoints) {
		rowCount = GetJacobianDerivativesParallel (island, false, rowCount, timestep);
	}
	rowCount = GetJacobianDerivativesParallel (island, true, rowCount, timestep);

	solverMemory.m_rowCount = rowCount;
	solverMemory.m_bodyCount = bodyCount;
	solverMemory.m_bodyArray = bodyArray;
	solverMemory.m_jointCount = jointCount;
	solverMemory.m_timeStep = timestep;
	solverMemory.m_invTimeStep = dgFloat32 (1.0f) / solverMemory.m_timeStep;

	acc = 0;
	m_world->m_threadsManager.CalculateChunkSizes(jointCount, chunkSizes);
	for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
		m_parallelSolverBuildJacobianRows[threadIndex].m_useSimd = archModel;
		m_parallelSolverBuildJacobianRows[threadIndex].m_bodyCount = bodyCount;
		m_parallelSolverBuildJacobianRows[threadIndex].m_bodyArray = bodyArray;
		m_parallelSolverBuildJacobianRows[threadIndex].m_threadIndex = threadIndex;
		m_parallelSolverBuildJacobianRows[threadIndex].m_world = m_world;
		m_parallelSolverBuildJacobianRows[threadIndex].m_count = chunkSizes[threadIndex];
		m_parallelSolverBuildJacobianRows[threadIndex].m_force = solverMemory.m_force;
		m_parallelSolverBuildJacobianRows[threadIndex].m_Jt = solverMemory.m_Jt;
		m_parallelSolverBuildJacobianRows[threadIndex].m_JMinv = solverMemory.m_JMinv;
		m_parallelSolverBuildJacobianRows[threadIndex].m_diagDamp = solverMemory.m_diagDamp;
		m_parallelSolverBuildJacobianRows[threadIndex].m_deltaAccel = solverMemory.m_deltaAccel;
		m_parallelSolverBuildJacobianRows[threadIndex].m_invDJMinvJt = solverMemory.m_invDJMinvJt;
		m_parallelSolverBuildJacobianRows[threadIndex].m_coordenateAccel = solverMemory.m_coordenateAccel;
		m_parallelSolverBuildJacobianRows[threadIndex].m_jointFeebackForce = solverMemory.m_jointFeebackForce;
		m_parallelSolverBuildJacobianRows[threadIndex].m_constraintArray = &constraintArray[acc];
		m_world->m_threadsManager.SubmitJob(&m_parallelSolverBuildJacobianRows[threadIndex]);
		acc += chunkSizes[threadIndex];
	}
	m_world->m_threadsManager.SynchronizationBarrier ();
}






void dgParallelSolverInitInternalForces::ThreadExecute()
{
	//	dgUnsigned32 ticks;
	//	ticks = m_world->m_getPerformanceCount();

	if (m_useSimd) {
#ifdef DG_BUILD_SIMD_CODE
		simd_type zero;
		zero = simd_set1(dgFloat32 (0.0f));

		for (dgInt32 i = 0; i < m_count; i ++) {
			dgInt32 m0;
			dgInt32 m1;
			dgInt32 first;
			dgInt32 count;

			simd_type y0_linear;
			simd_type y0_angular;
			simd_type y1_linear;
			simd_type y1_angular;

			first = m_constraintArray[i].m_autoPairstart;
			count = m_constraintArray[i].m_autoPairActiveCount;
			m0 = m_constraintArray[i].m_m0;
			m1 = m_constraintArray[i].m_m1;
			//dgJacobian y0 (internalForces[k0]);
			//dgJacobian y1 (internalForces[k1]);
			y0_linear = zero;
			y0_angular = zero;
			y1_linear = zero;
			y1_angular = zero;
			for (dgInt32 j = 0; j < count; j ++) {
				dgInt32 index;
				simd_type tmp0;
				index = j + first;
				//val = force[index]; 
				tmp0 = simd_set1(m_force[index]);
				//y0.m_linear += Jt[index].m_jacobian_IM0.m_linear.Scale (val);
				//y0.m_angular += Jt[index].m_jacobian_IM0.m_angular.Scale (val);
				//y1.m_linear += Jt[index].m_jacobian_IM1.m_linear.Scale (val);
				//y1.m_angular += Jt[index].m_jacobian_IM1.m_angular.Scale (val);
				y0_linear  = simd_mul_add_v (y0_linear, (simd_type&) m_Jt[index].m_jacobian_IM0.m_linear, tmp0);
				y0_angular = simd_mul_add_v (y0_angular,(simd_type&) m_Jt[index].m_jacobian_IM0.m_angular, tmp0);
				y1_linear  = simd_mul_add_v (y1_linear, (simd_type&) m_Jt[index].m_jacobian_IM1.m_linear, tmp0);
				y1_angular = simd_mul_add_v (y1_angular,(simd_type&) m_Jt[index].m_jacobian_IM1.m_angular, tmp0);
			}
			//internalForces[k0] = y0;
			//internalForces[k1] = y1;
			//			m_world->dgGetUserLock();
			m_world->dgGetIndirectLock(&m_locks[m0]);
			(simd_type&) m_internalForces[m0].m_linear = simd_add_v ((simd_type&) m_internalForces[m0].m_linear, y0_linear);
			(simd_type&) m_internalForces[m0].m_angular = simd_add_v ((simd_type&) m_internalForces[m0].m_angular, y0_angular);
			m_world->dgReleaseIndirectLock(&m_locks[m0]);

			m_world->dgGetIndirectLock(&m_locks[m1]);
			(simd_type&) m_internalForces[m1].m_linear = simd_add_v ((simd_type&) m_internalForces[m1].m_linear, y1_linear);
			(simd_type&) m_internalForces[m1].m_angular = simd_add_v ((simd_type&) m_internalForces[m1].m_angular, y1_angular);
			m_world->dgReleaseIndirectLock(&m_locks[m1]);
			//			m_world->dgReleasedUserLock();
		}
#endif
	} else {
		dgVector zero(dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
		for (dgInt32 i = 0; i < m_count; i ++) {
			dgInt32 m0;
			dgInt32 m1;
			dgInt32 first;
			dgInt32 count;

			dgFloat32 val;
			dgJacobian y0;
			dgJacobian y1;
			first = m_constraintArray[i].m_autoPairstart;
			count = m_constraintArray[i].m_autoPaircount;

			m0 = m_constraintArray[i].m_m0;
			m1 = m_constraintArray[i].m_m1;

			y0.m_linear = zero;
			y0.m_angular = zero;
			y1.m_linear = zero;
			y1.m_angular = zero;
			for (dgInt32 j = 0; j < count; j ++) { 
				dgInt32 index;

				index = j + first;
				val = m_force[index]; 
				_ASSERTE (dgCheckFloat(val));
				y0.m_linear += m_Jt[index].m_jacobian_IM0.m_linear.Scale (val);
				y0.m_angular += m_Jt[index].m_jacobian_IM0.m_angular.Scale (val);
				y1.m_linear += m_Jt[index].m_jacobian_IM1.m_linear.Scale (val);
				y1.m_angular += m_Jt[index].m_jacobian_IM1.m_angular.Scale (val);
			}

			m_world->dgGetIndirectLock(&m_locks[m0]);
			m_internalForces[m0].m_linear += y0.m_linear;
			m_internalForces[m0].m_angular += y0.m_angular;
			m_world->dgReleaseIndirectLock(&m_locks[m0]);

			m_world->dgGetIndirectLock(&m_locks[m1]);
			m_internalForces[m1].m_linear += y1.m_linear;
			m_internalForces[m1].m_angular += y1.m_angular;
			m_world->dgReleaseIndirectLock(&m_locks[m1]);
		}
	}

	//	m_world->m_perfomanceCounters[m_threadIndex][m_dynamicsSolveSpanningTreeTicks] += (m_world->m_getPerformanceCount() - ticks);
}


void dgParallelSolverJointAcceleration::ThreadExecute()
{
	//	dgUnsigned32 ticks;
	//	ticks = m_world->m_getPerformanceCount();

	if (m_useSimd) {
		dgInt32 currJoint;

		currJoint = m_jointStart;
		for (dgInt32 i = 0; i < m_count; i ++) {
			dgInt32 index;

			dgJointAccelerationDecriptor joindDesc;
			index = m_constraintArray[currJoint].m_autoPairstart;
			joindDesc.m_rowsCount = m_constraintArray[currJoint].m_autoPaircount;

			joindDesc.m_timeStep = m_timeStep;
			joindDesc.m_invTimeStep = m_invTimeStep;
			joindDesc.m_firstPassCoefFlag = m_firstPassCoef;

			joindDesc.m_Jt = &m_Jt[index];
			joindDesc.m_penetration = &m_penetration[index];
			joindDesc.m_restitution = &m_restitution[index];
			joindDesc.m_externAccelaration = &m_externAccel[index];
			joindDesc.m_coordenateAccel = &m_coordenateAccel[index];
			joindDesc.m_accelIsMotor = &m_accelIsMortor[index];
			joindDesc.m_normalForceIndex = &m_normalForceIndex[index];
			joindDesc.m_penetrationStiffness = &m_penetrationStiffness[index];
			m_constraintArray[currJoint].m_joint->JointAccelerationsSimd (joindDesc);

			currJoint ++;
		}
	} else {
		dgInt32 currJoint;

		currJoint = m_jointStart;
		for (dgInt32 i = 0; i < m_count; i ++) {
			dgInt32 index;
			dgJointAccelerationDecriptor joindDesc;

			index = m_constraintArray[currJoint].m_autoPairstart;
			joindDesc.m_rowsCount = m_constraintArray[currJoint].m_autoPaircount;

			joindDesc.m_timeStep = m_timeStep;
			joindDesc.m_invTimeStep = m_invTimeStep;
			joindDesc.m_firstPassCoefFlag = m_firstPassCoef;

			joindDesc.m_Jt = &m_Jt[index];
			joindDesc.m_penetration = &m_penetration[index];
			joindDesc.m_restitution = &m_restitution[index];
			joindDesc.m_externAccelaration = &m_externAccel[index];
			joindDesc.m_coordenateAccel = &m_coordenateAccel[index];
			joindDesc.m_accelIsMotor = &m_accelIsMortor[index];
			joindDesc.m_normalForceIndex = &m_normalForceIndex[index];
			joindDesc.m_penetrationStiffness = &m_penetrationStiffness[index];
			m_constraintArray[currJoint].m_joint->JointAccelerations (joindDesc);

			currJoint ++;
		}
	}

	m_firstPassCoef = dgFloat32 (1.0f);
	//	m_world->m_perfomanceCounters[m_threadIndex][m_dynamicsSolveSpanningTreeTicks] += (m_world->m_getPerformanceCount() - ticks);
}


void dgParallelSolverUpdateVeloc::ThreadExecute()
{
	//	dgUnsigned32 ticks;
	//	ticks = m_world->m_getPerformanceCount();

	if (m_useSimd) {
#ifdef DG_BUILD_SIMD_CODE
		simd_type timeStepSimd;
		timeStepSimd = simd_set1 (m_timeStep);
		for (dgInt32 i = 0; i < m_count; i ++) {
			dgBody* body;
			simd_type force;
			simd_type torque;
			simd_type accel;
			simd_type alpha;

			body = m_bodyArray[i].m_body;
			force = simd_add_v ((simd_type&) body->m_accel, (simd_type&)m_internalForces[i].m_linear);
			torque = simd_add_v ((simd_type&) body->m_alpha, (simd_type&)m_internalForces[i].m_angular);

			//dgVector accel (force.Scale (body->m_invMass.m_w));
			accel = simd_mul_v (force, simd_set1 (body->m_invMass.m_w));
			//dgVector alpha (body->m_invWorldInertiaMatrix.RotateVector (torque));
			alpha = simd_mul_add_v (simd_mul_add_v (simd_mul_v ((simd_type&)body->m_invWorldInertiaMatrix[0], simd_permut_v (torque, torque, PURMUT_MASK(0, 0, 0, 0))), 
				(simd_type&)body->m_invWorldInertiaMatrix[1], simd_permut_v (torque, torque, PURMUT_MASK(1, 1, 1, 1))), 
				(simd_type&)body->m_invWorldInertiaMatrix[2], simd_permut_v (torque, torque, PURMUT_MASK(2, 2, 2, 2)));

			//body->m_veloc += accel.Scale(timeStep);
			(simd_type&) body->m_veloc = simd_mul_add_v ((simd_type&) body->m_veloc, accel, timeStepSimd);
			//body->m_omega += alpha.Scale(timeStep);
			(simd_type&) body->m_omega = simd_mul_add_v ((simd_type&) body->m_omega, alpha, timeStepSimd);

			//body->m_netForce += body->m_veloc;
			(simd_type&)m_internalVeloc[i].m_linear = simd_add_v ((simd_type&)m_internalVeloc[i].m_linear, (simd_type&) body->m_veloc);
			//body->m_netTorque += body->m_omega;
			(simd_type&)m_internalVeloc[i].m_angular = simd_add_v ((simd_type&)m_internalVeloc[i].m_angular, (simd_type&) body->m_omega);
		}
#endif
	} else {

		for (dgInt32 i = 0; i < m_count; i ++) {
			dgBody* body;
			body = m_bodyArray[i].m_body;
			dgVector force (body->m_accel + m_internalForces[i].m_linear);
			dgVector torque (body->m_alpha + m_internalForces[i].m_angular);

			dgVector accel (force.Scale (body->m_invMass.m_w));
			dgVector alpha (body->m_invWorldInertiaMatrix.RotateVector (torque));
			body->m_veloc += accel.Scale(m_timeStep);
			body->m_omega += alpha.Scale(m_timeStep);
			m_internalVeloc[i].m_linear += body->m_veloc;
			m_internalVeloc[i].m_angular += body->m_omega;
		}

	}

	//	m_world->m_perfomanceCounters[m_threadIndex][m_dynamicsSolveSpanningTreeTicks] += (m_world->m_getPerformanceCount() - ticks);
}


void dgParallelSolverUpdateForce::ThreadExecute()
{
	//	dgUnsigned32 ticks;
	//	ticks = m_world->m_getPerformanceCount();

	if (m_useSimd) {
#ifdef DG_BUILD_SIMD_CODE
		simd_type invStepSimd;
		simd_type invTimeStepSimd;
		simd_type accelerationTolerance;
		//	signMask = simd_set1 (invStep);	
		invStepSimd = simd_set1 (m_invStep);	

		invTimeStepSimd = simd_set1 (m_invTimeStep);	
		accelerationTolerance = simd_set1 (m_maxAccNorm2);	
		for (dgInt32 i = 0; i < m_count; i ++) {
			dgBody* body;
			simd_type tmp;
			simd_type accel;
			simd_type alpha;
			body = m_bodyArray[i].m_body;

			//body->m_veloc = internalVeloc[i].m_linear.Scale(invStep);
			//body->m_omega = internalVeloc[i].m_angular.Scale(invStep);
			(simd_type&) body->m_veloc = simd_mul_v ((simd_type&) m_internalVeloc[i].m_linear, invStepSimd);
			(simd_type&) body->m_omega = simd_mul_v ((simd_type&) m_internalVeloc[i].m_angular, invStepSimd);


			//dgVector accel = (body->m_veloc - body->m_netForce).Scale (m_invTimeStep);
			//dgVector alpha = (body->m_omega - body->m_netTorque).Scale (m_invTimeStep);
			accel = simd_mul_v (simd_sub_v ((simd_type&) body->m_veloc, (simd_type&) body->m_netForce), invTimeStepSimd);
			alpha = simd_mul_v (simd_sub_v ((simd_type&) body->m_omega, (simd_type&) body->m_netTorque), invTimeStepSimd);

			//if ((accel % accel) < maxAccNorm2) {
			//	accel = zero;
			//}
			//body->m_accel = accel;
			//body->m_netForce = accel.Scale (body->m_mass[3]);
			tmp = simd_mul_v (accel, accel);
			tmp = simd_add_v (tmp, simd_move_hl_v (tmp, tmp));
			tmp = simd_add_s (tmp, simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 1)));
			tmp = simd_cmplt_s (tmp, accelerationTolerance);
			(simd_type&)body->m_accel = simd_andnot_v (accel, simd_permut_v (tmp, tmp, PURMUT_MASK(0, 0, 0, 0)));
			(simd_type&)body->m_netForce = simd_mul_v ((simd_type&)body->m_accel, simd_set1 (body->m_mass[3]));

			//if ((alpha % alpha) < maxAccNorm2) {
			//	alpha = zero;
			//}
			//body->m_alpha = alpha;
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
#endif

	} else {
		dgVector zero (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
		for (dgInt32 i = 0; i < m_count; i ++) {
			dgBody* body;
			body = m_bodyArray[i].m_body;

			body->m_veloc = m_internalVeloc[i].m_linear.Scale(m_invStep);
			body->m_omega = m_internalVeloc[i].m_angular.Scale(m_invStep);

			dgVector accel = (body->m_veloc - body->m_netForce).Scale (m_invTimeStep);
			dgVector alpha = (body->m_omega - body->m_netTorque).Scale (m_invTimeStep);
			if ((accel % accel) < m_maxAccNorm2) {
				accel = zero;
			}

			if ((alpha % alpha) < m_maxAccNorm2) {
				alpha = zero;
			}

			body->m_accel = accel;
			body->m_alpha = alpha;
			body->m_netForce = accel.Scale (body->m_mass[3]);

			alpha = body->m_matrix.UnrotateVector(alpha);
			body->m_netTorque = body->m_matrix.RotateVector (alpha.CompProduct(body->m_mass));
		}
	}

	//	m_world->m_perfomanceCounters[m_threadIndex][m_dynamicsSolveSpanningTreeTicks] += (m_world->m_getPerformanceCount() - ticks);
}

void dgParallelSolverInitFeedbackUpdate::ThreadExecute()
{
	//	dgUnsigned32 ticks;
	//	ticks = m_world->m_getPerformanceCount();

	for (dgInt32 i = 0; i < m_count; i ++) {
		dgInt32 first;
		dgInt32 count;
		dgInt32 index;
		dgFloat32 val;
		//		dgFloat32 maxForce;

		//		maxForce = dgFloat32 (0.0f);
		first = m_constraintArray[i].m_autoPairstart;
		count = m_constraintArray[i].m_autoPaircount;
		for (dgInt32 j = 0; j < count; j ++) { 
			index = j + first;
			val = m_force[index]; 
			//			maxForce = GetMax(dgAbsf (val), maxForce);
			_ASSERTE (dgCheckFloat(val));
			m_jointForceFeeback[index][0] = val;
		}
		m_hasJointFeeback |= (m_constraintArray[i].m_joint->m_updaFeedbackCallback ? 1 : 0);
		//		if (m_constraintArray[i].m_joint->m_updaFeedbackCallback) {
		//			m_constraintArray[i].m_joint->m_updaFeedbackCallback (*m_constraintArray[i].m_joint, m_timestep, m_threadIndex);
		//		}

		//		if (m_constraintArray[i].m_joint->GetId() == dgContactConstraintId) {
		//			dgInt32 m0;
		//			dgInt32 m1;
		//			m0 = m_constraintArray[i].m_m0;
		//			m1 = m_constraintArray[i].m_m1;
		//			m_world->dgGetIndirectLock(&m_locks[m0]);
		//				m_world->AddToBreakQueue ((dgContact*)m_constraintArray[i].m_joint, m_constraintArray[i].m_joint->m_body0, maxForce);
		//			m_world->dgReleaseIndirectLock(&m_locks[m0]);
		//			m_world->dgGetIndirectLock(&m_locks[m1]);
		//				m_world->AddToBreakQueue ((dgContact*)m_constraintArray[i].m_joint, m_constraintArray[i].m_joint->m_body1, maxForce);
		//			m_world->dgReleaseIndirectLock(&m_locks[m1]);
		//		}
	}

	//	m_world->m_perfomanceCounters[m_threadIndex][m_dynamicsSolveSpanningTreeTicks] += (m_world->m_getPerformanceCount() - ticks);
}


void dgParallelSolverCalculateForces::ThreadExecute()
{
	//	dgInt32 threadIndex;
	//	dgUnsigned32 ticks;
	//	ticks = m_world->m_getPerformanceCount();

	//	threadIndex = m_threadIndex;
	if (m_useSimd) {
#ifdef DG_BUILD_SIMD_CODE
		dgFloatSign tmpIndex;
		simd_type signMask;
		simd_type accNorm;

		tmpIndex.m_integer.m_iVal = 0x7fffffff;
		signMask = simd_set1(tmpIndex.m_fVal);

		accNorm = simd_set1 (dgFloat32 (0.0f));
		for (dgInt32 i = 0; i < m_count; i += m_threads) {
			//		for (dgInt32 currJoint = 0; currJoint < m_count; currJoint += m_threads) {

			dgInt32 m0;
			dgInt32 m1;
			dgInt32 index;
			//			dgInt32 rowIndex;
			dgInt32 currJoint;
			dgInt32 rowsCount;
			simd_type linearM0; 
			simd_type angularM0;
			simd_type linearM1; 
			simd_type angularM1;

			currJoint = m_threadIndex + i;
			//			rowIndex = m_jointRemapArray[i];
			index = m_constraintArray[currJoint].m_autoPairstart;
			rowsCount = m_constraintArray[currJoint].m_autoPaircount;
			m0 = m_constraintArray[currJoint].m_m0;
			m1 = m_constraintArray[currJoint].m_m1;

			if (m0) {
				m_world->dgGetIndirectLock(&m_locks[m0]);
			}
			if (m1) {
				m_world->dgGetIndirectLock(&m_locks[m1]);
			}

			linearM0  = (simd_type&)m_internalForces[m0].m_linear;
			angularM0 = (simd_type&)m_internalForces[m0].m_angular;
			linearM1  = (simd_type&)m_internalForces[m1].m_linear;
			angularM1 = (simd_type&)m_internalForces[m1].m_angular;

			for (dgInt32 k = 0; k < rowsCount; k ++) {
				dgInt32 frictionIndex;
				simd_type a;
				simd_type f;
				simd_type frictionNormal;
				simd_type lowerFrictionForce;
				simd_type upperFrictionForce;

				//				dgVector acc (m_JMinv[index].m_jacobian_IM0.m_linear.CompProduct(linearM0));
				//				acc += m_JMinv[index].m_jacobian_IM0.m_angular.CompProduct (angularM0);
				//				acc += m_JMinv[index].m_jacobian_IM1.m_linear.CompProduct (linearM1);
				//				acc += m_JMinv[index].m_jacobian_IM1.m_angular.CompProduct (angularM1);

				a = simd_mul_v (       (simd_type&)m_JMinv[index].m_jacobian_IM0.m_linear, linearM0);
				a = simd_mul_add_v (a, (simd_type&)m_JMinv[index].m_jacobian_IM0.m_angular, angularM0);
				a = simd_mul_add_v (a, (simd_type&)m_JMinv[index].m_jacobian_IM1.m_linear, linearM1);
				a = simd_mul_add_v (a, (simd_type&)m_JMinv[index].m_jacobian_IM1.m_angular, angularM1);

				//a = coordenateAccel[index] - acc.m_x - acc.m_y - acc.m_z - force[index] * diagDamp[index];
				a = simd_add_v (a, simd_move_hl_v(a, a));
				a = simd_add_s (a, simd_permut_v (a, a, PURMUT_MASK(3, 3, 3, 1)));
				a = simd_sub_s(simd_load_s(m_coordenateAccel[index]), simd_mul_add_s(a, simd_load_s(m_force[index]), simd_load_s(m_diagDamp[index])));


				//f = force[index] + invDJMinvJt[index] * a;
				f = simd_mul_add_s (simd_load_s(m_force[index]), simd_load_s(m_invDJMinvJt[index]), a);

				frictionIndex = m_normalForceIndex[index];
				_ASSERTE (((frictionIndex < 0) && (m_force[frictionIndex] == dgFloat32 (1.0f))) || ((frictionIndex >= 0) && (m_force[frictionIndex] >= dgFloat32 (0.0f))));


				//frictionNormal = force[frictionIndex];
				//lowerFrictionForce = frictionNormal * lowerFrictionCoef[index];
				//upperFrictionForce = frictionNormal * upperFrictionCoef[index];
				frictionNormal = simd_load_s(m_force[frictionIndex]);
				lowerFrictionForce = simd_mul_s (frictionNormal, simd_load_s(m_lowerFrictionCoef[index]));
				upperFrictionForce = simd_mul_s (frictionNormal, simd_load_s(m_upperFrictionCoef[index]));


				//if (f > upperFrictionForce) {
				//	a = dgFloat32 (0.0f);
				//	f = upperFrictionForce;
				//} else if (f < lowerFrictionForce) {
				//	a = dgFloat32 (0.0f);
				//	f = lowerFrictionForce;
				//}
				//frictionNormal = simd_or_v (simd_cmplt_s (f, lowerFrictionForce), simd_cmpgt_s (f, upperFrictionForce));
				f = simd_min_s (simd_max_s (f, lowerFrictionForce), upperFrictionForce);
				a = simd_andnot_v (a, simd_or_v (simd_cmplt_s (f, lowerFrictionForce), simd_cmpgt_s (f, upperFrictionForce)));
				accNorm = simd_max_s (accNorm, simd_and_v (a, signMask));


				//prevValue = f - force[index]);
				a = simd_sub_s (f, simd_load_s (m_force[index]));
				a = simd_permut_v (a, a, PURMUT_MASK(0, 0, 0, 0));

				//force[index] = f;
				simd_store_s (f, &m_force[index]);

				linearM0 = simd_mul_add_v (linearM0, (simd_type&) m_Jt[index].m_jacobian_IM0.m_linear, a);
				angularM0 = simd_mul_add_v (angularM0,(simd_type&) m_Jt[index].m_jacobian_IM0.m_angular, a);
				linearM1 = simd_mul_add_v (linearM1, (simd_type&) m_Jt[index].m_jacobian_IM1.m_linear, a);
				angularM1 = simd_mul_add_v (angularM1,(simd_type&) m_Jt[index].m_jacobian_IM1.m_angular, a);
				index ++;
			}

			(simd_type&)m_internalForces[m0].m_linear = linearM0;
			(simd_type&)m_internalForces[m0].m_angular = angularM0;
			(simd_type&)m_internalForces[m1].m_linear = linearM1;
			(simd_type&)m_internalForces[m1].m_angular = angularM1;

			if (m1) {
				m_world->dgReleaseIndirectLock(&m_locks[m1]);
			}
			if (m0) {
				m_world->dgReleaseIndirectLock(&m_locks[m0]);
			}
		}
		simd_store_s (accNorm, &m_accNorm);
#endif

	} else {
		dgFloat32 accNorm;
		accNorm = dgFloat32 (0.0f);
		for (dgInt32 i = 0; i < m_count; i += m_threads) {
			dgInt32 m0;
			dgInt32 m1;
			dgInt32 index;
			dgInt32 currJoint;
			dgInt32 rowsCount;

			currJoint = m_threadIndex + i;
			index = m_constraintArray[currJoint].m_autoPairstart;
			rowsCount = m_constraintArray[currJoint].m_autoPaircount;
			m0 = m_constraintArray[currJoint].m_m0;
			m1 = m_constraintArray[currJoint].m_m1;

			if (m0) {
				m_world->dgGetIndirectLock(&m_locks[m0]);
			}
			if (m1) {
				m_world->dgGetIndirectLock(&m_locks[m1]);
			}

			dgVector linearM0 (m_internalForces[m0].m_linear);
			dgVector angularM0 (m_internalForces[m0].m_angular);
			dgVector linearM1 (m_internalForces[m1].m_linear);
			dgVector angularM1 (m_internalForces[m1].m_angular);
			for (dgInt32 k = 0; k < rowsCount; k ++) {
				dgInt32 frictionIndex;
				dgFloat32 a;
				dgFloat32 f;
				dgFloat32 prevValue;
				dgFloat32 frictionNormal;
				dgFloat32 lowerFrictionForce;
				dgFloat32 upperFrictionForce;

				dgVector acc (m_JMinv[index].m_jacobian_IM0.m_linear.CompProduct(linearM0));
				acc += m_JMinv[index].m_jacobian_IM0.m_angular.CompProduct (angularM0);
				acc += m_JMinv[index].m_jacobian_IM1.m_linear.CompProduct (linearM1);
				acc += m_JMinv[index].m_jacobian_IM1.m_angular.CompProduct (angularM1);

				a = m_coordenateAccel[index] - acc.m_x - acc.m_y - acc.m_z - m_force[index] * m_diagDamp[index];
				f = m_force[index] + m_invDJMinvJt[index] * a;

				frictionIndex = m_normalForceIndex[index];
				_ASSERTE (((frictionIndex < 0) && (m_force[frictionIndex] == dgFloat32 (1.0f))) || ((frictionIndex >= 0) && (m_force[frictionIndex] >= dgFloat32 (0.0f))));

				frictionNormal = m_force[frictionIndex];
				lowerFrictionForce = frictionNormal * m_lowerFrictionCoef[index];
				upperFrictionForce = frictionNormal * m_upperFrictionCoef[index];

				if (f > upperFrictionForce) {
					a = dgFloat32 (0.0f);
					f = upperFrictionForce;
				} else if (f < lowerFrictionForce) {
					a = dgFloat32 (0.0f);
					f = lowerFrictionForce;
				}

				accNorm = GetMax (accNorm, dgAbsf (a));
				prevValue = f - m_force[index];
				m_force[index] = f;

				linearM0 += m_Jt[index].m_jacobian_IM0.m_linear.Scale (prevValue);
				angularM0 += m_Jt[index].m_jacobian_IM0.m_angular.Scale (prevValue);
				linearM1 += m_Jt[index].m_jacobian_IM1.m_linear.Scale (prevValue);
				angularM1 += m_Jt[index].m_jacobian_IM1.m_angular.Scale (prevValue);
				index ++;
			}
			m_internalForces[m0].m_linear = linearM0;
			m_internalForces[m0].m_angular = angularM0;
			m_internalForces[m1].m_linear = linearM1;
			m_internalForces[m1].m_angular = angularM1;

			if (m1) {
				m_world->dgReleaseIndirectLock(&m_locks[m1]);
			}
			if (m0) {
				m_world->dgReleaseIndirectLock(&m_locks[m0]);
			}
		}
		m_accNorm = accNorm;
	}

	//	m_world->m_perfomanceCounters[m_threadIndex][m_dynamicsSolveSpanningTreeTicks] += (m_world->m_getPerformanceCount() - ticks);
}



void dgJacobianMemory::CalculateForcesGameModeParallel (dgInt32 iterations, dgFloat32 maxAccNorm, dgInt32 archModel) const
{
	dgInt32 acc;
	dgInt32 threads; 
	dgInt32 maxPasses;
	dgInt32 hasJointFeeback;
	dgFloat32 invStep;
	dgFloat32 timeStep;
	dgFloat32 invTimeStep;
	dgFloat32 maxAccNorm2;

	dgFloat32* const force = m_force;
	const dgJacobianPair* const Jt = m_Jt;
	const dgJacobianPair* const JMinv = m_JMinv;
	const dgFloat32* const diagDamp = m_diagDamp;
	const dgFloat32* const invDJMinvJt = m_invDJMinvJt;
	const dgBodyInfo* bodyArray = m_bodyArray;
	dgFloat32* const penetration = m_penetration;
	const dgFloat32* const externAccel = m_deltaAccel;
	const dgFloat32* const restitution = m_restitution;
	dgFloat32* const coordenateAccel = m_coordenateAccel;
	dgJacobian* const internalVeloc = m_internalVeloc;
	dgJacobian* const internalForces = m_internalForces;;
	//	const dgInt32* const jointRemapArray = m_jointRemapArray;
	dgFloat32** const jointForceFeeback = m_jointFeebackForce;
	const dgInt32* const accelIsMortor = m_accelIsMotor;
	const dgInt32* const normalForceIndex = m_normalForceIndex;
	const dgJointInfo* const constraintArray = m_constraintArray;
	const dgFloat32* const penetrationStiffness = m_penetrationStiffness;
	const dgFloat32* const lowerFrictionCoef = m_lowerBoundFrictionCoefficent;
	const dgFloat32* const upperFrictionCoef = m_upperBoundFrictionCoefficent;
	dgVector zero(dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));

	invStep = (dgFloat32 (1.0f) / dgFloat32 (LINEAR_SOLVER_SUB_STEPS));
	timeStep =  m_timeStep * invStep;
	invTimeStep = m_invTimeStep * dgFloat32 (LINEAR_SOLVER_SUB_STEPS);

	threads = m_world->m_numberOfTheads;

	dgInt32 chunkJointSizes[DG_MAX_THREADS_HIVE_COUNT];
	dgInt32 chunkBodiesSizes[DG_MAX_THREADS_HIVE_COUNT];

	acc = 0;
	m_world->m_threadsManager.CalculateChunkSizes(m_bodyCount - 1, chunkBodiesSizes);
	dgParallelSolverClear* const clearAccumulators = m_world->m_dynamicSolver.m_clearAccumulators;
	for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
		clearAccumulators[threadIndex].m_useSimd = archModel;
		clearAccumulators[threadIndex].m_world = m_world;
		clearAccumulators[threadIndex].m_threadIndex = threadIndex;
		clearAccumulators[threadIndex].m_count = chunkBodiesSizes[threadIndex];

		clearAccumulators[threadIndex].m_bodyArray = &bodyArray[acc + 1];
		clearAccumulators[threadIndex].m_internalVeloc = &internalVeloc[acc + 1];
		clearAccumulators[threadIndex].m_internalForces = &internalForces[acc + 1];
		m_world->m_threadsManager.SubmitJob(&clearAccumulators[threadIndex]);
		acc += chunkBodiesSizes[threadIndex];
	}
	m_world->m_threadsManager.SynchronizationBarrier ();

	internalVeloc[0].m_linear = zero;
	internalVeloc[0].m_angular = zero;
	internalForces[0].m_linear = zero;
	internalForces[0].m_angular = zero;


	acc = 0;
	m_world->m_threadsManager.CalculateChunkSizes(m_jointCount, chunkJointSizes);
	dgParallelSolverInitInternalForces* const parallelInitIntenalForces = m_world->m_dynamicSolver.m_parallelInitIntenalForces;
	for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
		parallelInitIntenalForces[threadIndex].m_useSimd = archModel;
		parallelInitIntenalForces[threadIndex].m_world = m_world;
		parallelInitIntenalForces[threadIndex].m_locks =  m_treadLocks;
		parallelInitIntenalForces[threadIndex].m_threadIndex = threadIndex;
		parallelInitIntenalForces[threadIndex].m_Jt = Jt;
		parallelInitIntenalForces[threadIndex].m_force = force;
		parallelInitIntenalForces[threadIndex].m_internalForces = internalForces;
		parallelInitIntenalForces[threadIndex].m_count = chunkJointSizes[threadIndex];
		parallelInitIntenalForces[threadIndex].m_constraintArray = &constraintArray[acc];
		m_world->m_threadsManager.SubmitJob(&parallelInitIntenalForces[threadIndex]);
		acc += chunkJointSizes[threadIndex];
	}
	m_world->m_threadsManager.SynchronizationBarrier ();

	acc = 0;
	dgParallelSolverJointAcceleration* const parallelSolverJointAcceleration = m_world->m_dynamicSolver.m_parallelSolverJointAcceleration;
	for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
		parallelSolverJointAcceleration[threadIndex].m_useSimd = archModel;
		parallelSolverJointAcceleration[threadIndex].m_world = m_world;
		parallelSolverJointAcceleration[threadIndex].m_threadIndex = threadIndex;
		parallelSolverJointAcceleration[threadIndex].m_Jt = Jt;
		parallelSolverJointAcceleration[threadIndex].m_count = chunkJointSizes[threadIndex];
		parallelSolverJointAcceleration[threadIndex].m_constraintArray = constraintArray;
		parallelSolverJointAcceleration[threadIndex].m_jointStart = acc;
		//		parallelSolverJointAcceleration[threadIndex].m_jointRemapArray = &jointRemapArray[acc];
		parallelSolverJointAcceleration[threadIndex].m_timeStep = timeStep;
		parallelSolverJointAcceleration[threadIndex].m_invTimeStep = invTimeStep;
		parallelSolverJointAcceleration[threadIndex].m_firstPassCoef = dgFloat32 (1.0f);
		parallelSolverJointAcceleration[threadIndex].m_penetration = penetration;
		parallelSolverJointAcceleration[threadIndex].m_restitution = restitution;
		parallelSolverJointAcceleration[threadIndex].m_externAccel = externAccel;
		parallelSolverJointAcceleration[threadIndex].m_coordenateAccel = coordenateAccel;
		parallelSolverJointAcceleration[threadIndex].m_accelIsMortor = accelIsMortor;
		parallelSolverJointAcceleration[threadIndex].m_normalForceIndex = normalForceIndex;
		parallelSolverJointAcceleration[threadIndex].m_penetrationStiffness = penetrationStiffness;
		acc += chunkJointSizes[threadIndex];
	}

	acc = 0;
	dgParallelSolverUpdateVeloc* const parallelSolverUpdateVeloc = m_world->m_dynamicSolver.m_parallelSolverUpdateVeloc;
	for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
		parallelSolverUpdateVeloc[threadIndex].m_useSimd = archModel;
		parallelSolverUpdateVeloc[threadIndex].m_world = m_world;
		parallelSolverUpdateVeloc[threadIndex].m_threadIndex = threadIndex;
		parallelSolverUpdateVeloc[threadIndex].m_timeStep = timeStep;
		parallelSolverUpdateVeloc[threadIndex].m_count = chunkBodiesSizes[threadIndex];
		parallelSolverUpdateVeloc[threadIndex].m_bodyArray = &bodyArray[acc + 1];
		parallelSolverUpdateVeloc[threadIndex].m_internalVeloc = &internalVeloc[acc + 1];
		parallelSolverUpdateVeloc[threadIndex].m_internalForces = &internalForces[acc + 1];
		acc += chunkBodiesSizes[threadIndex];
	}



	dgParallelSolverCalculateForces* const parallelSolverCalculateForces = m_world->m_dynamicSolver.m_parallelSolverCalculateForces;
	for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
		parallelSolverCalculateForces[threadIndex].m_useSimd = archModel;
		parallelSolverCalculateForces[threadIndex].m_world = m_world;
		parallelSolverCalculateForces[threadIndex].m_threads = threads;
		parallelSolverCalculateForces[threadIndex].m_threadIndex = threadIndex;

		parallelSolverCalculateForces[threadIndex].m_Jt = Jt;
		parallelSolverCalculateForces[threadIndex].m_JMinv = JMinv;
		parallelSolverCalculateForces[threadIndex].m_count = chunkJointSizes[threadIndex] * threads;
		//		parallelSolverCalculateForces[threadIndex].m_jointRemapArray = &jointRemapArray[threadIndex];
		parallelSolverCalculateForces[threadIndex].m_constraintArray = constraintArray;

		parallelSolverCalculateForces[threadIndex].m_locks = m_treadLocks;
		parallelSolverCalculateForces[threadIndex].m_force = force;
		parallelSolverCalculateForces[threadIndex].m_diagDamp = diagDamp;
		parallelSolverCalculateForces[threadIndex].m_invDJMinvJt = invDJMinvJt;
		parallelSolverCalculateForces[threadIndex].m_normalForceIndex = normalForceIndex;
		parallelSolverCalculateForces[threadIndex].m_internalForces = internalForces;
		parallelSolverCalculateForces[threadIndex].m_coordenateAccel = coordenateAccel;
		parallelSolverCalculateForces[threadIndex].m_lowerFrictionCoef = lowerFrictionCoef;
		parallelSolverCalculateForces[threadIndex].m_upperFrictionCoef = upperFrictionCoef;
	}



	maxPasses = iterations + DG_BASE_ITERATION_COUNT;
	for (dgInt32 step = 0; step < LINEAR_SOLVER_SUB_STEPS; step ++) {
		for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
			m_world->m_threadsManager.SubmitJob(&parallelSolverJointAcceleration[threadIndex]);
		}
		m_world->m_threadsManager.SynchronizationBarrier ();

		dgFloat32 accNorm;
		accNorm = maxAccNorm * dgFloat32 (2.0f);
		for (dgInt32 passes = 0; (passes < maxPasses) && (accNorm > maxAccNorm); passes ++) {

			for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
				m_world->m_threadsManager.SubmitJob(&parallelSolverCalculateForces[threadIndex]);
			}
			m_world->m_threadsManager.SynchronizationBarrier ();

			accNorm = dgFloat32 (0.0f);
			for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
				if (parallelSolverCalculateForces[threadIndex].m_accNorm > accNorm) {
					accNorm = parallelSolverCalculateForces[threadIndex].m_accNorm;
				}
			}
		}


		for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
			m_world->m_threadsManager.SubmitJob(&parallelSolverUpdateVeloc[threadIndex]);
		}
		m_world->m_threadsManager.SynchronizationBarrier ();
	}

	acc = 0;
	m_world->m_threadsManager.CalculateChunkSizes(m_jointCount, chunkJointSizes);
	dgParallelSolverInitFeedbackUpdate* const parallelSolverInitFeedbackUpdate = m_world->m_dynamicSolver.m_parallelSolverInitFeedbackUpdate;
	for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
		parallelSolverInitFeedbackUpdate[threadIndex].m_locks = m_treadLocks;
		parallelSolverInitFeedbackUpdate[threadIndex].m_world = m_world;
		parallelSolverInitFeedbackUpdate[threadIndex].m_hasJointFeeback = 0;
		parallelSolverInitFeedbackUpdate[threadIndex].m_threadIndex = threadIndex;
		parallelSolverInitFeedbackUpdate[threadIndex].m_force = force;
		parallelSolverInitFeedbackUpdate[threadIndex].m_timestep = timeStep;
		parallelSolverInitFeedbackUpdate[threadIndex].m_jointForceFeeback = jointForceFeeback;
		parallelSolverInitFeedbackUpdate[threadIndex].m_count = chunkJointSizes[threadIndex];
		parallelSolverInitFeedbackUpdate[threadIndex].m_constraintArray = &constraintArray[acc];
		m_world->m_threadsManager.SubmitJob(&parallelSolverInitFeedbackUpdate[threadIndex]);
		acc += chunkJointSizes[threadIndex];
	}
	m_world->m_threadsManager.SynchronizationBarrier ();


	acc = 0;
	maxAccNorm2 = maxAccNorm * maxAccNorm;
	dgParallelSolverUpdateForce* const parallelSolverUpdateForce = m_world->m_dynamicSolver.m_parallelSolverUpdateForce;
	for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
		parallelSolverUpdateForce[threadIndex].m_useSimd = archModel;
		parallelSolverUpdateForce[threadIndex].m_world = m_world;
		parallelSolverUpdateForce[threadIndex].m_threadIndex = threadIndex;
		parallelSolverUpdateForce[threadIndex].m_invStep = invStep;
		parallelSolverUpdateForce[threadIndex].m_invTimeStep = m_invTimeStep;
		parallelSolverUpdateForce[threadIndex].m_maxAccNorm2 = maxAccNorm2;
		parallelSolverUpdateForce[threadIndex].m_count = chunkBodiesSizes[threadIndex];
		parallelSolverUpdateForce[threadIndex].m_bodyArray = &bodyArray[acc + 1];
		parallelSolverUpdateForce[threadIndex].m_internalVeloc = &internalVeloc[acc + 1];
		acc += chunkBodiesSizes[threadIndex];
		m_world->m_threadsManager.SubmitJob(&parallelSolverUpdateForce[threadIndex]);
	}
	m_world->m_threadsManager.SynchronizationBarrier ();

	hasJointFeeback = 0;
	for (dgInt32 threadIndex = 0; threadIndex < threads; threadIndex ++) {
		hasJointFeeback |= parallelSolverInitFeedbackUpdate[threadIndex].m_hasJointFeeback;
	}

	if (hasJointFeeback) {
		for (dgInt32 i = 0; i < m_jointCount; i ++) {
			if (constraintArray[i].m_joint->m_updaFeedbackCallback) {
				constraintArray[i].m_joint->m_updaFeedbackCallback (*constraintArray[i].m_joint, m_timeStep, m_threadIndex);
			}
		}
	}

}


void dgJacobianMemory::CalculateReactionsForcesParallel (dgInt32 solverMode, dgFloat32 tolerance, dgInt32 archModel) const
{
	//	_ASSERTE (m_jointCount >= DG_PARALLEL_JOINT_COUNT);
	if (solverMode) {
		CalculateForcesGameModeParallel(solverMode, tolerance, archModel);
	} else if (archModel) {
		CalculateForcesSimulationModeSimd (tolerance);
	} else {
		CalculateForcesSimulationMode (tolerance);
	}
}

#endif


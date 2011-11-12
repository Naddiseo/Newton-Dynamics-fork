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

#if !defined(AFX_DYNAMIC_UPDATE_H__EC18C699_D48D_448F_A510_A865B2CC0789__INCLUDED_)
#define AFX_DYNAMIC_UPDATE_H__EC18C699_D48D_448F_A510_A865B2CC0789__INCLUDED_


#include "dgPhysicsStdafx.h"


//#define DG_PSD_DAMP_TOL				dgFloat32 (1.0e-2f)
#define DG_PSD_DAMP_TOL					dgFloat32 (1.0e-3f)
//#define DG_PSD_DAMP_TOL				dgFloat32 (1.0e-4f)

#define	DG_FREEZZING_VELOCITY_DRAG		dgFloat32 (0.9f)
#define	DG_SOLVER_MAX_ERROR				(DG_FREEZE_MAG * dgFloat32 (0.5f))


#define LINEAR_SOLVER_SUB_STEPS			4

#ifdef _MAC_IPHONE
	#define DG_BASE_ITERATION_COUNT		2
#else
	#define DG_BASE_ITERATION_COUNT		3
#endif

// in my twist to RK4 I am no sure if the final derivative have to be weighted, it seems a mistake, I need to investigate more
// uncomment this out for more stable behavior but no exactly correct solution on the iterative solver
//#define DG_WIGHT_FINAL_RK4_DERIVATIVES


class dgIslandCallbackStruct
{
	public:
	dgWorld* m_world;
	dgInt32 m_count;
	dgInt32 m_strideInByte;
	void* m_bodyArray;
};

class dgBodyInfo
{
	public:
	dgBody* m_body;
};

class dgIsland
{
	public:
#ifdef _DEBUG
	dgInt32 m_islandId;
#endif
	dgInt32 m_bodyCount;
	dgInt32 m_bodyStart;
	dgInt32 m_jointCount;
	dgInt32 m_jointStart;
	dgInt32 m_rowsCount;
	dgUnsigned32 m_hasUnilateralJoints : 1;
	dgUnsigned32 m_isContinueCollision : 1;
	dgUnsigned32 m_hasExactSolverJoints : 1;
};


class dgJointInfo
{
	public:
	dgConstraint* m_joint;
	dgInt32 m_autoPairstart;
	dgInt32 m_autoPaircount;
	dgInt32 m_autoPairActiveCount;
	dgInt32 m_m0;
	dgInt32 m_m1;
};



template<class T>
class dgQueue
{
	public:
	dgQueue (T* const pool, dgInt32 size)
		:m_pool (pool)
	{
		m_mod = size;
		m_lastIndex = 0;
		m_firstIndex = 0;
	}

	void Insert (T info) 
	{
		m_pool[m_firstIndex] = info;
		m_firstIndex ++;
		if (m_firstIndex >= m_mod) {
			m_firstIndex = 0;
		}
		_ASSERTE (m_firstIndex != m_lastIndex);
	}

	T Remove () 
	{
		_ASSERTE (m_firstIndex != m_lastIndex);

		T element = m_pool[m_lastIndex];
		m_lastIndex ++;
		if (m_lastIndex >= m_mod) {
			m_lastIndex = 0;
		}
		
		return element;
	}

	void Reset ()
	{
		m_lastIndex = m_firstIndex;
	}

	bool IsEmpty () const 
	{
		return (m_firstIndex == m_lastIndex);
	}

	dgInt32 m_mod;
	dgInt32 m_firstIndex;
	dgInt32 m_lastIndex;
	T* m_pool;
};

DG_MSC_VECTOR_ALIGMENT
class dgJacobianMatrixElement
{
	public:
	dgJacobianPair m_Jt;
	dgJacobianPair m_JMinv;

	dgFloat32 m_force;
	dgFloat32 m_accel;
	dgFloat32 m_deltaAccel;
	dgFloat32 m_deltaForce;

	dgFloat32 m_diagDamp;
	dgFloat32 m_invDJMinvJt;
	dgFloat32 m_restitution;
	dgFloat32 m_penetration;

	dgFloat32 m_coordenateAccel;
	dgFloat32 m_penetrationStiffness;
	dgFloat32 m_lowerBoundFrictionCoefficent;
	dgFloat32 m_upperBoundFrictionCoefficent;

	dgFloat32* m_jointFeebackForce;
	dgInt32 m_normalForceIndex;
	bool m_accelIsMotor;
} DG_GCC_VECTOR_ALIGMENT;

class dgJacobianMemory
{
	public:
	dgJacobian* m_internalVeloc;
	dgJacobian* m_internalForces;
	dgInt32* m_treadLocks;

	dgJacobianMatrixElement* m_memory;

	void Init (dgWorld* const world, dgInt32 rowsCount, dgInt32 bodyCount);
	dgInt32 CalculateIntenalMemorySize() const 
	{
		return sizeof (dgJacobian) +  // internalfoces 
			   sizeof (dgJacobian) +  // internal velocites
			   sizeof (dgInt32);      // local atomics
	}

//	void SwapRows (dgInt32 i, dgInt32 j) const;
//	void SwapRowsSimd (dgInt32 i, dgInt32 j) const;
//	void CalculateForcesSimulationMode (dgFloat32 maxAccNorm) const;
//	void CalculateForcesSimulationModeSimd (dgFloat32 maxAccNorm) const;
//	void CalculateForcesGameModeParallel (dgInt32 itertions, dgFloat32 maxAccNorm, dgInt32 archModel) const;
//	void CalculateReactionsForcesParallel(dgInt32 solverMode, dgFloat32 maxAccNorm, dgInt32 archModel) const;
};


class dgParallelSolverSyncData;
class dgWorldDynamicUpdateSyncDescriptor;

class dgWorldDynamicUpdate
{
	dgWorldDynamicUpdate();
	void UpdateDynamics (dgFloat32 timestep);

	private:
	void SpanningTree (dgBody* const body);
	void BuildIsland (dgQueue<dgBody*>& queue, dgInt32 jountCount, dgInt32 rowsCount, dgInt32 hasUnilateralJoints, dgInt32 isContinueCollisionIsland, dgInt32 forceExactSolver);

	static dgInt32 CompareIslands (const dgIsland* const islandA, const dgIsland* const islandB, void* notUsed);
	static void CalculateIslandReactionForces (void* const context, dgInt32 threadID);
	static void ParallelSolverDriver (void* const userParamArray, dgInt32 threadID);

	void InitilizeBodyArrayParallel (dgParallelSolverSyncData* const syncData, dgInt32 threadIndex) const; 
	void BuildJacobianMatrixParallel (dgParallelSolverSyncData* const syncData, dgInt32 threadIndex) const; 
	void SolverInitInternalForcesParallel (dgParallelSolverSyncData* const syncData, dgInt32 threadIndex) const; 
	void CalculateForcesGameModeParallel (dgParallelSolverSyncData* const syncData, dgInt32 threadIndex) const; 
	
	void CalculateReactionForcesParallel (const dgIsland* const island, dgInt32 inlansdCount, dgFloat32 timestep) const;
	void GetJacobianDerivativesParallel (dgJointInfo* const jointInfo, dgInt32 threadIndex, bool bitMode, dgInt32 rowBase, dgFloat32 timestep) const;	
	
	
	
	void CalculateIslandReactionForces (const dgIsland* const island, dgFloat32 timestep, dgInt32 threadID) const;
	dgInt32 BuildJacobianMatrix (const dgIsland* const island, dgInt32 threadIndex, dgFloat32 timestep) const;
	void CalculateForcesGameMode (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const;

	//dgFloat32 CalculateJointForces (const dgIsland* const island, dgInt32 joint, dgFloat32* const forceStep, dgFloat32 maxAccNorm) const;
	dgFloat32 CalculateJointForces (const dgIsland* const island, dgInt32 rowStart, dgInt32 joint, dgFloat32* const forceStep, dgFloat32 maxAccNorm) const;
	void CalculateForcesSimulationMode (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const;


	void CalculateReactionsForces(const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const;
	void ApplyExternalForcesAndAcceleration(const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const;
	void CalculateSimpleBodyReactionsForces (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const;

	void CalculateIslandReactionForcesSimd (const dgIsland* const island, dgFloat32 timestep, dgInt32 threadID) const;
	dgInt32 BuildJacobianMatrixSimd (const dgIsland* const island, dgInt32 threadIndex, dgFloat32 timestep) const; 
	void CalculateForcesGameModeSimd (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const;
	void CalculateReactionsForcesSimd(const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const;
	void ApplyExternalForcesAndAccelerationSimd(const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const;
	void CalculateSimpleBodyReactionsForcesSimd (const dgIsland* const island, dgInt32 rowStart, dgInt32 threadIndex, dgFloat32 timestep, dgFloat32 maxAccNorm) const;



	//void IntegrateArray (const dgBodyInfo* bodyArray, dgInt32 count, dgFloat32 accelTolerance, dgFloat32 timestep, dgInt32 threadIndex, bool update) const
	void IntegrateArray (const dgIsland* const island, dgFloat32 accelTolerance, dgFloat32 timestep, dgInt32 threadIndex, bool update) const;
	dgInt32 GetJacobianDerivatives (const dgIsland* const island, dgInt32 threadIndex, bool bitMode, dgInt32 rowBase, dgInt32 rowCount, dgFloat32 timestep) const;	

	dgInt32 m_bodies;
	dgInt32 m_joints;
	dgInt32 m_islands;
	dgUnsigned32 m_markLru;

	mutable dgInt32 m_rowCountAtomicIndex;
	dgJacobianMemory m_solverMemory;
	

 //	dgParallelSolverBodyInertia m_parallelBodyInertiaMatrix[DG_MAX_THREADS_HIVE_COUNT];
//	dgParallelSolverUpdateVeloc m_parallelSolverUpdateVeloc[DG_MAX_THREADS_HIVE_COUNT];
//	dgParallelSolverUpdateForce m_parallelSolverUpdateForce[DG_MAX_THREADS_HIVE_COUNT];
//	dgParallelSolverCalculateForces m_parallelSolverCalculateForces[DG_MAX_THREADS_HIVE_COUNT];
//	dgParallelSolverInitInternalForces m_parallelInitIntenalForces[DG_MAX_THREADS_HIVE_COUNT];
//	dgParallelSolverBuildJacobianRows m_parallelSolverBuildJacobianRows[DG_MAX_THREADS_HIVE_COUNT];
//	dgParallelSolverJointAcceleration m_parallelSolverJointAcceleration[DG_MAX_THREADS_HIVE_COUNT];
//	dgParallelSolverInitFeedbackUpdate m_parallelSolverInitFeedbackUpdate[DG_MAX_THREADS_HIVE_COUNT];
//	dgParallelSolverBuildJacobianMatrix m_parallelSolverBuildJacobianMatrix[DG_MAX_THREADS_HIVE_COUNT];

	dgBody* m_sentinelBody;
	friend class dgWorld;
	friend class dgJacobianMemory;
	friend class dgSolverWorlkerThreads;
};


#define DG_CHECK_ACTIVE(body) _ASSERTE (((body->m_invMass.m_w == dgFloat32 (0.0f)) && !body->m_active) || (body->m_invMass.m_w > dgFloat32 (0.0f)))
#endif


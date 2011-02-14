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

#ifndef __AFX_BROADPHASE__INCLUDED__
#define __AFX_BROADPHASE__INCLUDED__

#include "dgPhysicsStdafx.h"
#include "dgBody.h"
#include "dgList.h"

typedef void (dgApi *OnBodiesInAABB) (dgBody* body, void* const userData);
typedef void (dgApi *OnLeavingWorldAction) (dgBody* body, dgInt32 threadIndex);


class dgBroadphaseSyncDescriptor;

DG_MSC_VECTOR_ALIGMENT
class dgBroadPhaseNode
{
	protected:
	DG_CLASS_ALLOCATOR(allocator)
	dgBroadPhaseNode (dgBroadPhaseNode* const sibling, dgBroadPhaseNode* const myNode);
	dgBroadPhaseNode (dgBroadPhaseNode* const parent, const dgVector& minBox, const dgVector& maxBox);
	
	~dgBroadPhaseNode ();

	void SetAABB (const dgVector& minBox, const dgVector& maxBox);

	dgVector m_minBox;
	dgVector m_maxBox;
	dgFloat32 m_surfaceArea;
	dgBroadPhaseNode* m_parent;
	dgBroadPhaseNode* m_left;
	dgBroadPhaseNode* m_right;
	dgList<dgBroadPhaseNode*>::dgListNode* m_fitnessNode;

#ifdef _DEBUG
	dgInt32 m_uniqueID;	
#endif

	friend dgBroadPhaseCollision;
} DG_GCC_VECTOR_ALIGMENT;

class dgBroadPhaseLeafNode: public dgBroadPhaseNode
{
	protected:
	DG_CLASS_ALLOCATOR(allocator)

	dgBroadPhaseLeafNode (dgBody* const body);
	~dgBroadPhaseLeafNode();

	dgBody* m_body;	
	dgInt32 m_active : 1;

	friend class dgBody;
	friend dgBroadPhaseCollision;
};



// this is an adaptive AABB tree used for  all hight level collision queries 
// it is not as tight as a static tree or a multi resolution grid, but it much friendlier to parallel executions
class dgBroadPhaseCollision
{
	public:
	void GetWorldSize (dgVector& p0, dgVector& p1) const;
	void SetWorldSize (const dgVector& min, const dgVector& max);
	void RayCast (const dgVector& p0, const dgVector& p1, OnRayCastAction filter, OnRayPrecastAction prefilter, void* const userData) const;
	dgInt32 ConvexCast (dgCollision* const shape, const dgMatrix& p0, const dgVector& p1, dgFloat32& timetoImpact, OnRayPrecastAction prefilter, void* const userData, dgConvexCastReturnInfo* const info, dgInt32 maxContacts, dgInt32 threadIndex) const;
	void ForEachBodyInAABB (const dgVector& q0, const dgVector& q1, OnBodiesInAABB callback, void* const userData) const;

	private:
	class dgFitnessList: public dgList <dgBroadPhaseNode*>
	{
		public:
		dgFitnessList (dgMemoryAllocator* const allocator)
			:dgList <dgBroadPhaseNode*>(allocator), m_current(NULL)
		{
		}

#ifdef _DEBUG
		dgFloat64 TotalCost () const
		{
			dgFloat64 cost = dgFloat32 (0.0f);
			for (dgListNode* node = GetFirst(); node; node = node->GetNext()) {
				dgBroadPhaseNode* const box = node->GetInfo();
				cost += box->m_surfaceArea;
			}
			return cost;
		}
#endif

		dgListNode* m_current;
	};

	dgBroadPhaseCollision(dgMemoryAllocator* const allocator);
	~dgBroadPhaseCollision();

	void Init();
	void Add (dgBody* const body);
	void Remove (dgBody* const body);
	void InvalidateCache ();
	void UpdateContacts (dgFloat32 tiemstep, bool collisionUpdate);
	void UpdateBodyBroadphase(dgBody* const body, dgInt32 threadIndex);

	private:
	void ImproveFitness();
	void ImproveNodeFitness (dgBroadPhaseNode* const node);
	dgBroadPhaseNode* InsertNode (dgBroadPhaseNode* const node);
	dgFloat32 CalculateSurfaceArea (const dgBroadPhaseNode* const node0, const dgBroadPhaseNode* const node1, dgVector& minBox, dgVector& maxBox) const;

	static void UpdateContactsDriver (void* const descriptor, dgInt32 threadID);

	void UpdateContactsBroadPhaseEnd ();
	void SubmitPairs (dgBroadPhaseLeafNode* const body, dgBroadPhaseNode* const node, dgInt32 threadID);
	void ApplyForceAndtorque (dgBroadphaseSyncDescriptor* const desctiptor, dgInt32 threadID);
	void FindCollidingPairs (dgBroadphaseSyncDescriptor* const desctiptor, dgInt32 threadID);
	void CalculatePairContacts (dgBroadphaseSyncDescriptor* const descriptor, dgInt32 threadID);
	void SubmitContactJoint (dgBroadphaseSyncDescriptor* const descriptor, dgInt32 threadID);


	dgVector m_appMinBox;
	dgVector m_appMaxBox;
	dgInt32 m_contactCount;
	dgBroadPhaseNode* m_rootNode;
	dgFitnessList m_fitness;
	dgArray<dgContactPoint> m_contactBuffer;
	dgInt32 m_updateLock;

	friend class dgBody;
	friend class dgWorld;
};
#endif

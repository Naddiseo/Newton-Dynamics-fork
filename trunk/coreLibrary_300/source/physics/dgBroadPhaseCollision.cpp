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
#include "dgCollisionConvex.h"
#include "dgWorldDynamicUpdate.h"
#include "dgBroadPhaseCollision.h"


#define DG_BROADPHASE_MAX_STACK_DEPTH	256
#define DG_BROADPHASE_AABB_SCALE		dgFloat32 (4.0f)
#define DG_BROADPHASE_AABB_INV_SCALE	(dgFloat32 (1.0f) / DG_BROADPHASE_AABB_SCALE)

class dgBroadphaseSyncDescriptor
{
	public:
	dgBroadphaseSyncDescriptor()
	{
		memset (this, 0, sizeof (dgBroadphaseSyncDescriptor));
	}

	dgInt32 m_lock;
	dgFloat32 m_timestep;
	dgWorld* m_world;

	dgInt32 m_collindPairSync;
	dgInt32 m_collindContactsSync;
	dgInt32 m_pairsAtomicCounter;
	dgInt32 m_jointsAtomicCounter;
	dgBodyMasterList::dgListNode* m_collindPairBodyNode;
	dgBodyMasterList::dgListNode* m_forceAndTorqueBodyNode;
};



#ifdef _DEBUG
static dgInt32 broadPhaseCounter = 0;
#endif

dgBroadPhaseNode::dgBroadPhaseNode (dgBroadPhaseNode* const parent, const dgVector& minBox, const dgVector& maxBox)
	:m_minBox (minBox)
	,m_maxBox(maxBox)
	,m_parent(parent)
	,m_left(NULL)
	,m_right(NULL)
	,m_fitnessNode(NULL) 
{
#ifdef _DEBUG
	m_uniqueID = broadPhaseCounter;	
	broadPhaseCounter ++;
#endif
}

dgBroadPhaseNode::dgBroadPhaseNode (dgBroadPhaseNode* const sibling, dgBroadPhaseNode* const myNode)
	:m_parent(sibling->m_parent)
	,m_left(sibling)
	,m_right(myNode)
	,m_fitnessNode(NULL)  
{
	#ifdef _DEBUG
		m_uniqueID = broadPhaseCounter;	
		broadPhaseCounter ++;
	#endif

	if (m_parent) {
		if (m_parent->m_left == sibling) {
			m_parent->m_left = this;
		} else {
			_ASSERTE (m_parent->m_right == sibling);
			m_parent->m_right = this;
		}
	}
	sibling->m_parent = this;
	myNode->m_parent = this;

//	UpdateAABBFromChidren ();

	dgBroadPhaseNode* const left = m_left;
	dgBroadPhaseNode* const right = m_right;

	m_minBox = dgVector (GetMin (left->m_minBox.m_x, right->m_minBox.m_x), GetMin (left->m_minBox.m_y, right->m_minBox.m_y), GetMin (left->m_minBox.m_z, right->m_minBox.m_z), dgFloat32 (0.0f));
	m_maxBox = dgVector (GetMax (left->m_maxBox.m_x, right->m_maxBox.m_x), GetMax (left->m_maxBox.m_y, right->m_maxBox.m_y), GetMax (left->m_maxBox.m_z, right->m_maxBox.m_z), dgFloat32 (0.0f));

	dgVector side0 (m_maxBox - m_minBox);
	dgVector side1 (side0.m_y, side0.m_z, side0.m_x, dgFloat32 (0.0f));
	m_surfaceArea = side0 % side1;
}


dgBroadPhaseNode::~dgBroadPhaseNode ()
{
	if (m_left) {
		delete m_left;
	}
	if (m_right) {
		delete m_right;
	}
}



void dgBroadPhaseNode::SetAABBSimd (const dgVector& minBox, const dgVector& maxBox)
{
	_ASSERTE (minBox.m_x <= maxBox.m_x);
	_ASSERTE (minBox.m_y <= maxBox.m_y);
	_ASSERTE (minBox.m_z <= maxBox.m_z);
	_ASSERTE (minBox.m_w == dgFloat32 (0.0f));
	_ASSERTE (maxBox.m_w == dgFloat32 (0.0f));

	simd_128 scale (DG_BROADPHASE_AABB_SCALE);
	simd_128 invScale (DG_BROADPHASE_AABB_INV_SCALE);

	simd_128 p0 ((simd_128&)minBox * scale);
	simd_128 p1 ((simd_128&)maxBox * scale);

	p0 = p0.Floor() * invScale; 
	p1 = (p1 + simd_128(dgFloat32(1.0f))).Floor() * invScale; 

	(simd_128&) m_minBox = p0;
	(simd_128&) m_maxBox = p1;
	m_minBox.m_w = dgFloat32 (0.0f);
	m_maxBox.m_w = dgFloat32 (0.0f);

	simd_128 size (p1 - p0);
	dgVector temp;
	size.StoreVector(&temp.m_x);
	simd_128 size1 (temp.m_y, temp.m_z, temp.m_x, dgFloat32 (0.0f));
	size.DotProduct (size1).StoreScalar(&m_surfaceArea);
}



void dgBroadPhaseNode::SetAABB (const dgVector& minBox, const dgVector& maxBox)
{
	_ASSERTE (minBox.m_x <= maxBox.m_x);
	_ASSERTE (minBox.m_y <= maxBox.m_y);
	_ASSERTE (minBox.m_z <= maxBox.m_z);

	dgVector p0 (minBox.CompProduct(dgVector (DG_BROADPHASE_AABB_SCALE, DG_BROADPHASE_AABB_SCALE, DG_BROADPHASE_AABB_SCALE, dgFloat32 (0.0f))));
	dgVector p1 (maxBox.CompProduct(dgVector (DG_BROADPHASE_AABB_SCALE, DG_BROADPHASE_AABB_SCALE, DG_BROADPHASE_AABB_SCALE, dgFloat32 (0.0f))));

	p0.m_x = dgFloor (p0.m_x) * DG_BROADPHASE_AABB_INV_SCALE; 
	p0.m_y = dgFloor (p0.m_y) * DG_BROADPHASE_AABB_INV_SCALE;  
	p0.m_z = dgFloor (p0.m_z) * DG_BROADPHASE_AABB_INV_SCALE;  

	p1.m_x = dgFloor (p1.m_x + dgFloat32 (1.0f)) * DG_BROADPHASE_AABB_INV_SCALE;  
	p1.m_y = dgFloor (p1.m_y + dgFloat32 (1.0f)) * DG_BROADPHASE_AABB_INV_SCALE;  
	p1.m_z = dgFloor (p1.m_z + dgFloat32 (1.0f)) * DG_BROADPHASE_AABB_INV_SCALE;  

	m_minBox = p0;
	m_maxBox = p1;
	m_minBox.m_w = dgFloat32 (0.0f);
	m_maxBox.m_w = dgFloat32 (0.0f);

	dgVector side0 (p1 - p0);
	dgVector side1 (side0.m_y, side0.m_z, side0.m_x, dgFloat32 (0.0f));
	m_surfaceArea = side0 % side1;
}

dgBroadPhaseLeafNode::dgBroadPhaseLeafNode (dgBody* const body)
	:dgBroadPhaseNode (NULL, body->m_minAABB, body->m_maxAABB), m_body (body), m_active(true)
{
	SetAABB(body->m_minAABB, body->m_maxAABB);
	body->m_collisionCell = this;
}

dgBroadPhaseLeafNode::~dgBroadPhaseLeafNode()
{
	_ASSERTE (m_body->m_collisionCell == this);

	m_body->m_collisionCell = NULL;
}


void dgBroadPhaseCollision::InvalidateCache ()
{
	dgTrace (("Invalidate cache\n"));
}


void dgBroadPhaseCollision::GetWorldSize (dgVector& p0, dgVector& p1) const
{
	_ASSERTE (0);
}

void dgBroadPhaseCollision::SetWorldSize (const dgVector& minBox, const dgVector& maxBox)
{
	m_appMinBox = minBox;
	m_appMaxBox = maxBox;
}

dgInt32 dgBroadPhaseCollision::ConvexCast (dgCollision* const shape, const dgMatrix& p0, const dgVector& p1, dgFloat32& timetoImpact, OnRayPrecastAction prefilter, void* const userData, dgConvexCastReturnInfo* const info, dgInt32 maxContacts, dgInt32 threadIndex) const
{
	_ASSERTE (0);
	return 0;
}

void dgBroadPhaseCollision::ForEachBodyInAABB (const dgVector& q0, const dgVector& q1, OnBodiesInAABB callback, void* const userData) const
{

	if (m_rootNode) {
		const dgBroadPhaseNode* stackPool[DG_BROADPHASE_MAX_STACK_DEPTH];
		dgInt32 stack = 1;
		stackPool[0] = m_rootNode;

		dgBody* const sentinel = ((dgWorld*)this)->GetSentinelBody();
		while (stack) {
			stack --;
			const dgBroadPhaseNode* const rootNode = stackPool[stack];
			if (dgOverlapTest (rootNode->m_minBox, rootNode->m_maxBox, q0, q1)) {

				if (!rootNode->m_left) {
					_ASSERTE (!rootNode->m_right);
					dgBody* const body = ((dgBroadPhaseLeafNode*) rootNode)->m_body;
					if (dgOverlapTest (body->m_minAABB, body->m_maxAABB, q0, q1)) {
						if (body != sentinel) {
							callback (body, userData);
						}
					}

				} else {
					stackPool[stack] = rootNode->m_left;
					stack ++;
					_ASSERTE (stack < sizeof (stackPool) / sizeof (stackPool[0]));

					stackPool[stack] = rootNode->m_right;
					stack ++;
					_ASSERTE (stack < sizeof (stackPool) / sizeof (stackPool[0]));
				}
			}
		}
	}
}


dgBroadPhaseCollision::dgBroadPhaseCollision(dgMemoryAllocator* const allocator)
	:m_appMinBox (-dgFloat32(1000.0f), -dgFloat32(1000.0f), -dgFloat32(1000.0f), dgFloat32(0.0f))
	,m_appMaxBox ( dgFloat32(1000.0f),  dgFloat32(1000.0f),  dgFloat32(1000.0f), dgFloat32(0.0f)) 
	,m_contactCount(0)
	,m_rootNode (NULL)
	,m_fitness(allocator)
	,m_contactBuffer (DG_MAX_CONTATCS * 256, allocator, 64)
	,m_updateLock(0)
{
}

dgBroadPhaseCollision::~dgBroadPhaseCollision()
{
	if (m_rootNode) {
		delete m_rootNode;
	}
}


void dgBroadPhaseCollision::Init()
{
}


dgFloat32 dgBroadPhaseCollision::CalculateSurfaceArea (const dgBroadPhaseNode* const node0, const dgBroadPhaseNode* const node1, dgVector& minBox, dgVector& maxBox) const
{
	minBox = dgVector (GetMin (node0->m_minBox.m_x, node1->m_minBox.m_x), GetMin (node0->m_minBox.m_y, node1->m_minBox.m_y), GetMin (node0->m_minBox.m_z, node1->m_minBox.m_z), dgFloat32 (0.0f));
	maxBox = dgVector (GetMax (node0->m_maxBox.m_x, node1->m_maxBox.m_x), GetMax (node0->m_maxBox.m_y, node1->m_maxBox.m_y), GetMax (node0->m_maxBox.m_z, node1->m_maxBox.m_z), dgFloat32 (0.0f));		
	dgVector side0 (maxBox - minBox);
	dgVector side1 (side0.m_y, side0.m_z, side0.m_x, dgFloat32 (0.0f));
	return side0 % side1;
}

dgFloat32 dgBroadPhaseCollision::CalculateSurfaceAreaSimd (const dgBroadPhaseNode* const node0, const dgBroadPhaseNode* const node1, dgVector& minBox, dgVector& maxBox) const
{
	(simd_128&) minBox = ((simd_128&)node0->m_minBox).GetMin((simd_128&)node1->m_minBox);
	(simd_128&) maxBox = ((simd_128&)node0->m_maxBox).GetMax((simd_128&)node1->m_maxBox);

	simd_128 size (((simd_128&) maxBox) - ((simd_128&)minBox));
	dgVector temp;
	size.StoreVector(&temp.m_x);
	simd_128 size1 (temp.m_y, temp.m_z, temp.m_x, dgFloat32 (0.0f));
	dgFloat32 dot;
	size.DotProduct (size1).StoreScalar(&dot);
	return dot;
}


dgBroadPhaseNode* dgBroadPhaseCollision::InsertNode (dgBroadPhaseNode* const node)
{
	dgVector p0;
	dgVector p1;

	dgBroadPhaseNode* sibling = m_rootNode;
	dgFloat32 surfaceArea = CalculateSurfaceArea (node, sibling, p0, p1);
	while(sibling->m_left && sibling->m_right) {
		if (surfaceArea > sibling->m_surfaceArea) {
			break;
		} 

		sibling->m_minBox = p0;
		sibling->m_maxBox = p1;
		sibling->m_surfaceArea = surfaceArea;

		dgVector leftP0;
		dgVector leftP1;		
		dgFloat32 leftSurfaceArea = CalculateSurfaceArea (node, sibling->m_left, leftP0, leftP1);

		dgVector rightP0;
		dgVector rightP1;		
		dgFloat32 rightSurfaceArea = CalculateSurfaceArea (node, sibling->m_right, rightP0, rightP1);

		if (leftSurfaceArea < rightSurfaceArea) {
			sibling = sibling->m_left;
			p0 = leftP0;
			p1 = leftP1;
			surfaceArea = leftSurfaceArea;
		} else {
			sibling = sibling->m_right;
			p0 = rightP0;
			p1 = rightP1;
			surfaceArea = rightSurfaceArea;
		}
	} 

	const dgWorld* const me = (dgWorld*) this;
	dgBroadPhaseNode* const parent = new (me->GetAllocator()) dgBroadPhaseNode (sibling, node);
	parent->m_fitnessNode = m_fitness.Append (parent);

	return parent;
}

void dgBroadPhaseCollision::Add (dgBody* const body)
{
	// create a new leaf node;
	dgWorld* const me = (dgWorld*) this;
	dgBroadPhaseLeafNode* const newNode = new (me->GetAllocator()) dgBroadPhaseLeafNode (body);

	if (!m_rootNode) {
		m_rootNode = newNode;
	} else {
		dgBroadPhaseNode* const node = InsertNode(newNode);

		if (!node->m_parent) {
			m_rootNode = node;
		}
	}
}


void dgBroadPhaseCollision::Remove (dgBody* const body)
{
	dgBroadPhaseLeafNode* const node = body->m_collisionCell;

	_ASSERTE (!node->m_fitnessNode);

	if (node->m_parent) {
		dgBroadPhaseNode* const grandParent = node->m_parent->m_parent;
		if (grandParent) {
			if (grandParent->m_left == node->m_parent) {
				if (node->m_parent->m_right == node) {
					grandParent->m_left = node->m_parent->m_left;
					node->m_parent->m_left->m_parent = grandParent;
					node->m_parent->m_left = NULL;
					node->m_parent->m_parent = NULL;
				} else {
					grandParent->m_left = node->m_parent->m_right;
					node->m_parent->m_right->m_parent = grandParent;
					node->m_parent->m_right = NULL;
					node->m_parent->m_parent = NULL;
				}
			} else {
				if (node->m_parent->m_right == node) {
					grandParent->m_right = node->m_parent->m_left;
					node->m_parent->m_left->m_parent = grandParent;
					node->m_parent->m_left = NULL;
					node->m_parent->m_parent = NULL;
				} else {
					grandParent->m_right = node->m_parent->m_right;
					node->m_parent->m_right->m_parent = grandParent;
					node->m_parent->m_right = NULL;
					node->m_parent->m_parent = NULL;
				}
			}
		} else {
			if (node->m_parent->m_right == node) {
				m_rootNode = node->m_parent->m_left;
				m_rootNode->m_parent = NULL;
				node->m_parent->m_left = NULL;
			} else {
				m_rootNode = node->m_parent->m_right;
				m_rootNode->m_parent = NULL;
				node->m_parent->m_right = NULL;
			}
		}

		_ASSERTE (node->m_parent->m_fitnessNode);
		m_fitness.Remove(node->m_parent->m_fitnessNode);
		if (node->m_parent->m_fitnessNode == m_fitness.m_current) {
			m_fitness.m_current = NULL;
		}

		delete node->m_parent;
	} else {
		delete node;
		m_rootNode = NULL;
	}
}


void dgBroadPhaseCollision::ImproveNodeFitnessSimd (dgBroadPhaseNode* const node)
{
	_ASSERTE (node->m_left);
	_ASSERTE (node->m_right);

	if (node->m_parent)	{
		if (node->m_parent->m_left == node) {
			dgFloat32 cost0 = node->m_surfaceArea;

			dgVector cost1P0;
			dgVector cost1P1;		
			dgFloat32 cost1 = CalculateSurfaceAreaSimd (node->m_right, node->m_parent->m_right, cost1P0, cost1P1);

			dgVector cost2P0;
			dgVector cost2P1;		
			dgFloat32 cost2 = CalculateSurfaceAreaSimd (node->m_left, node->m_parent->m_right, cost2P0, cost2P1);

			if ((cost1 <= cost0) && (cost1 <= cost2)) {
				dgBroadPhaseNode* const parent = node->m_parent;
				node->m_minBox = parent->m_minBox;
				node->m_maxBox = parent->m_maxBox;
				node->m_surfaceArea = parent->m_surfaceArea; 
				if (parent->m_parent) {
					if (parent->m_parent->m_left == parent) {
						parent->m_parent->m_left = node;
					} else {
						_ASSERTE (parent->m_parent->m_right == parent);
						parent->m_parent->m_right = node;
					}
				} else {
					m_rootNode = node;
				}
				node->m_parent = parent->m_parent;
				parent->m_parent = node;
				node->m_right->m_parent = parent;
				parent->m_left = node->m_right;
				node->m_right = parent;
				parent->m_minBox = cost1P0;
				parent->m_maxBox = cost1P1;		
				parent->m_surfaceArea = cost1;


			} else if ((cost2 <= cost0) && (cost2 <= cost1)) {
				dgBroadPhaseNode* const parent = node->m_parent;
				node->m_minBox = parent->m_minBox;
				node->m_maxBox = parent->m_maxBox;
				node->m_surfaceArea = parent->m_surfaceArea; 

				if (parent->m_parent) {
					if (parent->m_parent->m_left == parent) {
						parent->m_parent->m_left = node;
					} else {
						_ASSERTE (parent->m_parent->m_right == parent);
						parent->m_parent->m_right = node;
					}
				} else {
					m_rootNode = node;
				}
				node->m_parent = parent->m_parent;
				parent->m_parent = node;
				node->m_left->m_parent = parent;
				parent->m_left = node->m_left;
				node->m_left = parent;

				parent->m_minBox = cost2P0;
				parent->m_maxBox = cost2P1;		
				parent->m_surfaceArea = cost2;
			}
		} else {
			dgFloat32 cost0 = node->m_surfaceArea;

			dgVector cost1P0;
			dgVector cost1P1;		
			dgFloat32 cost1 = CalculateSurfaceAreaSimd (node->m_left, node->m_parent->m_left, cost1P0, cost1P1);

			dgVector cost2P0;
			dgVector cost2P1;		
			dgFloat32 cost2 = CalculateSurfaceAreaSimd (node->m_right, node->m_parent->m_left, cost2P0, cost2P1);


			if ((cost1 <= cost0) && (cost1 <= cost2)) {

				dgBroadPhaseNode* const parent = node->m_parent;
				node->m_minBox = parent->m_minBox;
				node->m_maxBox = parent->m_maxBox;
				node->m_surfaceArea = parent->m_surfaceArea; 
				if (parent->m_parent) {
					if (parent->m_parent->m_left == parent) {
						parent->m_parent->m_left = node;
					} else {
						_ASSERTE (parent->m_parent->m_right == parent);
						parent->m_parent->m_right = node;
					}
				} else {
					m_rootNode = node;
				}
				node->m_parent = parent->m_parent;
				parent->m_parent = node;
				node->m_left->m_parent = parent;
				parent->m_right = node->m_left;
				node->m_left = parent;

				parent->m_minBox = cost1P0;
				parent->m_maxBox = cost1P1;		
				parent->m_surfaceArea = cost1;

			} else if ((cost2 <= cost0) && (cost2 <= cost1)) {
				dgBroadPhaseNode* const parent = node->m_parent;
				node->m_minBox = parent->m_minBox;
				node->m_maxBox = parent->m_maxBox;
				node->m_surfaceArea = parent->m_surfaceArea; 
				if (parent->m_parent) {
					if (parent->m_parent->m_left == parent) {
						parent->m_parent->m_left = node;
					} else {
						_ASSERTE (parent->m_parent->m_right == parent);
						parent->m_parent->m_right = node;
					}
				} else {
					m_rootNode = node;
				}
				node->m_parent = parent->m_parent;
				parent->m_parent = node;
				node->m_right->m_parent = parent;
				parent->m_right = node->m_right;
				node->m_right = parent;

				parent->m_minBox = cost2P0;
				parent->m_maxBox = cost2P1;		
				parent->m_surfaceArea = cost2;
			}
		}
	}
	_ASSERTE (!m_rootNode->m_parent);
}


void dgBroadPhaseCollision::ImproveNodeFitness (dgBroadPhaseNode* const node)
{
	_ASSERTE (node->m_left);
	_ASSERTE (node->m_right);

	if (node->m_parent)	{
		if (node->m_parent->m_left == node) {
			dgFloat32 cost0 = node->m_surfaceArea;

			dgVector cost1P0;
			dgVector cost1P1;		
			dgFloat32 cost1 = CalculateSurfaceArea (node->m_right, node->m_parent->m_right, cost1P0, cost1P1);

			dgVector cost2P0;
			dgVector cost2P1;		
			dgFloat32 cost2 = CalculateSurfaceArea (node->m_left, node->m_parent->m_right, cost2P0, cost2P1);

			if ((cost1 <= cost0) && (cost1 <= cost2)) {
				dgBroadPhaseNode* const parent = node->m_parent;
				node->m_minBox = parent->m_minBox;
				node->m_maxBox = parent->m_maxBox;
				node->m_surfaceArea = parent->m_surfaceArea; 
				if (parent->m_parent) {
					if (parent->m_parent->m_left == parent) {
						parent->m_parent->m_left = node;
					} else {
						_ASSERTE (parent->m_parent->m_right == parent);
						parent->m_parent->m_right = node;
					}
				} else {
					m_rootNode = node;
				}
				node->m_parent = parent->m_parent;
				parent->m_parent = node;
				node->m_right->m_parent = parent;
				parent->m_left = node->m_right;
				node->m_right = parent;
				parent->m_minBox = cost1P0;
				parent->m_maxBox = cost1P1;		
				parent->m_surfaceArea = cost1;


			} else if ((cost2 <= cost0) && (cost2 <= cost1)) {
				dgBroadPhaseNode* const parent = node->m_parent;
				node->m_minBox = parent->m_minBox;
				node->m_maxBox = parent->m_maxBox;
				node->m_surfaceArea = parent->m_surfaceArea; 

				if (parent->m_parent) {
					if (parent->m_parent->m_left == parent) {
						parent->m_parent->m_left = node;
					} else {
						_ASSERTE (parent->m_parent->m_right == parent);
						parent->m_parent->m_right = node;
					}
				} else {
					m_rootNode = node;
				}
				node->m_parent = parent->m_parent;
				parent->m_parent = node;
				node->m_left->m_parent = parent;
				parent->m_left = node->m_left;
				node->m_left = parent;

				parent->m_minBox = cost2P0;
				parent->m_maxBox = cost2P1;		
				parent->m_surfaceArea = cost2;
			}
		} else {
			dgFloat32 cost0 = node->m_surfaceArea;

			dgVector cost1P0;
			dgVector cost1P1;		
			dgFloat32 cost1 = CalculateSurfaceArea (node->m_left, node->m_parent->m_left, cost1P0, cost1P1);

			dgVector cost2P0;
			dgVector cost2P1;		
			dgFloat32 cost2 = CalculateSurfaceArea (node->m_right, node->m_parent->m_left, cost2P0, cost2P1);


			if ((cost1 <= cost0) && (cost1 <= cost2)) {

				dgBroadPhaseNode* const parent = node->m_parent;
				node->m_minBox = parent->m_minBox;
				node->m_maxBox = parent->m_maxBox;
				node->m_surfaceArea = parent->m_surfaceArea; 
				if (parent->m_parent) {
					if (parent->m_parent->m_left == parent) {
						parent->m_parent->m_left = node;
					} else {
						_ASSERTE (parent->m_parent->m_right == parent);
						parent->m_parent->m_right = node;
					}
				} else {
					m_rootNode = node;
				}
				node->m_parent = parent->m_parent;
				parent->m_parent = node;
				node->m_left->m_parent = parent;
				parent->m_right = node->m_left;
				node->m_left = parent;

				parent->m_minBox = cost1P0;
				parent->m_maxBox = cost1P1;		
				parent->m_surfaceArea = cost1;

			} else if ((cost2 <= cost0) && (cost2 <= cost1)) {
				dgBroadPhaseNode* const parent = node->m_parent;
				node->m_minBox = parent->m_minBox;
				node->m_maxBox = parent->m_maxBox;
				node->m_surfaceArea = parent->m_surfaceArea; 
				if (parent->m_parent) {
					if (parent->m_parent->m_left == parent) {
						parent->m_parent->m_left = node;
					} else {
						_ASSERTE (parent->m_parent->m_right == parent);
						parent->m_parent->m_right = node;
					}
				} else {
					m_rootNode = node;
				}
				node->m_parent = parent->m_parent;
				parent->m_parent = node;
				node->m_right->m_parent = parent;
				parent->m_right = node->m_right;
				node->m_right = parent;

				parent->m_minBox = cost2P0;
				parent->m_maxBox = cost2P1;		
				parent->m_surfaceArea = cost2;
			}
		}
	}
	_ASSERTE (!m_rootNode->m_parent);
}


void dgBroadPhaseCollision::ImproveFitness()
{
	dgInt32 count = m_fitness.GetCount() >> 2;

#ifdef _DEBUG 
	dgFloat64 cost0 = m_fitness.TotalCost ();
#endif

	dgWorld* const world = (dgWorld*) this;
	if (count) {
		dgFitnessList::dgListNode* node = m_fitness.m_current;
		if (world->m_cpu == dgSimdPresent) {
			for (dgInt32 i = 0; i < count; i ++) {
				if (node == NULL) {
					node = m_fitness.GetFirst();
				}
				ImproveNodeFitnessSimd (node->GetInfo());
				node = node->GetNext();
			}
		} else {
			for (dgInt32 i = 0; i < count; i ++) {
				if (node == NULL) {
					node = m_fitness.GetFirst();
				}
				ImproveNodeFitness (node->GetInfo());
				node = node->GetNext();
			}
		}
		m_fitness.m_current = node;
	} else {
		if (world->m_cpu == dgSimdPresent) {
			for (dgFitnessList::dgListNode* node = m_fitness.GetFirst(); node; node = node->GetNext()) {
				ImproveNodeFitnessSimd (node->GetInfo());
			}
		} else {
			for (dgFitnessList::dgListNode* node = m_fitness.GetFirst(); node; node = node->GetNext()) {
				ImproveNodeFitness (node->GetInfo());
			}
		}
	}

#ifdef _DEBUG 
	dgFloat64 cost1 = m_fitness.TotalCost ();
	_ASSERTE (cost1 <= cost0);
#endif
}



void dgBroadPhaseCollision::SubmitPairs (dgBroadPhaseLeafNode* const bodyNode, dgBroadPhaseNode* const node, dgInt32 threadID)
{
	dgBroadPhaseNode* pool[DG_BROADPHASE_MAX_STACK_DEPTH];
	pool[0] = node;
	dgInt32 stack = 1;

	dgBody* const body0 = bodyNode->m_body;
	_ASSERTE (!body0->m_collision->IsType (dgCollision::dgCollisionNull_RTTI));

	dgWorld* const world = (dgWorld*) this;
	dgCollidingPairCollector* const contactPairs = (dgWorld*)this;

	if (world->m_cpu == dgSimdPresent) {
		while (stack) {
			stack --;
			dgBroadPhaseNode* const rootNode = pool[stack];
			if (dgOverlapTestSimd (rootNode->m_minBox, rootNode->m_maxBox, body0->m_minAABB, body0->m_maxAABB)) {
				if (!rootNode->m_left) {
					_ASSERTE (!rootNode->m_right);
					dgBody* const body1 = ((dgBroadPhaseLeafNode*) rootNode)->m_body;
					if (dgOverlapTestSimd(body1->m_minAABB, body1->m_maxAABB, body0->m_minAABB, body0->m_maxAABB)) {
						if (!body1->m_collision->IsType (dgCollision::dgCollisionNull_RTTI)) {
							contactPairs->AddPair(body0, body1, threadID);
						}
					}
				} else {
					pool[stack] = rootNode->m_left;
					stack ++;
					_ASSERTE (stack < sizeof (pool) / sizeof (pool[0]));

					pool[stack] = rootNode->m_right;
					stack ++;
					_ASSERTE (stack < sizeof (pool) / sizeof (pool[0]));
				}
			}
		}
	} else {

		while (stack) {
			stack --;
			dgBroadPhaseNode* const rootNode = pool[stack];
			if (dgOverlapTest (rootNode->m_minBox, rootNode->m_maxBox, body0->m_minAABB, body0->m_maxAABB)) {
				if (!rootNode->m_left) {
					_ASSERTE (!rootNode->m_right);
					dgBody* const body1 = ((dgBroadPhaseLeafNode*) rootNode)->m_body;
					if (dgOverlapTest(body1->m_minAABB, body1->m_maxAABB, body0->m_minAABB, body0->m_maxAABB)) {
						if (!body1->m_collision->IsType (dgCollision::dgCollisionNull_RTTI)) {
							contactPairs->AddPair(body0, body1, threadID);
						}
					}
				} else {
					pool[stack] = rootNode->m_left;
					stack ++;
					_ASSERTE (stack < sizeof (pool) / sizeof (pool[0]));

					pool[stack] = rootNode->m_right;
					stack ++;
					_ASSERTE (stack < sizeof (pool) / sizeof (pool[0]));
				}
			}
		}
	}
}

void dgBroadPhaseCollision::UpdateContactsBroadPhaseEnd ()
{
	// delete all non used contacts
	dgInt32 count = 0;
	dgWorld* const world = (dgWorld*) this;
	dgUnsigned32 lru = world->m_broadPhaseLru;

	dgCollidingPairCollector& contactPair = *world;
	dgContact** const deadContacs = (dgContact**) contactPair.m_pairs;
	dgActiveContacts* const contactList = (dgActiveContacts*) world;

	for (dgActiveContacts::dgListNode* contactNode = contactList->GetFirst(); contactNode; contactNode = contactNode->GetNext()) {
		dgContact* const contact = contactNode->GetInfo();
		if ((contact->m_broadphaseLru != lru) || (contact->GetCount() == 0)) {
			// note this is in observation (to prevent bodies from not going to sleep  inside triggers	
			//if (! (contact->m_body0->m_sleeping & contact->m_body1->m_sleeping) ) {
			const dgBody* const body0 = contact->m_body0;
			const dgBody* const body1 = contact->m_body1;
			if (! ((body0->m_sleeping | body0->m_equilibrium) & (body1->m_sleeping | body1->m_equilibrium)) ) {
				deadContacs[count] = contact;
				count ++;
			}
		}
	}

	for (dgInt32 i = 0; i < count; i ++) {
		dgContact* const contact = deadContacs[i];
		world->DestroyConstraint (contact);
	}
}



void dgBroadPhaseCollision::UpdateContactsDriver (void* const context, dgInt32 threadID)
{
	dgBroadphaseSyncDescriptor* const descriptor = (dgBroadphaseSyncDescriptor*) context;

	dgWorld* const world = descriptor->m_world;

	if (!threadID) {
		dgUnsigned32 ticks0 = world->m_getPerformanceCount();
		world->ApplyForceAndtorque (descriptor, threadID);
		dgUnsigned32 ticks1 = world->m_getPerformanceCount();
		world->m_perfomanceCounters[m_forceCallback] = ticks1 - ticks0;

		ticks0 = ticks1;
		world->FindCollidingPairs (descriptor, threadID);
		ticks1 = world->m_getPerformanceCount();
		world->m_perfomanceCounters[m_broadPhaceTicks] = ticks1 - ticks0;

		ticks0 = ticks1;
		world->CalculatePairContacts (descriptor, threadID);
		world->SubmitContactJoint (descriptor, threadID);
		ticks1 = world->m_getPerformanceCount();
		world->m_perfomanceCounters[m_narrowPhaseTicks] = ticks1 - ticks0;

	} else {
		world->ApplyForceAndtorque (descriptor, threadID);
		world->FindCollidingPairs (descriptor, threadID);
		world->CalculatePairContacts (descriptor, threadID);
		world->SubmitContactJoint (descriptor, threadID);
	}
}

void dgBroadPhaseCollision::ApplyForceAndtorque (dgBroadphaseSyncDescriptor* const descriptor, dgInt32 threadID)
{
	dgWorld* const world = (dgWorld*) this;
	_ASSERTE (world == descriptor->m_world);

	dgFloat32 timeStep = descriptor->m_timestep; 

	bool simd = (world->m_cpu == dgSimdPresent);
	world->GetIndirectLock (&descriptor->m_lock);
	dgBodyMasterList::dgListNode* node = descriptor->m_forceAndTorqueBodyNode;
	if (node) {
		descriptor->m_forceAndTorqueBodyNode = node->GetNext();
	}
	world->ReleaseIndirectLock (&descriptor->m_lock);

	for ( ; node; ) {
		dgBody* const body = node->GetInfo().GetBody();
		if (body->m_invMass.m_w == dgFloat32(0.0f)) {

			body->m_sleeping = true;
			body->m_autoSleep = true;
			body->m_equilibrium = true;
			body->m_solverInContinueCollision = false;

		} else {
			body->m_solverInContinueCollision = false;
			_ASSERTE (body->m_invMass.m_w > dgFloat32(0.0f));
			_ASSERTE (body->m_collision->IsType(dgCollision::dgConvexCollision_RTTI) || body->m_collision->IsType(dgCollision::dgCollisionCompound_RTTI) ||	body->m_collision->IsType(dgCollision::dgCollisionConvexModifier_RTTI));

			body->ApplyExtenalForces (timeStep, threadID);
			if (!body->IsInEquelibrium()) {
				body->m_sleeping = false;
				body->m_equilibrium = false;
				if (simd) {
					body->UpdateCollisionMatrixSimd (timeStep, threadID);
				} else {
					body->UpdateCollisionMatrix (timeStep, threadID);
				}
			}
			body->m_prevExternalForce = body->m_accel;
			body->m_prevExternalTorque = body->m_alpha;
		}

		world->GetIndirectLock (&descriptor->m_lock);
		node = descriptor->m_forceAndTorqueBodyNode;
		if (node) {
			descriptor->m_forceAndTorqueBodyNode = node->GetNext();
		}
		world->ReleaseIndirectLock (&descriptor->m_lock);
	}
}



void dgBroadPhaseCollision::FindCollidingPairs (dgBroadphaseSyncDescriptor* const descriptor, dgInt32 threadID)
{
	dgWorld* const world = (dgWorld*) this;
	_ASSERTE (world == descriptor->m_world);

	world->GetIndirectLock (&descriptor->m_lock);
	dgBodyMasterList::dgListNode* node = descriptor->m_collindPairBodyNode;
	if (node) {
		descriptor->m_collindPairBodyNode = node->GetNext();
	}
	world->ReleaseIndirectLock (&descriptor->m_lock);


	for ( ;node; ) {
		dgBody* const body = node->GetInfo().GetBody();
		if (body->m_collisionCell) {
			if (!body->m_collision->IsType (dgCollision::dgCollisionNull_RTTI)) {
				dgBroadPhaseLeafNode* const bodyNode = body->m_collisionCell;
				for (dgBroadPhaseNode* ptr = bodyNode; ptr->m_parent; ptr = ptr->m_parent) {
					dgBroadPhaseNode* const sibling = ptr->m_parent->m_right;
					if (sibling != ptr) {
						SubmitPairs (bodyNode, sibling, threadID);
					}
				}
			}
		}

		world->GetIndirectLock (&descriptor->m_lock);
		node = descriptor->m_collindPairBodyNode;
		if (node) {
			descriptor->m_collindPairBodyNode = node->GetNext();
		}
		world->ReleaseIndirectLock (&descriptor->m_lock);
	}
	world->SyncThreads(&descriptor->m_collindPairSync);
}


void dgBroadPhaseCollision::CalculatePairContacts (dgBroadphaseSyncDescriptor* const descriptor, dgInt32 threadID)
{
	dgContactPoint contacts[DG_MAX_CONTATCS];
	dgWorld* const world = (dgWorld*) this;
	dgFloat32 timestep = descriptor->m_timestep;
	dgCollidingPairCollector* const pairCollector = world;
	dgCollidingPairCollector::dgPair* const pairs = pairCollector->m_pairs;
	dgInt32 count = pairCollector->m_count;
	if (world->m_cpu == dgSimdPresent) {
		for (dgInt32 i = dgAtomicAdd(&descriptor->m_pairsAtomicCounter, 1); i < count; i = dgAtomicAdd(&descriptor->m_pairsAtomicCounter, 1)) {
			dgCollidingPairCollector::dgPair* const pair = &pairs[i];
			pair->m_contactBufferIndex = -1;

			world->CalculateContactsSimd (pair, contacts, timestep, threadID);

			world->GetIndirectLock (&descriptor->m_lock);
			m_contactBuffer.ExpandCapacityIfNeessesary (m_contactCount + pair->m_contactCount, 1);
			_ASSERTE ((m_contactCount + pair->m_contactCount) <= m_contactBuffer.GetElementsCapacity());
			if (pair->m_contactCount) {
				pair->m_contactBufferIndex = m_contactCount;
				memcpy (&m_contactBuffer[m_contactCount], contacts, pair->m_contactCount * sizeof (dgContactPoint));
				m_contactCount += pair->m_contactCount;
			}
			world->ReleaseIndirectLock (&descriptor->m_lock);
		}
	} else {

		for (dgInt32 i = dgAtomicAdd(&descriptor->m_pairsAtomicCounter, 1); i < count; i = dgAtomicAdd(&descriptor->m_pairsAtomicCounter, 1)) {
			dgCollidingPairCollector::dgPair* const pair = &pairs[i];
			pair->m_contactBufferIndex = -1;

			world->CalculateContacts (pair, contacts, timestep, threadID);

			world->GetIndirectLock (&descriptor->m_lock);
			m_contactBuffer.ExpandCapacityIfNeessesary (m_contactCount + pair->m_contactCount, 1);
			_ASSERTE ((m_contactCount + pair->m_contactCount) <= m_contactBuffer.GetElementsCapacity());
			if (pair->m_contactCount) {
				pair->m_contactBufferIndex = m_contactCount;
				memcpy (&m_contactBuffer[m_contactCount], contacts, pair->m_contactCount * sizeof (dgContactPoint));
				m_contactCount += pair->m_contactCount;
			}
			world->ReleaseIndirectLock (&descriptor->m_lock);
		}
	}
	world->SyncThreads(&descriptor->m_collindContactsSync);
}


void dgBroadPhaseCollision::SubmitContactJoint (dgBroadphaseSyncDescriptor* const descriptor, dgInt32 threadID)
{
	dgWorld* const world = (dgWorld*) this;
	dgFloat32 timestep = descriptor->m_timestep;

	dgCollidingPairCollector* const pairCollector = world;
	dgCollidingPairCollector::dgPair* const pairs = pairCollector->m_pairs;
	dgInt32 count = pairCollector->m_count;
	for (dgInt32 i = dgAtomicAdd(&descriptor->m_jointsAtomicCounter, 1); i < count; i = dgAtomicAdd(&descriptor->m_jointsAtomicCounter, 1)) {

		dgCollidingPairCollector::dgPair* const pair = &pairs[i];

		if (pair->m_contactCount || pair->m_isTrigger) {
			_ASSERTE (pair->m_contactCount <= (DG_CONSTRAINT_MAX_ROWS / 3));
			if (pair->m_isTrigger) {
				world->ProcessTriggers (pair, timestep, threadID);
			} else {
				world->ProcessContacts (pair, timestep, threadID);
			}

		} else if (pair->m_contact) {
			if (pair->m_contactBufferIndex >= 0) {
				world->ProcessCachedContacts (pair->m_contact, pair->m_material, timestep, threadID);
			} else {
				pair->m_contact->m_maxDOF = 0;
			}
		}
	}
}


void dgBroadPhaseCollision::RayCast (const dgVector& l0, const dgVector& l1, OnRayCastAction filter, OnRayPrecastAction prefilter, void* const userData) const
{
	if (filter && m_rootNode) {
		dgVector segment (l1 - l0);
		dgFloat32 dist2 = segment % segment;
		if (dist2 > dgFloat32 (1.0e-8f)) {
			const dgBroadPhaseNode* stackPool[DG_BROADPHASE_MAX_STACK_DEPTH];
			dgInt32 stack = 1;
			stackPool[0] = m_rootNode;
			FastRayTest ray (l0, l1);
			dgFloat32 maxParam = dgFloat32 (1.2f);
			const dgWorld* const world = (dgWorld*) this;

			if (world->m_cpu == dgSimdPresent) {
				dgLineBox line;	

				line.m_l0 = l0;
				line.m_l1 = l1;
				line.m_boxL0 = dgVector ((((simd_128&)l0).GetMin((simd_128&)l1)).m_type);
				line.m_boxL1 = dgVector ((((simd_128&)l0).GetMax((simd_128&)l1)).m_type);

				while (stack) {
					stack --;
					const dgBroadPhaseNode* const node = stackPool[stack];

					if (ray.BoxTestSimd(node->m_minBox, node->m_maxBox)) {

						if (!node->m_left) {
							_ASSERTE (!node->m_right);
							const dgBroadPhaseLeafNode* const leafNode = (dgBroadPhaseLeafNode*) node;
							dgFloat32 param = leafNode->m_body->RayCastSimd(line, filter, prefilter, userData, maxParam);
							if (param < maxParam) {
								maxParam = param;
								ray.Reset (maxParam);
							}
						} else {
							_ASSERTE (node->m_left);
							_ASSERTE (stack < sizeof (stackPool) / sizeof (dgBroadPhaseNode*));
							stackPool[stack] = node->m_left;
							stack++;

							_ASSERTE (node->m_right);
							_ASSERTE (stack < sizeof (stackPool) / sizeof (dgBroadPhaseNode*));
							stackPool[stack] = node->m_right;
							stack++;
						}
					}
				}

			} else {
				dgLineBox line;	

				line.m_l0 = l0;
				line.m_l1 = l1;
				if (line.m_l0.m_x <= line.m_l1.m_x) {
					line.m_boxL0.m_x = line.m_l0.m_x;
					line.m_boxL1.m_x = line.m_l1.m_x;
				} else {
					line.m_boxL0.m_x = line.m_l1.m_x;
					line.m_boxL1.m_x = line.m_l0.m_x;
				}

				if (line.m_l0.m_y <= line.m_l1.m_y) {
					line.m_boxL0.m_y = line.m_l0.m_y;
					line.m_boxL1.m_y = line.m_l1.m_y;
				} else {
					line.m_boxL0.m_y = line.m_l1.m_y;
					line.m_boxL1.m_y = line.m_l0.m_y;
				}

				if (line.m_l0.m_z <= line.m_l1.m_z) {
					line.m_boxL0.m_z = line.m_l0.m_z;
					line.m_boxL1.m_z = line.m_l1.m_z;
				} else {
					line.m_boxL0.m_z = line.m_l1.m_z;
					line.m_boxL1.m_z = line.m_l0.m_z;
				}

				while (stack) {

					stack --;
					const dgBroadPhaseNode* const node = stackPool[stack];

					if (ray.BoxTest (node->m_minBox, node->m_maxBox)) {

						if (!node->m_left) {
							_ASSERTE (!node->m_right);
							const dgBroadPhaseLeafNode* const leafNode = (dgBroadPhaseLeafNode*) node;
							dgFloat32 param = leafNode->m_body->RayCast (line, filter, prefilter, userData, maxParam);
							if (param < maxParam) {
								maxParam = param;
								ray.Reset (maxParam);
							}
						} else {
							_ASSERTE (node->m_left);
							_ASSERTE (stack < sizeof (stackPool) / sizeof (dgBroadPhaseNode*));
							stackPool[stack] = node->m_left;
							stack++;

							_ASSERTE (node->m_right);
							_ASSERTE (stack < sizeof (stackPool) / sizeof (dgBroadPhaseNode*));
							stackPool[stack] = node->m_right;
							stack++;
						}
					}
				}
			}
		}
	}
}


void dgBroadPhaseCollision::UpdateBodyBroadphaseSimd(dgBody* const body, dgInt32 threadIndex)
{
	if (m_rootNode) {

		// if the body is move inside the world, then active it
		if (!body->m_isInWorld) {
			if (dgOverlapTestSimd(body->m_minAABB, body->m_maxAABB, m_appMinBox, m_appMaxBox)) {
				body->m_isInWorld = true;
				body->m_sleeping = false;
				body->m_equilibrium = false;
			}
		}

		// update bodies only if they are in the world
		if (body->m_isInWorld) {
			dgBroadPhaseLeafNode* const node = body->m_collisionCell;
			_ASSERTE (!node->m_left);
			_ASSERTE (!node->m_right);

			if (!dgBoxInclusionTestSimd (body->m_minAABB, body->m_maxAABB, node->m_minBox, node->m_maxBox)) {
				node->SetAABBSimd(body->m_minAABB, body->m_maxAABB);
				dgWorld* const world = (dgWorld*) this;
				for (dgBroadPhaseNode* parent = node->m_parent; parent; parent = parent->m_parent) {
					dgVector minBox;
					dgVector maxBox;
					dgFloat32 area = CalculateSurfaceAreaSimd (parent->m_left, parent->m_right, minBox, maxBox);
					if (dgBoxInclusionTestSimd(minBox, maxBox, parent->m_minBox, parent->m_maxBox)) {
						break;
					}

					world->GetIndirectLock (&m_updateLock);
					parent->m_minBox = minBox;
					parent->m_maxBox = maxBox;
					parent->m_surfaceArea = area;
					world->ReleaseIndirectLock (&m_updateLock);
				}
			}
		}
	}
}

void dgBroadPhaseCollision::UpdateBodyBroadphase(dgBody* const body, dgInt32 threadIndex)
{
	if (m_rootNode) {

		// if the body is move inside the world, then active it
		if (!body->m_isInWorld) {
			if (dgOverlapTest (body->m_minAABB, body->m_maxAABB, m_appMinBox, m_appMaxBox)) {
				body->m_isInWorld = true;
				body->m_sleeping = false;
				body->m_equilibrium = false;
			}
		}


		// update bodies only if they are in the world
		if (body->m_isInWorld) {

			dgBroadPhaseLeafNode* const node = body->m_collisionCell;
			_ASSERTE (!node->m_left);
			_ASSERTE (!node->m_right);

			if (!dgBoxInclusionTest (body->m_minAABB, body->m_maxAABB, node->m_minBox, node->m_maxBox)) {
				node->SetAABB(body->m_minAABB, body->m_maxAABB);
				dgWorld* const world = (dgWorld*) this;
				for (dgBroadPhaseNode* parent = node->m_parent; parent; parent = parent->m_parent) {
					dgVector minBox;
					dgVector maxBox;
					dgFloat32 area = CalculateSurfaceArea (parent->m_left, parent->m_right, minBox, maxBox);
					if (dgBoxInclusionTest (minBox, maxBox, parent->m_minBox, parent->m_maxBox)) {
						break;
					}

					world->GetIndirectLock (&m_updateLock);
					parent->m_minBox = minBox;
					parent->m_maxBox = maxBox;
					parent->m_surfaceArea = area;
					world->ReleaseIndirectLock (&m_updateLock);
				}
			}
		}
	}
}


void dgBroadPhaseCollision::UpdateContacts (dgFloat32 timestep, bool collisionUpdate)
{
	dgWorld* const world = (dgWorld*) this;
	dgUnsigned32 ticks = world->m_getPerformanceCount();

	ImproveFitness();

	m_contactCount = 0;
	dgCollidingPairCollector* const contactPairs = (dgWorld*)this;
	contactPairs->Init();
	world->m_broadPhaseLru = world->m_broadPhaseLru + 1;

	dgInt32 threadsCount = world->GetThreadCount();	
	dgBroadphaseSyncDescriptor sincksPoints;
	sincksPoints.m_world = world;
	sincksPoints.m_timestep = timestep;

	const dgBodyMasterList* const masterList = world;
	dgBodyMasterList::dgListNode* const firstBodyNode = masterList->GetFirst()->GetNext();
	sincksPoints.m_collindPairBodyNode = firstBodyNode;
	sincksPoints.m_forceAndTorqueBodyNode = firstBodyNode;

	for (dgInt32 i = 0; i < threadsCount; i ++) {
		world->QueueJob (UpdateContactsDriver, &sincksPoints);
	}
	world->SynchronizationBarrier();

	UpdateContactsBroadPhaseEnd();

	dgUnsigned32 endTicks = world->m_getPerformanceCount();
	//	world->m_perfomanceCounters[m_narrowPhaseTicks] = endTicks - narrowTicks;
	world->m_perfomanceCounters[m_collisionTicks] = endTicks - ticks;
}

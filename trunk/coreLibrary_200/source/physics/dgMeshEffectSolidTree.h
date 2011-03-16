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

#ifndef __dgMeshEffectSolidTree_H__
#define __dgMeshEffectSolidTree_H__

//class dgCollision;
//class dgMeshTreeCSGFace;
//class dgMeshEffectSolidTree;
//class dgMeshTreeCSGEdgePool;
//class dgMeshTreeCSGPointsPool;

class dgMeshEffectSolidTree
{
	public:
	class CSGConvexCurve: public dgList<dgHugeVector>
	{
		public:
		CSGConvexCurve ();
		CSGConvexCurve (dgMemoryAllocator* const allocator);
		bool CheckConvex(const dgHugeVector& normal, const dgHugeVector& point) const;
	};

	DG_CLASS_ALLOCATOR(allocator)

	dgMeshEffectSolidTree (const dgMeshEffect& mesh, dgEdge* const face);
	dgMeshEffectSolidTree (const dgHugeVector& normal, const dgHugeVector& point);
	~dgMeshEffectSolidTree();

	void AddFace (const dgMeshEffect& mesh, dgEdge* const face);
	void BuildPlane (const dgMeshEffect& mesh, dgEdge* const face, dgHugeVector& normal, dgHugeVector& point) const;

	dgHugeVector m_point;
	dgHugeVector m_normal;
	dgMeshEffectSolidTree* m_back;
	dgMeshEffectSolidTree* m_front;
};




inline dgMeshEffectSolidTree::CSGConvexCurve::CSGConvexCurve ()
	:dgList<dgHugeVector>(NULL)
{
}

inline dgMeshEffectSolidTree::CSGConvexCurve::CSGConvexCurve (dgMemoryAllocator* const allocator)
	:dgList<dgHugeVector>(allocator)
{
}


inline bool dgMeshEffectSolidTree::CSGConvexCurve::CheckConvex(const dgHugeVector& normal, const dgHugeVector& point) const
{
//	dgBigVector p1 (GetLast()->GetInfo());
//	dgBigVector p0 (GetLast()->GetPrev()->GetInfo());
//	dgBigVector e0 (p0 - p1);
//	for (CSGConvexCurve::dgListNode* node = GetFirst(); node; node = node->GetNext()) {
//		dgFloat64 convex;
//		dgBigVector p2 (node->GetInfo());
//		dgBigVector e1 (p2 - p1);
//		dgBigVector n (e1 * e0);
//		convex = n % plane;
//		if (convex < dgFloat64 (-1.0e5f)) {
//			return false;
//		}
//		p1 = p2;
//		e0 = e1.Scale (-1.0f);
//	}
	return true;
}


inline dgMeshEffectSolidTree::dgMeshEffectSolidTree (const dgMeshEffect& mesh, dgEdge* const face)
{
	BuildPlane (mesh, face, m_normal, m_point);
//	normal = normal.Scale (1.0f / sqrt (normal % normal));
//	m_plane = dgBigPlane (normal, - (normal % origin));
	m_front = NULL;
	m_back = NULL;
}


inline dgMeshEffectSolidTree::dgMeshEffectSolidTree (const dgHugeVector& normal, const dgHugeVector& point)
	:m_point (point), m_normal (normal)
{
	m_front = NULL;
	m_back = NULL;
}

inline dgMeshEffectSolidTree::~dgMeshEffectSolidTree()
{
	if (m_front)	{
		delete m_front; 
	}

	if (m_back)	{
		delete m_back;
	}
}



#endif

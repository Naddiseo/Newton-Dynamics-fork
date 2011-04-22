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

#include <dgRefCounter.h>

class dgMeshEffect;
class dgMeshEffectSolidTree;


class dgCSGFacePoint
{
	public:
	dgHugeVector m_vertex;
	dgFloat64 m_normal_x;
	dgFloat64 m_normal_y;
	dgFloat64 m_normal_z;
	dgFloat64 m_u0;
	dgFloat64 m_v0;
	dgFloat64 m_u1;
	dgFloat64 m_v1;
	dgFloat64 m_material;

	dgCSGFacePoint()
	{
	}

	dgCSGFacePoint(const dgMeshEffect::dgVertexAtribute& p)
		:m_vertex (p.m_vertex)
		,m_normal_x (p.m_normal_x)
		,m_normal_y (p.m_normal_y)
		,m_normal_z (p.m_normal_z)
		,m_u0 (p.m_u0)
		,m_v0 (p.m_v0)
		,m_u1 (p.m_u1)
		,m_v1 (p.m_v1)
		,m_material (p.m_material)
	{

	}

	dgMeshEffect::dgVertexAtribute GetPoint() const
	{
		dgMeshEffect::dgVertexAtribute p;
		p.m_vertex.m_x = m_vertex.m_x.GetAproximateValue();
		p.m_vertex.m_y = m_vertex.m_y.GetAproximateValue();
		p.m_vertex.m_z = m_vertex.m_z.GetAproximateValue();
		p.m_vertex.m_w = m_vertex.m_w.GetAproximateValue();
		p.m_normal_x = m_normal_x;
		p.m_normal_y = m_normal_y;
		p.m_normal_z = m_normal_z;
		p.m_u0 = m_u0;
		p.m_v0 = m_v0;
		p.m_u1 = m_u1;
		p.m_v1 = m_v1;
		p.m_material = m_material;

		return p;
	}

};

//class dgMeshTreeCSGFace: public dgList<dgMeshEffect::dgVertexAtribute>, public dgRefCounter
class dgMeshTreeCSGFace: public dgList<dgCSGFacePoint>, public dgRefCounter

{

	public:

	enum dgFaceCode
	{
		m_back = 1,
		m_front,
		m_coplanar,
	};

	dgMeshTreeCSGFace (dgMemoryAllocator* const allocator, const dgMeshEffect& mesh, dgEdge* const face);
	dgMeshTreeCSGFace (dgMemoryAllocator* const allocator, dgInt32 count, const dgCSGFacePoint* const points);


	void Clip (const dgHugeVector& normal, const dgHugeVector& origin, dgMeshTreeCSGFace** leftOut, dgMeshTreeCSGFace** rightOut);

	dgCSGFacePoint Interpolate (const dgHugeVector& normal, const dgHugeVector& origin, const dgCSGFacePoint& p0, const dgCSGFacePoint& p1) const;

	void MergeMissingVertex (const dgMeshTreeCSGFace* const face);
	bool IsPointOnEdge (const dgBigVector& p0, const dgBigVector& p1, const dgBigVector& q) const;

	dgFaceCode DetermineSide (const dgMeshEffectSolidTree* const bsp);

	dgFaceCode m_side;
};

class dgMeshEffectSolidTree
{
	public:
	DG_CLASS_ALLOCATOR(allocator)

	enum dgPlaneType
	{
		m_divider = 1,
		m_empty,
		m_solid,
	};

	class CSGConvexCurve: public dgList<dgHugeVector>
	{
		public:
		CSGConvexCurve ();
		CSGConvexCurve (dgMemoryAllocator* const allocator);
		bool CheckConvex(const dgHugeVector& normal, const dgHugeVector& point) const;
	};

	dgMeshEffectSolidTree (dgPlaneType type);
	dgMeshEffectSolidTree (const dgMeshEffect& mesh, dgEdge* const face);
	dgMeshEffectSolidTree (const dgHugeVector& normal, const dgHugeVector& point, dgMemoryAllocator* const allocator);
	~dgMeshEffectSolidTree();

	void BuildPlane (const dgMeshEffect& mesh, dgEdge* const face, dgHugeVector& normal, dgHugeVector& point) const;
	void AddFace (const dgMeshEffect& mesh, dgEdge* const face);

#ifdef _DEBUG
	dgInt32 m_id;
	static dgInt32 m_enumerator;
#endif

	dgPlaneType m_planeType;
	dgMeshEffectSolidTree* m_back;
	dgMeshEffectSolidTree* m_front;

	dgHugeVector m_origin;
	dgHugeVector m_normal;
};
#endif

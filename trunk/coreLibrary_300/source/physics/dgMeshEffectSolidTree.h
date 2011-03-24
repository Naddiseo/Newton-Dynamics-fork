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

class dgCollision;
class dgMeshEffect;


class dgMeshTreeCSGFace: public dgPolyhedra
{
	public:
	dgMeshTreeCSGFace (const dgMeshEffect& mesh, const dgEdge* const face);
	void ClipFace (dgEdge* const face, const dgHugeVector& normal, const dgHugeVector& origin, dgEdge** const poolReplacement, dgInt32 poolCount, dgEdge** leftOut, dgEdge** rightOut);
	dgInt32 AddPoint (const dgBigVector& point);

	bool CheckConsistency ();
	dgInt32 m_count;
	dgBigVector m_points[DG_MESH_EFFECT_POINT_SPLITED];
};

/*
class dgMeshTreeCSGFace: public dgRefCounter
{
	public:
	class CSGLinearEdge
	{
		public:
		DG_CLASS_ALLOCATOR(allocator)

		CSGLinearEdge(dgInt32 index)
			:m_index(index) 
		{
			m_next = this;
		}

		CSGLinearEdge(dgInt32 index, CSGLinearEdge* parent)
			:m_index(index) 
		{
			parent->m_next = this;
		}

		dgInt32 m_index;
		CSGLinearEdge* m_next;
	};

	DG_CLASS_ALLOCATOR(allocator)

	dgMeshTreeCSGFace(dgMemoryAllocator* const allocator);
	~dgMeshTreeCSGFace(); 

	void AddPoint (dgInt32 index);
	bool CheckConvex(const dgMeshTreeCSGPointsPool& pool) const;
	void InsertVertex (const dgMeshTreeCSGFace* const vertices, const dgMeshTreeCSGPointsPool& pool); 



	CSGLinearEdge* m_face;
	CSGLinearEdge* m_last;
	dgMemoryAllocator* m_allocator;
};
*/

class dgMeshEffectSolidTree
{
	public:
	DG_CLASS_ALLOCATOR(allocator)

	class CSGConvexCurve: public dgList<dgHugeVector>
	{
		public:
		CSGConvexCurve ();
		CSGConvexCurve (dgMemoryAllocator* const allocator);
		bool CheckConvex(const dgHugeVector& normal, const dgHugeVector& point) const;
	};

	dgMeshEffectSolidTree (const dgMeshEffect& mesh, dgEdge* const face);
	dgMeshEffectSolidTree (const dgHugeVector& normal, const dgHugeVector& point);
	~dgMeshEffectSolidTree();

	void BuildPlane (const dgMeshEffect& mesh, dgEdge* const face, dgHugeVector& normal, dgHugeVector& point) const;
	void AddFace (const dgMeshEffect& mesh, dgEdge* const face);

/*
	void ReconstructFace (dgMeshEffect& polygon, const dgMeshTreeCSGPointsPool& pointsPool,
						  dgInt32 frontFaceCount, dgMeshTreeCSGFace** const frontFace, dgInt32 frontFaceID,
						  dgInt32 backFaceCount, dgMeshTreeCSGFace** const backFace, dgInt32 backFaceID) const;


	void AddFaces(dgMeshEffect& polygon, dgInt32 count, dgMeshTreeCSGFace** const faceList, dgInt32 faceID, const dgMeshTreeCSGPointsPool& pointsPool) const;
	void MergeVertex(const dgMeshTreeCSGPointsPool& pointsPool, dgInt32 count, dgMeshTreeCSGFace** const faceList) const;
*/	
		                   


	dgHugeVector m_origin;
	dgHugeVector m_normal;
	dgMeshEffectSolidTree* m_back;
	dgMeshEffectSolidTree* m_front;
};




#endif

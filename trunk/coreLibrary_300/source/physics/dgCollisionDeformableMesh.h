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


#ifndef __DGCOLLISION_DEFORMABLE_MESH_H__
#define __DGCOLLISION_DEFORMABLE_MESH_H__


#include "dgCollision.h"
#include "dgCollisionConvex.h"



class dgCollisionDeformableMesh: public dgCollisionConvex
{
public:
	//dgCollisionDeformableMesh(dgMemoryAllocator* allocator, const dgVector& boxP0, const dgVector& boxP1, const dgUserMeshCreation& data);
	dgCollisionDeformableMesh(dgMemoryAllocator* allocator, dgMeshEffect* const mesh);
	dgCollisionDeformableMesh (dgWorld* const world, dgDeserialize deserialization, void* const userData);
	virtual ~dgCollisionDeformableMesh(void);

//	void GetVertexListIndexList (const dgVector& p0, const dgVector& p1, dgGetVertexListIndexList &data) const;

private:
	class dgDeformableNode;

	void Serialize(dgSerialize callback, void* const userData) const;
	virtual void GetCollisionInfo(dgCollisionInfo* info) const;
	virtual dgFloat32 RayCast (const dgVector& localP0, const dgVector& localP1, dgContactPoint& contactOut, OnRayPrecastAction preFilter, const dgBody* const body, void* const userData) const;
	virtual dgFloat32 RayCastSimd (const dgVector& localP0, const dgVector& localP1, dgContactPoint& contactOut, OnRayPrecastAction preFilter, const dgBody* const body, void* const userData) const;
	virtual void GetCollidingFaces (dgPolygonMeshDesc* const data) const;
	virtual void GetCollidingFacesSimd (dgPolygonMeshDesc* const data) const;

	virtual void DebugCollision (const dgMatrix& matrixPtr, OnDebugCollisionMeshCallback callback, void* const userData) const;

	virtual dgInt32 CalculateSignature () const;
	virtual void SetCollisionBBox (const dgVector& p0, const dgVector& p1);

	
	void ImproveTotalFitness();
	void ImproveNodeFitness (dgDeformableNode* const node);
	dgDeformableNode* BuildTopDown (dgInt32 count, dgDeformableNode* const children, dgDeformableNode* const parent);
	dgFloat32 CalculateSurfaceArea (const dgDeformableNode* const node0, const dgDeformableNode* const node1, dgVector& minBox, dgVector& maxBox) const;
	

	
	dgInt32 m_trianglesCount;
	dgInt32 m_nodesCount;
	dgInt32* m_indexList;
	dgVector* m_vertexArray;
	dgDeformableNode* m_rootNode;
	dgDeformableNode* m_nodesMemory;
		
	
};


#endif // !defined(AFX_DGCOLLISIONBOX_H__364692C2_5F23_41AE_A167_7A92E2D2DA5F__INCLUDED_)


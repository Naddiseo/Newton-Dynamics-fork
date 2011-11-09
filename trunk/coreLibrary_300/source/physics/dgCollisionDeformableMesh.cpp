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
#include "dgContact.h"
#include "dgMeshEffect.h"
#include "dgCollisionDeformableMesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#define DG_DEFORMABLE_PADDING dgFloat32 (0.5f)

class dgCollisionDeformableMesh::dgDeformableNode
{
	public:
	dgDeformableNode ()
	{
	}

	~dgDeformableNode ()
	{
		if (m_back) {
			delete m_back;
		}
		if (m_front) {
			delete m_front;
		}
	}

	void CalcBox (const dgVector* const m_vertexArray, dgInt32* const faceIndices) 
	{
		dgVector minP (m_vertexArray[faceIndices[0]]); 
		dgVector maxP (m_vertexArray[faceIndices[0]]); 
		for (dgInt32 i = 1; i < 3; i ++) {
			dgInt32 index = faceIndices[i];
			const dgVector& p  = m_vertexArray[index];

			minP.m_x = GetMin (p.m_x, minP.m_x); 
			minP.m_y = GetMin (p.m_y, minP.m_y); 
			minP.m_z = GetMin (p.m_z, minP.m_z); 

			maxP.m_x = GetMax (p.m_x, maxP.m_x); 
			maxP.m_y = GetMax (p.m_y, maxP.m_y); 
			maxP.m_z = GetMax (p.m_z, maxP.m_z); 
		}

		m_p0.m_x = minP.m_x - DG_DEFORMABLE_PADDING;
		m_p0.m_y = minP.m_y - DG_DEFORMABLE_PADDING;
		m_p0.m_z = minP.m_z - DG_DEFORMABLE_PADDING;
		m_p1.m_x = maxP.m_x + DG_DEFORMABLE_PADDING;
		m_p1.m_y = maxP.m_y + DG_DEFORMABLE_PADDING;
		m_p1.m_z = maxP.m_z + DG_DEFORMABLE_PADDING;

		dgVector side0 (m_p1 - m_p0);
		dgVector side1 (side0.m_y, side0.m_z, side0.m_x, dgFloat32 (0.0f));
		m_surfaceArea = side0 % side1;
	}


	dgVector m_p0;
	dgVector m_p1;
	dgInt32 m_indexStart;
	dgFloat32 m_surfaceArea;
	dgDeformableNode* m_back;
	dgDeformableNode* m_front;
	dgDeformableNode* m_parent;
};


dgCollisionDeformableMesh::dgCollisionDeformableMesh (dgWorld* const world, dgDeserialize deserialization, void* const userData)
	:dgCollisionConvex (world, deserialization, userData)
	,m_nodesMemory(NULL)
	,m_indexList(NULL)
	,m_vertexArray(NULL)
	,m_rootNode(NULL)
{
_ASSERTE (0);
	m_rtti |= dgCollisionDeformableMesh_RTTI;
	
/*
	dgAABBPolygonSoup::Deserialize (deserialization, userData);

	dgVector p0; 
	dgVector p1; 
	GetAABB (p0, p1);
	SetCollisionBBox(p0, p1);
*/
}

dgCollisionDeformableMesh::~dgCollisionDeformableMesh(void)
{
	if (m_vertexArray) {
		dgFree (m_vertexArray);
	}

	if (m_indexList) {
		dgFree (m_indexList);
	}

	if (m_nodesMemory) {
		dgFree (m_nodesMemory);
	}
}

void dgCollisionDeformableMesh::Serialize(dgSerialize callback, void* const userData) const
{
	_ASSERTE (0);
/*
	SerializeLow(callback, userData);
	dgAABBPolygonSoup::Serialize ((dgSerialize) callback, userData);
*/
}

dgInt32 dgCollisionDeformableMesh::CalculateSignature () const
{
	return 0;
}

void dgCollisionDeformableMesh::SetCollisionBBox (const dgVector& p0, const dgVector& p1)
{
	_ASSERTE (0);
}


/*
void dgCollisionDeformableMesh::GetVertexListIndexList (const dgVector& p0, const dgVector& p1, dgGetVertexListIndexList &data) const
{
	_ASSERTE (0);

	if (m_faceInAabb) {
		return m_faceInAabb (m_userData, &p0[0], &p1[0], (const dgFloat32**) &data.m_veterxArray, &data.m_vertexCount, &data.m_vertexStrideInBytes,
							 data.m_indexList, data.m_maxIndexCount, data.m_userDataList);

	} else {
		data.m_triangleCount = 0;
	}
}
*/
void dgCollisionDeformableMesh::GetCollisionInfo(dgCollisionInfo* info) const
{
	_ASSERTE (0);
/*
	dgCollision::GetCollisionInfo(info);
	info->m_offsetMatrix = GetOffsetMatrix();
	if (m_getInfo) {
		m_getInfo (m_userData, info);
	}
*/
}


dgFloat32 dgCollisionDeformableMesh::RayCastSimd (const dgVector& localP0, const dgVector& localP1, dgContactPoint& contactOut, OnRayPrecastAction preFilter, const dgBody* const body, void* const userData) const
{
_ASSERTE (0);
return 0;
//	return RayCast (localP0, localP1, contactOut, preFilter, body, userData);
}


dgFloat32 dgCollisionDeformableMesh::RayCast (
	const dgVector& localP0, 
	const dgVector& localP1, 
	dgContactPoint& contactOut,
	OnRayPrecastAction preFilter, 
	const dgBody* const body,
	void* const userData) const
{
_ASSERTE (0);
return 0;
/*
	dgFloat32 t;
	dgFloat32 param;
	if (PREFILTER_RAYCAST (preFilter, body, this, userData)) {
		return dgFloat32 (1.2f);
	}

	param = dgFloat32 (1.2f);
	if (m_rayHitCallBack) {
		dgCollisionMeshRayHitDesc data;
		data.m_localP0 = localP0;
		data.m_localP1 = localP1;
		data.m_userData = m_userData;
		data.m_altenateUserData = userData;
		if (body) {
			data.m_matrix = body->m_collisionWorldMatrix;
		}

		t = m_rayHitCallBack (data);
		if ((t < dgFloat32 (1.0f)) && (t > dgFloat32 (0.0f))) {
			param = t;
			contactOut.m_normal = data.m_normal;
			contactOut.m_userId = data.m_userId;
		} 
	}
	return param;
*/
}


void dgCollisionDeformableMesh::GetCollidingFacesSimd (dgPolygonMeshDesc* const data) const
{
	_ASSERTE (0);
//	GetCollidingFaces (data);
}

void dgCollisionDeformableMesh::GetCollidingFaces (dgPolygonMeshDesc* const data) const
{
	_ASSERTE (0);
/*
	data->m_faceCount = 0;
	if (m_collideCallback) {
		data->m_me = this;
		data->m_userData = m_userData;
		m_collideCallback (*data);
	}
*/
}


void dgCollisionDeformableMesh::DebugCollision (const dgMatrix& matrixPtr, OnDebugCollisionMeshCallback callback, void* const userData) const
{
	_ASSERTE (0);
/*
	dgCollisionDeformableMeshShowPolyContext context;

	context.m_matrix = matrixPtr;
	context.m_userData = userData;;
	context.m_callback = callback;

	dgVector p0 (dgFloat32 (-1.0e20f), dgFloat32 (-1.0e20f), dgFloat32 (-1.0e20f), dgFloat32 (0.0f));
	dgVector p1 (dgFloat32 ( 1.0e20f), dgFloat32 ( 1.0e20f), dgFloat32 ( 1.0e20f), dgFloat32 (0.0f));
	ForAllSectors (p0, p1, ShowDebugPolygon, &context);
*/
}





//dgCollisionDeformableMesh::dgCollisionDeformableMesh(dgMemoryAllocator* allocator, const dgVector& boxP0, const dgVector& boxP1, const dgUserMeshCreation& data)
dgCollisionDeformableMesh::dgCollisionDeformableMesh(dgMemoryAllocator* allocator, dgMeshEffect* const mesh)
	:dgCollisionConvex (allocator, 0, dgGetIdentityMatrix(), m_deformableMesh)
	,m_nodesMemory(NULL)
	,m_vertexArray(NULL)
	,m_rootNode(NULL)
{

	//	m_rtti |= dgCollisionDeformableMesh_RTTI;
	//	m_userData = data.m_userData;
	//	m_getInfo = data.m_getInfo;
	//	m_faceInAabb = data.m_faceInAabb;
	//	m_rayHitCallBack = data.m_rayHitCallBack;
	//	m_collideCallback = data.m_collideCallback;
	//	m_destroyCallback = data.m_destroyCallback;
	//	SetCollisionBBox(boxP0, boxP1);

	mesh->Triangulate();
	int vertexCount = mesh->GetVertexCount (); 
	int faceCount = mesh->GetTotalFaceCount (); 
	int indexCount = mesh->GetTotalIndexCount (); 

	m_rootNode = (dgDeformableNode*) dgMallocStack(sizeof (dgDeformableNode) * faceCount * 2);
	m_nodesMemory = m_rootNode;

	m_indexList = (dgInt32*) dgMallocStack (sizeof (dgInt32) * faceCount * 3);
	m_vertexArray = (dgVector*) dgMallocStack( sizeof (dgVector) *(vertexCount + faceCount * 2));

	dgInt32 stride = mesh->GetVertexStrideInByte() / sizeof (dgFloat64);  
	dgFloat64* const vertex = mesh->GetVertexPool();  
	for (dgInt32 i = 0; i < vertexCount; i ++) {
		m_vertexArray[i] = dgVector (dgFloat32 (vertex[i * stride + 0]), dgFloat32 (vertex[i * stride + 1]), dgFloat32 (vertex[i * stride + 2]), dgFloat32 (0.0f));
	}

	int* const faceArray = (dgInt32*) dgMallocStack(faceCount * sizeof (dgInt32));
	int* const materialIndexArray = (dgInt32*) dgMallocStack(faceCount * sizeof (dgInt32));
	void** const indexArray = (void**) dgMallocStack(indexCount * sizeof (void*));
	mesh->GetFaces (faceArray, materialIndexArray, indexArray);

	for (dgInt32 i = 0; i < faceCount; i ++) {
		dgInt32 count = faceArray[i];
		_ASSERTE (count == 3);
		for (dgInt32 j = 0; j < count; j ++) {
			dgInt32 k = mesh->GetVertexIndex(indexArray[i * 3 + j]);
			m_indexList[i * 3 + j] = k;
		}
		dgDeformableNode& node = m_rootNode[i];
		node.m_back = NULL;
		node.m_front = NULL;
		node.m_parent = NULL;
		node.m_indexStart = i * 3;
		node.CalcBox(m_vertexArray, &m_indexList[i * 3]);
	}



	dgFree (indexArray);
	dgFree (materialIndexArray);
	dgFree (faceArray);
}

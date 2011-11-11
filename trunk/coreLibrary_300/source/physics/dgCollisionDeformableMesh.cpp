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
		if (m_left) {
			delete m_left;
		}
		if (m_right) {
			delete m_right;
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

		m_minBox.m_x = minP.m_x - DG_DEFORMABLE_PADDING;
		m_minBox.m_y = minP.m_y - DG_DEFORMABLE_PADDING;
		m_minBox.m_z = minP.m_z - DG_DEFORMABLE_PADDING;
		m_maxBox.m_x = maxP.m_x + DG_DEFORMABLE_PADDING;
		m_maxBox.m_y = maxP.m_y + DG_DEFORMABLE_PADDING;
		m_maxBox.m_z = maxP.m_z + DG_DEFORMABLE_PADDING;

		dgVector side0 (m_maxBox - m_minBox);
		dgVector side1 (side0.m_y, side0.m_z, side0.m_x, dgFloat32 (0.0f));
		m_surfaceArea = side0 % side1;
	}




	dgVector m_minBox;
	dgVector m_maxBox;
	dgInt32 m_indexStart;
	dgFloat32 m_surfaceArea;
	dgDeformableNode* m_left;
	dgDeformableNode* m_right;
	dgDeformableNode* m_parent;
};


dgCollisionDeformableMesh::dgCollisionDeformableMesh (dgWorld* const world, dgDeserialize deserialization, void* const userData)
	:dgCollisionConvex (world, deserialization, userData)
	,m_trianglesCount(0)
	,m_indexList(NULL)
	,m_vertexArray(NULL)
	,m_rootNode(NULL)
	,m_nodesMemory(NULL)
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
	_ASSERTE (p0.m_x <= p1.m_x);
	_ASSERTE (p0.m_y <= p1.m_y);
	_ASSERTE (p0.m_z <= p1.m_z);

	m_boxSize = (p1 - p0).Scale (dgFloat32 (0.5f)); 
	m_boxOrigin = (p1 + p0).Scale (dgFloat32 (0.5f)); 
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




dgCollisionDeformableMesh::dgCollisionDeformableMesh(dgMemoryAllocator* allocator, dgMeshEffect* const mesh)
	:dgCollisionConvex (allocator, 0, dgGetIdentityMatrix(), m_deformableMesh)
	,m_trianglesCount(0)
	,m_nodesCount(0)
	,m_vertexCount(0)
	,m_indexList(NULL)
	,m_vertexArray(NULL)
	,m_rootNode(NULL)
	,m_nodesMemory(NULL)
{
	m_rtti |= dgCollisionDeformableMesh_RTTI;
	mesh->Triangulate();
	dgInt32 vertexCount = mesh->GetVertexCount (); 
	dgInt32 faceCount = mesh->GetTotalFaceCount (); 
	dgInt32 indexCount = mesh->GetTotalIndexCount (); 

	m_trianglesCount = faceCount;
	m_rootNode = (dgDeformableNode*) dgMallocStack(sizeof (dgDeformableNode) * (faceCount * 2 - 1));
	m_nodesMemory = m_rootNode;

	m_vertexCount = vertexCount + faceCount * 2;
	m_indexList = (dgInt32*) dgMallocStack (sizeof (dgInt32) * faceCount * 3);
	m_vertexArray = (dgVector*) dgMallocStack (sizeof (dgVector) * m_vertexCount);

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
		node.m_left = NULL;
		node.m_right = NULL;
		node.m_parent = NULL;
		node.m_indexStart = i * 3;
		node.CalcBox(m_vertexArray, &m_indexList[i * 3]);
	}

	m_nodesCount = faceCount;
	m_rootNode = BuildTopDown (faceCount, m_nodesMemory, NULL);
	ImproveTotalFitness();

	dgFree (indexArray);
	dgFree (materialIndexArray);
	dgFree (faceArray);
}


dgCollisionDeformableMesh::dgCollisionDeformableMesh (const dgCollisionDeformableMesh& source)
	:dgCollisionConvex (source.GetAllocator(), 0, dgGetIdentityMatrix(), m_deformableMesh)
	,m_trianglesCount(source.m_trianglesCount)
	,m_nodesCount(source.m_trianglesCount)
	,m_vertexCount(source.m_vertexCount)
{
	m_rtti = source.m_rtti;

	m_indexList = (dgInt32*) dgMallocStack (sizeof (dgInt32) * m_trianglesCount * 3);
	m_vertexArray = (dgVector*) dgMallocStack (sizeof (dgVector) * m_vertexCount);
	m_nodesMemory = (dgDeformableNode*) dgMallocStack(sizeof (dgDeformableNode) * (m_trianglesCount * 2 - 1));
/*
	memcpy (m_indexList, source.m_indexList, sizeof (dgInt32) * m_trianglesCount * 3);
	memcpy (m_vertexArray, source.m_vertexArray, sizeof (dgVector) * m_vertexCount);
	memcpy (m_nodesMemory, source.m_nodesMemory, sizeof (dgDeformableNode) * (m_trianglesCount * 2 - 1));

	dgInt32 index = source.m_rootNode - source.m_nodesMemory;
	m_rootNode = &m_nodesMemory[index];

	for (dgInt32 i = 0; i < (m_trianglesCount * 2 - 1); i ++) {
		dgDeformableNode* const node = &m_nodesMemory[i];
		if (node->m_parent) {
			dgInt32 index = node->m_parent - source.m_nodesMemory;
			node->m_parent = &m_nodesMemory[index];
		}

		if (node->m_left) {
			dgInt32 index = node->m_left - source.m_nodesMemory;
			node->m_left = &m_nodesMemory[index];
		}

		if (node->m_right) {
			dgInt32 index = node->m_right - source.m_nodesMemory;
			node->m_right = &m_nodesMemory[index];
		}
	}

	SetCollisionBBox (m_rootNode->m_minBox, m_rootNode->m_maxBox);
*/
}


dgFloat32 dgCollisionDeformableMesh::CalculateSurfaceArea (const dgDeformableNode* const node0, const dgDeformableNode* const node1, dgVector& minBox, dgVector& maxBox) const
{
	minBox = dgVector (GetMin (node0->m_minBox.m_x, node1->m_minBox.m_x), GetMin (node0->m_minBox.m_y, node1->m_minBox.m_y), GetMin (node0->m_minBox.m_z, node1->m_minBox.m_z), dgFloat32 (0.0f));
	maxBox = dgVector (GetMax (node0->m_maxBox.m_x, node1->m_maxBox.m_x), GetMax (node0->m_maxBox.m_y, node1->m_maxBox.m_y), GetMax (node0->m_maxBox.m_z, node1->m_maxBox.m_z), dgFloat32 (0.0f));		
	dgVector side0 (maxBox - minBox);
	dgVector side1 (side0.m_y, side0.m_z, side0.m_x, dgFloat32 (0.0f));
	return side0 % side1;
}


dgCollisionDeformableMesh::dgDeformableNode* dgCollisionDeformableMesh::BuildTopDown (dgInt32 count, dgDeformableNode* const children, dgDeformableNode* const parent)
{
	dgDeformableNode* root = NULL;				
	if (count == 1) {
		root = children;
		root->m_left = NULL;
		root->m_right = NULL;
		root->m_parent = parent;
	} else if (count == 2) {
		root = &m_nodesMemory[m_nodesCount];
		m_nodesCount ++;
		root->m_indexStart = -1;
		root->m_parent = parent;
		root->m_left = BuildTopDown (1, children, root);
		root->m_right = BuildTopDown (1, &children[1], root);
		root->m_surfaceArea = CalculateSurfaceArea (root->m_left, root->m_right, root->m_minBox, root->m_maxBox);
	} else {

		dgVector median (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
		dgVector varian (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
		for (dgInt32 i = 0; i < count; i ++) {
			const dgDeformableNode* const node = &children[i];
			dgVector p ((node->m_minBox + node->m_maxBox).Scale (0.5f));
			median += p;
			varian += p.CompProduct (p);
		}

		varian = varian.Scale (dgFloat32 (count)) - median.CompProduct(median);

		dgInt32 index = 0;
		dgFloat32 maxVarian = dgFloat32 (-1.0e10f);
		for (dgInt32 i = 0; i < 3; i ++) {
			if (varian[i] > maxVarian) {
				index = i;
				maxVarian = varian[i];
			}
		}

		dgVector center = median.Scale (dgFloat32 (1.0f) / dgFloat32 (count));
		dgFloat32 test = center[index];

		dgInt32 i0 = 0;
		dgInt32 i1 = count - 1;
		do {    
			for (; i0 <= i1; i0 ++) {
				const dgDeformableNode* const node = &m_nodesMemory[i0];
				dgFloat32 val = (node->m_minBox[index] + node->m_maxBox[index]) * dgFloat32 (0.5f);
				if (val > test) {
					break;
				}
			}

			for (; i1 >= i0; i1 --) {
				const dgDeformableNode* const node = &m_nodesMemory[i1];
				//dgFloat32 val = (points[j0 * step + index] + points[j1 * step + index]) * dgFloat32 (0.5f);
				dgFloat32 val = (node->m_minBox[index] + node->m_maxBox[index]) * dgFloat32 (0.5f);
				if (val < test) {
					break;
				}
			}

			if (i0 < i1)	{
				Swap(m_nodesMemory[i0], m_nodesMemory[i1]);
				i0++; 
				i1--;
			}

		} while (i0 <= i1);

		if (i0 > 0){
			i0 --;
		}
		if ((i0 + 1) >= count) {
			i0 = count - 2;
		}

		dgInt32 spliteCount = i0 + 1;

		root = &m_nodesMemory[m_nodesCount];
		m_nodesCount ++;
		root->m_indexStart = -1;
		root->m_parent = parent;
		root->m_left = BuildTopDown (spliteCount, children, root);
		root->m_right = BuildTopDown (count - spliteCount, &children[spliteCount], root);
		root->m_surfaceArea = CalculateSurfaceArea (root->m_left, root->m_right, root->m_minBox, root->m_maxBox);
	}

	return root;
}


void dgCollisionDeformableMesh::ImproveNodeFitness (dgDeformableNode* const node)
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
				dgDeformableNode* const parent = node->m_parent;
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
				dgDeformableNode* const parent = node->m_parent;
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
				dgDeformableNode* const parent = node->m_parent;
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
				dgDeformableNode* const parent = node->m_parent;
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


void dgCollisionDeformableMesh::ImproveTotalFitness()
{
	dgInt32 count = m_nodesCount - m_trianglesCount; 
	dgInt32 maxPasses = 2 * exp_2 (count) + 1;

	dgDeformableNode* const nodes = &m_nodesMemory[m_trianglesCount];
	dgFloat64 newCost = dgFloat32 (1.0e20f);
	dgFloat64 prevCost = newCost;
	do {
		prevCost = newCost;
		for (dgInt32 i = 0; i < count; i ++) {
			dgDeformableNode* const node = &nodes[i];
			ImproveNodeFitness (node);
		}

		newCost	= dgFloat32 (0.0f);
		for (dgInt32 i = 0; i < count; i ++) {
			const dgDeformableNode* const node = &nodes[i];
			newCost += node->m_surfaceArea;
		}

		maxPasses --;
	} while (maxPasses && (newCost < prevCost));

	SetCollisionBBox (m_rootNode->m_minBox, m_rootNode->m_maxBox);

/*
	p0[3] = dgFloat32 (0.0f);
	p1[3] = dgFloat32 (0.0f);
	m_boxSize = (p1 - p0).Scale (dgFloat32 (0.5f)); 
	m_boxOrigin = (p1 + p0).Scale (dgFloat32 (0.5f)); 
	m_boxMinRadius = GetMin(m_boxSize.m_x, m_boxSize.m_y, m_boxSize.m_z);
	m_boxMaxRadius = dgSqrt (m_boxSize % m_boxSize);

	m_size_x.m_x = m_boxSize.m_x;
	m_size_x.m_y = m_boxSize.m_x;
	m_size_x.m_z = m_boxSize.m_x;
	m_size_x.m_w = dgFloat32 (0.0f); 

	m_size_y.m_x = m_boxSize.m_y;
	m_size_y.m_y = m_boxSize.m_y;
	m_size_y.m_z = m_boxSize.m_y;
	m_size_y.m_w = dgFloat32 (0.0f); 

	m_size_z.m_x = m_boxSize.m_z;
	m_size_z.m_y = m_boxSize.m_z;
	m_size_z.m_z = m_boxSize.m_z;
	m_size_z.m_w = dgFloat32 (0.0f); 
*/
}


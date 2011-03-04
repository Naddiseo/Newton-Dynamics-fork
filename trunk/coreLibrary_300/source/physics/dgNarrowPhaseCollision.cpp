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

#include "dgWorld.h"

#include "dgCollision.h"
#include "dgCollisionBVH.h"
#include "dgCollisionBox.h"
#include "dgCollisionCone.h"
#include "dgCollisionNull.h"
#include "dgMinkowskiConv.h"
#include "dgBodyMasterList.h"
#include "dgCollisionScene.h"
#include "dgCollisionSphere.h"
#include "dgCollisionCapsule.h"
#include "dgCollisionEllipse.h"
#include "dgCollisionCylinder.h"
#include "dgCollisionCompound.h"
#include "dgCollisionUserMesh.h"
#include "dgWorldDynamicUpdate.h"
#include "dgCollisionConvexHull.h"
#include "dgCollisionHeightField.h"
#include "dgCollisionConvexModifier.h"
#include "dgCollisionChamferCylinder.h"
#include "dgCollisionCompoundBreakable.h"

#define DG_USE_CACHE_CONTACTS


#define DG_PRUNE_PADDING_BYTES  128

//#define DG_CONTACT_CACHE_TOLERANCE dgFloat32 (1.0e-6f)
//#define DG_REDUCE_CONTACT_TOLERANCE dgFloat32 (1.0e-2f)
//#define DG_PRUNE_CONTACT_TOLERANCE dgFloat32 (1.0e-2f)

#define DG_COMPOUND_MAX_SORT_ARRAY	(1024 * 2)


dgCollision* dgWorld::CreateNull ()
{
	dgUnsigned32 crc;
	dgCollision* collision;
	dgBodyCollisionList::dgTreeNode* node;

	crc = dgCollision::dgCollisionNull_RTTI;

	node = dgBodyCollisionList::Find (crc);
	if (!node) {
    	collision = new  (m_allocator) dgCollisionNull (m_allocator, crc);
		node = dgBodyCollisionList::Insert (collision, crc);
	}

	node->GetInfo()->AddRef();
	return node->GetInfo();
}


dgCollision* dgWorld::CreateBox(dgFloat32 dx, dgFloat32 dy, dgFloat32 dz, dgInt32 shapeID, const dgMatrix& offsetMatrix)
{
//	dgUnsigned32 crc;
//	dgCollision* collision;
//	dgBodyCollisionList::dgTreeNode* node;
	dgUnsigned32 buffer[2 * sizeof (dgMatrix) / sizeof(dgInt32)];

	dx = dgAbsf (dx);
	dy = dgAbsf (dy);
	dz = dgAbsf (dz);

	memset (buffer, 0, sizeof (buffer));
	buffer[0] = m_boxCollision;
	buffer[1] = dgCollision::Quantize (dx * dgFloat32 (0.5f));
	buffer[2] = dgCollision::Quantize (dy * dgFloat32 (0.5f));
	buffer[3] = dgCollision::Quantize (dz * dgFloat32 (0.5f));
	buffer[4] = dgUnsigned32 (shapeID);
	memcpy (&buffer[5], &offsetMatrix, sizeof (dgMatrix));
	dgUnsigned32 crc = dgCollision::MakeCRC(buffer, sizeof (buffer));

	dgBodyCollisionList::dgTreeNode* node = dgBodyCollisionList::Find (crc);
	if (!node) {
		dgCollision* const collision = new  (m_allocator) dgCollisionBox (m_allocator, crc, dx, dy, dz, offsetMatrix);
		collision->SetUserDataID (dgUnsigned32 (shapeID));
		node = dgBodyCollisionList::Insert (collision, crc);
	}

	node->GetInfo()->AddRef();
	return node->GetInfo();
}


dgCollision* dgWorld::CreateSphere(dgFloat32 radii, dgInt32 shapeID, const dgMatrix& offsetMatrix)
{
	dgUnsigned32 buffer[2 * sizeof (dgMatrix) / sizeof(dgInt32)];
	radii = dgAbsf (radii);

	memset (buffer, 0, sizeof (buffer));
	buffer[0] = m_sphereCollision;
	buffer[1] = dgCollision::Quantize (radii);
	buffer[2] = dgUnsigned32 (shapeID);
	memcpy (&buffer[3], &offsetMatrix, sizeof (dgMatrix));
	dgUnsigned32 crc = dgCollision::MakeCRC(buffer, sizeof (buffer));

	dgBodyCollisionList::dgTreeNode* node = dgBodyCollisionList::Find (crc);
	if (!node) {
    	dgCollision* const collision = new  (m_allocator) dgCollisionSphere (m_allocator, crc, radii, offsetMatrix);
		collision->SetUserDataID (dgUnsigned32 (shapeID));
		node = dgBodyCollisionList::Insert (collision, crc);
	}

	node->GetInfo()->AddRef();
	return node->GetInfo();
}

dgCollision* dgWorld::CreateEllipse (dgFloat32 rx, dgFloat32 ry, dgFloat32 rz, dgInt32 shapeID, const dgMatrix& offsetMatrix)
{
	dgUnsigned32 buffer[2 * sizeof (dgMatrix) / sizeof(dgInt32)];

	rx = dgAbsf (rx);
	ry = dgAbsf (ry);
	rz = dgAbsf (rz);

	memset (buffer, 0, sizeof (buffer));
	buffer[0] = m_ellipseCollision;
	buffer[1] = dgCollision::Quantize (rx);
	buffer[2] = dgCollision::Quantize (ry);
	buffer[3] = dgCollision::Quantize (rz);
	buffer[4] = dgUnsigned32 (shapeID);
	memcpy (&buffer[5], &offsetMatrix, sizeof (dgMatrix));
	dgUnsigned32 crc = dgCollision::MakeCRC(buffer, sizeof (buffer));

	dgBodyCollisionList::dgTreeNode* node = dgBodyCollisionList::Find (crc);
	if (!node) {
    	dgCollision* const collision = new  (m_allocator) dgCollisionEllipse (m_allocator, crc, rx, ry, rz, offsetMatrix);
		collision->SetUserDataID (dgUnsigned32 (shapeID));
		node = dgBodyCollisionList::Insert (collision, crc);
	}

	node->GetInfo()->AddRef();
	return node->GetInfo();
}


dgCollision* dgWorld::CreateCapsule (dgFloat32 radius, dgFloat32 height, dgInt32 shapeID, const dgMatrix& offsetMatrix)
{
	dgUnsigned32 crc;
	dgCollision* collision;
	dgBodyCollisionList::dgTreeNode* node;
	dgUnsigned32 buffer[2 * sizeof (dgMatrix) / sizeof(dgInt32)];


	radius = dgAbsf (radius); 
	height = dgAbsf (height);

	memset (buffer, 0, sizeof (buffer));
	buffer[0] = m_capsuleCollision;
	buffer[1] = dgCollision::Quantize (radius);
	buffer[2] = dgCollision::Quantize (height * dgFloat32 (0.5f));
	buffer[3] = dgUnsigned32 (shapeID);
	memcpy (&buffer[4], &offsetMatrix, sizeof (dgMatrix));
	crc = dgCollision::MakeCRC(buffer, sizeof (buffer));

	node = dgBodyCollisionList::Find (crc);
	if (!node) {
		collision = new  (m_allocator) dgCollisionCapsule (m_allocator, crc, radius, height, offsetMatrix);
		collision->SetUserDataID (dgUnsigned32 (shapeID));
		node = dgBodyCollisionList::Insert (collision, crc);
	}

	node->GetInfo()->AddRef();
	return node->GetInfo();
}

dgCollision* dgWorld::CreateCone (dgFloat32 radius, dgFloat32 height, dgInt32 shapeID, const dgMatrix& offsetMatrix)
{
	dgUnsigned32 crc;
	dgCollision* collision;
	dgBodyCollisionList::dgTreeNode* node;
	dgUnsigned32 buffer[2 * sizeof (dgMatrix) / sizeof(dgInt32)];

	radius = dgAbsf (radius); 
	height = dgAbsf (height);

	memset (buffer, 0, sizeof (buffer));
	buffer[0] = m_coneCollision;
	buffer[1] = dgCollision::Quantize (radius);
	buffer[2] = dgCollision::Quantize (height * dgFloat32 (0.5f));
	buffer[3] = dgUnsigned32 (shapeID);
	memcpy (&buffer[4], &offsetMatrix, sizeof (dgMatrix));
	crc = dgCollision::MakeCRC(buffer, sizeof (buffer));

	node = dgBodyCollisionList::Find (crc);
	if (!node) {
		collision = new  (m_allocator) dgCollisionCone (m_allocator, crc, radius, height, offsetMatrix);
		collision->SetUserDataID (dgUnsigned32 (shapeID));
		node = dgBodyCollisionList::Insert (collision, crc);
	}

	node->GetInfo()->AddRef();
	return node->GetInfo();

}


dgCollision* dgWorld::CreateCylinder (dgFloat32 radius, dgFloat32 height, dgInt32 shapeID, const dgMatrix& offsetMatrix)
{
	dgUnsigned32 crc;
	dgCollision* collision;
	dgBodyCollisionList::dgTreeNode* node;
	dgUnsigned32 buffer[2 * sizeof (dgMatrix) / sizeof(dgInt32)];

	radius = dgAbsf (radius); 
	height = dgAbsf (height);

	memset (buffer, 0, sizeof (buffer));
	buffer[0] = m_cylinderCollision;
	buffer[1] = dgCollision::Quantize (radius);
	buffer[2] = dgCollision::Quantize (height * dgFloat32 (0.5f));
	buffer[3] = dgUnsigned32 (shapeID);
	memcpy (&buffer[4], &offsetMatrix, sizeof (dgMatrix));
	crc = dgCollision::MakeCRC(buffer, sizeof (buffer));

	node = dgBodyCollisionList::Find (crc);
	if (!node) {
		collision = new (m_allocator) dgCollisionCylinder (m_allocator, crc, radius, height, offsetMatrix);
		collision->SetUserDataID (dgUnsigned32 (shapeID));
		node = dgBodyCollisionList::Insert (collision, crc);
	}

	node->GetInfo()->AddRef();
	return node->GetInfo();
}


dgCollision* dgWorld::CreateChamferCylinder (dgFloat32 radius, dgFloat32 height, dgInt32 shapeID, const dgMatrix& offsetMatrix)
{
	dgUnsigned32 crc;
	dgCollision* collision;
	dgBodyCollisionList::dgTreeNode* node;
	dgUnsigned32 buffer[2 * sizeof (dgMatrix) / sizeof(dgInt32)];

	radius = dgAbsf (radius); 
	height = dgAbsf (height);

	memset (buffer, 0, sizeof (buffer));
	buffer[0] = m_chamferCylinderCollision;
	buffer[1] = dgCollision::Quantize (radius);
	buffer[2] = dgCollision::Quantize (height * dgFloat32 (0.5f));
	buffer[3] = dgUnsigned32 (shapeID);
	memcpy (&buffer[4], &offsetMatrix, sizeof (dgMatrix));
	crc = dgCollision::MakeCRC(buffer, sizeof (buffer));

	node = dgBodyCollisionList::Find (crc);
	if (!node) {
		collision = new  (m_allocator) dgCollisionChamferCylinder (m_allocator, crc, radius, height, offsetMatrix);
		collision->SetUserDataID (dgUnsigned32 (shapeID));
		node = dgBodyCollisionList::Insert (collision, crc);
	}

	node->GetInfo()->AddRef();
	return node->GetInfo();
}

dgCollision* dgWorld::CreateConvexHull (dgInt32 count, const dgFloat32 *vertexArray, dgInt32 strideInBytes, dgFloat32 tolerance, dgInt32 shapeID, const dgMatrix& offsetMatrix)
{
	dgStack<dgUnsigned32> buffer(2 + 3 * count + dgInt32 (sizeof (dgMatrix) / sizeof (dgInt32)));  

	// create crc signature for cache lookup
	memset (&buffer[0], 0, size_t (buffer.GetSizeInBytes()));
	buffer[0] = m_convexHullCollision;
	buffer[1] = dgUnsigned32 (shapeID);
	dgInt32 stride = dgInt32 (strideInBytes / sizeof (dgFloat32));
	for (dgInt32 i = 0; i < count; i ++) {
		buffer[2 + i * 3 + 0] = dgCollision::Quantize (vertexArray[i * stride + 0]);
		buffer[2 + i * 3 + 1] = dgCollision::Quantize (vertexArray[i * stride + 1]);
		buffer[2 + i * 3 + 2] = dgCollision::Quantize (vertexArray[i * stride + 2]);
	}
	memcpy (&buffer[2 + count * 3], &offsetMatrix, sizeof (dgMatrix));
	dgUnsigned32 crc = dgCollision::MakeCRC(&buffer[0], buffer.GetSizeInBytes());

	// find the shape in cache
	dgBodyCollisionList::dgTreeNode* node = dgBodyCollisionList::Find (crc);
	if (!node) {
		// chape not found crate a new one and add to teh cache
		dgCollisionConvexHull* const collision = new (m_allocator) dgCollisionConvexHull (m_allocator, crc, count, strideInBytes, tolerance, vertexArray, offsetMatrix);
		if (collision->GetVertexCount()) {
			collision->SetUserDataID (dgUnsigned32 (shapeID));
			node = dgBodyCollisionList::Insert (collision, crc);
		} else {
			// could not make the shape destroy the shell and return NULL 
			// note this is the only newton shape that can return NULL;
			collision->Release();
			return NULL;
		}
	}

	// add reference to teh shape and return the collision pointer
	node->GetInfo()->AddRef();
	return node->GetInfo();
}

dgCollision* dgWorld::CreateConvexModifier (dgCollision* convexCollision)
{
	dgCollision* collision;

	collision = NULL;
	if (convexCollision->IsType (dgCollision::dgConvexCollision_RTTI)) {
//		if (!convexCollision->IsType (dgCollision::dgCollisionCompound_RTTI) && !convexCollision->IsType (m_nullType)) {
		if (!convexCollision->IsType (dgCollision::dgCollisionNull_RTTI)) {
			collision = new  (m_allocator) dgCollisionConvexModifier ((dgCollisionConvex*)convexCollision, this);
		}
	}

	return collision;
}


dgCollision* dgWorld::CreateCollisionCompound (dgInt32 count, dgCollision* const array[])
{
	// compound collision are not cached
	return new  (m_allocator) dgCollisionCompound (count, (dgCollisionConvex**) array, this);
}

//dgCollision* dgWorld::CreateCollisionCompoundBreakable (
//	dgInt32 count, 
//	dgMeshEffect* const solidArray[], 
//	dgMeshEffect* const splitePlanes[],
// dgMatrix* const matrixArray, 
//	dgInt32* const idArray, 
//	dgFloat32* const densities,
//	dgInt32 debriID, 
//	dgCollisionCompoundBreakableCallback callback,
//	void* buildUsedData)
dgCollision* dgWorld::CreateCollisionCompoundBreakable (
	dgInt32 count, const dgMeshEffect* const solidArray[], const dgInt32* const idArray, 
	const dgFloat32* const densities, const dgInt32* const internalFaceMaterial, dgInt32 debriID, dgFloat32 gap)
{
//	return new dgCollisionCompoundBreakable (count, solidArray, splitePlanes, matrixArray, idArray, densities, debriID, callback, buildUsedData, this);
	return new  (m_allocator) dgCollisionCompoundBreakable (count, solidArray, idArray, densities, internalFaceMaterial, debriID, gap, this);
}


dgCollision* dgWorld::CreateBVH ()	
{
	// collision tree are not cached
	return new  (m_allocator) dgCollisionBVH (m_allocator);
}

dgCollision* dgWorld::CreateStaticUserMesh (const dgVector& boxP0, const dgVector& boxP1, const dgUserMeshCreation& data)
{
	return new (m_allocator) dgCollisionUserMesh(m_allocator, boxP0, boxP1, data);
}

dgCollision* dgWorld::CreateBVHFieldCollision(
	dgInt32 width, dgInt32 height, dgInt32 contructionMode, 
	const dgUnsigned16* const elevationMap, const dgInt8* const atributeMap, dgFloat32 horizontalScale, dgFloat32 vertcalScale)
{
	return new  (m_allocator) dgCollisionHeightField (this, width, height, contructionMode, elevationMap, atributeMap, horizontalScale, vertcalScale);
}


dgCollision* dgWorld::CreateScene ()
{
	return new  (m_allocator) dgCollisionScene(this);
}

void dgWorld::Serialize (dgCollision* shape, dgSerialize serialization, void* const userData) const
{
	dgInt32 signature[4];

	signature[0] = shape->GetCollisionPrimityType();
	signature[1] = dgInt32 (shape->GetSignature());
	signature[2] = 0;
	signature[3] = 0;
	serialization (userData, signature, sizeof (signature));

	shape->Serialize(serialization, userData);

	dgInt32 end = SERIALIZE_END;
	serialization (userData, &end, sizeof (dgInt32));
}

//dgCollision* dgWorld::CreateFromSerialization (dgInt32 signature, dgCollisionID type, dgDeserialize deserialization, void* const userData)
dgCollision* dgWorld::CreateFromSerialization (dgDeserialize deserialization, void* const userData)
{
	dgInt32 signature[4];

	deserialization (userData, signature, sizeof (signature));

	dgBodyCollisionList::dgTreeNode* node = dgBodyCollisionList::Find (dgUnsigned32 (signature[1]));
	dgCollision* collision = NULL;
	if (node) {
		collision = node->GetInfo();
		collision->AddRef();

	} else {
		switch (signature[0])
		{
			case m_sceneCollision:
			{
				collision = new  (m_allocator) dgCollisionScene (this, deserialization, userData);
				break;
			}

			case m_heightField:
			{
				collision = new  (m_allocator) dgCollisionHeightField (this, deserialization, userData);
				break;
			}
			case m_boundingBoxHierachy:
			{
				collision = new  (m_allocator) dgCollisionBVH (this, deserialization, userData);
				break;
			}

			//if (!stricmp (type, "box")) 
			case m_boxCollision:
			{
				collision = new  (m_allocator) dgCollisionBox (this, deserialization, userData);
				node = dgBodyCollisionList::Insert (collision, collision->GetSignature());
				collision->AddRef();
				break;
			}

			//} else if (!stricmp (type, "cone")) {
			case m_coneCollision:
			{
				collision = new  (m_allocator) dgCollisionCone (this, deserialization, userData);
				node = dgBodyCollisionList::Insert (collision, collision->GetSignature());
				collision->AddRef();
				break;
			}

			//} else if (!stricmp (type, "sphere_1")) {
			case m_ellipseCollision:
			{
				collision = new  (m_allocator) dgCollisionEllipse (this, deserialization, userData);
				node = dgBodyCollisionList::Insert (collision, collision->GetSignature());
				collision->AddRef();
				break;
			}

			//} else if (!stricmp (type, "sphere")) {
			case m_sphereCollision:
			{
				collision = new  (m_allocator) dgCollisionSphere (this, deserialization, userData);
				node = dgBodyCollisionList::Insert (collision, collision->GetSignature());
				collision->AddRef();
				break;
			}

			//} else if (!stricmp (type, "cylinder")) {
			case m_cylinderCollision:
			{
				collision = new  (m_allocator) dgCollisionCylinder (this, deserialization, userData);
				node = dgBodyCollisionList::Insert (collision, collision->GetSignature());
				collision->AddRef();
				break;
			}

			//} else if (!stricmp (type, "capsule")) {
			case m_capsuleCollision:
			{
				collision = new  (m_allocator) dgCollisionCapsule (this, deserialization, userData);
				node = dgBodyCollisionList::Insert (collision, collision->GetSignature());
				collision->AddRef();
				break;
			}

			//} else if (!stricmp (type, "compound")) {
			case m_compoundCollision:
			{
				collision = new  (m_allocator) dgCollisionCompound (this, deserialization, userData);
				break;
			}

			case m_compoundBreakable:
			{
				collision = new  (m_allocator) dgCollisionCompoundBreakable (this, deserialization, userData);
				break;
			}


			//} else if (!stricmp (type, "convexHull")) {
			case m_convexHullCollision:
			{
				collision = new  (m_allocator) dgCollisionConvexHull (this, deserialization, userData);
				node = dgBodyCollisionList::Insert (collision, collision->GetSignature());
				collision->AddRef();
				break;
			}

			case m_convexConvexIntance:
			{
				collision = new  (m_allocator) dgCollisionCompoundBreakable::dgCollisionConvexIntance (this, deserialization, userData);
				break;
			}


			//} else if (!stricmp (type, "chamferCylinder")) {
			case m_chamferCylinderCollision:
			{
				collision = new  (m_allocator) dgCollisionChamferCylinder (this, deserialization, userData);
				node = dgBodyCollisionList::Insert (collision, collision->GetSignature());
				collision->AddRef();
				break;
			}

			case m_nullCollision:
			{
				collision = new  (m_allocator) dgCollisionNull (this, deserialization, userData);
				node = dgBodyCollisionList::Insert (collision, collision->GetSignature());
				collision->AddRef();
				break;
			}

			case m_convexCollisionModifier:	
			{
				collision = new  (m_allocator) dgCollisionConvexModifier (this, deserialization, userData);
				break;
			}


			default:
				_ASSERTE (0);
		}
	}


	dgInt32 endMarker;
	do {
		deserialization (userData, &endMarker, sizeof (dgInt32));
	} while (endMarker != SERIALIZE_END);
	return collision;
}


dgContactMaterial* dgWorld::GetMaterial (dgUnsigned32 bodyGroupId0, dgUnsigned32 bodyGroupId1)	const
{
	if (bodyGroupId0 > bodyGroupId1) {
		Swap (bodyGroupId0, bodyGroupId1);
	}

	dgUnsigned32 key = (bodyGroupId1 << 16) + bodyGroupId0;
	dgBodyMaterialList::dgTreeNode *const node = dgBodyMaterialList::Find (key);

	return node ? &node->GetInfo() : NULL;
}

dgContactMaterial* dgWorld::GetFirstMaterial () const
{
	dgBodyMaterialList::dgTreeNode *const node = dgBodyMaterialList::Minimum();
	_ASSERTE (node);
	return &node->GetInfo();
}

dgContactMaterial* dgWorld::GetNextMaterial (dgContactMaterial* material) const
{
	dgBodyMaterialList::dgTreeNode *const thisNode = dgBodyMaterialList::GetNodeFromInfo (*material);
	_ASSERTE (thisNode);
	dgBodyMaterialList::dgTreeNode *const node = (dgBodyMaterialList::dgTreeNode *)thisNode->Next();
	if (node) {
		return &node->GetInfo();
	}

	return NULL;
}



dgUnsigned32 dgWorld::GetDefualtBodyGroupID() const
{
	return m_defualtBodyGroupID;
}

dgUnsigned32 dgWorld::CreateBodyGroupID()
{
	dgContactMaterial pairMaterial;

	pairMaterial.m_aabbOverlap = NULL;
	pairMaterial.m_contactPoint = NULL;
	pairMaterial.m_compoundAABBOverlap = NULL;

	dgUnsigned32 newId = m_bodyGroupID;
	m_bodyGroupID += 1;
	for (dgUnsigned32 i = 0; i < m_bodyGroupID ; i ++) {
		dgUnsigned32 key = (newId  << 16) + i;

		dgBodyMaterialList::Insert(pairMaterial, key);
	}

	return newId;
}

void dgWorld::RemoveFromCache (dgCollision* const collision)
{
	dgBodyCollisionList::dgTreeNode* const node = dgBodyCollisionList::Find (collision->m_signature);
	if (node) {
		collision->m_signature = 0xffffffff;
		_ASSERTE (node->GetInfo() == collision);
		collision->Release();
		dgBodyCollisionList::Remove (node);
	}
}

void dgWorld::ReleaseCollision(dgCollision* const collision)
{

	if (m_destroyCollision) {
		if (collision->GetRefCount() == 1) {
			m_destroyCollision (this, collision);
		}
	}

	dgInt32 ref = collision->Release();
	if (ref == 1) {
		dgBodyCollisionList::dgTreeNode* const node = dgBodyCollisionList::Find (collision->m_signature);
		if (node) {
			_ASSERTE (node->GetInfo() == collision);
			if (m_destroyCollision) {
				m_destroyCollision (this, collision);
			}
			collision->Release();
			dgBodyCollisionList::Remove (node);
		}
	}

}




// ********************************************************************************
//
// separate collision system 
//
// ********************************************************************************
dgInt32 dgWorld::ClosestPoint (
	dgTriplex& point, 
	dgCollision* const collision, 
	const dgMatrix& matrix, 
	dgTriplex& contact, 
	dgTriplex& normal,
	dgInt32 threadIndex) const
{
	dgTriplex contactA;
	dgMatrix pointMatrix (dgGetIdentityMatrix());

	contact = point;
	pointMatrix.m_posit.m_x = point.m_x;
	pointMatrix.m_posit.m_y = point.m_y;
	pointMatrix.m_posit.m_z = point.m_z;
	return ClosestPoint(collision, matrix, m_pointCollision, pointMatrix, contact, contactA, normal, threadIndex);
}

dgInt32 dgWorld::ClosestPoint(dgCollision* const collisionA, const dgMatrix& matrixA, dgCollision* const collisionB, 
							 const dgMatrix& matrixB, dgTriplex& contactA, dgTriplex& contactB, dgTriplex& normalAB,dgInt32 threadIndex) const
{
	dgBody collideBodyA;
	dgBody collideBodyB;

	collideBodyA.m_matrix = matrixA;
	collideBodyA.m_collision = collisionA;
	collideBodyA.m_collisionWorldMatrix = collisionA->m_offset * matrixA;

	collideBodyB.m_matrix = matrixB;
	collideBodyB.m_collision = collisionB;
	collideBodyB.m_collisionWorldMatrix = collisionB->m_offset * matrixB;

	if (collisionA->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		return ClosestCompoundPoint (&collideBodyA, &collideBodyB, contactA, contactB, normalAB, threadIndex);
	} else if (collisionB->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		dgInt32 flag;
		flag = ClosestCompoundPoint (&collideBodyB, &collideBodyA, contactB, contactA, normalAB, threadIndex);
		normalAB.m_x *= dgFloat32 (-1.0f);
		normalAB.m_y *= dgFloat32 (-1.0f);
		normalAB.m_z *= dgFloat32 (-1.0f);
		return flag;

	} else if (collisionA->IsType (dgCollision::dgConvexCollision_RTTI) && collisionB->IsType (dgCollision::dgConvexCollision_RTTI)) {
		dgInt32 flag;
		dgContactPoint contacts[16];
		dgCollisionParamProxy proxy(threadIndex, &contacts[0]);

		proxy.m_referenceBody = &collideBodyA;
		proxy.m_referenceCollision = collideBodyA.m_collision;
		proxy.m_referenceMatrix = collideBodyA.m_collisionWorldMatrix;

		proxy.m_floatingBody = &collideBodyB;
		proxy.m_floatingCollision = collideBodyB.m_collision;
		proxy.m_floatingMatrix = collideBodyB.m_collisionWorldMatrix ;

		proxy.m_timestep = dgFloat32 (0.0f);
		proxy.m_penetrationPadding = dgFloat32 (0.0f);
		proxy.m_unconditionalCast = 1;
		proxy.m_continueCollision = 0;
		proxy.m_maxContacts = 16;
		//proxy.m_contacts = &contacts[0];
		flag = ClosestPoint (proxy);

		if (flag) {
			contactA.m_x = contacts[0].m_point.m_x;
			contactA.m_y = contacts[0].m_point.m_y;
			contactA.m_z = contacts[0].m_point.m_z;

			contactB.m_x = contacts[1].m_point.m_x;
			contactB.m_y = contacts[1].m_point.m_y;
			contactB.m_z = contacts[1].m_point.m_z;

			normalAB.m_x = contacts[0].m_normal.m_x;
			normalAB.m_y = contacts[0].m_normal.m_y;
			normalAB.m_z = contacts[0].m_normal.m_z;
		}
		return flag;
	}

	return 0;
}


dgInt32 dgWorld::ClosestCompoundPoint (
	dgBody* const compoundConvexA, 
	dgBody* const collisionB, 
	dgTriplex& contactA, 
	dgTriplex& contactB, 
	dgTriplex& normalAB,
	dgInt32 threadIndex) const
{
	dgCollisionCompound* const collision = (dgCollisionCompound*) compoundConvexA->m_collision;
	_ASSERTE (collision->IsType(dgCollision::dgCollisionCompound_RTTI));
	return collision->ClosestDitance(compoundConvexA, contactA, collisionB, contactB, normalAB);

/*
	dgInt32 retFlag;
	dgInt32 count;
	dgMatrix* collisionMatrixArray;
	dgCollisionConvex** collisionArray;
	dgCollisionCompound *compoundCollision;

	dgContactPoint contact0;
	dgContactPoint contact1;

	compoundCollision = (dgCollisionCompound *) compoundConvexA->m_collision;
	count = compoundCollision->m_count;
	collisionArray = compoundCollision->m_array;
	collisionMatrixArray = compoundCollision->m_collisionMatrix;

	retFlag = 0;
	if (collisionB->m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		dgFloat32 minDist2;
		dgCollisionParamProxy proxy(threadIndex);
		dgContactPoint contacts[16];

		proxy.m_referenceBody = compoundConvexA;
		proxy.m_floatingBody = collisionB;
		proxy.m_floatingCollision = collisionB->m_collision;
		proxy.m_floatingMatrix = collisionB->m_collisionWorldMatrix ;

		proxy.m_timestep = dgFloat32 (0.0f);
		proxy.m_penetrationPadding = dgFloat32 (0.0f);
		proxy.m_unconditionalCast = 1;
		proxy.m_continueCollision = 0;
		proxy.m_maxContacts = 16;
		proxy.m_contacts = &contacts[0];

		dgMatrix saveCollMatrix (compoundConvexA->m_collisionWorldMatrix);
		minDist2 = dgFloat32 (1.0e10f);
		
		for (dgInt32 i = 0; i < count; i ++) {
			dgInt32 flag;

			compoundConvexA->m_collision = collisionArray[i];
			compoundConvexA->m_collisionWorldMatrix = collisionArray[i]->m_offset * saveCollMatrix;
			proxy.m_referenceCollision = compoundConvexA->m_collision;
			proxy.m_referenceMatrix = compoundConvexA->m_collisionWorldMatrix;
			flag = ClosestPoint (proxy);
			if (flag) {
				dgFloat32 dist2;
				retFlag = 1;
				dgVector err (contacts[0].m_point - contacts[1].m_point);
				dist2 = err % err;
				if (dist2 < minDist2) {
					minDist2 = dist2;
					contact0 = contacts[0];
					contact1 = contacts[1];
				}
			}
		}

	} else {
		dgInt32 count1;
		dgFloat32 minDist2;
		dgCollisionParamProxy proxy(threadIndex);
		dgContactPoint contacts[16];
		dgMatrix* collisionMatrixArray1;
		dgCollisionConvex** collisionArray1;
		dgCollisionCompound *compoundCollision1;

		_ASSERTE (collisionB->m_collision->IsType (dgCollision::dgCollisionCompound_RTTI));

		dgMatrix saveCollMatrix (compoundConvexA->m_collisionWorldMatrix);
		dgMatrix saveCollMatrix1 (collisionB->m_collisionWorldMatrix);

		compoundCollision1 = (dgCollisionCompound *) collisionB->m_collision;
		count1 = compoundCollision1->m_count;
		collisionArray1 = compoundCollision1->m_array;
		collisionMatrixArray1 = compoundCollision1->m_collisionMatrix;

		proxy.m_referenceBody = compoundConvexA;
		proxy.m_floatingBody = collisionB;
		proxy.m_timestep = dgFloat32 (0.0f);
		proxy.m_penetrationPadding = dgFloat32 (0.0f);
		proxy.m_unconditionalCast = 1;
		proxy.m_continueCollision = 0;
		proxy.m_maxContacts = 16;
		proxy.m_contacts = &contacts[0];

		minDist2 = dgFloat32 (1.0e10f);
		for (dgInt32 i = 0; i < count; i ++) {
			compoundConvexA->m_collision = collisionArray[i];
			compoundConvexA->m_collisionWorldMatrix = collisionArray[i]->m_offset * saveCollMatrix;

			proxy.m_referenceCollision = compoundConvexA->m_collision;
			proxy.m_referenceMatrix = compoundConvexA->m_collisionWorldMatrix;
			for (dgInt32 j = 0; j < count1; j ++) {
				dgInt32 flag;
				collisionB->m_collision = collisionArray1[j];
				collisionB->m_collisionWorldMatrix = collisionArray1[j]->m_offset * saveCollMatrix1;

				proxy.m_floatingCollision = collisionB->m_collision;
				proxy.m_floatingMatrix = collisionB->m_collisionWorldMatrix ;
				flag = ClosestPoint (proxy);
				if (flag) {
					dgFloat32 dist2;
					retFlag = 1;
					dgVector err (contacts[0].m_point - contacts[1].m_point);
					dist2 = err % err;
					if (dist2 < minDist2) {
						minDist2 = dist2;
						contact0 = contacts[0];
						contact1 = contacts[1];
					}
				}
			}
		}
	}

	if (retFlag) {
		contactA.m_x = contact0.m_point.m_x;
		contactA.m_y = contact0.m_point.m_y;
		contactA.m_z = contact0.m_point.m_z;

		contactB.m_x = contact1.m_point.m_x;
		contactB.m_y = contact1.m_point.m_y;
		contactB.m_z = contact1.m_point.m_z;

		normalAB.m_x = contact0.m_normal.m_x;
		normalAB.m_y = contact0.m_normal.m_y;
		normalAB.m_z = contact0.m_normal.m_z;
	}

	return retFlag;
*/
}





// **********************************************************************************
//
// dynamics collision system
//
// **********************************************************************************
static inline dgInt32 CompareContact (const dgContactPoint* const contactA, const dgContactPoint* const contactB, void* dommy)
{
	if (contactA->m_point[0] < contactB->m_point[0]) {
		return -1;
	} else if (contactA->m_point[0] > contactB->m_point[0]) {
		return 1;
	} else {
		return 0;
	}
}

void dgWorld::SortContacts (dgContactPoint* const contact, dgInt32 count) const
{
	dgSort (contact, count, CompareContact, NULL);
}


dgInt32 dgWorld::ReduceContacts (dgInt32 count, dgContactPoint* const contact,  dgInt32 maxCount, dgFloat32 tol, dgInt32 arrayIsSorted) const
{
	if ((count > maxCount) && (maxCount > 1)) {
//		dgInt32 index;
//		dgInt32 countOver;
//		dgFloat32 window;
//		dgFloat32 window2;
		dgUnsigned8 mask[DG_MAX_CONTATCS];

		if (!arrayIsSorted) {
			SortContacts (contact, count);
		}

		dgInt32 index = 0;
		dgFloat32 window = tol;
		dgFloat32 window2 = window * window;
		dgInt32 countOver = count - maxCount;

		_ASSERTE (countOver >= 0);
		memset (mask, 0, size_t (count));
		do {
			for (dgInt32 i = 0; (i < count) && countOver; i ++) {
				if (!mask[i]) {
					dgFloat32 val = contact[i].m_point[index] + window;
					for (dgInt32 j = i + 1; (j < count) && countOver && (contact[j].m_point[index] < val) ; j ++) {
						if (!mask[j]) {
							dgVector dp (contact[j].m_point - contact[i].m_point);
							dgFloat32 dist2 = dp % dp;
							if (dist2 < window2) {
								mask[j] = 1;
								countOver --;
							}
						}
					}
				}
			}
			window *= dgFloat32(2.0f);
			window2 = window * window;

		} while (countOver);

		dgInt32 j = 0;
		for (dgInt32 i = 0; i < count; i ++) {
			if (!mask[i]) {
				contact[j] = contact[i];
				j ++;
			}
		}
		_ASSERTE (j == maxCount);

	} else {
		maxCount = count;
	}

	return maxCount;
}


dgInt32 dgWorld::PruneContacts (dgInt32 count, dgContactPoint* const contact, dgInt32 maxCount) const
{
//	dgInt32 index;
//	dgInt32 packContacts;
//	dgFloat32 window;
//	dgFloat32 window2;
	dgUnsigned8 mask[DG_MAX_CONTATCS];

	dgInt32 index = 0;
	dgInt32 packContacts = 0;
	dgFloat32 window = DG_PRUNE_CONTACT_TOLERANCE;
	dgFloat32 window2 = window * window;

	memset (mask, 0, size_t (count));
	SortContacts (contact, count);

	for (dgInt32 i = 0; i < count; i ++) {
		if (!mask[i]) {
			dgFloat32 val = contact[i].m_point[index] + window;
			for (dgInt32 j = i + 1; (j < count) && (contact[j].m_point[index] < val) ; j ++) {
				if (!mask[j]) {
					dgVector dp (contact[j].m_point - contact[i].m_point);
					dgFloat32 dist2 = dp % dp;
					if (dist2 < window2) {
						mask[j] = 1;
						packContacts = 1;
					}
				}
			}
		}
	}

	if (packContacts) {
		dgInt32 j = 0;
		for (dgInt32 i = 0; i < count; i ++) {
			if (!mask[i]) {
				contact[j] = contact[i];
				j ++;
			}
		}
		count = j;
	}

	if (count > maxCount) {
		count = ReduceContacts (count, contact, maxCount, window * dgFloat32 (2.0f), 1);
	}
	return count;
}



void dgWorld::ProcessCachedContacts (dgContact* const contact, const dgContactMaterial* const material, dgFloat32 timestep, dgInt32 threadIndex) const
{
	_ASSERTE (contact);
	_ASSERTE (contact->m_body0);
	_ASSERTE (contact->m_body1);
	_ASSERTE (contact->m_myCacheMaterial);
	_ASSERTE (contact->m_myCacheMaterial == material);

	_ASSERTE (contact->m_body0 != contact->m_body1);
	dgList<dgContactMaterial>& list = *contact;
	contact->m_broadphaseLru = m_broadPhaseLru;
	contact->m_myCacheMaterial = material;

	dgList<dgContactMaterial>::dgListNode *nextContactNode;
	for (dgList<dgContactMaterial>::dgListNode *contactNode = list.GetFirst(); contactNode; contactNode = nextContactNode) {
		nextContactNode = contactNode->GetNext();
		dgContactMaterial& contactMaterial = contactNode->GetInfo();

		_ASSERTE (dgCheckFloat(contactMaterial.m_point.m_x));
		_ASSERTE (dgCheckFloat(contactMaterial.m_point.m_y));
		_ASSERTE (dgCheckFloat(contactMaterial.m_point.m_z));
		_ASSERTE (contactMaterial.m_body0);
		_ASSERTE (contactMaterial.m_body1);
		_ASSERTE (contactMaterial.m_collision0);
		_ASSERTE (contactMaterial.m_collision1);
		_ASSERTE (contactMaterial.m_body0 == contact->m_body0);
		_ASSERTE (contactMaterial.m_body1 == contact->m_body1);
//		_ASSERTE (contactMaterial.m_userId != 0xffffffff);

		contactMaterial.m_softness = material->m_softness;
		contactMaterial.m_restitution = material->m_restitution;
		contactMaterial.m_staticFriction0 = material->m_staticFriction0;
		contactMaterial.m_staticFriction1 = material->m_staticFriction1;
		contactMaterial.m_dynamicFriction0 = material->m_dynamicFriction0;
		contactMaterial.m_dynamicFriction1 = material->m_dynamicFriction1;

//		contactMaterial.m_collisionEnable = true;
//		contactMaterial.m_friction0Enable = material->m_friction0Enable;
//		contactMaterial.m_friction1Enable = material->m_friction1Enable;
//		contactMaterial.m_override0Accel = false;
//		contactMaterial.m_override1Accel = false;
//		contactMaterial.m_overrideNormalAccel = false;
		contactMaterial.m_flags = dgContactMaterial::m_collisionEnable__ | (material->m_flags & (dgContactMaterial::m_friction0Enable__ | dgContactMaterial::m_friction1Enable__));
		contactMaterial.m_userData = material->m_userData;
	}

	if (material->m_contactPoint) {
		material->m_contactPoint(*contact, timestep, threadIndex);
	}

	contact->m_maxDOF = dgUnsigned32 (3 * contact->GetCount());
}

void dgWorld::ProcessTriggers (dgCollidingPairCollector::dgPair* const pair, dgFloat32 timestep, dgInt32 threadIndex)
{
	dgBody* const body0 = pair->m_body0;
	dgBody* const body1 = pair->m_body1;
	dgContact* contact1 = pair->m_contact;
	const dgContactMaterial* const material = pair->m_material;
	_ASSERTE (body0 != body1);

	if (!contact1) {
//		dgGetUserLock();
		GetLock();
		contact1 = new (m_allocator) dgContact (this);
		pair->m_contact = contact1;
		AttachConstraint (contact1, body0, body1);
//		dgReleasedUserLock();
		ReleaseLock();
	} else if (contact1->GetBody0() != body0) {
		_ASSERTE (0);
		_ASSERTE (contact1->GetBody1() == body0);
		_ASSERTE (contact1->GetBody0() == body1);
		Swap (contact1->m_body0, contact1->m_body1);
		Swap (contact1->m_link0, contact1->m_link1);
	}

	dgContact* const contact = contact1;
	contact->m_myCacheMaterial = material;
	contact->m_broadphaseLru = m_broadPhaseLru;

	_ASSERTE (body0);
	_ASSERTE (body1);
	_ASSERTE (contact->m_body0 == body0);
	_ASSERTE (contact->m_body1 == body1);

	if (material->m_contactPoint) {
		material->m_contactPoint(*contact, timestep, threadIndex);
	}
	contact->m_maxDOF = 0;
}

void dgWorld::ProcessContacts (dgCollidingPairCollector::dgPair* const pair, dgFloat32 timestep, dgInt32 threadIndex)
{
	dgBody* const body0 = pair->m_body0;
	dgBody* const body1 = pair->m_body1;
	dgContact* contact1 = pair->m_contact;
	const dgContactMaterial* const material = pair->m_material;
	dgBroadPhaseCollision* const broadPhase = this;
	_ASSERTE (pair->m_contactBufferIndex >= 0);
//	dgContactPoint* const contactArray = pair->m_contactBuffer;
	dgContactPoint* const contactArray = &broadPhase->m_contactBuffer[pair->m_contactBufferIndex];
	_ASSERTE (body0 != body1);

	if (!contact1) {
		GetLock();
		contact1 = new (m_allocator) dgContact (this);
		pair->m_contact = contact1;
		AttachConstraint (contact1, body0, body1);
		ReleaseLock();
	} else if (contact1->GetBody0() != body0) {
		_ASSERTE (0);
		_ASSERTE (contact1->GetBody1() == body0);
		_ASSERTE (contact1->GetBody0() == body1);
		Swap (contact1->m_body0, contact1->m_body1);
		Swap (contact1->m_link0, contact1->m_link1);
	}

	dgContact* const contact = contact1;
	dgInt32 contactCount = pair->m_contactCount;
	contact->m_myCacheMaterial = material;
	dgList<dgContactMaterial>& list = *contact;

	contact->m_broadphaseLru = m_broadPhaseLru;

	_ASSERTE (body0);
	_ASSERTE (body1);
	_ASSERTE (contact->m_body0 == body0);
	_ASSERTE (contact->m_body1 == body1);

	contact->m_prevPosit0 = body0->m_matrix.m_posit;
	contact->m_prevPosit1 = body1->m_matrix.m_posit;
	contact->m_prevRotation0 = body0->m_rotation;
	contact->m_prevRotation1 = body1->m_rotation;

	dgInt32 count = 0;
	dgVector cachePosition [DG_MAX_CONTATCS];
	dgList<dgContactMaterial>::dgListNode *nodes[DG_MAX_CONTATCS];
	for (dgList<dgContactMaterial>::dgListNode *contactNode = list.GetFirst(); contactNode; contactNode = contactNode->GetNext()) {
		nodes[count] = contactNode;
		cachePosition[count] = contactNode->GetInfo().m_point;
		count ++;
	}


	const dgVector& v0 = body0->m_veloc;
	const dgVector& w0 = body0->m_omega;
	const dgMatrix& matrix0 = body0->m_matrix;

	const dgVector& v1 = body1->m_veloc;
	const dgVector& w1 = body1->m_omega;
	const dgMatrix& matrix1 = body1->m_matrix;

	dgVector controlDir0 (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgVector controlDir1 (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgVector controlNormal (contactArray[0].m_normal);
	dgVector vel0 (v0 + w0 * (contactArray[0].m_point - matrix0.m_posit));
	dgVector vel1 (v1 + w1 * (contactArray[0].m_point - matrix1.m_posit));
	dgVector vRel (vel1 - vel0);
	dgVector tangDir (vRel - controlNormal.Scale (vRel % controlNormal));
	dgFloat32 diff = tangDir % tangDir;

	dgInt32 staticMotion = 0;
//	if (diff > dgFloat32 (1.0e-2f)) {
//		staticMotion = 0;
//	} else {
	if (diff <= dgFloat32 (1.0e-2f)) {
		staticMotion = 1;
		if (dgAbsf (controlNormal.m_z) > dgFloat32 (0.577f)) {
			tangDir = dgVector (-controlNormal.m_y, controlNormal.m_z, dgFloat32 (0.0f), dgFloat32 (0.0f));
		} else {
			tangDir = dgVector (-controlNormal.m_y, controlNormal.m_x, dgFloat32 (0.0f), dgFloat32 (0.0f));
		}
		controlDir0 = controlNormal * tangDir;
		_ASSERTE (controlDir0 % controlDir0 > dgFloat32 (1.0e-8f));
		controlDir0 = controlDir0.Scale (dgRsqrt (controlDir0 % controlDir0));
		controlDir1 = controlNormal * controlDir0;
		_ASSERTE (dgAbsf((controlDir0 * controlDir1) % controlNormal - dgFloat32 (1.0f)) < dgFloat32 (1.0e-3f));
	}

//dgTrace (("contact pair %d %d\n", body0->m_uniqueID, body1->m_uniqueID));

	dgFloat32 maxImpulse = dgFloat32 (-1.0f);
	dgFloat32 breakImpulse0 = dgFloat32 (0.0f);
	dgFloat32 breakImpulse1 = dgFloat32 (0.0f);
	for (dgInt32 i = 0; i < contactCount; i ++) {
		dgList<dgContactMaterial>::dgListNode *contactNode;
		contactNode = NULL;
		dgFloat32 min = dgFloat32 (1.0e20f);
		dgInt32 index = -1;
		for (dgInt32 j = 0; j < count; j ++) {
			dgVector v (cachePosition[j] - contactArray[i].m_point);
			diff = v % v;
			if (diff < min) {
				min = diff;
				index = j;
				contactNode = nodes[j];
			}
		}

		if (contactNode) {
			count --;
			_ASSERTE (index != -1);
			nodes[index] = nodes[count];
			cachePosition[index] = cachePosition[count];
		} else {
			GetLock();
			contactNode = list.Append ();
			ReleaseLock();
		}

		dgContactMaterial* const contactMaterial = &contactNode->GetInfo();

		_ASSERTE (dgCheckFloat(contactArray[i].m_point.m_x));
		_ASSERTE (dgCheckFloat(contactArray[i].m_point.m_y));
		_ASSERTE (dgCheckFloat(contactArray[i].m_point.m_z));
		_ASSERTE (contactArray[i].m_body0);
		_ASSERTE (contactArray[i].m_body1);
		_ASSERTE (contactArray[i].m_collision0);
		_ASSERTE (contactArray[i].m_collision1);
		_ASSERTE (contactArray[i].m_body0 == body0);
		_ASSERTE (contactArray[i].m_body1 == body1);
//		_ASSERTE (contactArray[i].m_userId != 0xffffffff);

		contactMaterial->m_point = contactArray[i].m_point;
		contactMaterial->m_normal = contactArray[i].m_normal;
		contactMaterial->m_userId = contactArray[i].m_userId;
		contactMaterial->m_penetration = contactArray[i].m_penetration;
		contactMaterial->m_body0 = contactArray[i].m_body0;
		contactMaterial->m_body1 = contactArray[i].m_body1;
		contactMaterial->m_collision0 = contactArray[i].m_collision0;
		contactMaterial->m_collision1 = contactArray[i].m_collision1;
		contactMaterial->m_softness = material->m_softness;
		contactMaterial->m_restitution = material->m_restitution;
		contactMaterial->m_staticFriction0 = material->m_staticFriction0;
		contactMaterial->m_staticFriction1 = material->m_staticFriction1;
		contactMaterial->m_dynamicFriction0 = material->m_dynamicFriction0;
		contactMaterial->m_dynamicFriction1 = material->m_dynamicFriction1;

  		_ASSERTE ((dgAbsf(contactMaterial->m_normal % contactMaterial->m_normal) - dgFloat32 (1.0f)) < dgFloat32 (1.0e-5f));

//dgTrace (("%f\n", contactMaterial.m_penetration));
//dgTrace (("p(%f %f %f) n(%f %f %f)\n", contactMaterial.m_point.m_x, contactMaterial.m_point.m_y, contactMaterial.m_point.m_z, 
//									   contactMaterial.m_normal.m_x, contactMaterial.m_normal.m_y, contactMaterial.m_normal.m_z));

//		contactMaterial.m_collisionEnable = true;
//		contactMaterial.m_friction0Enable = material->m_friction0Enable;
//		contactMaterial.m_friction1Enable = material->m_friction1Enable;
//		contactMaterial.m_override0Accel = false;
//		contactMaterial.m_override1Accel = false;
//		contactMaterial.m_overrideNormalAccel = false;
		contactMaterial->m_flags = dgContactMaterial::m_collisionEnable__ | (material->m_flags & (dgContactMaterial::m_friction0Enable__ | dgContactMaterial::m_friction1Enable__));
		contactMaterial->m_userData = material->m_userData;

		if (staticMotion) {
			if ((contactMaterial->m_normal % controlNormal) > dgFloat32 (0.9995f)) {
				contactMaterial->m_dir0 = controlDir0;
				contactMaterial->m_dir1 = controlDir1;
			} else {
				if (dgAbsf (contactMaterial->m_normal.m_z) > dgFloat32 (0.577f)) {
					tangDir = dgVector (-contactMaterial->m_normal.m_y, contactMaterial->m_normal.m_z, dgFloat32 (0.0f), dgFloat32 (0.0f));
				} else {
					tangDir = dgVector (-contactMaterial->m_normal.m_y, contactMaterial->m_normal.m_x, dgFloat32 (0.0f), dgFloat32 (0.0f));
				}
				contactMaterial->m_dir0 = contactMaterial->m_normal * tangDir;
				_ASSERTE (contactMaterial->m_dir0 % contactMaterial->m_dir0 > dgFloat32 (1.0e-8f));
				contactMaterial->m_dir0 = contactMaterial->m_dir0.Scale (dgRsqrt (contactMaterial->m_dir0 % contactMaterial->m_dir0));
				contactMaterial->m_dir1 = contactMaterial->m_normal * contactMaterial->m_dir0;
				_ASSERTE (dgAbsf((contactMaterial->m_dir0 * contactMaterial->m_dir1) % contactMaterial->m_normal - dgFloat32 (1.0f)) < dgFloat32 (1.0e-3f));
			}

		} else {
			dgVector vel0 (v0 + w0 * (contactMaterial->m_point - matrix0.m_posit));
			dgVector vel1 (v1 + w1 * (contactMaterial->m_point - matrix1.m_posit));
			dgVector vRel (vel1 - vel0);

			dgFloat32 impulse = vRel % contactMaterial->m_normal;
			if (dgAbsf (impulse) > maxImpulse) {
				maxImpulse = dgAbsf (impulse); 
				breakImpulse0 = contactMaterial->m_collision0->GetBreakImpulse();
				breakImpulse1 = contactMaterial->m_collision1->GetBreakImpulse();
			}
			
			dgVector tangDir (vRel - contactMaterial->m_normal.Scale (impulse));
			diff = tangDir % tangDir;

			if (diff > dgFloat32 (1.0e-2f)) {
				contactMaterial->m_dir0 = tangDir.Scale (dgRsqrt (diff));
			} else {
				if (dgAbsf (contactMaterial->m_normal.m_z) > dgFloat32 (0.577f)) {
					tangDir = dgVector (-contactMaterial->m_normal.m_y, contactMaterial->m_normal.m_z, dgFloat32 (0.0f), dgFloat32 (0.0f));
				} else {
					tangDir = dgVector (-contactMaterial->m_normal.m_y, contactMaterial->m_normal.m_x, dgFloat32 (0.0f), dgFloat32 (0.0f));
				}
				contactMaterial->m_dir0 = contactMaterial->m_normal * tangDir;
				_ASSERTE (contactMaterial->m_dir0 % contactMaterial->m_dir0 > dgFloat32 (1.0e-8f));
				contactMaterial->m_dir0 = contactMaterial->m_dir0.Scale (dgRsqrt (contactMaterial->m_dir0 % contactMaterial->m_dir0));
			}
			contactMaterial->m_dir1 = contactMaterial->m_normal * contactMaterial->m_dir0;
			_ASSERTE (dgAbsf((contactMaterial->m_dir0 * contactMaterial->m_dir1) % contactMaterial->m_normal - dgFloat32 (1.0f)) < dgFloat32 (1.0e-3f));
		}
		contactMaterial->m_normal.m_w = dgFloat32 (0.0f);
		contactMaterial->m_dir0.m_w = dgFloat32 (0.0f); 
		contactMaterial->m_dir1.m_w = dgFloat32 (0.0f); 
	}

	for (dgInt32 i = 0; i < count; i ++) {
		GetLock();
		list.Remove(nodes[i]);
		ReleaseLock();
	}

	if (material->m_contactPoint) {
		material->m_contactPoint(*contact, timestep, threadIndex);
	}

	if (maxImpulse > dgFloat32 (1.0f)) {
		maxImpulse /= (body0->m_invMass.m_w + body1->m_invMass.m_w) ;
		if ((maxImpulse > breakImpulse0) || (maxImpulse > breakImpulse1)) {
			GetLock();
			if (maxImpulse > breakImpulse0) {
				AddToBreakQueue (contact, body0, maxImpulse);
			}
			if (maxImpulse > breakImpulse1) {
				AddToBreakQueue (contact, body1, maxImpulse);
			}
			ReleaseLock();
		}
	}

	contact->m_maxDOF = dgUnsigned32 (3 * contact->GetCount());
}


dgInt32 dgWorld::ValidateContactCache (dgBody* const convexBody, dgBody* const otherBody, dgContact* const contact) const
{
	_ASSERTE (contact && (contact->GetId() == dgContactConstraintId));
	_ASSERTE ((contact->GetBody0() == otherBody) || (contact->GetBody1() == otherBody));
	_ASSERTE ((contact->GetBody0() == convexBody) || (contact->GetBody1() == convexBody));

	dgInt32 contactCount = 0;

#ifdef DG_USE_CACHE_CONTACTS
	#define DG_CACHE_DIST_TOL dgFloat32 (1.0e-3f)

	dgBody* const body0 = contact->GetBody0();
	dgVector error0 (contact->m_prevPosit0 - body0->m_matrix.m_posit);
	dgFloat32 err2 = error0 % error0;
	if (err2 < (DG_CACHE_DIST_TOL * DG_CACHE_DIST_TOL)) {
		dgBody* const body1 = contact->GetBody1();
		dgVector error1 (contact->m_prevPosit1 - body1->m_matrix.m_posit);
		err2 = error1 % error1;
		if (err2 < (DG_CACHE_DIST_TOL * DG_CACHE_DIST_TOL)) {
			dgQuaternion errorRot0 (contact->m_prevRotation0 - body0->m_rotation);
			err2 = errorRot0.DotProduct(errorRot0);
			if (err2 < (DG_CACHE_DIST_TOL * DG_CACHE_DIST_TOL)) {
				dgQuaternion errorRot1 (contact->m_prevRotation1 - body1->m_rotation);
				err2 = errorRot1.DotProduct(errorRot1);
				if (err2 < (DG_CACHE_DIST_TOL * DG_CACHE_DIST_TOL)) {
					dgMatrix matrix0 (dgMatrix (contact->m_prevRotation0, contact->m_prevPosit0).Inverse() * body0->m_matrix);
					dgMatrix matrix1 (dgMatrix (contact->m_prevRotation1, contact->m_prevPosit1).Inverse() * body1->m_matrix);

					dgList<dgContactMaterial>& list = *contact;
					for (dgList<dgContactMaterial>::dgListNode* ptr = list.GetFirst(); ptr; ptr = ptr->GetNext()) {
						dgContactMaterial& contactMaterial = ptr->GetInfo();
						dgVector p0 (matrix0.TransformVector (contactMaterial.m_point));
						dgVector p1 (matrix1.TransformVector (contactMaterial.m_point));
						dgVector error (p1 - p0);

						err2 = error % error;
						if (err2 > (DG_CACHE_DIST_TOL * DG_CACHE_DIST_TOL)) {
							contactCount = 0;
							break;
						}
						contactCount ++;
					}
				}
			}
		}
	}


#endif

	return contactCount;
}



void dgWorld::CompoundContactsSimd (dgCollidingPairCollector::dgPair* const pair, dgCollisionParamProxy& proxy) const
{
	_ASSERTE(0);
/*
	dgInt32 contactCount;
	dgBody* otherBody; 
	dgBody* compoundBody;
	dgContact* constraint;
	dgContactPoint* const contacts = pair->m_contactBuffer;

	constraint = pair->m_contact;
	compoundBody = pair->m_body0;
	otherBody = pair->m_body1;

	pair->m_contactCount = 0;
	proxy.m_contacts = contacts;

	pair->m_isTrigger = 0;
	proxy.m_isTriggerVolume = 0;
	proxy.m_inTriggerVolume = 0;

	if (constraint) {
		contactCount = ValidateContactCache (compoundBody, otherBody, constraint);
		if (contactCount) {
			pair->m_isTrigger = 0;
			pair->m_contactCount = 0;
			//pair->m_contactBuffer = NULL;
			_ASSERTE (pair->m_contactBufferIndex == -1);
			pair->m_contactBufferIndex = 0;
			return ;
		}
	}

	contactCount = ((dgCollisionCompound *) compoundBody->m_collision)->CalculateContacts(pair, proxy, 1);

	if (contactCount) {
		// prune close contacts
		pair->m_contactCount = dgInt16 (PruneContacts (contactCount, contacts));
	}
*/
}


void dgWorld::CompoundContacts (dgCollidingPairCollector::dgPair* const pair, dgCollisionParamProxy& proxy) const
{
	_ASSERTE(0);
/*
	dgInt32 contactCount;
	dgBody* otherBody; 
	dgBody* compoundBody;
	dgContact* constraint;
	dgContactPoint* const contacts = pair->m_contactBuffer;

	constraint = pair->m_contact;
	compoundBody = pair->m_body0;
	otherBody = pair->m_body1;

	pair->m_contactCount = 0;
	proxy.m_contacts = contacts;

	pair->m_isTrigger = 0;
	proxy.m_isTriggerVolume = 0;
	proxy.m_inTriggerVolume = 0;

	if (constraint) {
		contactCount = ValidateContactCache (compoundBody, otherBody, constraint);
		if (contactCount) {
			pair->m_isTrigger = 0;
			pair->m_contactCount = 0;
			//pair->m_contactBuffer = NULL;
			_ASSERTE (pair->m_contactBufferIndex == -1);
			pair->m_contactBufferIndex = 0;
			return ;
		}
	}
	
	contactCount = ((dgCollisionCompound *) compoundBody->m_collision)->CalculateContacts(pair, proxy, 0);

	if (contactCount) {
		// prune close contacts
		pair->m_contactCount = dgInt16 (PruneContacts (contactCount, contacts));
	}
*/
}



void dgWorld::ConvexContactsSimd (dgCollidingPairCollector::dgPair* const pair, dgCollisionParamProxy& proxy) const
{
	dgContact* const constraint = pair->m_contact;
	dgBody* convexBody = pair->m_body0;
	dgBody* otherBody = pair->m_body1;
	if (constraint) {
		dgInt32 contactCount = ValidateContactCache (convexBody, otherBody, constraint);
		if (contactCount) {
			pair->m_isTrigger = 0;
			pair->m_contactCount = 0;
			_ASSERTE (pair->m_contactBufferIndex == -1);
			pair->m_contactBufferIndex = 0;
			return ;
		}
	}

	if (otherBody->m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		if (convexBody->m_invMass.m_w <= dgFloat32 (1.0e-6f)) {
			Swap (convexBody, otherBody);
			pair->m_body0 = convexBody;
			pair->m_body1 = otherBody;
		}

		_ASSERTE (convexBody->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));
		_ASSERTE (otherBody->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));
		_ASSERTE (pair->m_body0->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));
		_ASSERTE (pair->m_body1->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));

		proxy.m_referenceBody = convexBody;
		proxy.m_floatingBody = otherBody;
		proxy.m_referenceCollision = convexBody->m_collision;
		proxy.m_floatingCollision = otherBody->m_collision;
		proxy.m_referenceMatrix = convexBody->m_collisionWorldMatrix;
		proxy.m_floatingMatrix = otherBody->m_collisionWorldMatrix;
		pair->m_contactCount =  dgInt16 (CalculateConvexToConvexContactsSimd (proxy));
		pair->m_isTrigger = proxy.m_inTriggerVolume;

	} else {
		_ASSERTE (pair->m_body0->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));
		_ASSERTE (convexBody->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));

		proxy.m_referenceBody = convexBody;
		proxy.m_floatingBody = otherBody;
		proxy.m_referenceCollision = convexBody->m_collision;
		proxy.m_floatingCollision = otherBody->m_collision;
		proxy.m_referenceMatrix = convexBody->m_collisionWorldMatrix ;
		proxy.m_floatingMatrix = otherBody->m_collisionWorldMatrix;
		pair->m_contactCount = dgInt16 (CalculateConvexToNonConvexContactsSimd (proxy));
		pair->m_isTrigger = proxy.m_inTriggerVolume;
	}


}


void dgWorld::ConvexContacts (dgCollidingPairCollector::dgPair* const pair, dgCollisionParamProxy& proxy) const
{
	dgContact* const constraint = pair->m_contact;

	dgBody* convexBody = pair->m_body0;
	dgBody* otherBody = pair->m_body1;
	if (constraint) {
		dgInt32 contactCount = ValidateContactCache (convexBody, otherBody, constraint);
		if (contactCount) {
			pair->m_isTrigger = 0;
			pair->m_contactCount = 0;
			_ASSERTE (pair->m_contactBufferIndex == -1);
			pair->m_contactBufferIndex = 0;
			return ;
		}
	}

//	proxy.m_maxContacts = DG_MAX_CONTATCS;
//	proxy.m_contacts = pair->m_contactBuffer;

	if (otherBody->m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		if (convexBody->m_invMass.m_w <= dgFloat32 (1.0e-6f)) {
			Swap (convexBody, otherBody);
			pair->m_body0 = convexBody;
			pair->m_body1 = otherBody;
		}

		_ASSERTE (convexBody->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));
		_ASSERTE (otherBody->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));
		_ASSERTE (pair->m_body0->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));
		_ASSERTE (pair->m_body1->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));

		proxy.m_referenceBody = convexBody;
		proxy.m_floatingBody = otherBody;
		proxy.m_referenceCollision = convexBody->m_collision;
		proxy.m_floatingCollision = otherBody->m_collision;
		proxy.m_referenceMatrix = convexBody->m_collisionWorldMatrix;
		proxy.m_floatingMatrix = otherBody->m_collisionWorldMatrix;
		pair->m_contactCount =  dgInt16 (CalculateConvexToConvexContacts (proxy));
		pair->m_isTrigger = proxy.m_inTriggerVolume;

	} else {
		_ASSERTE (pair->m_body0->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));
		_ASSERTE (convexBody->m_collision->IsType (dgCollision::dgConvexCollision_RTTI));

		proxy.m_referenceBody = convexBody;
		proxy.m_floatingBody = otherBody;
		proxy.m_referenceCollision = convexBody->m_collision;
		proxy.m_floatingCollision = otherBody->m_collision;
		proxy.m_referenceMatrix = convexBody->m_collisionWorldMatrix ;
		proxy.m_floatingMatrix = otherBody->m_collisionWorldMatrix;
		pair->m_contactCount = dgInt16 (CalculateConvexToNonConvexContacts (proxy));
		pair->m_isTrigger = proxy.m_inTriggerVolume;
	}
}



void dgWorld::SceneContactsSimd (const dgCollisionScene::dgProxy& sceneProxy, dgCollidingPairCollector::dgPair* const pair, dgCollisionParamProxy& proxy) const
{
	_ASSERTE(0);
/*

	_ASSERTE (pair->m_body1->GetCollision()->IsType(dgCollision::dgCollisionScene_RTTI));
	if (sceneProxy.m_shape->IsType (dgCollision::dgConvexCollision_RTTI)) {
		proxy.m_floatingCollision = sceneProxy.m_shape;
		proxy.m_floatingMatrix = sceneProxy.m_matrix;
		proxy.m_maxContacts = ((DG_MAX_CONTATCS - pair->m_contactCount) > 16) ? 16 : DG_MAX_CONTATCS - pair->m_contactCount;

		proxy.m_contacts = &pair->m_contactBuffer[pair->m_contactCount];
		pair->m_contactCount = pair->m_contactCount + dgInt16 (CalculateConvexToConvexContactsSimd (proxy));
		if (pair->m_contactCount > (DG_MAX_CONTATCS - 2 * (DG_CONSTRAINT_MAX_ROWS / 3))) {
			pair->m_contactCount = dgInt16 (ReduceContacts (pair->m_contactCount, pair->m_contactBuffer, DG_CONSTRAINT_MAX_ROWS / 3, DG_REDUCE_CONTACT_TOLERANCE));
		}
//		pair->m_isTrigger = proxy.m_inTriggerVolume;

	} else {
		proxy.m_floatingCollision = sceneProxy.m_shape;
		proxy.m_floatingMatrix = sceneProxy.m_matrix;
		proxy.m_maxContacts = ((DG_MAX_CONTATCS - pair->m_contactCount) > 32) ? 32 : DG_MAX_CONTATCS - pair->m_contactCount;

		proxy.m_contacts = &pair->m_contactBuffer[pair->m_contactCount];
		pair->m_contactCount = pair->m_contactCount + dgInt16 (CalculateConvexToNonConvexContactsSimd (proxy));
		if (pair->m_contactCount > (DG_MAX_CONTATCS - 2 * (DG_CONSTRAINT_MAX_ROWS / 3))) {
			pair->m_contactCount = dgInt16 (ReduceContacts (pair->m_contactCount, pair->m_contactBuffer, DG_CONSTRAINT_MAX_ROWS / 3, DG_REDUCE_CONTACT_TOLERANCE));
		}
//		pair->m_isTrigger = proxy.m_inTriggerVolume;
	}
*/
}


void dgWorld::SceneContacts (const dgCollisionScene::dgProxy& sceneProxy, dgCollidingPairCollector::dgPair* const pair, dgCollisionParamProxy& proxy) const
{

	_ASSERTE (pair->m_body1->GetCollision()->IsType(dgCollision::dgCollisionScene_RTTI));
	dgContactPoint* const savedBuffer = proxy.m_contacts;
	if (sceneProxy.m_shape->IsType (dgCollision::dgConvexCollision_RTTI)) {
		proxy.m_floatingCollision = sceneProxy.m_shape;
		proxy.m_floatingMatrix = sceneProxy.m_matrix;
		proxy.m_maxContacts = ((DG_MAX_CONTATCS - pair->m_contactCount) > 16) ? 16 : DG_MAX_CONTATCS - pair->m_contactCount;

		//proxy.m_contacts = &pair->m_contactBuffer[pair->m_contactCount];
		proxy.m_contacts = &savedBuffer[pair->m_contactCount];
		pair->m_contactCount = pair->m_contactCount + dgInt16 (CalculateConvexToConvexContacts (proxy));
		if (pair->m_contactCount > (DG_MAX_CONTATCS - 2 * (DG_CONSTRAINT_MAX_ROWS / 3))) {
			pair->m_contactCount = dgInt16 (ReduceContacts (pair->m_contactCount, savedBuffer, DG_CONSTRAINT_MAX_ROWS / 3, DG_REDUCE_CONTACT_TOLERANCE));
		}
//		pair->m_isTrigger = proxy.m_inTriggerVolume;

	} else {
		proxy.m_floatingCollision = sceneProxy.m_shape;
		proxy.m_floatingMatrix = sceneProxy.m_matrix;
		proxy.m_maxContacts = ((DG_MAX_CONTATCS - pair->m_contactCount) > 32) ? 32 : DG_MAX_CONTATCS - pair->m_contactCount;

		//proxy.m_contacts = &pair->m_contactBuffer[pair->m_contactCount];
		proxy.m_contacts = &savedBuffer[pair->m_contactCount];
		pair->m_contactCount = pair->m_contactCount + dgInt16 (CalculateConvexToNonConvexContacts (proxy));
		if (pair->m_contactCount > (DG_MAX_CONTATCS - 2 * (DG_CONSTRAINT_MAX_ROWS / 3))) {
			pair->m_contactCount = dgInt16 (ReduceContacts (pair->m_contactCount, savedBuffer, DG_CONSTRAINT_MAX_ROWS / 3, DG_REDUCE_CONTACT_TOLERANCE));
		}
//		pair->m_isTrigger = proxy.m_inTriggerVolume;
	}

	proxy.m_contacts = savedBuffer;
}

void dgWorld::SceneContacts (dgCollidingPairCollector::dgPair* const pair, dgCollisionParamProxy& proxy) const
{
	dgContact* const constraint = pair->m_contact;

	pair->m_isTrigger = 0;
	pair->m_contactCount = 0;
	
	proxy.m_isTriggerVolume = 0;
	proxy.m_inTriggerVolume = 0;
//	proxy.m_contacts = contacts;

//	_ASSERTE (pair->m_body0->m_invMass.m_w != dgFloat32 (0.0f));
//	_ASSERTE (pair->m_body1->m_invMass.m_w == dgFloat32 (0.0f));

	if (constraint) {
		_ASSERTE (0);
/*
		dgInt32 contactCount = ValidateContactCache (pair->m_body0, pair->m_body1, constraint);
		if (contactCount) {
			_ASSERTE (0);
			pair->m_isTrigger = 0;
			pair->m_contactCount = 0;
			//pair->m_contactBuffer = NULL;
			_ASSERTE (pair->m_contactBufferIndex == -1);
			pair->m_contactBufferIndex = 0;
			return ;
		}
*/
	}

	dgCollisionScene* const scene = (dgCollisionScene*) pair->m_body1->GetCollision();
	_ASSERTE (scene->IsType(dgCollision::dgCollisionScene_RTTI));
	if (pair->m_body0->GetCollision()->IsType (dgCollision::dgConvexCollision_RTTI)) {
		proxy.m_referenceBody = pair->m_body0;
		proxy.m_floatingBody = pair->m_body1;
		proxy.m_referenceCollision = pair->m_body0->m_collision;
		proxy.m_floatingCollision = NULL;
		proxy.m_referenceMatrix = pair->m_body0->m_collisionWorldMatrix ;

		scene->CollidePair (pair, proxy);
		if (pair->m_contactCount) {
			// prune close contacts
			pair->m_contactCount = dgInt16 (PruneContacts (pair->m_contactCount, proxy.m_contacts, pair->m_contactCount));
		}
	} else {
		_ASSERTE (0);
	}

}


void dgWorld::SceneContactsSimd (dgCollidingPairCollector::dgPair* const pair, dgCollisionParamProxy& proxy) const
{
	_ASSERTE(0);
/*
	dgContact* constraint;
	dgCollisionScene* scene;

	constraint = pair->m_contact;

	pair->m_isTrigger = 0;
	pair->m_contactCount = 0;
	
	proxy.m_isTriggerVolume = 0;
	proxy.m_inTriggerVolume = 0;
//	proxy.m_contacts = contacts;

//	_ASSERTE (pair->m_body0->m_invMass.m_w != dgFloat32 (0.0f));
//	_ASSERTE (pair->m_body1->m_invMass.m_w == dgFloat32 (0.0f));
	if (constraint) {
		dgInt32 contactCount = ValidateContactCache (pair->m_body0, pair->m_body1, constraint);
		if (contactCount) {
			//pair->m_contactCount = 0;
			//pair->m_contactBuffer = NULL;

			pair->m_isTrigger = 0;
			pair->m_contactCount = 0;
			//pair->m_contactBuffer = NULL;
			_ASSERTE (pair->m_contactBufferIndex == -1);
			pair->m_contactBufferIndex = 0;
			return ;
		}
	}

	scene = (dgCollisionScene*) pair->m_body1->GetCollision();
	_ASSERTE (scene->IsType(dgCollision::dgCollisionScene_RTTI));
	if (pair->m_body0->GetCollision()->IsType (dgCollision::dgConvexCollision_RTTI)) {
		proxy.m_referenceBody = pair->m_body0;
		proxy.m_floatingBody = pair->m_body1;
		proxy.m_referenceCollision = pair->m_body0->m_collision;
		proxy.m_floatingCollision = NULL;
		proxy.m_referenceMatrix = pair->m_body0->m_collisionWorldMatrix ;

		scene->CollidePairSimd (pair, proxy);

		if (pair->m_contactCount) {
			// prune close contacts
			pair->m_contactCount = dgInt16 (PruneContacts (pair->m_contactCount, pair->m_contactBuffer));
		}

	} else {
		_ASSERTE (0);
	}
*/
}


void dgWorld::CalculateContactsSimd (dgCollidingPairCollector::dgPair* const pair, dgContactPoint* const contactBuffer, dgFloat32 timestep, dgInt32 threadIndex)
{
	dgCollisionParamProxy proxy(threadIndex, contactBuffer);

	dgBody* const body0 = pair->m_body0;
	dgBody* const body1 = pair->m_body1;

	const dgContactMaterial* const material = pair->m_material;
	proxy.m_timestep = timestep;
	proxy.m_unconditionalCast = 0;
	proxy.m_maxContacts = DG_MAX_CONTATCS;
	proxy.m_penetrationPadding = material->m_penetrationPadding;
//	proxy.m_continueCollision = material->m_collisionContinueCollisionEnable & (body0->m_continueCollisionMode | body1->m_continueCollisionMode);
	proxy.m_continueCollision = ((material->m_flags & dgContactMaterial::m_collisionContinueCollisionEnable__) ? 1 : 0) & (body0->m_continueCollisionMode | body1->m_continueCollisionMode);
	proxy.m_isTriggerVolume = body0->m_collision->IsTriggerVolume() | body1->m_collision->IsTriggerVolume();

	if (body0->m_collision->IsType (dgCollision::dgCollisionScene_RTTI)) {
		Swap(pair->m_body0, pair->m_body1);
		SceneContactsSimd (pair, proxy);
	} else if (body1->m_collision->IsType (dgCollision::dgCollisionScene_RTTI)) {
		SceneContactsSimd (pair, proxy);
	} else if (body0->m_collision->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		CompoundContactsSimd (pair, proxy);
	} else if (body1->m_collision->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		Swap(pair->m_body0, pair->m_body1);
		CompoundContactsSimd (pair, proxy);
	} else if (body0->m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		ConvexContactsSimd (pair, proxy);
	} else if (body1->m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		Swap(pair->m_body0, pair->m_body1);
		ConvexContactsSimd (pair, proxy);
	}
}


void dgWorld::CalculateContacts (dgCollidingPairCollector::dgPair* const pair, dgContactPoint* const contactBuffer, dgFloat32 timestep, dgInt32 threadIndex)
{
	dgCollisionParamProxy proxy(threadIndex, contactBuffer);

	dgBody* const body0 = pair->m_body0;
	dgBody* const body1 = pair->m_body1;

	const dgContactMaterial* const material = pair->m_material;
	proxy.m_timestep = timestep;
	proxy.m_unconditionalCast = 0;
	proxy.m_maxContacts = DG_MAX_CONTATCS;
	proxy.m_penetrationPadding = material->m_penetrationPadding;
	proxy.m_isTriggerVolume = body0->m_collision->IsTriggerVolume() | body1->m_collision->IsTriggerVolume();
//	proxy.m_continueCollision = material->m_collisionContinueCollisionEnable & (body0->m_continueCollisionMode | body1->m_continueCollisionMode);
	proxy.m_continueCollision = dgInt32 (((material->m_flags & dgContactMaterial::m_collisionContinueCollisionEnable__) ? 1 : 0) & (body0->m_continueCollisionMode | body1->m_continueCollisionMode));

	if (body0->m_collision->IsType (dgCollision::dgCollisionScene_RTTI)) {
		Swap(pair->m_body0, pair->m_body1);
		SceneContacts (pair, proxy);
	} else if (body1->m_collision->IsType (dgCollision::dgCollisionScene_RTTI)) {
		SceneContacts (pair, proxy);
	}else if (body0->m_collision->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		CompoundContacts (pair, proxy);
	} else if (body1->m_collision->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		Swap(pair->m_body0, pair->m_body1);
		CompoundContacts (pair, proxy);
	} else if (body0->m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		ConvexContacts (pair, proxy);
	} else if (body1->m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		Swap(pair->m_body0, pair->m_body1);
		ConvexContacts (pair, proxy);
	}
}


// ***************************************************************************
//
// ***************************************************************************

dgInt32 dgWorld::CollideContinueSimd (dgCollision* collisionA, const dgMatrix& matrixA, const dgVector& velocA, 
	const dgVector& omegaA, dgCollision* collisionB, const dgMatrix& matrixB, const dgVector& velocB, const dgVector& omegaB, 
	dgFloat32& retTimeStep, dgTriplex* points, dgTriplex* normals, dgFloat32* penetration, dgInt32 maxSize, dgInt32 threadIndex)
{
_ASSERTE (0);
return 0;
/*
	dgInt32 count;
	dgBody collideBodyA;
	dgBody collideBodyB;
	dgContactPoint contacts[DG_MAX_CONTATCS];

	count = 0;
	retTimeStep = dgFloat32 (1.0e10f);
	maxSize = GetMin (DG_MAX_CONTATCS, maxSize);


	collideBodyA.m_world = this;
	collideBodyA.SetContinuesCollisionMode(true); 
	collideBodyA.m_matrix = matrixA;
	collideBodyA.m_collision = collisionA;
	collideBodyA.m_masterNode = NULL;
	collideBodyA.m_collisionCell.m_cell = NULL;
	collideBodyA.m_collisionWorldMatrix = collisionA->m_offset * matrixA;
	collideBodyA.m_veloc = dgVector (velocA[0], velocA[1], velocA[2], dgFloat32 (0.0f));
	collideBodyA.m_omega = dgVector (omegaA[0], omegaA[1], omegaA[2], dgFloat32 (0.0f));
	collideBodyA.m_accel = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	collideBodyA.m_alpha = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	collideBodyA.m_invMass = dgVector (dgFloat32 (1.0f), dgFloat32 (1.0f), dgFloat32 (1.0f), dgFloat32 (1.0f));
	collideBodyA.UpdateCollisionMatrixSimd(dgFloat32 (1.0f), 0);

	collideBodyB.m_world = this;
	collideBodyB.SetContinuesCollisionMode(true); 
	collideBodyB.m_matrix = matrixB;
	collideBodyB.m_collision = collisionB;
	collideBodyB.m_masterNode = NULL;
	collideBodyB.m_collisionCell.m_cell = NULL;
	collideBodyB.m_collisionWorldMatrix = collisionB->m_offset * matrixB;
	collideBodyB.m_veloc = dgVector (velocB[0], velocB[1], velocB[2], dgFloat32 (0.0f));
	collideBodyB.m_omega = dgVector (omegaB[0], omegaB[1], omegaB[2], dgFloat32 (0.0f));
	collideBodyB.m_accel = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	collideBodyB.m_alpha = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	collideBodyB.m_invMass = dgVector (dgFloat32 (1.0f), dgFloat32 (1.0f), dgFloat32 (1.0f), dgFloat32 (1.0f));
	collideBodyB.UpdateCollisionMatrixSimd(dgFloat32 (1.0f), 0);

	dgCollisionParamProxy proxy(threadIndex);
	proxy.m_timestep = dgFloat32 (1.0f);
	proxy.m_unconditionalCast = 1;
	proxy.m_penetrationPadding = 0.0f;
	proxy.m_continueCollision = 1;
//	proxy.m_maxContacts = DG_MAX_CONTATCS;
	proxy.m_maxContacts = maxSize;
	proxy.m_isTriggerVolume = 0;


	dgCollidingPairCollector::dgPair pair;
	pair.m_body0 = &collideBodyA;
	pair.m_body1 = &collideBodyB;
	pair.m_contact = NULL;
	pair.m_material = NULL;
	pair.m_contactCount = 0;
	pair.m_contactBuffer = contacts;


	dgFloat32 swapContactScale = dgFloat32 (1.0f);
	if (collideBodyA.m_collision->IsType (dgCollision::dgCollisionScene_RTTI)) {
		swapContactScale = dgFloat32 (-1.0f);
		Swap(pair.m_body0, pair.m_body1);
		SceneContactsSimd (&pair, proxy);
	} else if (collideBodyB.m_collision->IsType (dgCollision::dgCollisionScene_RTTI)) {
		SceneContactsSimd (&pair, proxy);

	} else if (collideBodyA.m_collision->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		CompoundContactsSimd (&pair, proxy);
	} else if (collideBodyB.m_collision->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		swapContactScale = dgFloat32 (-1.0f);
		Swap(pair.m_body0, pair.m_body1);
		CompoundContactsSimd (&pair, proxy);

	} else if (collideBodyA.m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		ConvexContactsSimd (&pair, proxy);
	} else if (collideBodyB.m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		swapContactScale = dgFloat32 (-1.0f);
		Swap(pair.m_body0, pair.m_body1);
		ConvexContactsSimd (&pair, proxy);
	}
	count = pair.m_contactCount;

	if (proxy.m_timestep < dgFloat32 (1.0f)) {
		retTimeStep = proxy.m_timestep;
	}

	if (count) {
		retTimeStep = proxy.m_timestep;
		if (count > maxSize) {
			count = PruneContacts (count, contacts, maxSize);
		}

		for (dgInt32 i = 0; i < count; i ++) {
			points[i].m_x = contacts[i].m_point.m_x; 
			points[i].m_y = contacts[i].m_point.m_y; 
			points[i].m_z = contacts[i].m_point.m_z; 
			normals[i].m_x = contacts[i].m_normal.m_x * swapContactScale;  
			normals[i].m_y = contacts[i].m_normal.m_y * swapContactScale;  
			normals[i].m_z = contacts[i].m_normal.m_z * swapContactScale;  
			penetration[i] = contacts[i].m_penetration; 
		}
	} 
	return count;
*/
}


dgInt32 dgWorld::CollideContinue (dgCollision* collisionA, const dgMatrix& matrixA, const dgVector& velocA, const dgVector& omegaA, dgCollision* collisionB, 
	const dgMatrix& matrixB, const dgVector& velocB, const dgVector& omegaB, dgFloat32& retTimeStep, dgTriplex* points, dgTriplex* normals, 
	dgFloat32* penetration, dgInt32 maxSize, dgInt32 threadIndex)
{
_ASSERTE (0);
return 0;
/*
	dgInt32 count;
	dgBody collideBodyA;
	dgBody collideBodyB;
	dgContactPoint contacts[DG_MAX_CONTATCS];

	count = 0;
	retTimeStep = dgFloat32 (1.0e10f);
	maxSize = GetMin (DG_MAX_CONTATCS, maxSize);


	collideBodyA.m_world = this;
	collideBodyA.SetContinuesCollisionMode(true); 
	collideBodyA.m_matrix = matrixA;
	collideBodyA.m_collision = collisionA;
	collideBodyA.m_masterNode = NULL;
	collideBodyA.m_collisionCell.m_cell = NULL;
	collideBodyA.m_collisionWorldMatrix = collisionA->m_offset * matrixA;
	collideBodyA.m_veloc = dgVector (velocA[0], velocA[1], velocA[2], dgFloat32 (0.0f));
	collideBodyA.m_omega = dgVector (omegaA[0], omegaA[1], omegaA[2], dgFloat32 (0.0f));
	collideBodyA.m_accel = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	collideBodyA.m_alpha = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	collideBodyA.m_invMass = dgVector (dgFloat32 (1.0f), dgFloat32 (1.0f), dgFloat32 (1.0f), dgFloat32 (1.0f));
	collideBodyA.UpdateCollisionMatrix(dgFloat32 (1.0f), 0);

	collideBodyB.m_world = this;
	collideBodyB.SetContinuesCollisionMode(true); 
	collideBodyB.m_matrix = matrixB;
	collideBodyB.m_collision = collisionB;
	collideBodyB.m_masterNode = NULL;
	collideBodyB.m_collisionCell.m_cell = NULL;
	collideBodyB.m_collisionWorldMatrix = collisionB->m_offset * matrixB;
	collideBodyB.m_veloc = dgVector (velocB[0], velocB[1], velocB[2], dgFloat32 (0.0f));
	collideBodyB.m_omega = dgVector (omegaB[0], omegaB[1], omegaB[2], dgFloat32 (0.0f));
	collideBodyB.m_accel = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	collideBodyB.m_alpha = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	collideBodyB.m_invMass = dgVector (dgFloat32 (1.0f), dgFloat32 (1.0f), dgFloat32 (1.0f), dgFloat32 (1.0f));
	collideBodyB.UpdateCollisionMatrix(dgFloat32 (1.0f), 0);

	dgCollisionParamProxy proxy(threadIndex);
	proxy.m_timestep = dgFloat32 (1.0f);
	proxy.m_unconditionalCast = 1;
	proxy.m_penetrationPadding = 0.0f;
	proxy.m_continueCollision = 1;
//	proxy.m_maxContacts = DG_MAX_CONTATCS;
	proxy.m_maxContacts = maxSize;
	proxy.m_isTriggerVolume = 0;

	dgCollidingPairCollector::dgPair pair;
	pair.m_body0 = &collideBodyA;
	pair.m_body1 = &collideBodyB;
	pair.m_contact = NULL;
	pair.m_material = NULL;
	pair.m_contactCount = 0;
	pair.m_contactBuffer = contacts;
	
	dgFloat32 swapContactScale = dgFloat32 (1.0f);
	if (collideBodyA.m_collision->IsType (dgCollision::dgCollisionScene_RTTI)) {
		swapContactScale = dgFloat32 (-1.0f);
		Swap(pair.m_body0, pair.m_body1);
		SceneContactsSimd (&pair, proxy);
	} else if (collideBodyB.m_collision->IsType (dgCollision::dgCollisionScene_RTTI)) {
		SceneContactsSimd (&pair, proxy);

	} else if (collideBodyA.m_collision->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		CompoundContacts (&pair, proxy);
	} else if (collideBodyB.m_collision->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		swapContactScale = dgFloat32 (-1.0f);
		Swap(pair.m_body0, pair.m_body1);
		CompoundContacts (&pair, proxy);

	} else if (collideBodyA.m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		ConvexContacts (&pair, proxy);
	} else if (collideBodyB.m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		swapContactScale = dgFloat32 (-1.0f);
		Swap(pair.m_body0, pair.m_body1);
		ConvexContacts (&pair, proxy);
	}
	count = pair.m_contactCount;

	if (proxy.m_timestep < dgFloat32 (1.0f)) {
		retTimeStep = proxy.m_timestep;
	}

	if (count) {
		
		if (count > maxSize) {
			count = PruneContacts (count, contacts, maxSize);
		}

		for (dgInt32 i = 0; i < count; i ++) {
			points[i].m_x = contacts[i].m_point.m_x; 
			points[i].m_y = contacts[i].m_point.m_y; 
			points[i].m_z = contacts[i].m_point.m_z; 
			normals[i].m_x = contacts[i].m_normal.m_x * swapContactScale;  
			normals[i].m_y = contacts[i].m_normal.m_y * swapContactScale;  
			normals[i].m_z = contacts[i].m_normal.m_z * swapContactScale;  
			penetration[i] = contacts[i].m_penetration; 
		}
	} 

	return count;
*/
}


dgInt32 dgWorld::Collide (dgCollision* collisionA, const dgMatrix& matrixA, dgCollision* collisionB, const dgMatrix& matrixB, dgTriplex* points, 
	dgTriplex* normals, dgFloat32* penetration, dgInt32 maxSize, dgInt32 threadIndex)
{
_ASSERTE (0);
return 0;
/*
	bool isTriggerA;
	bool isTriggerB;
	dgInt32 count;
	dgBody collideBodyA;
	dgBody collideBodyB;
	dgContactPoint contacts[DG_MAX_CONTATCS];

	count = 0;
	maxSize = GetMin (DG_MAX_CONTATCS, maxSize);

	collideBodyA.m_world = this;
	collideBodyA.m_masterNode = NULL;
	collideBodyA.m_collisionCell.m_cell = NULL;
	collideBodyA.SetContinuesCollisionMode(false); 
	collideBodyA.m_matrix = matrixA;
	collideBodyA.m_collision = collisionA;
//	collideBodyA.m_collisionWorldMatrix = collisionA->m_offset * matrixA;
	collideBodyA.UpdateCollisionMatrix(dgFloat32 (0.0f), 0);

	collideBodyB.m_world = this;
	collideBodyB.m_masterNode = NULL;
	collideBodyB.m_collisionCell.m_cell = NULL;
	collideBodyB.SetContinuesCollisionMode(false); 
	collideBodyB.m_matrix = matrixB;
	collideBodyB.m_collision = collisionB;
//	collideBodyB.m_collisionWorldMatrix = collisionB->m_offset * matrixB;
	collideBodyB.UpdateCollisionMatrix(dgFloat32 (0.0f), 0);

	isTriggerA = collisionA->IsTriggerVolume();
	isTriggerB = collisionB->IsTriggerVolume();

	dgCollisionParamProxy proxy(threadIndex);
	proxy.m_timestep = dgFloat32 (0.0f);
	proxy.m_unconditionalCast = 1;
	proxy.m_penetrationPadding = 0.0f;
	proxy.m_continueCollision = 0;
	proxy.m_maxContacts = maxSize;
	proxy.m_isTriggerVolume = 0;

	dgCollidingPairCollector::dgPair pair;
	pair.m_body0 = &collideBodyA;
	pair.m_body1 = &collideBodyB;
	pair.m_contact = NULL;
	pair.m_material = NULL;
	pair.m_contactCount = 0;
	pair.m_contactBuffer = contacts;

	dgFloat32 swapContactScale = dgFloat32 (1.0f);	
	if (collideBodyA.m_collision->IsType (dgCollision::dgCollisionScene_RTTI)) {
		swapContactScale = dgFloat32 (-1.0f);
		Swap(pair.m_body0, pair.m_body1);
		SceneContacts (&pair, proxy);
	} else if (collideBodyB.m_collision->IsType (dgCollision::dgCollisionScene_RTTI)) {
		SceneContacts (&pair, proxy);

	} else if (collideBodyA.m_collision->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		CompoundContacts (&pair, proxy);
	} else if (collideBodyB.m_collision->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		swapContactScale = dgFloat32 (-1.0f);
		Swap(pair.m_body0, pair.m_body1);
		CompoundContacts (&pair, proxy);

	} else if (collideBodyA.m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		ConvexContacts (&pair, proxy);
	} else if (collideBodyB.m_collision->IsType (dgCollision::dgConvexCollision_RTTI)) {
		swapContactScale = dgFloat32 (-1.0f);
		Swap(pair.m_body0, pair.m_body1);
		ConvexContacts (&pair, proxy);
	}

	count = pair.m_contactCount;
	if (count > maxSize) {
		count = ReduceContacts (count, contacts, maxSize, DG_REDUCE_CONTACT_TOLERANCE);
	}

	for (dgInt32 i = 0; i < count; i ++) {
		points[i].m_x = contacts[i].m_point.m_x; 
		points[i].m_y = contacts[i].m_point.m_y; 
		points[i].m_z = contacts[i].m_point.m_z; 
		normals[i].m_x = contacts[i].m_normal.m_x * swapContactScale;  
		normals[i].m_y = contacts[i].m_normal.m_y * swapContactScale;  
		normals[i].m_z = contacts[i].m_normal.m_z * swapContactScale;  
		penetration[i] = contacts[i].m_penetration; 
	}

	return count;
*/
}



void dgWorld::InitConvexCollision ()
{
	((dgVector&) dgContactSolver::m_zero) = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f)); 
	((dgVector&) dgContactSolver::m_negativeOne) = dgVector (dgFloat32 (-1.0f), dgFloat32 (-1.0f), dgFloat32 (-1.0f), dgFloat32 (-1.0f));
	((dgVector&) dgContactSolver::m_zeroTolerenace) = dgVector (DG_DISTANCE_TOLERANCE_ZERO, DG_DISTANCE_TOLERANCE_ZERO, DG_DISTANCE_TOLERANCE_ZERO, DG_DISTANCE_TOLERANCE_ZERO);
	((dgVector&) dgContactSolver::m_nrh0p5) = dgVector (dgFloat32 (0.5f), dgFloat32 (0.5f), dgFloat32 (0.5f), dgFloat32 (0.5f));
	((dgVector&) dgContactSolver::m_nrh3p0) = dgVector (dgFloat32 (3.0f), dgFloat32 (3.0f), dgFloat32 (3.0f), dgFloat32 (3.0f));


	((dgVector&) dgContactSolver::m_index_yx) = dgVector (dgFloat32 (0.0f), dgFloat32 (1.0f), dgFloat32 (0.0f), dgFloat32 (1.0f)); 

	((dgVector&) dgContactSolver::m_index_wz) = dgVector (dgFloat32 (2.0f), dgFloat32 (3.0f), dgFloat32 (2.0f), dgFloat32 (3.0f)); 

	((dgVector&) dgContactSolver::m_negIndex) = dgVector (dgFloat32 (-1.0f), dgFloat32 (-1.0f), dgFloat32 (-1.0f), dgFloat32 (-1.0f)); 


	dgContactSolver::m_dir[0]  = dgVector ( dgFloat32 (1.0f), -dgFloat32 (1.0f),  dgFloat32 (1.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[1]  = dgVector (-dgFloat32 (1.0f), -dgFloat32 (1.0f), -dgFloat32 (1.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[2]  = dgVector ( dgFloat32 (1.0f), -dgFloat32 (1.0f), -dgFloat32 (1.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[3]  = dgVector (-dgFloat32 (1.0f),  dgFloat32 (1.0f),  dgFloat32 (1.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[4]  = dgVector ( dgFloat32 (1.0f),  dgFloat32 (1.0f), -dgFloat32 (1.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[5]  = dgVector (-dgFloat32 (1.0f),  dgFloat32 (1.0f), -dgFloat32 (1.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[6]  = dgVector (-dgFloat32 (1.0f), -dgFloat32 (1.0f),  dgFloat32 (1.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[7]  = dgVector ( dgFloat32 (1.0f),  dgFloat32 (1.0f),  dgFloat32 (1.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[8]  = dgVector ( dgFloat32 (0.0f), -dgFloat32 (1.0f),  dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[9]  = dgVector ( dgFloat32 (0.0f),  dgFloat32 (1.0f),  dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[10] = dgVector ( dgFloat32 (1.0f),  dgFloat32 (0.0f),  dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[11] = dgVector (-dgFloat32 (1.0f),  dgFloat32 (0.0f),  dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[12] = dgVector ( dgFloat32 (0.0f),  dgFloat32 (0.0f),  dgFloat32 (1.0f), dgFloat32 (0.0f));
	dgContactSolver::m_dir[13] = dgVector ( dgFloat32 (0.0f),  dgFloat32 (0.0f), -dgFloat32 (1.0f), dgFloat32 (0.0f));

	for (dgInt32 i = 0; i < dgInt32(sizeof(dgContactSolver::m_dir) / sizeof(dgContactSolver::m_dir[0])); i ++) {
		dgVector dir (dgContactSolver::m_dir[i]);
		dgContactSolver::m_dir[i] = dir.Scale (dgFloat32 (1.0f) / dgSqrt (dir % dir));
	}


	dgCollisionConvex::m_multiResDir[0] = dgVector (dgFloat32 ( 0.577350f), dgFloat32 ( 0.577350f), dgFloat32 ( 0.577350f), dgFloat32 (0.0f));
	dgCollisionConvex::m_multiResDir[1] = dgVector (dgFloat32 (-0.577350f), dgFloat32 ( 0.577350f), dgFloat32 ( 0.577350f), dgFloat32 (0.0f));
	dgCollisionConvex::m_multiResDir[2] = dgVector (dgFloat32 ( 0.577350f), dgFloat32 (-0.577350f), dgFloat32 ( 0.577350f), dgFloat32 (0.0f));
	dgCollisionConvex::m_multiResDir[3] = dgVector (dgFloat32 (-0.577350f), dgFloat32 (-0.577350f), dgFloat32 ( 0.577350f), dgFloat32 (0.0f));

	dgCollisionConvex::m_multiResDir[4] = dgCollisionConvex::m_multiResDir[0].Scale (dgFloat32 (-1.0f));
	dgCollisionConvex::m_multiResDir[5] = dgCollisionConvex::m_multiResDir[1].Scale (dgFloat32 (-1.0f));
	dgCollisionConvex::m_multiResDir[6] = dgCollisionConvex::m_multiResDir[2].Scale (dgFloat32 (-1.0f));
	dgCollisionConvex::m_multiResDir[7] = dgCollisionConvex::m_multiResDir[3].Scale (dgFloat32 (-1.0f));

	dgCollisionConvex::m_multiResDir_sse[0] = dgVector (dgCollisionConvex::m_multiResDir[0].m_x, dgCollisionConvex::m_multiResDir[1].m_x, dgCollisionConvex::m_multiResDir[2].m_x, dgCollisionConvex::m_multiResDir[3].m_x);
	dgCollisionConvex::m_multiResDir_sse[1] = dgVector (dgCollisionConvex::m_multiResDir[0].m_y, dgCollisionConvex::m_multiResDir[1].m_y, dgCollisionConvex::m_multiResDir[2].m_y, dgCollisionConvex::m_multiResDir[3].m_y);
	dgCollisionConvex::m_multiResDir_sse[2] = dgVector (dgCollisionConvex::m_multiResDir[0].m_z, dgCollisionConvex::m_multiResDir[1].m_z, dgCollisionConvex::m_multiResDir[2].m_z, dgCollisionConvex::m_multiResDir[3].m_z);
	dgCollisionConvex::m_multiResDir_sse[3] = dgVector (dgCollisionConvex::m_multiResDir[4].m_x, dgCollisionConvex::m_multiResDir[5].m_x, dgCollisionConvex::m_multiResDir[6].m_x, dgCollisionConvex::m_multiResDir[7].m_x);
	dgCollisionConvex::m_multiResDir_sse[4] = dgVector (dgCollisionConvex::m_multiResDir[4].m_y, dgCollisionConvex::m_multiResDir[5].m_y, dgCollisionConvex::m_multiResDir[6].m_y, dgCollisionConvex::m_multiResDir[7].m_y);
	dgCollisionConvex::m_multiResDir_sse[5] = dgVector (dgCollisionConvex::m_multiResDir[4].m_z, dgCollisionConvex::m_multiResDir[5].m_z, dgCollisionConvex::m_multiResDir[6].m_z, dgCollisionConvex::m_multiResDir[7].m_z);

_ASSERTE (0);
	dgFloatSign tmp;
	tmp.m_integer.m_iVal = 0x7fffffff;
	dgCollisionConvex::m_signMask.m_x = tmp.m_fVal;
	dgCollisionConvex::m_signMask.m_y = tmp.m_fVal;
	dgCollisionConvex::m_signMask.m_z = tmp.m_fVal;
	dgCollisionConvex::m_signMask.m_w = tmp.m_fVal;

	tmp.m_integer.m_iVal = 0xffffffff;
	dgCollisionConvex::m_triplexMask.m_x = tmp.m_fVal;
	dgCollisionConvex::m_triplexMask.m_y = tmp.m_fVal;
	dgCollisionConvex::m_triplexMask.m_z = tmp.m_fVal;
	dgCollisionConvex::m_triplexMask.m_w = 0.0f;


#ifdef _DEBUG
	for (dgInt32 i = 0; i < dgInt32(sizeof(dgContactSolver::m_dir) / sizeof(dgContactSolver::m_dir[0])); i ++) {
		_ASSERTE (dgContactSolver::m_dir[i] % dgContactSolver::m_dir[i] > dgFloat32 (0.9999f));
		for (dgInt32 j = i + 1; j < dgInt32(sizeof(dgContactSolver::m_dir) / sizeof(dgContactSolver::m_dir[0])); j ++) {
			_ASSERTE (dgContactSolver::m_dir[i] % dgContactSolver::m_dir[j] < dgFloat32 (0.9999f));
		}
	}
#endif
}


dgInt32 dgWorld::FilterPolygonEdgeContacts (dgInt32 count, dgContactPoint* const contact) const
{
	dgInt32 j;
	dgInt32 faceCount;

	if (count > 1) {
		faceCount = 0;
		j = count - 1;
		while (faceCount <= j) {
			while ((faceCount <= j) && !contact[faceCount].m_isEdgeContact) {
				faceCount ++;
			}
			while ((faceCount <= j) && contact[j].m_isEdgeContact) {
				j --;
			}

			if (faceCount < j) {
				Swap (contact[faceCount], contact[j]);
			}
		}

		if (faceCount < count) {
			for (dgInt32 i = 0; i < faceCount; i ++) {
				_ASSERTE ((contact[i].m_isEdgeContact == 0) || (contact[i].m_isEdgeContact == 1));
				for (dgInt32 j = faceCount; j < count; j ++) {
					dgFloat32 dist;
					dgVector distVector (contact[i].m_point - contact[j].m_point);
					dist = distVector % distVector;
					//					if (dist < dgFloat32 (0.04f)) {
					if (dist < dgFloat32 (1.e-2f)) {
						count --;
						contact[j] = contact[count];
						j --;
					}
				}
			}
		}

		for (dgInt32 i = 0; i < count - 1; i ++) {
			_ASSERTE ((contact[i].m_isEdgeContact == 0) || (contact[i].m_isEdgeContact == 1));
			for (dgInt32 j = i + 1; j < count; j ++) {
				dgFloat32 dist;
				dgVector distVector (contact[i].m_point - contact[j].m_point);
				dist = distVector % distVector;
				//				if (dist < dgFloat32 (0.04f)) {
				//				if ((dist < dgFloat32 (0.001f)) || ((dist < dgFloat32 (0.01f)) && ((contact[i].m_normal % contact[j].m_normal) > dgFloat32 (0.86f)))) {
				if (dist < dgFloat32 (1.e-3f)) {
					count --;
					contact[j] = contact[count];
					j --;
				}
			}
		}
	}

	return count;
}


dgInt32 dgWorld::ClosestPoint (dgCollisionParamProxy& proxy) const	
{
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgConvexCollision_RTTI));

	dgMatrix matrix (proxy.m_floatingMatrix * proxy.m_referenceMatrix.Inverse());
	proxy.m_localMatrixInv = &matrix;

	dgContactSolver mink (proxy);
	return mink.CalculateClosestPoints ();
}


dgInt32 dgWorld::SphereSphereCollision (const dgVector& sph0, dgFloat32 radius0, const dgVector& sph1, dgFloat32 radius1, dgCollisionParamProxy& proxy) const
{
	dgFloat32 dist;
	dgFloat32 mag2;

	dgVector dir (sph1 - sph0);
	dgContactPoint* const contactOut = proxy.m_contacts;

	proxy.m_inTriggerVolume = 0;

	mag2 = dir % dir; 
	if (mag2 < dgFloat32 (1.0e-4f)) {
		// if the two spheres are located at the exact same origin just move then apart in any direction
		if (proxy.m_isTriggerVolume) {
			proxy.m_inTriggerVolume = 1;
			return 0;
		}

		contactOut[0].m_normal = dgVector (dgFloat32 (0.0f), dgFloat32 (1.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
		contactOut[0].m_point = sph0 + contactOut[0].m_normal.Scale (radius0);
		contactOut[0].m_penetration = dgFloat32 (0.01f);
		contactOut[0].m_userId = 0;
		return 1;

	} 
	dgFloat32 mag;
	mag = dgSqrt (mag2) ;

	// get penetration distance
	dist = mag - (radius0 + radius1 + proxy.m_penetrationPadding);
	if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
		return 0;
	}

	dist = (dgAbsf (dist) - DG_IMPULSIVE_CONTACT_PENETRATION);
	if (dist < dgFloat32 (0.0f)) {
		dist = dgFloat32 (0.0f);
	}

	if (proxy.m_isTriggerVolume) {
		proxy.m_inTriggerVolume = 1;
		return 0;
	}

	contactOut[0].m_normal = dir.Scale (- dgFloat32 (1.0f) / mag);
	contactOut[0].m_point = sph0 - contactOut[0].m_normal.Scale (mag * dgFloat32 (0.5f));
	contactOut[0].m_penetration = dist;
	contactOut[0].m_userId = 0;
	return 1;
}




dgInt32 dgWorld::CalculateSphereToSphereContacts (dgCollisionParamProxy& proxy) const
{
	dgFloat32 radius1;
	dgFloat32 radius2;
	const dgCollisionSphere* collSph1;
	const dgCollisionSphere* collSph2;

	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgCollisionSphere_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionSphere_RTTI));

	collSph1 = (dgCollisionSphere*) proxy.m_referenceCollision;
	collSph2 = (dgCollisionSphere*) proxy.m_floatingCollision;

	radius1 = collSph1->m_radius;
	radius2 = collSph2->m_radius;

	const dgVector& center1 = proxy.m_referenceMatrix.m_posit;
	const dgVector& center2 = proxy.m_floatingMatrix.m_posit;
	return SphereSphereCollision (center1, radius1, center2, radius2, proxy); 
}


dgInt32 dgWorld::CalculateCapsuleToSphereContacts (dgCollisionParamProxy& proxy) const
{
	dgFloat32 sphereRadius;
	dgFloat32 capsuleRadius;
	const dgCollisionSphere* sphere;
	const dgCollisionCapsule* capsule;

	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionSphere_RTTI));
	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgCollisionCapsule_RTTI));

	sphere = (dgCollisionSphere*) proxy.m_floatingCollision;
	capsule = (dgCollisionCapsule*) proxy.m_referenceCollision;

	const dgVector& sphereCenter = proxy.m_floatingMatrix.m_posit;

	sphereRadius = sphere->m_radius;
	capsuleRadius = capsule->GetRadius();
	dgVector cylP0 (proxy.m_referenceMatrix.TransformVector(dgVector (-capsule->GetHeight(), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f))));
	dgVector cylP1 (proxy.m_referenceMatrix.TransformVector(dgVector ( capsule->GetHeight(), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f))));
	dgVector point (dgPointToRayDistance (sphereCenter, cylP0, cylP1));
	return SphereSphereCollision (point, capsuleRadius, sphereCenter, sphereRadius, proxy); 
}

dgInt32 dgWorld::CalculateBoxToSphereContacts (dgCollisionParamProxy& proxy) const
{
	dgInt32 code;
	dgInt32 codeX;
	dgInt32 codeY;
	dgInt32 codeZ;
	dgFloat32 dist;
	dgFloat32 radius;
	dgContactPoint* contactOut;
	const dgCollisionBox* collBox;
	const dgCollisionSphere* collSph;

	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgCollisionBox_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionSphere_RTTI));

	const dgMatrix& boxMatrix = proxy.m_referenceMatrix;
	const dgMatrix& sphMatrix = proxy.m_floatingMatrix;

	collBox = (dgCollisionBox*) proxy.m_referenceCollision;
	collSph = (dgCollisionSphere*) proxy.m_floatingCollision;

	radius = collSph->m_radius + proxy.m_penetrationPadding;
	dgVector size (collBox->m_size[0]);
	dgVector center (boxMatrix.UntransformVector (sphMatrix.m_posit));

	codeX = (center.m_x < - size.m_x) + (center.m_x > size.m_x) * 2;
	codeY = (center.m_y < - size.m_y) + (center.m_y > size.m_y) * 2;
	codeZ = (center.m_z < - size.m_z) + (center.m_z > size.m_z) * 2;
	code = codeZ * 9 + codeY * 3 + codeX;

	if (!code) {
		return CalculateHullToHullContacts (proxy);
	}

	dist = dgFloat32 (0.0f);
	dgVector point (center); 
	dgVector normal (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f)); 

	switch (code) 
	{
		case 2 * 9 + 1 * 3 + 2:
		{
			size.m_y *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 2 * 9 + 1 * 3 + 1:
		{
			size.m_x *= dgFloat32 (-1.0f);
			size.m_y *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 1 * 9 + 1 * 3 + 2:
		{
			size.m_y *= dgFloat32 (-1.0f);
			size.m_z *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 1 * 9 + 1 * 3 + 1:
		{
			size.m_x *= dgFloat32 (-1.0f);
			size.m_y *= dgFloat32 (-1.0f);
			size.m_z *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 2 * 9 + 2 * 3 + 2:
		{
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 2 * 9 + 2 * 3 + 1:
		{
			size.m_x *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 1 * 9 + 2 * 3 + 2:
		{
			size.m_z *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 1 * 9 + 2 * 3 + 1:
		{
			size.m_x *= dgFloat32 (-1.0f);
			size.m_z *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}


		case 2 * 9 + 0 * 3 + 2:
		{
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 2 * 9 + 0 * 3 + 1:
		{
			size.m_x *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}


		case 1 * 9 + 0 * 3 + 2:
		{
			size.m_z *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 1 * 9 + 0 * 3 + 1:
		{
			size.m_x *= dgFloat32 (-1.0f);
			size.m_z *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}


		case 0 * 9 + 2 * 3 + 2:
		{
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 0 * 9 + 2 * 3 + 1:
		{
			size.m_x *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}


		case 0 * 9 + 1 * 3 + 2:
		{
			size.m_y *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 0 * 9 + 1 * 3 + 1:
		{
			size.m_x *= dgFloat32 (-1.0f);
			size.m_y *= dgFloat32 (-1.0f);
			normal.m_x = size.m_x -  center.m_x; 
			normal.m_y = size.m_y -  center.m_y; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 2 * 9 + 2 * 3 + 0:
		{
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 1 * 9 + 2 * 3 + 0:
		{
			size.m_z *= dgFloat32 (-1.0f);
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}


		case 2 * 9 + 1 * 3 + 0:
		{
			size.m_y *= dgFloat32 (-1.0f);
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 1 * 9 + 1 * 3 + 0:
		{
			size.m_y *= dgFloat32 (-1.0f);
			size.m_z *= dgFloat32 (-1.0f);
			normal.m_y = size.m_y -  center.m_y; 
			normal.m_z = size.m_z -  center.m_z; 
			normal = normal.Scale (dgRsqrt (normal % normal));
			dist =  normal % (size - center) - radius;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			point += normal.Scale (radius + (dist - proxy.m_penetrationPadding) * dgFloat32 (0.5f));
			break;
		}

		case 0 * 9 + 0 * 3 + 1:
		{
			dist = -((center.m_x + radius) + size.m_x);
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			normal.m_x = 1.0f;
			point.m_x = -size.m_x - (dist + proxy.m_penetrationPadding) * dgFloat32 (0.5f);
			break;
		}

		case 0 * 9 + 1 * 3 + 0:
		{
			dist = -((center.m_y + radius) + size.m_y);
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			normal.m_y = dgFloat32 (1.0f);
			point.m_y = -size.m_y - (dist + proxy.m_penetrationPadding) * dgFloat32 (0.5f);
			break;
		}

		case 1 * 9 + 0 * 3 + 0:
		{
			dist = -((center.m_z + radius) + size.m_z);
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			normal.m_z = dgFloat32 (1.0f);
			point.m_z = -size.m_z - (dist + proxy.m_penetrationPadding) * dgFloat32 (0.5f);
			break;
		}

		case 0 * 9 + 0 * 3 + 2:
		{
			dist = (center.m_x - radius) - size.m_x;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			normal.m_x = dgFloat32 (-1.0f);
			point.m_x = size.m_x + (dist + proxy.m_penetrationPadding) * dgFloat32 (0.5f);
			break;
		}

		case 0 * 9 + 2 * 3 + 0:
		{
			dist = (center.m_y - radius) - size.m_y;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			normal.m_y = dgFloat32 (-1.0f);
			point.m_y = size.m_y + (dist + proxy.m_penetrationPadding) * dgFloat32 (0.5f);
			break;
		}

		case 2 * 9 + 0 * 3 + 0:
		{
			dist = (center.m_z - radius) - size.m_z;
			if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
				return 0;
			}
			normal.m_z = dgFloat32 (-1.0f);
			point.m_z = size.m_z + (dist + proxy.m_penetrationPadding) * dgFloat32 (0.5f);
			break;
		}

		default:
		{
			return 0;
		}
	}

	if (proxy.m_isTriggerVolume) {
		proxy.m_inTriggerVolume = 1;
		return 0;
	}

	dist = (dgAbsf (dist) - DG_IMPULSIVE_CONTACT_PENETRATION);
	if (dist < dgFloat32 (0.0f)) {
		dist = dgFloat32 (0.0f);
	}

	contactOut = proxy.m_contacts;
	contactOut[0].m_point = boxMatrix.TransformVector (point);
	contactOut[0].m_normal = boxMatrix.RotateVector (normal);
	contactOut[0].m_penetration = dist;
	contactOut[0].m_userId = 0;
	return 1;
}


dgInt32 dgWorld::CalculateCapsuleToCapsuleContacts (dgCollisionParamProxy& proxy) const
{
	dgInt32 count;
	dgFloat32 dot;
	dgFloat32 mag;
	dgFloat32 mag2;
	dgFloat32 dist;
	dgFloat32 radius1;
	dgFloat32 radius2;
	const dgCollisionCapsule* collSph1;
	const dgCollisionCapsule* collSph2;

	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgCollisionCapsule_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionCapsule_RTTI));

	proxy.m_inTriggerVolume = 0;
	collSph1 = (dgCollisionCapsule*) proxy.m_referenceCollision;
	collSph2 = (dgCollisionCapsule*) proxy.m_floatingCollision;

	radius1 = collSph1->GetRadius();
	radius2 = collSph2->GetRadius();
	dgContactPoint* const contactOut = proxy.m_contacts;

	dgVector cylP0 (-collSph1->GetHeight(), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgVector cylP1 ( collSph1->GetHeight(), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgVector cylQ0 (-collSph2->GetHeight(), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgVector cylQ1 ( collSph2->GetHeight(), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	cylQ0 = proxy.m_referenceMatrix.UntransformVector((proxy.m_floatingMatrix.TransformVector(cylQ0)));
	cylQ1 = proxy.m_referenceMatrix.UntransformVector((proxy.m_floatingMatrix.TransformVector(cylQ1)));
	dgVector p10 (cylP1 - cylP0);
	dgVector q10 (cylQ1 - cylQ0);

	count = 0;
	p10 = p10.Scale (dgFloat32 (1.0f) / dgSqrt (p10 % p10 + dgFloat32 (1.0e-8f)));
	q10 = q10.Scale (dgFloat32 (1.0f) / dgSqrt (q10 % q10 + dgFloat32 (1.0e-8f)));
	dot = q10 % p10;
	if (dgAbsf (dot) > dgFloat32 (0.998f)) {
		dgFloat32 pl0;
		dgFloat32 pl1;
		dgFloat32 ql0;
		dgFloat32 ql1;
		dgFloat32 distSign1;
		dgFloat32 distSign2;
		pl0 = cylP0 % p10;
		pl1 = cylP1 % p10;
		ql0 = cylQ0 % p10;
		ql1 = cylQ1 % p10;

		distSign1 = -1.0f;

		if (pl0 > pl1) {
			distSign1 = 1.0f;
			Swap (pl0, pl1);
			Swap (cylP0, cylP1);
			p10 = p10.Scale (dgFloat32 (-1.0f));
		}

		distSign2 = -1.0f;
		if (ql0 > ql1) {
			distSign2 = 1.0f;
			Swap (ql0, ql1);
		}

		_ASSERTE (pl0 <= pl1);
		_ASSERTE (ql0 <= ql1);
		if (ql0 >=  pl1) {
			dgVector center1 (proxy.m_referenceMatrix.TransformVector(dgVector (-collSph1->GetHeight() * distSign1, dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f))));
			dgVector center2 (proxy.m_floatingMatrix.TransformVector(dgVector (collSph2->GetHeight() * distSign2, dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f))));
			return SphereSphereCollision (center1, radius1, center2, radius2, proxy); 

		} else if (ql1 <= pl0) {
			dgVector center1 (proxy.m_referenceMatrix.TransformVector(dgVector (collSph1->GetHeight() * distSign1, dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f))));
			dgVector center2 (proxy.m_floatingMatrix.TransformVector(dgVector (-collSph2->GetHeight() * distSign2, dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f))));
			return SphereSphereCollision (center1, radius1, center2, radius2, proxy); 
		}


		dgFloat32 clip0;
		dgFloat32 clip1;
		clip0 = (ql0 > pl0) ? ql0 : pl0;
		clip1 = (ql1 < pl1) ? ql1 : pl1;

		count = 2;
		dgVector projectedQ (cylP0 + p10.Scale ((cylQ0 - cylP0) % p10));

		dgVector dir (cylQ0 - projectedQ);
		mag2 = dir % dir; 
		if (mag2 < dgFloat32 (1.0e-4f)) {
			return 0;
		}

		mag = dgSqrt (mag2) ;
		dist = mag - (radius1 + radius1 + proxy.m_penetrationPadding);
		if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
			return 0;
		}

		dist = (dgAbsf (dist) - DG_IMPULSIVE_CONTACT_PENETRATION);
		if (dist < dgFloat32 (0.0f)) {
			dist = dgFloat32 (0.0f);
		}

		if (proxy.m_isTriggerVolume) {
			proxy.m_inTriggerVolume = 1;
			return 0;
		}

		mag = -dgFloat32 (1.0f) / mag;
		contactOut[0].m_normal = dir.Scale (mag);
		contactOut[0].m_point = cylP0 + p10.Scale (clip0 - pl0) - contactOut[0].m_normal.Scale (mag * dgFloat32 (0.5f));
		contactOut[0].m_userId = 0;
		contactOut[0].m_penetration = dist;

		contactOut[1].m_normal = dir.Scale (mag );
		contactOut[1].m_point = cylP0 + p10.Scale (clip1 - pl0) - contactOut[1].m_normal.Scale (mag * dgFloat32 (0.5f));
		contactOut[1].m_userId = 0;
		contactOut[1].m_penetration = dist;

	} else {
		dgVector center1;
		dgVector center2;
		dgRayToRayDistance (cylP0, cylP1, cylQ0, cylQ1, center1, center2);
		count = SphereSphereCollision (center1, radius1, center2, radius2, proxy); 
	}

	for (dgInt32 i = 0; i < count; i ++) {
		_ASSERTE (dgAbsf((contactOut[i].m_normal % contactOut[i].m_normal) - dgFloat32 (1.0f)) < dgFloat32 (1.0e-3f));
		contactOut[i].m_normal = proxy.m_referenceMatrix.RotateVector(contactOut[i].m_normal);
		contactOut[i].m_point = proxy.m_referenceMatrix.TransformVector(contactOut[i].m_point);
	}
	return count;
}

dgInt32 dgWorld::CalculatePolySoupToSphereContactsDescrete (dgCollisionParamProxy& proxy) const
{
	dgInt32 count;
	dgInt32 count1;
	dgInt32 thread;
	dgInt32 countleft;
	dgInt32 indexCount;
	dgInt32 reduceContactCountLimit;
	dgFloat32 radius;
	dgBody* soupBody; 
	dgBody* spheBody; 
	dgCollisionSphere *sphere;
	dgCollisionMesh *polysoup;
	dgCollisionMesh::dgCollisionConvexPolygon* polygon;
	dgVector point;


	count = 0;
	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgCollisionSphere_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionMesh_RTTI));

	spheBody = proxy.m_referenceBody;
	soupBody = proxy.m_floatingBody;
	sphere = (dgCollisionSphere*) proxy.m_referenceCollision;
	polysoup = (dgCollisionMesh *) proxy.m_floatingCollision;

	const dgMatrix& sphMatrix = proxy.m_referenceMatrix;
	const dgMatrix& soupMatrix = proxy.m_floatingMatrix;

	radius = sphere->m_radius + proxy.m_penetrationPadding;
	dgVector center (soupMatrix.UntransformVector (sphMatrix.m_posit));

	const dgPolygonMeshDesc& data = *proxy.m_polyMeshData;
	thread = data.m_threadNumber;

	const dgInt32* const idArray = (dgInt32*)data.m_userAttribute; 
	dgInt32* const indexArray = (dgInt32*)data.m_faceVertexIndex;

	_ASSERTE (idArray);
	polygon = polysoup->m_polygon[thread];
	polygon->m_vertex = data.m_vertex;
	polygon->m_stride = dgInt32 (data.m_vertexStrideInBytes / sizeof (dgFloat32));

	dgContactPoint* const contactOut = proxy.m_contacts;
	reduceContactCountLimit = 0;
	countleft = proxy.m_maxContacts;

	indexCount = 0;
	_ASSERTE (data.m_faceCount);
	//strideInBytes = data.m_vertexStrideInBytes;
	for (dgInt32 i = 0; (i < data.m_faceCount) && (countleft > 0); i ++) {
		polygon->m_count = data.m_faceIndexCount[i];
		polygon->m_index = &indexArray[indexCount];

		if (data.m_faceNormalIndex) {
			polygon->m_normalIndex = data.m_faceNormalIndex[i];
			polygon->m_adjacentNormalIndex = (dgInt32*) &data.m_faceAdjencentEdgeNormal[indexCount];
		} else {
			polygon->m_normalIndex = 0;
			polygon->m_adjacentNormalIndex = NULL;
		}


		if (polygon->PointToPolygonDistance (center, radius, point)) {
			dgFloat32 dist2;	
			dgVector dp (center - point);
			dist2 = dp % dp;
			if (dist2 > dgFloat32 (0.0f)) {
				dgFloat32 side;
				dgFloat32 dist2Inv;	
				_ASSERTE (dist2 > dgFloat32 (0.0f));
				dist2Inv = dgRsqrt (dist2);
				side =  dist2 * dist2Inv - radius;
				if (side < (-DG_RESTING_CONTACT_PENETRATION)) {
					dgVector normal (dp.Scale (dist2Inv));

					_ASSERTE (dgAbsf (normal % normal - 1.0f) < dgFloat32 (1.0e-5f));
					contactOut[count].m_point = soupMatrix.TransformVector (center - normal.Scale (radius + side * dgFloat32 (0.5f)));  
					contactOut[count].m_normal = soupMatrix.RotateVector (normal);
					contactOut[count].m_userId = idArray[i];

					side = (dgAbsf (side) - DG_IMPULSIVE_CONTACT_PENETRATION);
					if (side < dgFloat32 (0.0f)) {
						side = dgFloat32 (0.0f);
					}
					contactOut[count].m_penetration = side;

					dgVector prevNormal (contactOut[count].m_normal);
					count1 = polygon->ClipContacts (1, &contactOut[count], soupMatrix);
					if ((prevNormal % contactOut[count].m_normal) < dgFloat32 (0.9999f)) {
						contactOut[count].m_point = soupMatrix.TransformVector (center) - contactOut[count].m_normal.Scale (radius + side * dgFloat32 (0.5f));  
					}


					count += count1;
					countleft -= count1;
					reduceContactCountLimit += count;
					if ((reduceContactCountLimit > 24) || (countleft <= 0)) {
						count = ReduceContacts (count, contactOut, proxy.m_maxContacts >> 2, dgFloat32 (1.0e-2f));
						countleft = proxy.m_maxContacts - count;
						reduceContactCountLimit = 0;
					}
				}
			}
		}
		indexCount += data.m_faceIndexCount[i];
	}

	count = FilterPolygonEdgeContacts (count, contactOut);
	return count;
}


/*
dgInt32 dgWorld::FlattenContinueContacts (dgInt32 count, dgContactPoint* const contact, dgFloat32 size) const
{
	dgInt32 tmp;
	dgInt8 mark[256];

	for (dgInt32 i = 0; i < count; i += 4) {
		((dgInt32*) mark)[i] = 0;
	}

	size *= 2.0f;
	tmp = count;
	for (dgInt32 i = 0; i < tmp; i ++) {
		if (!mark[i]) {
			dgPlane plane (contact[i].m_normal, - (contact[i].m_point % contact[i].m_normal));
			for (dgInt32 j = i + 1; j < count ; j ++) {
				if (!mark[j]) {
					dgFloat32 dist;
					dist = plane.Evalue(contact[j].m_point);
					if (dgAbsf (dist) < dgFloat32 (0.01f)) {
						mark[j] = true;
					}
				}
			}

			dgVector testPoint;
			if (dgAbsf (plane.m_z) > dgFloat32 (0.577f)) {
				testPoint = dgVector (-plane.m_y, plane.m_z, dgFloat32 (0.0f), dgFloat32 (0.0f));
			} else {
				testPoint = dgVector (-plane.m_y, plane.m_x, dgFloat32 (0.0f), dgFloat32 (0.0f));
			}

			dgVector dir0 (plane * testPoint);
			_ASSERTE (dir0  % dir0  > dgFloat32 (1.0e-8f));
			dir0  = dir0 .Scale (dgRsqrt (dir0 % dir0) * size);
			dgVector dir1 (plane * dir0);

			contact[count] = contact[i];
			contact[count].m_point += dir0;
			count ++;
			contact[count] = contact[i];
			contact[count].m_point -= dir0;
			count ++;

			contact[count] = contact[i];
			contact[count].m_point += dir1;
			count ++;
			contact[count] = contact[i];
			contact[count].m_point -= dir1;
			count ++;
		}
	}

	return count;
}


dgInt32 dgWorld::CalculateBoxToBoxContacts (dgBody* box1, dgBody* box2, dgContactPoint* const contactOut) const
{
_ASSERTE (0);
return 0;

	dgInt32 i;
	dgInt32 k;
	dgInt32 count1;
	dgInt32 count2;
	dgFloat32 d1;
	dgFloat32 d2;
	dgFloat32 min;
	dgFloat32 dist;
	dgFloat32 test;
	dgFloat32 minDist;
	dgPlane plane; 
	dgVector shape1[16];
	dgVector shape2[16];
	dgCollisionBox* collision1;
	dgCollisionBox* collision2;
	


	_ASSERTE (box1->m_collision->IsType (m_boxType));
	_ASSERTE (box2->m_collision->IsType (m_boxType));

	const dgMatrix& matrix1 = box1->m_collisionWorldMatrix;
	const dgMatrix& matrix2 = box2->m_collisionWorldMatrix;

	collision1 = (dgCollisionBox*) box1->m_collision;
	collision2 = (dgCollisionBox*) box2->m_collision;

	const dgVector& size1 = collision1->m_size;
	const dgVector& size2 = collision2->m_size;

	minDist = dgFloat32 (-1.0e10f);

	dgMatrix mat12 (matrix1 * matrix2.Inverse ());
	for (i = 0; i < 3; i ++) {
		min = dgAbsf (mat12[0][i]) * size1[0] + dgAbsf (mat12[1][i]) * size1[1] + dgAbsf (mat12[2][i]) * size1[2];
		dist = dgAbsf (mat12[3][i]) - size2[i] - min;
		if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
			return 0;
		}
		if (dist > minDist) {
			minDist = dist;
			plane[0] = dgFloat32 (0.0f);
			plane[1] = dgFloat32 (0.0f);
			plane[2] = dgFloat32 (0.0f);
			plane[3] = - (size2[i] + dist * dgFloat32 (0.5f));

			plane[i] = dgFloat32 (1.0f);
			test = plane[3] + mat12[3][i] + min; 
			if (test < dgFloat32 (0.0f)) {
				plane[i] = dgFloat32 (-1.0f);
			}
			plane = matrix2.TransformPlane (plane);
		}
	}

	//	dgMatrix mat21 (matrix2 * matrix1.Inverse ());
	dgMatrix mat21 (mat12.Inverse ());
	for (i = 0; i < 3; i ++) {
		min = dgAbsf (mat21[0][i]) * size2[0] + dgAbsf (mat21[1][i]) * size2[1] + dgAbsf (mat21[2][i]) * size2[2];
		dist = dgAbsf (mat21[3][i]) - size1[i] - min;
		if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
			return 0;
		}
		if (dist > minDist) {
			minDist = dist;
			plane[0] = dgFloat32 (0.0f);
			plane[1] = dgFloat32 (0.0f);
			plane[2] = dgFloat32 (0.0f);
			plane[3] = - (size1[i] + dist * dgFloat32 (0.5f));

			plane[i] = dgFloat32 (1.0f);

			test = plane[3] + mat21[3][i] + min; 
			if (test < dgFloat32 (0.0f)) {
				plane[i] = dgFloat32 (-1.0f);
			}
			plane = matrix1.TransformPlane (plane).Scale (dgFloat32 (-1.0f));
		}
	}

	for (k = 0; k < 3; k ++) {
		for (i = 0; i < 3; i ++) { 
			dgVector normal (matrix1[k] * matrix2[i]);
			test = (normal % normal) ;
			if (test > dgFloat32(1.0e-6f)) {
				normal = normal.Scale (dgRsqrt (test));
				d2 = size2[0] * dgAbsf (matrix2[0] % normal) + size2[1] * dgAbsf (matrix2[1] % normal) + size2[2] * dgAbsf (matrix2[2] % normal);
				d1 = size1[0] * dgAbsf (matrix1[0] % normal) + size1[1] * dgAbsf (matrix1[1] % normal) + size1[2] * dgAbsf (matrix1[2] % normal);

				dgVector q (matrix2[3] - normal.Scale (d2));
				dgVector p (matrix1[3] + normal.Scale (d1));
				dist = (q - p) % normal;
				if (dist > (-DG_RESTING_CONTACT_PENETRATION)) {
					return 0;
				}

				dgVector q1 (matrix2[3] + normal.Scale (d2));
				dgVector p1 (matrix1[3] - normal.Scale (d1));
				test = (p1 - q1) % normal;
				if (test > (-DG_RESTING_CONTACT_PENETRATION)) {
					return 0;
				}

				if (test > dist) {
					dist = test;
					p = p1;
					q = q1;
				}

				if (dist > minDist) {
					minDist = dist;
					plane[0] = normal[0];
					plane[1] = normal[1];
					plane[2] = normal[2];
					plane[3] = - dgFloat32 (0.5f) * ((q + p) % normal);

					test = plane.Evalue (matrix1[3]); 
					if (test < dgFloat32 (0.0f)) {
						plane.m_x *= dgFloat32 (-1.0f);
						plane.m_y *= dgFloat32 (-1.0f);
						plane.m_z *= dgFloat32 (-1.0f);
						plane.m_w *= dgFloat32 (-1.0f);
					}
				}
			}
		}
	}

	dgPlane plane1 (matrix1.UntransformPlane (plane));	
	count1 = collision1->CalculatePlaneIntersection (plane1, shape1);
	if (!count1) {
		dgVector p1 (collision1->SupportVertex (plane1.Scale (dgFloat32 (-1.0f))));
		dgPlane plane (plane1, - (plane1 % p1) - DG_ROBUST_PLANE_CLIP);
		count1 = collision1->CalculatePlaneIntersection (plane, shape1);
		if (count1) {
			dgVector err (plane1.Scale (plane1.Evalue (shape1[0])));
			for (i = 0; i < count1; i ++) {
				shape1[i] -= err;
			}
		}
	}
	if (count1 == 0) {
		return 0;
	}

	dgPlane plane2 (matrix2.UntransformPlane (plane));	
	count2 = collision2->CalculatePlaneIntersection (plane2, shape2);
	if (!count2) {
		dgVector p2 (collision2->SupportVertex (plane2));
		dgPlane plane (plane2, DG_ROBUST_PLANE_CLIP - (plane2 % p2));
		count2 = collision2->CalculatePlaneIntersection (plane, shape2);
		if (count2) {
			dgVector err (plane2.Scale (plane2.Evalue (shape2[0])));
			for (i = 0; i < count2; i ++) {
				shape2[i] -= err;
			}
		}
	}

	if (count2 == 0) {
		return 0;
	}

	_ASSERTE (count1 <= 6);
	_ASSERTE (count2 <= 6);
	matrix1.TransformTriplex (shape1, sizeof (dgVector), shape1, sizeof (dgVector), count1);
	matrix2.TransformTriplex (shape2, sizeof (dgVector), shape2, sizeof (dgVector), count2);

	minDist = (dgAbsf (minDist) - DG_IMPULSIVE_CONTACT_PENETRATION);
	if (minDist < dgFloat32 (0.0f)) {
		minDist = dgFloat32 (0.0f);
	}
	k = dgContactSolver::CalculateConvexShapeIntersection (plane, 0, minDist, count1, shape1, count2, shape2, contactOut);
	return k;

}
*/


dgInt32 dgWorld::CalculatePolySoupToElipseContactsDescrete (dgCollisionParamProxy& proxy) const
{
	dgInt32 count;
	dgInt32 count1;
	dgInt32 thread;
	dgInt32 countleft;
	dgInt32 indexCount;
	dgInt32 reduceContactCountLimit;
	dgFloat32 radius;
	dgBody* soupBody; 
	dgBody* spheBody; 
	dgCollisionEllipse *sphere;
	dgCollisionMesh *polysoup;
	dgCollisionMesh::dgCollisionConvexPolygon* polygon;
	dgVector point;

	count = 0;
	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgCollisionEllipse_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionMesh_RTTI));

	spheBody = proxy.m_referenceBody;
	soupBody = proxy.m_floatingBody;
	sphere = (dgCollisionEllipse*) proxy.m_referenceCollision;
	polysoup = (dgCollisionMesh *) proxy.m_floatingCollision;

	const dgMatrix& sphMatrix = proxy.m_referenceMatrix;
	const dgMatrix& soupMatrix = proxy.m_floatingMatrix;
	const dgMatrix& matrix = *proxy.m_localMatrixInv;
	const dgVector& scale = sphere->m_scale;
	const dgVector& invScale = sphere->m_invScale;


	radius = sphere->m_radius + proxy.m_penetrationPadding;
	//	dgVector center (soupMatrix.UntransformVector (sphMatrix.m_posit));

	const dgPolygonMeshDesc& data = *proxy.m_polyMeshData;
	thread = data.m_threadNumber;

	const dgInt32* const idArray = (dgInt32*)data.m_userAttribute; 
	dgInt32* const indexArray = (dgInt32*)data.m_faceVertexIndex;

	_ASSERTE (idArray);
	polygon = polysoup->m_polygon[thread];
	polygon->m_vertex = data.m_vertex;
	polygon->m_stride = dgInt32 (data.m_vertexStrideInBytes / sizeof (dgFloat32));

	dgContactPoint* const contactOut = proxy.m_contacts;
	reduceContactCountLimit = 0;
	countleft = proxy.m_maxContacts;


	indexCount = 0;
	_ASSERTE (data.m_faceCount);
	//strideInBytes = data.m_vertexStrideInBytes;
	for (dgInt32 i = 0; (i < data.m_faceCount) && (countleft > 0); i ++) {
		polygon->m_count = data.m_faceIndexCount[i];
		polygon->m_index = &indexArray[indexCount];

		if (data.m_faceNormalIndex) {
			polygon->m_normalIndex = data.m_faceNormalIndex[i];
			polygon->m_adjacentNormalIndex = (dgInt32*) &data.m_faceAdjencentEdgeNormal[indexCount];
		} else {
			polygon->m_normalIndex = 0;
			polygon->m_adjacentNormalIndex = NULL;
		}

		if (polygon->DistanceToOrigen (matrix, invScale, radius, point)) {
			dgFloat32 dist2;
			dist2 = point % point;
			if (dist2 > dgFloat32 (0.0f)) {
				dgFloat32 side;
				dgFloat32 contactDist;
				dgFloat32 surfaceDist;

				dgVector contact (scale.CompProduct(point));
				dgVector surfaceContact (scale.CompProduct(point.Scale (radius * dgRsqrt (dist2))));
				contactDist = dgSqrt(contact % contact);
				surfaceDist = dgSqrt(surfaceContact % surfaceContact);

				side = contactDist - surfaceDist;
				if (side < (-DG_RESTING_CONTACT_PENETRATION)) {
					dgVector normal (invScale.CompProduct (point.Scale (dgRsqrt(dist2))));
					normal = normal.Scale (-dgRsqrt(normal % normal));

					dgVector midPoint (contact + surfaceContact);
					contactOut[count].m_point = sphMatrix.TransformVector (midPoint.Scale (dgFloat32 (0.5f)));  
					contactOut[count].m_normal = sphMatrix.RotateVector (normal);
					contactOut[count].m_userId = idArray[i];

					side = (dgAbsf (side) - DG_IMPULSIVE_CONTACT_PENETRATION);
					if (side < dgFloat32 (0.0f)) {
						side = dgFloat32 (0.0f);
					}
					contactOut[count].m_penetration = side;

					dgVector prevNormal (contactOut[count].m_normal);
					count1 = polygon->ClipContacts (1, &contactOut[count], soupMatrix);
					if ((prevNormal % contactOut[count].m_normal) < dgFloat32 (0.9999f)) {
						//_ASSERTE (0);
						//						contactOut[count].m_point = soupMatrix.RotateVector(polygon->m_normal);  
						contactOut[count].m_normal = soupMatrix.RotateVector(polygon->m_normal);  
					}

					count += count1;
					countleft -= count1;
					reduceContactCountLimit += count;
					if ((reduceContactCountLimit > 24) || (countleft <= 0)) {
						count = ReduceContacts (count, contactOut, proxy.m_maxContacts >> 2, dgFloat32 (1.0e-2f));
						countleft = proxy.m_maxContacts - count;
						reduceContactCountLimit = 0;
					}
				}
			}
		}
		indexCount += data.m_faceIndexCount[i];
	}

	count = FilterPolygonEdgeContacts (count, contactOut);
	return count;
}


dgInt32 dgWorld::CalculatePolySoupToSphereContactsContinue (dgCollisionParamProxy& proxy) const
{
	dgInt32 count;
	dgInt32 count1;
	dgInt32 thread;
	dgInt32 countleft;
	dgInt32 indexCount;
	dgInt32 reduceContactCountLimit;
	dgFloat32 minTime;
	dgFloat32 radius;
	dgInt32* indexArray; 
	dgInt32* idArray; 
	dgBody* soupBody; 
	dgBody* spheBody; 
	dgCollisionSphere *sphere;
	dgContactPoint* contactOut;
	dgCollisionMesh *polysoup;
	dgCollisionMesh::dgCollisionConvexPolygon* polygon;

	count = 0;

	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgCollisionSphere_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionMesh_RTTI));

	spheBody = proxy.m_referenceBody;
	soupBody = proxy.m_floatingBody;
	sphere = (dgCollisionSphere*) proxy.m_referenceCollision;
	polysoup = (dgCollisionMesh *) proxy.m_floatingCollision;

	const dgMatrix& sphMatrix = proxy.m_referenceMatrix;
	const dgMatrix& soupMatrix = proxy.m_floatingMatrix;

	radius = sphere->m_radius + proxy.m_penetrationPadding;
	dgVector center (soupMatrix.UntransformVector (sphMatrix.m_posit));
	dgVector veloc (soupMatrix.UnrotateVector (spheBody->m_veloc));

	const dgPolygonMeshDesc& data = *proxy.m_polyMeshData;
	thread = data.m_threadNumber;

	idArray = (dgInt32*)data.m_userAttribute; 
	indexArray = (dgInt32*)data.m_faceVertexIndex;

	_ASSERTE (idArray);
	polygon = polysoup->m_polygon[thread];
	polygon->m_vertex = data.m_vertex;
	polygon->m_stride = dgInt32 (data.m_vertexStrideInBytes / sizeof (dgFloat32));

	contactOut = proxy.m_contacts;
	reduceContactCountLimit = 0;
	countleft = proxy.m_maxContacts;

	_ASSERTE (data.m_faceCount);
	//strideInBytes = data.m_vertexStrideInBytes;

	indexCount = 0;
	minTime = proxy.m_timestep + dgFloat32 (1.0e-5f);
	//	for (dgInt32 i = 0; i < data.m_faceCount; i ++) {
	for (dgInt32 i = 0; (i < data.m_faceCount) && (countleft > 0); i ++) {
		dgFloat32 timestep;
		dgContactPoint contact;
		polygon->m_count = data.m_faceIndexCount[i];
		polygon->m_index = &indexArray[indexCount];

		if (data.m_faceNormalIndex) {
			polygon->m_normalIndex = data.m_faceNormalIndex[i];
			polygon->m_adjacentNormalIndex = (dgInt32*) &data.m_faceAdjencentEdgeNormal[indexCount];
		} else {
			polygon->m_normalIndex = 0;
			polygon->m_adjacentNormalIndex = NULL;
		}

		timestep = polygon->MovingPointToPolygonContact (center, veloc, radius, contact);
		if (timestep >= dgFloat32 (0.0f)) {
			if (timestep <= minTime) {
				minTime = timestep + dgFloat32 (1.0e-5f);

				_ASSERTE (dgAbsf (contact.m_normal % contact.m_normal - 1.0f) < dgFloat32 (1.0e-5f));
				//contactOut[count].m_point = soupMatrix.TransformVector (contact.m_point);  
				contactOut[count].m_point = soupMatrix.TransformVector (center - contact.m_normal.Scale (radius));
				contactOut[count].m_normal = soupMatrix.RotateVector (contact.m_normal);
				contactOut[count].m_userId = idArray[i];
				contactOut[count].m_penetration = contact.m_penetration;
				contactOut[count].m_point.m_w = timestep;

				count1 = polygon->ClipContacts (1, &contactOut[count], soupMatrix);

				count += count1;
				countleft -= count1;
				reduceContactCountLimit += count;
				if ((reduceContactCountLimit > 24) || (countleft <= 0)) {
					count = ReduceContacts (count, contactOut, proxy.m_maxContacts >> 2, dgFloat32 (1.0e-2f));
					countleft = proxy.m_maxContacts - count;
					reduceContactCountLimit = 0;
				}
			}
		}
		//indexArray += data.m_faceIndexCount[i];
		indexCount += data.m_faceIndexCount[i];

	}
	proxy.m_timestep = minTime;

	if (count > 1) {
		if (data.m_faceCount > 1) {
			dgFloat32 dt;
			dt = dgFloat32 (0.01f) * dgRsqrt (veloc % veloc);
			for (dgInt32 i = 0; i < count; i ++) {
				dgFloat32 err;
				err = contactOut[i].m_point.m_w - minTime;
				if (dgAbsf (err) > dt) {
					contactOut[i] = contactOut[count - 1];
					i --;
					count --;
				}
			}
		}
		count = FilterPolygonEdgeContacts (count, contactOut);
	}
	return count;
}


dgInt32 dgWorld::CalculateConvexToNonConvexContactsSimd (dgCollisionParamProxy& proxy) const
{
	dgInt32 count = 0;
	proxy.m_inTriggerVolume = 0;
	dgCollisionConvex* collision = (dgCollisionConvex*) proxy.m_referenceCollision;
	if (!collision->m_vertexCount || collision->m_isTriggerVolume) {
		return count;
	}
	proxy.m_isTriggerVolume = 0;

	dgBody* const hullBody = proxy.m_referenceBody; 
	dgBody* const soupBody = proxy.m_floatingBody; 
	dgCollisionMesh* const polysoup = (dgCollisionMesh *) proxy.m_floatingCollision;
	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionMesh_RTTI));

	const dgMatrix& hullMatrix = proxy.m_referenceMatrix;
	const dgMatrix& soupMatrix = proxy.m_floatingMatrix;

	dgMatrix matrix (hullMatrix.MultiplySimd(soupMatrix.InverseSimd()));

	dgPolygonMeshDesc data;
	collision->CalcAABBSimd (matrix, data.m_boxP0, data.m_boxP1);

	_ASSERTE (proxy.m_timestep <= dgFloat32 (1.0f));
	_ASSERTE (proxy.m_timestep >= dgFloat32 (0.0f));

	dgInt32 doContinueCollision = 0;
	bool solverInContinueCollision = false;
	if (proxy.m_continueCollision) {
		dgVector hullOmega;
		dgVector hullVeloc;

		_ASSERTE (proxy.m_timestep <= dgFloat32 (1.0f));
		_ASSERTE (proxy.m_timestep >= dgFloat32 (0.0f));
		hullBody->CalculateContinueVelocitySimd (proxy.m_timestep, hullVeloc, hullOmega);

		_ASSERTE ((proxy.m_unconditionalCast == 0) || (proxy.m_unconditionalCast == 1));
		if (proxy.m_unconditionalCast) {
			doContinueCollision = 1;
			dgVector step (soupMatrix.UnrotateVectorSimd (hullVeloc.Scale (proxy.m_timestep)));
			for (dgInt32 j = 0; j < 3; j ++) {
				if (step[j] > dgFloat32 (0.0f)) {
					//data.m_boxP1.m_z += (step.m_z - boxSize.m_z); 
					data.m_boxP1[j] += step[j]; 
				} else {
					//data.m_boxP0.m_z += (step.m_z + boxSize.m_z); 
					data.m_boxP0[j] += step[j]; 
				}
			}
		} else {
			dgFloat32 mag2 = hullVeloc % hullVeloc;
			if (mag2 > dgFloat32 (0.1f)) {
				dgFloat32 spand = collision->GetBoxMinRadius();
				dgFloat32 padding = collision->GetBoxMaxRadius() - spand;
				dgFloat32 maxOmega = (hullOmega % hullOmega) * proxy.m_timestep * proxy.m_timestep;
				padding = (maxOmega > 1.0f) ? padding : padding * dgSqrt (maxOmega);

				dgFloat32 dist = dgSqrt (mag2) * proxy.m_timestep + padding;
				if (dist > (dgFloat32 (0.25f) * spand)) {
					doContinueCollision = 1;
					solverInContinueCollision = true;

					dgVector step (soupMatrix.UnrotateVectorSimd (hullVeloc.Scale (proxy.m_timestep)));
					step.m_x += (step.m_x > 0.0f) ? padding : -padding;
					step.m_y += (step.m_y > 0.0f) ? padding : -padding;
					step.m_z += (step.m_z > 0.0f) ? padding : -padding;

					dgVector boxSize ((data.m_boxP1 - data.m_boxP0).Scale (dgFloat32 (0.25f)));
					for (dgInt32 j = 0; j < 3; j ++) {
						if (dgAbsf (step[j]) > boxSize[j]) {
							if (step[j] > dgFloat32 (0.0f)) {
								data.m_boxP1[j] += step[j]; 
							} else {
								data.m_boxP0[j] += step[j]; 
							}
						}
					}
				}
			}
		}
	}

	
	data.m_vertex = NULL;
	data.m_threadNumber = proxy.m_threadIndex;
	data.m_faceCount = 0;
	data.m_faceIndexCount = 0;
	data.m_vertexStrideInBytes = 0;

	data.m_faceMaxSize = NULL;
	data.m_userAttribute = NULL;
	data.m_faceVertexIndex = NULL;
	data.m_faceNormalIndex = NULL;
	data.m_faceAdjencentEdgeNormal = NULL;
	data.m_userData = polysoup->GetUserData();
	data.m_objBody = hullBody;
	data.m_polySoupBody = soupBody;
	polysoup->GetCollidingFacesSimd (&data);

	if (data.m_faceCount) {
		dgMatrix matrixInv (matrix.InverseSimd());
		proxy.m_polyMeshData = &data;
		proxy.m_localMatrixInv = &matrixInv;

		if (doContinueCollision) {
			switch (collision->GetCollisionPrimityType()) 
			{
				case m_sphereCollision:
				{
					count = CalculatePolySoupToSphereContactsContinue (proxy);
					break;
				}

				default: 
				{
					count = CalculateConvexToNonConvexContactsContinueSimd (proxy);
					break;
				}
			}

			count = PruneContacts (count, proxy.m_contacts);
			if (count && solverInContinueCollision) {
				hullBody->GetWorld()->GetIndirectLock(&hullBody->m_locks);
				hullBody->m_solverInContinueCollision = true;
				hullBody->GetWorld()->ReleaseIndirectLock(&hullBody->m_locks);
			}


		} else {

			switch (collision->GetCollisionPrimityType()) 
			{
				case m_sphereCollision:
				{
					count = CalculatePolySoupToSphereContactsDescrete (proxy);
					break;
				}

				case m_ellipseCollision:
				{
					count = CalculatePolySoupToElipseContactsDescrete (proxy);
					break;
				}

				default: 
				{
					count = CalculatePolySoupToHullContactsDescreteSimd (proxy);
				}
			}

			count = PruneContacts (count, proxy.m_contacts);
			if (count) {
				proxy.m_timestep = dgFloat32 (0.0f);
			}
		}

		dgContactPoint* contactOut = proxy.m_contacts;
		for (dgInt32 i = 0; i < count; i ++) {
			_ASSERTE ((dgAbsf(contactOut[i].m_normal % contactOut[i].m_normal) - dgFloat32 (1.0f)) < dgFloat32 (1.0e-5f));
			contactOut[i].m_body0 = proxy.m_referenceBody;
			contactOut[i].m_body1 = proxy.m_floatingBody;
			contactOut[i].m_collision0 = proxy.m_referenceCollision;
			contactOut[i].m_collision1 = proxy.m_floatingCollision;
		}
	}
	return count;

}


dgInt32 dgWorld::CalculateConvexToNonConvexContacts (dgCollisionParamProxy& proxy) const
{
	dgPolygonMeshDesc data;
	dgInt32 count = 0;
	proxy.m_inTriggerVolume = 0;
	dgCollisionConvex* collision = (dgCollisionConvex*) proxy.m_referenceCollision;
	if (!collision->m_vertexCount || collision->m_isTriggerVolume) {
		return count;
	}
	proxy.m_isTriggerVolume = 0;

	dgBody* hullBody = proxy.m_referenceBody; 
	dgBody* soupBody = proxy.m_floatingBody; 
	dgCollisionMesh* polysoup = (dgCollisionMesh *) proxy.m_floatingCollision;
	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionMesh_RTTI));


	const dgMatrix& hullMatrix = proxy.m_referenceMatrix;
	const dgMatrix& soupMatrix = proxy.m_floatingMatrix;
	dgMatrix matrix (hullMatrix * soupMatrix.Inverse());
	collision->CalcAABB (matrix, data.m_boxP0, data.m_boxP1);

	_ASSERTE (proxy.m_timestep <= dgFloat32 (1.0f));
	_ASSERTE (proxy.m_timestep >= dgFloat32 (0.0f));

	dgInt32 doContinueCollision = 0;
	bool solverInContinueCollision = false;
	if (proxy.m_continueCollision) {
		dgVector hullOmega;
		dgVector hullVeloc;

		_ASSERTE (proxy.m_timestep <= dgFloat32 (1.0f));
		_ASSERTE (proxy.m_timestep >= dgFloat32 (0.0f));
		//timestep = GetMin (timestep, dgFloat32 (1.0f));
		//timestep = GetMin (proxy.m_timestep, dgFloat32 (1.0f));
		hullBody->CalculateContinueVelocity (proxy.m_timestep, hullVeloc, hullOmega);


		_ASSERTE ((proxy.m_unconditionalCast == 0) || (proxy.m_unconditionalCast == 1));
		if (proxy.m_unconditionalCast) {
			doContinueCollision = 1;
			dgVector step (soupMatrix.UnrotateVector (hullVeloc.Scale (proxy.m_timestep)));
			for (dgInt32 j = 0; j < 3; j ++) {
				if (step[j] > dgFloat32 (0.0f)) {
					//data.m_boxP1.m_z += (step.m_z - boxSize.m_z); 
					data.m_boxP1[j] += step[j]; 
				} else {
					//data.m_boxP0.m_z += (step.m_z + boxSize.m_z); 
					data.m_boxP0[j] += step[j]; 
				}
			}
		} else {
			dgFloat32 mag2 = hullVeloc % hullVeloc;
			if (mag2 > dgFloat32 (0.1f)) {
				dgFloat32 spand = collision->GetBoxMinRadius();
				dgFloat32 padding = collision->GetBoxMaxRadius() - spand;
				dgFloat32 maxOmega = (hullOmega % hullOmega) * proxy.m_timestep * proxy.m_timestep;
				padding = (maxOmega > 1.0f) ? padding : padding * dgSqrt (maxOmega);

				dgFloat32 dist = dgSqrt (mag2) * proxy.m_timestep + padding;
				if (dist > (dgFloat32 (0.25f) * spand)) {
					doContinueCollision = 1;
					solverInContinueCollision = true;

					dgVector step (soupMatrix.UnrotateVector (hullVeloc.Scale (proxy.m_timestep)));
					step.m_x += (step.m_x > 0.0f) ? padding : -padding;
					step.m_y += (step.m_y > 0.0f) ? padding : -padding;
					step.m_z += (step.m_z > 0.0f) ? padding : -padding;

					dgVector boxSize ((data.m_boxP1 - data.m_boxP0).Scale (dgFloat32 (0.25f)));
					for (dgInt32 j = 0; j < 3; j ++) {
						if (dgAbsf (step[j]) > boxSize[j]) {
							if (step[j] > dgFloat32 (0.0f)) {
								data.m_boxP1[j] += step[j]; 
							} else {
								data.m_boxP0[j] += step[j]; 
							}
						}
					}
				}
			}
		}
	}

	data.m_vertex = NULL;
	data.m_threadNumber = proxy.m_threadIndex;
	data.m_faceCount = 0;
	data.m_faceIndexCount = 0;
	data.m_vertexStrideInBytes = 0;

	data.m_faceMaxSize = NULL;
	data.m_userAttribute = NULL;
	data.m_faceVertexIndex = NULL;
	data.m_faceNormalIndex = NULL;
	data.m_faceAdjencentEdgeNormal = NULL;
	data.m_userData = polysoup->GetUserData();
	data.m_objBody = hullBody;
	data.m_polySoupBody = soupBody;

	polysoup->GetCollidingFaces (&data);

	if (data.m_faceCount) {
		dgMatrix matrixInv (matrix.Inverse());
		proxy.m_polyMeshData = &data;
		proxy.m_localMatrixInv = &matrixInv;

		if (doContinueCollision) {
			switch (collision->GetCollisionPrimityType()) 
			{
			case m_sphereCollision:
				{
					count = CalculatePolySoupToSphereContactsContinue (proxy);
					break;
				}

			default: 
				{
					//if (!proxy.m_unconditionalCast &&  (xxx == 5)) {
					//xxx *= 1;
					//}
					count = CalculateConvexToNonConvexContactsContinue (proxy);
					break;
				}
			}

			count = PruneContacts (count, proxy.m_contacts);
			if (count && solverInContinueCollision) {
				hullBody->GetWorld()->GetIndirectLock(&hullBody->m_locks);
				hullBody->m_solverInContinueCollision = true;
				hullBody->GetWorld()->ReleaseIndirectLock(&hullBody->m_locks);
			}


		} else {

			switch (collision->GetCollisionPrimityType()) 
			{
			case m_sphereCollision:
				{
					count = CalculatePolySoupToSphereContactsDescrete (proxy);
					break;
				}

			case m_ellipseCollision:
				{
					count = CalculatePolySoupToElipseContactsDescrete (proxy);
					break;
				}

			default: 
				{
					count = CalculatePolySoupToHullContactsDescrete (proxy);
				}
			}

			count = PruneContacts (count, proxy.m_contacts);
			if (count) {
				proxy.m_timestep = dgFloat32 (0.0f);
			}
		}

		dgContactPoint* contactOut = proxy.m_contacts;
		for (dgInt32 i = 0; i < count; i ++) {
			_ASSERTE ((dgAbsf(contactOut[i].m_normal % contactOut[i].m_normal) - dgFloat32 (1.0f)) < dgFloat32 (1.0e-5f));
			contactOut[i].m_body0 = proxy.m_referenceBody;
			contactOut[i].m_body1 = proxy.m_floatingBody;
			contactOut[i].m_collision0 = proxy.m_referenceCollision;
			contactOut[i].m_collision1 = proxy.m_floatingCollision;
		}
	}
	return count;
}


dgInt32 dgWorld::CalculateConvexToConvexContactsSimd (dgCollisionParamProxy& proxy) const
{
	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgConvexCollision_RTTI));

	proxy.m_inTriggerVolume = 0;

	dgInt32 count = 0;
	dgCollision* const collision1 = proxy.m_referenceCollision;
	dgCollision* const collision2 = proxy.m_floatingCollision;

	if (!(((dgCollisionConvex*)collision1)->m_vertexCount && ((dgCollisionConvex*)collision2)->m_vertexCount)) {
		return count;
	}

	_ASSERTE (collision1->GetCollisionPrimityType() != m_nullCollision);
	_ASSERTE (collision2->GetCollisionPrimityType() != m_nullCollision);

	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	if (proxy.m_continueCollision) {
		dgMatrix matrix (proxy.m_floatingMatrix.MultiplySimd(proxy.m_referenceMatrix.InverseSimd()));
		proxy.m_localMatrixInv = &matrix;
		dgContactSolver mink (proxy);

		dgFloat32 timestep = proxy.m_timestep;
		mink.CalculateVelocitiesSimd(proxy.m_timestep);

		dgInt32 maxContaCount = GetMin (proxy.m_maxContacts, 16); 
		count = mink.HullHullContinueContactsSimd(proxy.m_timestep, proxy.m_contacts, 0, maxContaCount, proxy.m_unconditionalCast);

		if (count) {
			dgFloat32 dist = GetMin (collision1->GetBoxMinRadius(), collision2->GetBoxMinRadius());
			dist *= dist;
			dgFloat32 distTravel = (mink.m_localRelVeloc % mink.m_localRelVeloc) * timestep * timestep;
			if (distTravel * dgFloat32 (0.25f * 0.25f) > dist) {
				dgWorld* const world = proxy.m_referenceBody->GetWorld();
				_ASSERTE (world);
				if (proxy.m_referenceBody->m_mass.m_w > dgFloat32 (0.0f)) {
					world->GetIndirectLock(&proxy.m_referenceBody->m_locks);
					proxy.m_referenceBody->m_solverInContinueCollision = true;
					world->ReleaseIndirectLock(&proxy.m_referenceBody->m_locks);
				}
				if (proxy.m_floatingBody->m_mass.m_w > dgFloat32 (0.0f)) {
					world->GetIndirectLock(&proxy.m_floatingBody->m_locks);
					proxy.m_floatingBody->m_solverInContinueCollision = true;
					world->ReleaseIndirectLock(&proxy.m_floatingBody->m_locks);
				}
			}
		}

	} else {
		dgCollisionID id1 = collision1->GetCollisionPrimityType();
		dgCollisionID id2 = collision2->GetCollisionPrimityType();
		_ASSERTE (id1 != m_nullCollision);
		_ASSERTE (id2 != m_nullCollision);

		switch (id1) 
		{
			case m_sphereCollision:
			{
				switch (id2) 
				{
					case m_sphereCollision:
					{
						count = CalculateSphereToSphereContacts (proxy);
						break;
					}

					case m_capsuleCollision:
					{
						dgCollisionParamProxy tmp(proxy.m_threadIndex, proxy.m_contacts);
						tmp.m_referenceBody = proxy.m_floatingBody;
						tmp.m_floatingBody = proxy.m_referenceBody;
						tmp.m_referenceCollision = proxy.m_floatingCollision;
						tmp.m_floatingCollision = proxy.m_referenceCollision;
						tmp.m_referenceMatrix = proxy.m_floatingMatrix;
						tmp.m_floatingMatrix = proxy.m_referenceMatrix;
						tmp.m_timestep = proxy.m_timestep;
						tmp.m_penetrationPadding = proxy.m_penetrationPadding;
						//tmp.m_contacts = proxy.m_contacts;
						tmp.m_inTriggerVolume = 0;
						tmp.m_isTriggerVolume = proxy.m_isTriggerVolume; 

						count = CalculateCapsuleToSphereContacts (tmp);
						for (dgInt32 i = 0; i < count; i ++) {
							proxy.m_contacts[i].m_normal = tmp.m_contacts[0].m_normal.Scale (dgFloat32 (-1.0f));
						}
						proxy.m_inTriggerVolume = tmp.m_inTriggerVolume;
						break;
					}

					case m_boxCollision:
					{
						dgCollisionParamProxy tmp(proxy.m_threadIndex, proxy.m_contacts);

						tmp.m_referenceBody = proxy.m_floatingBody;
						tmp.m_floatingBody = proxy.m_referenceBody;
						tmp.m_referenceCollision = proxy.m_floatingCollision;
						tmp.m_floatingCollision = proxy.m_referenceCollision;
						tmp.m_referenceMatrix = proxy.m_floatingMatrix;
						tmp.m_floatingMatrix = proxy.m_referenceMatrix;
						tmp.m_timestep = proxy.m_timestep;
						tmp.m_penetrationPadding = proxy.m_penetrationPadding;
						//tmp.m_contacts = proxy.m_contacts;
						tmp.m_inTriggerVolume = 0;
						tmp.m_maxContacts = proxy.m_maxContacts;
						tmp.m_isTriggerVolume = proxy.m_isTriggerVolume; 

						count = CalculateBoxToSphereContacts (tmp);
						if (count) {
							proxy.m_contacts[0].m_normal = tmp.m_contacts[0].m_normal.Scale (dgFloat32 (-1.0f));
						}
						proxy.m_inTriggerVolume = tmp.m_inTriggerVolume;

						break;
					}

					default:
					{
						count = CalculateHullToHullContactsSimd(proxy);
						break;
					}
				}

				break;
			}

			case m_capsuleCollision:
			{
				switch (id2) 
				{
					case m_sphereCollision:
					{
						count = CalculateCapsuleToSphereContacts (proxy);
						break;
					}

					case m_capsuleCollision:
					{
						count = CalculateCapsuleToCapsuleContacts (proxy);
						break;
					}

					default:
					{
						count = CalculateHullToHullContactsSimd(proxy);
						break;
					}
				}
				break;
			}

			case m_boxCollision:
			{
				switch (id2) 
				{
					case m_sphereCollision:
					{
						count = CalculateBoxToSphereContacts (proxy);
						break;
					}

					//case m_boxCollision:
					//{
					//count = CalculateBoxToBoxContacts (body1, body2, contactOut);
					//break;
					//}

					default:
					{
						count = CalculateHullToHullContactsSimd(proxy);
						break;
					}
				}
				break;
			}

			default: 
			{
				count = CalculateHullToHullContactsSimd(proxy);
				break;
			}
		}

		if (count) {
			proxy.m_timestep = dgFloat32 (0.0f);
		}
	}

	dgContactPoint* const contactOut = proxy.m_contacts;
	for (dgInt32 i = 0; i < count; i ++) {
		contactOut[i].m_body0 = proxy.m_referenceBody;
		contactOut[i].m_body1 = proxy.m_floatingBody;
		contactOut[i].m_collision0 = collision1;
		contactOut[i].m_collision1 = collision2;
	}
	return count;

}



dgInt32 dgWorld::CalculateConvexToConvexContacts (dgCollisionParamProxy& proxy) const
{
	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgConvexCollision_RTTI));

	proxy.m_inTriggerVolume = 0;

	dgInt32 count = 0;
	dgCollision* const collision1 = proxy.m_referenceCollision;
	dgCollision* const collision2 = proxy.m_floatingCollision;


	if (!(((dgCollisionConvex*)collision1)->m_vertexCount && ((dgCollisionConvex*)collision2)->m_vertexCount)) {
		return count;
	}

	_ASSERTE (collision1->GetCollisionPrimityType() != m_nullCollision);
	_ASSERTE (collision2->GetCollisionPrimityType() != m_nullCollision);

	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	if (proxy.m_continueCollision) {
		dgMatrix matrix (proxy.m_floatingMatrix * proxy.m_referenceMatrix.Inverse());
		proxy.m_localMatrixInv = &matrix;
		dgContactSolver mink (proxy);

		dgFloat32 timestep = proxy.m_timestep;
		mink.CalculateVelocities(proxy.m_timestep);

		dgInt32 maxContaCount = GetMin (proxy.m_maxContacts, 16); 
		count = mink.HullHullContinueContacts (proxy.m_timestep, proxy.m_contacts, 0, maxContaCount, proxy.m_unconditionalCast);

		if (count) {
			dgFloat32 dist = GetMin (collision1->GetBoxMinRadius(), collision2->GetBoxMinRadius());
			dist *= dist;
			dgFloat32 distTravel = (mink.m_localRelVeloc % mink.m_localRelVeloc) * timestep * timestep;
			if (distTravel * dgFloat32 (0.25f * 0.25f) > dist) {
				dgWorld* const world = proxy.m_referenceBody->GetWorld();
				_ASSERTE (world);
				if (proxy.m_referenceBody->m_mass.m_w > dgFloat32 (0.0f)) {
					world->GetIndirectLock(&proxy.m_referenceBody->m_locks);
					proxy.m_referenceBody->m_solverInContinueCollision = true;
					world->ReleaseIndirectLock(&proxy.m_referenceBody->m_locks);
				}
				if (proxy.m_floatingBody->m_mass.m_w > dgFloat32 (0.0f)) {
					world->GetIndirectLock(&proxy.m_floatingBody->m_locks);
					proxy.m_floatingBody->m_solverInContinueCollision = true;
					world->ReleaseIndirectLock(&proxy.m_floatingBody->m_locks);
				}
			}
		}

	} else {
		dgCollisionID id1 = collision1->GetCollisionPrimityType();
		dgCollisionID id2 = collision2->GetCollisionPrimityType();
		_ASSERTE (id1 != m_nullCollision);
		_ASSERTE (id2 != m_nullCollision);

		switch (id1) 
		{
			case m_sphereCollision:
			{
				switch (id2) 
				{
					case m_sphereCollision:
					{
						count = CalculateSphereToSphereContacts (proxy);
						break;
					}

					case m_capsuleCollision:
					{
						dgCollisionParamProxy tmp(proxy.m_threadIndex, proxy.m_contacts);
						tmp.m_referenceBody = proxy.m_floatingBody;
						tmp.m_floatingBody = proxy.m_referenceBody;
						tmp.m_referenceCollision = proxy.m_floatingCollision;
						tmp.m_floatingCollision = proxy.m_referenceCollision;
						tmp.m_referenceMatrix = proxy.m_floatingMatrix;
						tmp.m_floatingMatrix = proxy.m_referenceMatrix;
						tmp.m_timestep = proxy.m_timestep;
						tmp.m_penetrationPadding = proxy.m_penetrationPadding;
						//tmp.m_contacts = proxy.m_contacts;
						tmp.m_inTriggerVolume = 0;
						tmp.m_isTriggerVolume = proxy.m_isTriggerVolume; 

						count = CalculateCapsuleToSphereContacts (tmp);
						for (dgInt32 i = 0; i < count; i ++) {
							proxy.m_contacts[i].m_normal = tmp.m_contacts[0].m_normal.Scale (dgFloat32 (-1.0f));
						}
						proxy.m_inTriggerVolume = tmp.m_inTriggerVolume;
						break;
					}

					case m_boxCollision:
					{
						dgCollisionParamProxy tmp(proxy.m_threadIndex, proxy.m_contacts);

						tmp.m_referenceBody = proxy.m_floatingBody;
						tmp.m_floatingBody = proxy.m_referenceBody;
						tmp.m_referenceCollision = proxy.m_floatingCollision;
						tmp.m_floatingCollision = proxy.m_referenceCollision;
						tmp.m_referenceMatrix = proxy.m_floatingMatrix;
						tmp.m_floatingMatrix = proxy.m_referenceMatrix;
						tmp.m_timestep = proxy.m_timestep;
						tmp.m_penetrationPadding = proxy.m_penetrationPadding;
						//tmp.m_contacts = proxy.m_contacts;
						tmp.m_inTriggerVolume = 0;
						tmp.m_maxContacts = proxy.m_maxContacts;
						tmp.m_isTriggerVolume = proxy.m_isTriggerVolume; 

						count = CalculateBoxToSphereContacts (tmp);
						if (count) {
							proxy.m_contacts[0].m_normal = tmp.m_contacts[0].m_normal.Scale (dgFloat32 (-1.0f));
						}
						proxy.m_inTriggerVolume = tmp.m_inTriggerVolume;

						break;
					}

					default:
					{
						count = CalculateHullToHullContacts (proxy);
						break;
					}
				}

				break;
			}

			case m_capsuleCollision:
			{
				switch (id2) 
				{
					case m_sphereCollision:
					{
						count = CalculateCapsuleToSphereContacts (proxy);
						break;
					}

					case m_capsuleCollision:
					{
						count = CalculateCapsuleToCapsuleContacts (proxy);
						break;
					}

					default:
					{
						count = CalculateHullToHullContacts (proxy);
						break;
					}
				}
				break;
			}

			case m_boxCollision:
			{
				switch (id2) 
				{
					case m_sphereCollision:
					{
						count = CalculateBoxToSphereContacts (proxy);
						break;
					}

					//case m_boxCollision:
					//{
					//count = CalculateBoxToBoxContacts (body1, body2, contactOut);
					//break;
					//}

					default:
					{
						count = CalculateHullToHullContacts (proxy);
						break;
					}
				}
				break;
			}

			default: 
			{
				count = CalculateHullToHullContacts (proxy);
				break;
			}
		}

		if (count) {
			proxy.m_timestep = dgFloat32 (0.0f);
		}
	}

	dgContactPoint* const contactOut = proxy.m_contacts;
	for (dgInt32 i = 0; i < count; i ++) {
		contactOut[i].m_body0 = proxy.m_referenceBody;
		contactOut[i].m_body1 = proxy.m_floatingBody;
		contactOut[i].m_collision0 = collision1;
		contactOut[i].m_collision1 = collision2;
	}
	return count;
}

dgInt32 dgWorld::CalculatePolySoupToHullContactsDescreteSimd (dgCollisionParamProxy& proxy) const
{
	dgInt32 count;
	dgInt32 count1;
	dgInt32 faceId;
	dgInt32 thread;
	dgInt32 countleft;
	dgInt32 indexCount;
	dgInt32 reduceContactCountLimit;
	dgCollisionBoundPlaneCache planeCache;

	count = 0;
	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionMesh_RTTI));

	//	hullBody = proxy.m_referenceBody;
	//	soupBody = proxy.m_floatingBody;
	dgCollisionConvex* const collision = (dgCollisionConvex*) proxy.m_referenceCollision;
	dgCollisionMesh *const polysoup = (dgCollisionMesh *) proxy.m_floatingCollision;
	_ASSERTE (collision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (polysoup->IsType (dgCollision::dgCollisionMesh_RTTI));
	//	_ASSERTE (polysoup == soupBody->m_collision);

	const dgMatrix& soupMatrix = proxy.m_floatingMatrix;
	const dgPolygonMeshDesc& data = *proxy.m_polyMeshData;
	thread = data.m_threadNumber;

	dgFloat32* const faceSize = data.m_faceMaxSize; 
	dgInt32* const idArray = (dgInt32*)data.m_userAttribute; 
	dgInt32* const indexArray = (dgInt32*)data.m_faceVertexIndex;
	_ASSERTE (data.m_faceCount);
	_ASSERTE (idArray);

	dgCollisionMesh::dgCollisionConvexPolygon* const polygon = polysoup->m_polygon[thread];
	polygon->m_vertex = data.m_vertex;
	polygon->m_stride = dgInt32 (data.m_vertexStrideInBytes / sizeof (dgFloat32));

	reduceContactCountLimit = 0;
	countleft = proxy.m_maxContacts;
	proxy.m_floatingCollision = polygon;

	indexCount = 0;
	dgContactPoint* const contactOut = proxy.m_contacts;
	dgContactSolver mink (proxy);
	dgFloat32 convexSphapeSize = mink.GetShapeClipSize (collision);

	//	for (j = 0; j < data.m_faceCount; j ++) {
	for (dgInt32 j = 0; (j < data.m_faceCount) && (countleft > 0); j ++) {
		polygon->m_count = data.m_faceIndexCount[j];
		polygon->m_index = indexArray;
		polygon->m_index = &indexArray[indexCount];

		if (data.m_faceNormalIndex) {
			polygon->m_normalIndex = data.m_faceNormalIndex[j];
			polygon->m_adjacentNormalIndex = (dgInt32*) &data.m_faceAdjencentEdgeNormal[indexCount];
		} else {
			polygon->m_normalIndex = 0;
			polygon->m_adjacentNormalIndex = NULL;
		}

		if (polygon->QuickTestSimd(collision, mink.m_matrix)) {
			if ((data.m_faceCount < 8) || collision->OOBBTest (mink.m_matrix, polygon, &planeCache))  {
				//			if (collision->OOBBTest (mink.m_matrix, polygon, &planeCache))  {

				if (faceSize && (faceSize[j] > convexSphapeSize)) {
					polygon->BeamClippingSimd (collision, mink.m_matrix, convexSphapeSize);
				}

				faceId = idArray[j];
				proxy.m_maxContacts = countleft;
				proxy.m_contacts = &contactOut[count];

				count1 = mink.HullHullContactsSimd (faceId);
				if (count1) {
					count1 = polygon->ClipContacts (count1, &contactOut[count], soupMatrix);

					count += count1;
					countleft -= count1;
					reduceContactCountLimit += count1;
					if ((reduceContactCountLimit > 24) || (countleft <= 0)) {
						count = ReduceContacts (count, contactOut, proxy.m_maxContacts >> 2, dgFloat32 (1.0e-2f));
						countleft = proxy.m_maxContacts - count;
						reduceContactCountLimit = 0;
					}
				}
			}
		}
		indexCount += data.m_faceIndexCount[j];
	}

	proxy.m_contacts = contactOut;
	count = FilterPolygonEdgeContacts (count, contactOut);

	// restore the pointer
	proxy.m_floatingCollision = polysoup;
	return count;
}


dgInt32 dgWorld::CalculatePolySoupToHullContactsDescrete (dgCollisionParamProxy& proxy) const
{
	dgInt32 count;
	dgInt32 count1;
	dgInt32 faceId;
	dgInt32 thread;
	dgInt32 countleft;
	dgInt32 indexCount;
	dgInt32 reduceContactCountLimit;
	dgCollisionBoundPlaneCache planeCache;

	count = 0;
	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionMesh_RTTI));

	dgCollisionConvex* const collision = (dgCollisionConvex*) proxy.m_referenceCollision;
	dgCollisionMesh *const polysoup = (dgCollisionMesh *) proxy.m_floatingCollision;
	_ASSERTE (collision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (polysoup->IsType (dgCollision::dgCollisionMesh_RTTI));

	const dgMatrix& soupMatrix = proxy.m_floatingMatrix;
	const dgPolygonMeshDesc& data = *proxy.m_polyMeshData;
	thread = data.m_threadNumber;

	_ASSERTE (data.m_faceCount); 
	dgFloat32* const faceSize = data.m_faceMaxSize; 
	dgInt32* const idArray = (dgInt32*)data.m_userAttribute; 
	dgInt32* const indexArray = (dgInt32*)data.m_faceVertexIndex;

	_ASSERTE (idArray);

	dgCollisionMesh::dgCollisionConvexPolygon* const polygon = polysoup->m_polygon[thread];
	polygon->m_vertex = data.m_vertex;
	polygon->m_stride = dgInt32 (data.m_vertexStrideInBytes / sizeof (dgFloat32));

	reduceContactCountLimit = 0;
	countleft = proxy.m_maxContacts;

	indexCount = 0;
	proxy.m_floatingCollision = polygon;
	dgContactPoint* const contactOut = proxy.m_contacts;
	dgContactSolver mink (proxy);
	dgFloat32 convexSphapeSize = mink.GetShapeClipSize (collision);

	for (dgInt32 j = 0; (j < data.m_faceCount) && (countleft > 0); j ++) {
		polygon->m_count = data.m_faceIndexCount[j];
		polygon->m_index = &indexArray[indexCount];

		if (data.m_faceNormalIndex) {
			polygon->m_normalIndex = data.m_faceNormalIndex[j];
			polygon->m_adjacentNormalIndex = (dgInt32*) &data.m_faceAdjencentEdgeNormal[indexCount];
		} else {
			polygon->m_normalIndex = 0;
			polygon->m_adjacentNormalIndex = NULL;
		}

		if (polygon->QuickTest(collision, mink.m_matrix)) {
			if ((data.m_faceCount < 8) || collision->OOBBTest (mink.m_matrix, polygon, &planeCache))  {

				if (faceSize && (faceSize[j] > convexSphapeSize)) {
					polygon->BeamClipping (collision, mink.m_matrix, convexSphapeSize);
				}


				faceId = idArray[j];
				proxy.m_maxContacts = countleft;
				proxy.m_contacts = &contactOut[count];
				count1 = mink.HullHullContacts (faceId);

				if (count1) {
					count1 = polygon->ClipContacts (count1, &contactOut[count], soupMatrix);
					count += count1;
					countleft -= count1;
					reduceContactCountLimit += count1;
					if ((reduceContactCountLimit > 24) || (countleft <= 0)) {
						count = ReduceContacts (count, contactOut, proxy.m_maxContacts >> 2, dgFloat32 (1.0e-2f));
						countleft = proxy.m_maxContacts - count;
						reduceContactCountLimit = 0;
					}
				}
			}
		}
		indexCount += data.m_faceIndexCount[j];
	}

	proxy.m_contacts = contactOut;
	count = FilterPolygonEdgeContacts (count, contactOut);

	// restore the pointer
	proxy.m_floatingCollision = polysoup;
	return count;
}

dgInt32 dgWorld::CalculateHullToHullContactsSimd (dgCollisionParamProxy& proxy) const
{
	dgFloat32 radiusA;
	dgFloat32 radiusB;

	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgConvexCollision_RTTI));

	dgMatrix matrix (proxy.m_floatingMatrix.MultiplySimd(proxy.m_referenceMatrix.InverseSimd()));
	proxy.m_localMatrixInv = &matrix;
	//	dgContactSolver mink (hull1, hull2, penetrationPadding);
	dgContactSolver mink (proxy);

	radiusA = proxy.m_referenceCollision->GetBoxMaxRadius();
	radiusB = proxy.m_floatingCollision->GetBoxMaxRadius();

	//	return mink.HullHullContactsSimd (0);
	if ((radiusA * dgFloat32 (64.0f) < radiusB) || (radiusB * dgFloat32 (64.0f) < radiusA)) {
		return mink.HullHullContactsLarge (0);
	} else {
		return mink.HullHullContactsSimd (0);
	}
}



dgInt32 dgWorld::CalculateHullToHullContacts (dgCollisionParamProxy& proxy) const
{
	dgFloat32 radiusA;
	dgFloat32 radiusB;
	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgConvexCollision_RTTI));

	dgMatrix matrix (proxy.m_floatingMatrix * proxy.m_referenceMatrix.Inverse());
	proxy.m_localMatrixInv = &matrix;

	//	dgContactSolver mink (hull1, hull2, penetrationPadding);
	dgContactSolver mink (proxy);

	radiusA = proxy.m_referenceCollision->GetBoxMaxRadius();
	radiusB = proxy.m_floatingCollision->GetBoxMaxRadius();

#ifdef __USE_DOUBLE_PRECISION__
	return mink.HullHullContactsLarge (0);
#else
	if ((radiusA * dgFloat64 (32.0f) < radiusB) || (radiusB * dgFloat64 (32.0f) < radiusA)) {
		return mink.HullHullContactsLarge (0);
	} else {
		return mink.HullHullContacts (0);
	}
#endif
}



dgInt32 dgWorld::CalculateConvexToNonConvexContactsContinueSimd (dgCollisionParamProxy& proxy) const
{
	dgInt32 count;
	dgInt32 thread;
	dgInt32 countleft;
	dgInt32 indexCount;
	dgInt32 reduceContactCountLimit;
	//	dgInt32* idArray; 
	//	dgInt32* indexArray; 
	dgFloat32 minTime;
	dgFloat32 timestep;
	dgFloat32 oldTimeStep;
	//	dgBody* hullBody; 
	//	dgBody* soupBody; 
	//	dgContactPoint* contactOut;
	//	dgCollisionConvex* collision;
	//	dgCollisionMesh *polysoup;
	//	dgCollisionMesh::dgCollisionConvexPolygon* polygon;

	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionMesh_RTTI));

	//	dgBody* const hullBody = proxy.m_referenceBody;
	//	dgBody* const soupBody = proxy.m_floatingBody;

	dgCollisionConvex* const collision = (dgCollisionConvex*) proxy.m_referenceCollision;
	dgCollisionMesh *const polysoup = (dgCollisionMesh *) proxy.m_floatingCollision;
	_ASSERTE (collision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (polysoup->IsType (dgCollision::dgCollisionMesh_RTTI));
	//	_ASSERTE (polysoup == soupBody->m_collision);
	const dgMatrix& soupMatrix = proxy.m_floatingMatrix;
	const dgPolygonMeshDesc& data = *proxy.m_polyMeshData;

	thread = data.m_threadNumber;
	count = 0;
	dgContactPoint* const contactOut = proxy.m_contacts;
	_ASSERTE (proxy.m_timestep <= dgFloat32 (1.0f));
	_ASSERTE (proxy.m_timestep >= dgFloat32 (0.0f));

	//	timestep = GetMin (proxy.m_timestep, dgFloat32 (1.0f));
	timestep = proxy.m_timestep;
	_ASSERTE (timestep >= dgFloat32 (0.0f));
	_ASSERTE (timestep <= dgFloat32 (1.0f));
	// hack Omega to 0.5
	//hullBody->SetOmega (hullBody->GetOmega().Scale(dgFloat32 (0.9f)));
	_ASSERTE (data.m_faceCount);

	dgFloat32* const faceSize = data.m_faceMaxSize; 
	dgInt32* const idArray = (dgInt32*)data.m_userAttribute; 
	dgInt32* const indexArray = (dgInt32*)data.m_faceVertexIndex;

	dgCollisionMesh::dgCollisionConvexPolygon* const polygon = polysoup->m_polygon[thread];
	polygon->m_vertex = data.m_vertex;
	polygon->m_stride = dgInt32 (data.m_vertexStrideInBytes / sizeof (dgFloat32));

	//	countleft = maxContacts;
	reduceContactCountLimit = 0;
	countleft = proxy.m_maxContacts;


	indexCount = 0;
	dgContactSolver mink (proxy, polygon);
	mink.CalculateVelocitiesSimd (timestep);
	minTime = timestep;
	dgFloat32 convexSphapeSize = mink.GetShapeClipSize (collision);

	//	for (dgInt32 j = 0; j < data.m_faceCount; j ++) {
	for (dgInt32 j = 0; (j < data.m_faceCount) && (countleft > 0); j ++) {
		polygon->m_count = data.m_faceIndexCount[j];
		//		polygon->m_index = indexArray;
		polygon->m_index = &indexArray[indexCount];

		if (data.m_faceNormalIndex) {
			polygon->m_normalIndex = data.m_faceNormalIndex[j];
			polygon->m_adjacentNormalIndex = (dgInt32*) &data.m_faceAdjencentEdgeNormal[indexCount];
		} else {
			polygon->m_normalIndex = 0;
			polygon->m_adjacentNormalIndex = NULL;
		}


		if (polygon->QuickTestContinueSimd(collision, mink.m_matrix)) {
			dgInt32 count1;
			dgInt32 faceId;
			dgFloat32 tmpTimestep;

			faceId = idArray[j];
			oldTimeStep = timestep;
			tmpTimestep = timestep;

			if (faceSize && (faceSize[j] > convexSphapeSize)) {
				polygon->BeamClippingSimd (collision, mink.m_matrix, convexSphapeSize);
			}


			count1 = mink.HullHullContinueContactsSimd (tmpTimestep, &contactOut[count], faceId, countleft, proxy.m_unconditionalCast);
			if (count1) {
				count1 = polygon->ClipContacts (count1, &contactOut[count], soupMatrix);
				if (count1) {
					timestep = tmpTimestep;
					for (dgInt32 i = 0; i < count1; i ++) {
						contactOut[count + i].m_point.m_w = timestep;
					}

					minTime = GetMin (minTime, timestep);	
					if (timestep < oldTimeStep) {
						timestep = oldTimeStep + (timestep - oldTimeStep) * dgFloat32 (0.5f);
					}

					count += count1;
					countleft -= count1;
					reduceContactCountLimit += count1;
					if ((reduceContactCountLimit > 24) || (countleft <= 0)) {
						count = ReduceContacts (count, contactOut, proxy.m_maxContacts >> 2, dgFloat32 (1.0e-2f));
						countleft = proxy.m_maxContacts - count;
						reduceContactCountLimit = 0;
					}
				}
			}
		}

		//		indexArray += data.m_faceIndexCount[j];
		indexCount += data.m_faceIndexCount[j];
	}

	if (count > 1) {
		if (data.m_faceCount > 1) {
			dgFloat32 dt;
			dt = dgFloat32 (0.01f) * dgRsqrt (mink.m_localRelVeloc % mink.m_localRelVeloc);
			for (dgInt32 i = 0; i < count; i ++) {
				dgFloat32 err;
				err = contactOut[i].m_point.m_w - minTime;
				if (dgAbsf (err) > dt) {
					contactOut[i] = contactOut[count - 1];
					i --;
					count --;
				}
			}
		}
		count = FilterPolygonEdgeContacts (count, contactOut);
	}

	proxy.m_timestep = minTime;
	return count;
}


dgInt32 dgWorld::CalculateConvexToNonConvexContactsContinue (dgCollisionParamProxy& proxy) const
{
	dgInt32 count;
	dgInt32 thread;
	dgInt32 countleft;
	dgInt32 indexCount;
	dgInt32 reduceContactCountLimit;
	dgFloat32 minTime;
	dgFloat32 timestep;
	dgFloat32 oldTimeStep;

	_ASSERTE (proxy.m_referenceCollision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (proxy.m_floatingCollision->IsType (dgCollision::dgCollisionMesh_RTTI));


	dgCollisionConvex* const collision = (dgCollisionConvex*) proxy.m_referenceCollision;
	dgCollisionMesh *const polysoup = (dgCollisionMesh *) proxy.m_floatingCollision;
	_ASSERTE (collision->IsType (dgCollision::dgConvexCollision_RTTI));
	_ASSERTE (polysoup->IsType (dgCollision::dgCollisionMesh_RTTI));

	const dgMatrix& soupMatrix = proxy.m_floatingMatrix;
	const dgPolygonMeshDesc& data = *proxy.m_polyMeshData;
	thread = data.m_threadNumber;

	count = 0;
	dgContactPoint* const contactOut = proxy.m_contacts;
	_ASSERTE (proxy.m_timestep <= dgFloat32 (1.0f));
	_ASSERTE (proxy.m_timestep >= dgFloat32 (0.0f));


	timestep = proxy.m_timestep;
	_ASSERTE (timestep >= dgFloat32 (0.0f));
	_ASSERTE (timestep <= dgFloat32 (1.0f));
	// hack Omega to 0.5
	//hullBody->SetOmega (hullBody->GetOmega().Scale(dgFloat32 (0.9f)));
	_ASSERTE (data.m_faceCount);

	dgFloat32* const faceSize = data.m_faceMaxSize; 
	dgInt32* const idArray = (dgInt32*)data.m_userAttribute; 
	dgInt32* const indexArray = (dgInt32*)data.m_faceVertexIndex;

	dgCollisionMesh::dgCollisionConvexPolygon* const polygon = polysoup->m_polygon[thread];
	polygon->m_vertex = data.m_vertex;
	polygon->m_stride = dgInt32 (data.m_vertexStrideInBytes / sizeof (dgFloat32));

	reduceContactCountLimit = 0;
	countleft = proxy.m_maxContacts;

	indexCount = 0;
	dgContactSolver mink (proxy, polygon);
	minTime = timestep;
	mink.CalculateVelocities (timestep);
	dgFloat32 convexSphapeSize = mink.GetShapeClipSize (collision);

	//	for (dgInt32 j = 0; j < data.m_faceCount; j ++) {
	for (dgInt32 j = 0; (j < data.m_faceCount) && (countleft > 0); j ++) {
		polygon->m_count = data.m_faceIndexCount[j];
		//		polygon->m_index = indexArray;
		polygon->m_index = &indexArray[indexCount];

		if (data.m_faceNormalIndex) {
			polygon->m_normalIndex = data.m_faceNormalIndex[j];
			polygon->m_adjacentNormalIndex = (dgInt32*) &data.m_faceAdjencentEdgeNormal[indexCount];
		} else {
			polygon->m_normalIndex = 0;
			polygon->m_adjacentNormalIndex = NULL;
		}

		if (polygon->QuickTestContinue(collision, mink.m_matrix)) {
			dgInt32 count1;
			dgInt32 faceId;
			dgFloat32 tmpTimestep;

			faceId = idArray[j];
			oldTimeStep = timestep;
			tmpTimestep = timestep;

			if (faceSize && (faceSize[j] > convexSphapeSize)) {
				polygon->BeamClipping (collision, mink.m_matrix, convexSphapeSize);
			}

			count1 = mink.HullHullContinueContacts (tmpTimestep, &contactOut[count], faceId, countleft, proxy.m_unconditionalCast);
			if (count1) {
				count1 = polygon->ClipContacts (count1, &contactOut[count], soupMatrix);
				if (count1) {
					timestep = tmpTimestep;
					for (dgInt32 i = 0; i < count1; i ++) {
						contactOut[count + i].m_point.m_w = timestep;
					}

					minTime = GetMin (minTime, timestep);	
					if (timestep < oldTimeStep) {
						timestep = oldTimeStep + (timestep - oldTimeStep) * dgFloat32 (0.5f);
					}

					count += count1;
					countleft -= count1;
					reduceContactCountLimit += count1;
					if ((reduceContactCountLimit > 24) || (countleft <= 0)) {
						count = ReduceContacts (count, contactOut, proxy.m_maxContacts >> 2, dgFloat32 (1.0e-2f));
						countleft = proxy.m_maxContacts - count;
						reduceContactCountLimit = 0;
					}
				}
			}
		}

		//		indexArray += data.m_faceIndexCount[j];
		indexCount += data.m_faceIndexCount[j];
	}


	if (count > 1) {
		if (data.m_faceCount > 1) {
			dgFloat32 dt;
			dt = dgFloat32 (0.01f) * dgRsqrt (mink.m_localRelVeloc % mink.m_localRelVeloc);
			for (dgInt32 i = 0; i < count; i ++) {
				dgFloat32 err;
				err = contactOut[i].m_point.m_w - minTime;
				if (dgAbsf (err) > dt) {
					contactOut[i] = contactOut[count - 1];
					i --;
					count --;
				}
			}
		}
		count = FilterPolygonEdgeContacts (count, contactOut);
	}

	proxy.m_timestep = minTime;
	return count;
}

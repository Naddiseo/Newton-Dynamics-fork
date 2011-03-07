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
#include "dgCollisionBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

dgConvexSimplexEdge dgCollisionBox::m_edgeArray[24] = 
{
	{1, &m_edgeArray[3], &m_edgeArray[12], &m_edgeArray[5]}, 
	{2, &m_edgeArray[6], &m_edgeArray[3], &m_edgeArray[7]}, 
	{7, &m_edgeArray[12], &m_edgeArray[6], &m_edgeArray[14]}, 
	{0, &m_edgeArray[0], &m_edgeArray[9], &m_edgeArray[1]}, 
	{3, &m_edgeArray[9], &m_edgeArray[15], &m_edgeArray[11]}, 
	{6, &m_edgeArray[15], &m_edgeArray[0], &m_edgeArray[16]}, 
	{0, &m_edgeArray[1], &m_edgeArray[18], &m_edgeArray[2]}, 
	{3, &m_edgeArray[10], &m_edgeArray[1], &m_edgeArray[9]}, 
	{5, &m_edgeArray[18], &m_edgeArray[10], &m_edgeArray[20]}, 
	{1, &m_edgeArray[4], &m_edgeArray[7], &m_edgeArray[3]}, 
	{2, &m_edgeArray[7], &m_edgeArray[21], &m_edgeArray[8]}, 
	{4, &m_edgeArray[21], &m_edgeArray[4], &m_edgeArray[22]}, 
	{0, &m_edgeArray[2], &m_edgeArray[16], &m_edgeArray[0]}, 
	{6, &m_edgeArray[16], &m_edgeArray[19], &m_edgeArray[17]}, 
	{5, &m_edgeArray[19], &m_edgeArray[2], &m_edgeArray[18]}, 
	{1, &m_edgeArray[5], &m_edgeArray[22], &m_edgeArray[4]}, 
	{7, &m_edgeArray[13], &m_edgeArray[5], &m_edgeArray[12]}, 
	{4, &m_edgeArray[22], &m_edgeArray[13], &m_edgeArray[23]}, 
	{2, &m_edgeArray[8], &m_edgeArray[14], &m_edgeArray[6]}, 
	{7, &m_edgeArray[14], &m_edgeArray[23], &m_edgeArray[13]}, 
	{4, &m_edgeArray[23], &m_edgeArray[8], &m_edgeArray[21]}, 
	{3, &m_edgeArray[11], &m_edgeArray[20], &m_edgeArray[10]}, 
	{6, &m_edgeArray[17], &m_edgeArray[11], &m_edgeArray[15]}, 
	{5, &m_edgeArray[20], &m_edgeArray[17], &m_edgeArray[19]}, 
};


dgCollisionBox::dgCollisionBox(
	dgMemoryAllocator* allocator, 
	dgUnsigned32 signature, 
	dgFloat32 size_x, 
	dgFloat32 size_y, 
	dgFloat32 size_z, 
	const dgMatrix& matrix)
	:dgCollisionConvex(allocator, signature, matrix, m_boxCollision)
{
	m_destructionImpulse = dgFloat32 (1.0e20f); 
	Init (size_x, size_y, size_z);
}

dgCollisionBox::dgCollisionBox(dgWorld* const world, dgDeserialize deserialization, void* const userData)
	:dgCollisionConvex (world, deserialization, userData)
{
	dgVector size;
	deserialization (userData, &size, sizeof (dgVector));
	m_destructionImpulse = size.m_w;
	Init (size.m_x, size.m_y, size.m_z);
}

void dgCollisionBox::Init (dgFloat32 size_x, dgFloat32 size_y, dgFloat32 size_z)
{
	m_rtti |= dgCollisionBox_RTTI;
	m_size[0].m_x = dgAbsf (size_x) * dgFloat32 (0.5f);
	m_size[0].m_y = dgAbsf (size_y) * dgFloat32 (0.5f);
	m_size[0].m_z = dgAbsf (size_z) * dgFloat32 (0.5f);
	m_size[0].m_w = dgFloat32 (0.0f);

	m_size[1].m_x = - m_size[0].m_x;
	m_size[1].m_y = - m_size[0].m_y;
	m_size[1].m_z = - m_size[0].m_z;
	m_size[1].m_w = dgFloat32 (0.0f);

	m_edgeCount = 24;
	m_vertexCount = 8;

	m_vertex[0]	= dgVector ( m_size[0].m_x,  m_size[0].m_y,  m_size[0].m_z, dgFloat32 (1.0f));
	m_vertex[1]	= dgVector (-m_size[0].m_x,  m_size[0].m_y,  m_size[0].m_z, dgFloat32 (1.0f));
	m_vertex[2]	= dgVector ( m_size[0].m_x, -m_size[0].m_y,  m_size[0].m_z, dgFloat32 (1.0f));
	m_vertex[3]	= dgVector (-m_size[0].m_x, -m_size[0].m_y,  m_size[0].m_z, dgFloat32 (1.0f));
	m_vertex[4]	= dgVector (-m_size[0].m_x, -m_size[0].m_y, -m_size[0].m_z, dgFloat32 (1.0f));
	m_vertex[5]	= dgVector ( m_size[0].m_x, -m_size[0].m_y, -m_size[0].m_z, dgFloat32 (1.0f));
	m_vertex[6]	= dgVector (-m_size[0].m_x,  m_size[0].m_y, -m_size[0].m_z, dgFloat32 (1.0f));
	m_vertex[7]	= dgVector ( m_size[0].m_x,  m_size[0].m_y, -m_size[0].m_z, dgFloat32 (1.0f));

	simd_128 tmp;
	Transpose4x4Simd_128 (m_vertex_sse[0], m_vertex_sse[1], m_vertex_sse[2], tmp, 
						 (simd_128&)m_vertex[0], (simd_128&)m_vertex[1], (simd_128&)m_vertex[2], (simd_128&)m_vertex[3]);
	Transpose4x4Simd_128 (m_vertex_sse[3], m_vertex_sse[4], m_vertex_sse[5], tmp, 
						 (simd_128&)m_vertex[4], (simd_128&)m_vertex[5], (simd_128&)m_vertex[6], (simd_128&)m_vertex[7]);

	dgCollisionConvex::m_vertex = m_vertex;
	dgCollisionConvex::m_simplex = m_edgeArray;
	SetVolumeAndCG ();
}

dgCollisionBox::~dgCollisionBox()
{
//	m_shapeRefCount --;
//	_ASSERTE (m_shapeRefCount >= 0);

	dgCollisionConvex::m_simplex = NULL;
	dgCollisionConvex::m_vertex = NULL;
}

void dgCollisionBox::SetCollisionBBox (const dgVector& p0__, const dgVector& p1__)
{
	_ASSERTE (0);
}

dgInt32 dgCollisionBox::CalculateSignature () const
{
	dgUnsigned32 buffer[2 * sizeof (dgMatrix) / sizeof(dgInt32)];

	memset (buffer, 0, sizeof (buffer));
	buffer[0] = m_collsionId;
	buffer[1] = Quantize (m_size[0].m_x);
	buffer[2] = Quantize (m_size[0].m_y);
	buffer[3] = Quantize (m_size[0].m_z);
	memcpy (&buffer[4], &m_offset, sizeof (dgMatrix));
	return dgInt32 (MakeCRC(buffer, sizeof (buffer)));
}

dgVector dgCollisionBox::SupportVertexSimd (const dgVector& dir) const
{
	simd_128 test ((simd_128&)dir > simd_128(dgFloat32 (0.0f)));
  	return ((simd_128&)m_size[0] & test) | ((simd_128&)m_size[1]).AndNot(test); 
}

dgVector dgCollisionBox::SupportVertex (const dgVector& dir) const
{
	_ASSERTE (dgAbsf(dir % dir - dgFloat32 (1.0f)) < dgFloat32 (1.0e-3f));

#if 0
	dgFloatSign* const ptr =  (dgFloatSign*) &dir; 

	dgInt32 x = -(ptr[0].m_integer.m_iVal >> 31);
	dgInt32 y = -(ptr[1].m_integer.m_iVal >> 31);
	dgInt32 z = -(ptr[2].m_integer.m_iVal >> 31);
	return dgVector (m_size[x].m_x, m_size[y].m_y, m_size[z].m_z, dgFloat32 (0.0f));

#else
	// according to Intel in latest possessors this is better, because read after write are very, very expensive
	return dgVector (dir.m_x < dgFloat32 (0.0f) ? m_size[1].m_x : m_size[0].m_x, 
					 dir.m_y < dgFloat32 (0.0f) ? m_size[1].m_y : m_size[0].m_y, 
					 dir.m_z < dgFloat32 (0.0f) ? m_size[1].m_z : m_size[0].m_z, dgFloat32 (0.0f));
#endif

}


void dgCollisionBox::CalcAABB (const dgMatrix &matrix, dgVector &p0, dgVector &p1) const
{
	dgFloat32 x = m_size[0].m_x * dgAbsf(matrix[0][0]) + m_size[0].m_y * dgAbsf(matrix[1][0]) + m_size[0].m_z * dgAbsf(matrix[2][0]) + DG_MAX_COLLISION_PADDING;  
	dgFloat32 y = m_size[0].m_x * dgAbsf(matrix[0][1]) + m_size[0].m_y * dgAbsf(matrix[1][1]) + m_size[0].m_z * dgAbsf(matrix[2][1]) + DG_MAX_COLLISION_PADDING;  
	dgFloat32 z = m_size[0].m_x * dgAbsf(matrix[0][2]) + m_size[0].m_y * dgAbsf(matrix[1][2]) + m_size[0].m_z * dgAbsf(matrix[2][2]) + DG_MAX_COLLISION_PADDING;  

	p0.m_x = matrix[3][0] - x;
	p1.m_x = matrix[3][0] + x;

	p0.m_y = matrix[3][1] - y;
	p1.m_y = matrix[3][1] + y;

	p0.m_z = matrix[3][2] - z;
	p1.m_z = matrix[3][2] + z;

	p0.m_w = dgFloat32 (0.0f);
	p1.m_w = dgFloat32 (0.0f);
}


void dgCollisionBox::CalcAABBSimd (const dgMatrix &matrix, dgVector &p0, dgVector &p1) const
{
	simd_128 size (m_aabbPadding + ((simd_128&)matrix[0] & m_signMask) * (simd_128&) m_size_x + 
								   ((simd_128&)matrix[1] & m_signMask) * (simd_128&) m_size_y + 
								   ((simd_128&)matrix[2] & m_signMask) * (simd_128&) m_size_z); 

	(simd_128&)p0 = ((simd_128&)matrix[3] - size) & m_triplexMask;
	(simd_128&)p1 = ((simd_128&)matrix[3] + size) & m_triplexMask;
}


dgFloat32 dgCollisionBox::RayCast (const dgVector& localP0, const dgVector& localP1, dgContactPoint& contactOut, OnRayPrecastAction preFilter, const dgBody* const body, void* const userData) const
{
	if (PREFILTER_RAYCAST (preFilter, body, this, userData)) {
		return dgFloat32 (1.2f);
	}

#if 0
	bool cut;
	dgFloat32 t;
	dgFloat32 s;
	dgFloat32 s1;

	cut = false;
	s = dgFloat32 (0.0f);

	dgVector p0 (localP0); 
	dgVector p1 (localP1); 
	for (int i = 0; i < 3; i ++) {
		dgFloatSign tmp0;
		dgFloatSign tmp1;

		tmp0.m_fVal = m_size[0][i] - p0[i];
		if (tmp0.m_integer.m_iVal > 0) {
			tmp1.m_fVal = m_size[0][i] - p1[i];
			if (tmp1.m_integer.m_iVal < 0) {
				t = tmp0.m_fVal / (p1[i] - p0[i]);
				p1 = p0 + (p1 - p0).Scale (t);

			}
		} else {
			tmp1.m_fVal = m_size[0][i] - p1[i];
			if (tmp1.m_integer.m_iVal > 0) {
				s1 = (m_size[0][i] - localP0[i]) / (localP1[i] - localP0[i]);
				if (s1 > s) {
					s = s1;
					cut = true;
					contactOut.m_normal = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
					contactOut.m_normal[i] = dgFloat32 (1.0f);
					contactOut.m_userId = SetUserDataID();
				}
				t = tmp0.m_fVal / (p1[i] - p0[i]);
				p0 = p0 + (p1 - p0).Scale (t);
			} else {
				return dgFloat32 (1.2f);
			}
		}
		
		tmp0.m_fVal = -m_size[0][i] - p0[i];
		if (tmp0.m_integer.m_iVal < 0) {
			tmp1.m_fVal = -m_size[0][i] - p1[i];
			if (tmp1.m_integer.m_iVal > 0) {
				t = tmp0.m_fVal / (p1[i] - p0[i]);
				p1 = p0 + (p1 - p0).Scale (t);
			}
		} else {
			tmp1.m_fVal = -m_size[0][i] - p1[i];
			if (tmp1.m_integer.m_iVal < 0) {
				s1 = (-m_size[0][i] - localP0[i]) / (localP1[i] - localP0[i]);
				if (s1 > s) {
					s = s1;
					cut = true;
					contactOut.m_normal = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
					contactOut.m_normal[i] = -dgFloat32 (1.0f);
					contactOut.m_userId = SetUserDataID();
				}

				t = tmp0.m_fVal / (p1[i] - p0[i]);
				p0 = p0 + (p1 - p0).Scale (t);
			} else {
				return dgFloat32 (1.2f);
			}
		}
	}
	return cut ? s : 1.2f;
#else

	dgInt32 index = 0;
	dgFloat32 signDir = dgFloat32 (0.0f);
	dgFloat32 tmin = dgFloat32 (0.0f);
	dgFloat32 tmax = dgFloat32 (1.0f);
	for (dgInt32 i = 0; i < 3; i++) {
		dgFloat32 dp = localP1[i] - localP0[i];
		if (dgAbsf (dp) < dgFloat32 (1.0e-8f)) {
			if (localP0[i] <= m_size[1][i] || localP0[i] >= m_size[0][i]) {
				return dgFloat32 (1.2f);
			}
		} else {
			dp = dgFloat32 (1.0f) / dp; 
			dgFloat32 t1 = (m_size[1][i] - localP0[i]) * dp;
			dgFloat32 t2 = (m_size[0][i] - localP0[i]) * dp;

			dgFloat32 sign = dgFloat32 (-1.0f);
			if (t1 > t2) {
				sign = 1;
				Swap(t1, t2);
			}
			if (t1 > tmin) {
				signDir = sign;
				index = i;
				tmin = t1;
			}
			if (t2 < tmax) {
				tmax = t2;
			}
			if (tmin > tmax) {
				return dgFloat32 (1.2f);
			}
		}
	}

	if (tmin >= dgFloat32 (0.0f)) {
		_ASSERTE (tmin < 1.0f);
		contactOut.m_normal = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
		contactOut.m_normal[index] = signDir;
		//contactOut.m_userId = SetUserData();
		contactOut.m_userId = SetUserDataID();
	} else {
		tmin = dgFloat32 (1.2f);
	}
	return tmin;
#endif
}

dgFloat32 dgCollisionBox::RayCastSimd (const dgVector& localP0, const dgVector& localP1, dgContactPoint& contactOut, OnRayPrecastAction preFilter, const dgBody* const body, void* const userData) const
{
	return RayCast (localP0, localP1, contactOut, preFilter, body, userData);
}


dgFloat32 dgCollisionBox::CalculateMassProperties (dgVector& inertia, dgVector& crossInertia, dgVector& centerOfMass) const
{
//	volume = dgCollisionConvex::CalculateMassProperties (inertia, crossInertia, centerOfMass);

	centerOfMass = GetOffsetMatrix().m_posit;
	dgFloat32 volume = dgFloat32 (8.0f) * m_size[0].m_x * m_size[0].m_y * m_size[0].m_z; 

	dgVector II (dgFloat32 (1.0f / 3.0f) * volume * (m_size[0].m_y * m_size[0].m_y + m_size[0].m_z * m_size[0].m_z),
				 dgFloat32 (1.0f / 3.0f) * volume * (m_size[0].m_x * m_size[0].m_x + m_size[0].m_z * m_size[0].m_z),
				 dgFloat32 (1.0f / 3.0f) * volume * (m_size[0].m_x * m_size[0].m_x + m_size[0].m_y * m_size[0].m_y),
				 dgFloat32 (0.0f));

	dgMatrix inertiaTensor (dgGetIdentityMatrix());

	inertiaTensor[0][0] = II.m_x;
	inertiaTensor[1][1] = II.m_y;
	inertiaTensor[2][2] = II.m_z;

	inertiaTensor = GetOffsetMatrix().Inverse() * inertiaTensor * GetOffsetMatrix();

	crossInertia.m_x = inertiaTensor[1][2] - volume * centerOfMass.m_y * centerOfMass.m_z;
	crossInertia.m_y = inertiaTensor[0][2] - volume * centerOfMass.m_z * centerOfMass.m_x;
	crossInertia.m_z = inertiaTensor[0][1] - volume * centerOfMass.m_x * centerOfMass.m_y;
	crossInertia.m_w = dgFloat32 (0.0f);

	dgVector central (centerOfMass.CompProduct(centerOfMass));
	inertia.m_x = inertiaTensor[0][0] + volume * (central.m_y + central.m_z);
	inertia.m_y = inertiaTensor[1][1] + volume * (central.m_z + central.m_x);
	inertia.m_z = inertiaTensor[2][2] + volume * (central.m_x + central.m_y);
	inertia.m_w = dgFloat32 (0.0f);

	centerOfMass = centerOfMass.Scale (volume);
	return volume;
}


void dgCollisionBox::SetBreakImpulse(dgFloat32 force)
{
	m_destructionImpulse = force;
}

dgFloat32 dgCollisionBox::GetBreakImpulse() const
{
	return m_destructionImpulse;
}



dgInt32 dgCollisionBox::CalculatePlaneIntersection (const dgVector& normal, const dgVector& point, dgVector* const contactsOut) const
{
	dgFloat32 test[8];
	dgPlane plane (normal, - (normal % point));

	dgConvexSimplexEdge* edge = NULL;
	dgFloat32 side1 = dgFloat32 (1.0e20f);
	for (dgInt32 i = 0; i < 8; i ++) {
		dgFloat32 side0 = plane.Evalue (m_vertex[i]);
		test[i] = side0;
		if (side0 > dgFloat32 (0.0f)) {
			if (side0 < side1) {
				side1 = side0;
				edge = m_supportVertexStarCuadrant[i];
			}
		}
	}

	dgInt32 count = 0;
	if (edge) {
		_ASSERTE (test[edge->m_vertex] > dgFloat32 (0.0f));

		dgConvexSimplexEdge* ptr = edge;
		dgConvexSimplexEdge* firstEdge = NULL;
		dgFloat32 side0 = test[edge->m_vertex];
		do {
			_ASSERTE (m_vertex[ptr->m_twin->m_vertex].m_w == dgFloat32 (1.0f));
			dgFloat32 side1 = test[ptr->m_twin->m_vertex];
			if (side1 < side0) {
				if (side1 < dgFloat32 (0.0f)) {
					firstEdge = ptr;
					break;
				}

				side0 = side1;
				edge = ptr->m_twin;
				ptr = edge;
			}
			ptr = ptr->m_twin->m_next;
		} while (ptr != edge);

		if (firstEdge) {
			edge = firstEdge;
			ptr = edge;
			do {
				dgVector dp (m_vertex[ptr->m_twin->m_vertex] - m_vertex[ptr->m_vertex]);

				dgFloat32 t = plane % dp;
				if (t >= dgFloat32 (-1.e-24f)) {
					t = dgFloat32 (0.0f);
				} else {
					t = test[ptr->m_vertex] / t;
					if (t > dgFloat32 (0.0f)) {
						t = dgFloat32 (0.0f);
					}
					if (t < dgFloat32 (-1.0f)) {
						t = dgFloat32 (-1.0f);
					}
				}

				_ASSERTE (t <= dgFloat32 (0.01f));
				_ASSERTE (t >= dgFloat32 (-1.05f));
				contactsOut[count] = m_vertex[ptr->m_vertex] - dp.Scale (t);
				count ++;

				dgConvexSimplexEdge* ptr1 = ptr1 = ptr->m_next;
				for (; ptr1 != ptr; ptr1 = ptr1->m_next) {
					dgInt32 index0 = ptr1->m_twin->m_vertex;
					if (test[index0] >= dgFloat32 (0.0f)) {
						_ASSERTE (test[ptr1->m_vertex] <= dgFloat32 (0.0f));
						break;
					}
				}
				_ASSERTE (ptr != ptr1);
				ptr = ptr1->m_twin;

			} while ((ptr != edge) && (count < 8));
		}
	}

	if (count > 1) {
		count = RectifyConvexSlice (count, normal, contactsOut);
	}
	return count;
}


dgInt32 dgCollisionBox::CalculatePlaneIntersectionSimd (const dgVector& normal, const dgVector& point, dgVector* const contactsOut) const
{
	dgInt32 count = 0;
	simd_128 plane (((simd_128&)normal & m_triplexMask));
	plane = plane - plane.DotProduct((simd_128&)point).AndNot(m_triplexMask);

	simd_128 plane_a;
	simd_128 plane_b;
	simd_128 plane_c;
	simd_128 plane_d;
	simd_128 side[2];

	Transpose4x4Simd_128 (plane_a, plane_b, plane_c, plane_d, plane, plane, plane, plane);
	
	side[0] = plane_a * m_vertex_sse[0] + plane_b * m_vertex_sse[1] + plane_c * m_vertex_sse[2] + plane_d;
	side[1] = plane_a * m_vertex_sse[3] + plane_b * m_vertex_sse[4] + plane_c * m_vertex_sse[5] + plane_d;
	
	simd_128 zero (dgFloat32 (0.0f));
	simd_128 huge (dgFloat32 (1.0e20f));
	simd_128 negOne (dgFloat32 (-1.0f));
	
	simd_128 sideTest (side[0] > zero);
	simd_128 positiveSide0 ((side[0] & sideTest) | huge.AndNot(sideTest));
	simd_128 index0 ((m_index_0123 & sideTest) | negOne.AndNot(sideTest));

	sideTest = side[1] > zero;
	simd_128 positiveSide1 ((side[1] & sideTest) | huge.AndNot(sideTest));
	simd_128 index1 ((m_index_4567 & sideTest) | negOne.AndNot(sideTest));

	sideTest = positiveSide0 < positiveSide1;
	positiveSide0 = positiveSide0.GetMin(positiveSide1);
	index0 = (index0 & sideTest) | index1.AndNot(sideTest);

	positiveSide1 = positiveSide0.MoveHighToLow(positiveSide0);
	sideTest = positiveSide0 < positiveSide1;
	positiveSide0 = positiveSide0.GetMin(positiveSide1);
	index0 = (index0 & sideTest) | index0.MoveHighToLow(index0).AndNot(sideTest);

	positiveSide1 = positiveSide0.PackLow(positiveSide0);
	sideTest = positiveSide0 < positiveSide1.MoveHighToLow(positiveSide1);
	index1 = index0.PackLow(index0);
	dgInt32 index = ((index0 & sideTest) | (index1.MoveHighToLow(index1).AndNot(sideTest))).GetInt();

	if (index >= 0) {
		dgConvexSimplexEdge* edge = m_supportVertexStarCuadrant[index];

		const dgFloat32* const test = (dgFloat32*) &side[0];
		_ASSERTE (test[edge->m_vertex] > dgFloat32 (0.0f));

		dgConvexSimplexEdge* ptr = edge;
		dgConvexSimplexEdge* firstEdge = NULL;

		simd_128 side0 (test[edge->m_vertex]);
		do {
			_ASSERTE (m_vertex[ptr->m_twin->m_vertex].m_w == dgFloat32 (1.0f));
			simd_128 side1 (test[ptr->m_twin->m_vertex]);
			if ((side1 < side0).GetSignMask()) {
				if ((side1 < zero).GetSignMask()) {
					firstEdge = ptr;
					break;
				}
				side0 = side1;
				edge = ptr->m_twin;
				ptr = edge;
			}
			ptr = ptr->m_twin->m_next;
		} while (ptr != edge);

		if (firstEdge) {
			edge = firstEdge;
			ptr = edge;
			do {

				dgInt32 index0 = ptr->m_vertex;
				dgInt32 index1 = ptr->m_twin->m_vertex;
				_ASSERTE (m_vertex[index0].m_w == dgFloat32 (1.0f));
				_ASSERTE (m_vertex[index1].m_w == dgFloat32 (1.0f));

				simd_128 p1p0 ((simd_128&)m_vertex[index1] - (simd_128&)m_vertex[index0]);
				simd_128 dot (plane.DotProduct(p1p0));
				
				simd_128 den (simd_128(test[index0]) / dot);
				den = zero.GetMin (negOne.GetMax (den));

				_ASSERTE (den.m_type.m128_f32[0] <= dgFloat32 (0.0f));
				_ASSERTE (den.m_type.m128_f32[0] >= dgFloat32 (-1.0f));
				(simd_128&)contactsOut[count] = ((simd_128&)m_vertex[index0] & m_triplexMask) - p1p0 * den;
				count ++;

				dgConvexSimplexEdge* ptr1 = ptr1 = ptr->m_next;
				for (; ptr1 != ptr; ptr1 = ptr1->m_next) {
					dgInt32 index0 = ptr1->m_twin->m_vertex;
					if ((simd_128 (test[index0]) >= zero).GetSignMask()) {
						_ASSERTE (test[ptr1->m_vertex] <= dgFloat32 (0.0f));
						break;
					}
				}
				_ASSERTE (ptr != ptr1);
				ptr = ptr1->m_twin;

			} while ((ptr != edge) && (count < 8));
		}
	}
	if (count > 1) {
		count = RectifyConvexSlice (count, normal, contactsOut);
	}
	return count;

}

void dgCollisionBox::GetCollisionInfo(dgCollisionInfo* info) const
{
	dgCollisionConvex::GetCollisionInfo(info);

	info->m_box.m_x = m_size[0].m_x * dgFloat32 (2.0f);
	info->m_box.m_y = m_size[0].m_y * dgFloat32 (2.0f);
	info->m_box.m_z = m_size[0].m_z * dgFloat32 (2.0f);
	info->m_offsetMatrix = GetOffsetMatrix();
//	strcpy (info->m_collisionType, "box");
	info->m_collisionType = m_collsionId;
}

void dgCollisionBox::Serialize(dgSerialize callback, void* const userData) const
{
	dgVector size (m_size[0].Scale (dgFloat32 (2.0f)));
	size.m_w = m_destructionImpulse;

	SerializeLow(callback, userData);
	callback (userData, &size, sizeof (dgVector));
}



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

#ifndef __dgMinkowskiConv__
#define __dgMinkowskiConv__


#define DG_MINK_MAX_FACES								64
#define DG_MINK_MAX_FACES_SIZE							(DG_MINK_MAX_FACES + 16)
#define DG_MINK_MAX_POINTS								64
#define DG_MINK_MAX_POINTS_SIZE							(DG_MINK_MAX_POINTS + 16)
#define DG_MAX_SIMPLEX_FACE								(DG_MINK_MAX_POINTS * 4)
#define DG_HEAP_EDGE_COUNT								256 
#define DG_ROBUST_PLANE_CLIP							dgFloat32 (1.0f / 256.0f)
#define DG_DISTANCE_TOLERANCE							dgFloat32 (1.0e-3f)
#define DG_DISTANCE_TOLERANCE_ZERO						dgFloat32 (1.0e-24f)
#define DG_UPDATE_SEPARATING_PLANE_MAX_ITERATION		32
#define DG_UPDATE_SEPARATING_PLANE_DISTANCE_TOLERANCE1	(DG_DISTANCE_TOLERANCE * dgFloat32 (1.0e-1f))
#define DG_UPDATE_SEPARATING_PLANE_DISTANCE_TOLERANCE2	(DG_DISTANCE_TOLERANCE * dgFloat32 (1.0e-3f))
#define DG_MIN_VERTEX_ERROR								(dgFloat32 (1.0e-3f))
#define DG_MIN_VERTEX_ERROR_2							(DG_MIN_VERTEX_ERROR * DG_MIN_VERTEX_ERROR)
#define DG_FALLBACK_SEPARATING_PLANE_ITERATIONS			32
#define DG_FALLBACK_SEPARATING_DIST_TOLERANCE			dgFloat32(1.0e-6f)
#define DG_CALCULATE_SEPARATING_PLANE_ERROR				(DG_ROBUST_PLANE_CLIP * dgFloat32 (2.0f))
#define DG_CALCULATE_SEPARATING_PLANE_ERROR1			(DG_ROBUST_PLANE_CLIP * dgFloat32 (0.5f))
#define DG_GETADJACENTINDEX_ACTIVE(x)					((!m_simplex[x->m_adjancentFace[1]].m_isActive) ? 1 : ((!m_simplex[x->m_adjancentFace[2]].m_isActive) ? 2 : 0))
#define DG_GETADJACENTINDEX_VERTEX(x,v)					(((x->m_vertex[1] == v) ? 1 : 0) | ((x->m_vertex[2] == v) ? 2 : 0))
//#define DG_RSQRT_SIMD_S(x,y)							{simd_type tmp0 = simd_rsqrt_s(x); y = simd_mul_s (simd_mul_s(dgContactSolver::m_nrh0p5, tmp0), simd_mul_sub_v (dgContactSolver::m_nrh3p0, simd_mul_s (x, tmp0), tmp0));}


DG_MSC_VECTOR_ALIGMENT 
class dgContactSolver
{
	enum dgMinkReturnCode
	{
		dgMinkError,
		dgMinkDisjoint,
		dgMinkIntersecting
	};

	DG_MSC_VECTOR_ALIGMENT
	class dgMinkFace: public dgPlane
	{
		public:
		dgInt16 m_vertex[4];	
		dgInt16 m_adjancentFace[3];	
		dgInt8	m_inHeap;
		dgInt8	m_isActive;
	}DG_GCC_VECTOR_ALIGMENT;

	class dgMinkFacePurge
	{	
		public:
		dgMinkFacePurge* m_next;
	};

	class SilhouetteFaceCap
	{
		public:
		dgMinkFace* m_face;
		dgInt16* m_faceCopling;
	};


	DG_MSC_VECTOR_ALIGMENT
	struct dgPerimenterEdge
	{
		const dgVector* m_vertex;
		dgPerimenterEdge* m_next;
		dgPerimenterEdge* m_prev;
	} DG_GCC_VECTOR_ALIGMENT;


	class dgClosestFace: public dgDownHeap<dgMinkFace *, dgFloat32>
	{
		public:
		inline dgClosestFace(void* ptr, dgInt32 sizeInBytes)
			:dgDownHeap<dgMinkFace *, dgFloat32> (ptr, sizeInBytes)
		{
		}
	};


	inline dgFloat32 GetShapeClipSize (dgCollision* const collision) const
	{
		return GetMax (collision->GetBoxMaxRadius() * dgFloat32 (4.0f) + dgFloat32 (1.0f), dgFloat32 (32.0f));
//		return GetMax (collision->GetBoxMaxRadius() * dgFloat32 (4.0f) + dgFloat32 (1.0f), dgFloat32 (10000.0f));
	}

	inline dgInt32 CheckTetrahedronVolume () const;
	inline dgInt32 CheckTetrahedronVolumeSimd () const;
	inline dgInt32 CheckTetrahedronVolumeLarge () const;

	void CalcSupportVertex (const dgVector& dir, dgInt32 entry);
	void CalcSupportVertexSimd (const dgVector& dir, dgInt32 entry);
	void CalcSupportVertexLarge (const dgVector& dir, dgInt32 entry);
	bool CheckNormal (dgPerimenterEdge* const polygon, const dgVector& shapeNormal) const;
	dgInt32 CalculateClosestPoints ();
	dgInt32 CalculateConvexShapeIntersectionLine (const dgMatrix& matrix, const dgVector& shapeNormal, dgUnsigned32 id, dgFloat32 penetration, 
			dgInt32 shape1VertexCount, dgVector* const shape1, dgInt32 shape2VertexCount, dgVector* const shape2,dgContactPoint* const contactOut) const;

	dgPerimenterEdge *ReduceContacts (dgPerimenterEdge *poly, dgInt32 maxCount) const;

	dgInt32 CalculateContactAlternateMethod(dgMinkFace* const face, dgInt32 contacID, dgContactPoint* const contactOut, dgInt32 maxContacts);
	dgInt32 CalculateConvexShapeIntersectionSimd (const dgMatrix& matrix, const dgVector& shapeNormal, dgUnsigned32 id, dgFloat32 penetration,
												  dgInt32 shape1VertexCount, dgVector* const shape1, dgInt32 shape2VertexCount, dgVector* const shape2, dgContactPoint* const contactOut, dgInt32 maxContacts) const;
	dgInt32 CalculateConvexShapeIntersection (const dgMatrix& matrix, const dgVector& shapeNormal, dgUnsigned32 id,	dgFloat32 penetration,
											 dgInt32 shape1VertexCount, dgVector* const shape1, dgInt32 shape2VertexCount, dgVector* const shape2, dgContactPoint* const contactOut, dgInt32 maxContacts) const;

	dgInt32 CalculateContactsSimd(dgMinkFace* const face, dgInt32 contacID, dgContactPoint* const contactOut, dgInt32 maxContacts);
	dgInt32 CalculateContacts(dgMinkFace* const face, dgInt32 contacID, dgContactPoint* const contactOut, dgInt32 maxContacts);

	dgInt32 CalculateContactsContinuesSimd(dgInt32 contacID, dgContactPoint* const contactOut, dgInt32 maxContacts, const dgVector* diffPoins, const dgVector* averPoins, dgFloat32 timestep);
	dgInt32 CalculateContactsContinues(dgInt32 contacID, dgContactPoint* const contactOut, dgInt32 maxContacts, const dgVector* diffPoins, const dgVector* averPoins, dgFloat32 timestep);
	dgVector ReduceLine (const dgVector& origin);
	dgBigVector ReduceLineLarge (const dgBigVector& origin);
	dgVector ReduceTriangle (const dgVector& origin);
	dgBigVector ReduceTriangleLarge (const dgBigVector& origin);
	dgVector ReduceTetrahedrum (const dgVector& origin);
	dgBigVector ReduceTetrahedrumLarge (const dgBigVector& origin);
	dgMinkReturnCode UpdateSeparatingPlaneFallbackSolution(dgMinkFace*& plane, const dgVector& origin);
	dgMinkReturnCode UpdateSeparatingPlaneFallbackSolutionLarge(dgMinkFace*& plane, const dgBigVector& origin);
	dgMinkReturnCode UpdateSeparatingPlaneSimd(dgMinkFace*& plane, const dgVector& origin);
	dgMinkReturnCode UpdateSeparatingPlane(dgMinkFace*& plane, const dgVector& origin);
	dgMinkReturnCode UpdateSeparatingPlaneLarge(dgMinkFace*& plane, const dgBigVector& origin);
	dgMinkReturnCode CalcSeparatingPlaneSimd(dgMinkFace*& plane, const dgVector& origin = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (1.0f)));
	dgMinkReturnCode CalcSeparatingPlane(dgMinkFace*& plane, const dgVector& origin = dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (1.0f)));
	dgMinkReturnCode CalcSeparatingPlaneLarge(dgMinkFace*& plane, const dgBigVector& origin = dgBigVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (1.0f)));


//	bool CalcFacePlaneSimd (dgMinkFace *face);
	inline bool CalcFacePlaneSimd (dgMinkFace *face)
	{
		return CalcFacePlane (face);
	}

	bool CalcFacePlane (dgMinkFace* const face);
	bool CalcFacePlaneLarge (dgMinkFace* const face);
	inline dgMinkFace *NewFace();
	dgMinkFace *CalculateClipPlaneSimd ();
	dgMinkFace *CalculateClipPlane ();
	dgMinkFace *CalculateClipPlaneLarge ();


	public:
	dgContactSolver(dgCollisionParamProxy& proxy);
	dgContactSolver(dgCollisionParamProxy& proxy, dgCollision *polygon);
	dgContactSolver& operator= (const dgContactSolver& me);
	
	dgInt32 HullHullContactsSimd (dgInt32 contactID);
	dgInt32 HullHullContacts (dgInt32 contactID);
	dgInt32 HullHullContactsLarge (dgInt32 contactID);
	dgInt32 HullHullContinueContactsSimd (dgFloat32& timeStep, dgContactPoint* const contactOut, dgInt32 contactID, dgInt32 maxContacts, dgInt32 conditionalContactCalculationAtOrigin);
	dgInt32 HullHullContinueContacts (dgFloat32& timeStep, dgContactPoint* const contactOut, dgInt32 contactID, dgInt32 maxContacts, dgInt32 conditionalContactCalculationAtOrigin);
	void CalculateVelocities (dgFloat32 timestep); 
	void CalculateVelocitiesSimd (dgFloat32 timestep) ;

	dgMatrix m_matrix;
	dgVector m_localRelVeloc;
	dgVector m_floatingBodyVeloc;
	dgVector m_referenceBodyVeloc;
	dgVector m_hullVertex[DG_MINK_MAX_POINTS_SIZE * 2];
	dgVector m_averVertex[DG_MINK_MAX_POINTS_SIZE * 2];
	dgMinkFace m_simplex[DG_MINK_MAX_FACES_SIZE * 2];

	dgInt32 m_planeIndex;
	dgInt32 m_vertexIndex;
	dgFloat32 m_penetrationPadding;
	dgBody* m_floatingBody; 
	dgBody* m_referenceBody; 
	dgCollisionConvex* m_floatingcollision;
	dgCollisionConvex* m_referenceCollision;
	dgCollisionParamProxy* m_proxy;
	dgMinkFacePurge *m_facePurge;
	dgBigVector* m_hullVertexLarge;
	dgBigVector* m_averVertexLarge;

	dgMinkReturnCode m_lastFaceCode;

//	static simd_128 m_zero;
//	static simd_128 m_negIndex;
	static dgVector m_dir[14];
	static dgInt32 m_faceIndex[][4];

	friend class dgWorld;
}DG_GCC_VECTOR_ALIGMENT;


inline dgInt32 dgContactSolver::CheckTetrahedronVolume () const
{
	dgVector e0 (m_hullVertex[1] - m_hullVertex[0]);
	dgVector e1 (m_hullVertex[2] - m_hullVertex[0]);
	dgVector e2 (m_hullVertex[3] - m_hullVertex[0]);

	dgFloat32 volume = (e1 * e0) % e2;
	return (volume >= dgFloat32 (0.0f));
}

inline dgInt32 dgContactSolver::CheckTetrahedronVolumeSimd () const
{
	simd_128 e0 ((simd_128&)m_hullVertex[1] - (simd_128&)m_hullVertex[0]);
	simd_128 e1 ((simd_128&)m_hullVertex[2] - (simd_128&)m_hullVertex[0]);
	simd_128 e2 ((simd_128&)m_hullVertex[3] - (simd_128&)m_hullVertex[0]);
	simd_128 volume (e1.CrossProduct(e0).DotProduct(e2));
	return !volume.GetSignMask();
}

inline dgInt32 dgContactSolver::CheckTetrahedronVolumeLarge () const
{
	dgBigVector e0 (m_hullVertexLarge[1] - m_hullVertexLarge[0]);
	dgBigVector e1 (m_hullVertexLarge[2] - m_hullVertexLarge[0]);
	dgBigVector e2 (m_hullVertexLarge[3] - m_hullVertexLarge[0]);

	dgFloat64 volume = (e1 * e0) % e2;
	return (volume >= dgFloat32 (0.0f));
}

inline dgContactSolver::dgMinkFace* dgContactSolver::NewFace()
{
	dgMinkFace *face;
	if (m_facePurge)	{
		face = (dgMinkFace *)m_facePurge;
		m_facePurge = m_facePurge->m_next;
	} else {
		face = &m_simplex[m_planeIndex];
		m_planeIndex ++;
		_ASSERTE (m_planeIndex < sizeof (m_simplex) / sizeof (m_simplex[0]));
	}
	return face ;
}



inline bool dgContactSolver::CalcFacePlaneLarge (dgMinkFace* const face)
{
	dgInt32 i0 = face->m_vertex[0];
	dgInt32 i1 = face->m_vertex[1];
	dgInt32 i2 = face->m_vertex[2];

	//		dgBigPlane &plane = *face;
	dgBigPlane plane (m_hullVertexLarge[i0], m_hullVertexLarge[i1], m_hullVertexLarge[i2]);

	bool ret = false;
	dgFloat64 mag2 = plane % plane;
	//		if (mag2 > DG_DISTANCE_TOLERANCE_ZERO) {
	if (mag2 > dgFloat32 (1.0e-12f)) {
		ret = true;
		plane = plane.Scale (dgFloat64 (1.0f)/ sqrt (mag2));
	} else {
		//_ASSERTE (0);
		plane.m_w = dgFloat64 (0.0f);
	}

	face->m_x = dgFloat32 (plane.m_x);
	face->m_y = dgFloat32 (plane.m_y);
	face->m_z = dgFloat32 (plane.m_z);
	face->m_w = dgFloat32 (plane.m_w);
	face->m_isActive = 1;
	return ret;
}


inline bool dgContactSolver::CalcFacePlane (dgMinkFace* const face)
{
	dgInt32 i0 = face->m_vertex[0];
	dgInt32 i1 = face->m_vertex[1];
	dgInt32 i2 = face->m_vertex[2];

	dgPlane &plane = *face;
	plane = dgPlane (m_hullVertex[i0], m_hullVertex[i1], m_hullVertex[i2]);

	bool ret = false;
	dgFloat32 mag2 = plane % plane;

	//if (mag2 > DG_DISTANCE_TOLERANCE_ZERO) {
	if (mag2 > dgFloat32 (1.0e-12f)) {
		ret = true;
		plane = plane.Scale (dgRsqrt (mag2));
	} else {
		//_ASSERTE (0);
		plane.m_w = dgFloat32 (0.0f);
	}

	face->m_isActive = 1;
	return ret;
}


#endif


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
#include "dgMeshEffect.h"
#include "dgMeshEffectSolidTree.h"

#define DG_CLIPPER_TOL	dgFloat64 (1.0e-12)

/*
dgMeshTreeCSGFace::dgMeshTreeCSGFace (const dgMeshEffect& mesh, dgEdge* const face)
	:dgPolyhedra (mesh.GetAllocator()), m_count (0)
{
	const dgEdge* ptr = face;
	const dgMeshEffect::dgVertexAtribute* const attib = mesh.m_attib;

	dgInt32 indexList[256];
	do {
		indexList[m_count] = m_count;	
		AddPoint (attib[dgInt32 (ptr->m_userData)]);
		ptr = ptr->m_next;
	} while (ptr != face);

	m_baseCount = m_count;

	BeginFace();
	AddFace(m_count, indexList);
	EndFace();

	_ASSERTE (CheckConsistency ());
}


dgInt32 dgMeshTreeCSGFace::AddPoint (const dgMeshEffect::dgVertexAtribute& point)
{
	m_points[m_count] = point;
	m_count ++;
	_ASSERTE (m_count < sizeof (m_points)/sizeof (m_points[0]));
	return (m_count - 1);
}


bool dgMeshTreeCSGFace::CheckConsistency () const
{
	dgInt32 outFaceCount = 0;
	dgInt32 mark = IncLRU();
	dgMeshTreeCSGFace::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		dgEdge* const edge = &(*iter);
		if ((edge->m_incidentFace < 0) && ((edge->m_mark != mark))){
			outFaceCount ++;
			dgEdge* ptr = edge;
			do {
				//dgInt32 i = ptr->m_incidentVertex;
				//dgTrace (("%f %f %f\n", m_points[i].m_x, m_points[i].m_y, m_points[i].m_z));
				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr != edge);
			//dgTrace (("\n"));
		} 
	}
	return (outFaceCount == 1) ? true : false;
}


void dgMeshTreeCSGFace::ClipFace (dgEdge* const face, const dgHugeVector& normal, const dgHugeVector& origin, dgEdge** leftOut, dgEdge** rightOut)
{
	dgEdge* left = NULL;
	dgEdge* right = NULL;
	dgEdge* ptr = face;
	dgEdge* lastEdge = face;
	dgMeshEffect::dgVertexAtribute p0 (m_points[ptr->m_incidentVertex]);
	dgGoogol test0 = normal % (dgHugeVector(p0.m_vertex) - origin);
	if (ptr->m_incidentVertex > m_baseCount) {
		if (fabs (test0.GetAproximateValue()) < dgFloat64 (1.0e-12f)) {
			test0 = dgGoogol(dgFloat64 (0.0f));
		}
	}

	*leftOut = NULL; 
	*rightOut = NULL;	
	do {
		dgMeshEffect::dgVertexAtribute p1 (m_points[ptr->m_next->m_incidentVertex]);
		dgGoogol test1 = normal % (dgHugeVector (p1.m_vertex) - origin);
		if (ptr->m_next->m_incidentVertex > m_baseCount) {
			if (fabs (test1.GetAproximateValue()) < dgFloat64 (1.0e-12f)) {
				test1 = dgGoogol(dgFloat64 (0.0f));
			}
		}

		dgFloat64 val0 = test0.GetAproximateValue();
		dgFloat64 val1 = test1.GetAproximateValue();
		if (val0 < dgFloat64 (0.0f)) {
			if (val1 > dgFloat64 (0.0f)) {
				bool replaceface = false;
				if (lastEdge == ptr) {
					replaceface = true;
				}

				dgHugeVector dp (p1.m_vertex - p0.m_vertex);
				dgGoogol den (normal % dp);
				dgFloat64 t = -test0.GetAproximateValue() / den.GetAproximateValue();
				dgHugeVector q (dgHugeVector(p0.m_vertex) + dp.Scale (t));

				dgMeshEffect::dgVertexAtribute attr;
				attr.m_vertex = dgBigVector (q.m_x.GetAproximateValue(), q.m_y.GetAproximateValue(), q.m_z.GetAproximateValue(), dgFloat64 (0.0f));

				dgBigVector normal0(p0.m_normal_x, p0.m_normal_y, p0.m_normal_z, dgFloat64 (0.0f));
				dgBigVector normal1(p1.m_normal_x, p1.m_normal_y, p1.m_normal_z, dgFloat64 (0.0f));
				dgBigVector n (normal0 + (normal1 - normal0).Scale (t));
				n = n.Scale (dgFloat64 (1.0f) / sqrt (n % n)); 

				attr.m_normal_x = n.m_x;
				attr.m_normal_y = n.m_y;
				attr.m_normal_z = n.m_z; 

				attr.m_u0 = p0.m_u0 + (p1.m_u0 - p0.m_u0) * t;
				attr.m_v0 = p0.m_v0 + (p1.m_v0 - p0.m_v0) * t;

				attr.m_u1 = p0.m_u1 + (p1.m_u1 - p0.m_u1) * t;
				attr.m_v1 = p0.m_v1 + (p1.m_v1 - p0.m_v1) * t;

				attr.m_material = p0.m_material;

				ptr = SpliteEdge (m_count, ptr);
				if (replaceface) {
					lastEdge = ptr;
				}

				AddPoint (attr);
				ptr = ptr->m_next;
				right = ptr;

			} else if (val1 < dgFloat64 (0.0f)) {
				if (!left) {
					left = ptr;
				}
				//			} else {
				//				_ASSERTE (0);
			}

		} else if (val0 > dgFloat64 (0.0f)) {
			if (val1 > dgFloat64 (0.0f)) {
				if (!right) {
					right = ptr;
				}
			} else if (val1 < dgFloat64 (0.0f)) {
				bool replaceface = false;
				if (lastEdge == ptr) {
					replaceface = true;
				}

				dgHugeVector dp (p1.m_vertex - p0.m_vertex);
				dgGoogol den (normal % dp);
				dgFloat64 t = -test0.GetAproximateValue() / den.GetAproximateValue();
				dgHugeVector q (dgHugeVector(p0.m_vertex) + dp.Scale (t));

				dgMeshEffect::dgVertexAtribute attr;
				attr.m_vertex = dgBigVector (q.m_x.GetAproximateValue(), q.m_y.GetAproximateValue(), q.m_z.GetAproximateValue(), dgFloat64 (0.0f));

				dgBigVector normal0(p0.m_normal_x, p0.m_normal_y, p0.m_normal_z, dgFloat64 (0.0f));
				dgBigVector normal1(p1.m_normal_x, p1.m_normal_y, p1.m_normal_z, dgFloat64 (0.0f));
				dgBigVector n (normal0 + (normal1 - normal0).Scale (t));
				n = n.Scale (dgFloat64 (1.0f) / sqrt (n % n)); 

				attr.m_normal_x = n.m_x;
				attr.m_normal_y = n.m_y;
				attr.m_normal_z = n.m_z; 

				attr.m_u0 = p0.m_u0 + (p1.m_u0 - p0.m_u0) * t;
				attr.m_v0 = p0.m_v0 + (p1.m_v0 - p0.m_v0) * t;

				attr.m_u1 = p0.m_u1 + (p1.m_u1 - p0.m_u1) * t;
				attr.m_v1 = p0.m_v1 + (p1.m_v1 - p0.m_v1) * t;

				attr.m_material = p0.m_material;

				ptr = SpliteEdge (m_count, ptr);
				if (replaceface) {
					lastEdge = ptr;
				}

				AddPoint (attr);
				ptr = ptr->m_next;
				left = ptr;

				//			} else {
				//				_ASSERTE (0);
			}

		} else {
			if (val1 > dgFloat64 (0.0f)) {
				right = ptr;
			} else if (val1 < dgFloat64 (0.0f)) {
				left = ptr;
			}
		}

		test0 = test1;
		p0 = p1;

		ptr = ptr->m_next;
	} while (ptr != lastEdge);



	_ASSERTE (left || right);
	if (left && right) {
		*leftOut = left; 
		*rightOut = right;

		dgEdge* const edge = AddHalfEdge (left->m_incidentVertex, right->m_incidentVertex);
		dgEdge* const twin = AddHalfEdge (right->m_incidentVertex, left->m_incidentVertex);
		_ASSERTE (edge);
		_ASSERTE (twin);

		edge->m_incidentFace = right->m_incidentFace;
		twin->m_incidentFace = left->m_incidentFace;

		edge->m_twin = twin;
		twin->m_twin = edge;

		edge->m_prev = left->m_prev;
		left->m_prev->m_next = edge;

		twin->m_prev = right->m_prev;
		right->m_prev->m_next = twin;

		edge->m_next = right;
		right->m_prev = edge;

		twin->m_next = left;
		left->m_prev = twin;

	} else if (left) {
		_ASSERTE (!right);
		*leftOut = left;
	} else {
		_ASSERTE (right);
		*rightOut = right;
	}
}
*/



dgMeshTreeCSGFace::dgMeshTreeCSGFace (dgMemoryAllocator* const allocator, const dgMeshEffect& mesh, dgEdge* const face)
	:dgList<dgCSGFacePoint>(allocator), m_side(dgMeshEffectSolidTree::m_divider)
{
	const dgEdge* ptr = face;
	const dgMeshEffect::dgVertexAtribute* const attib = mesh.m_attib;
	do {
		Append (attib[ptr->m_userData]);
		ptr = ptr->m_next;
	} while (ptr != face);
}


dgMeshTreeCSGFace::dgMeshTreeCSGFace (dgMemoryAllocator* const allocator, dgInt32 count, const dgCSGFacePoint* const points)
	:dgList<dgCSGFacePoint>(allocator), m_side(dgMeshEffectSolidTree::m_divider)
{
	for (dgInt32 i = 0; i < count; i ++) {
		Append (points[i]);
	}
}


bool dgMeshTreeCSGFace::IsPointOnEdge (const dgBigVector& p0, const dgBigVector& p1, const dgBigVector& q0) const
{
	dgBigVector p1p0 (p1 - p0);
	dgFloat64 den = p1p0 % p1p0;

	dgBigVector q0p0 (q0 - p0);
	dgFloat64 num = q0p0 % p1p0;

	if (num > (DG_CLIPPER_TOL * dgFloat64 (1.0e3))) {
		if (num < den * dgFloat64 (1.0 - (DG_CLIPPER_TOL* dgFloat64 (1.0e3)))) {
			dgFloat64 t = num / den;
			_ASSERTE (t > dgFloat64 (0.0f));
			_ASSERTE (t < dgFloat64 (1.0f));
			dgBigVector q1 (p0 + p1p0.Scale (t));
			dgBigVector dist (q1 - q0);
			dgFloat64 dist2 = dist % dist;		
			if (dist2 < DG_CLIPPER_TOL) {
				return true;
			}
		}
	}
	return false;
}


void dgMeshTreeCSGFace::MergeMissingVertex (const dgMeshTreeCSGFace* const face)
{
	for (dgMeshTreeCSGFace::dgListNode* outNode = GetFirst(); outNode != GetLast(); outNode = outNode->GetNext()) {
		dgBigVector p0 (outNode->GetInfo().GetPoint().m_vertex);
		for (dgMeshTreeCSGFace::dgListNode* node = face->GetFirst(); node; node = node->GetNext()) {
			if (IsPointOnEdge (p0, outNode->GetNext()->GetInfo().GetPoint().m_vertex, node->GetInfo().GetPoint().m_vertex)) {
				dgMeshTreeCSGFace::dgListNode* const insertNode = Append(node->GetInfo());
				InsertAfter(outNode, insertNode);
			}
		}
	}

	RotateToBegin (GetLast());
	dgListNode* const last = GetFirst()->GetNext();
	for (dgMeshTreeCSGFace::dgListNode* outNode = GetFirst(); outNode != last; outNode = outNode->GetNext()) {	
		dgBigVector p0 (outNode->GetInfo().GetPoint().m_vertex);
		dgBigVector p1 (outNode->GetNext()->GetInfo().GetPoint().m_vertex);

		for (dgMeshTreeCSGFace::dgListNode* node = face->GetFirst(); node; node = node->GetNext()) {
			if (IsPointOnEdge (p0, outNode->GetNext()->GetInfo().GetPoint().m_vertex, node->GetInfo().GetPoint().m_vertex)) {
				dgMeshTreeCSGFace::dgListNode* const insertNode = Append(node->GetInfo());
				InsertAfter(outNode, insertNode);
			}
		}
	}
}

dgInt32 dgMeshTreeCSGFace::RemoveDulicates (dgInt32 count, dgCSGFacePoint* const points) const
{
	dgInt32 index[256];
	for (dgInt32 i = 0;  i < count; i ++) {
		index[i] = i + 1;
	}
	index[count - 1] = 0;

	dgInt32 originalCount = count; 
	dgInt32 start = index[0];
	for (dgInt32 count1 = 0; (count1 != count) && (count >= 3); ) {
		count1 = 0;
		for (dgInt32 i = 0; i < count; i ++) {
			dgInt32 next = index[start];
			dgHugeVector err (points[start].m_vertex - points[next].m_vertex);
			dgGoogol dist2 = err % err;
			dgFloat64 val = dist2.GetAproximateValue();
			if (val < dgFloat64 (1.0e-12f)) {
				index[start] = index[index[start]];
				count1 = 0;
				count --;
				break;
			}

			count1 ++;
			start = index[start];
		} 
	}

	if ((count != originalCount) && (count >= 3)) {
		dgCSGFacePoint tmp[256];
		for (dgInt32 i = 0; i < count; i ++) {
			tmp[i] = points[start];
			start = index[start];
		} 

		for (dgInt32 i = 0; i < count; i ++) {
			points[i] = tmp[i];
		}
	}

	return count;
}

#ifdef _DEBUG
dgMatrix dgMeshTreeCSGFace::DebugMatrix () const
{
	dgMatrix matrix (dgGetIdentityMatrix());
	dgHugeVector e0 (GetFirst()->GetNext()->GetInfo().m_vertex - GetFirst()->GetInfo().m_vertex);
	dgHugeVector e1 (GetFirst()->GetNext()->GetNext()->GetInfo().m_vertex - GetFirst()->GetInfo().m_vertex);
	dgHugeVector aaa (e0 * e1);
	dgVector aaaa (dgFloat32 (aaa.m_x.GetAproximateValue()), dgFloat32 (aaa.m_y.GetAproximateValue()), dgFloat32 (aaa.m_z.GetAproximateValue()), 0.0f);
	aaaa = aaaa.Scale (dgFloat32 (1.0)/ dgSqrt (aaaa % aaaa));

	dgVector bbbb (dgFloat32 (e0.m_x.GetAproximateValue()), dgFloat32 (e0.m_y.GetAproximateValue()), dgFloat32 (e0.m_z.GetAproximateValue()), 0.0f);
	bbbb = bbbb.Scale (dgFloat32 (1.0)/ dgSqrt (bbbb % bbbb));

	matrix.m_up = bbbb;
	matrix.m_right = aaaa;
	matrix.m_front = bbbb * aaaa;

	return matrix;
}

void dgMeshTreeCSGFace::Trace (const dgMatrix& matrix) const
{
	for (dgMeshTreeCSGFace::dgListNode* node = GetFirst(); node; node = node->GetNext()) {
		const dgHugeVector& bbb = node->GetInfo().m_vertex;
		dgVector bbbb (dgFloat32 (bbb.m_x.GetAproximateValue()), dgFloat32 (bbb.m_y.GetAproximateValue()), dgFloat32 (bbb.m_z.GetAproximateValue()), 0.0f);
		bbbb = matrix.UnrotateVector(bbbb);
		bbbb.Trace();
	}
	dgTrace (("\n"));
}

#endif

bool dgMeshTreeCSGFace::CheckFaceArea (dgInt32 count, const dgCSGFacePoint* const points) const
{
	dgHugeVector area (0.0f, 0.0f, 0.0f, 0.0f);
	dgHugeVector e0 (points[1].m_vertex - points[0].m_vertex);
	for (dgInt32 i = 2; i < count; i ++) {
		dgHugeVector e1 (points[i].m_vertex - points[0].m_vertex);
		area += e0 * e1;
		e0 = e1;
	}

	dgBigVector a (area.m_x.GetAproximateValue(), area.m_y.GetAproximateValue(), area.m_z.GetAproximateValue(), 0.0);
	dgFloat64 val = a % a;
	return (val > dgFloat64 (1.0e-12f)) ? true : false;
}


dgCSGFacePoint dgMeshTreeCSGFace::Interpolate (const dgHugeVector& plane, const dgCSGFacePoint& p0, const dgCSGFacePoint& p1) const
{
	dgCSGFacePoint attr;

	dgHugeVector dp (p1.m_vertex - p0.m_vertex);
	dgGoogol den (plane % dp);
	dgGoogol num = plane.EvaluePlane (p0.m_vertex);
	_ASSERTE (fabs (num.GetAproximateValue()) > dgFloat64 (0.0f));


	dgGoogol ti (num / den);

	attr.m_vertex  = p0.m_vertex - dp.Scale (ti);

	dgFloat64 t = -ti.GetAproximateValue();
	dgBigVector normal0(p0.m_normal_x, p0.m_normal_y, p0.m_normal_z, dgFloat64 (0.0f));
	dgBigVector normal1(p1.m_normal_x, p1.m_normal_y, p1.m_normal_z, dgFloat64 (0.0f));
	dgBigVector n (normal0 + (normal1 - normal0).Scale (t));
	n = n.Scale (dgFloat64 (1.0f) / sqrt (n % n)); 

	attr.m_normal_x = n.m_x;
	attr.m_normal_y = n.m_y;
	attr.m_normal_z = n.m_z; 

	attr.m_u0 = p0.m_u0 + (p1.m_u0 - p0.m_u0) * t;
	attr.m_v0 = p0.m_v0 + (p1.m_v0 - p0.m_v0) * t;

	attr.m_u1 = p0.m_u1 + (p1.m_u1 - p0.m_u1) * t;
	attr.m_v1 = p0.m_v1 + (p1.m_v1 - p0.m_v1) * t;

	attr.m_material = p0.m_material;

	return attr;
}


void dgMeshTreeCSGFace::Clip (const dgHugeVector& plane, dgMeshTreeCSGFace** leftOut, dgMeshTreeCSGFace** rightOut)
{
	dgInt8 pointSide[256];

	dgInt32 count = 0;
	dgInt32 rightCount = 0;
	dgInt32 leftCount = 0;
	for (dgMeshTreeCSGFace::dgListNode* ptr = GetFirst(); ptr; ptr = ptr->GetNext()) {
		const dgCSGFacePoint& p = ptr->GetInfo();

		dgGoogol test = plane.EvaluePlane(p.m_vertex);
		dgFloat64 val = test.GetAproximateValue();
		if (fabs (val) < DG_CLIPPER_TOL) {
			val = dgFloat64 (0.0f);
		}

		if (val > dgFloat64 (0.0f)) {
			pointSide[count] = 1;
			rightCount ++;
		} else if (val < dgFloat64 (0.0f)) {
			pointSide[count] = -1;
			leftCount++;
		} else {
			pointSide[count] = 0;
		}
		count ++;
	}


	*leftOut = NULL;
	*rightOut = NULL;
	if ((leftCount && !rightCount) || (!leftCount && rightCount)) {
		if (leftCount) {
			_ASSERTE (!rightCount);
			AddRef();
			*leftOut = this;
		} else {
			_ASSERTE (!leftCount);
			*rightOut = this;
			AddRef();
		}
	} else if (!(leftCount && leftCount)) {
		_ASSERTE (!leftCount);
		_ASSERTE (!rightCount);
		AddRef();
		AddRef();
	} else {
		dgInt32 leftCount = 0;
		dgInt32 rightCount = 0;
		dgCSGFacePoint leftFace[256];
		dgCSGFacePoint rightFace[256];

		dgInt32 i1 = 0;
		dgInt32 i0 = count - 1;
		dgCSGFacePoint p0 (GetLast()->GetInfo());
		for (dgMeshTreeCSGFace::dgListNode* ptr = GetFirst(); ptr; ptr = ptr->GetNext()) {
			const dgCSGFacePoint& p1 (ptr->GetInfo());

			dgCSGFacePoint inter; 
			if (((pointSide[i0] == -1) && (pointSide[i1] == 1)) || ((pointSide[i0] == 1) && (pointSide[i1] == -1))) {
				inter = Interpolate (plane, p0, p1);
			}

			if (pointSide[i1] == -1)	{
				if (pointSide[i0] == 1)	{
					rightFace[rightCount] = inter;
					rightCount ++;
				}
			} else {
				if ((pointSide[i1] == 1) && (pointSide[i0] == -1)) {
					rightFace[rightCount] = inter;
					rightCount ++;
				}
				rightFace[rightCount] = p1;
				rightCount ++;
			}


			if (pointSide[i1] == 1)	{
				if (pointSide[i0] == -1)	{
					leftFace[leftCount] = inter;
					leftCount ++;
				}
			} else {
				if ((pointSide[i1] == -1) && (pointSide[i0] == 1)) {
					leftFace[leftCount] = inter;
					leftCount ++;
				}
				leftFace[leftCount] = p1;
				leftCount ++;
			}
			_ASSERTE (leftCount < (sizeof (leftFace)/sizeof (leftFace[0]) - 1));
			_ASSERTE (rightCount < (sizeof (rightFace)/sizeof (rightFace[0]) - 1));

			i0 = i1;
			i1 ++;
			p0 = p1;
		}

		leftCount = RemoveDulicates (leftCount, leftFace);
		rightCount = RemoveDulicates (rightCount, rightFace);
		if ((leftCount >= 3) && CheckFaceArea (leftCount, leftFace)) {
			*leftOut = new (GetAllocator()) dgMeshTreeCSGFace (GetAllocator(), leftCount, leftFace);
		}

		if ((rightCount >= 3) && CheckFaceArea (rightCount, rightFace)) {
			*rightOut = new (GetAllocator()) dgMeshTreeCSGFace (GetAllocator(), rightCount, rightFace);
		}
		_ASSERTE (*leftOut || *rightOut);
	}
}





dgMeshEffectSolidTree::CSGConvexCurve::CSGConvexCurve (dgMemoryAllocator* const allocator)
	:dgList<dgHugeVector>(allocator)
{
}

dgMeshEffectSolidTree::CSGConvexCurve::CSGConvexCurve (const dgMeshEffect& mesh, dgEdge* const face)
	:dgList<dgHugeVector>(mesh.GetAllocator())
{
	dgEdge* ptr = face;
	do {
		Append(mesh.m_points[ptr->m_incidentVertex]);
		ptr = ptr->m_next;
	} while (ptr != face);
}

bool dgMeshEffectSolidTree::CSGConvexCurve::CheckConvex(const dgHugeVector& normal) const
{
	dgHugeVector p1 (GetLast()->GetInfo());
	dgHugeVector p0 (GetLast()->GetPrev()->GetInfo());
	dgHugeVector e0 (p0 - p1);
	for (CSGConvexCurve::dgListNode* node = GetFirst(); node; node = node->GetNext()) {
		dgHugeVector p2 (node->GetInfo());
		dgHugeVector e1 (p2 - p1);

		dgHugeVector n (e1 * e0);
		dgGoogol convex = n % normal;
		if (convex.GetAproximateValue() < dgFloat64 (-1.0e10f)) {
			return false;
		}
		p1 = p2;
		e0 = e1.Scale (dgGoogol(-1.0f));
	}
	return true;
};




dgMeshEffectSolidTree::dgMeshEffectSolidTree (dgPlaneType type)
	:m_planeType(type)
	,m_front(NULL)
	,m_back(NULL)
	,m_plane(0.0, 0.0, 0.0, 0.0)
{
}

dgMeshEffectSolidTree::dgMeshEffectSolidTree (const dgMeshEffect& mesh, dgEdge* const face)
	:m_planeType(m_divider)
	,m_back (new (mesh.GetAllocator()) dgMeshEffectSolidTree (m_solid))
	,m_front (new (mesh.GetAllocator()) dgMeshEffectSolidTree (m_empty))
	,m_plane(BuildPlane (mesh, face))
{
}

dgMeshEffectSolidTree::dgMeshEffectSolidTree (const dgHugeVector& plane, dgMemoryAllocator* const allocator)
	:m_planeType(m_divider)
	,m_back (new (allocator) dgMeshEffectSolidTree (m_solid))
	,m_front (new (allocator) dgMeshEffectSolidTree (m_empty))
	,m_plane (plane) 
{
}


dgMeshEffectSolidTree::~dgMeshEffectSolidTree()
{
	if (m_front) {
		delete m_front; 
	}

	if (m_back)	{
		delete m_back;
	}
}



dgHugeVector dgMeshEffectSolidTree::BuildPlane (const dgMeshEffect& mesh, dgEdge* const face) const
{
	dgEdge* edge = face;
	dgHugeVector plane (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));

	dgHugeVector p0 (mesh.m_points[edge->m_incidentVertex]);

	edge = edge->m_next;
	dgHugeVector p1 (mesh.m_points[edge->m_incidentVertex]);

	dgHugeVector e1 (p1 - p0);
	for (edge = edge->m_next; edge != face; edge = edge->m_next) {
		dgHugeVector p2 (mesh.m_points[edge->m_incidentVertex]);
		dgHugeVector e2 (p2 - p0);
		plane += e1 * e2;
		e1 = e2;
	} 

	plane.m_w = (p0 % plane) * dgGoogol (-1.0);
	return plane;
}






void dgMeshEffectSolidTree::AddFace (const dgMeshEffect& mesh, dgEdge* const face)
{
	dgBigVector normal (mesh.FaceNormal (face, &mesh.m_points[0][0], sizeof (dgBigVector)));
	dgFloat64 mag2 = normal % normal;

	if (mag2 > dgFloat32 (1.0e-14f)) {
		CSGConvexCurve* faces[DG_MESH_EFFECT_BOLLEAN_STACK];
		dgMeshEffectSolidTree* pool[DG_MESH_EFFECT_BOLLEAN_STACK];

		dgHugeVector plane (BuildPlane (mesh, face));

		dgInt32 stack = 1;
		pool[0] = this;
		faces[0] = new (mesh.GetAllocator()) CSGConvexCurve (mesh, face);

		while (stack) {
			stack --;
			dgMeshEffectSolidTree* const root = pool[stack];
			_ASSERTE (root->m_planeType == m_divider);

			CSGConvexCurve* const curve = faces[stack];
			_ASSERTE (curve->CheckConvex(plane));

			dgGoogol minDist (0.0);
			dgGoogol maxDist (0.0);
			for (CSGConvexCurve::dgListNode* node = curve->GetFirst(); node; node = node->GetNext()) {
				dgGoogol dist = root->m_plane.EvaluePlane(node->GetInfo());
				dgGoogol error = dist - minDist;
				if (error.GetAproximateValue() < dgFloat64 (0.0f)) {
					minDist = dist;
				}
				error = dist - maxDist;
				if (error.GetAproximateValue() > dgFloat64 (0.0f)) {
					maxDist = dist;
				} 
			} 

			dgFloat64 minVal = minDist.GetAproximateValue();
			if (minVal > dgFloat64 (-1.0e-2f)) {
				if (minVal != dgFloat64 (0.0f)) {
					dgFloat64 invMag = sqrt ((root->m_plane % root->m_plane).GetAproximateValue());
					dgFloat64 dist = minVal / invMag;
					if (dist > dgFloat64 (-1.0e-10f)) {
						minDist = dgGoogol (dgFloat64 (0.0f));
						minVal = dgFloat64 (0.0f);
					}
				}
			}


			dgFloat64 maxVal = maxDist.GetAproximateValue();
			if (maxVal < dgFloat64 (1.0e-2f)) {
				if (maxVal != dgFloat64 (0.0f)) {
					dgFloat64 invMag = sqrt ((root->m_plane % root->m_plane).GetAproximateValue());
					dgFloat64 dist = maxVal / invMag;
					if (dist < dgFloat64 (1.0e-10f)) {
						maxDist = dgGoogol (dgFloat64 (0.0f));
						maxVal = dgFloat64 (0.0f);
					}
				}
			}


			if ((minVal < dgFloat64 (0.0f)) && (maxVal > dgFloat64 (0.0f))) {

				_ASSERTE (0);
				if (root->m_back->m_planeType != m_divider) {
					delete root->m_back;
					root->m_back = new (mesh.GetAllocator()) dgMeshEffectSolidTree (plane, mesh.GetAllocator());
				} else {

					dgHugeVector p0 (curve->GetLast()->GetInfo());
					CSGConvexCurve* const backFace = new (mesh.GetAllocator()) CSGConvexCurve (mesh.GetAllocator());

					dgGoogol test0 (root->m_plane.EvaluePlane(p0));
					for (CSGConvexCurve::dgListNode* node = curve->GetFirst(); node; node = node->GetNext()) {

						const dgHugeVector& p1 = node->GetInfo();
						dgGoogol test1 (root->m_plane.EvaluePlane(p1));
						if (test0.GetAproximateValue() <= dgFloat64 (0.0f)) {

							backFace->Append(p0);
							if (test1.GetAproximateValue() > dgFloat64 (0.0f)) {
								dgHugeVector dp (p1 - p0);
								dgGoogol den (root->m_plane % dp);
								dgHugeVector p (p0 - dp.Scale (test0 / den));
								backFace->Append(p);
							}

						} else if (test1.GetAproximateValue() < dgFloat64 (0.0f)) {
							dgHugeVector dp (p1 - p0);
							dgGoogol den (root->m_plane % dp);
							dgHugeVector p (p0 - dp.Scale (test0 / den));
							backFace->Append(p);
						}

						test0 = test1;
						p0 = p1;
					}

					// check Here because the clipper can generate a point and lines
					//_ASSERTE (!backFace->GetCount() || (backFace->GetCount() >= 3));
					_ASSERTE (backFace->GetCount() >= 3);

					faces[stack] = backFace;
					pool[stack] = root->m_back;
					stack ++;
					_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
				}

				if (root->m_front->m_planeType != m_divider) {
					delete root->m_front;
					root->m_front = new (mesh.GetAllocator())dgMeshEffectSolidTree (plane, mesh.GetAllocator());
				} else {
					dgHugeVector p0 (curve->GetLast()->GetInfo());
					CSGConvexCurve* const frontFace = new (mesh.GetAllocator()) CSGConvexCurve (mesh.GetAllocator());


					dgGoogol test0 (root->m_plane.EvaluePlane(p0));
					for (CSGConvexCurve::dgListNode* node = curve->GetFirst(); node; node = node->GetNext()) {

						const dgHugeVector& p1 = node->GetInfo();
						dgGoogol test1 (root->m_plane.EvaluePlane(p1));
						if (test0.GetAproximateValue() >= dgFloat64 (0.0f)) {

							frontFace->Append(p0);
							if (test1.GetAproximateValue() < dgFloat32 (0.0f)) {
								dgHugeVector dp (p1 - p0);
								dgGoogol den (root->m_plane % dp);
								dgHugeVector p (p0 - dp.Scale (test0 / den));
								frontFace->Append(p);
							}
						} else if (test1.GetAproximateValue() > dgFloat64 (0.0f)) {
							dgHugeVector dp (p1 - p0);
							dgGoogol den (root->m_plane % dp);
							dgHugeVector p (p0 - dp.Scale (test0 / den));
							frontFace->Append(p);
						}

						test0 = test1;
						p0 = p1;
					}

					// check Here because the clipper can generate a point and lines
					//_ASSERTE (!frontFace->GetCount() || (frontFace->GetCount() >= 3));
					_ASSERTE (frontFace->GetCount() >= 3);

					faces[stack] = frontFace;
					pool[stack] = root->m_front;
					stack ++;
					_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
				}

			} else {

				if (minVal < dgFloat64 (0.0f)) {
					_ASSERTE (maxVal <= 0.0f);
					if (root->m_back->m_planeType != m_divider) {
						delete root->m_back;
						root->m_back = new (mesh.GetAllocator()) dgMeshEffectSolidTree (plane, mesh.GetAllocator());
					} else {
						curve->AddRef();
						faces[stack] = curve;
						pool[stack] = root->m_back;
						stack ++;
						_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
					}
				} else if (maxVal > dgFloat64 (0.0f)) {
					_ASSERTE (minVal >= 0.0f);
					if (root->m_front->m_planeType != m_divider) {
						delete root->m_front;
						root->m_front = new (mesh.GetAllocator())dgMeshEffectSolidTree (plane, mesh.GetAllocator());
					} else {
						curve->AddRef();
						faces[stack] = curve;
						pool[stack] = root->m_front;
						stack ++;
						_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
					}

				} else {
					_ASSERTE (minVal == dgFloat64 (0.0f));
					_ASSERTE (maxVal == dgFloat64 (0.0f));
					if (root->m_front->m_planeType == m_divider) {
						curve->AddRef();
						faces[stack] = curve;
						pool[stack] = root->m_front;
						stack ++;
						_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
					}

					if (root->m_back->m_planeType == m_divider) {
						curve->AddRef();
						faces[stack] = curve;
						pool[stack] = root->m_back;
						stack ++;
						_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
					}
				}
			}

			curve->Release();
		}
	}
}

dgMeshEffectSolidTree::dgPlaneType dgMeshEffectSolidTree::GetPointSide (const dgHugeVector& point) const
{
	const dgMeshEffectSolidTree* root = this;

	_ASSERTE (root);
	while (root->m_planeType == dgMeshEffectSolidTree::m_divider) { 
		dgGoogol test = root->m_plane.EvaluePlane(point);
		dgFloat64 dist = test.GetAproximateValue();

		if (fabs (dist) < dgFloat64 (1.0e-16f)) {
			dgPlaneType isBackSide = root->m_back->GetPointSide (point);
			dgPlaneType isFrontSide = root->m_front->GetPointSide (point);
			return (isBackSide == isFrontSide) ? isFrontSide : m_divider;

		} else if (dist > dgFloat64 (0.0f)) {
			root = root->m_front;
		} else {
			_ASSERTE (dist < dgFloat64 (0.0f));
			root = root->m_back;
		}
	}
	return root->m_planeType;
}


dgMeshEffectSolidTree::dgPlaneType dgMeshEffectSolidTree::GetFaceSide (const dgMeshTreeCSGFace* const face) const
{
	dgHugeVector center (0.0, 0.0, 0.0, 0.0);
	for (dgMeshTreeCSGFace::dgListNode* node = face->GetFirst(); node; node = node->GetNext()) {
		const dgHugeVector& point = node->GetInfo().m_vertex;
		center += point;
	}
	center = center.Scale (dgGoogol (1.0) / dgGoogol (face->GetCount())) ;
	dgPlaneType faceSide (GetPointSide (center));
	return faceSide;
}

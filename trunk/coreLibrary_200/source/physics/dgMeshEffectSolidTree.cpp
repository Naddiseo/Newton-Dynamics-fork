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

dgMeshEffectSolidTree::CSGConvexCurve::CSGConvexCurve (dgMemoryAllocator* const allocator)
	:dgList<dgHugeVector>(allocator)
{
}

dgMeshEffectSolidTree::CSGConvexCurve::CSGConvexCurve (const dgMeshEffect& mesh, dgEdge* const face)
	:dgList<dgHugeVector>(mesh.GetAllocator())
{
	dgEdge* ptr = face;
	do {
//dgTrace (("%f %f %f\n", mesh.m_points[ptr->m_incidentVertex].m_x, mesh.m_points[ptr->m_incidentVertex].m_y, mesh.m_points[ptr->m_incidentVertex].m_z));
		Append(mesh.m_points[ptr->m_incidentVertex]);
		ptr = ptr->m_next;
	} while (ptr != face);
//dgTrace (("\n"));
}

bool dgMeshEffectSolidTree::CSGConvexCurve::CheckConvex(const dgHugeVector& normal, const dgHugeVector& point) const
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


#ifdef _DEBUG
dgInt32 dgMeshEffectSolidTree::m_enumerator;
#endif


dgMeshEffectSolidTree::dgMeshEffectSolidTree (dgPlaneType type)
	:m_planeType(type), m_front(NULL), m_back(NULL)
{
	#ifdef _DEBUG
		m_id = m_enumerator;
	#endif
}

dgMeshEffectSolidTree::dgMeshEffectSolidTree (const dgMeshEffect& mesh, dgEdge* const face)
	:m_planeType(m_divider)
	,m_back (new (mesh.GetAllocator()) dgMeshEffectSolidTree (m_solid))
	,m_front (new (mesh.GetAllocator()) dgMeshEffectSolidTree (m_empty))
	,m_origin(0.0f, 0.0f, 0.0f, 0.0f)
	,m_normal(0.0f, 0.0f, 0.0f, 0.0f)
{
#ifdef _DEBUG
	m_id = m_enumerator;
#endif

	BuildPlane (mesh, face, m_normal, m_origin);
}

dgMeshEffectSolidTree::dgMeshEffectSolidTree (const dgHugeVector& normal, const dgHugeVector& origin, dgMemoryAllocator* const allocator)
	:m_planeType(m_divider)
	,m_back (new (allocator) dgMeshEffectSolidTree (m_solid))
	,m_front (new (allocator) dgMeshEffectSolidTree (m_empty))
	,m_origin (origin) 
	,m_normal (normal)
{
#ifdef _DEBUG
	m_id = m_enumerator;
#endif
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



void dgMeshEffectSolidTree::BuildPlane (const dgMeshEffect& mesh, dgEdge* const face, dgHugeVector& normal, dgHugeVector& point) const
{
	point = dgHugeVector(mesh.m_points[face->m_incidentVertex]);

	dgEdge* edge = face;
	normal = dgHugeVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));

	dgHugeVector p0 (mesh.m_points[edge->m_incidentVertex]);
	edge = edge->m_next;

	dgHugeVector p1 (mesh.m_points[edge->m_incidentVertex]);
	dgHugeVector e1 (p1 - p0);
	for (edge = edge->m_next; edge != face; edge = edge->m_next) {
		dgHugeVector p2 (mesh.m_points[edge->m_incidentVertex]);
		dgHugeVector e2 (p2 - p0);
		normal += e1 * e2;
		e1 = e2;
	} 
}


void dgMeshEffectSolidTree::AddFace (const dgMeshEffect& mesh, dgEdge* const face)
{
	dgBigVector normal (mesh.FaceNormal (face, &mesh.m_points[0][0], sizeof (dgBigVector)));
	dgFloat64 mag2 = normal % normal;

	if (mag2 > dgFloat32 (1.0e-14f)) {

		CSGConvexCurve* faces[DG_MESH_EFFECT_BOLLEAN_STACK];
		dgMeshEffectSolidTree* pool[DG_MESH_EFFECT_BOLLEAN_STACK];

		dgHugeVector point;
		dgHugeVector normal;
		BuildPlane (mesh, face, normal, point);

		dgInt32 stack = 1;
		pool[0] = this;
		faces[0] = new (mesh.GetAllocator()) CSGConvexCurve (mesh, face);

		while (stack) {
			stack --;
			dgMeshEffectSolidTree* const root = pool[stack];
			_ASSERTE (root->m_planeType == m_divider);

			CSGConvexCurve* const curve = faces[stack];
			_ASSERTE (curve->CheckConvex(normal, point));

			dgGoogol minDist (dgFloat64 (0.0f));
			dgGoogol maxDist (dgFloat64 (0.0f));

			for (CSGConvexCurve::dgListNode* node = curve->GetFirst(); node; node = node->GetNext()) {
				dgGoogol dist = root->m_normal % (node->GetInfo() - root->m_origin);
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
					dgFloat64 t = minVal / (root->m_normal % root->m_normal).GetAproximateValue();
					dgHugeVector dist(root->m_normal.Scale (dgGoogol(t)));
					dgFloat64 dist2 = (dist % dist).GetAproximateValue();
					if (dist2 < DG_CLIPPER_TOL) {
						minDist = dgGoogol (dgFloat64 (0.0f));
						minVal = dgFloat64 (0.0f);
					}
				}
			}

			dgFloat64 maxVal = maxDist.GetAproximateValue();
			if (maxVal < dgFloat64 (1.0e-2f)) {
				if (maxVal != dgFloat64 (0.0f)) {
					dgFloat64 t = maxVal / (root->m_normal % root->m_normal).GetAproximateValue();
					dgHugeVector dist(root->m_normal.Scale (dgGoogol(t)));
					dgFloat64 dist2 = (dist % dist).GetAproximateValue();
					if (dist2 < DG_CLIPPER_TOL) {
						maxDist = dgGoogol (dgFloat64 (0.0f));
						maxVal = dgFloat64 (0.0f);
					}
				}
			}


			if ((minVal < dgFloat64 (0.0f)) && (maxVal > dgFloat64 (0.0f))) {
				if (root->m_back->m_planeType != m_divider) {
					delete root->m_back;
					root->m_back = new (mesh.GetAllocator()) dgMeshEffectSolidTree (normal, point, mesh.GetAllocator());
				} else {
	
					dgHugeVector p0 (curve->GetLast()->GetInfo());
					CSGConvexCurve* const backFace = new (mesh.GetAllocator()) CSGConvexCurve (mesh.GetAllocator());

					//backFace.SetAllocator(mesh.GetAllocator());
					dgGoogol test0 (root->m_normal % (p0 - root->m_origin));
					for (CSGConvexCurve::dgListNode* node = curve->GetFirst(); node; node = node->GetNext()) {

						const dgHugeVector& p1 = node->GetInfo();
						dgGoogol test1 (root->m_normal % (p1 - root->m_origin));
						if (test0.GetAproximateValue() <= dgFloat64 (0.0f)) {

							backFace->Append(p0);
							if (test1.GetAproximateValue() > dgFloat64 (0.0f)) {
								dgHugeVector dp (p1 - p0);
								dgGoogol den = root->m_normal % dp;
								dgHugeVector p (p0 + dp.Scale (-test0.GetAproximateValue() / den.GetAproximateValue()));
								backFace->Append(p);
							}

						} else if (test1.GetAproximateValue() < dgFloat64 (0.0f)) {
							dgHugeVector dp (p1 - p0);
							dgGoogol den = root->m_normal % dp;
							dgHugeVector p (p0 + dp.Scale (-test0.GetAproximateValue() / den.GetAproximateValue()));
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
					root->m_front = new (mesh.GetAllocator())dgMeshEffectSolidTree (normal, point, mesh.GetAllocator());
				} else {
					dgHugeVector p0 (curve->GetLast()->GetInfo());
					CSGConvexCurve* const frontFace = new (mesh.GetAllocator()) CSGConvexCurve (mesh.GetAllocator());

					dgGoogol test0 (root->m_normal % (p0 - root->m_origin));

					for (CSGConvexCurve::dgListNode* node = curve->GetFirst(); node; node = node->GetNext()) {

						const dgHugeVector& p1 = node->GetInfo();
						dgGoogol test1 (root->m_normal % (p1 - root->m_origin));
						if (test0.GetAproximateValue() >= dgFloat64 (0.0f)) {

							frontFace->Append(p0);
							if (test1.GetAproximateValue() < dgFloat32 (0.0f)) {
								dgHugeVector dp (p1 - p0);
								dgGoogol den = root->m_normal % dp;
								dgHugeVector p (p0 + dp.Scale (-test0.GetAproximateValue() / den.GetAproximateValue()));
								frontFace->Append(p);
							}
						} else if (test1.GetAproximateValue() > dgFloat64 (0.0f)) {
							dgHugeVector dp (p1 - p0);
							dgGoogol den = root->m_normal % dp;
							dgHugeVector p (p0 + dp.Scale (-test0.GetAproximateValue() / den.GetAproximateValue()));
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
					if (root->m_back->m_planeType != m_divider) {
						delete root->m_back;
						root->m_back = new (mesh.GetAllocator()) dgMeshEffectSolidTree (normal, point, mesh.GetAllocator());
					} else {
						curve->AddRef();
						faces[stack] = curve;
						pool[stack] = root->m_back;
						stack ++;
						_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
					}
				} else if (maxVal > dgFloat64 (0.0f)) {
					if (root->m_front->m_planeType != m_divider) {
						delete root->m_front;
						root->m_front = new (mesh.GetAllocator())dgMeshEffectSolidTree (normal, point, mesh.GetAllocator());
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




dgMeshTreeCSGFace::dgMeshTreeCSGFace (dgMemoryAllocator* const allocator, const dgMeshEffect& mesh, dgEdge* const face)
//	:dgList<dgMeshEffect::dgVertexAtribute>(allocator), m_side(m_back)
	:dgList<dgCSGFacePoint>(allocator), m_side(m_back)
{
	const dgEdge* ptr = face;
	const dgMeshEffect::dgVertexAtribute* const attib = mesh.m_attib;
	do {
		Append (attib[ptr->m_userData]);
//dgTrace (("%f %f %f\n", attib[ptr->m_userData].m_vertex.m_x, attib[ptr->m_userData].m_vertex.m_y, attib[ptr->m_userData].m_vertex.m_z));
		ptr = ptr->m_next;
	} while (ptr != face);
//dgTrace (("\n"));

}

dgMeshTreeCSGFace::dgMeshTreeCSGFace (dgMemoryAllocator* const allocator, dgInt32 count, const dgCSGFacePoint* const points)
	:dgList<dgCSGFacePoint>(allocator), m_side(m_back)
{
	for (dgInt32 i = 0; i < count; i ++) {
//dgTrace (("%f %f %f\n", points[i].m_vertex.m_x, points[i].m_vertex.m_y, points[i].m_vertex.m_z));
		Append (points[i]);
	}
//dgTrace (("\n"));
}

dgCSGFacePoint dgMeshTreeCSGFace::Interpolate (const dgHugeVector& normal, const dgHugeVector& origin, const dgCSGFacePoint& p0, const dgCSGFacePoint& p1) const
{
	dgCSGFacePoint attr;

	dgHugeVector dp (p1.m_vertex - p0.m_vertex);
	dgGoogol den (normal % dp);
	dgGoogol num = normal % (dgHugeVector(p0.m_vertex) - origin);
	_ASSERTE (fabs (num.GetAproximateValue()) > dgFloat64 (0.0f));

	dgGoogol ti (- num / den);
	attr.m_vertex  = p0.m_vertex + dp.Scale (ti);

	dgFloat64 t = ti.GetAproximateValue();
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

bool dgMeshTreeCSGFace::CheckFaceArea (dgInt32 count, const dgCSGFacePoint* const points) const
{
	dgHugeVector area (0.0f, 0.0f, 0.0f, 0.0f);
	dgHugeVector e0 (points[1].m_vertex - points[0].m_vertex);
//e0.Trace ();
	for (dgInt32 i = 2; i < count; i ++) {
		dgHugeVector e1 (points[i].m_vertex - points[0].m_vertex);
//e1.Trace ();
		area += e0 * e1;
//area.Trace ();
		e0 = e1;
	}
//area.Trace ();
	dgGoogol areaMag2 = area % area;
	dgFloat64 val = areaMag2.GetAproximateValue();
	return (val > dgFloat64 (1.0e-12f)) ? true : false;
}

void dgMeshTreeCSGFace::Clip (const dgHugeVector& normal, const dgHugeVector& origin, dgMeshTreeCSGFace** leftOut, dgMeshTreeCSGFace** rightOut)
{
	dgInt8 pointSide[256];

	dgInt32 count = 0;
	dgInt32 rightCount = 0;
	dgInt32 leftCount = 0;
	for (dgMeshTreeCSGFace::dgListNode* ptr = GetFirst(); ptr; ptr = ptr->GetNext()) {
		const dgCSGFacePoint& p = ptr->GetInfo();

		dgGoogol test = normal % (p.m_vertex - origin);
		dgFloat64 val = test.GetAproximateValue();
		if (fabs (val) < DG_CLIPPER_TOL) {
			val = dgFloat64 (0.0f);
		}

//		dgFloat64 val = test.GetAproximateValue();
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
				inter = Interpolate (normal, origin, p0, p1);
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
		
		_ASSERTE (leftCount >= 3);
		_ASSERTE (rightCount >= 3);
		if (CheckFaceArea (leftCount, leftFace)) {
			*leftOut = new (GetAllocator()) dgMeshTreeCSGFace (GetAllocator(), leftCount, leftFace);
		}

		if (CheckFaceArea (rightCount, rightFace)) {
			*rightOut = new (GetAllocator()) dgMeshTreeCSGFace (GetAllocator(), rightCount, rightFace);
		}
		_ASSERTE (*leftOut || *rightOut);
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


dgMeshTreeCSGFace::dgFaceCode dgMeshTreeCSGFace::DetermineSide (const dgMeshEffectSolidTree* const bsp)
{
	const dgMeshEffectSolidTree* root = bsp;

	_ASSERTE (root);
	while (root->m_planeType == dgMeshEffectSolidTree::m_divider) { 
		#ifdef _DEBUG

//dgBigVector xxx1 (root->m_normal.m_x.GetAproximateValue(), root->m_normal.m_y.GetAproximateValue(), root->m_normal.m_z.GetAproximateValue(), 0.0);
//dgBigVector xxx2 (root->m_origin.m_x.GetAproximateValue(), root->m_origin.m_y.GetAproximateValue(), root->m_origin.m_z.GetAproximateValue(), 0.0);
//xxx1 = xxx1.Scale (1.0 / sqrt ((xxx1 % xxx1)));
//xxx1.m_w = - (xxx1 % xxx2);
//dgTrace (("%f %f %f %f\n", xxx1.m_x, xxx1.m_y, xxx1.m_z, xxx1.m_w));


			dgFloat64 maxVal = dgFloat64 (-1.0e20f);
			dgFloat64 minVal = dgFloat64 ( 1.0e20f);
			for (dgMeshTreeCSGFace::dgListNode* node = GetFirst(); node; node = node->GetNext()) {
				dgHugeVector p (node->GetInfo().m_vertex);
				dgGoogol test = root->m_normal % (p - root->m_origin);

				dgFloat64 dist = test.GetAproximateValue();
				if (dist > maxVal) {
					maxVal = dist;
				}
				if (dist < minVal) {
					minVal = dist;
				}
			}
			if (fabs (minVal) < (DG_CLIPPER_TOL * dgFloat64 (1.0e3))) {
				minVal = dgFloat64 (0.0f);
			}
			if (fabs (maxVal) < (DG_CLIPPER_TOL * dgFloat64 (1.0e3))) {
				maxVal = dgFloat64 (0.0f);
			}
			_ASSERTE ((minVal * maxVal) >= dgFloat64 (0.0f));
		#endif

		dgFloat64 maxDist = dgFloat64 (0.0f);
		for (dgMeshTreeCSGFace::dgListNode* node = GetFirst(); node; node = node->GetNext()) {
			dgHugeVector p (node->GetInfo().m_vertex);
			dgGoogol test = root->m_normal % (p - root->m_origin);
			dgFloat64 dist = test.GetAproximateValue();
			if (fabs (dist) > fabs (maxDist)) {
				maxDist = dist;
			}
		}
		if (fabs (maxDist) < DG_CLIPPER_TOL) {
			maxDist = dgFloat64 (0.0f);
		}

		if (maxDist > dgFloat64 (0.0f)) {
			root = root->m_front;
		} else if (maxDist < dgFloat64 (0.0f)) {
			root = root->m_back;
		} else {

			dgFaceCode isBack = DetermineSide (root->m_back);
			dgFaceCode isFront = DetermineSide (root->m_front);
			return (isBack == isFront) ? isFront : m_coplanar;
		}
	}

	_ASSERTE (root->m_planeType != dgMeshEffectSolidTree::m_divider);
	return (root->m_planeType == dgMeshEffectSolidTree::m_empty) ? m_front : m_back;
}

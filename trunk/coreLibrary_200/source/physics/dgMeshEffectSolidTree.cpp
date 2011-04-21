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



dgMeshEffectSolidTree::CSGConvexCurve::CSGConvexCurve ()
	:dgList<dgHugeVector>(NULL)
{
}

dgMeshEffectSolidTree::CSGConvexCurve::CSGConvexCurve (dgMemoryAllocator* const allocator)
	:dgList<dgHugeVector>(allocator)
{
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

dgMeshEffectSolidTree::dgMeshEffectSolidTree (const dgMeshEffect& mesh, dgEdge* const face)
{
#ifdef _DEBUG
	m_id = m_enumerator;
#endif

	BuildPlane (mesh, face, m_normal, m_origin);
	m_front = NULL;
	m_back = NULL;
}

dgMeshEffectSolidTree::dgMeshEffectSolidTree (const dgHugeVector& normal, const dgHugeVector& origin)
	:m_origin (origin), m_normal (normal)
{
#ifdef _DEBUG
	m_id = m_enumerator;
#endif

	m_front = NULL;
	m_back = NULL;
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

		CSGConvexCurve faces[DG_MESH_EFFECT_BOLLEAN_STACK];
		dgMeshEffectSolidTree* pool[DG_MESH_EFFECT_BOLLEAN_STACK];

		dgHugeVector point;
		dgHugeVector normal;
		BuildPlane (mesh, face, normal, point);

		dgEdge* ptr = face;
		faces[0].SetAllocator(mesh.GetAllocator());
		do {
			faces[0].Append(mesh.m_points[ptr->m_incidentVertex]);
			ptr = ptr->m_next;
		} while (ptr != face);

		dgInt32 stack = 1;
		pool[0] = this;
		while (stack) {

			stack --;
			dgMeshEffectSolidTree* const root = pool[stack];
			CSGConvexCurve& curve = faces[stack];
			_ASSERTE (curve.CheckConvex(normal, point));

			dgGoogol minDist (dgFloat64 (0.0f));
			dgGoogol maxDist (dgFloat64 (0.0f));
			for (CSGConvexCurve::dgListNode* node = curve.GetFirst(); node; node = node->GetNext()) {
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
					if (dist2 < dgFloat64 (1.0e-10f)) {
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
					if (dist2 < dgFloat64 (1.0e-10f)) {
						maxDist = dgGoogol (dgFloat64 (0.0f));
						maxVal = dgFloat64 (0.0f);
					}
				}
			}

			if ((minVal < dgFloat64 (0.0f)) && (maxVal > dgFloat64 (0.0f))) {

				CSGConvexCurve tmp(mesh.GetAllocator());
				for (CSGConvexCurve::dgListNode* node = curve.GetFirst(); node; node = node->GetNext()) {
					tmp.Append(node->GetInfo());
				}
				curve.RemoveAll();

				if (!root->m_back) {
					root->m_back = new (mesh.GetAllocator()) dgMeshEffectSolidTree (normal, point);
				} else {
					dgHugeVector p0 (tmp.GetLast()->GetInfo());
					CSGConvexCurve& backFace = faces[stack];

					backFace.SetAllocator(mesh.GetAllocator());
					dgGoogol test0 = root->m_normal % (p0 - root->m_origin);
					for (CSGConvexCurve::dgListNode* node = tmp.GetFirst(); node; node = node->GetNext()) {

						dgHugeVector p1 (node->GetInfo());
						dgGoogol test1 = root->m_normal % (p1 - root->m_origin);
						if (test0.GetAproximateValue() <= dgFloat64 (0.0f)) {

							backFace.Append(p0);
							if (test1.GetAproximateValue() > dgFloat64 (0.0f)) {
								dgHugeVector dp (p1 - p0);
								dgGoogol den = root->m_normal % dp;
								dgHugeVector p (p0 + dp.Scale (-test0.GetAproximateValue() / den.GetAproximateValue()));
								backFace.Append(p);
							}

						} else if (test1.GetAproximateValue() < dgFloat64 (0.0f)) {
							dgHugeVector dp (p1 - p0);
							dgGoogol den = root->m_normal % dp;
							dgHugeVector p (p0 + dp.Scale (-test0.GetAproximateValue() / den.GetAproximateValue()));
							backFace.Append(p);
						}

						test0 = test1;
						p0 = p1;
					}

					// check Here because the clipper can generate a point and lines
					_ASSERTE (!backFace.GetCount() || (backFace.GetCount() >= 3));

					pool[stack] = root->m_back;
					stack ++;
					_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
				}

				if (!root->m_front) {
					root->m_front = new (mesh.GetAllocator())dgMeshEffectSolidTree (normal, point);
				} else {
					dgHugeVector p0 (tmp.GetLast()->GetInfo());
					CSGConvexCurve& frontFace = faces[stack];

					frontFace.SetAllocator(mesh.GetAllocator());
					dgGoogol test0 = root->m_normal % (p0 - root->m_origin);

					for (CSGConvexCurve::dgListNode* node = tmp.GetFirst(); node; node = node->GetNext()) {
						dgHugeVector p1 (node->GetInfo());
						dgGoogol test1 = root->m_normal % (p1 - root->m_origin);
						if (test0.GetAproximateValue() >= dgFloat64 (0.0f)) {

							frontFace.Append(p0);
							if (test1.GetAproximateValue() < dgFloat32 (0.0f)) {
								dgHugeVector dp (p1 - p0);
								dgGoogol den = root->m_normal % dp;
								dgHugeVector p (p0 + dp.Scale (-test0.GetAproximateValue() / den.GetAproximateValue()));
								frontFace.Append(p);
							}
						} else if (test1.GetAproximateValue() > dgFloat64 (0.0f)) {
							dgHugeVector dp (p1 - p0);
							dgGoogol den = root->m_normal % dp;
							dgHugeVector p (p0 + dp.Scale (-test0.GetAproximateValue() / den.GetAproximateValue()));
							frontFace.Append(p);
						}

						test0 = test1;
						p0 = p1;
					}

					// check Here because the clipper can generate a point and lines
					_ASSERTE (!frontFace.GetCount() || (frontFace.GetCount() >= 3));

					pool[stack] = root->m_front;
					stack ++;
					_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
				}

			} else {

				if (minVal < dgFloat64 (0.0f)) {
					if (!root->m_back) {
						root->m_back = new (mesh.GetAllocator())dgMeshEffectSolidTree (normal, point);
					} else {
						pool[stack] = root->m_back;
						stack ++;
						_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
					}

				} else if (maxVal > dgFloat64 (0.0f)) {
					if (!root->m_front) {
						root->m_front = new (mesh.GetAllocator())dgMeshEffectSolidTree (normal, point);
					} else {
						pool[stack] = root->m_front;
						stack ++;
						_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
					}
				} else {
					_ASSERTE (minVal == dgFloat64 (0.0f));
					_ASSERTE (maxVal == dgFloat64 (0.0f));

					CSGConvexCurve tmp(mesh.GetAllocator());
					for (CSGConvexCurve::dgListNode* node = curve.GetFirst(); node; node = node->GetNext()) {
						tmp.Append(node->GetInfo());
					}
					curve.RemoveAll();
 
					if (root->m_front) {
						CSGConvexCurve& frontFace = faces[stack];
						frontFace.SetAllocator(mesh.GetAllocator());
						for (CSGConvexCurve::dgListNode* node = tmp.GetFirst(); node; node = node->GetNext()) {
							frontFace.Append(node->GetInfo());
						}
						
						pool[stack] = root->m_front;
						stack ++;
						_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
					}

					if (root->m_back) {
						CSGConvexCurve& backFace = faces[stack];
						backFace.SetAllocator(mesh.GetAllocator());
						for (CSGConvexCurve::dgListNode* node = tmp.GetFirst(); node; node = node->GetNext()) {
							backFace.Append(node->GetInfo());
						}

						pool[stack] = root->m_back;
						stack ++;
						_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
					}
				}
			}
		}
	}
}




dgMeshTreeCSGFace::dgMeshTreeCSGFace (dgMemoryAllocator* const allocator, const dgMeshEffect& mesh, dgEdge* const face)
	:dgList<dgMeshEffect::dgVertexAtribute>(allocator), m_iscoplanar(false), m_frontSize(false)
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

dgMeshTreeCSGFace::dgMeshTreeCSGFace (dgMemoryAllocator* const allocator, dgInt32 count, const dgMeshEffect::dgVertexAtribute* const points)
	:dgList<dgMeshEffect::dgVertexAtribute>(allocator), m_iscoplanar(false), m_frontSize(false)
{
	for (dgInt32 i = 0; i < count; i ++) {
//dgTrace (("%f %f %f\n", points[i].m_vertex.m_x, points[i].m_vertex.m_y, points[i].m_vertex.m_z));
		Append (points[i]);
	}
//dgTrace (("\n"));
}

dgMeshEffect::dgVertexAtribute dgMeshTreeCSGFace::InterPolalate (const dgHugeVector& normal, const dgHugeVector& origin, const dgMeshEffect::dgVertexAtribute& p0, const dgMeshEffect::dgVertexAtribute& p1) const
{
	dgHugeVector dp (p1.m_vertex - p0.m_vertex);
	dgGoogol den (normal % dp);
	dgGoogol num = normal % (dgHugeVector(p0.m_vertex) - origin);
	_ASSERTE (fabs (num.GetAproximateValue()) > dgFloat64 (0.0f));

	dgFloat64 t = -num.GetAproximateValue() / den.GetAproximateValue();
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
	return attr;
}

void dgMeshTreeCSGFace::Clip (const dgHugeVector& normal, const dgHugeVector& origin, dgMeshTreeCSGFace** leftOut, dgMeshTreeCSGFace** rightOut)
{
	dgInt8 pointSide[256];

	dgInt32 count = 0;
	dgInt32 rightCount = 0;
	dgInt32 leftCount = 0;
	for (dgMeshTreeCSGFace::dgListNode* ptr = GetFirst(); ptr; ptr = ptr->GetNext()) {
		const dgMeshEffect::dgVertexAtribute& p = ptr->GetInfo();

		dgGoogol test = normal % (dgHugeVector (p.m_vertex) - origin);
		if (fabs (test.GetAproximateValue()) < dgFloat64 (1.0e-12f)) {
			test = dgGoogol(dgFloat64 (0.0f));
		}

		dgFloat64 val = test.GetAproximateValue();
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
		dgMeshEffect::dgVertexAtribute leftFace[256];
		dgMeshEffect::dgVertexAtribute rightFace[256];

		dgInt32 i1 = 0;
		dgInt32 i0 = count - 1;
		dgMeshEffect::dgVertexAtribute p0 (GetLast()->GetInfo());
		for (dgMeshTreeCSGFace::dgListNode* ptr = GetFirst(); ptr; ptr = ptr->GetNext()) {
			dgMeshEffect::dgVertexAtribute p1 (ptr->GetInfo());

			dgMeshEffect::dgVertexAtribute inter; 
			if (((pointSide[i0] == -1) && (pointSide[i1] == 1)) || ((pointSide[i0] == 1) && (pointSide[i1] == -1))) {
				inter = InterPolalate (normal, origin, p0, p1);
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
		*leftOut = new (GetAllocator()) dgMeshTreeCSGFace (GetAllocator(), leftCount, leftFace);
		*rightOut = new (GetAllocator()) dgMeshTreeCSGFace (GetAllocator(), rightCount, rightFace);
	}
}

bool dgMeshTreeCSGFace::IsPointOnEdge (const dgBigVector& p0, const dgBigVector& p1, const dgBigVector& q0) const
{
	dgBigVector p1p0 (p1 - p0);
	dgFloat64 den = p1p0 % p1p0;

	dgBigVector q0p0 (q0 - p0);
	dgFloat64 num = q0p0 % p1p0;

	if (num > dgFloat32 (1.0e-12f)) {
		if (num < den * dgFloat64 (1.0 - 1.0e-12f)) {
			dgFloat64 t = num / den;
			_ASSERTE (t > dgFloat64 (0.0f));
			_ASSERTE (t < dgFloat64 (1.0f));
			dgBigVector q1 (p0 + p1p0.Scale (t));
			dgBigVector dist (q1 - q0);
			dgFloat64 dist2 = dist % dist;		
			if (dist2 < dgFloat64 (1.0e-16f)) {
				return true;
			}
		}
	}
	return false;
}

void dgMeshTreeCSGFace::MergeMissingVertex (const dgMeshTreeCSGFace* const face)
{
	for (dgMeshTreeCSGFace::dgListNode* outNode = GetFirst(); outNode != GetLast(); outNode = outNode->GetNext()) {
		dgBigVector p0 (outNode->GetInfo().m_vertex);
		for (dgMeshTreeCSGFace::dgListNode* node = face->GetFirst(); node; node = node->GetNext()) {
			if (IsPointOnEdge (p0, outNode->GetNext()->GetInfo().m_vertex, node->GetInfo().m_vertex)) {
				dgMeshTreeCSGFace::dgListNode* const insertNode = Append(node->GetInfo());
				InsertAfter(outNode, insertNode);
			}
		}
	}

	RotateToBegin (GetLast());
	dgListNode* const last = GetFirst()->GetNext();
	for (dgMeshTreeCSGFace::dgListNode* outNode = GetFirst(); outNode != last; outNode = outNode->GetNext()) {	
		dgBigVector p0 (outNode->GetInfo().m_vertex);
		dgBigVector p1 (outNode->GetNext()->GetInfo().m_vertex);

		for (dgMeshTreeCSGFace::dgListNode* node = face->GetFirst(); node; node = node->GetNext()) {
			if (IsPointOnEdge (p0, outNode->GetNext()->GetInfo().m_vertex, node->GetInfo().m_vertex)) {
				dgMeshTreeCSGFace::dgListNode* const insertNode = Append(node->GetInfo());
				InsertAfter(outNode, insertNode);
			}
		}
	}
}


void dgMeshTreeCSGFace::DetermineSide (const dgMeshEffectSolidTree* const bsp)
{
	for (const dgMeshEffectSolidTree* root = bsp; root;) { 
		#ifdef _DEBUG

dgBigVector xxx1 (root->m_normal.m_x.GetAproximateValue(), root->m_normal.m_y.GetAproximateValue(), root->m_normal.m_z.GetAproximateValue(), 0.0);
dgBigVector xxx2 (root->m_origin.m_x.GetAproximateValue(), root->m_origin.m_y.GetAproximateValue(), root->m_origin.m_z.GetAproximateValue(), 0.0);
xxx1 = xxx1.Scale (1.0 / sqrt ((xxx1 % xxx1)));
xxx1.m_w = - (xxx1 % xxx2);
dgTrace (("%f %f %f %f\n", xxx1.m_x, xxx1.m_y, xxx1.m_z, xxx1.m_w));


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
			if (fabs (minVal) < dgFloat64 (1.0e-7f)) {
				minVal = dgFloat64 (0.0f);
			}
			if (fabs (maxVal) < dgFloat64 (1.0e-7f)) {
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
		if (fabs (maxDist) < dgFloat64 (1.0e-7f)) {
			maxDist = dgFloat64 (0.0f);
		}

		if (maxDist > dgFloat64 (0.0f)) {
			root = root->m_front;
			m_frontSize = true;
			m_iscoplanar = false;
		} else if (maxDist < dgFloat64 (0.0f)) {
			m_frontSize = false;
			root = root->m_back;
		} else {
			m_iscoplanar = true;
			if (root->m_front) {
				root = root->m_front;
			} else {
				root = root->m_back;
			}
		}
	}
}

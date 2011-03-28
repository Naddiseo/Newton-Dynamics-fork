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



dgMeshTreeCSGFace::dgMeshTreeCSGFace (dgMeshEffect& mesh, dgEdge* const face, dgInt32 vertexCount)
	:dgPolyhedra (mesh.GetAllocator())
	,m_count (0)
	,m_lastVertexIndex(vertexCount)
	,m_face(face)
	,m_mesh(&mesh)
{
	const dgEdge* ptr = face;

	const dgFloat64 *const vertex = mesh.GetVertexPool();
	dgInt32 stride = mesh.GetVertexStrideInByte() / sizeof (dgFloat64);
	
	dgInt32 indexList[256];
	do {
		indexList[m_count] = m_count;	
		m_intepolatedVertex[m_count] = (ptr->m_incidentVertex >= m_lastVertexIndex) ? true : false;
		dgInt32 index = ptr->m_incidentVertex * stride;

		dgBigVector p (vertex[index + 0], vertex[index + 1], vertex[index + 2], dgFloat64 (0.0f));
		AddPoint (p);
		
		ptr = ptr->m_next;
	} while (ptr != face);

	BeginFace();
	dgEdge* const edge = AddFace(m_count, indexList);
	EndFace();

	const dgEdge* src = face;
	dgEdge* dst = edge;
	do {
		dst->m_userData = src->m_incidentVertex;
		dst->m_twin->m_userData = dgUnsigned64 (-1);
		src = src->m_next;
		dst = dst->m_next;
	} while (dst != edge);

	_ASSERTE (CheckConsistency ());
}

dgInt32 dgMeshTreeCSGFace::AddPoint (const dgBigVector& point)
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

/*
dgMeshTreeCSGFace::dgMeshTreeCSGFace(dgMemoryAllocator* const allocator)
	:m_face(NULL), m_last(NULL), m_allocator (allocator)
{
}


dgMeshTreeCSGFace::~dgMeshTreeCSGFace() 
{
	CSGLinearEdge* ptr = m_face;
	do {
		CSGLinearEdge* const me = ptr;
		ptr = ptr->m_next;
		delete me;
	} while (ptr != m_face);
}


void dgMeshTreeCSGFace::AddPoint (dgInt32 index)
{
	if (!m_last) {
		m_face = new (m_allocator) CSGLinearEdge (index);
		m_last = m_face;
	} else {
		m_last = new (m_allocator) CSGLinearEdge (index, m_last);
	}
	m_last->m_next = m_face;
}



void dgMeshTreeCSGFace::InsertVertex (const dgMeshTreeCSGFace* const vertices, const dgMeshTreeCSGPointsPool& pool) 
{
	CSGLinearEdge* points = vertices->m_face;
	do {
		CSGLinearEdge* closestEdge = NULL; 
		dgFloat64 smallDist (dgFloat64 (1.0e10f));
		dgHugeVector p (pool.m_points[points->m_index]);

		CSGLinearEdge* edge = m_face;
		do {
			dgHugeVector p0 (pool.m_points[edge->m_index]);
			dgHugeVector p1 (pool.m_points[edge->m_next->m_index]);
			dgHugeVector dp0 (p - p0);
			dgHugeVector dp  (p1 - p0);
			dgFloat64 t = (dp0 % dp).GetAproximateValue() / (dp % dp).GetAproximateValue();
			if ((t > dgFloat64 (0.0)) && (t < dgFloat64 (1.0f))) {
				dgHugeVector q (p0 + dp.Scale(t));
				dgHugeVector dist (q - p);
				dgGoogol dist2 = (dist % dist);
				dgFloat64 val (dist2.GetAproximateValue());
				if (val < smallDist) {
					smallDist = val;
					closestEdge = edge;
				}
			}
			edge = edge->m_next;
		} while (edge != m_face);

		if (smallDist < dgFloat64 (1.0e-20f)) {
			CSGLinearEdge* const edge = new (m_allocator) CSGLinearEdge (points->m_index);
			edge->m_next = closestEdge->m_next;
			closestEdge->m_next = edge;
		}

		points = points->m_next;
	} while (points != vertices->m_face);
}

bool dgMeshTreeCSGFace::CheckConvex(const dgMeshTreeCSGPointsPool& pool) const
{
	dgBigVector normal (dgFloat64 (0.0f), dgFloat64 (0.0f), dgFloat64 (0.0f), dgFloat64 (0.0f));

	CSGLinearEdge* ptr = m_face;

	dgBigVector p0 (pool.m_points[ptr->m_index]);
	ptr = ptr->m_next;

	dgBigVector p1 (pool.m_points[ptr->m_index]);
	ptr = ptr->m_next;

	dgBigVector e0 (p1 - p0);
	do {
		dgBigVector p2 (pool.m_points[ptr->m_index]);
		dgBigVector e1 (p2 - p0);
		normal += e1 * e0;
		e0 = e1;
		ptr = ptr->m_next;
	} while (ptr != m_face);

	if ((normal % normal) < dgFloat64 (1.0e-24f)) {
		return false;
	}

	ptr = m_face;
	p0 = pool.m_points[ptr->m_index];
	ptr = ptr->m_next;

	p1 = pool.m_points[ptr->m_index];
	ptr = ptr->m_next;

	e0 = p1 - p0;
	do {
		dgBigVector p2 (pool.m_points[ptr->m_index]);
		dgBigVector e1 (p2 - p0);
		dgBigVector normal1 (e1 * e0);
		if ((normal1 % normal)< dgFloat64(0.0f)) {
			return false;
		}
		e0 = e1;
		ptr = ptr->m_next;
	} while (ptr != m_face);

	return true;
}
*/


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




dgMeshEffectSolidTree::dgMeshEffectSolidTree (const dgMeshEffect& mesh, dgEdge* const face)
{
	BuildPlane (mesh, face, m_normal, m_origin);
	//normal = normal.Scale (1.0f / sqrt (normal % normal));
	//m_plane = dgBigPlane (normal, - (normal % origin));
	m_front = NULL;
	m_back = NULL;
}

dgMeshEffectSolidTree::dgMeshEffectSolidTree (const dgHugeVector& normal, const dgHugeVector& origin)
	:m_origin (origin), m_normal (normal)
{
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

	dgHugeVector p0 (dgBigVector (mesh.m_points[edge->m_incidentVertex]));
	edge = edge->m_next;

	dgHugeVector p1 (dgBigVector (mesh.m_points[edge->m_incidentVertex]));
	dgHugeVector e1 (p1 - p0);
	//		_ASSERTE (edge->m_next->m_next->m_next == edge);
	for (edge = edge->m_next; edge != face; edge = edge->m_next) {
		dgHugeVector p2 (dgBigVector (mesh.m_points[edge->m_incidentVertex]));
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
			faces[0].Append(dgBigVector (mesh.m_points[ptr->m_incidentVertex]));
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
				if (dist.GetAproximateValue() < dgFloat32 (0.0f)) {
					minDist = dist;
				}
				if (dist.GetAproximateValue() > dgFloat32 (0.0f)) {
					maxDist = dist;
				} 
			} 


			if ((minDist.GetAproximateValue() < dgFloat64 (0.0f)) && (maxDist.GetAproximateValue() > dgFloat64 (0.0f))) {
_ASSERTE (0);
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

				if (minDist.GetAproximateValue() < dgFloat64 (0.0f)) {
					if (!root->m_back) {
						root->m_back = new (mesh.GetAllocator())dgMeshEffectSolidTree (normal, point);
					} else {
						pool[stack] = root->m_back;
						stack ++;
						_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
					}

				} else if (maxDist.GetAproximateValue() > dgFloat64 (0.0f)) {
					if (!root->m_front) {
						root->m_front = new (mesh.GetAllocator())dgMeshEffectSolidTree (normal, point);
					} else {
						pool[stack] = root->m_front;
						stack ++;
						_ASSERTE (stack < (sizeof (pool)/sizeof (pool[0])));
					}
				}
			}
		}
	}
}




void dgMeshTreeCSGFace::ClipFace (dgEdge* const face, const dgHugeVector& normal, const dgHugeVector& origin, dgEdge** const poolReplacement, dgInt32 poolCount, dgEdge** leftOut, dgEdge** rightOut)
{
	*leftOut = NULL; 
	*rightOut = NULL;	

_ASSERTE (!poolCount);

	dgEdge* left = NULL;
	dgEdge* right = NULL;
	dgEdge* lastEdge = face;
	dgEdge* ptr = face;
	dgHugeVector p0 (m_points[ptr->m_incidentVertex]);
	dgGoogol test0 = normal % (p0 - origin);
	if (m_intepolatedVertex[ptr->m_incidentVertex]) {
		if (fabs (test0.GetAproximateValue()) < dgFloat64 (1.0e-12f)) {
			test0 = dgGoogol(dgFloat64 (0.0f));
		}
	}

	do {
		dgHugeVector p1 (m_points[ptr->m_next->m_incidentVertex]);
		dgGoogol test1 = normal % (p1 - origin);
		if (m_intepolatedVertex[ptr->m_next->m_incidentVertex]) {
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
				dgHugeVector dp (p1 - p0);
				dgGoogol den (normal % dgHugeVector (dp));
				dgGoogol t (-test0.GetAproximateValue() / den.GetAproximateValue());
				dgHugeVector q (p0 + (p1 - p0).Scale (t));

				dgUnsigned64 edgeUserData = ptr->m_userData;
				dgUnsigned64 twinUserData = ptr->m_twin->m_userData;

				ptr = SpliteEdge (m_count, ptr);
				if (replaceface) {
					lastEdge = ptr;
				}

				ptr->m_userData = edgeUserData;
				ptr->m_twin->m_userData = dgUnsigned64 (-1);
				ptr->m_next->m_userData = dgUnsigned64 (-1);
				ptr->m_next->m_twin->m_userData = twinUserData;

				m_intepolatedVertex[m_count] = false;
				AddPoint (dgBigVector (q.m_x.GetAproximateValue(), q.m_y.GetAproximateValue(), q.m_z.GetAproximateValue(), dgFloat64 (0.0f)));
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
				dgHugeVector dp (p1 - p0);
				dgGoogol den (normal % dgHugeVector (dp));
				dgGoogol t (-test0.GetAproximateValue() / den.GetAproximateValue());
				dgHugeVector q (p0 + (p1 - p0).Scale (t));

				dgUnsigned64 edgeUserData = ptr->m_userData;
				dgUnsigned64 twinUserData = ptr->m_twin->m_userData;
		
				ptr = SpliteEdge (m_count, ptr);
				if (replaceface) {
					lastEdge = ptr;
				}

				ptr->m_userData = edgeUserData;
				ptr->m_twin->m_userData = dgUnsigned64 (-1);
				ptr->m_next->m_userData = dgUnsigned64 (-1);
				ptr->m_next->m_twin->m_userData = twinUserData;

				m_intepolatedVertex[m_count] = false;
				AddPoint (dgBigVector (q.m_x.GetAproximateValue(), q.m_y.GetAproximateValue(), q.m_z.GetAproximateValue(), dgFloat64 (0.0f)));
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

		edge->m_userData = dgUnsigned64 (-1);
		twin->m_userData = dgUnsigned64 (-1);

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



void dgMeshTreeCSGFace::MatchFace (dgInt32 leftFaceId, dgInt32 rightFaceId, dgTree<dgEdge*,dgEdge*>& edgeList)
{

static int xxx;
static int xxx1;
xxx ++;
if (xxx == 8)
xxx *=1;


	dgEdge* outerEdge = NULL;
	dgMeshTreeCSGFace::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		dgEdge* const edge = &(*iter);
		if (edge->m_incidentFace < 0) {
			outerEdge = edge;
			break;
		}
	}
	_ASSERTE (outerEdge);
	_ASSERTE (CheckConsistency ());

	dgEdge* edge = m_face;
	do {
		dgEdge* const nextEdge = edge->m_next;

		dgEdge* outerEdgeFirst = NULL;
		dgEdge* ptr = outerEdge;
		do {
			if (dgInt32 (ptr->m_prev->m_twin->m_userData) == edge->m_incidentVertex) {
				outerEdgeFirst = ptr;
				break;
			}
			ptr = ptr->m_next;
		} while (ptr != outerEdge);
		_ASSERTE (outerEdgeFirst);

		dgEdge* outerEdgeLast = NULL;
		ptr = outerEdge;
		do {
			if (dgInt32 (ptr->m_prev->m_twin->m_userData) == edge->m_next->m_incidentVertex) {
				outerEdgeLast = ptr;
				break;
			}
			ptr = ptr->m_next;
		} while (ptr != outerEdge);
		_ASSERTE (outerEdgeLast);
		_ASSERTE (outerEdgeFirst != outerEdgeLast);

		if (outerEdgeFirst->m_prev == outerEdgeLast) {
			edge->m_incidentFace |= outerEdgeFirst->m_prev->m_twin->m_incidentFace & (leftFaceId + rightFaceId);

			dgEdge* tmp = outerEdgeFirst;
			do {
				tmp->m_userData = edge->m_incidentVertex;
				tmp = tmp->m_twin->m_next;
			} while (tmp != outerEdgeFirst);

			tmp = outerEdgeLast;
			do {
				tmp->m_userData = edge->m_next->m_incidentVertex;
				tmp = tmp->m_twin->m_next;
			} while (tmp != outerEdgeLast);

		} else {
			dgBigVector p0 (m_mesh->m_points[edge->m_incidentVertex]);
			dgBigVector p1 (m_mesh->m_points[edge->m_next->m_incidentVertex]);
			for (outerEdgeFirst = outerEdgeFirst->m_prev; outerEdgeFirst != outerEdgeLast; outerEdgeFirst = outerEdgeFirst->m_prev) {
				dgBigVector q (m_points[outerEdgeFirst->m_incidentVertex]);
				dgBigVector dist (p1 - p0);
				dgFloat64 num = (q - p0) % dist;
				dgFloat64 den = dist % dist;
				_ASSERTE (num > dgFloat64 (0.0f));
				_ASSERTE (num < den);
				dgFloat64 t = num / den;


				dgTree<dgEdge*,dgEdge*>::dgTreeNode* const node = edgeList.Find(edge->m_twin);
				bool fistLoop = (edge == m_face) ;
				edge = m_mesh->InsertEdgeVertex (edge, t);
				if (fistLoop) {
					m_face = edge;
				}

				edge->m_incidentFace = outerEdgeFirst->m_twin->m_incidentFace + 1;
				dgEdge* tmp = outerEdgeFirst->m_twin;
				do {
					tmp->m_userData = edge->m_incidentVertex;
					tmp = tmp->m_twin->m_next;
				} while (tmp != outerEdgeFirst->m_twin);

				edge = edge->m_next;
				edge->m_incidentFace = outerEdgeFirst->m_prev->m_twin->m_incidentFace + 1;
				tmp = outerEdgeFirst->m_prev->m_twin;
				do {
					tmp->m_userData = edge->m_incidentVertex;
					tmp = tmp->m_twin->m_next;
				} while (tmp != outerEdgeFirst->m_prev->m_twin);

				if (node) {
					edgeList.Insert(edge->m_twin, edge->m_twin);
					edgeList.Insert(edge->m_twin->m_next, edge->m_twin->m_next);
				}
			}
		}
		
		edge = nextEdge;
	} while (edge != m_face);



	dgList<dgClipEdgePair> queue (GetAllocator());
	dgInt32 interiorEdgeMark = IncLRU();
	edge = outerEdge;
	do {
		edge->m_mark = interiorEdgeMark;
		edge->m_twin->m_mark = interiorEdgeMark;
		dgClipEdgePair pair;
		pair.m_clipEdge = edge->m_twin;
		pair.m_edgeOneFace = m_mesh->FindEdge(dgInt32 (edge->m_twin->m_userData), dgInt32 (edge->m_prev->m_twin->m_userData));
		_ASSERTE (pair.m_edgeOneFace);
		queue.Append (pair);
		edge = edge->m_next;
	} while (edge != outerEdge);

if (xxx == 8)
xxx *=1;

	while (queue.GetCount()) {
xxx1 ++;
if (xxx1 == 77)
xxx1 *= 1;

		dgClipEdgePair pair (queue.GetFirst()->GetInfo());
		queue.Remove(queue.GetFirst());

		dgEdge* ptr = pair.m_clipEdge;
		if ((ptr->m_userData != -1) && (ptr->m_next->m_userData != -1)) {
			dgEdge* const missingEdge = m_mesh->FindEdge(dgInt32 (ptr->m_userData), dgInt32 (ptr->m_next->m_userData));
			if (!missingEdge) {
				dgEdge* const edge = m_mesh->AddHalfEdge(dgInt32 (ptr->m_userData), dgInt32 (ptr->m_twin->m_userData));
				dgEdge* const twin = m_mesh->AddHalfEdge(dgInt32 (ptr->m_twin->m_userData), dgInt32 (ptr->m_userData));
				_ASSERTE (edge);
				_ASSERTE (twin);

				dgEdge* begin = NULL;	
				dgEdge* tmp = pair.m_edgeOneFace;	
				do {
					if (tmp->m_incidentVertex == dgInt32 (ptr->m_userData)) {
						begin = tmp;
						break;
					}
					tmp = tmp->m_next;
				} while (tmp != pair.m_edgeOneFace);
				_ASSERTE (begin);
				_ASSERTE (begin->m_incidentVertex == dgInt32 (ptr->m_userData));


				dgEdge* end = NULL;	
				tmp = pair.m_edgeOneFace;	
				do {
					if (tmp->m_incidentVertex == dgInt32 (ptr->m_twin->m_userData)) {
						end = tmp;
						break;
					}
					tmp = tmp->m_next;
				} while (tmp != pair.m_edgeOneFace);
				_ASSERTE (end);
				_ASSERTE (end->m_incidentVertex == dgInt32 (ptr->m_twin->m_userData));

				edge->m_userData = begin->m_userData;
				twin->m_userData = end->m_userData;;

				edge->m_incidentFace = begin->m_prev->m_incidentFace;
				twin->m_incidentFace = end->m_prev->m_incidentFace;

				edge->m_twin = twin;
				twin->m_twin = edge;

				edge->m_prev = begin->m_prev;
				begin->m_prev->m_next = edge;

				twin->m_next = begin;
				begin->m_prev = twin;

				twin->m_prev = end->m_prev;
				end->m_prev->m_next = twin;

				edge->m_next = end;
				end->m_prev = edge;
			}
		} else {
			_ASSERTE ((ptr->m_next->m_userData != -1) || (ptr->m_userData != -1));
			if (ptr->m_userData != -1) {

				_ASSERTE (ptr->m_next->m_userData == -1);
				dgBigVector p (m_points[ptr->m_twin->m_incidentVertex]);

				dgMeshEffect::dgVertexAtribute atrib (m_mesh->InterpolateVertex (p, pair.m_edgeOneFace));
				dgEdge* const edge = m_mesh->AddHalfEdge(dgInt32 (ptr->m_userData), m_mesh->m_pointCount);
				dgEdge* const twin = m_mesh->AddHalfEdge(m_mesh->m_pointCount, dgInt32 (ptr->m_userData));
				_ASSERTE (edge);
				_ASSERTE (twin);

				dgEdge* begin = NULL;	
				dgEdge* tmp = pair.m_edgeOneFace;	
				do {
					if (tmp->m_incidentVertex == dgInt32 (ptr->m_userData)) {
						begin = tmp;
						break;
					}
					tmp = tmp->m_next;
				} while (tmp != pair.m_edgeOneFace);
				_ASSERTE (begin);
				_ASSERTE (begin->m_incidentVertex == dgInt32 (ptr->m_userData));

				edge->m_userData = begin->m_userData;
				twin->m_userData = m_mesh->m_atribCount;

				edge->m_incidentFace = begin->m_prev->m_incidentFace;
				twin->m_incidentFace = begin->m_incidentFace;

				edge->m_twin = twin;
				twin->m_twin = edge;

				edge->m_next = twin;
				twin->m_prev = edge;

				edge->m_prev = begin->m_prev;
				begin->m_prev->m_next = edge;

				twin->m_next = begin;
				begin->m_prev = twin;

				m_mesh->AddPoint(&atrib.m_vertex.m_x, dgFastInt (atrib.m_material));
				_ASSERTE (edge);
				_ASSERTE (twin);

				tmp = ptr->m_next;
				do {
					tmp->m_userData = twin->m_incidentVertex;
					tmp = tmp->m_twin->m_next;
				} while (tmp != ptr->m_next);

			} else {
				_ASSERTE (ptr->m_userData == -1);
				dgBigVector p (m_points[ptr->m_incidentVertex]);

				dgMeshEffect::dgVertexAtribute atrib (m_mesh->InterpolateVertex (p, pair.m_edgeOneFace));
				dgEdge* const edge = m_mesh->AddHalfEdge(m_mesh->m_pointCount, dgInt32 (ptr->m_twin->m_userData));
				dgEdge* const twin = m_mesh->AddHalfEdge(dgInt32 (ptr->m_twin->m_userData), m_mesh->m_pointCount);
				_ASSERTE (edge);
				_ASSERTE (twin);

				dgEdge* begin = NULL;	
				dgEdge* tmp = pair.m_edgeOneFace;	
				do {
					if (tmp->m_incidentVertex == dgInt32 (ptr->m_twin->m_userData)) {
						begin = tmp;
						break;
					}
					tmp = tmp->m_next;
				} while (tmp != pair.m_edgeOneFace);
				_ASSERTE (begin);
				_ASSERTE (begin->m_incidentVertex == dgInt32 (ptr->m_twin->m_userData));

				edge->m_userData = m_mesh->m_atribCount;
				twin->m_userData = begin->m_prev->m_userData;

				edge->m_incidentFace = begin->m_incidentFace;
				twin->m_incidentFace = begin->m_prev->m_incidentFace;

				edge->m_twin = twin;
				twin->m_twin = edge;

				edge->m_prev = twin;
				twin->m_next = edge;

				twin->m_prev = begin->m_prev;
				begin->m_prev->m_next = twin;

				edge->m_next = begin;
				begin->m_prev = edge;

				m_mesh->AddPoint(&atrib.m_vertex.m_x, dgFastInt (atrib.m_material));

				tmp = ptr;
				do {
					tmp->m_userData = edge->m_incidentVertex;
					tmp = tmp->m_twin->m_next;
				} while (tmp != ptr);
			}
		}

		do {
			if (ptr->m_mark != interiorEdgeMark) {

				dgClipEdgePair pair;
				pair.m_clipEdge = ptr;
				pair.m_edgeOneFace = NULL;
				dgEdge* ptr1 = ptr;
				do {
					if ((ptr1->m_userData != -1) && (ptr1->m_next->m_userData != -1)) {
						pair.m_edgeOneFace = m_mesh->FindEdge(dgInt32 (ptr1->m_userData), dgInt32 (ptr1->m_next->m_userData));
						if (pair.m_edgeOneFace) {
							break;
						}
					}

					ptr1 = ptr1->m_next;
				} while (ptr1 != ptr);
				if (pair.m_edgeOneFace) {
					ptr->m_mark = interiorEdgeMark;
					queue.Append (pair);
				}
			}

			if (ptr->m_twin->m_mark != interiorEdgeMark) {


				dgClipEdgePair pair;
				pair.m_clipEdge = ptr->m_twin;
				pair.m_edgeOneFace = NULL;
				dgEdge* ptr1 = ptr->m_twin;
				do {
					if ((ptr1->m_userData != -1) && (ptr1->m_next->m_userData != -1)) {
						pair.m_edgeOneFace = m_mesh->FindEdge(dgInt32 (ptr1->m_userData), dgInt32 (ptr1->m_next->m_userData));
						if (pair.m_edgeOneFace) {
							break;
						}
					}
					ptr1 = ptr1->m_next;
				} while (ptr1 != ptr->m_twin);
				if (pair.m_edgeOneFace) {
					ptr->m_twin->m_mark = interiorEdgeMark;
					queue.Append (pair);
				}
			}
			ptr = ptr->m_twin->m_next;
		} while (ptr != pair.m_clipEdge);
	}
}

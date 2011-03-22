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
//#include "dgBody.h"
//#include "dgWorld.h"
#include "dgMeshEffect.h"
#include "dgMeshEffectSolidTree.h"
//#include "dgCollisionConvexHull.h"


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
		const dgHugeVector& p = pool.m_points[points->m_index];

		CSGLinearEdge* edge = m_face;
		do {
			const dgHugeVector& p0 = pool.m_points[edge->m_index];
			const dgHugeVector& p1 = pool.m_points[edge->m_next->m_index];
			dgHugeVector dp0 (p - p0);
			dgHugeVector dp  (p1 - p0);
			dgFloat64 t = (dp0 % dp).GetAproximateValue() / (dp % dp).GetAproximateValue();
			if ((t > dgFloat64 (0.0)) && (t < dgFloat64 (1.0f))) {
				dgHugeVector dist (dp0 - dp.Scale(t));
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



//bool dgMeshTreeCSGFace::CheckConvex(const dgMeshEffect* mesh, dgEdge* face, const dgMeshTreeCSGPointsPool& pool) const
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



dgMeshEffectSolidTree::CSGConvexCurve::CSGConvexCurve ()
	:dgList<dgHugeVector>(NULL)
{
}

dgMeshEffectSolidTree::CSGConvexCurve::CSGConvexCurve (dgMemoryAllocator* const allocator)
	:dgList<dgHugeVector>(allocator)
{
}

//bool CheckConvex(const dgBigVector& plane) const
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
	BuildPlane (mesh, face, m_normal, m_point);
	//normal = normal.Scale (1.0f / sqrt (normal % normal));
	//m_plane = dgBigPlane (normal, - (normal % origin));
	m_front = NULL;
	m_back = NULL;
}

dgMeshEffectSolidTree::dgMeshEffectSolidTree (const dgHugeVector& normal, const dgHugeVector& point)
	:m_point (point), m_normal (normal)
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
				dgGoogol dist = root->m_normal % (node->GetInfo() - root->m_point);
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
					dgGoogol test0 = root->m_normal % (p0 - root->m_point);
					for (CSGConvexCurve::dgListNode* node = tmp.GetFirst(); node; node = node->GetNext()) {

						dgHugeVector p1 (node->GetInfo());
						dgGoogol test1 = root->m_normal % (p1 - root->m_point);
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
					dgGoogol test0 = root->m_normal % (p0 - root->m_point);

					for (CSGConvexCurve::dgListNode* node = tmp.GetFirst(); node; node = node->GetNext()) {
						dgHugeVector p1 (node->GetInfo());
						dgGoogol test1 = root->m_normal % (p1 - root->m_point);
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


/*
void dgMeshEffect::CopyCGSFace (const dgMeshEffect& reference, dgEdge* const face)
{
	dgVertexAtribute points[DG_MESH_EFFECT_POINT_SPLITED];

	dgInt32 count = 0;
	dgEdge* ptr = face;
	do {
		points[count] = reference.m_attib[ptr->m_userData];
		count ++;
		ptr = ptr->m_next;
	} while (ptr != face);

	AddPolygon(count, &points[0].m_vertex.m_x, sizeof (dgVertexAtribute), dgInt32 (points[0].m_material));
}

*/


void dgMeshEffectSolidTree::MergeVertex(const dgMeshTreeCSGPointsPool& pointsPool,  dgInt32 count, dgMeshTreeCSGFace** const faceList) const
{
static int xxx;
	for (dgInt32 i = 0; i < (count - 1); i ++) {
		dgMeshTreeCSGFace* const src = faceList[i]; 
		for (dgInt32 j = i + 1; j < count; j ++) {
			dgMeshTreeCSGFace* const dst = faceList[j]; 

xxx ++;
			_ASSERTE (src->CheckConvex(pointsPool));
			_ASSERTE (dst->CheckConvex(pointsPool));

			src->InsertVertex (dst, pointsPool);
			dst->InsertVertex (src, pointsPool);

			_ASSERTE (src->CheckConvex(pointsPool));
			_ASSERTE (dst->CheckConvex(pointsPool));
		}
	}
}

void dgMeshEffectSolidTree::AddFaces(dgMeshEffect& polygon, dgInt32 count, dgMeshTreeCSGFace** const faceList, dgInt32 faceID, const dgMeshTreeCSGPointsPool& pointsPool) const
{
	dgMeshEffect::dgVertexAtribute points[DG_MESH_EFFECT_POINT_SPLITED];
	for (dgInt32 i = 0; i < count; i ++) {
		
		dgMeshTreeCSGFace* const face = faceList[i]; 
		dgMeshTreeCSGFace::CSGLinearEdge* const edge = face->m_face;
		dgMeshTreeCSGFace::CSGLinearEdge* ptr = face->m_face;

		dgInt32 indexCount = 0;
		do {
			points[indexCount].m_vertex = pointsPool.m_points[ptr->m_index];
			points[indexCount].m_normal_x = dgFloat32 (0.0f);
			points[indexCount].m_normal_y = dgFloat32 (0.0f);
			points[indexCount].m_normal_z = dgFloat32 (0.0f);
			points[indexCount].m_u0 = dgFloat32 (0.0f);
			points[indexCount].m_v0 = dgFloat32 (0.0f);
			points[indexCount].m_u1 = dgFloat32 (0.0f);
			points[indexCount].m_v1 = dgFloat32 (0.0f);
			points[indexCount].m_material = dgFloat32 (0.0f);
			indexCount ++;
			ptr = ptr->m_next;
		} while (ptr != edge);
		polygon.AddPolygon(indexCount, &points[0].m_vertex.m_x, sizeof (dgMeshEffect::dgVertexAtribute), faceID);
	}
}

//void dgMeshEffect::AddCGSFace (const dgMeshEffect& reference, dgEdge* const refFace, dgInt32 count, dgMeshTreeCSGFace** const faces, const dgMeshTreeCSGPointsPool& pointsPool)
void dgMeshEffectSolidTree::ReconstructFace (dgMeshEffect& polygon, const dgMeshTreeCSGPointsPool& pointsPool,
					  dgInt32 frontFaceCount, dgMeshTreeCSGFace** const frontFace, dgInt32 frontFaceID,
					  dgInt32 backFaceCount, dgMeshTreeCSGFace** const backFace, dgInt32 backFaceID) const
{
	MergeVertex(pointsPool, backFaceCount, backFace);
	MergeVertex(pointsPool, frontFaceCount, frontFace);

	polygon.BeginPolygon();
	AddFaces(polygon, backFaceCount, backFace, backFaceID, pointsPool);
	AddFaces(polygon, frontFaceCount, frontFace, frontFaceID, pointsPool);
	polygon.EndPolygon();

	_ASSERTE (polygon.GetCount());

	polygon.ConvertToPolygons();

#ifdef _DEBUG
	dgInt32 mark = polygon.IncLRU();
	dgInt32 outerEdgeCount = 0;
	dgMeshEffect::Iterator iter (polygon);
	for (iter.Begin(); iter; iter ++) {
		dgEdge* const edge = &(*iter);
		if ((edge->m_incidentFace < 0) && ((edge->m_mark != mark))){
			outerEdgeCount ++;
			dgEdge* ptr = edge;
			do {
				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr != edge);
			
		} 
	}
	_ASSERTE (outerEdgeCount == 1);
#endif
}

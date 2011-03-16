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
#include "dgWorld.h"
#include "dgMeshEffect.h"
//#include "dgCollisionConvexHull.h"
#include "dgMeshEffectSolidTree.h"






void dgMeshEffectSolidTree::BuildPlane (const dgMeshEffect& mesh, dgEdge* const face, dgHugeVector& normal, dgHugeVector& point) const
{
	point = dgHugeVector(dgBigVector (mesh.m_points[face->m_incidentVertex]));

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
	dgBigVector normal (mesh.FaceNormal (face, &mesh.m_points[0][0], sizeof (dgVector)));
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
_ASSERTE (0);

			if ((minDist.GetAproximateValue() < dgFloat64 (0.0f)) && (maxDist.GetAproximateValue() > dgFloat64 (0.0f))) {
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



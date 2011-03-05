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
#include "dgCollisionBox.h"
#include "dgCollisionMesh.h"
#include "dgMinkowskiConv.h"
#include "dgCollisionConvex.h"
#include "dgCollisionSphere.h"
#include "dgCollisionEllipse.h"
#include "dgCollisionCapsule.h"
#include "dgWorldDynamicUpdate.h"




void dgContactSolver::CalculateVelocitiesSimd (dgFloat32 timestep) 
{
	dgVector refOmega;
	dgVector floatOmega;

	m_referenceBody->CalculateContinueVelocitySimd(timestep, m_referenceBodyVeloc, refOmega);
	m_floatingBody->CalculateContinueVelocitySimd (timestep, m_floatingBodyVeloc, floatOmega);
	simd_128 vRel ((simd_128&) m_floatingBodyVeloc - (simd_128&)m_referenceBodyVeloc);
	m_localRelVeloc = m_proxy->m_referenceMatrix.UnrotateVectorSimd(vRel);
}


void dgContactSolver::CalcSupportVertexSimd (const dgVector& dir, dgInt32 entry)
{
	_ASSERTE ((dir % dir) > dgFloat32 (0.999f));
	dgVector p (m_referenceCollision->SupportVertexSimd (dir));
	dgVector dir1 (m_matrix.UnrotateVectorSimd(simd_128(dgFloat32 (-1.0f)) * (simd_128&)dir));
	dgVector q (m_matrix.TransformVectorSimd(m_floatingcollision->SupportVertexSimd (dir1)));
	(simd_128&)m_hullVertex[entry] = (simd_128&)p - (simd_128&)q;
	(simd_128&)m_averVertex[entry] = (simd_128&)p + (simd_128&)q;
}




dgContactSolver::dgMinkReturnCode dgContactSolver::CalcSeparatingPlaneSimd(dgMinkFace*& plane, const dgVector& origin)
{
	dgInt32 best;
	dgFloat32 maxErr;
	dgFloat32 error2;
	dgVector e1;
	dgVector e2;
	dgVector e3;
	dgVector normal (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));

	CalcSupportVertexSimd (m_dir[0], 0);

	dgInt32 i = 1;
	for (; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
		CalcSupportVertexSimd (m_dir[i], 1);
		e1 = m_hullVertex[1] - m_hullVertex[0];
		error2 = e1 % e1;
		if (error2 > DG_CALCULATE_SEPARATING_PLANE_ERROR) {
			break;
		}
	}

	for (i ++; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
		CalcSupportVertexSimd (m_dir[i], 2);
		e2 = m_hullVertex[2] - m_hullVertex[0];
		normal = e1 * e2;
		error2 = normal % normal;
		if (error2 > DG_CALCULATE_SEPARATING_PLANE_ERROR1) {
			break;
		}
	}

	error2 = dgFloat32 (0.0f);
	for (i ++; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
		CalcSupportVertexSimd (m_dir[i], 3);

		e3 = m_hullVertex[3] - m_hullVertex[0];
		error2 = normal % e3;
		if (dgAbsf (error2) > DG_CALCULATE_SEPARATING_PLANE_ERROR1) {
			break;
		}
	}

	if (i >= dgInt32(sizeof(m_dir) / sizeof(m_dir[0]))) {
		best = 0;
		maxErr = dgFloat32 (0.0f);
		for (i = 1; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
			CalcSupportVertexSimd (m_dir[i], 1);
			e1 = m_hullVertex[1] - m_hullVertex[0];
			error2 = e1 % e1;
			if (error2 > maxErr) {
				best = i;
				maxErr = error2;
			}
		}
		CalcSupportVertexSimd (m_dir[best], 1);
		e1 = m_hullVertex[1] - m_hullVertex[0];

		best = 0;
		maxErr = dgFloat32 (0.0f);
		for (i = 1; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
			CalcSupportVertexSimd (m_dir[i], 2);
			e2 = m_hullVertex[2] - m_hullVertex[0];
			normal = e1 * e2;
			error2 = normal % normal;
			if (error2 > maxErr) {
				best = i;
				maxErr = error2;
			}
		}

		CalcSupportVertexSimd (m_dir[best], 2);
		e2 = m_hullVertex[2] - m_hullVertex[0];
		normal = e1 * e2;

		best = 0;
		maxErr = dgFloat32 (0.0f);
		for (i = 1; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
			CalcSupportVertexSimd (m_dir[i], 3);

			e3 = m_hullVertex[3] - m_hullVertex[0];
			error2 = normal % e3;
			if (dgAbsf (error2) > dgAbsf (maxErr)) {
				best = i;
				maxErr = error2;
			}
		}
		error2 = maxErr;
		CalcSupportVertexSimd (m_dir[best], 3);
	}


	m_vertexIndex = 4;
	if (error2 > dgFloat32 (0.0f)) {
		Swap (m_hullVertex[1], m_hullVertex[2]);
		Swap (m_averVertex[1], m_averVertex[2]);
	}

	_ASSERTE (CheckTetraHedronVolume ());

	_ASSERTE ( (((dgUnsigned64)&m_simplex[0]) & 0x0f)== 0);
	_ASSERTE ( (((dgUnsigned64)&m_simplex[1]) & 0x0f)== 0);

	// face 0
	m_simplex[0].m_vertex[0] = 0;
	m_simplex[0].m_vertex[1] = 1;
	m_simplex[0].m_vertex[2] = 2;
	m_simplex[0].m_vertex[3] = 0;
	m_simplex[0].m_adjancentFace[0] = 1;	
	m_simplex[0].m_adjancentFace[1] = 3;	
	m_simplex[0].m_adjancentFace[2] = 2;	

	// face 1
	m_simplex[1].m_vertex[0] = 1;
	m_simplex[1].m_vertex[1] = 0;
	m_simplex[1].m_vertex[2] = 3;
	m_simplex[1].m_vertex[3] = 1;
	m_simplex[1].m_adjancentFace[0] = 0;	
	m_simplex[1].m_adjancentFace[1] = 2;	
	m_simplex[1].m_adjancentFace[2] = 3;	

	// face 2
	m_simplex[2].m_vertex[0] = 0;
	m_simplex[2].m_vertex[1] = 2;
	m_simplex[2].m_vertex[2] = 3;
	m_simplex[2].m_vertex[3] = 0;
	m_simplex[2].m_adjancentFace[0] = 0;	
	m_simplex[2].m_adjancentFace[1] = 3;	
	m_simplex[2].m_adjancentFace[2] = 1;	

	// face 3
	m_simplex[3].m_vertex[0] = 2;
	m_simplex[3].m_vertex[1] = 1;
	m_simplex[3].m_vertex[2] = 3;
	m_simplex[3].m_vertex[3] = 2;
	m_simplex[3].m_adjancentFace[0] = 0;	
	m_simplex[3].m_adjancentFace[1] = 1;	
	m_simplex[3].m_adjancentFace[2] = 2;	

	return UpdateSeparatingPlaneSimd(plane, origin);
}


dgContactSolver::dgMinkFace* dgContactSolver::CalculateClipPlaneSimd ()
{
	dgInt32 i;
	dgInt32 i0;
	dgInt32 i1;
	dgInt32 i2;
	dgInt32 stack;
	dgInt32 cicling;
	dgInt32 deadCount;
	dgInt32 adjacentIndex;
	dgInt32 prevEdgeIndex;
	dgInt32 silhouetteCapCount;
	dgInt32 lastSilhouetteVertex;

	dgFloat32 dist;
	dgFloat32 minValue;
	dgFloat32 penetration;
	dgFloat32 ciclingMem[4];
	dgMinkFace *face;
	dgMinkFace *adjacent;
	dgMinkFace *prevFace;
	dgMinkFace *firstFace;
	dgMinkFace *silhouette;
	dgMinkFace *lastSilhouette;
	dgMinkFace *closestFace;
	dgMinkFacePurge* nextPurge;
	dgMinkFace *stackPool[128];
	dgMinkFace *deadFaces[128];
	SilhouetteFaceCap sillueteCap[128];
	dgVector diff[3];
	dgVector aver[3];
	dgInt8  buffer[DG_HEAP_EDGE_COUNT * (sizeof (dgFloat32) + sizeof (dgMinkFace *))];
	dgClosestFace heapSort (buffer, sizeof (buffer));

	m_planeIndex = 4;
	closestFace = NULL;
	m_facePurge = NULL;
	penetration = dgFloat32 (0.0f);


	_ASSERTE (m_vertexIndex == 4);
	for (i = 0; i < 4; i ++) {
		face = &m_simplex[i];
		face->m_inHeap = 0;
		face->m_isActive = 1;
		if (CalcFacePlaneSimd (face)) {
			face->m_inHeap = 1;
			heapSort.Push(face, face->m_w);
		}
	}

	cicling = 0;
	ciclingMem[0] = dgFloat32 (1.0e10f);
	ciclingMem[1] = dgFloat32 (1.0e10f);
	ciclingMem[2] = dgFloat32 (1.0e10f);
	ciclingMem[3] = dgFloat32 (1.0e10f);

	minValue = dgFloat32 ( 1.0e10f);
	dgPlane bestPlane (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f),dgFloat32 (0.0f));
	diff[0] = bestPlane;
	diff[1] = bestPlane;
	diff[2] = bestPlane;
	aver[0] = bestPlane;
	aver[1] = bestPlane;
	aver[2] = bestPlane;

	while (heapSort.GetCount()) {
		face = heapSort[0];
		face->m_inHeap = 0;
		heapSort.Pop();

		if (face->m_isActive) {
			const dgPlane& plane = *face;

			CalcSupportVertexSimd (plane, m_vertexIndex);
			const dgVector& p = m_hullVertex[m_vertexIndex];
			dist = plane.Evalue (p);
			m_vertexIndex ++;

			if (m_vertexIndex > 16) {
				if (dist < minValue) {
					if (dist >= dgFloat32 (0.0f)) {
						minValue = dist;
						bestPlane = plane;

						i = face->m_vertex[0];
						diff[0] = m_hullVertex[i];
						aver[0] = m_averVertex[i];

						i = face->m_vertex[1];
						diff[1] = m_hullVertex[i];
						aver[1] = m_averVertex[i];

						i = face->m_vertex[2];
						diff[2] = m_hullVertex[i];
						aver[2] = m_averVertex[i];
					}
				}
			}

			ciclingMem[cicling] = dist;
			cicling = (cicling + 1) & 3;
			for (i = 0; i < 4; i ++) {
				if (dgAbsf (dist - ciclingMem[i]) > dgFloat32 (1.0e-6f)) {
					break;
				}
			}
			if (i == 4) {
				dist = dgFloat32 (0.0f);
			}


			if ((m_vertexIndex > DG_MINK_MAX_POINTS) ||
				(m_planeIndex > DG_MINK_MAX_FACES) ||
				(heapSort.GetCount() > (DG_HEAP_EDGE_COUNT - 24))) {

					//						dgTrace (("Max face count overflow, breaking with last best face\n"));

					dgPlane& plane = *face;
					plane = bestPlane;

					i = face->m_vertex[0];
					face->m_vertex[0] = 0;
					m_hullVertex[i] = diff[0];
					m_averVertex[i] = aver[0];

					i = face->m_vertex[1];
					face->m_vertex[1] = 1;
					m_hullVertex[i] = diff[1];
					m_averVertex[i] = aver[1];

					i = face->m_vertex[2];
					face->m_vertex[2] = 2;
					m_hullVertex[i] = diff[2];
					m_averVertex[i] = aver[2];
					dist = dgFloat32 (0.0f);
			}

			if (dist < (dgFloat32 (DG_IMPULSIVE_CONTACT_PENETRATION) / dgFloat32 (16.0f))) {
				_ASSERTE (m_planeIndex <= DG_MINK_MAX_FACES_SIZE);
				_ASSERTE (heapSort.GetCount() <= DG_HEAP_EDGE_COUNT);
				closestFace = face;
				break;
			} else if (dist > dgFloat32 (0.0f)) {
				_ASSERTE (face->m_inHeap == 0);

				stack = 0;
				deadCount = 1;
				silhouette = NULL;
				deadFaces[0] = face;
				closestFace = face;
				face->m_isActive = 0;
				for (i = 0; i < 3; i ++) {
					adjacent = &m_simplex[face->m_adjancentFace[i]];
					_ASSERTE (adjacent->m_isActive);
					dist = adjacent->Evalue (p);  
					if (dist > dgFloat32 (0.0f)) { 
						adjacent->m_isActive = 0;
						stackPool[stack] = adjacent;
						deadFaces[deadCount] = adjacent;
						stack ++;
						deadCount ++;
					} else {
						silhouette = adjacent;
					}
				}
				while (stack) {
					stack --;
					face = stackPool[stack];
					for (i = 0; i < 3; i ++) {
						adjacent = &m_simplex[face->m_adjancentFace[i]];
						if (adjacent->m_isActive){
							dist = adjacent->Evalue (p);  
							if (dist > dgFloat32 (0.0f)) { 
								adjacent->m_isActive = 0;
								stackPool[stack] = adjacent;
								deadFaces[deadCount] = adjacent;
								stack ++;
								deadCount ++;
								_ASSERTE (stack < sizeof (stackPool) / sizeof (stackPool[0]));
								_ASSERTE (deadCount < sizeof (deadFaces) / sizeof (deadFaces[0]));

							} else {
								silhouette = adjacent;
							}
						}
					}
				}

				if (!silhouette) {
					closestFace = face;
					break;
				}


				// build silhouette						
				_ASSERTE (silhouette);
				_ASSERTE (silhouette->m_isActive);

				i2 = (m_vertexIndex - 1);
				lastSilhouette = silhouette;
				//					_ASSERTE ( (((!m_simplex[silhouette->m_adjancentFace[0]].m_isActive) + 
				//						(!m_simplex[silhouette->m_adjancentFace[1]].m_isActive) +
				//						(!m_simplex[silhouette->m_adjancentFace[2]].m_isActive)) == 1) ||
				//						(((!m_simplex[silhouette->m_adjancentFace[0]].m_isActive) + 
				//						(!m_simplex[silhouette->m_adjancentFace[1]].m_isActive) +
				//						(!m_simplex[silhouette->m_adjancentFace[2]].m_isActive)) == 2));
				_ASSERTE ((silhouette->m_adjancentFace[0] != silhouette->m_adjancentFace[1]) &&
					(silhouette->m_adjancentFace[0] != silhouette->m_adjancentFace[2]) &&
					(silhouette->m_adjancentFace[1] != silhouette->m_adjancentFace[2]));


				adjacentIndex = DG_GETADJACENTINDEX_ACTIVE (silhouette);
				face = NewFace();
				i0 = silhouette->m_vertex[adjacentIndex];
				i1 = silhouette->m_vertex[adjacentIndex + 1];

				face->m_vertex[0] = dgInt16 (i1);
				face->m_vertex[1] = dgInt16 (i0);
				face->m_vertex[2] = dgInt16 (i2);
				face->m_vertex[3] = face->m_vertex[0];
				face->m_adjancentFace[0] = dgInt16 (silhouette - m_simplex);
				face->m_inHeap = 0; 
				face->m_isActive = 1; 

				sillueteCap[0].m_face = face;
				sillueteCap[0].m_faceCopling = &silhouette->m_adjancentFace[adjacentIndex];
				silhouetteCapCount = 1;
				_ASSERTE (silhouetteCapCount < (sizeof (sillueteCap) / sizeof (sillueteCap[0])));
				do {
					silhouette = &m_simplex[silhouette->m_adjancentFace[adjacentIndex]];
					adjacentIndex = (DG_GETADJACENTINDEX_VERTEX(silhouette, i0)); 
				} while (!silhouette->m_isActive);

				prevFace = face;
				firstFace = face;
				lastSilhouetteVertex = i0;
				prevEdgeIndex = dgInt32 (face - m_simplex);
				do {
					//						_ASSERTE ( (((!m_simplex[silhouette->m_adjancentFace[0]].m_isActive) + 
					//							(!m_simplex[silhouette->m_adjancentFace[1]].m_isActive) +
					//							(!m_simplex[silhouette->m_adjancentFace[2]].m_isActive)) == 1) ||
					//							(((!m_simplex[silhouette->m_adjancentFace[0]].m_isActive) + 
					//							(!m_simplex[silhouette->m_adjancentFace[1]].m_isActive) +
					//							(!m_simplex[silhouette->m_adjancentFace[2]].m_isActive)) == 2));
					_ASSERTE ((silhouette->m_adjancentFace[0] != silhouette->m_adjancentFace[1]) &&
						(silhouette->m_adjancentFace[0] != silhouette->m_adjancentFace[2]) &&
						(silhouette->m_adjancentFace[1] != silhouette->m_adjancentFace[2]));


					adjacentIndex = adjacentIndex ? adjacentIndex - 1 : 2;

					face = NewFace();
					i0 = silhouette->m_vertex[adjacentIndex];
					i1 = silhouette->m_vertex[adjacentIndex + 1];

					face->m_vertex[0] = dgInt16 (i1);
					face->m_vertex[1] = dgInt16 (i0);
					face->m_vertex[2] = dgInt16 (i2);
					face->m_vertex[3] = face->m_vertex[0];
					face->m_adjancentFace[0] = dgInt16 (silhouette - m_simplex);
					face->m_adjancentFace[2] = dgInt16 (prevEdgeIndex);
					face->m_inHeap = 0; 
					face->m_isActive = 1; 

					prevEdgeIndex = dgInt32 (face - m_simplex);
					prevFace->m_adjancentFace[1] = dgInt16 (prevEdgeIndex);
					prevFace = face;

					sillueteCap[silhouetteCapCount].m_face = face;
					sillueteCap[silhouetteCapCount].m_faceCopling = &silhouette->m_adjancentFace[adjacentIndex];
					silhouetteCapCount ++;
					_ASSERTE (silhouetteCapCount < (sizeof (sillueteCap) / sizeof (sillueteCap[0])));

					do {
						silhouette = &m_simplex[silhouette->m_adjancentFace[adjacentIndex]];
						adjacentIndex = (DG_GETADJACENTINDEX_VERTEX(silhouette, i0)); 
					} while (!silhouette->m_isActive);

				} while ((silhouette != lastSilhouette) || (silhouette->m_vertex[adjacentIndex ? adjacentIndex - 1 : 2] != lastSilhouetteVertex));
				firstFace->m_adjancentFace[2] = dgInt16 (prevEdgeIndex);
				prevFace->m_adjancentFace[1] = dgInt16 (firstFace - m_simplex);

				for (i = 0; i < deadCount; i ++) {
					if (!deadFaces[i]->m_inHeap){
						nextPurge = (dgMinkFacePurge*) deadFaces[i];
						nextPurge->m_next = m_facePurge;
						m_facePurge = nextPurge;
					}
				}

				while (heapSort.GetCount() && (!heapSort[0]->m_isActive)) {
					face = heapSort[0];
					heapSort.Pop();
					nextPurge = (dgMinkFacePurge*) face;
					nextPurge->m_next = m_facePurge;
					m_facePurge = nextPurge;
				}

				for (i = 0; i < silhouetteCapCount; i ++) {
					face = sillueteCap[i].m_face;
					*sillueteCap[i].m_faceCopling = dgInt16 (face - m_simplex);

					if (CalcFacePlaneSimd (face)) {
						face->m_inHeap = 1;
						heapSort.Push(face, face->m_w);
					}
				}
			}
		} else {
			_ASSERTE (0);
			nextPurge = (dgMinkFacePurge*) face;
			nextPurge->m_next = m_facePurge;
			m_facePurge = nextPurge;
		}
	}
	return closestFace;
}


dgInt32 dgContactSolver::HullHullContinueContactsSimd (dgFloat32& timeStep, dgContactPoint* const contactOut, dgInt32 contactID, dgInt32 maxContacts, dgInt32 conditionalContactCalculationAtOrigin)
{
	dgInt32 count;
	dgMinkFace *face;
	dgMinkReturnCode code;

	count = 0;
	code = CalcSeparatingPlaneSimd(face);

	m_lastFaceCode = code;
	if (code == dgMinkIntersecting) {

		if (m_proxy->m_isTriggerVolume) {
			timeStep = dgFloat32 (0.0f);
			m_proxy->m_inTriggerVolume = 1;
			count = 0;
		} else {
			face = CalculateClipPlaneSimd();
			if (face) {
				if (conditionalContactCalculationAtOrigin) {
					dgFloat32 projVeloc;
					const dgPlane& plane = *face;
					projVeloc = plane.DotProductSimd(m_localRelVeloc);
					if (projVeloc >= dgFloat32 (0.0f)) {
						return 0;
					}
				}
				if (maxContacts) {
					count = CalculateContactsSimd (face, contactID, contactOut, maxContacts);
					_ASSERTE (count <= maxContacts);
				}

				timeStep = dgFloat32 (0.0f);
			}
		}
	} else if (code == dgMinkDisjoint) {
		dgInt32 i0;
		dgInt32 i1;
		dgInt32 i2;
		dgFloat32 t0;
		dgFloat32 projVeloc;
		dgFloat32 timeOfImpact;
		dgVector saveHull[3];
		dgVector saveAver[3];

		_ASSERTE (face);
		t0 = dgFloat32 (0.0f);
		i0 = face->m_vertex[0];
		i1 = face->m_vertex[1];
		i2 = face->m_vertex[2];
		dgVector plane ((m_hullVertex[i1] - m_hullVertex[i0]) * (m_hullVertex[i2] - m_hullVertex[i0]));
		_ASSERTE (plane % plane > dgFloat32 (0.0f));
		//			_ASSERTE (dgAbsf (plane % vRel) > dgFloat32 (0.0f));
		projVeloc = plane.DotProductSimd(m_localRelVeloc);
		if (projVeloc >= dgFloat32 (-1.0e-24f)) {
			code = UpdateSeparatingPlaneSimd(face, dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f)));
			if (code != dgMinkDisjoint) {
				return 0;
			}
			_ASSERTE (code == dgMinkDisjoint);

			i0 = face->m_vertex[0];
			i1 = face->m_vertex[1];
			i2 = face->m_vertex[2];
			plane = (m_hullVertex[i1] - m_hullVertex[i0]) * (m_hullVertex[i2] - m_hullVertex[i0]);
			_ASSERTE (plane % plane > dgFloat32 (0.0f));
			projVeloc = plane.DotProductSimd(m_localRelVeloc);
			if (projVeloc > dgFloat32 (-1.0e-24f)) {
				//_ASSERTE ((plane % m_localRelVeloc) < dgFloat32 (0.0f));
				return 0;
			}

		}

		//			timeOfImpact = - plane.m_w / (plane % vRel + dgFloat32 (1.0e-24f));
		//			timeOfImpact = (plane % m_hullVertex[i0]) / (plane % m_localRelVeloc + dgFloat32 (1.0e-24f));
		timeOfImpact = (plane.DotProductSimd (m_hullVertex[i0])) / (plane.DotProductSimd (m_localRelVeloc) + dgFloat32 (1.0e-24f));
		if (timeOfImpact > 0.0f) {
			saveHull[0] = m_hullVertex[i0];
			saveHull[1] = m_hullVertex[i1];
			saveHull[2] = m_hullVertex[i2];
			saveAver[0] = m_averVertex[i0];
			saveAver[1] = m_averVertex[i1];
			saveAver[2] = m_averVertex[i2];
			dgVector p1 (m_localRelVeloc.Scale (timeOfImpact));

			for (dgInt32 maxPasses = 0; (maxPasses < 32) && (timeOfImpact < timeStep) && (timeOfImpact > t0); maxPasses ++) {
				dgMinkFace *tmpFaceface;
				t0 = timeOfImpact;
				code = UpdateSeparatingPlaneSimd(tmpFaceface, p1);
				_ASSERTE (code != dgMinkError);
				if (code == dgMinkDisjoint) {
					dgFloat32 den;

					_ASSERTE (tmpFaceface);

					face = tmpFaceface;
					i0 = face->m_vertex[0];
					i1 = face->m_vertex[1];
					i2 = face->m_vertex[2];
					//dgPlane plane (m_hullVertex[i0], m_hullVertex[i1], m_hullVertex[i2]);
					dgVector plane ((m_hullVertex[i1] - m_hullVertex[i0]) * (m_hullVertex[i2] - m_hullVertex[i0]));
					_ASSERTE (plane % plane > dgFloat32 (0.0f));

					//						den = plane % m_localRelVeloc ;
					den = plane.DotProductSimd(m_localRelVeloc);
					if (den >= dgFloat32 (-1.0e-24f)) {
						code = UpdateSeparatingPlaneSimd(tmpFaceface, p1);
						_ASSERTE (code == dgMinkDisjoint);

						i0 = face->m_vertex[0];
						i1 = face->m_vertex[1];
						i2 = face->m_vertex[2];
						//dgPlane plane (m_hullVertex[i0], m_hullVertex[i1], m_hullVertex[i2]);
						dgVector plane ((m_hullVertex[i1] - m_hullVertex[i0]) * (m_hullVertex[i2] - m_hullVertex[i0]));
						_ASSERTE (plane % plane > dgFloat32 (0.0f));
						//							den = plane % m_localRelVeloc;
						den = plane.DotProductSimd(m_localRelVeloc);
						if (den > dgFloat32 (-1.0e-24f)) {
							return 0;
						}
					}

					saveHull[0] = m_hullVertex[i0];
					saveHull[1] = m_hullVertex[i1];
					saveHull[2] = m_hullVertex[i2];
					saveAver[0] = m_averVertex[i0];
					saveAver[1] = m_averVertex[i1];
					saveAver[2] = m_averVertex[i2];
					if (den < dgFloat32 (-1.0e-24f)) {
						//							timeOfImpact = (plane % m_hullVertex[i0]) / den;
						timeOfImpact = (plane.DotProductSimd(m_hullVertex[i0])) / den;
						if (timeOfImpact < 0.0f) {
							return 0;
						}
						_ASSERTE (timeOfImpact >= 0.0f);
						//_ASSERTE (timeOfImpact >= dgFloat32 (-1.0f));
						p1 = m_localRelVeloc.Scale (timeOfImpact);
					}
				}
			}

			if ((timeOfImpact <= timeStep) && (timeOfImpact >= dgFloat32 (0.0f))) {

				if (maxContacts) {
					count = CalculateContactsContinuesSimd(contactID, contactOut, maxContacts, saveHull, saveAver, timeOfImpact);
					_ASSERTE(count <= maxContacts);
				}

				if (m_proxy->m_isTriggerVolume) {
					m_proxy->m_inTriggerVolume = 1;
					count = 0;
				}

				timeStep = timeOfImpact;
				if (count) {
					//dgVector step (vRel.Scale (- timeOfImpact * 0.5f));
					//dgVector step (refVeloc.Scale (timeOfImpact));
					dgVector step (m_referenceBodyVeloc.Scale (timeOfImpact));
					for (i0 = 0; i0 < count; i0 ++) {
						//							contactOut[i0].m_point += step;
						(simd_type&)contactOut[i0].m_point = simd_add_v ((simd_type&)contactOut[i0].m_point, (simd_type&)step);
					}
				}
			}
		}
	}
	return count;
}

dgInt32 dgContactSolver::CalculateContactsSimd(dgMinkFace* const face, dgInt32 contacID, dgContactPoint* const contactOut, dgInt32 maxContacts)
{
	dgInt32 count = 0;
	// Get the contact form the last face
	const dgPlane& plane = *face;
	dgFloat32 penetration = plane.m_w - m_penetrationPadding;
	dgFloat32 dist = (plane % m_averVertex[face->m_vertex[0]]) * dgFloat32 (0.5f);
	const dgPlane clipPlane (plane.Scale(dgFloat32 (-1.0f)), dist);

	dgVector point1 (clipPlane.Scale (-clipPlane.m_w));
	dgVector* const shape1 = m_hullVertex;
	dgInt32 count1 = m_referenceCollision->CalculatePlaneIntersectionSimd (clipPlane, point1, shape1);
	if (!count1) {
		dgVector p1 (m_referenceCollision->SupportVertexSimd (clipPlane.Scale (dgFloat32 (-1.0f))));
		p1 += clipPlane.Scale (DG_ROBUST_PLANE_CLIP);
		count1 = m_referenceCollision->CalculatePlaneIntersectionSimd (clipPlane, p1, shape1);
		dgVector err (clipPlane.Scale (clipPlane % (point1 - p1)));
		for (dgInt32 i = 0; i < count1; i ++) {
			shape1[i] += err;
		}
	}

	_ASSERTE (penetration <= dgFloat32 (2.0e-1f));
	dist = GetMax (- (penetration + DG_IMPULSIVE_CONTACT_PENETRATION), dgFloat32 (0.0f));
	if (count1) {
		dgVector* const shape2 = &m_hullVertex[count1];
		const dgPlane clipPlane2 (m_matrix.UntransformPlane(clipPlane));

		dgVector point2 (clipPlane2.Scale (-clipPlane2.m_w));
		dgInt32 count2 = m_floatingcollision->CalculatePlaneIntersectionSimd (clipPlane2, point2, shape2);
		if (!count2) {
			dgVector p2 (m_floatingcollision->SupportVertexSimd (clipPlane2.Scale(dgFloat32 (-1.0f))));
			p2 += clipPlane2.Scale (DG_ROBUST_PLANE_CLIP);
			count2 = m_floatingcollision->CalculatePlaneIntersectionSimd (clipPlane2, p2, shape2);
			dgVector err (clipPlane2.Scale (clipPlane2 % (point2 - p2)));
			for (dgInt32 i = 0; i < count2; i ++) {
				shape2[i] += err;
			}
		}

		if (count2) {

			_ASSERTE (count1);
			_ASSERTE (count2);
			if (count1 == 1) {
				const dgMatrix& matrix1 = m_proxy->m_referenceMatrix;

				count = 1;
				contactOut[0].m_point = matrix1.TransformVectorSimd (shape1[0]);
				contactOut[0].m_normal = matrix1.RotateVectorSimd (clipPlane);
				contactOut[0].m_userId = contacID;
				contactOut[0].m_penetration = dist;
			} else if (count2 == 1) {
				const dgMatrix& matrix1 = m_proxy->m_referenceMatrix;
				//const dgMatrix& matrix2 = m_proxy->m_floatingMatrix;

				count = 1;
				//contactOut[0].m_point = matrix2.TransformVectorSimd (shape2[0]);
				contactOut[0].m_point =  matrix1.TransformVectorSimd (m_matrix.TransformVectorSimd(shape2[0]));
				contactOut[0].m_normal = matrix1.RotateVectorSimd (clipPlane);
				contactOut[0].m_userId = contacID;
				contactOut[0].m_penetration = dist;

			} else if ((count1 == 2) && (count2 == 2)) {

				const dgMatrix& matrix1 = m_proxy->m_referenceMatrix;
				dgVector p0 (shape1[0]); 
				dgVector p1 (shape1[1]); 
				dgVector q0 (m_matrix.TransformVectorSimd(shape2[0])); 
				dgVector q1 (m_matrix.TransformVectorSimd(shape2[1])); 
				dgVector p10 (p1 - p0);
				dgVector q10 (q1 - q0);
				p10 = p10.Scale (dgFloat32 (1.0f) / dgSqrt (p10 % p10 + dgFloat32 (1.0e-8f)));
				q10 = q10.Scale (dgFloat32 (1.0f) / dgSqrt (q10 % q10 + dgFloat32 (1.0e-8f)));
				dgFloat32 dot = q10 % p10;
				if (dgAbsf (dot) > dgFloat32 (0.998f)) {
					dgFloat32 pl0 = p0 % p10;
					dgFloat32 pl1 = p1 % p10;
					dgFloat32 ql0 = q0 % p10;
					dgFloat32 ql1 = q1 % p10;
					if (pl0 > pl1) {
						Swap (pl0, pl1);
						Swap (p0, p1);
						p10 = p10.Scale (dgFloat32 (-1.0f));
					}
					if (ql0 > ql1) {
						Swap (ql0, ql1);
					}
					if ( !((ql0 > pl1) && (ql1 < pl0))) {
						dgFloat32 clip0 = (ql0 > pl0) ? ql0 : pl0;
						dgFloat32 clip1 = (ql1 < pl1) ? ql1 : pl1;

						count = 2;
						contactOut[0].m_point = p0 + p10.Scale (clip0 - pl0);
						contactOut[0].m_normal = matrix1.RotateVector (clipPlane);
						contactOut[0].m_userId = contacID;
						contactOut[0].m_penetration = dist;

						contactOut[1].m_point = p0 + p10.Scale (clip1 - pl0);
						contactOut[1].m_normal = matrix1.RotateVector (clipPlane);
						contactOut[1].m_userId = contacID;
						contactOut[1].m_penetration = dist;
					}

				} else {
					dgVector c0;
					dgVector c1;
					count = 1;
					dgRayToRayDistance (p0, p1, q0, q1, c0, c1);
					contactOut[0].m_point = (c0 + c1).Scale (dgFloat32 (0.5f));
					contactOut[0].m_normal = matrix1.RotateVector (clipPlane);
					contactOut[0].m_userId = contacID;
					contactOut[0].m_penetration = dist;
				}
				if (count > maxContacts) {
					count = maxContacts;
				}
				for (dgInt32 i = 0; i < count; i ++) {
					contactOut[i].m_point = matrix1.TransformVectorSimd(contactOut[i].m_point);
				}

			} else {
				const dgMatrix& matrix1 = m_proxy->m_referenceMatrix;
				m_matrix.TransformVectorsSimd(shape2, shape2, count2);
				count = CalculateConvexShapeIntersectionSimd (matrix1, clipPlane, dgUnsigned32 (contacID), dist, count1, shape1, count2, shape2, contactOut, maxContacts);
				if (!count) {
					count = CalculateContactAlternateMethod(face, contacID, contactOut, maxContacts);
				}
			}
			//_ASSERTE (count);
		}

	}

	return count;
}


dgInt32 dgContactSolver::CalculateContactsContinuesSimd(dgInt32 contacID, dgContactPoint* const contactOut, dgInt32 maxContacts, const dgVector* diffPoins, const dgVector* averPoins, dgFloat32 timestep)
{
_ASSERTE (0);
return 0;
/*
	//dgInt32 i;
	//dgInt32 count;
	//dgMinkFace *face; 
	simd_type invMag; 

	//		m_planePurge = NULL;
	//		m_facePlaneIndex = 0;
	dgMinkFace* const face = &m_simplex[0];

	//		dgVector step (veloc.Scale (timestep + DG_ROBUST_PLANE_CLIP * dgRsqrt(veloc % veloc)));
	DG_RSQRT_SIMD_S(simd_set1(m_localRelVeloc.DotProductSimd(m_localRelVeloc)), invMag)
		invMag = simd_mul_add_s (simd_set1(timestep), simd_set1(DG_ROBUST_PLANE_CLIP), invMag);
	dgVector step (simd_mul_v((simd_type&) m_localRelVeloc, simd_permut_v (invMag, invMag, PURMUT_MASK(0, 0, 0, 0))));


	for (dgInt32 i = 0; i < 3; i ++) {
		//			m_hullVertex[i] = diffPoins[i] - step;
		(simd_type&)m_hullVertex[i] = simd_sub_v ((simd_type&)diffPoins[i], (simd_type&)step);

		//			m_averVertex[i] = averPoins[i] + step;
		(simd_type&)m_averVertex[i] = simd_add_v ((simd_type&)averPoins[i], (simd_type&)step);
	}

	CalcFacePlaneSimd (face);
	dgPlane &facePlane  = *face;
	if ((facePlane % m_localRelVeloc) > dgFloat32 (0.0f)) {
		facePlane = facePlane.Scale (dgFloat32 (-1.0f));
	}

	dgVector minkFloatingPosit (m_matrix.m_posit );
	(simd_type&)m_matrix.m_posit = simd_add_v ((simd_type&)m_matrix.m_posit, (simd_type&)step);
	dgInt32 count = CalculateContactsSimd(face, contacID, contactOut, maxContacts);
	_ASSERTE (count <= maxContacts);

	m_matrix.m_posit = minkFloatingPosit;

	return count;
*/
}

dgInt32 dgContactSolver::HullHullContactsSimd (dgInt32 contactID)
{
	dgInt32 i0;
	dgInt32 count;
	dgMinkFace *face;
	dgMinkReturnCode code;
	dgContactPoint* contactOut;

	count = 0;
	m_proxy->m_inTriggerVolume = 0;
	code = CalcSeparatingPlaneSimd (face);
	switch (code)
	{
		case dgMinkIntersecting:
		{
			if (m_proxy->m_isTriggerVolume) {
				m_proxy->m_inTriggerVolume = 1;
			} else {
				face = CalculateClipPlaneSimd ();
				if (face) {
					count = CalculateContactsSimd (face, contactID, m_proxy->m_contacts, m_proxy->m_maxContacts);
					_ASSERTE (count <= m_proxy->m_maxContacts);
				}
			}
			break;
		}

		case dgMinkDisjoint:
		{
			_ASSERTE (face);
			if (CalcFacePlaneSimd (face)) {
				//_ASSERTE (face->m_w >= dgFloat32 (0.0f));
				_ASSERTE ((*face) % (*face) > dgFloat32 (0.0f));
				if (face->m_w < m_penetrationPadding) {
					dgVector step (*face);
					step = step.Scale (-(face->m_w + DG_IMPULSIVE_CONTACT_PENETRATION));

					i0 = face->m_vertex[0];
					m_hullVertex[i0] -= step;
					m_averVertex[i0] += step;

					m_matrix.m_posit += step;
					dgVector stepWorld (m_proxy->m_referenceMatrix.RotateVectorSimd(step));
					m_proxy->m_floatingMatrix.m_posit += stepWorld;

					count = CalculateContactsSimd (face, contactID, m_proxy->m_contacts, m_proxy->m_maxContacts);
					_ASSERTE (count < m_proxy->m_maxContacts);
					stepWorld = stepWorld.Scale (dgFloat32 (0.5f));

					if (m_proxy->m_isTriggerVolume) {
						m_proxy->m_inTriggerVolume = 1;
						count = 0;
					}

					contactOut = m_proxy->m_contacts; 
					for (i0 = 0; i0 < count; i0 ++ ) {
						//contactOut[i0].m_point -= stepWorld ;
						(simd_type&)contactOut[i0].m_point = simd_sub_v ((simd_type&)contactOut[i0].m_point, (simd_type&)stepWorld) ;
					}
					return count;
				}
			}
		}
		case dgMinkError:
		default:;

	}
	return count;
}

dgContactSolver::dgMinkReturnCode dgContactSolver::UpdateSeparatingPlaneSimd(dgMinkFace*& plane, const dgVector& origin)
{
//return UpdateSeparatingPlane(plane, origin);
	simd_128 diff[4];
	simd_128 aveg[4];

	plane = NULL;
	dgMinkFace* face = &m_simplex[0];
	dgMinkReturnCode code = dgMinkIntersecting;

	dgInt32 ciclingCount = -1;
	dgMinkFace* lastDescendFace = NULL;
//	dgFloat32 minDist = dgFloat32 (1.0e20f);

	dgInt32 j = 0;
	simd_128 zero (dgFloat32 (0.0f));
	simd_128 negOne(dgFloat32 (-1.0f));
	simd_128 minDist (dgFloat32 (1.0e20f));
	simd_128 distTolerance (DG_DISTANCE_TOLERANCE);
	simd_128 zeroTolerance (DG_DISTANCE_TOLERANCE_ZERO);
	simd_128 separatingTol (DG_UPDATE_SEPARATING_PLANE_DISTANCE_TOLERANCE1);
	simd_128 index_yx (dgFloat32 (0.0f), dgFloat32 (1.0f), dgFloat32 (0.0f), dgFloat32 (1.0f));
	simd_128 index_wz (dgFloat32 (2.0f), dgFloat32 (3.0f), dgFloat32 (2.0f), dgFloat32 (3.0f));
	for (; face && (j < DG_UPDATE_SEPARATING_PLANE_MAX_ITERATION); j ++) {
		face = NULL;
		simd_128 p0 ((simd_128&)m_hullVertex[0]);
		simd_128 p1 ((simd_128&)m_hullVertex[1]);
		simd_128 p2 ((simd_128&)m_hullVertex[2]);
		simd_128 p3 ((simd_128&)m_hullVertex[3]);
		
		simd_128 e10 (p1 - p0);
		simd_128 e20 (p2 - p0);
		simd_128 e30 (p3 - p0);
		simd_128 e12 (p1 - p2);
		simd_128 e32 (p3 - p2);

		simd_128 tmp;
		simd_128 e10_x;
		simd_128 e10_y;
		simd_128 e10_z;
		simd_128 e20_x;
		simd_128 e20_y;
		simd_128 e20_z;
		Transpose4x4Simd_128 (e10_x, e10_y, e10_z, tmp, e10, e30, e20, e12); 
		Transpose4x4Simd_128 (e20_x, e20_y, e20_z, tmp, e20, e10, e30, e32); 

		simd_128 nx (e10_y * e20_z - e10_z * e20_y);
		simd_128 ny (e10_z * e20_x - e10_x * e20_z);
		simd_128 nz (e10_x * e20_y - e10_y * e20_x);

		simd_128 dist2 (nx * nx + ny * ny + nz * nz);
		simd_128 mask (dist2 >= zeroTolerance);
		dist2 = dist2.GetMax(zeroTolerance);

		dist2  = dist2.InvRqrt();
		nx = nx * dist2;
		ny = ny * dist2;
		nz = nz * dist2;

		simd_128 origin_P0_xxxx;
		simd_128 origin_P0_yyyy;
		simd_128 origin_P0_zzzz;
		simd_128 origin_P0 ((simd_128&)origin - p0);
		simd_128 origin_P3 ((simd_128&)origin - p3);
		Transpose4x4Simd_128 (origin_P0_xxxx, origin_P0_yyyy, origin_P0_zzzz, tmp, origin_P0, origin_P0, origin_P0, origin_P3); 

		//dist2_ = simd_mul_add_v (simd_mul_add_v (simd_mul_v(nx_, origin_P0_xxxx), ny_, origin_P0_yyyy), nz_, origin_P0_zzzz);
		dist2 = origin_P0_xxxx * nx + origin_P0_yyyy * ny + origin_P0_zzzz * nz; 
		//dist2_ = simd_or_v (simd_and_v(dist2_, mask_), simd_andnot_v (m_negativeOne, mask_));
		dist2 = (dist2 & mask) | negOne.AndNot(mask);

		tmp = dist2.MoveHighToLow(dist2);
		mask = dist2 > tmp;
		dist2 = (dist2 & mask) | tmp.AndNot(mask);
		simd_128 index ((index_yx & mask) | index_wz.AndNot(mask));

		tmp = dist2.PackLow(dist2);
		tmp = tmp.MoveHighToLow(tmp);
		mask = dist2 > tmp;
		dist2 = (dist2 & mask) | tmp.AndNot(mask);
		tmp = index.PackLow(index);
		index = ((index & mask) | tmp.MoveHighToLow(tmp).AndNot(mask));

		mask = dist2 > zero;
		index = (index & mask) | negOne.AndNot(mask);
		dgInt32 faceIndex = index.GetInt();
		if (faceIndex >= 0) {
			simd_128 transposedNormals[4];
			face = &m_simplex[faceIndex];

			Transpose4x4Simd_128 (transposedNormals[0], transposedNormals[1], transposedNormals[2], transposedNormals[3], nx, ny, nz, zero); 
			dgVector normal (transposedNormals[faceIndex]);

			//i0 = face->m_vertex;
			dgInt32 index = face->m_vertex[0];
			CalcSupportVertexSimd (normal, 4);
			simd_128 dist (((simd_128&)normal).DotProduct((simd_128&)m_hullVertex[4] - (simd_128&)m_hullVertex[index]));

			// if we are doing too many passes it means that it is a skew shape 
			// increasing the tolerance help to resolve the problem
			if ((dist < separatingTol).GetSignMask() & 1) {
				plane = face;
				code = dgMinkDisjoint;
				break;
			}

			if ((dist < minDist).GetSignMask() & 1) {
				minDist = dist;
				lastDescendFace = face;
				ciclingCount = -1;
				for (dgInt32 k = 0; k < 4; k ++) {
					diff[k] = (simd_128&)m_hullVertex[k];
					aveg[k] = (simd_128&)m_averVertex[k];
				}
			}

			ciclingCount ++;
			if (ciclingCount > 4) {
				for (dgInt32 k = 0; k < 4; k ++) {
					(simd_128&)m_hullVertex[k] = diff[k];
					(simd_128&)m_averVertex[k] = aveg[k];
				}
				code = dgMinkDisjoint;
				plane = lastDescendFace;
				break;
			}

			if ((dist < distTolerance).GetSignMask() & 1) {
				dgInt32 i = 0;
				for (; i < 4; i ++ ) {
					dgVector error (m_hullVertex[i] - m_hullVertex[4]);
					if ((error % error) < (DG_DISTANCE_TOLERANCE * DG_DISTANCE_TOLERANCE)) {
						plane = face;
						//code = dgMinkDisjoint;
						code = UpdateSeparatingPlaneFallbackSolution (plane, origin);
						_ASSERTE ((code == dgMinkDisjoint) || ((code == dgMinkIntersecting) && (m_vertexIndex == 4)));
						break;
					}
				}
				if (i < 4) {
					break;
				}
			}

			dgInt32 i0 = face->m_vertex[0];
			dgInt32 i1 = face->m_vertex[1];
			dgInt32 i2 = m_faceIndex[face - m_simplex][3];
			_ASSERTE (i2 != face->m_vertex[0]);
			_ASSERTE (i2 != face->m_vertex[1]);
			_ASSERTE (i2 != face->m_vertex[2]);
			Swap ((simd_128&)m_hullVertex[i0], (simd_128&)m_hullVertex[i1]);
			Swap ((simd_128&)m_averVertex[i0], (simd_128&)m_averVertex[i1]);
			(simd_128&)m_hullVertex[i2] = (simd_128&)m_hullVertex[4];
			(simd_128&)m_averVertex[i2] = (simd_128&)m_averVertex[4];
			if (!CheckTetraHedronVolume ()) {
				Swap ((simd_128&)m_hullVertex[1], (simd_128&)m_hullVertex[2]);
				Swap ((simd_128&)m_averVertex[1], (simd_128&)m_averVertex[2]);
				_ASSERTE (CheckTetraHedronVolume ());
			}
		}
	} 

	if (j >= DG_UPDATE_SEPARATING_PLANE_MAX_ITERATION) {
		_ASSERTE (CheckTetraHedronVolume());
		code = UpdateSeparatingPlaneFallbackSolution (plane, origin);
	}
	return code;
}




dgInt32 dgContactSolver::CalculateConvexShapeIntersectionSimd (const dgMatrix& matrix, const dgVector& shapeNormal, dgUnsigned32 id, dgFloat32 penetration,
															   dgInt32 shape1VertexCount, dgVector* const shape1, dgInt32 shape2VertexCount, dgVector* const shape2,
															   dgContactPoint* const contactOut, dgInt32 maxContacts) const
{
	dgInt32 count = 0;
	if (shape2VertexCount <= 2) {
		count = CalculateConvexShapeIntersectionLine (matrix, shapeNormal, id, penetration, shape1VertexCount, shape1, shape2VertexCount, shape2, contactOut);
		if (count > maxContacts) {
			count = maxContacts;
		}

	} else if (shape1VertexCount <= 2) {
		count = CalculateConvexShapeIntersectionLine (matrix, shapeNormal, id, penetration, shape2VertexCount, shape2, shape1VertexCount, shape1, contactOut);
		if (count > maxContacts) {
			count = maxContacts;
		}

	} else {
		dgPerimenterEdge* edgeClipped[2];
		dgPerimenterEdge subdivision[128];

		_ASSERTE (shape1VertexCount >= 3);
		_ASSERTE (shape2VertexCount >= 3);
		dgVector* output = (dgVector*) &m_hullVertex[shape1VertexCount + shape2VertexCount];

		_ASSERTE ((shape1VertexCount + shape2VertexCount) < sizeof (subdivision) / (2 * sizeof (subdivision[0])));
		for (dgInt32 i0 = 0; i0 < shape2VertexCount; i0 ++) {
			subdivision[i0].m_vertex = &shape2[i0];
			subdivision[i0].m_prev = &subdivision[i0 - 1];
			subdivision[i0].m_next = &subdivision[i0 + 1];
		}
		subdivision[0].m_prev = &subdivision[shape2VertexCount - 1];
		subdivision[shape2VertexCount - 1].m_next = &subdivision[0];

		dgPerimenterEdge* poly = &subdivision[0];
		//_ASSERTE (CheckNormal (poly, shapeNormal));
#ifdef _DEBUG
		{
			dgVector p0;
			dgVector p1;
			m_referenceCollision->CalcAABB (dgGetIdentityMatrix(), p0, p1);
			p0-= dgVector (0.5f, 0.5f, 0.5f, 0.0f);
			p1+= dgVector (0.5f, 0.5f, 0.5f, 0.0f);
			for (dgInt32 i = 0; i < shape1VertexCount; i ++) {
				dgVector p (shape1[i]);
				_ASSERTE (p.m_x >= p0.m_x);
				_ASSERTE (p.m_y >= p0.m_y);
				_ASSERTE (p.m_z >= p0.m_z);
				_ASSERTE (p.m_x <= p1.m_x);
				_ASSERTE (p.m_y <= p1.m_y);
				_ASSERTE (p.m_z <= p1.m_z);
			} 
		}
#endif

		simd_128 zero (dgFloat32 (0.0f));
		simd_128 neg_one (dgFloat32 (-1.0f));
		simd_128 tol_pos_1e_24 (dgFloat32 (1.0e-24f));
		simd_128 tol_neg_1e_24 (dgFloat32 (-1.0e-24f));

		edgeClipped[0] = NULL;
		edgeClipped[1] = NULL;

int xxx = 0;
		dgInt32 i0 = shape1VertexCount - 1;
		dgInt32 edgeIndex = shape2VertexCount;
		simd_128 normal ((simd_128&)shapeNormal & simd_128 (-1, -1, -1, 0));
		for (dgInt32 i1 = 0; i1 < shape1VertexCount; i1 ++) {
			simd_128 edgePlane (normal.CrossProduct((simd_128&)shape1[i1] - (simd_128&)shape1[i0]));
			simd_128 edgePlaneOrigin ((simd_128&)shape1[i0]);

			i0 = i1;
			count = 0;
			dgPerimenterEdge* tmp = poly;
			dgInt32 isInside = 0;
			simd_128 test0 (edgePlane.DotProduct (*(simd_128*)tmp->m_vertex - edgePlaneOrigin));
			do {
xxx ++;
				simd_128 test1 (edgePlane.DotProduct (*(simd_128*)tmp->m_next->m_vertex - edgePlaneOrigin));
				if ((test0 >= zero).GetSignMask()) {

					isInside |= 1;
					if ((test1 < zero).GetSignMask()) {

						simd_128 p0 (*(simd_128*)tmp->m_vertex);
						simd_128 p1 (*(simd_128*)tmp->m_next->m_vertex);
						simd_128 dp = p1 - p0;
						simd_128 den (edgePlane.DotProduct(dp));
						simd_128 test (den >= zero);
						den = (den.GetMax(tol_pos_1e_24) & test) | den.GetMin(tol_neg_1e_24).AndNot(test);
						den = test0 / den; 
						den = neg_one.GetMax(zero.GetMin(den));
						(simd_128&)output[0] = p0 - dp * den;

						edgeClipped[0] = tmp;
						count ++;
					}

				} else if ((test1 >= zero).GetSignMask()) {
					isInside |= 1;
					simd_128 p0 (*(simd_128*)tmp->m_vertex);
					simd_128 p1 (*(simd_128*)tmp->m_next->m_vertex);
					simd_128 dp = p1 - p0;
					simd_128 den (edgePlane.DotProduct(dp));
					simd_128 test (den >= zero);
					den = (den.GetMax(tol_pos_1e_24) & test) | den.GetMin(tol_neg_1e_24).AndNot(test);
					den = test0 / den; 
					den = neg_one.GetMax(zero.GetMin(den));
					(simd_128&)output[1] = p0 - dp * den;

					edgeClipped[1] = tmp;
					count ++;
				}
				test0 = test1;
				tmp = tmp->m_next;
			} while (tmp != poly && (count < 2));

			if (!isInside) {
				return 0;
			}

			if (count == 2) {
				//dgPerimenterEdge *newEdge;
				dgPerimenterEdge* const newEdge = &subdivision[edgeIndex];
				newEdge->m_next = edgeClipped[1];
				newEdge->m_prev = edgeClipped[0];
				edgeClipped[0]->m_next = newEdge;
				edgeClipped[1]->m_prev = newEdge;

				newEdge->m_vertex = &output[0];
				edgeClipped[1]->m_vertex = &output[1];
				poly = newEdge;

				output += 2;
				edgeIndex ++;
				_ASSERTE (edgeIndex < sizeof (subdivision) / sizeof (subdivision[0]));
				//_ASSERTE (CheckNormal (poly, shapeNormal));
			}
		}

		_ASSERTE (poly);

#ifdef _DEBUG
		{
			dgVector p0;
			dgVector p1;
			m_referenceCollision->CalcAABB (dgGetIdentityMatrix(), p0, p1);
			p0-= dgVector (0.5f, 0.5f, 0.5f, 0.0f);
			p1+= dgVector (0.5f, 0.5f, 0.5f, 0.0f);
			dgPerimenterEdge *tmp;
			tmp = poly;
			do {
				dgVector p (*tmp->m_vertex);
				_ASSERTE (p.m_x >= p0.m_x);
				_ASSERTE (p.m_y >= p0.m_y);
				_ASSERTE (p.m_z >= p0.m_z);
				_ASSERTE (p.m_x <= p1.m_x);
				_ASSERTE (p.m_y <= p1.m_y);
				_ASSERTE (p.m_z <= p1.m_z);
				tmp = tmp->m_next;
			} while (tmp != poly);
		}
#endif

		_ASSERTE (poly);
		poly = ReduceContacts (poly, maxContacts);

		//if (count > 0) {
		//dgPerimenterEdge *intersection;
		count = 0;
		dgPerimenterEdge* intersection = poly;
		//dgVector normal = matrix.RotateVector(shapeNormal);
		normal = matrix.RotateVectorSimd(normal);
		do {
			(simd_128&)contactOut[count].m_point = matrix.TransformVectorSimd(*intersection->m_vertex);
			(simd_128&)contactOut[count].m_normal = normal;
			contactOut[count].m_penetration = penetration;
			contactOut[count].m_userId = id;
			count ++;
			intersection = intersection->m_next;
		} while (intersection != poly);
	}

	return count;
}

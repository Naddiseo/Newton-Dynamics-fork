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


dgVector dgContactSolver::m_dir[14];

dgInt32 dgContactSolver::m_faceIndex[][4] =
{ 
	{0, 1, 2, 3},
	{1, 0, 3, 2},
	{0, 2, 3, 1},
	{2, 1, 3, 0},
};


dgContactSolver::dgContactSolver(dgCollisionParamProxy& proxy)
	:m_matrix (*proxy.m_localMatrixInv)
{
	void* hullVertexLarge = m_hullVertex;
	void* averVertexLarge = m_averVertex;
	m_hullVertexLarge = (dgBigVector*)hullVertexLarge;
	m_averVertexLarge = (dgBigVector*)averVertexLarge;

	_ASSERTE ((m_matrix.m_front % m_matrix.m_front) > dgFloat32 (0.9995f));
	_ASSERTE ((m_matrix.m_up % m_matrix.m_up) > dgFloat32 (0.9995f));
	_ASSERTE ((m_matrix.m_right % m_matrix.m_right) > dgFloat32 (0.9995f));
	_ASSERTE (((m_matrix.m_front * m_matrix.m_up) % m_matrix.m_right) < dgFloat32 (1.0001f));

	m_lastFaceCode = dgMinkError;

	m_proxy = &proxy;
	m_floatingBody = proxy.m_floatingBody; 
	m_referenceBody = proxy.m_referenceBody; 
	m_penetrationPadding = proxy.m_penetrationPadding;
	m_floatingcollision = (dgCollisionConvex*) proxy.m_floatingCollision;
	m_referenceCollision = (dgCollisionConvex*) proxy.m_referenceCollision;
}


dgContactSolver::dgContactSolver(dgCollisionParamProxy& proxy, dgCollision *polygon)
	:m_matrix (*proxy.m_localMatrixInv) 
{
	void* hullVertexLarge = m_hullVertex;
	void* averVertexLarge = m_averVertex;
	m_hullVertexLarge = (dgBigVector*)hullVertexLarge;
	m_averVertexLarge = (dgBigVector*)averVertexLarge;

	_ASSERTE ((m_matrix.m_front % m_matrix.m_front) > dgFloat32 (0.9999f));
	_ASSERTE ((m_matrix.m_up % m_matrix.m_up) > dgFloat32 (0.9999f));
	_ASSERTE ((m_matrix.m_right % m_matrix.m_right) > dgFloat32 (0.9999f));
	_ASSERTE (((m_matrix.m_front * m_matrix.m_up) % m_matrix.m_right) < dgFloat32 (1.0001f));


	m_lastFaceCode = dgMinkError;

	m_proxy = &proxy;
	m_floatingBody = proxy.m_floatingBody; 
	//m_floatingcollision = (dgCollisionConvex*) proxy.m_floatingCollision;
	m_floatingcollision = (dgCollisionConvex*) polygon;
	m_referenceBody = proxy.m_referenceBody; 
	m_referenceCollision = (dgCollisionConvex*) proxy.m_referenceCollision;
	m_penetrationPadding = proxy.m_penetrationPadding;
}

void dgContactSolver::CalcSupportVertexLarge (const dgVector& dir, dgInt32 entry)
{
	_ASSERTE ((dir % dir) > dgFloat32 (0.999f));
	dgVector p0 (m_referenceCollision->SupportVertex (dir));
	dgVector dir1 (m_matrix.UnrotateVector (dir.Scale (dgFloat32 (-1.0f))));
	dgVector q0 (m_matrix.TransformVector (m_floatingcollision->SupportVertex (dir1)));

	dgBigVector p(p0);
	dgBigVector q(q0);

	m_hullVertexLarge[entry] = p - q;
	m_averVertexLarge[entry] = p + q;
}


void dgContactSolver::CalcSupportVertex (const dgVector& dir, dgInt32 entry)
{
	_ASSERTE ((dir % dir) > dgFloat32 (0.999f));
	dgVector p (m_referenceCollision->SupportVertex (dir));
	dgVector dir1 (m_matrix.UnrotateVector (dir.Scale (dgFloat32 (-1.0f))));
	dgVector q (m_matrix.TransformVector (m_floatingcollision->SupportVertex (dir1)));

	m_hullVertex[entry] = p - q;
	m_averVertex[entry] = p + q;
}



void dgContactSolver::CalculateVelocities (dgFloat32 timestep) 
{
	dgVector refOmega;
	dgVector floatOmega;

	m_referenceBody->CalculateContinueVelocity (timestep, m_referenceBodyVeloc, refOmega);
	m_floatingBody->CalculateContinueVelocity (timestep, m_floatingBodyVeloc, floatOmega);
	dgVector vRel (m_floatingBodyVeloc - m_referenceBodyVeloc);
	m_localRelVeloc = m_proxy->m_referenceMatrix.UnrotateVector(vRel);
}

dgBigVector dgContactSolver::ReduceLineLarge (const dgBigVector& origin)
{
	dgFloat64 alpha0;
	dgBigVector v;
	const dgBigVector& p0 = m_hullVertex[0];
	const dgBigVector& p1 = m_hullVertex[1];
	dgBigVector dp (p1 - p0);

	alpha0 = ((origin - p0) % dp) / (dp % dp + dgFloat64 (1.0e-24f));
	if (alpha0 > dgFloat64 (1.0f)) {
		v = p1;
		m_vertexIndex = 1;
		m_hullVertexLarge[0] = m_hullVertexLarge[1];
		m_averVertexLarge[0] = m_averVertexLarge[1];

	} else if (alpha0 < dgFloat64 (0.0f)) {
		v = p0;
		m_vertexIndex = 1;
	} else {
		v = p0 + dp.Scale (alpha0);
	}
	return v;
}


dgVector dgContactSolver::ReduceLine (const dgVector& origin)
{
	const dgVector& p0 = m_hullVertex[0];
	const dgVector& p1 = m_hullVertex[1];
	dgVector dp (p1 - p0);

	dgVector v;
	dgFloat32 alpha0 = ((origin - p0) % dp) / (dp % dp + dgFloat32 (1.0e-24f));
	if (alpha0 > dgFloat32 (1.0f)) {
		v = p1;
		m_vertexIndex = 1;
		m_hullVertex[0] = m_hullVertex[1];
		m_averVertex[0] = m_averVertex[1];

	} else if (alpha0 < dgFloat32 (0.0f)) {
		v = p0;
		m_vertexIndex = 1;
	} else {
		v = p0 + dp.Scale (alpha0);
	}
	return v;
}

dgBigVector dgContactSolver::ReduceTriangleLarge (const dgBigVector& origin)
{
	const dgBigVector& p0 = m_hullVertexLarge[0];
	const dgBigVector& p1 = m_hullVertexLarge[1]; 
	const dgBigVector& p2 = m_hullVertexLarge[2];
	const dgBigVector p (origin);

	dgBigVector p10 (p1 - p0);
	dgBigVector p20 (p2 - p0);
	dgBigVector p_p0 (p - p0);

	dgFloat64 alpha1 = p10 % p_p0;
	dgFloat64 alpha2 = p20 % p_p0;
	if ((alpha1 <= dgFloat64 (0.0f)) && (alpha2 <= dgFloat64 (0.0f))) {
		m_vertexIndex = 1;
		return p0;
	}

	dgBigVector p_p1 (p - p1);
	dgFloat64 alpha3 = p10 % p_p1;
	dgFloat64 alpha4 = p20 % p_p1;
	if ((alpha3 >= dgFloat64 (0.0f)) && (alpha4 <= alpha3)) {
		m_vertexIndex = 1;
		m_hullVertexLarge[0] = p1;
		m_averVertexLarge[0] = m_averVertexLarge[1];
		return p1;
	}

	dgFloat64 vc = alpha1 * alpha4 - alpha3 * alpha2;
	if ((vc <= dgFloat64 (0.0f)) && (alpha1 >= dgFloat64 (0.0f)) && (alpha3 <= dgFloat64 (0.0f))) {
		dgFloat64 t = alpha1 / ( alpha1 - alpha3);
		_ASSERTE (t >= dgFloat64 (0.0f));
		_ASSERTE (t <= dgFloat64 (1.0f));
		m_vertexIndex = 2;
		return p0.Scale(dgFloat64 (1.0f) - t) + p1.Scale (t);
	}


	dgBigVector p_p2 (p - p2);
	dgFloat64 alpha5 = p10 % p_p2;
	dgFloat64 alpha6 = p20 % p_p2;
	if ((alpha6 >= dgFloat64 (0.0f)) && (alpha5 <= alpha6)) {
		m_vertexIndex = 1;
		m_hullVertexLarge[0] = p2;
		m_averVertexLarge[0] = m_averVertexLarge[2];
		return p2;
	}


	dgFloat64 vb = alpha5 * alpha2 - alpha1 * alpha6;
	if ((vb <= dgFloat64 (0.0f)) && (alpha2 >= dgFloat64 (0.0f)) && (alpha6 <= dgFloat64 (0.0f))) {
		dgFloat64 t = alpha2 / ( alpha2 - alpha6);
		_ASSERTE (t >= dgFloat64 (0.0f));
		_ASSERTE (t <= dgFloat64 (1.0f));
		m_vertexIndex = 2;
		m_hullVertexLarge[1] = p2;
		m_averVertexLarge[1] = m_averVertexLarge[2];
		return p0.Scale(dgFloat64 (1.0f) - t) + p2.Scale (t);
	}


	dgFloat64 va = alpha3 * alpha6 - alpha5 * alpha4;
	if ((va <= dgFloat64 (0.0f)) && ((alpha4 - alpha3) >= dgFloat64 (0.0f)) && ((alpha5 - alpha6) >= dgFloat64 (0.0f))) {
		dgFloat64 t = (alpha4 - alpha3) / ((alpha4 - alpha3) + (alpha5 - alpha6));
		_ASSERTE (t >= dgFloat64 (0.0f));
		_ASSERTE (t <= dgFloat64 (1.0f));
		//return b + (c - b).Scale (t);
		//return dgBigVector (dgFloat64 (0.0f), dgFloat64 (1.0f) - t, t);
		m_vertexIndex = 2;
		m_hullVertexLarge[0] = p2;
		m_averVertexLarge[0] = m_averVertexLarge[2];
		return p1.Scale(dgFloat64 (1.0f) - t) + p2.Scale (t);
	}

	dgFloat64 den = dgFloat64(1.0f) / (va + vb + vc);
	dgFloat64 t = vb * den;
	dgFloat64 s = vc * den;

	_ASSERTE (t >= dgFloat64 (0.0f));
	_ASSERTE (s >= dgFloat64 (0.0f));
	_ASSERTE (t <= dgFloat64 (1.0f));
	_ASSERTE (s <= dgFloat64 (1.0f));
	m_vertexIndex = 3;
	return p0 + p10.Scale(t) + p20.Scale (s);
}


dgVector dgContactSolver::ReduceTriangle (const dgVector& origin)
{
	const dgVector& p0 = m_hullVertex[0];
	const dgVector& p1 = m_hullVertex[1]; 
	const dgVector& p2 = m_hullVertex[2];
	const dgVector p (origin);

	dgVector p10 (p1 - p0);
	dgVector p20 (p2 - p0);
	dgVector p_p0 (p - p0);

	dgFloat32 alpha1 = p10 % p_p0;
	dgFloat32 alpha2 = p20 % p_p0;
	if ((alpha1 <= dgFloat32 (0.0f)) && (alpha2 <= dgFloat32 (0.0f))) {
		m_vertexIndex = 1;
		return p0;
	}

	dgVector p_p1 (p - p1);
	dgFloat32 alpha3 = p10 % p_p1;
	dgFloat32 alpha4 = p20 % p_p1;
	if ((alpha3 >= dgFloat32 (0.0f)) && (alpha4 <= alpha3)) {
		m_vertexIndex = 1;
		m_hullVertex[0] = p1;
		m_averVertex[0] = m_averVertex[1];
		return p1;
	}

	dgFloat32 vc = alpha1 * alpha4 - alpha3 * alpha2;
	if ((vc <= dgFloat32 (0.0f)) && (alpha1 >= dgFloat32 (0.0f)) && (alpha3 <= dgFloat32 (0.0f))) {
		dgFloat32 t = alpha1 / ( alpha1 - alpha3);
		_ASSERTE (t >= dgFloat32 (0.0f));
		_ASSERTE (t <= dgFloat32 (1.0f));
		m_vertexIndex = 2;
		return p0.Scale(dgFloat32 (1.0f) - t) + p1.Scale (t);
	}


	dgVector p_p2 (p - p2);
	dgFloat32 alpha5 = p10 % p_p2;
	dgFloat32 alpha6 = p20 % p_p2;
	if ((alpha6 >= dgFloat32 (0.0f)) && (alpha5 <= alpha6)) {
		m_vertexIndex = 1;
		m_hullVertex[0] = p2;
		m_averVertex[0] = m_averVertex[2];
		return p2;
	}


	dgFloat32 vb = alpha5 * alpha2 - alpha1 * alpha6;
	if ((vb <= dgFloat32 (0.0f)) && (alpha2 >= dgFloat32 (0.0f)) && (alpha6 <= dgFloat32 (0.0f))) {
		dgFloat32 t = alpha2 / ( alpha2 - alpha6);
		_ASSERTE (t >= dgFloat32 (0.0f));
		_ASSERTE (t <= dgFloat32 (1.0f));
		m_vertexIndex = 2;
		m_hullVertex[1] = p2;
		m_averVertex[1] = m_averVertex[2];
		return p0.Scale(dgFloat32 (1.0f) - t) + p2.Scale (t);
	}


	dgFloat32 va = alpha3 * alpha6 - alpha5 * alpha4;
	if ((va <= dgFloat32 (0.0f)) && ((alpha4 - alpha3) >= dgFloat32 (0.0f)) && ((alpha5 - alpha6) >= dgFloat32 (0.0f))) {
		dgFloat32 t = (alpha4 - alpha3) / ((alpha4 - alpha3) + (alpha5 - alpha6));
		_ASSERTE (t >= dgFloat32 (0.0f));
		_ASSERTE (t <= dgFloat32 (1.0f));
		//return b + (c - b).Scale (t);
		//return dgVector (dgFloat32 (0.0f), dgFloat32 (1.0f) - t, t);
		m_vertexIndex = 2;
		m_hullVertex[0] = p2;
		m_averVertex[0] = m_averVertex[2];
		return p1.Scale(dgFloat32 (1.0f) - t) + p2.Scale (t);
	}

	dgFloat32 den = dgFloat32(1.0f) / (va + vb + vc);
	dgFloat32 t = vb * den;
	dgFloat32 s = vc * den;

	_ASSERTE (t >= dgFloat32 (0.0f));
	_ASSERTE (s >= dgFloat32 (0.0f));
	_ASSERTE (t <= dgFloat32 (1.0f));
	_ASSERTE (s <= dgFloat32 (1.0f));
	m_vertexIndex = 3;
	return p0 + p10.Scale(t) + p20.Scale (s);
}


dgBigVector dgContactSolver::ReduceTetrahedrumLarge (const dgBigVector& origin)
{
	dgInt32 index0 = -1;
	dgInt32 index1 = -1;
	dgInt32 index2 = -1;
	dgBigVector p (origin);
	dgFloat64 minDist = dgFloat32 (1.0e20f);
	for (dgInt32 i = 0; i < 4; i ++) {
		dgInt32 i0 = m_faceIndex[i][0];
		dgInt32 i1 = m_faceIndex[i][1];
		dgInt32 i2 = m_faceIndex[i][2];
		const dgBigVector& p0 = m_hullVertexLarge[i0];
		const dgBigVector& p1 = m_hullVertexLarge[i1]; 
		const dgBigVector& p2 = m_hullVertexLarge[i2];

		dgBigVector p10 (p1 - p0);
		dgBigVector p20 (p2 - p0);
		dgBigVector p_p0 (origin - p0);
		dgFloat64 volume = p_p0 % (p10 * p20);
		if (volume < dgFloat64 (0.0f)) {
			dgBigVector q (dgPointToTriangleDistance (origin, p0, p1, p2));
			dgBigVector qDist (q - origin);

			dgFloat64 dist = qDist % qDist;
			if (dist < minDist) {
				p = q;
				index0 = i0;
				index1 = i1;
				index2 = i2;
				minDist = dist;
			}
		}
	}

	if (index0 != -1) {
		dgBigVector tmpSum[2];
		dgBigVector tmpDiff[2];
		tmpDiff[0] = m_hullVertexLarge[index1];
		tmpDiff[1] = m_hullVertexLarge[index2];
		tmpSum[0] = m_averVertexLarge[index1];
		tmpSum[1] = m_averVertexLarge[index2];

		m_hullVertexLarge[0] = m_hullVertexLarge[index0];
		m_hullVertexLarge[1] = tmpDiff[0];
		m_hullVertexLarge[2] = tmpDiff[1];
		m_averVertexLarge[0] = m_averVertexLarge[index0];
		m_averVertexLarge[1] = tmpSum[0];
		m_averVertexLarge[2] = tmpSum[1];
		return ReduceTriangleLarge (origin);
	}
	return p;
}


dgVector dgContactSolver::ReduceTetrahedrum (const dgVector& origin)
{
	dgInt32 index0 = -1;
	dgInt32 index1 = -1;
	dgInt32 index2 = -1;
	dgVector p (origin);
	dgFloat32 minDist = dgFloat32 (1.0e20f);
	for (dgInt32 i = 0; i < 4; i ++) {
		dgInt32 i0 = m_faceIndex[i][0];
		dgInt32 i1 = m_faceIndex[i][1];
		dgInt32 i2 = m_faceIndex[i][2];
		const dgVector& p0 = m_hullVertex[i0];
		const dgVector& p1 = m_hullVertex[i1]; 
		const dgVector& p2 = m_hullVertex[i2];

		dgVector p10 (p1 - p0);
		dgVector p20 (p2 - p0);
		const dgVector p_p0 (origin - p0);
		dgFloat32 volume = p_p0 % (p10 * p20);
		if (volume < dgFloat32 (0.0f)) {
			dgVector q (dgPointToTriangleDistance (origin, p0, p1, p2));
			dgVector qDist (q - origin);

			dgFloat32 dist = qDist % qDist;
			if (dist < minDist) {
				p = q;
				index0 = i0;
				index1 = i1;
				index2 = i2;
				minDist = dist;
			}
		}
	}

	if (index0 != -1) {
		dgVector tmpSum[2];
		dgVector tmpDiff[2];

		tmpDiff[0] = m_hullVertex[index1];
		tmpDiff[1] = m_hullVertex[index2];
		tmpSum[0] = m_averVertex[index1];
		tmpSum[1] = m_averVertex[index2];

		m_hullVertex[0] = m_hullVertex[index0];
		m_hullVertex[1] = tmpDiff[0];
		m_hullVertex[2] = tmpDiff[1];
		m_averVertex[0] = m_averVertex[index0];
		m_averVertex[1] = tmpSum[0];
		m_averVertex[2] = tmpSum[1];
		return ReduceTriangle (origin);
	}
	return p;
}

dgContactSolver::dgMinkReturnCode dgContactSolver::UpdateSeparatingPlaneFallbackSolutionLarge(dgMinkFace*& plane, const dgBigVector& origin)
{
	dgInt32 cicling = -1;
	dgFloat64 minDist = dgFloat64 (1.0e20f);
	dgMinkReturnCode code = dgMinkError;

	dgBigVector v (ReduceTetrahedrumLarge (origin) - origin);
	dgBigVector dir0 (dgFloat64 (0.0f), dgFloat64 (0.0f), dgFloat64 (0.0f), dgFloat64 (0.0f));

	for (dgInt32 i = 0; (i < DG_FALLBACK_SEPARATING_PLANE_ITERATIONS) && (m_vertexIndex < 4); i ++) {
		dgFloat64 dist = v % v;
		if (dist < dgFloat64 (1.0e-9f)) {
			switch (m_vertexIndex) 
			{
				case 1:
				{
					dgInt32 best = 0;
					dgFloat64 maxErr = dgFloat64 (0.0f);
					dgInt32 i = 0;
					for (; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
						dgFloat64 error2;
						CalcSupportVertexLarge (m_dir[i], 1);
						dgBigVector e (m_hullVertexLarge[1] - m_hullVertexLarge[0]);
						error2 = e % e;
						if (error2 > dgFloat64 (1.0e-4f)) {
							break;
						}
						if (error2 > maxErr) {
							best = i;
							maxErr = error2;
						}
					}

					if (i == dgInt32(sizeof(m_dir) / sizeof(m_dir[0]))) {
						_ASSERTE (maxErr > dgFloat64 (0.0f));
						CalcSupportVertexLarge (m_dir[best], 1);
					}
					m_vertexIndex = 2;

				}

				case 2:
				{
					dgInt32 best = 0;
					dgFloat64 maxErr = dgFloat64 (0.0f);
					dgBigVector e0 (m_hullVertexLarge[1] - m_hullVertexLarge[0]);
					dgInt32 i = 0;
					for (; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
						dgFloat64 error2;
						CalcSupportVertexLarge (m_dir[i], 2);
						dgBigVector e1 (m_hullVertexLarge[2] - m_hullVertexLarge[0]);
						dgBigVector n (e0 * e1);
						error2 = n % n;
						if (error2 > dgFloat64 (1.0e-4f)) {
							break;
						}
						if (error2 > maxErr) {
							best = i;
							maxErr = error2;
						}
					}

					if (i == dgInt32(sizeof(m_dir) / sizeof(m_dir[0]))) {
						_ASSERTE (maxErr > dgFloat64 (0.0f));
						CalcSupportVertexLarge (m_dir[best], 2);
					}
					m_vertexIndex = 3;
				}

				default:
				{
					const dgBigVector& p0 = m_hullVertexLarge[0];
					const dgBigVector& p1 = m_hullVertexLarge[1]; 
					const dgBigVector& p2 = m_hullVertexLarge[2];
					dgBigVector normal ((p1 - p0) * (p2 - p0));
					dgFloat64 mag2 = normal % normal;
					_ASSERTE (mag2 > dgFloat64 (1.0e-10f));
					normal = normal.Scale (dgFloat64 (1.0f)/ sqrt(mag2));
					dgVector dir (dgFloat32 (normal.m_x), dgFloat32 (normal.m_y), dgFloat32 (normal.m_z), dgFloat32 (0.0f)); 
					CalcSupportVertexLarge (dir, 3);
					CalcSupportVertexLarge (dir.Scale (dgFloat32 (-1.0f)), 4);
					if (fabs((m_hullVertexLarge[4] - p0) % normal) > fabs((m_hullVertexLarge[3] - p0) % normal)) {
						m_hullVertexLarge[3] = m_hullVertexLarge[4];
						m_averVertexLarge[3] = m_averVertexLarge[4];
					}
					m_vertexIndex = 4;

					if (!CheckTetrahedronVolumeLarge()) {
						Swap (m_hullVertexLarge[2], m_hullVertexLarge[1]);
						Swap (m_averVertexLarge[2], m_averVertexLarge[1]);
						_ASSERTE (CheckTetrahedronVolumeLarge());
					}
				}
			}
			return dgMinkIntersecting;
		}


		if (dist < minDist) {
			minDist = dist;
			cicling = -1;
		}


		_ASSERTE (dist > dgFloat64 (1.0e-24f));
		dgBigVector dir (v.Scale (- dgFloat64 (1.0f) / sqrt (dist)));
		dist = dir0 % dir;
		if (dist < dgFloat64 (0.9995f)) {
			dgVector dir1 (dgFloat32 (dir.m_x), dgFloat32 (dir.m_y), dgFloat32 (dir.m_z), dgFloat32 (0.0f)); 
			CalcSupportVertexLarge (dir1, m_vertexIndex);
			dgBigVector w (m_hullVertexLarge[m_vertexIndex] - origin);
			dgBigVector wv (w - v);
			dist = dir % wv;
		} else {
			dist = dgFloat64 (0.0f);
		}

		cicling ++;
		if (cicling > 4) {
			dist = dgFloat64 (0.0f);
		}

		dir0 = dir;
		if (dist < dgFloat64 (5.0e-4f)) {
			dgMatrix rotMatrix;
			//dgBigPlane separatingPlane (dir.Scale (dgFloat64 (-1.0f)), origin % dir);
			dgVector dir32 (dgFloat32 (dir.m_x), dgFloat32 (dir.m_y), dgFloat32 (dir.m_z), dgFloat32 (0.0f));
			dgPlane separatingPlane (dir32.Scale (dgFloat32 (-1.0f)), dgFloat32 (origin % dir));

			switch (m_vertexIndex) 
			{
				case 1:
				{
					dgFloat64 minDist = dgFloat64 (1.0e10f);
					rotMatrix = dgMatrix (dir32);
					_ASSERTE (rotMatrix.m_front.m_w == dgFloat64 (0.0f));
					_ASSERTE (rotMatrix.m_up.m_w == dgFloat64 (0.0f));
					_ASSERTE (rotMatrix.m_right.m_w == dgFloat64 (0.0f));


					dgInt32 keepSeaching = 1;
					dgVector dir1 (dgFloat32 (1.0), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat64 (0.0f) );
					// 2.0 degree rotation
					dgVector yawRoll (dgFloat32 (0.99939083f), dgFloat32 (0.0f), dgFloat32 (0.034899497f), dgFloat32 (0.0f));
					// 45 degree rotation
					dgVector yawPitch (dgFloat32 (0.0f), dgFloat64 (0.70710678f), dgFloat32 (0.70710678f), dgFloat32 (0.0f));
					for (dgInt32 j = 0; keepSeaching && (j < 180) ; j ++) {
						//						do {
						dgVector tmp (yawRoll.m_x * dir1.m_x - yawRoll.m_z * dir1.m_z, dgFloat64 (0.0f),
							yawRoll.m_z * dir1.m_x + yawRoll.m_x * dir1.m_z, dgFloat64 (0.0f));

						dgFloat32 val1 = tmp % tmp;
						if (dgAbsf (val1 - dgFloat32 (1.0f)) > dgFloat32 (1.0e-4f)) {
							tmp = tmp.Scale (dgFloat32 (1.0f) / dgSqrt (dgFloat32 (val1)));
						}

						dir1 = tmp;
						dgVector dir2 (dir1);
						for (dgInt32 i = 0; i < 8; i ++) {
							dgVector tmp (dir2.m_x, dir2.m_y * yawPitch.m_y - dir2.m_z * yawPitch.m_z, 
								dir2.m_y * yawPitch.m_z + dir2.m_z * yawPitch.m_y, dgFloat64 (0.0f));

							//_ASSERTE (dgAbsf ((tmp % tmp) - dgFloat32 (1.0f)) < dgFloat32 (1.0e-4f));

							dir2 = tmp;
							tmp = rotMatrix.RotateVector(dir2);
							CalcSupportVertexLarge (tmp, 2);
							dgBigVector err0 (m_hullVertex[2] - m_hullVertex[0]);
							dgFloat64 val = err0 % err0;
							if (val > DG_FALLBACK_SEPARATING_DIST_TOLERANCE ) {
								//									val = separatingPlane.Evalue(m_hullVertex[2]);
								val = separatingPlane.m_x * m_hullVertexLarge[2].m_x +
									separatingPlane.m_y * m_hullVertexLarge[2].m_y +
									separatingPlane.m_z * m_hullVertexLarge[2].m_z +
									separatingPlane.m_w;
								_ASSERTE (val > dgFloat64 (0.0f));
								if (val < minDist) {
									keepSeaching = 0;
									minDist = val;
									m_hullVertexLarge[1] = m_hullVertexLarge[2];
									m_averVertexLarge[1] = m_averVertexLarge[2];
								}
							}
						}
						//} while (keepSeaching);
					}
					if (keepSeaching) {
						return dgMinkDisjoint;
					}
				}

				case 2:
				{
					rotMatrix.m_front = dir32;
					//rotMatrix.m_up = m_hullVertexLarge[1] - m_hullVertexLarge[0];
					//mag2 = rotMatrix.m_up % rotMatrix.m_up;
					dgBigVector up (m_hullVertexLarge[1] - m_hullVertexLarge[0]);
					dgFloat64 mag2 = up % up;
					_ASSERTE (mag2 > dgFloat64 (1.0e-24f));
					//rotMatrix.m_up = rotMatrix.m_up.Scale(dgRsqrt (mag2));
					up = up.Scale(dgFloat64(1.0f) / sqrt (mag2));
					rotMatrix.m_up = dgVector (dgFloat32 (up.m_x), dgFloat32 (up.m_y), dgFloat32 (up.m_z), dgFloat32 (0.0f));
					rotMatrix.m_right = rotMatrix.m_front * rotMatrix.m_up;

					//rotMatrix.m_front.m_w = dgFloat64 (0.0f);
					//rotMatrix.m_up.m_w    = dgFloat64 (0.0f);
					rotMatrix.m_right.m_w = dgFloat64 (0.0f);

					dgFloat64 val = dgFloat64 (0.0f);
					// 2.0 degree rotation
					dgVector rot (dgFloat32 (0.99939083f), dgFloat32 (0.0f), dgFloat32 (0.034899497f), dgFloat32 (0.0f));
					dgVector dir1 (dgFloat64 (1.0), dgFloat64 (0.0f), dgFloat64 (0.0f), dgFloat64 (0.0f) );
					do {
						dgVector tmp (rot.m_x * dir1.m_x - rot.m_z * dir1.m_z, dgFloat64 (0.0f),
							rot.m_z * dir1.m_x + rot.m_x * dir1.m_z, dgFloat64 (0.0f));

						dir1 = tmp;
						tmp = rotMatrix.RotateVector(dir1);
						tmp  = tmp .Scale (dgRsqrt(tmp  % tmp ));
						CalcSupportVertexLarge (tmp, 2);
						dgBigVector err0 (m_hullVertexLarge[2] - m_hullVertexLarge[0]);
						val = err0 % err0;
						if (val > DG_FALLBACK_SEPARATING_DIST_TOLERANCE ) {
							dgBigVector err0 (m_hullVertexLarge[2] - m_hullVertexLarge[1]);
							val = err0 % err0;
						}
					} while (val < DG_FALLBACK_SEPARATING_DIST_TOLERANCE);
					_ASSERTE (((m_hullVertexLarge[0] - m_hullVertexLarge[2]) % dir) >= dgFloat64 (-1.0e-1f));
					dir1 = dgVector (dgFloat64 (1.0), dgFloat64 (0.0f), dgFloat64 (0.0f), dgFloat64 (0.0f) );

					do {
						dgVector tmp (rot.m_x * dir1.m_x + rot.m_z * dir1.m_z, dgFloat64 (0.0f),
							rot.m_x * dir1.m_z - rot.m_z * dir1.m_x, dgFloat64 (0.0f));

						dir1 = tmp;
						tmp = rotMatrix.RotateVector(dir1);
						tmp  = tmp .Scale (dgRsqrt(tmp  % tmp ));
						CalcSupportVertexLarge (tmp, 3);
						dgBigVector err0 (m_hullVertexLarge[3] - m_hullVertexLarge[0]);
						val = err0 % err0;
						if (val > DG_FALLBACK_SEPARATING_DIST_TOLERANCE) {
							dgBigVector err0 (m_hullVertexLarge[3] - m_hullVertexLarge[1]);
							val = err0 % err0;
						}
					} while (val < DG_FALLBACK_SEPARATING_DIST_TOLERANCE);
					_ASSERTE (((m_hullVertexLarge[0] - m_hullVertexLarge[3]) % dir) >= dgFloat64 (-1.0e-1f));


					//						dist2 = separatingPlane.Evalue(m_hullVertexLarge[2]);
					//						dist3 = separatingPlane.Evalue(m_hullVertexLarge[3]);
					dgFloat64 dist2 = separatingPlane.m_x * m_hullVertexLarge[2].m_x +
						separatingPlane.m_y * m_hullVertexLarge[2].m_y +
						separatingPlane.m_z * m_hullVertexLarge[2].m_z +
						separatingPlane.m_w;
					dgFloat64 dist3 = separatingPlane.m_x * m_hullVertexLarge[3].m_x +
						separatingPlane.m_y * m_hullVertexLarge[3].m_y +
						separatingPlane.m_z * m_hullVertexLarge[3].m_z +
						separatingPlane.m_w;

					_ASSERTE (dist2 > dgFloat64 (0.0f));
					_ASSERTE (dist3 > dgFloat64 (0.0f));
					if (dist3 < dist2) {
						m_hullVertexLarge[2] = m_hullVertexLarge[3];
						m_averVertexLarge[2] = m_averVertexLarge[3];
						dist2 = dist3;
					}
				}

				case 3:
				{
					CalcSupportVertexLarge (separatingPlane, 3);
				}
			}

			m_vertexIndex = 4;
			plane = &m_simplex[0];
			if (!CheckTetrahedronVolumeLarge()) {
				Swap (m_hullVertexLarge[2], m_hullVertexLarge[1]);
				Swap (m_averVertexLarge[2], m_averVertexLarge[1]);
				_ASSERTE (CheckTetrahedronVolumeLarge());
			}

			return dgMinkDisjoint;
		}

		m_vertexIndex ++;
		switch (m_vertexIndex) 
		{
			case 1:
			{
				_ASSERTE (0);
				break;
			}

			case 2:
			{
				v = ReduceLineLarge (origin) - origin;
				break;
			}

			case 3:
			{
				v = ReduceTriangleLarge (origin) - origin;
				break;
			}

			case 4:
			{
				v = ReduceTetrahedrumLarge (origin) - origin;
				break;
			}
		}
	}


	if (m_vertexIndex == 4) {
		if (!CheckTetrahedronVolumeLarge()) {
			Swap (m_hullVertexLarge[2], m_hullVertexLarge[1]);
			Swap (m_averVertexLarge[2], m_averVertexLarge[1]);
			_ASSERTE (CheckTetrahedronVolumeLarge());
		}

		dgFloat64 minDist = dgFloat64 (1.0e20f);
		for (dgInt32 i = 0; i < 4; i ++) {
			dgInt32 i0 = m_faceIndex[i][0];
			dgInt32 i1 = m_faceIndex[i][1];
			dgInt32 i2 = m_faceIndex[i][2];

			_ASSERTE (i0 == m_simplex[i].m_vertex[0]);
			_ASSERTE (i1 == m_simplex[i].m_vertex[1]);
			_ASSERTE (i2 == m_simplex[i].m_vertex[2]);

			const dgBigVector& p0 = m_hullVertexLarge[i0];
			const dgBigVector& p1 = m_hullVertexLarge[i1];
			const dgBigVector& p2 = m_hullVertexLarge[i2];
			dgBigVector e0 (p1 - p0);
			dgBigVector e1 (p2 - p0);
			dgBigVector n (e0 * e1);

			dgFloat64 dist = n % n;
			_ASSERTE (dist > dgFloat64 (1.0e-20f));
			if (dist > DG_DISTANCE_TOLERANCE_ZERO) {
				n = n.Scale (dgFloat32 (1.0f) / sqrt (dist));
				dist = fabs (n % (origin - p0));
				if (dist < minDist) {
					minDist = dist;
					plane = &m_simplex[i];
				}
			}
		}
		_ASSERTE (plane);
		code = dgMinkIntersecting;
	}
#ifdef _DEBUG
	if (m_vertexIndex < 4) {
		_ASSERTE (0);
		dgTrace (("too many iterations  in: %s\n",  __FUNCDNAME__));
	}
#endif
	return code;
}


dgContactSolver::dgMinkReturnCode dgContactSolver::UpdateSeparatingPlaneFallbackSolution(dgMinkFace*& plane, const dgVector& origin)
{
	dgInt32 cicling = -1;
	dgFloat32 minDist = dgFloat32 (1.0e20f);
	dgMinkReturnCode code = dgMinkError;

	dgVector v (ReduceTetrahedrum (origin) - origin);
	dgVector dir0 (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	for (dgInt32 i = 0; (i < DG_FALLBACK_SEPARATING_PLANE_ITERATIONS) && (m_vertexIndex < 4); i ++) {
		dgFloat32 dist = v % v;
		if (dist < dgFloat32 (1.0e-9f)) {
			switch (m_vertexIndex) 
			{
				case 1:
				{
					//dgInt32 i;
					//dgInt32 best; 
					//dgFloat32 maxErr;

					dgInt32 best = 0;
					dgFloat32 maxErr = dgFloat32 (0.0f);
					dgInt32 i = 0;
					for (; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
						//dgFloat32 error2;
						CalcSupportVertex (m_dir[i], 1);
						dgVector e (m_hullVertex[1] - m_hullVertex[0]);
						dgFloat32 error2 = e % e;
						if (error2 > dgFloat32 (1.0e-4f)) {
							break;
						}
						if (error2 > maxErr) {
							best = i;
							maxErr = error2;
						}
					}

					if (i == dgInt32(sizeof(m_dir) / sizeof(m_dir[0]))) {
						_ASSERTE (maxErr > dgFloat32 (0.0f));
						CalcSupportVertex (m_dir[best], 1);
					}
					m_vertexIndex = 2;

				}

				case 2:
				{
					dgInt32 best = 0;
					dgFloat32 maxErr = dgFloat32 (0.0f);
					dgVector e0 (m_hullVertex[1] - m_hullVertex[0]);
					dgInt32 i = 0;
					for (; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
						CalcSupportVertex (m_dir[i], 2);
						dgVector e1 (m_hullVertex[2] - m_hullVertex[0]);
						dgVector n (e0 * e1);
						dgFloat32 error2 = n % n;
						if (error2 > dgFloat32 (1.0e-4f)) {
							break;
						}
						if (error2 > maxErr) {
							best = i;
							maxErr = error2;
						}
					}

					if (i == dgInt32(sizeof(m_dir) / sizeof(m_dir[0]))) {
						_ASSERTE (maxErr > dgFloat32 (0.0f));
						CalcSupportVertex (m_dir[best], 2);
					}
					m_vertexIndex = 3;
				}

				default:
				{
					const dgVector& p0 = m_hullVertex[0];
					const dgVector& p1 = m_hullVertex[1]; 
					const dgVector& p2 = m_hullVertex[2];
					dgVector normal ((p1 - p0) * (p2 - p0));
					dgFloat32 mag2 = normal % normal;
					_ASSERTE (mag2 > dgFloat32 (1.0e-10f));
					normal = normal.Scale (dgRsqrt(mag2));
					CalcSupportVertex (normal, 3);
					CalcSupportVertex (normal.Scale (dgFloat32 (-1.0f)), 4);
					if (dgAbsf((m_hullVertex[4] - p0) % normal) > dgAbsf((m_hullVertex[3] - p0) % normal)) {
						m_hullVertex[3] = m_hullVertex[4];
						m_averVertex[3] = m_averVertex[4];
					}
					m_vertexIndex = 4;

					if (!CheckTetrahedronVolume()) {
						Swap (m_hullVertex[2], m_hullVertex[1]);
						Swap (m_averVertex[2], m_averVertex[1]);
						_ASSERTE (CheckTetrahedronVolume());
					}
				}
			}
			return dgMinkIntersecting;
		}


		if (dist < minDist) {
			minDist = dist;
			cicling = -1;
		}


		_ASSERTE (dist > dgFloat32 (1.0e-24f));
		dgVector dir (v.Scale (-dgRsqrt (dist)));
		dist = dir0 % dir;
		if (dist < dgFloat32 (0.9995f)) {
			CalcSupportVertex (dir, m_vertexIndex);
			dgVector w (m_hullVertex[m_vertexIndex] - origin);
			dgVector wv (w - v);
			dist = dir % wv;
		} else {
			dist = dgFloat32 (0.0f);
		}

		cicling ++;
		if (cicling > 4) {
			dist = dgFloat32 (0.0f);
		}

		dir0 = dir;
		if (dist < dgFloat32 (5.0e-4f)) {
			dgMatrix rotMatrix;
			dgPlane separatingPlane (dir.Scale (dgFloat32 (-1.0f)), origin % dir);
			switch (m_vertexIndex) 
			{
				case 1:
				{
					dgFloat32 minDist = dgFloat32 (1.0e10f);
					rotMatrix = dgMatrix (dir);
					_ASSERTE (rotMatrix.m_front.m_w == dgFloat32 (0.0f));
					_ASSERTE (rotMatrix.m_up.m_w == dgFloat32 (0.0f));
					_ASSERTE (rotMatrix.m_right.m_w == dgFloat32 (0.0f));

					dgInt32 keepSeaching = 1;
					dgVector dir1 (dgFloat32 (1.0), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f) );
					// 2.0 degree rotation
					dgVector yawRoll (dgFloat32 (0.99939083f), dgFloat32 (0.0f), dgFloat32 (0.034899497f), dgFloat32 (0.0f));
					// 45 degree rotation
					dgVector yawPitch (dgFloat32 (0.0f), dgFloat32 (0.70710678f), dgFloat32 (0.70710678f), dgFloat32 (0.0f));
					//						do {
					for (dgInt32 j = 0; keepSeaching && (j < 180) ; j ++) {
						dgFloat32 val;
						dgVector tmp (yawRoll.m_x * dir1.m_x - yawRoll.m_z * dir1.m_z, dgFloat32 (0.0f),
							yawRoll.m_z * dir1.m_x + yawRoll.m_x * dir1.m_z, dgFloat32 (0.0f));

						val = tmp % tmp;
						if (dgAbsf (val - dgFloat32 (1.0f)) > dgFloat32 (1.0e-4f)) {
							tmp = tmp.Scale (dgFloat32 (1.0f) / dgSqrt (val));
						}

						dir1 = tmp;
						dgVector dir2 (dir1);
						for (dgInt32 i = 0; i < 8; i ++) {
							dgVector tmp (dir2.m_x, dir2.m_y * yawPitch.m_y - dir2.m_z * yawPitch.m_z, 
								dir2.m_y * yawPitch.m_z + dir2.m_z * yawPitch.m_y, dgFloat32 (0.0f));

							_ASSERTE (dgAbsf ((tmp % tmp) - dgFloat32 (1.0f)) < dgFloat32 (1.0e-4f));

							dir2 = tmp;
							tmp = rotMatrix.RotateVector(dir2);
							CalcSupportVertex (tmp, 2);
							dgVector err0 (m_hullVertex[2] - m_hullVertex[0]);
							val = err0 % err0;
							if (val > DG_FALLBACK_SEPARATING_DIST_TOLERANCE ) {
								val = separatingPlane.Evalue(m_hullVertex[2]);
								_ASSERTE (val > dgFloat32 (0.0f));
								if (val < minDist) {
									keepSeaching = 0;
									minDist = val;
									m_hullVertex[1] = m_hullVertex[2];
									m_averVertex[1] = m_averVertex[2];
								}
							}
						}
						//						} while (keepSeaching);
					}

					if (keepSeaching) {
						return dgMinkDisjoint;
					}
				}

				case 2:
				{
					rotMatrix.m_front = dir;
					rotMatrix.m_up = m_hullVertex[1] - m_hullVertex[0];
					dgFloat32 mag2 = rotMatrix.m_up % rotMatrix.m_up;
					_ASSERTE (mag2 > dgFloat32 (1.0e-24f));
					rotMatrix.m_up = rotMatrix.m_up.Scale(dgRsqrt (mag2));
					rotMatrix.m_right = rotMatrix.m_front * rotMatrix.m_up;

					rotMatrix.m_front.m_w = dgFloat32 (0.0f);
					rotMatrix.m_up.m_w    = dgFloat32 (0.0f);
					rotMatrix.m_right.m_w = dgFloat32 (0.0f);

					dgFloat32 val = dgFloat32 (0.0f);
					// 2.0 degree rotation
					dgVector rot (dgFloat32 (0.99939083f), dgFloat32 (0.0f), dgFloat32 (0.034899497f), dgFloat32 (0.0f));
					dgVector dir1 (dgFloat32 (1.0), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f) );
					do {
						dgVector tmp (rot.m_x * dir1.m_x - rot.m_z * dir1.m_z, dgFloat32 (0.0f),
							rot.m_z * dir1.m_x + rot.m_x * dir1.m_z, dgFloat32 (0.0f));

						dir1 = tmp;
						tmp = rotMatrix.RotateVector(dir1);
						tmp  = tmp .Scale (dgRsqrt(tmp  % tmp ));
						CalcSupportVertex (tmp, 2);
						dgVector err0 (m_hullVertex[2] - m_hullVertex[0]);
						val = err0 % err0;
						if (val > DG_FALLBACK_SEPARATING_DIST_TOLERANCE ) {
							dgVector err0 (m_hullVertex[2] - m_hullVertex[1]);
							val = err0 % err0;
						}
					} while (val < DG_FALLBACK_SEPARATING_DIST_TOLERANCE);
#ifdef _DEBUG
					dgFloat32 test = (m_hullVertex[0] - m_hullVertex[2]) % dir;
					_ASSERTE (test  >= dgFloat32 (-2.0e-1f));
#endif
					dir1 = dgVector (dgFloat32 (1.0), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f) );

					do {
						dgVector tmp (rot.m_x * dir1.m_x + rot.m_z * dir1.m_z, dgFloat32 (0.0f),
							rot.m_x * dir1.m_z - rot.m_z * dir1.m_x, dgFloat32 (0.0f));

						dir1 = tmp;
						tmp = rotMatrix.RotateVector(dir1);
						tmp  = tmp .Scale (dgRsqrt(tmp  % tmp ));
						CalcSupportVertex (tmp, 3);
						dgVector err0 (m_hullVertex[3] - m_hullVertex[0]);
						val = err0 % err0;
						if (val > DG_FALLBACK_SEPARATING_DIST_TOLERANCE) {
							dgVector err0 (m_hullVertex[3] - m_hullVertex[1]);
							val = err0 % err0;
						}
					} while (val < DG_FALLBACK_SEPARATING_DIST_TOLERANCE);
#ifdef _DEBUG
					dgFloat32 test1 = (m_hullVertex[0] - m_hullVertex[3]) % dir;
					_ASSERTE (test1  >= dgFloat32 (-2.0e-1f));
#endif


					dgFloat32 dist2 = separatingPlane.Evalue(m_hullVertex[2]);
					dgFloat32 dist3 = separatingPlane.Evalue(m_hullVertex[3]);
					_ASSERTE (dist2 > dgFloat32 (0.0f));
					_ASSERTE (dist3 > dgFloat32 (0.0f));
					if (dist3 < dist2) {
						m_hullVertex[2] = m_hullVertex[3];
						m_averVertex[2] = m_averVertex[3];
						dist2 = dist3;
					}
				}

				case 3:
				{
					CalcSupportVertex (separatingPlane, 3);
				}
			}

			m_vertexIndex = 4;
			plane = &m_simplex[0];
			if (!CheckTetrahedronVolume()) {
				Swap (m_hullVertex[2], m_hullVertex[1]);
				Swap (m_averVertex[2], m_averVertex[1]);
				_ASSERTE (CheckTetrahedronVolume());
			}

			return dgMinkDisjoint;
		}

		m_vertexIndex ++;
		switch (m_vertexIndex) 
		{
			case 1:
			{
				_ASSERTE (0);
				break;
			}

			case 2:
			{
				v = ReduceLine (origin) - origin;
				break;
			}

			case 3:
			{
				v = ReduceTriangle (origin) - origin;
				break;
			}

			case 4:
			{
				v = ReduceTetrahedrum (origin) - origin;
				break;
			}
		}
	}

	if (m_vertexIndex == 4) {
		dgFloat32 minDist;
		if (!CheckTetrahedronVolume()) {
			Swap (m_hullVertex[2], m_hullVertex[1]);
			Swap (m_averVertex[2], m_averVertex[1]);
			_ASSERTE (CheckTetrahedronVolume());
		}

		minDist = dgFloat32 (1.0e20f);
		for (dgInt32 i = 0; i < 4; i ++) {
			//dgFloat32 dist;
			dgInt32 i0 = m_faceIndex[i][0];
			dgInt32 i1 = m_faceIndex[i][1];
			dgInt32 i2 = m_faceIndex[i][2];

			_ASSERTE (i0 == m_simplex[i].m_vertex[0]);
			_ASSERTE (i1 == m_simplex[i].m_vertex[1]);
			_ASSERTE (i2 == m_simplex[i].m_vertex[2]);

			const dgVector& p0 = m_hullVertex[i0];
			const dgVector& p1 = m_hullVertex[i1];
			const dgVector& p2 = m_hullVertex[i2];
			dgVector e0 (p1 - p0);
			dgVector e1 (p2 - p0);
			dgVector n (e0 * e1);

			dgFloat32 dist = n % n;
			_ASSERTE (dist > dgFloat32 (1.0e-20f));
			if (dist > DG_DISTANCE_TOLERANCE_ZERO) {
				n = n.Scale (dgRsqrt (dist));
				dist = dgAbsf (n % (origin - p0));
				if (dist < minDist) {
					minDist = dist;
					plane = &m_simplex[i];
				}
			}
		}
		_ASSERTE (plane);
		code = dgMinkIntersecting;
	}
#ifdef _DEBUG
	if (m_vertexIndex < 4) {
		_ASSERTE (0);
		dgTrace (("too many iterations  in: %s\n",  __FUNCDNAME__));
	}
#endif

	return code;
}


dgContactSolver::dgMinkReturnCode dgContactSolver::UpdateSeparatingPlaneLarge(dgMinkFace*& plane, const dgBigVector& origin)
{
	dgBigVector diff[4];
	dgBigVector aveg[4];

	dgBigVector normal (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));

	plane = NULL;
	dgMinkFace* face = &m_simplex[0];
	dgMinkReturnCode code = dgMinkIntersecting;

	dgInt32 ciclingCount = -1;
	dgMinkFace* lastDescendFace = NULL;
	dgFloat64 minDist = dgFloat32 (1.0e20f);


	// this loop can calculate the closest point to the origin usually in 4 to 5 passes,
	dgInt32 j = 0;
	for (; face && (j < DG_UPDATE_SEPARATING_PLANE_MAX_ITERATION); j ++) {
		face = NULL;
		// initialize distance to zero (very important)
		dgFloat64 maxDist = dgFloat32 (0.0f);

		for (dgInt32 i = 0; i < 4; i ++) {
			dgInt32 i0 = m_faceIndex[i][0];
			dgInt32 i1 = m_faceIndex[i][1];
			dgInt32 i2 = m_faceIndex[i][2];

			_ASSERTE (i0 == m_simplex[i].m_vertex[0]);
			_ASSERTE (i1 == m_simplex[i].m_vertex[1]);
			_ASSERTE (i2 == m_simplex[i].m_vertex[2]);

			const dgBigVector& p0 = m_hullVertexLarge[i0];
			const dgBigVector& p1 = m_hullVertexLarge[i1];
			const dgBigVector& p2 = m_hullVertexLarge[i2];

			dgBigVector e0 (p1 - p0);
			dgBigVector e1 (p2 - p0);
			dgBigVector n (e0 * e1);

			dgFloat64 dist = n % n;
			if (dist > DG_DISTANCE_TOLERANCE_ZERO) {
				n = n.Scale (dgFloat64 (1.0f)/ sqrt(dist));
				dist = n % (origin - p0);

				// find the plane farther away from the origin
				if (dist > maxDist) {
					maxDist = dist;
					normal = n;
					face = &m_simplex[i];
				}
			}
		}


		// if we do not have a face at this point it means that the mink shape of the tow convexity face have a very 
		// skew ratios on floating point accuracy is not enough to guarantee convexity of the shape
		if (face) {
			dgInt32 index = face->m_vertex[0];
			dgVector dir (dgFloat32 (normal.m_x), dgFloat32 (normal.m_y), dgFloat32 (normal.m_z), 0.0f);
			CalcSupportVertexLarge (dir, 4);

			dgFloat64 dist = normal % (m_hullVertexLarge[4] - m_hullVertexLarge[index]);

			// if we are doing too many passes it means that it is a skew shape with big and small floats  
			// significant bits may be lost in dist calculation, increasing the tolerance help to resolve the problem
			if(dist < DG_UPDATE_SEPARATING_PLANE_DISTANCE_TOLERANCE1) {
				plane = face;
				code = dgMinkDisjoint;
				break;
			}

			if (dist < minDist) {
				minDist = dist;
				lastDescendFace = face;
				ciclingCount = -1;
				for (dgInt32 k = 0; k < 4; k ++) {
					diff[k] = m_hullVertexLarge[k];
					aveg[k] = m_averVertexLarge[k];
				}
			}

			ciclingCount ++;
			if (ciclingCount > 4) {
				for (dgInt32 k = 0; k < 4; k ++) {
					m_hullVertexLarge[k] = diff[k];
					m_averVertexLarge[k] = aveg[k];
				}
				code = dgMinkDisjoint;
				plane = lastDescendFace;
				break;
			}


			if (dist < DG_DISTANCE_TOLERANCE) {
				dgInt32 i = 0;
				for (; i < 4; i ++ ) {
					dgBigVector error (m_hullVertexLarge[i] - m_hullVertexLarge[4]);
					if ((error % error) < (DG_DISTANCE_TOLERANCE * DG_DISTANCE_TOLERANCE)) {
						plane = face;
						//code = dgMinkDisjoint;
						code = UpdateSeparatingPlaneFallbackSolutionLarge (plane, origin);
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
			Swap (m_hullVertexLarge[i0], m_hullVertexLarge[i1]);
			Swap (m_averVertexLarge[i0], m_averVertexLarge[i1]);
			m_hullVertexLarge[i2] = m_hullVertexLarge[4];
			m_averVertexLarge[i2] = m_averVertexLarge[4];
			if (!CheckTetrahedronVolumeLarge ()) {
				Swap (m_hullVertexLarge[1], m_hullVertexLarge[2]);
				Swap (m_averVertexLarge[1], m_averVertexLarge[2]);
				_ASSERTE (CheckTetrahedronVolumeLarge ());
			}
		}

	} 

	if (j >= DG_UPDATE_SEPARATING_PLANE_MAX_ITERATION) {
		_ASSERTE (CheckTetrahedronVolumeLarge());
		code = UpdateSeparatingPlaneFallbackSolutionLarge (plane, origin);
	}
	return code;
}


dgContactSolver::dgMinkReturnCode dgContactSolver::UpdateSeparatingPlane(dgMinkFace*& plane, const dgVector& origin)
{
	dgVector diff[4];
	dgVector aveg[4];

	plane = NULL;
	dgMinkFace* face = &m_simplex[0];
	dgMinkFace* lastDescendFace = NULL;
	dgMinkReturnCode code = dgMinkIntersecting;

	// this loop can calculate the closest point to the origin usually in 4 to 5 passes,
	dgInt32 j = 0;
	dgInt32 ciclingCount = -1;
	dgFloat32 minDist = dgFloat32 (1.0e20f);
	for (; face && (j < DG_UPDATE_SEPARATING_PLANE_MAX_ITERATION); j ++) {
		face = NULL;
		dgVector normal;
		// initialize distance to zero (very important)
		dgFloat32 maxDist = dgFloat32 (0.0f);
		for (dgInt32 i = 0; i < 4; i ++) {
			dgInt32 i0 = m_faceIndex[i][0];
			dgInt32 i1 = m_faceIndex[i][1];
			dgInt32 i2 = m_faceIndex[i][2];

			_ASSERTE (i0 == m_simplex[i].m_vertex[0]);
			_ASSERTE (i1 == m_simplex[i].m_vertex[1]);
			_ASSERTE (i2 == m_simplex[i].m_vertex[2]);

			const dgVector& p0 = m_hullVertex[i0];
			const dgVector& p1 = m_hullVertex[i1];
			const dgVector& p2 = m_hullVertex[i2];
			dgVector e0 (p1 - p0);
			dgVector e1 (p2 - p0);
			dgVector n (e0 * e1);

			dgFloat32 dist = n % n;
			if (dist > DG_DISTANCE_TOLERANCE_ZERO) {
				n = n.Scale (dgRsqrt (dist));
				dist = n % (origin - p0);

				// find the plane farther away from the origin
				if (dist > maxDist) {
					maxDist = dist;
					normal = n;
					face = &m_simplex[i];
				}
			}
		}


		// if we do not have a face at this point it means that the mink shape of the tow convexity face have a very 
		// skew ratios on floating point accuracy is not enough to guarantee convexity of the shape
		if (face) {
			dgInt32 index = face->m_vertex[0];
			CalcSupportVertex (normal, 4);
			dgFloat32 dist = normal % (m_hullVertex[4] - m_hullVertex[index]);

			// if we are doing too many passes it means that it is a skew shape with big and small floats  
			// significant bits may be lost in dist calculation, increasing the tolerance help to resolve the problem
			if(dist < DG_UPDATE_SEPARATING_PLANE_DISTANCE_TOLERANCE1) {
				plane = face;
				code = dgMinkDisjoint;
				break;
			}

			if (dist < minDist) {
				minDist = dist;
				lastDescendFace = face;
				ciclingCount = -1;
				for (dgInt32 k = 0; k < 4; k ++) {
					diff[k] = m_hullVertex[k];
					aveg[k] = m_averVertex[k];
				}
			}

			ciclingCount ++;
			if (ciclingCount > 4) {
				for (dgInt32 k = 0; k < 4; k ++) {
					m_hullVertex[k] = diff[k];
					m_averVertex[k] = aveg[k];
				}
				code = dgMinkDisjoint;
				plane = lastDescendFace;
				break;
			}


			if (dist < DG_DISTANCE_TOLERANCE) {
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
			Swap (m_hullVertex[i0], m_hullVertex[i1]);
			Swap (m_averVertex[i0], m_averVertex[i1]);
			m_hullVertex[i2] = m_hullVertex[4];
			m_averVertex[i2] = m_averVertex[4];
			if (!CheckTetrahedronVolume ()) {
				Swap (m_hullVertex[1], m_hullVertex[2]);
				Swap (m_averVertex[1], m_averVertex[2]);
				_ASSERTE (CheckTetrahedronVolume ());
			}
		}
	} 

	if (j >= DG_UPDATE_SEPARATING_PLANE_MAX_ITERATION) {
		_ASSERTE (CheckTetrahedronVolume());
		code = UpdateSeparatingPlaneFallbackSolution (plane, origin);
	}
	return code;
}


dgContactSolver::dgMinkReturnCode dgContactSolver::CalcSeparatingPlaneLarge(dgMinkFace*& plane, const dgBigVector& origin)
{
	dgFloat64 error2;
	dgBigVector e1;
	dgBigVector e2;
	dgBigVector e3;
	dgBigVector normal (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));

	CalcSupportVertexLarge (m_dir[0], 0);
	dgInt32 i = 1;
	for (; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
		dgFloat64 error2;
		CalcSupportVertexLarge (m_dir[i], 1);
		e1 = m_hullVertexLarge[1] - m_hullVertexLarge[0];
		error2 = e1 % e1;
		if (error2 > DG_CALCULATE_SEPARATING_PLANE_ERROR) {
			break;
		}
	}

	for (i ++; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
		dgFloat64 error2;
		CalcSupportVertexLarge (m_dir[i], 2);
		e2 = m_hullVertexLarge[2] - m_hullVertexLarge[0];
		normal = e1 * e2;
		error2 = normal % normal;
		if (error2 > DG_CALCULATE_SEPARATING_PLANE_ERROR1) {
			break;
		}
	}

	error2 = dgFloat32 (0.0f);
	for (i ++; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
		CalcSupportVertexLarge (m_dir[i], 3);
		e3 = m_hullVertexLarge[3] - m_hullVertexLarge[0];
		error2 = normal % e3;
		if (fabs (error2) > DG_CALCULATE_SEPARATING_PLANE_ERROR1) {
			break;
		}
	}


	if (i >= dgInt32(sizeof(m_dir) / sizeof(m_dir[0]))) {
		dgInt32 best;
		dgFloat64 maxErr;

		best = 0;
		maxErr = dgFloat32 (0.0f);
		for (i = 1; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
			CalcSupportVertexLarge (m_dir[i], 1);
			e1 = m_hullVertexLarge[1] - m_hullVertexLarge[0];
			error2 = e1 % e1;
			if (error2 > maxErr) {
				best = i;
				maxErr = error2;
			}
		}
		CalcSupportVertexLarge (m_dir[best], 1);
		e1 = m_hullVertexLarge[1] - m_hullVertexLarge[0];

		best = 0;
		maxErr = dgFloat32 (0.0f);
		for (i = 1; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
			CalcSupportVertexLarge (m_dir[i], 2);
			dgBigVector e2 (m_hullVertexLarge[2] - m_hullVertexLarge[0]);
			normal = e1 * e2;
			error2 = normal % normal;
			if (error2 > maxErr) {
				best = i;
				maxErr = error2;
			}
		}

		CalcSupportVertexLarge (m_dir[best], 2);
		dgBigVector e2 (m_hullVertexLarge[2] - m_hullVertexLarge[0]);
		normal = e1 * e2;

		best = 0;
		maxErr = dgFloat32 (0.0f);
		for (i = 1; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
			CalcSupportVertexLarge (m_dir[i], 3);

			dgBigVector e3 (m_hullVertexLarge[3] - m_hullVertexLarge[0]);
			error2 = normal % e3;
			if (fabs (error2) > fabs (maxErr)) {
				best = i;
				maxErr = error2;
			}
		}
		error2 = maxErr;
		CalcSupportVertexLarge (m_dir[best], 3);
	}


	m_vertexIndex = 4;
	if (error2 > dgFloat32 (0.0f)) {
		Swap (m_hullVertexLarge[1], m_hullVertexLarge[2]);
		Swap (m_averVertexLarge[1], m_averVertexLarge[2]);
	}
	_ASSERTE (CheckTetrahedronVolumeLarge ());

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

	return UpdateSeparatingPlaneLarge(plane, origin);
}



dgContactSolver::dgMinkReturnCode dgContactSolver::CalcSeparatingPlane(dgMinkFace*& plane, const dgVector& origin)
{
	dgInt32 best;
	dgFloat32 maxErr;
	dgFloat32 error2;
	dgVector e1;
	dgVector e2;
	dgVector e3;
	dgVector normal (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));

	CalcSupportVertex (m_dir[0], 0);
	dgInt32 i = 1;
	for (; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
		CalcSupportVertex (m_dir[i], 1);
		e1 = m_hullVertex[1] - m_hullVertex[0];
		error2 = e1 % e1;
		if (error2 > DG_CALCULATE_SEPARATING_PLANE_ERROR) {
			break;
		}
	}

	for (i ++; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
		CalcSupportVertex (m_dir[i], 2);
		e2 = m_hullVertex[2] - m_hullVertex[0];
		normal = e1 * e2;
		error2 = normal % normal;
		if (error2 > DG_CALCULATE_SEPARATING_PLANE_ERROR1) {
			break;
		}
	}

	error2 = dgFloat32 (0.0f);
	for (i ++; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
		CalcSupportVertex (m_dir[i], 3);
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
			CalcSupportVertex (m_dir[i], 1);
			e1 = m_hullVertex[1] - m_hullVertex[0];
			error2 = e1 % e1;
			if (error2 > maxErr) {
				best = i;
				maxErr = error2;
			}
		}
		CalcSupportVertex (m_dir[best], 1);
		e1 = m_hullVertex[1] - m_hullVertex[0];

		best = 0;
		maxErr = dgFloat32 (0.0f);
		for (i = 1; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
			CalcSupportVertex (m_dir[i], 2);
			e2 = m_hullVertex[2] - m_hullVertex[0];
			normal = e1 * e2;
			error2 = normal % normal;
			if (error2 > maxErr) {
				best = i;
				maxErr = error2;
			}
		}

		CalcSupportVertex (m_dir[best], 2);
		e2 = m_hullVertex[2] - m_hullVertex[0];
		normal = e1 * e2;

		best = 0;
		maxErr = dgFloat32 (0.0f);
		for (i = 1; i < dgInt32(sizeof(m_dir) / sizeof(m_dir[0])); i ++) {
			CalcSupportVertex (m_dir[i], 3);

			e3 = m_hullVertex[3] - m_hullVertex[0];
			error2 = normal % e3;
			if (dgAbsf (error2) > dgAbsf (maxErr)) {
				best = i;
				maxErr = error2;
			}
		}
		error2 = maxErr;
		CalcSupportVertex (m_dir[best], 3);
	}


	m_vertexIndex = 4;
	if (error2 > dgFloat32 (0.0f)) {
		Swap (m_hullVertex[1], m_hullVertex[2]);
		Swap (m_averVertex[1], m_averVertex[2]);
	}
	_ASSERTE (CheckTetrahedronVolume ());

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

	return UpdateSeparatingPlane(plane, origin);
}



dgInt32 dgContactSolver::CalculateClosestPoints ()
{
	dgMinkFace* face;
	//dgMinkReturnCode code;

	dgVector contactA (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgVector contactB (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
	dgMinkReturnCode code = CalcSeparatingPlane(face);
	if (code == dgMinkDisjoint) {
		_ASSERTE (face);
		dgVector hullVertex[3];
		dgVector averVertex[3];

		for (dgInt32 i = 0; i < 3; i ++) {
			hullVertex[i] = m_hullVertex[face->m_vertex[i]];
			averVertex[i] = m_averVertex[face->m_vertex[i]];
		}
		for (dgInt32 i = 0; i < 3; i ++) {
			m_hullVertex[i] = hullVertex[i];
			m_averVertex[i] = averVertex[i];
		}

		m_vertexIndex = 3;
		dgVector dir0 (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
		dgVector origin (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f));
		dgVector v (ReduceTriangle (origin));
		for (dgInt32 i = 0; (i < 32) && (m_vertexIndex < 4); i ++) {
			//				dist = v % v + dgFloat32 (1.0e-12f);
			dgFloat32 dist = v % v;
			dgVector dir (dir0);
			if (dist > dgFloat32 (1.0e-12f)) {
				_ASSERTE (dist > DG_DISTANCE_TOLERANCE_ZERO);

				dir = v.Scale (-dgRsqrt (dist));
				dist = dir0 % dir;
				if (dist < dgFloat32 (0.9995f)) {
					CalcSupportVertex (dir, m_vertexIndex);
					const dgVector& w = m_hullVertex[m_vertexIndex];
					dgVector wv (w - v);
					dist = dir % wv;
				} else {
					dist = dgFloat32 (0.0f);
				}
			} else {
				dist = dgFloat32 (0.0f);
			}

			dir0 = dir;

			//				_ASSERTE (0); // !!! this can be take outside the loop
			//				if (dist < dgFloat32 (5.0e-4f)) {
			if (dist < (DG_DISTANCE_TOLERANCE * dgFloat32 (0.5f))) 
			{
				switch (m_vertexIndex) 
				{
					case 1:
					{
						contactA = m_hullVertex[0] + m_averVertex[0];
						contactB = m_averVertex[0] - m_hullVertex[0];
						break;
					}

					case 2:
					{
						dgVector lp0 (m_hullVertex[0] + m_averVertex[0]);
						dgVector lp1 (m_hullVertex[1] + m_averVertex[1]);
						dgVector lr0 (m_averVertex[0] - m_hullVertex[0]);
						dgVector lr1 (m_averVertex[1] - m_hullVertex[1]);

						dgVector dp  (m_hullVertex[1] - m_hullVertex[0]);

						dgFloat32 alpha1 = - (m_hullVertex[0] % dp) / (dp % dp + dgFloat32 (1.0e-24f));
						dgFloat32 alpha0 = dgFloat32 (1.0f) - alpha1;
						_ASSERTE (alpha1 >= dgFloat32 (0.0f));
						_ASSERTE (alpha1 <= dgFloat32 (1.0f));

						contactA = lp0.Scale (alpha0) + lp1.Scale (alpha1);
						contactB = lr0.Scale (alpha0) + lr1.Scale (alpha1);
						break;
					}

					case 3:
					{
						//dgFloat32 b1;
						//dgFloat32 b2;
						//dgFloat32 a11;
						//dgFloat32 a22;
						//dgFloat32 a21;
						//dgFloat32 den;

						dgVector lp0 (m_hullVertex[0] + m_averVertex[0]);
						dgVector lp1 (m_hullVertex[1] + m_averVertex[1]);
						dgVector lp2 (m_hullVertex[2] + m_averVertex[2]);
						dgVector lr0 (m_averVertex[0] - m_hullVertex[0]);
						dgVector lr1 (m_averVertex[1] - m_hullVertex[1]);
						dgVector lr2 (m_averVertex[2] - m_hullVertex[2]);

						const dgVector& p0 = m_hullVertex[0];
						const dgVector& p1 = m_hullVertex[1];
						const dgVector& p2 = m_hullVertex[2];

						dgVector p10 (p1 - p0);
						dgVector p20 (p2 - p0);

						dgFloat32 a11 = p10 % p10;
						dgFloat32 a22 = p20 % p20;
						dgFloat32 a21 = p10 % p20;
						dgFloat32 b1 = - (p10 % p0);
						dgFloat32 b2 = - (p20 % p0);
						dgFloat32 den = a11 * a22 - a21 * a21;

						dgFloat32 alpha0;
						dgFloat32 alpha1;
						dgFloat32 alpha2;
						if (den > dgFloat32 (1.0e-7f)) {
							_ASSERTE (den > dgFloat32 (0.0f));
							alpha1 = b1 * a22 - a21 * b2;
							alpha2 = a11 * b2 - b1 * a21;
							alpha0 = den - alpha1 - alpha2;

							den = dgFloat32 (1.0f) / den;

							alpha0 *= den;
							alpha1 *= den;
							alpha2 *= den;
						} else {
							alpha0 = dgFloat32 (0.33f);
							alpha1 = dgFloat32 (0.33f);
							alpha2 = dgFloat32 (0.33f);
						}

						_ASSERTE (alpha0 >= dgFloat32 (-2.0e-2f));
						_ASSERTE (alpha1 >= dgFloat32 (-2.0e-2f));
						_ASSERTE (alpha2 >= dgFloat32 (-2.0e-2f));
						_ASSERTE (alpha0 <= dgFloat32 ( 1.0f + 2.0e-2f));
						_ASSERTE (alpha1 <= dgFloat32 ( 1.0f + 2.0e-2f));				
						_ASSERTE (alpha2 <= dgFloat32 ( 1.0f + 2.0e-2f));

						contactA = lp0.Scale (alpha0) + lp1.Scale (alpha1) + lp2.Scale (alpha2);
						contactB = lr0.Scale (alpha0) + lr1.Scale (alpha1) + lr2.Scale (alpha2);
						break;
					}
				}


				//contactA = m_referenceBody->m_collisionWorldMatrix.TransformVector (contactA.Scale (dgFloat32 (0.5f)));
				//contactB = m_referenceBody->m_collisionWorldMatrix.TransformVector (contactB.Scale (dgFloat32 (0.5f)));
				//dir = m_referenceBody->m_collisionWorldMatrix.RotateVector(dir);
				contactA = m_proxy->m_referenceMatrix.TransformVector (contactA.Scale (dgFloat32 (0.5f)));
				contactB = m_proxy->m_referenceMatrix.TransformVector (contactB.Scale (dgFloat32 (0.5f)));
				dir = m_proxy->m_referenceMatrix.RotateVector(dir);

				dgContactPoint* contact = m_proxy->m_contacts;
				contact[0].m_point.m_x = contactA.m_x;
				contact[0].m_point.m_y = contactA.m_y;
				contact[0].m_point.m_z = contactA.m_z;
				contact[1].m_point.m_x = contactB.m_x;
				contact[1].m_point.m_y = contactB.m_y;
				contact[1].m_point.m_z = contactB.m_z;

				contact[0].m_normal.m_x = dir.m_x;
				contact[0].m_normal.m_y = dir.m_y;
				contact[0].m_normal.m_z = dir.m_z;

				contact[1].m_normal.m_x = -dir.m_x;
				contact[1].m_normal.m_y = -dir.m_y;
				contact[1].m_normal.m_z = -dir.m_z;
				return 1;
			}

			m_vertexIndex ++;
			switch (m_vertexIndex) 
			{
				case 1:
				{
					_ASSERTE (0);
					break;
				}

				case 2:
				{
					v = ReduceLine (origin);	
					break;

				}

				case 3:
				{
					v = ReduceTriangle (origin);
					break;
				}

				case 4:
				{
					v = ReduceTetrahedrum (origin);
					break;
				}
			}
		}
	}

	//_ASSERTE (i < CLOSE_DISTANCE_MAX_ITERATION);
	_ASSERTE (code != dgMinkError);
	return 0;
}


dgContactSolver::dgPerimenterEdge* dgContactSolver::ReduceContacts (dgPerimenterEdge *poly, dgInt32 maxCount) const
{
	dgInt32 buffer [256];
	dgUpHeap<dgPerimenterEdge*, dgFloat32> heap (buffer, sizeof (buffer));	

	dgInt32 restart = 1;
	while (restart) {
		restart = 0;
		dgPerimenterEdge* ptr0 = poly; 
		poly = poly->m_next;
		if (poly->m_next != poly) {
			heap.Flush();
			dgPerimenterEdge* ptr = poly; 
			do {
				dgFloat32 dist2;
				dgVector error (*ptr->m_next->m_vertex - *ptr->m_vertex);
				dist2 = error % error;
				if (dist2 < DG_MIN_VERTEX_ERROR_2) {
					ptr0->m_next = ptr->m_next;
					if (ptr == poly) {
						poly = ptr0;
						restart = 1;
						break;
					} 
					ptr = ptr0;
				} else {
					heap.Push(ptr, dist2);
					ptr0 = ptr;
				}

				ptr = ptr->m_next;
			} while (ptr != poly);
		}
	}


	if (heap.GetCount()) {
		if (maxCount > 8) {
			maxCount = 8;
		}
		while (heap.GetCount() > maxCount) {
			//dgFloat32 dist2;
			dgPerimenterEdge* ptr = heap[0];
			heap.Pop();
			for (dgInt32 i = 0; i < heap.GetCount(); i ++) {
				if (heap[i] == ptr->m_next) {
					heap.Remove(i);
					break;
				}
			}

			ptr->m_next = ptr->m_next->m_next;
			dgVector error (*ptr->m_next->m_vertex - *ptr->m_vertex);
			dgFloat32 dist2 = error % error;
			heap.Push(ptr, dist2);
		}
		poly = heap[0];
	}

	return poly;
}



dgInt32 dgContactSolver::CalculateConvexShapeIntersectionLine (const dgMatrix& matrix, const dgVector& shapeNormal, dgUnsigned32 id, dgFloat32 penetration,
	dgInt32 shape1VertexCount, dgVector* const shape1, dgInt32 shape2VertexCount, dgVector* const shape2, dgContactPoint* const contactOut) const
{
	dgInt32 count = 0;
	dgVector* output = (dgVector*) &m_hullVertex[shape1VertexCount + shape2VertexCount + 1];

	_ASSERTE (shape1VertexCount >= 3);
	_ASSERTE (shape2VertexCount <= 2);

	dgVector* ptr = NULL;
	// face line intersection
	if (shape2VertexCount == 2) {
		ptr = (dgVector*)&shape2[0];
		dgInt32 i0 = shape1VertexCount - 1;
		for (dgInt32 i1 = 0; i1 < shape1VertexCount; i1 ++) {
			dgVector n (shapeNormal * (shape1[i1] - shape1[i0]));
			_ASSERTE ((n % n) > dgFloat32 (0.0f));
			dgPlane plane (n, - (n % shape1[i0]));

			dgFloat32 test0 = plane.Evalue (ptr[0]);
			dgFloat32 test1 = plane.Evalue (ptr[1]);
			//		ForceFloatConsistency ();
			if (test0 >= dgFloat32 (0.0f)) {
				if (test1 >= dgFloat32 (0.0f)) {
					output[count + 0] = ptr[0];
					output[count + 1] = ptr[1];
					count += 2;
				} else {
					dgVector dp (ptr[1] - ptr[0]);
					dgFloat32 den = plane % dp;
					if (dgAbsf(den) < 1.0e-10f) {
						den = 1.0e-10f;
					}
					output[count + 0] = ptr[0];
					output[count + 1] = ptr[0] - dp.Scale (test0 / den);
					count += 2;
				}
			} else if (test1 >= dgFloat32 (0.0f)) {
				dgVector dp (ptr[1] - ptr[0]);
				dgFloat32 den = plane % dp;
				if (dgAbsf(den) < 1.0e-10f) {
					den = 1.0e-10f;
				}
				output[count] = ptr[0] - dp.Scale (test0 / den);
				count ++;
				output[count] = ptr[1];
				count ++;
			} else {
				return 0;
			}


			shape2VertexCount = count;
			ptr = output;
			output = &output[count];
			count = 0;
			i0 = i1;
		}

	} else {
		shape2VertexCount = 0;
	}

	dgVector normal = matrix.RotateVector(shapeNormal);
	for (dgInt32 i0 = 0; i0 < shape2VertexCount; i0 ++) {
		contactOut[i0].m_point = matrix.TransformVector(ptr[i0]);
		contactOut[i0].m_normal = normal;
		contactOut[i0].m_penetration = penetration;
		contactOut[i0].m_userId = id;
	}

	return shape2VertexCount;
}


dgInt32 dgContactSolver::CalculateConvexShapeIntersection (const dgMatrix& matrix, const dgVector& shapeNormal, dgUnsigned32 id, dgFloat32 penetration,
	dgInt32 shape1VertexCount, dgVector* const shape1, dgInt32 shape2VertexCount, dgVector* const shape2, dgContactPoint* const contactOut, dgInt32 maxContacts) const
{

	dgInt32 count = 0;
	if (shape2VertexCount <= 2) {
		count = CalculateConvexShapeIntersectionLine (matrix, shapeNormal, id, penetration, 
			shape1VertexCount, shape1, shape2VertexCount, shape2, contactOut);

		if (count > maxContacts) {
			count = maxContacts;
		}
	} else if (shape1VertexCount <= 2) {
		count = CalculateConvexShapeIntersectionLine (matrix, shapeNormal, id, penetration, 
			shape2VertexCount, shape2, shape1VertexCount, shape1, contactOut);
		if (count > maxContacts) {
			count = maxContacts;
		}

	} else {
		dgPerimenterEdge *edgeClipped[2];
		dgPerimenterEdge subdivision[128];

		_ASSERTE (shape1VertexCount >= 3);
		_ASSERTE (shape2VertexCount >= 3);
		dgVector* output = (dgVector*) &m_hullVertex[shape1VertexCount + shape2VertexCount];

		//ptr = NULL;
		_ASSERTE ((shape1VertexCount + shape2VertexCount) < sizeof (subdivision) / (2 * sizeof (subdivision[0])));
		for (dgInt32 i0 = 0; i0 < shape2VertexCount; i0 ++) {
			subdivision[i0].m_vertex = &shape2[i0];
			subdivision[i0].m_prev = &subdivision[i0 - 1];
			subdivision[i0].m_next = &subdivision[i0 + 1];
		}
		//i0 --;
		//subdivision[0].m_prev = &subdivision[i0];
		//subdivision[i0].m_next = &subdivision[0];
		subdivision[0].m_prev = &subdivision[shape2VertexCount - 1];
		subdivision[shape2VertexCount - 1].m_next = &subdivision[0];

		dgPerimenterEdge* poly = &subdivision[0];
		edgeClipped[0] = NULL;
		edgeClipped[1] = NULL;
		dgInt32 i0 = shape1VertexCount - 1;
		dgInt32 edgeIndex = shape2VertexCount;
		for (dgInt32 i1 = 0; i1 < shape1VertexCount; i1 ++) {
			dgVector n (shapeNormal * (shape1[i1] - shape1[i0]));
			dgPlane plane (n, - (n % shape1[i0]));
			i0 = i1;

			count = 0;
			dgPerimenterEdge* tmp = poly;
			dgInt32 isInside = 0;
			dgFloat32 test0 = plane.Evalue (*tmp->m_vertex);
			do {
				dgFloat32 test1 = plane.Evalue (*tmp->m_next->m_vertex);
				if (test0 >= dgFloat32 (0.0f)) {
					isInside |= 1;
					if (test1 < dgFloat32 (0.0f)) {
						const dgVector& p0 = *tmp->m_vertex;
						const dgVector& p1 = *tmp->m_next->m_vertex;
						dgVector dp (p1 - p0); 
						dgFloat32 den = plane % dp;
						if (dgAbsf(den) < dgFloat32 (1.0e-24f)) {
							den = dgFloat32 (1.0e-24f) * (den > dgFloat32 (0.0f)) ? dgFloat32 (1.0f) : dgFloat32 (-1.0f);
						}

						den = test0 / den;
						if (den >= dgFloat32 (0.0f)) {
							den = dgFloat32 (0.0f);
						} else if (den <= -1.0f) {
							den = dgFloat32 (-1.0f);
						}
						output[0] = p0 - dp.Scale (den);
						edgeClipped[0] = tmp;
						count ++;
					}
				} else if (test1 >= dgFloat32 (0.0f)) {
					const dgVector& p0 = *tmp->m_vertex;
					const dgVector& p1 = *tmp->m_next->m_vertex;
					isInside |= 1;
					dgVector dp (p1 - p0); 
					dgFloat32 den = plane % dp;
					if (dgAbsf(den) < dgFloat32 (1.0e-24f)) {
						den = dgFloat32 (1.0e-24f) * (den > dgFloat32 (0.0f)) ? dgFloat32 (1.0f) : dgFloat32 (-1.0f);
					}
					den = test0 / den;
					if (den >= dgFloat32 (0.0f)) {
						den = dgFloat32 (0.0f);
					} else if (den <= -1.0f) {
						den = dgFloat32 (-1.0f);
					}
					output[1] = p0 - dp.Scale (den);
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
			p0-= dgVector (2.5f, 2.5f, 2.5f, 0.0f);
			p1+= dgVector (2.5f, 2.5f, 2.5f, 0.0f);
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

		//dgPerimenterEdge *intersection;
		count = 0;
		dgPerimenterEdge* intersection = poly;
		dgVector normal = matrix.RotateVector(shapeNormal);
		do {
			contactOut[count].m_point = matrix.TransformVector(*intersection->m_vertex);
			contactOut[count].m_normal = normal;
			contactOut[count].m_penetration = penetration;
			contactOut[count].m_userId = id;
			count ++;
			intersection = intersection->m_next;
		} while (intersection != poly);

	}

	return count;
}



dgInt32 dgContactSolver::CalculateContactAlternateMethod(dgMinkFace* const face, dgInt32 contacID, dgContactPoint* const contactOut, dgInt32 maxContacts)
{

	dgInt32 count = 0;
	// Get the contact form the last face
	const dgPlane& plane = *face;
	dgFloat32 penetration = plane.m_w - m_penetrationPadding;
	dgFloat32 dist = (plane % m_averVertex[face->m_vertex[0]]) * dgFloat32 (0.5f);
	const dgPlane clipPlane (plane.Scale(dgFloat32 (-1.0f)), dist);

	dgVector point1 (clipPlane.Scale (-clipPlane.m_w));
	dgVector* const shape1 = m_hullVertex;

	dgVector p1 (m_referenceCollision->SupportVertex (clipPlane.Scale (dgFloat32 (-1.0f))));
	p1 += clipPlane.Scale (DG_ROBUST_PLANE_CLIP);
	dgInt32 count1 = m_referenceCollision->CalculatePlaneIntersection (clipPlane, p1, shape1);
	dgVector err (clipPlane.Scale (clipPlane % (point1 - p1)));
	for (dgInt32 i = 0; i < count1; i ++) {
		shape1[i] += err;
	}

	//		_ASSERTE (penetration <= dgFloat32 (2.0e-1f));
	dist = GetMax (- (penetration + DG_IMPULSIVE_CONTACT_PENETRATION), dgFloat32 (0.0f));
	if (count1) {

		dgVector* const shape2 = &m_hullVertex[count1];
		const dgPlane clipPlane2 (m_matrix.UntransformPlane(clipPlane));

		dgVector point2 (clipPlane2.Scale (-clipPlane2.m_w));
		dgVector p2 (m_floatingcollision->SupportVertex (clipPlane2.Scale(dgFloat32 (-1.0f))));
		p2 += clipPlane2.Scale (DG_ROBUST_PLANE_CLIP);
		dgInt32 count2 = m_floatingcollision->CalculatePlaneIntersection (clipPlane2, p2, shape2);
		dgVector err (clipPlane2.Scale (clipPlane2 % (point2 - p2)));
		for (dgInt32 i = 0; i < count2; i ++) {
			shape2[i] += err;
		}

		if (count2) {
			_ASSERTE (count1);
			_ASSERTE (count2);

			if (count1 == 1) {
				const dgMatrix& matrix1 = m_proxy->m_referenceMatrix;

				count = 1;
				contactOut[0].m_point = matrix1.TransformVector (shape1[0]);
				contactOut[0].m_normal = matrix1.RotateVector (clipPlane);
				contactOut[0].m_userId = contacID;
				contactOut[0].m_penetration = dist;
			} else if (count2 == 1) {
				const dgMatrix& matrix1 = m_proxy->m_referenceMatrix;
				const dgMatrix& matrix2 = m_proxy->m_floatingMatrix;

				count = 1;
				contactOut[0].m_point = matrix2.TransformVector (shape2[0]);
				contactOut[0].m_normal = matrix1.RotateVector (clipPlane);
				contactOut[0].m_userId = contacID;
				contactOut[0].m_penetration = dist;

			} else if ((count1 == 2) && (count2 == 2)) {
				const dgMatrix& matrix1 = m_proxy->m_referenceMatrix;
				dgVector p0 (shape1[0]); 
				dgVector p1 (shape1[1]); 
				dgVector q0 (m_matrix.TransformVector(shape2[0])); 
				dgVector q1 (m_matrix.TransformVector(shape2[1])); 
				dgVector p10 (p1 - p0);
				dgVector q10 (q1 - q0);
				p10 = p10.Scale (dgFloat32 (1.0f) / dgSqrt (p10 % p10 + dgFloat32 (1.0e-8f)));
				q10 = q10.Scale (dgFloat32 (1.0f) / dgSqrt (q10 % q10 + dgFloat32 (1.0e-8f)));
				dgFloat32 dot = q10 % p10;
				if (dgAbsf (dot) > dgFloat32 (0.998f)) {
					//dgFloat32 pl0;
					//dgFloat32 pl1;
					//dgFloat32 ql0;
					//dgFloat32 ql1;
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
						//dgFloat32 clip0;
						//dgFloat32 clip1;
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
				for (dgInt32 i = 0; i < count; i ++) {
					contactOut[i].m_point = matrix1.TransformVector(contactOut[i].m_point);
				}

			} else {
				const dgMatrix& matrix1 = m_proxy->m_referenceMatrix;
				m_matrix.TransformTriplex(&shape2[0].m_x, sizeof (dgVector), &shape2[0].m_x, sizeof (dgVector), count2);
				count = CalculateConvexShapeIntersection (matrix1, clipPlane, dgUnsigned32 (contacID), dist, count1, shape1, count2, shape2, contactOut, maxContacts);
			}
		}
	}

	return count;
}



dgInt32 dgContactSolver::CalculateContacts(dgMinkFace* const face, dgInt32 contacID, dgContactPoint* const contactOut, dgInt32 maxContacts)
{
	dgInt32 count = 0;
	// Get the contact form the last face
	const dgPlane& plane = *face;
	dgFloat32 penetration = plane.m_w - m_penetrationPadding;
	dgFloat32 dist = (plane % m_averVertex[face->m_vertex[0]]) * dgFloat32 (0.5f);
	const dgPlane clipPlane (plane.Scale(dgFloat32 (-1.0f)), dist);

	dgVector point1 (clipPlane.Scale (-clipPlane.m_w));
	dgVector* const shape1 = m_hullVertex;
	dgInt32 count1 = m_referenceCollision->CalculatePlaneIntersection (clipPlane, point1, shape1);
	if (!count1) {
		dgVector p1 (m_referenceCollision->SupportVertex (clipPlane.Scale (dgFloat32 (-1.0f))));
		p1 += clipPlane.Scale (DG_ROBUST_PLANE_CLIP);
		count1 = m_referenceCollision->CalculatePlaneIntersection (clipPlane, p1, shape1);
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
		dgInt32 count2 = m_floatingcollision->CalculatePlaneIntersection (clipPlane2, point2, shape2);
		if (!count2) {
			dgVector p2 (m_floatingcollision->SupportVertex (clipPlane2.Scale(dgFloat32 (-1.0f))));
			p2 += clipPlane2.Scale (DG_ROBUST_PLANE_CLIP);
			count2 = m_floatingcollision->CalculatePlaneIntersection (clipPlane2, p2, shape2);
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
				contactOut[0].m_point = matrix1.TransformVector (shape1[0]);
				contactOut[0].m_normal = matrix1.RotateVector (clipPlane);
				contactOut[0].m_userId = contacID;
				contactOut[0].m_penetration = dist;
			} else if (count2 == 1) {
				const dgMatrix& matrix1 = m_proxy->m_referenceMatrix;
				//const dgMatrix& matrix2 = m_proxy->m_floatingMatrix;

				count = 1;
				//					contactOut[0].m_point = matrix2.TransformVector (shape2[0]);
				contactOut[0].m_point =  matrix1.TransformVector (m_matrix.TransformVector(shape2[0]));
				contactOut[0].m_normal = matrix1.RotateVector (clipPlane);
				contactOut[0].m_userId = contacID;
				contactOut[0].m_penetration = dist;

			} else if ((count1 == 2) && (count2 == 2)) {
				const dgMatrix& matrix1 = m_proxy->m_referenceMatrix;
				dgVector p0 (shape1[0]); 
				dgVector p1 (shape1[1]); 
				dgVector q0 (m_matrix.TransformVector(shape2[0])); 
				dgVector q1 (m_matrix.TransformVector(shape2[1])); 
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
					contactOut[i].m_point = matrix1.TransformVector(contactOut[i].m_point);
				}

			} else {
				const dgMatrix& matrix1 = m_proxy->m_referenceMatrix;
				m_matrix.TransformTriplex(&shape2[0].m_x, sizeof (dgVector), &shape2[0].m_x, sizeof (dgVector), count2);
				count = CalculateConvexShapeIntersection (matrix1, clipPlane, dgUnsigned32 (contacID), dist, count1, shape1, count2, shape2, contactOut, maxContacts);
				if (!count) {
					count = CalculateContactAlternateMethod(face, contacID, contactOut, maxContacts);
				}
			}

			dgInt32 edgeContactFlag = (m_floatingcollision->IsEdgeIntersection() | m_referenceCollision->IsEdgeIntersection()) ? 1 : 0;
			for (dgInt32 i = 0; i < count; i ++) {
				contactOut[i].m_isEdgeContact = edgeContactFlag;
			}
		}
	}
	return count;
}


dgContactSolver::dgMinkFace* dgContactSolver::CalculateClipPlaneLarge ()
{
//	dgInt32 i;
//	dgInt32 i0;
//	dgInt32 i1;
//	dgInt32 i2;
//	dgInt32 stack;
//	dgInt32 cicling;
//	dgInt32 deadCount;
//	dgInt32 adjacentIndex;
//	dgInt32 prevEdgeIndex;
//	dgInt32 silhouetteCapCount;
//	dgInt32 lastSilhouetteVertex;

//	dgFloat64 dist;
//	dgFloat64 minValue;
//	dgFloat64 penetration;
//	
//	dgMinkFace *face;
//	dgMinkFace *adjacent;
//	dgMinkFace *prevFace;
//	dgMinkFace *firstFace;
//	dgMinkFace *silhouette;
//	dgMinkFace *lastSilhouette;
//	dgMinkFace *closestFace;
//	dgMinkFacePurge* nextPurge;
	dgFloat64 ciclingMem[4];
	dgMinkFace* stackPool[128];
	dgMinkFace* deadFaces[128];
	SilhouetteFaceCap sillueteCap[128];
	dgBigVector diff[3];
	dgBigVector aver[3];
	dgInt8  buffer[DG_HEAP_EDGE_COUNT * (sizeof (dgFloat32) + sizeof (dgMinkFace *))];
	dgClosestFace heapSort (buffer, sizeof (buffer));

	m_planeIndex = 4;
	dgMinkFace* closestFace = NULL;
	m_facePurge = NULL;
//	dgFloat64 penetration = dgFloat64 (0.0f);

	_ASSERTE (m_vertexIndex == 4);
	for (dgInt32 i = 0; i < 4; i ++) {
		dgMinkFace* face = &m_simplex[i];
		face->m_inHeap = 0;
		face->m_isActive = 1;
		if (CalcFacePlaneLarge (face)) {
			face->m_inHeap = 1;
			heapSort.Push(face, face->m_w);
		}
	}

	dgInt32 cicling = 0;
	ciclingMem[0] = dgFloat32 (1.0e10f);
	ciclingMem[1] = dgFloat32 (1.0e10f);
	ciclingMem[2] = dgFloat32 (1.0e10f);
	ciclingMem[3] = dgFloat32 (1.0e10f);

	dgFloat64 minValue = dgFloat32 ( 1.0e10f);
	dgPlane bestPlane (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f),dgFloat32 (0.0f));
	diff[0] = dgBigVector (dgFloat64 (0.0f), dgFloat64 (0.0f), dgFloat64 (0.0f),dgFloat64 (0.0f));
	diff[1] = diff[0];
	diff[2] = diff[0];
	aver[0] = diff[0];
	aver[1] = diff[0];
	aver[2] = diff[0];

	dgMinkFace* face = NULL;
	while (heapSort.GetCount()) {
		face = heapSort[0];
		face->m_inHeap = 0;
		heapSort.Pop();

		if (face->m_isActive) {

			const dgPlane& plane = *face;

			CalcSupportVertexLarge (plane, m_vertexIndex);
			dgVector p (dgFloat32 (m_hullVertexLarge[m_vertexIndex].m_x), dgFloat32 (m_hullVertexLarge[m_vertexIndex].m_y), dgFloat32 (m_hullVertexLarge[m_vertexIndex].m_z), dgFloat64(0.0f));
			dgFloat64 dist = plane.Evalue (p);
			m_vertexIndex ++;

			if (m_vertexIndex > 16) {
				if (dist < minValue) {
					if (dist >= dgFloat64 (0.0f)) {
						minValue = dist;
						bestPlane = plane;

						dgInt32 i = face->m_vertex[0];
						diff[0] = m_hullVertexLarge[i];
						aver[0] = m_averVertexLarge[i];

						i = face->m_vertex[1];
						diff[1] = m_hullVertexLarge[i];
						aver[1] = m_averVertexLarge[i];

						i = face->m_vertex[2];
						diff[2] = m_hullVertexLarge[i];
						aver[2] = m_averVertexLarge[i];
					}
				}
			}

			ciclingMem[cicling] = dist;
			cicling = (cicling + 1) & 3;
			dgInt32 i = 0;
			for (; i < 4; i ++) {
				if (fabs (dist - ciclingMem[i]) > dgFloat64 (1.0e-6f)) {
					break;
				}
			}
			if (i == 4) {
				dist = dgFloat64 (0.0f);
			}


			if ((m_vertexIndex > DG_MINK_MAX_POINTS) ||
				(m_planeIndex > DG_MINK_MAX_FACES) ||
				(heapSort.GetCount() > (DG_HEAP_EDGE_COUNT - 24))) {

					//					dgTrace (("Max face count overflow, breaking with last best face\n"));

					dgPlane& plane = *face;
					plane = bestPlane;

					i = face->m_vertex[0];
					face->m_vertex[0] = 0;
					m_hullVertexLarge[i] = diff[0];
					m_averVertexLarge[i] = aver[0];

					i = face->m_vertex[1];
					face->m_vertex[1] = 1;
					m_hullVertexLarge[i] = diff[1];
					m_averVertexLarge[i] = aver[1];

					i = face->m_vertex[2];
					face->m_vertex[2] = 2;
					m_hullVertexLarge[i] = diff[2];
					m_averVertexLarge[i] = aver[2];
					dist = dgFloat32 (0.0f);
			}


			if (dist < (dgFloat32 (DG_IMPULSIVE_CONTACT_PENETRATION) / dgFloat32 (16.0f))) {
				_ASSERTE (m_planeIndex <= DG_MINK_MAX_FACES_SIZE);
				_ASSERTE (heapSort.GetCount() <= DG_HEAP_EDGE_COUNT);
				closestFace = face;
				break;
			} else if (dist > dgFloat32 (0.0f)) {
				_ASSERTE (face->m_inHeap == 0);

				dgInt32 stack = 0;
				dgInt32 deadCount = 1;
				dgMinkFace* silhouette = NULL;
				deadFaces[0] = face;
				closestFace = face;
				face->m_isActive = 0;
				for (dgInt32 i = 0; i < 3; i ++) {
					dgMinkFace* const adjacent = &m_simplex[face->m_adjancentFace[i]];
					_ASSERTE (adjacent->m_isActive);
					dist = adjacent->Evalue (p);  
					if (dist > dgFloat64 (0.0f)) { 
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
					for (dgInt32 i = 0; i < 3; i ++) {
						dgMinkFace* const adjacent = &m_simplex[face->m_adjancentFace[i]];
						if (adjacent->m_isActive){
							dist = adjacent->Evalue (p);  
							if (dist > dgFloat64 (0.0f)) { 
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

				dgInt32 i2 = (m_vertexIndex - 1);
				dgMinkFace* const lastSilhouette = silhouette;
				_ASSERTE ((silhouette->m_adjancentFace[0] != silhouette->m_adjancentFace[1]) &&
					(silhouette->m_adjancentFace[0] != silhouette->m_adjancentFace[2]) &&
					(silhouette->m_adjancentFace[1] != silhouette->m_adjancentFace[2]));


				dgInt32 adjacentIndex = DG_GETADJACENTINDEX_ACTIVE (silhouette);
				face = NewFace();
				dgInt32 i0 = silhouette->m_vertex[adjacentIndex];
				dgInt32 i1 = silhouette->m_vertex[adjacentIndex + 1];

				face->m_vertex[0] = dgInt16 (i1);
				face->m_vertex[1] = dgInt16 (i0);
				face->m_vertex[2] = dgInt16 (i2);
				face->m_vertex[3] = face->m_vertex[0];
				face->m_adjancentFace[0] = dgInt16 (silhouette - m_simplex);
				face->m_inHeap = 0; 
				face->m_isActive = 1; 

				sillueteCap[0].m_face = face;
				sillueteCap[0].m_faceCopling = &silhouette->m_adjancentFace[adjacentIndex];
				dgInt32 silhouetteCapCount = 1;
				_ASSERTE (silhouetteCapCount < (sizeof (sillueteCap) / sizeof (sillueteCap[0])));
				do {
					silhouette = &m_simplex[silhouette->m_adjancentFace[adjacentIndex]];
					adjacentIndex = (DG_GETADJACENTINDEX_VERTEX(silhouette, i0)); 
				} while (!silhouette->m_isActive);

				dgMinkFace* prevFace = face;
				dgMinkFace* const firstFace = face;
				dgInt32 lastSilhouetteVertex = i0;
				dgInt32 prevEdgeIndex = dgInt32 (face - m_simplex);
				do {
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


				for (dgInt32 i = 0; i < deadCount; i ++) {
					if (!deadFaces[i]->m_inHeap){
						dgMinkFacePurge* const nextPurge = (dgMinkFacePurge*) deadFaces[i];
						nextPurge->m_next = m_facePurge;
						m_facePurge = nextPurge;
					}
				}

				while (heapSort.GetCount() && (!heapSort[0]->m_isActive)) {
					face = heapSort[0];
					heapSort.Pop();
					dgMinkFacePurge* const nextPurge = (dgMinkFacePurge*) face;
					nextPurge->m_next = m_facePurge;
					m_facePurge = nextPurge;
				}

				for (dgInt32 i = 0; i < silhouetteCapCount; i ++) {
					face = sillueteCap[i].m_face;
					*sillueteCap[i].m_faceCopling = dgInt16 (face - m_simplex);

					if (CalcFacePlaneLarge (face)) {
						face->m_inHeap = 1;
						heapSort.Push(face, face->m_w);
					}
				}
			}

		} else {
			_ASSERTE (0);
			dgMinkFacePurge* const nextPurge = (dgMinkFacePurge*) face;
			nextPurge->m_next = m_facePurge;
			m_facePurge = nextPurge;
		}
	}

	_ASSERTE (face);
	dgInt32 i = face->m_vertex[0];
	//m_hullVertex[i] = dgVector (dgFloat32 (m_hullVertexLarge[i].m_x), dgFloat32 (m_hullVertexLarge[i].m_y), dgFloat32 (m_hullVertexLarge[i].m_z), dgFloat32 (0.0f));
	m_averVertex[i] = dgVector (dgFloat32 (m_averVertexLarge[i].m_x), dgFloat32 (m_averVertexLarge[i].m_y), dgFloat32 (m_averVertexLarge[i].m_z), dgFloat32 (0.0f));;
	return closestFace;
}



dgContactSolver::dgMinkFace* dgContactSolver::CalculateClipPlane ()
{
/*
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
	
	dgMinkFace *face;
	dgMinkFace *adjacent;
	dgMinkFace *prevFace;
	dgMinkFace *firstFace;
	dgMinkFace *silhouette;
	dgMinkFace *lastSilhouette;
	dgMinkFace *closestFace;
	dgMinkFacePurge* nextPurge;
*/
	dgFloat32 ciclingMem[4];
	dgMinkFace *stackPool[128];
	dgMinkFace *deadFaces[128];
	SilhouetteFaceCap sillueteCap[128];
	dgVector diff[3];
	dgVector aver[3];
	dgInt8  buffer[DG_HEAP_EDGE_COUNT * (sizeof (dgFloat32) + sizeof (dgMinkFace *))];
	dgClosestFace heapSort (buffer, sizeof (buffer));

	m_planeIndex = 4;
	dgMinkFace* closestFace = NULL;
	m_facePurge = NULL;
	//dgFloat32 penetration = dgFloat32 (0.0f);

	_ASSERTE (m_vertexIndex == 4);
	for (dgInt32 i = 0; i < 4; i ++) {
		dgMinkFace* face = &m_simplex[i];
		face->m_inHeap = 0;
		face->m_isActive = 1;
		if (CalcFacePlane (face)) {
			face->m_inHeap = 1;
			heapSort.Push(face, face->m_w);
		}
	}

	dgInt32 cicling = 0;
	ciclingMem[0] = dgFloat32 (1.0e10f);
	ciclingMem[1] = dgFloat32 (1.0e10f);
	ciclingMem[2] = dgFloat32 (1.0e10f);
	ciclingMem[3] = dgFloat32 (1.0e10f);

	dgFloat32 minValue = dgFloat32 ( 1.0e10f);
	dgPlane bestPlane (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f),dgFloat32 (0.0f));
	diff[0] = bestPlane;
	diff[1] = bestPlane;
	diff[2] = bestPlane;
	aver[0] = bestPlane;
	aver[1] = bestPlane;
	aver[2] = bestPlane;

	dgMinkFace* face = NULL;
	while (heapSort.GetCount()) {
		face = heapSort[0];
		face->m_inHeap = 0;
		heapSort.Pop();

		if (face->m_isActive) {
			const dgPlane& plane = *face;

			CalcSupportVertex (plane, m_vertexIndex);
			const dgVector& p = m_hullVertex[m_vertexIndex];
			dgFloat32 dist = plane.Evalue (p);
			m_vertexIndex ++;

			if (m_vertexIndex > 16) {
				if (dist < minValue) {
					if (dist >= dgFloat32 (0.0f)) {
						minValue = dist;
						bestPlane = plane;

						dgInt32 i = face->m_vertex[0];
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
			dgInt32 i = 0;
			for (; i < 4; i ++) {
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

					//					dgTrace (("Max face count overflow, breaking with last best face\n"));

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

				dgInt32 stack = 0;
				dgInt32 deadCount = 1;
				dgMinkFace* silhouette = NULL;
				deadFaces[0] = face;
				closestFace = face;
				face->m_isActive = 0;
				for (i = 0; i < 3; i ++) {
					dgMinkFace* const adjacent = &m_simplex[face->m_adjancentFace[i]];
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
						dgMinkFace* const adjacent = &m_simplex[face->m_adjancentFace[i]];
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

				dgInt32 i2 = (m_vertexIndex - 1);
				dgMinkFace* const lastSilhouette = silhouette;
				_ASSERTE ((silhouette->m_adjancentFace[0] != silhouette->m_adjancentFace[1]) &&
					(silhouette->m_adjancentFace[0] != silhouette->m_adjancentFace[2]) &&
					(silhouette->m_adjancentFace[1] != silhouette->m_adjancentFace[2]));


				dgInt32 adjacentIndex = DG_GETADJACENTINDEX_ACTIVE (silhouette);
				face = NewFace();
				dgInt32 i0 = silhouette->m_vertex[adjacentIndex];
				dgInt32 i1 = silhouette->m_vertex[adjacentIndex + 1];

				face->m_vertex[0] = dgInt16 (i1);
				face->m_vertex[1] = dgInt16 (i0);
				face->m_vertex[2] = dgInt16 (i2);
				face->m_vertex[3] = face->m_vertex[0];
				face->m_adjancentFace[0] = dgInt16 (silhouette - m_simplex);
				face->m_inHeap = 0; 
				face->m_isActive = 1; 

				sillueteCap[0].m_face = face;
				sillueteCap[0].m_faceCopling = &silhouette->m_adjancentFace[adjacentIndex];
				dgInt32 silhouetteCapCount = 1;
				_ASSERTE (silhouetteCapCount < (sizeof (sillueteCap) / sizeof (sillueteCap[0])));
				do {
					silhouette = &m_simplex[silhouette->m_adjancentFace[adjacentIndex]];
					adjacentIndex = (DG_GETADJACENTINDEX_VERTEX(silhouette, i0)); 
				} while (!silhouette->m_isActive);

				dgMinkFace* prevFace = face;
				dgMinkFace* const firstFace = face;
				dgInt32 lastSilhouetteVertex = i0;
				dgInt32 prevEdgeIndex = dgInt32 (face - m_simplex);
				do {
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
						dgMinkFacePurge* const nextPurge = (dgMinkFacePurge*) deadFaces[i];
						nextPurge->m_next = m_facePurge;
						m_facePurge = nextPurge;
					}
				}

				while (heapSort.GetCount() && (!heapSort[0]->m_isActive)) {
					face = heapSort[0];
					heapSort.Pop();
					dgMinkFacePurge* const nextPurge = (dgMinkFacePurge*) face;
					nextPurge->m_next = m_facePurge;
					m_facePurge = nextPurge;
				}

				for (i = 0; i < silhouetteCapCount; i ++) {
					face = sillueteCap[i].m_face;
					*sillueteCap[i].m_faceCopling = dgInt16 (face - m_simplex);

					if (CalcFacePlane (face)) {
						face->m_inHeap = 1;
						heapSort.Push(face, face->m_w);
					}
				}
			}
		} else {
			_ASSERTE (0);
			dgMinkFacePurge* const nextPurge = (dgMinkFacePurge*) face;
			nextPurge->m_next = m_facePurge;
			m_facePurge = nextPurge;
		}
	}
	return closestFace;
}




dgInt32 dgContactSolver::HullHullContinueContacts (dgFloat32& timeStep, dgContactPoint* const contactOut, dgInt32 contactID, dgInt32 maxContacts, dgInt32 conditionalContactCalculationAtOrigin)
{
	dgInt32 count;
	dgMinkFace *face;

	dgMinkReturnCode code;

	count = 0;
	code = CalcSeparatingPlane(face);

	m_lastFaceCode = code;
	if (code == dgMinkIntersecting) {

		if (m_proxy->m_isTriggerVolume) {
			timeStep = dgFloat32 (0.0f);
			m_proxy->m_inTriggerVolume = 1;
			count = 0;
		} else {
			face = CalculateClipPlane ();
			if (face) {
				if (conditionalContactCalculationAtOrigin) {
					dgFloat32 projVeloc;
					const dgPlane& plane = *face;
					projVeloc = plane % m_localRelVeloc;
					if (projVeloc >= dgFloat32 (0.0f)) {
						return 0;
					}
				}
				if (maxContacts) {
					count = CalculateContacts (face, contactID, contactOut, maxContacts);
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
		projVeloc = plane % m_localRelVeloc;
		if (projVeloc >= dgFloat32 (-1.0e-24f)) {
			code = UpdateSeparatingPlane(face, dgVector (dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f), dgFloat32 (0.0f)));
			if (code != dgMinkDisjoint) {
				return 0;
			}
			_ASSERTE (code == dgMinkDisjoint);
			i0 = face->m_vertex[0];
			i1 = face->m_vertex[1];
			i2 = face->m_vertex[2];
			plane = (m_hullVertex[i1] - m_hullVertex[i0]) * (m_hullVertex[i2] - m_hullVertex[i0]);

			_ASSERTE (plane % plane > dgFloat32 (0.0f));
			projVeloc = plane % m_localRelVeloc;
			if (projVeloc >= dgFloat32 (-1.0e-24f)) {
				//_ASSERTE ((plane % m_localRelVeloc) < dgFloat32 (0.0f));
				return 0;
			}
		}

		//			timeOfImpact = - plane.m_w / (plane % vRel + dgFloat32 (1.0e-24f));
		timeOfImpact = (plane % m_hullVertex[i0]) / (plane % m_localRelVeloc + dgFloat32 (1.0e-24f));

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
				code = UpdateSeparatingPlane(tmpFaceface, p1);
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

					den = plane % m_localRelVeloc ;
					//						if (dgAbsf (den) > dgFloat32 (1.0e-24f)) {
					if (den >= dgFloat32 (-1.0e-24f)) {
						code = UpdateSeparatingPlane(tmpFaceface, p1);
						_ASSERTE (code == dgMinkDisjoint);

						i0 = face->m_vertex[0];
						i1 = face->m_vertex[1];
						i2 = face->m_vertex[2];
						//dgPlane plane (m_hullVertex[i0], m_hullVertex[i1], m_hullVertex[i2]);
						dgVector plane ((m_hullVertex[i1] - m_hullVertex[i0]) * (m_hullVertex[i2] - m_hullVertex[i0]));
						_ASSERTE (plane % plane > dgFloat32 (-1.0e-24f));
						den = plane % m_localRelVeloc;
						if (den >= dgFloat32 (0.0f)) {
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
						timeOfImpact = (plane % m_hullVertex[i0]) / den;
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
					count = CalculateContactsContinues(contactID, contactOut, maxContacts, saveHull, saveAver, timeOfImpact);
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
						contactOut[i0].m_point += step;
					}
				}
			}
		}
	}
	return count;
}


dgInt32 dgContactSolver::CalculateContactsContinues(dgInt32 contacID, dgContactPoint* const contactOut, dgInt32 maxContacts, const dgVector* diffPoins, const dgVector* averPoins, dgFloat32 timestep)
{
	dgMinkFace* const face = &m_simplex[0];

	dgVector step (m_localRelVeloc.Scale (timestep + DG_ROBUST_PLANE_CLIP * dgRsqrt(m_localRelVeloc % m_localRelVeloc)));
	for (dgInt32 i = 0; i < 3; i ++) {
		m_hullVertex[i] = diffPoins[i] - step;
		m_averVertex[i] = averPoins[i] + step;
	}
	CalcFacePlane (face);
	dgPlane &facePlane  = *face;
	//		if ((m_facePlane[face->m_face] % veloc) > dgFloat32 (0.0f)) {
	if ((facePlane % m_localRelVeloc) > dgFloat32 (0.0f)) {
		//			_ASSERTE (0);
		facePlane = facePlane.Scale (dgFloat32 (-1.0f));
	}

	dgVector minkFloatingPosit (m_matrix.m_posit );
	m_matrix.m_posit += step;
	dgInt32 count = CalculateContacts(face, contacID, contactOut, maxContacts);
	_ASSERTE (count <= maxContacts);

	m_matrix.m_posit = minkFloatingPosit;
	return count;
}


dgInt32 dgContactSolver::HullHullContactsLarge (dgInt32 contactID)
{
	dgInt32 count = 0;
	m_proxy->m_inTriggerVolume = 0;
	dgMinkFace* face;
	dgMinkReturnCode code = CalcSeparatingPlaneLarge(face);

	switch (code)
	{
		case dgMinkIntersecting:
		{
			if (m_proxy->m_isTriggerVolume) {
				m_proxy->m_inTriggerVolume = 1;
			} else {
				face = CalculateClipPlaneLarge ();
				if (face) {
					count = CalculateContacts (face, contactID, m_proxy->m_contacts, m_proxy->m_maxContacts);
				}
			}
			break;
		}

		case dgMinkDisjoint:
		{
			if (CalcFacePlaneLarge (face)) {

				//_ASSERTE (face->m_w >= dgFloat32 (0.0f));
				_ASSERTE (face->m_w >= dgFloat32 (-1.0e-1f));
				_ASSERTE ((*face) % (*face) > dgFloat32 (0.0f));
				if (face->m_w < m_penetrationPadding) {
					dgVector step (*face);
					step = step.Scale (-(face->m_w + DG_IMPULSIVE_CONTACT_PENETRATION));

					dgInt32 i0 = face->m_vertex[0];
					m_hullVertex[i0] = dgVector (dgFloat32 (m_hullVertexLarge[i0].m_x), dgFloat32 (m_hullVertexLarge[i0].m_y), dgFloat32 (m_hullVertexLarge[i0].m_z), dgFloat32 (0.0f));
					m_averVertex[i0] = dgVector (dgFloat32 (m_averVertexLarge[i0].m_x), dgFloat32 (m_averVertexLarge[i0].m_y), dgFloat32 (m_averVertexLarge[i0].m_z), dgFloat32 (0.0f));
					m_hullVertex[i0] -= step;
					m_averVertex[i0] += step;

					m_matrix.m_posit += step;
					dgVector stepWorld (m_proxy->m_referenceMatrix.RotateVector(step));
					m_proxy->m_floatingMatrix.m_posit += stepWorld;

					count = CalculateContacts(face, contactID, m_proxy->m_contacts, m_proxy->m_maxContacts);
					stepWorld = stepWorld.Scale (dgFloat32 (0.5f));

					if (m_proxy->m_isTriggerVolume) {
						m_proxy->m_inTriggerVolume = 1;
						count = 0;
					}

					dgContactPoint* const contactOut = m_proxy->m_contacts; 
					for (dgInt32 i0 = 0; i0 < count; i0 ++ ) {
						contactOut[i0].m_point -= stepWorld ;
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


dgInt32 dgContactSolver::HullHullContacts (dgInt32 contactID)
{
	dgInt32 i0;
	dgInt32 count;
	dgMinkFace *face;
	dgMinkReturnCode code;
	dgContactPoint* contactOut; 

	count = 0;
	m_proxy->m_inTriggerVolume = 0;
	code = CalcSeparatingPlane(face);
	switch (code)
	{
		case dgMinkIntersecting:
		{
			if (m_proxy->m_isTriggerVolume) {
				m_proxy->m_inTriggerVolume = 1;
			} else {
				face = CalculateClipPlane ();
				if (face) {
					count = CalculateContacts (face, contactID, m_proxy->m_contacts, m_proxy->m_maxContacts);
					_ASSERTE (count <= m_proxy->m_maxContacts);
				}
			}
			break;
		}

		case dgMinkDisjoint:
		{
			_ASSERTE (face);

			if (CalcFacePlane (face)) {
				//_ASSERTE (face->m_w >= dgFloat32 (0.0f));
				_ASSERTE (face->m_w >= dgFloat32 (-1.0e-2f));
				_ASSERTE ((*face) % (*face) > dgFloat32 (0.0f));
				if (face->m_w < m_penetrationPadding) {
					dgVector step (*face);
					step = step.Scale (-(face->m_w + DG_IMPULSIVE_CONTACT_PENETRATION));

					i0 = face->m_vertex[0];
					m_hullVertex[i0] -= step;
					m_averVertex[i0] += step;

					m_matrix.m_posit += step;
					dgVector stepWorld (m_proxy->m_referenceMatrix.RotateVector(step));
					m_proxy->m_floatingMatrix.m_posit += stepWorld;

					count = CalculateContacts(face, contactID, m_proxy->m_contacts, m_proxy->m_maxContacts);
					stepWorld = stepWorld.Scale (dgFloat32 (0.5f));

					if (m_proxy->m_isTriggerVolume) {
						m_proxy->m_inTriggerVolume = 1;
						count = 0;
					}

					contactOut = m_proxy->m_contacts; 
					for (i0 = 0; i0 < count; i0 ++ ) {
						contactOut[i0].m_point -= stepWorld ;
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




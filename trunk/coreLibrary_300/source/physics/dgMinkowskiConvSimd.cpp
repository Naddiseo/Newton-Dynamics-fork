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


simd_type dgContactSolver::m_zero;
simd_type dgContactSolver::m_nrh0p5;
simd_type dgContactSolver::m_nrh3p0;
simd_type dgContactSolver::m_negIndex;
simd_type dgContactSolver::m_index_yx;
simd_type dgContactSolver::m_index_wz;
simd_type dgContactSolver::m_negativeOne;
simd_type dgContactSolver::m_zeroTolerenace;




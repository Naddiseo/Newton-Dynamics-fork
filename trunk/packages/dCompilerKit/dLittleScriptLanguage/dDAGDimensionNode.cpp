/* Copyright (c) <2009> <Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include "dLSCstdafx.h"
#include "dDAG.h"
#include "dDAGTypeNode.h"
#include "dDAGDimensionNode.h"
#include "dDAGExpressionNodeConstant.h"


dInitRtti(dDAGDimensionNode);

dDAGDimensionNode::dDAGDimensionNode(dList<dDAG*>& allNodes, dDAGExpressionNode* const size)
	:dDAG(allNodes)
	,m_arraySize("fix_ArraySize")
	,m_dimExp (size)
	,m_next (NULL)
{
	if (m_dimExp) {
		m_dimExp->AddRef();
	}
}


dDAGDimensionNode::~dDAGDimensionNode(void)
{
	if (m_dimExp) {
		m_dimExp->Release();
	}
}

void dDAGDimensionNode::ConnectParent(dDAG* const parent)  
{
	m_parent = parent;
	if (m_dimExp) {
		m_dimExp->ConnectParent(this);
	}
}

void dDAGDimensionNode::CompileCIL(dCIL& cil)  
{
	if (m_dimExp) {
		m_dimExp->CompileCIL(cil) ;
		m_result = m_dimExp->m_result;
	} else {
		_ASSERTE (0);
	}
}
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
	,m_size (size)
	,m_next (NULL)
{
	if (m_size) {
		m_size->AddRef();
	}
}


dDAGDimensionNode::~dDAGDimensionNode(void)
{
	if (m_size) {
		m_size->Release();
	}
}



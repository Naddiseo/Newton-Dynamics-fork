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

#include "dDirectAcyclicgraphNode.h"
#include "dDAGTypeNode.h"
#include "dDAGExpressionNodeConstant.h"


dInitRtti(dDAGExpressionNodeConstant);

dDAGExpressionNodeConstant::dDAGExpressionNodeConstant(dList<dDirectAcyclicgraphNode*>& allNodes, dType type, const char* const value)
	:dDAGExpressionNode(allNodes)
	,m_type(type)
{
	m_name = value;
	m_key = dCRC64 (m_name.c_str());
}


dDAGExpressionNodeConstant::~dDAGExpressionNodeConstant(void)
{
}


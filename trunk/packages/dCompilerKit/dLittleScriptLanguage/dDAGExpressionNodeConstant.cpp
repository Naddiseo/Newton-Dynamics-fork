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
#include "dDAGExpressionNodeConstant.h"


dInitRtti(dDAGExpressionNodeConstant);

dDAGExpressionNodeConstant::dDAGExpressionNodeConstant(dList<dDAG*>& allNodes, dType type, const char* const value)
	:dDAGExpressionNode(allNodes)
	,m_type(type)
{
	m_name = value;
}


dDAGExpressionNodeConstant::~dDAGExpressionNodeConstant(void)
{
}


void dDAGExpressionNodeConstant::ConnectParent(dDAG* const parent) 
{
	m_parent = parent;
}

void dDAGExpressionNodeConstant::CompileCIL(dCIL& cil)
{
	// may have to work on float and strings
	// but for now do nothing the const can be load it 
	m_result = m_name;
}
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
#include "dDAGFunctionStatementReturn.h"

dInitRtti(dDAGFunctionStatementReturn);

dDAGFunctionStatementReturn::dDAGFunctionStatementReturn(dList<dDirectAcyclicgraphNode*>& allNodes, dDAGExpressionNode* const expression)
	:dDAGFunctionStatement(allNodes)
	,m_expression(expression)
{
	if (m_expression) {
		m_expression->AddRef();
	}
}


dDAGFunctionStatementReturn::~dDAGFunctionStatementReturn()
{
	if (m_expression) {
		m_expression->Release();
	}
}



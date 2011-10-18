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
#include "dDAGFunctionStatementDO.h"
#include "dDAGExpressionNode.h"

dInitRtti(dDAGFunctionStatementDO);

dDAGFunctionStatementDO::dDAGFunctionStatementDO(dList<dDAG*>& allNodes, dDAGExpressionNode* const expression, dDAGFunctionStatement* const stmt)
	:dDAGFunctionStatement(allNodes)
	,m_expression(expression)
	,m_stmt (stmt)
{
	_ASSERTE (m_stmt);
	m_expression->AddRef();
	m_stmt->AddRef();
}


dDAGFunctionStatementDO::~dDAGFunctionStatementDO()
{
	m_expression->Release();
	m_stmt->Release();
}



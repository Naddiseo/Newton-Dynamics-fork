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
#include "dDAGExpressionNode.h"
#include "dDAGFunctionStatementFOR.h"


dInitRtti(dDAGFunctionStatementFOR);

dDAGFunctionStatementFOR::dDAGFunctionStatementFOR(dList<dDAG*>& allNodes, 
	dDAGFunctionStatement* const beginStmt, 
	dDAGExpressionNode* const expression, 
	dDAGFunctionStatement* const endStmt, 
	dDAGFunctionStatement* const stmt)
	:dDAGFunctionStatementFlow(allNodes)
	,m_initialStmt(beginStmt)
	,m_expression(expression)
	,m_endStmt(endStmt)
	,m_stmt(stmt)
{
	if (m_initialStmt) {
		m_initialStmt->AddRef();
	}
	if (m_expression) {
		m_expression->AddRef();
	}
	if (m_endStmt) {
		m_endStmt->AddRef();
	}
	if (m_stmt) {
		m_stmt->AddRef();
	}
}


dDAGFunctionStatementFOR::~dDAGFunctionStatementFOR()
{
	if (m_initialStmt) {
		m_initialStmt->Release();
	}
	if (m_expression) {
		m_expression->Release();
	}
	if (m_endStmt) {
		m_endStmt->Release();
	}
	if (m_stmt) {
		m_stmt->Release();
	}
}



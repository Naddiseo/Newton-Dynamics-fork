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

#include "dLittleScriptCompiler.h"
#include "dDirectAcyclicgraphNode.h"
#include "dDAGFunctionStatementFOR.h"
#include "dDAGExpressionNode.h"

dInitRtti(dDAGFunctionStatementFOR);

dDAGFunctionStatementFOR::dDAGFunctionStatementFOR(dList<dDirectAcyclicgraphNode*>& allNodes, 
	dDAGFunctionStatement* const beginStmt, 
	dDAGExpressionNode* const expression, 
	dDAGFunctionStatement* const endStmt, 
	dDAGFunctionStatement* const stmt)
	:dDAGFunctionStatement(allNodes)
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



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
#include "dDAGFunctionStatementWHILE.h"


dInitRtti(dDAGFunctionStatementWHILE);

dDAGFunctionStatementWHILE::dDAGFunctionStatementWHILE(dList<dDAG*>& allNodes, dDAGExpressionNode* const expression, dDAGFunctionStatement* const stmt)
	:dDAGFunctionStatementFlow(allNodes)
	,m_expression(expression)
	,m_stmt (stmt)
{
	if (m_expression) {
		m_expression->AddRef();
	}
	if (m_stmt) {
		m_stmt->AddRef();
	}
}


dDAGFunctionStatementWHILE::~dDAGFunctionStatementWHILE()
{
	if (m_expression) {
		m_expression->Release();
	}
	if (m_stmt) {
		m_stmt->Release();
	}
}


void dDAGFunctionStatementWHILE::ConnectParent(dDAG* const parent)
{
	m_parent = parent;
	if (m_stmt) {
		m_stmt->ConnectParent(this);
	}
	if (m_expression) {
		m_expression->ConnectParent(this);
	}
}


void dDAGFunctionStatementWHILE::CompileCIL(dCIL& cil)  
{
	dDAGFunctionStatementFlow::CompileCIL(cil);

	dCIL::dListNode* const startFlow = cil.NewStatement();
	dTreeAdressStmt& startLabel = startFlow->GetInfo();
	startLabel.m_instruction = dTreeAdressStmt::m_target;
	startLabel.m_arg0 = cil.NewLabel();
	dTRACE_INTRUCTION (&startLabel);

	dCIL::dListNode* expressionNode = NULL;
	if (m_expression) {
		m_expression->CompileCIL(cil);
		expressionNode = cil.NewStatement();
		dTreeAdressStmt& stmt = expressionNode->GetInfo();
		stmt.m_instruction = dTreeAdressStmt::m_ifnot;
		stmt.m_arg0 = m_expression->m_result;
		stmt.m_arg1 = cil.NewLabel();
		dTRACE_INTRUCTION (&stmt);
	}


	if (m_stmt) {
		m_stmt->CompileCIL(cil);
	}

	dTreeAdressStmt& endStmt = cil.NewStatement()->GetInfo();
	endStmt.m_instruction = dTreeAdressStmt::m_goto;
	endStmt.m_arg0 = startLabel.m_arg0;
	endStmt.m_jmpTarget = startFlow;
	dTRACE_INTRUCTION (&endStmt);

	if (expressionNode) {
		dTreeAdressStmt& stmt = expressionNode->GetInfo();
		stmt.m_jmpTarget = cil.NewStatement();
		dTreeAdressStmt& jmpTarget = stmt.m_jmpTarget->GetInfo();
		jmpTarget.m_instruction = dTreeAdressStmt::m_target;
		jmpTarget.m_arg0 = stmt.m_arg1;
		dTRACE_INTRUCTION (&jmpTarget);
	}

	BackPatch (cil);
}
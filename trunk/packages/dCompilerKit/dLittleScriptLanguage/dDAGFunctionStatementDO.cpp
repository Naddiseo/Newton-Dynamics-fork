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
#include "dDAGFunctionStatementDO.h"


dInitRtti(dDAGFunctionStatementDO);

dDAGFunctionStatementDO::dDAGFunctionStatementDO(dList<dDAG*>& allNodes, dDAGExpressionNode* const expression, dDAGFunctionStatement* const stmt)
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


dDAGFunctionStatementDO::~dDAGFunctionStatementDO()
{
	if (m_expression) {
		m_expression->Release();
	}
	if (m_stmt) {
		m_stmt->Release();
	}
}

void dDAGFunctionStatementDO::ConnectParent(dDAG* const parent)
{
	m_parent = parent;
	if (m_stmt) {
		m_stmt->ConnectParent(this);
	}
	if (m_expression) {
		m_expression->ConnectParent(this);
	}
}


void dDAGFunctionStatementDO::CompileCIL(dCIL& cil)  
{

	dDAGFunctionStatementFlow::CompileCIL(cil);

	dCIL::dListNode* const startFlow = cil.NewStatement();
	dTreeAdressStmt& startLabel = startFlow->GetInfo();
	startLabel.m_instruction = dTreeAdressStmt::m_target;
	startLabel.m_arg0 = cil.NewLabel();
	dTRACE_INTRUCTION (&startLabel);

	if (m_stmt) {
		m_stmt->CompileCIL(cil);
	}

	dCIL::dListNode* expressionNode = NULL;
	if (m_expression) {
		m_expression->CompileCIL(cil);
		expressionNode = cil.NewStatement();
		dTreeAdressStmt& stmt = expressionNode->GetInfo();
		stmt.m_instruction = dTreeAdressStmt::m_if;
		stmt.m_operator = dTreeAdressStmt::m_different;
		stmt.m_arg0 = m_expression->m_result;
		stmt.m_arg1 = "0"; 
		stmt.m_arg2 = startLabel.m_arg0; 
		stmt.m_jmpTarget = startFlow;
		dTRACE_INTRUCTION (&stmt);
	} else {
		dTreeAdressStmt& stmt = cil.NewStatement()->GetInfo();
		stmt.m_instruction = dTreeAdressStmt::m_goto;
		stmt.m_arg0 = startLabel.m_arg0; 
		stmt.m_jmpTarget = startFlow;
		dTRACE_INTRUCTION (&stmt);
	}

	BackPatch (cil);
}
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


void dDAGFunctionStatementFOR::ConnectParent(dDAG* const parent)  
{
	m_parent = parent;

	if (m_initialStmt) {
		m_initialStmt->ConnectParent(this);
	}
	if (m_expression) {
		m_expression->ConnectParent(this);
	}
	if (m_endStmt) {
		m_endStmt->ConnectParent(this);
	}
	if (m_stmt) {
		m_stmt->ConnectParent(this);
	}
}

void dDAGFunctionStatementFOR::CompileCIL(dCIL& cil)  
{
	if (m_initialStmt) {
		m_initialStmt->CompileCIL(cil);
	}

	dTreeAdressStmt& endStmt = cil.NewStatement()->GetInfo();
	endStmt.m_instruction = dTreeAdressStmt::m_goto;
	endStmt.m_arg0 = cil.NewLabel();
	dTRACE_INTRUCTION (&endStmt);

	dDAGFunctionStatementFlow::CompileCIL(cil);

	dCIL::dListNode* const startFlow = cil.NewStatement();
	dTreeAdressStmt& startLabel = startFlow->GetInfo();
	startLabel.m_instruction = dTreeAdressStmt::m_label;
	startLabel.m_arg0 = cil.NewLabel();
	dTRACE_INTRUCTION (&startLabel);

	if (m_stmt) {
		m_stmt->CompileCIL(cil);
	}

	if (m_endStmt) {
		m_endStmt->CompileCIL(cil);
	}

	dCIL::dListNode* const testFlow = cil.NewStatement();
	endStmt.m_jmpTarget = testFlow;

	dTreeAdressStmt& test = testFlow->GetInfo();
	test.m_instruction = dTreeAdressStmt::m_label;
	test.m_arg0 = endStmt.m_arg0;
	dTRACE_INTRUCTION (&test);

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
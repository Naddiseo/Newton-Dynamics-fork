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

	dDAGFunctionStatementFlow::CompileCIL(cil);

	if (m_initialStmt) {
		m_initialStmt->CompileCIL(cil);
	}


	dCIL::dProgram::dListNode* const startFlow = cil.NewStatement();
	dTreeAdressStmt& startLabel = startFlow->GetInfo();
	startLabel.m_instruction = dTreeAdressStmt::m_target;
	startLabel.m_arg0 = cil.NewLabel();
	dTRACE_INTRUCTION (&startLabel);

	dCIL::dProgram::dListNode* expressionNode = NULL;
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
	
	if (m_endStmt) {
		m_endStmt->CompileCIL(cil);
	}

	dTreeAdressStmt& endStmt = cil.NewStatement()->GetInfo();
	endStmt.m_instruction = dTreeAdressStmt::m_goto;
	endStmt.m_arg0 = startLabel.m_arg0;
	endStmt.m_jmpTarget = startFlow;
	dTRACE_INTRUCTION (&endStmt);


	dCIL::dProgram::dListNode* const exitLabel = cil.NewStatement();
	if (expressionNode) {
		dTreeAdressStmt& stmt = expressionNode->GetInfo();
		stmt.m_jmpTarget = exitLabel;
		dTreeAdressStmt& jmpTarget = exitLabel->GetInfo();
		jmpTarget.m_instruction = dTreeAdressStmt::m_target;
		jmpTarget.m_arg0 = stmt.m_arg1;
		dTRACE_INTRUCTION (&jmpTarget);
	}

	BackPatch (cil);
}
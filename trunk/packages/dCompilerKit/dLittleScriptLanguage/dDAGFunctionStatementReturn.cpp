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
#include "dDAGFunctionNode.h"
#include "dDAGScopeBlockNode.h"
#include "dDAGFunctionStatementReturn.h"

dInitRtti(dDAGFunctionStatementReturn);

dDAGFunctionStatementReturn::dDAGFunctionStatementReturn(dList<dDAG*>& allNodes, dDAGExpressionNode* const expression)
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


void dDAGFunctionStatementReturn::ConnectParent(dDAG* const parent)
{
	m_parent = parent;
	if (m_expression) {
		m_expression->ConnectParent(this);
	}

}

void dDAGFunctionStatementReturn::CompileCIL(dCIL& cil)
{
	if (m_expression) {
		m_expression->CompileCIL(cil);

		dDAGFunctionNode* const funtion = GetFunction();

		funtion->m_returnRegister = D_RETURN_REGISTER;

		dTreeAdressStmt& stmt = cil.NewStatement()->GetInfo();
		stmt.m_instruction = dTreeAdressStmt::m_assigment;
		stmt.m_arg0.m_label = D_RETURN_REGISTER;
		stmt.m_arg1.m_label = m_expression->m_result;
		dTRACE_INTRUCTION (&stmt);
	}

	dTreeAdressStmt& stmt = cil.NewStatement()->GetInfo();
	stmt.m_instruction = dTreeAdressStmt::m_goto;
	stmt.m_arg0.m_label = D_RETURN_LABEL;
	dTRACE_INTRUCTION (&stmt);
}
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
#include "dDAGFunctionStatementIF.h"
#include "dDAGExpressionNode.h"

dInitRtti(dDAGFunctionStatementIF);

dDAGFunctionStatementIF::dDAGFunctionStatementIF(dList<dDAG*>& allNodes, 
	dDAGExpressionNode* const expression,
	 dDAGFunctionStatement* const thenStmt, 
	 dDAGFunctionStatement* const elseStmt)
	:dDAGFunctionStatement(allNodes)
	,m_expression(expression)
	,m_thenStmt (thenStmt)
	,m_elseStmt (elseStmt)
{
	_ASSERTE (m_expression);
	_ASSERTE (m_thenStmt);
	m_expression->AddRef();
	m_thenStmt->AddRef();
	if (m_elseStmt) {
		m_elseStmt->AddRef();
	}
}


dDAGFunctionStatementIF::~dDAGFunctionStatementIF()
{
	m_expression->Release();
	m_thenStmt->Release();
	if (m_elseStmt) {
		m_elseStmt->Release();
	}
}


void dDAGFunctionStatementIF::ConnectParent(dDAG* const parent)  
{
	m_parent = parent;
	m_expression->ConnectParent(this);
	m_thenStmt->ConnectParent(this);
	if (m_elseStmt) {
		m_elseStmt->ConnectParent(this);
	}
}

void dDAGFunctionStatementIF::CompileCIL(dCIL& cil)  
{
	m_expression->CompileCIL(cil);

	dTreeAdressStmt& stmt = cil.NewStatement()->GetInfo();

	stmt.m_instruction = dTreeAdressStmt::m_ifnot;
	stmt.m_arg0 = m_expression->m_result;

	if (!m_elseStmt) {
		stmt.m_arg1 = cil.NewLabel();
		dTRACE_INTRUCTION (&stmt);

		m_thenStmt->CompileCIL(cil);
		
		stmt.m_jmpTarget = cil.NewStatement();
		dTreeAdressStmt& jmpTarget = stmt.m_jmpTarget->GetInfo();

		jmpTarget.m_instruction = dTreeAdressStmt::m_target;
		jmpTarget.m_arg0 = stmt.m_arg1;
		dTRACE_INTRUCTION (&jmpTarget);

	} else {
		_ASSERTE (0);
	}
	
}
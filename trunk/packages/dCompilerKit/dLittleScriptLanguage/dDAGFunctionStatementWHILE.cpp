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
#include "dDAGFunctionStatementWHILE.h"
#include "dDAGExpressionNode.h"

dInitRtti(dDAGFunctionStatementWHILE);

dDAGFunctionStatementWHILE::dDAGFunctionStatementWHILE(dList<dDAG*>& allNodes, dDAGExpressionNode* const expression, dDAGFunctionStatement* const stmt)
	:dDAGFunctionStatement(allNodes)
	,m_expression(expression)
	,m_stmt (stmt)
{
	_ASSERTE (m_stmt);
	m_expression->AddRef();
	m_stmt->AddRef();
}


dDAGFunctionStatementWHILE::~dDAGFunctionStatementWHILE()
{
	m_expression->Release();
	m_stmt->Release();
}


void dDAGFunctionStatementWHILE::ConnectParent(dDAG* const parent)
{
	m_parent = parent;
	m_stmt->ConnectParent(this);
	m_expression->ConnectParent(this);
}


void dDAGFunctionStatementWHILE::CompileCIL(dCIL& cil)  
{
	dTreeAdressStmt& startLabel = cil.NewStatement()->GetInfo();
	startLabel.m_instrution = dTreeAdressStmt::m_target;
	startLabel.m_arg0 = cil.NewLabel();
	dTRACE_INTRUCTION (&startLabel);

	m_expression->CompileCIL(cil);

	dTreeAdressStmt& stmt = cil.NewStatement()->GetInfo();
	stmt.m_instrution = dTreeAdressStmt::m_ifnot;
	stmt.m_arg0 = m_expression->m_result;
	stmt.m_arg1 = cil.NewLabel();
	dTRACE_INTRUCTION (&stmt);

	_ASSERTE (m_stmt);
	m_stmt->CompileCIL(cil);

	dTreeAdressStmt& endStmt = cil.NewStatement()->GetInfo();
	endStmt.m_instrution = dTreeAdressStmt::m_goto;
	endStmt.m_arg0 = startLabel.m_arg0;
	dTRACE_INTRUCTION (&endStmt);

	stmt.m_jmpTarget = cil.NewStatement();
	dTreeAdressStmt& jmpTarget = stmt.m_jmpTarget->GetInfo();
	jmpTarget.m_instrution = dTreeAdressStmt::m_target;
	jmpTarget.m_arg0 = stmt.m_arg1;
	dTRACE_INTRUCTION (&jmpTarget);
}
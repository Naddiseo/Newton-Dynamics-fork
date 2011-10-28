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
#include "dDAGFunctionStatementSWITCH.h"


dInitRtti(dDAGFunctionStatementSWITCH);

dDAGFunctionStatementSWITCH::dDAGFunctionStatementSWITCH(dList<dDAG*>& allNodes, dDAGExpressionNode* const expression, dDAGFunctionStatement* const stmt)
	:dDAGFunctionStatementFlow(allNodes)
	,m_expression(expression)
	,m_stmt (stmt)
{
	_ASSERTE (0);
//	_ASSERTE (m_stmt);
//	m_expression->AddRef();
//	m_stmt->AddRef();
}


dDAGFunctionStatementSWITCH::~dDAGFunctionStatementSWITCH()
{
	_ASSERTE (0);
//	m_expression->Release();
//	m_stmt->Release();
}


void dDAGFunctionStatementSWITCH::ConnectParent(dDAG* const parent)
{
	m_parent = parent;
	_ASSERTE (0);
//	m_stmt->ConnectParent(this);
//	m_expression->ConnectParent(this);
}


void dDAGFunctionStatementSWITCH::CompileCIL(dCIL& cil)  
{
	_ASSERTE (0);
/*
	dTreeAdressStmt& startLabel = cil.NewStatement()->GetInfo();
	startLabel.m_instruction = dTreeAdressStmt::m_label;
	startLabel.m_arg0 = cil.NewLabel();
	dTRACE_INTRUCTION (&startLabel);

	m_expression->CompileCIL(cil);

	dTreeAdressStmt& stmt = cil.NewStatement()->GetInfo();
	stmt.m_instruction = dTreeAdressStmt::m_ifnot;
	stmt.m_arg0 = m_expression->m_result;
	stmt.m_arg1 = cil.NewLabel();
	dTRACE_INTRUCTION (&stmt);

	_ASSERTE (m_stmt);
	m_stmt->CompileCIL(cil);

	dTreeAdressStmt& endStmt = cil.NewStatement()->GetInfo();
	endStmt.m_instruction = dTreeAdressStmt::m_goto;
	endStmt.m_arg0 = startLabel.m_arg0;
	dTRACE_INTRUCTION (&endStmt);

	stmt.m_jmpTarget = cil.NewStatement();
	dTreeAdressStmt& jmpTarget = stmt.m_jmpTarget->GetInfo();
	jmpTarget.m_instruction = dTreeAdressStmt::m_label;
	jmpTarget.m_arg0 = stmt.m_arg1;
	dTRACE_INTRUCTION (&jmpTarget);
*/
}
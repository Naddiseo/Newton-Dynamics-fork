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
#include "dDAGFunctionStatementAssigment.h"

dInitRtti(dDAGFunctionStatementAssigment);

dDAGFunctionStatementAssigment::dDAGFunctionStatementAssigment(dList<dDAG*>& allNodes, dDAGExpressionNodeVariable* const leftVariable, dDAGExpressionNode* const expression)
	:dDAGFunctionStatement(allNodes)
	,m_expression(expression)
	,m_leftVariable(leftVariable)
{
	_ASSERTE (m_expression);
	_ASSERTE (m_leftVariable);	
	m_expression->AddRef();
	m_leftVariable->AddRef();
}


dDAGFunctionStatementAssigment::~dDAGFunctionStatementAssigment()
{
	m_expression->Release();
	m_leftVariable->Release();
}

void dDAGFunctionStatementAssigment::ConnectParent(dDAG* const parent)  
{
	m_parent = parent;
	m_expression->ConnectParent(this);
	m_leftVariable->ConnectParent(this);
}


void dDAGFunctionStatementAssigment::CompileCIL(dCIL& cil)  
{
	m_expression->CompileCIL(cil); 
	m_leftVariable->CompileCIL(cil); 

	dTreeAdressStmt& stmt = cil.NewStatement()->GetInfo();
	stmt.m_instrution = dTreeAdressStmt::m_assigment;
	stmt.m_arg0 = m_leftVariable->m_result;
	stmt.m_arg1 = m_expression->m_result;
	dTRACE_INTRUCTION (&stmt);
}
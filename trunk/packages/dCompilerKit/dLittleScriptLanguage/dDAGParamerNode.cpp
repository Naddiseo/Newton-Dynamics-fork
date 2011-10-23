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
#include "dDAGTypeNode.h"
#include "dDAGFunctionNode.h"
#include "dDAGParameterNode.h"
#include "dDAGExpressionNode.h"
#include "dDAGScopeBlockNode.h"


dInitRtti(dDAGParameterNode);

dDAGParameterNode::dDAGParameterNode(dList<dDAG*>& allNodes, dDAGTypeNode* const type, const char* const identifier)
	:dDAGFunctionStatement(allNodes)
	,m_isPublic(true)
	,m_type(type)
	,m_next(NULL)
	,m_initializationExp(NULL)
{
	type->AddRef();
	m_name = string (identifier);
}


dDAGParameterNode::~dDAGParameterNode(void)
{
	m_type->Release();
	if (m_initializationExp) {
		m_initializationExp->Release();
	}
}

void dDAGParameterNode::SetInitializationExpression(dDAGExpressionNode* exp)
{
	m_initializationExp = exp;
	m_initializationExp->AddRef();
}


void dDAGParameterNode::ConnectParent(dDAG* const parent)  
{
	m_parent = parent;
	m_type->ConnectParent(this);
	if (m_initializationExp) {
		m_initializationExp->ConnectParent(this);
	}
}



void dDAGParameterNode::CompileCIL(dCIL& cil)  
{
	char text[256];
	dDAGScopeBlockNode* const scope = GetScope();
/*
	sprintf (text, "%s%d%s", D_SCOPE_PREFIX, scope->m_scopeLayer, m_name.c_str());
	if (scope->m_localVariablesFilter.FindVariable (text)) {
		DTRACE (("duplicated local variable\n"));
		_ASSERTE (0);
	}
	m_name = text;
	scope->m_localVariablesFilter.Append(m_name);

	dTreeAdressStmt& local = cil.NewStatement()->GetInfo();
	local.m_instruction = dTreeAdressStmt::m_local;
	local.m_arg0 = m_name;
	dTRACE_INTRUCTION (&local);

	if (m_initializationExp) {
		m_initializationExp->CompileCIL(cil);

		dTreeAdressStmt& stmt = cil.NewStatement()->GetInfo();
		stmt.m_instruction = dTreeAdressStmt::m_assigment;
		stmt.m_arg0 = m_name;
		stmt.m_arg1 = m_initializationExp->m_result;
		dTRACE_INTRUCTION (&stmt);
	}
*/



	const char* initName = NULL;
	if (m_initializationExp) {
		m_initializationExp->CompileCIL(cil);
		initName = m_initializationExp->m_result.c_str();
	}

	sprintf (text, "%s%d%s", D_SCOPE_PREFIX, scope->m_scopeLayer, m_name.c_str());
	if (scope->m_localVariablesFilter.FindVariable (text)) {
		DTRACE (("duplicated local variable\n"));
		_ASSERTE (0);
	}
	m_name = text;
	scope->m_localVariablesFilter.Append(m_name);

	dTreeAdressStmt& local = cil.NewStatement()->GetInfo();
	local.m_instruction = dTreeAdressStmt::m_local;
	local.m_arg0 = m_name;
	dTRACE_INTRUCTION (&local);

	if (initName) {
		dTreeAdressStmt& stmt = cil.NewStatement()->GetInfo();
		stmt.m_instruction = dTreeAdressStmt::m_assigment;
		stmt.m_arg0 = m_name;
		stmt.m_arg1 = initName;
		dTRACE_INTRUCTION (&stmt);
	}
}
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
#include "dDAGFunctionStatement.h"
#include "dDAGParameterNode.h"
#include "dDAGScopeBlockNode.h"
#include "dDAGExpressionNodeVariable.h"
#include "dDAGExpressionNodeBinaryOperator.h"

dInitRtti(dDAGScopeBlockNode);


dDAGScopeBlockNode::dDAGScopeBlockNode(dList<dDAG*>& allNodes)
	:dDAGFunctionStatement(allNodes)
	,m_statementList()
	,m_scopeLayer(0)
	,m_localVariablesFilter()
{

}


dDAGScopeBlockNode::~dDAGScopeBlockNode()
{
	for (dList<dDAGFunctionStatement*>::dListNode* node = m_statementList.GetFirst(); node; node = node->GetNext()) {
		dDAGFunctionStatement* const stmt = node->GetInfo();
		stmt->Release();
	}
}


void dDAGScopeBlockNode::AddStatement (dDAGFunctionStatement* const statement)
{
	m_statementList.Append(statement);
	statement->AddRef();
}



void dDAGScopeBlockNode::ConnectParent(dDAG* const parent)  
{
	m_parent = parent;

	for (dDAG* node = m_parent; node; node = node->m_parent) {
		if (node->GetTypeId() == dDAGScopeBlockNode::GetRttiType()) {
			dDAGScopeBlockNode* const scope = (dDAGScopeBlockNode*) node;
			m_scopeLayer = scope->m_scopeLayer + 1;
			break;
		}
	}

	for (dList<dDAGFunctionStatement*>::dListNode* node = m_statementList.GetFirst(); node; node = node->GetNext()) {
		dDAGFunctionStatement* const stmt = node->GetInfo();
		stmt->ConnectParent(this);
	}
}


void dDAGScopeBlockNode::CompileCIL(dCIL& cil)  
{
	for (dList<dDAGFunctionStatement*>::dListNode* node = m_statementList.GetFirst(); node; node = node->GetNext()) {
		dDAGFunctionStatement* const stmt = node->GetInfo();
		stmt->CompileCIL(cil);
	}
}



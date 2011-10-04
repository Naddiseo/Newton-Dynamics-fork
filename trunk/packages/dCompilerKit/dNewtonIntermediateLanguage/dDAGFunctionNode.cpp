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

#include "dDirectAcyclicgraphNode.h"
#include "dDAGFunctionNode.h"
#include "dDAGParameterNode.h"
#include "dDAGScopeBlockNode.h"

dInitRtti(dDAGFunctionNode);

dDAGFunctionNode::dDAGFunctionNode()
	:dDirectAcyclicgraphNode()
	,m_isconst (false)
	,m_returnType (NULL)
	,m_functionScopeBlock(NULL)
	,m_scopeStack()
{
}


dDAGFunctionNode::~dDAGFunctionNode(void)
{
	_ASSERTE (0);
}


void dDAGFunctionNode::CalculateKey() 
{
	if (m_isconst) {
		m_key = dCRC64 ("const", GetKey());
	}
	for (dList<const dDAGParameterNode*>::dListNode* node = m_parameters.GetFirst(); node; node = node->GetNext()) {
		const dDAGParameterNode* const type = node->GetInfo();
		m_key = CombineCRC (m_key, type->GetKey());
	}
	m_key = dCRC64 (m_name.c_str(), m_key);
}


void dDAGFunctionNode::AddParameter (const dDAGParameterNode* const parameter)
{
	for (dList<const dDAGParameterNode*>::dListNode* node = m_parameters.GetFirst(); node; node = node->GetNext()) {
		if (parameter->GetKey() == node->GetInfo()->GetKey()) {
			// error duplicated parameter
			_ASSERTE (0);
		}
	}
	m_parameters.Append(parameter);
	parameter->AddRef();
}


void dDAGFunctionNode::FinalizeFunctionPrototypeNode (dDAGTypeNode* const type, const char* const name, const char* const isConst)
{
	m_name = name;
	m_returnType = type;
	m_isconst =  (isConst[0] == 0) ? false : true;

	CalculateKey();
}


void dDAGFunctionNode::PushScope (dDAGScopeBlockNode* const scope)
{
	scope->AddRef();
	if (!m_functionScopeBlock) {
		m_functionScopeBlock = scope;
	} else {
		_ASSERTE (m_scopeStack.GetCount() >= 1);
		dDAGScopeBlockNode* const stackScopeBlock = m_scopeStack.GetFirst()->GetInfo();
		stackScopeBlock->m_subScopeBlocks.Append(scope);
	}
	m_scopeStack.Addtop(scope);
}

void dDAGFunctionNode::PopScope ()
{
	_ASSERTE (m_scopeStack.GetCount() >= 1);
	m_scopeStack.Remove(m_scopeStack.GetFirst());
}

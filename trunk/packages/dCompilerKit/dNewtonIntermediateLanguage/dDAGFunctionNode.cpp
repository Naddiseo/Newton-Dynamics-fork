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
#include "dDAGTypeNode.h"
#include "dDAGFunctionNode.h"
#include "dDAGParameterNode.h"
#include "dDAGScopeBlockNode.h"


dInitRtti(dDAGFunctionNode);

dDAGFunctionNode::dDAGFunctionNode()
	:dDirectAcyclicgraphNode()
	,m_isConst (false)
	,m_isPrivate(false)
	,m_returnType (NULL)
	,m_functionScopeBlock(NULL)
	,m_scopeStack()
{
}


dDAGFunctionNode::~dDAGFunctionNode(void)
{
	_ASSERTE (m_functionScopeBlock);
	m_returnType->Release();
	m_functionScopeBlock->Release();

	for (dList<dDAGParameterNode*>::dListNode* node = m_parameters.GetFirst(); node; node = node->GetNext()) {
		dDAGParameterNode* const parameter = node->GetInfo();
		parameter->Release();
	}
}


void dDAGFunctionNode::CalculateKey() 
{
	if (m_isConst) {
		m_key = dCRC64 ("const", GetKey());
	}
	for (dList<dDAGParameterNode*>::dListNode* node = m_parameters.GetFirst(); node; node = node->GetNext()) {
		dDAGParameterNode* const parameter = node->GetInfo();
		m_key = dCombineCRC (m_key, parameter->GetKey());
	}
	m_key = dCRC64 (m_name.c_str(), m_key);
}


void dDAGFunctionNode::AddParameter (dDAGParameterNode* const parameter)
{
	for (dList<dDAGParameterNode*>::dListNode* node = m_parameters.GetFirst(); node; node = node->GetNext()) {
		if (parameter->GetKey() == node->GetInfo()->GetKey()) {
			// error duplicated parameter
			_ASSERTE (0);
		}
	}
	m_parameters.Append(parameter);
	parameter->AddRef();
}


void dDAGFunctionNode::FinalizePrototype (dDAGTypeNode* const type, const char* const name, const char* const isConstant)
{
	m_name = name;
	m_returnType = type;
	m_isConst =  (isConstant[0] == 0) ? false : true;

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


dDAGScopeBlockNode* dDAGFunctionNode::GetCurrentBlock() const
{
	_ASSERTE (m_scopeStack.GetCount());
	return m_scopeStack.GetFirst()->GetInfo();
}
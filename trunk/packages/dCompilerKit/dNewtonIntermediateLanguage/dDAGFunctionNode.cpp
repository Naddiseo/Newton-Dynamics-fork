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

dDAGFunctionNode::dDAGFunctionNode(dList<dDirectAcyclicgraphNode*>& allNodes, dDAGTypeNode* const type, const char* const name, dDAGParameterNode* const parameterList, const char* const isConst)
	:dDirectAcyclicgraphNode(allNodes)
	,m_isConst (isConst[0] ? false : true)
	,m_isPrivate(false)
	,m_returnType (type)
	,m_body(NULL)
	,m_parameters() 
{
	m_returnType->AddRef();
	dDAGParameterNode* next;
	for (dDAGParameterNode* param = parameterList; param; param = next) {
		next = param->m_next;
		param->m_next = NULL;
		_ASSERTE (param->IsType(dDAGParameterNode::GetRttiType()));
		m_parameters.Append(param);
		param->AddRef(); 
	}
}


dDAGFunctionNode::~dDAGFunctionNode(void)
{
	_ASSERTE (m_body);
	_ASSERTE (m_returnType);
	m_returnType->Release();
	m_body->Release();

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

void dDAGFunctionNode::SetBody(dDAGScopeBlockNode* const body)
{
	m_body = body;
	m_body->AddRef();
}


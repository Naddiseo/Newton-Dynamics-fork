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
#include "dDAGExpressionFunctionCall.h"


dInitRtti(dDAGExpressionFunctionCall);

dDAGExpressionFunctionCall::dDAGExpressionFunctionCall(dList<dDirectAcyclicgraphNode*>& allNodes, const char* const identifier, dDAGExpressionNode* const argumentList)
	:dDAGExpressionNode(allNodes)
	,m_argumentList()
{
	m_name = identifier;

	dDAGExpressionNode* next;
	for (dDAGExpressionNode* param = argumentList; param; param = next) {
		next = param->m_argumentListNext;
		param->m_argumentListNext = NULL;
		_ASSERTE (param->IsType(dDAGExpressionNode::GetRttiType()));
		m_argumentList.Append(param);
		param->AddRef(); 
	}


	m_key = dCRC64 (m_name.c_str());
}


dDAGExpressionFunctionCall::~dDAGExpressionFunctionCall(void)
{
	for (dList<dDAGExpressionNode*>::dListNode* node = m_argumentList.GetFirst(); node; node = node->GetNext()) {
		dDAGExpressionNode* const parameter = node->GetInfo();
		parameter->Release();
	}
}



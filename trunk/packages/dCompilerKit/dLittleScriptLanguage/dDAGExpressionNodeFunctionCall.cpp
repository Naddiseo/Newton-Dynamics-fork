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
#include "dDAGExpressionNodeFunctionCall.h"


dInitRtti(dDAGExpressionNodeFunctionCall);

dDAGExpressionNodeFunctionCall::dDAGExpressionNodeFunctionCall(dList<dDAG*>& allNodes, const char* const identifier, dDAGExpressionNode* const argumentList)
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
}


dDAGExpressionNodeFunctionCall::~dDAGExpressionNodeFunctionCall(void)
{
	for (dList<dDAGExpressionNode*>::dListNode* node = m_argumentList.GetFirst(); node; node = node->GetNext()) {
		dDAGExpressionNode* const parameter = node->GetInfo();
		parameter->Release();
	}
}

void dDAGExpressionNodeFunctionCall::ConnectParent(dDAG* const parent)
{
	m_parent = parent;
	for (dList<dDAGExpressionNode*>::dListNode* node = m_argumentList.GetFirst(); node; node = node->GetNext()) {
		dDAGExpressionNode* const parameter = node->GetInfo();
		parameter->ConnectParent(this);
	}

}

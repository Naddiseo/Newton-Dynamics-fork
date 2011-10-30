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


void dDAGExpressionNodeFunctionCall::CompileCIL(dCIL& cil)  
{
	for (dList<dDAGExpressionNode*>::dListNode* node = m_argumentList.GetLast(); node; node = node->GetPrev()) {
		dDAGExpressionNode* const exp = node->GetInfo();
		exp->CompileCIL(cil);
		dTreeAdressStmt& stmt = cil.NewStatement()->GetInfo();
		stmt.m_instruction = dTreeAdressStmt::m_param;
		stmt.m_arg0.m_label = exp->m_result;
		dTRACE_INTRUCTION (&stmt);
	}

	dTreeAdressStmt& call = cil.NewStatement()->GetInfo();
	call.m_instruction = dTreeAdressStmt::m_call;
	call.m_arg0.m_label = m_name;
	dTRACE_INTRUCTION (&call);

	if (m_argumentList.GetCount()) {
		dTreeAdressStmt& param = cil.NewStatement()->GetInfo();
		char text[256];
		sprintf (text, "%d", m_argumentList.GetCount());
		param.m_instruction = dTreeAdressStmt::m_restoreParam;
		param.m_arg0.m_label = text;
		dTRACE_INTRUCTION (&param);
	}

	

}

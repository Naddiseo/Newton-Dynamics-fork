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
#include "dDAGScopeBlockNode.h"
#include "dDAGFunctionModifier.h"
#include "dDAGFunctionStatementReturn.h"

dInitRtti(dDAGFunctionNode);

dDAGFunctionNode::dDAGFunctionNode(dList<dDAG*>& allNodes, dDAGTypeNode* const type, const char* const name, dDAGParameterNode* const parameterList, const char* const isConst)
	:dDAG(allNodes)
	,m_isConst (isConst[0] ? false : true)
	,m_isPrivate(false)
	,m_isConstructor(false)
	,m_returnType (type)
	,m_body(NULL)
	,m_modifier(NULL)
	,m_parameters() 
{
	m_name = name;
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
	_ASSERTE (m_returnType);
	m_returnType->Release();
	if (m_body) {
		m_body->Release();
	}
	if (m_modifier) {
		m_modifier->Release();
	}
	for (dList<dDAGParameterNode*>::dListNode* node = m_parameters.GetFirst(); node; node = node->GetNext()) {
		dDAGParameterNode* const parameter = node->GetInfo();
		parameter->Release();
	}
}


void dDAGFunctionNode::SetBody(dDAGScopeBlockNode* const body)
{
	m_body = body;
	m_body->AddRef();
}

void dDAGFunctionNode::SetModifier(dDAGFunctionModifier* const modifier)
{
	m_modifier = modifier;
	m_modifier->AddRef();
}

bool dDAGFunctionNode::FindArgumentVariable(const char* name) const
{
	for (dList<dDAGParameterNode*>::dListNode* node = m_parameters.GetFirst(); node; node = node->GetNext()) {
		dDAGParameterNode* const variable = node->GetInfo();
		if (variable->m_name == name) {
			return true;
		}
	}

	return false;
}

void dDAGFunctionNode::ConnectParent(dDAG* const parent)
{
	m_parent = parent;
	m_body->ConnectParent(this);
	m_returnType->ConnectParent(this);

	if (m_modifier) {
		m_modifier->ConnectParent(this);
	}

	for (dList<dDAGParameterNode*>::dListNode* node = m_parameters.GetFirst(); node; node = node->GetNext()) {
		dDAGParameterNode* const variable = node->GetInfo();
		variable->ConnectParent(this);
	}
}

void dDAGFunctionNode::CompileCIL(dCIL& cil)  
{
	_ASSERTE (m_body);

	dCIL::dListNode* const functionNode = cil.NewStatement();
	dTreeAdressStmt& function = functionNode->GetInfo();
	function.m_instruction = dTreeAdressStmt::m_function;
	function.m_arg0 = m_name;
	dTRACE_INTRUCTION (&function);
	for (dList<dDAGParameterNode*>::dListNode* argNode = m_parameters.GetFirst(); argNode; argNode = argNode->GetNext()) {
		dDAGParameterNode* const arg = argNode->GetInfo();

		dTreeAdressStmt& fntArg = cil.NewStatement()->GetInfo();
		fntArg.m_instruction = dTreeAdressStmt::m_argument;
		fntArg.m_arg0 = arg->m_name;
		dTRACE_INTRUCTION (&fntArg);
	}

	dCIL::dListNode* const blockStart= cil.GetLast();
	cil.ResetTemporaries();
	m_body->CompileCIL(cil);

	bool returnStmt = false;
	for (dCIL::dListNode* node = functionNode; node; node = node->GetNext()) {
		dTreeAdressStmt& stmt = node->GetInfo();
		if ((stmt.m_instruction == dTreeAdressStmt::m_goto) && (stmt.m_arg0 == D_RETURN_LABEL))  {
			returnStmt = true;
			break;
		}
	}

	if (returnStmt) {
		dCIL::dListNode* const retLabelNode = cil.NewStatement();
		dTreeAdressStmt& retLabel = retLabelNode->GetInfo();
		retLabel.m_instruction = dTreeAdressStmt::m_label;
		retLabel.m_arg0 = D_RETURN_LABEL;
		dTRACE_INTRUCTION (&retLabel);

		for (dCIL::dListNode* node = functionNode; node; node = node->GetNext()) {
			dTreeAdressStmt& stmt = node->GetInfo();
			if ((stmt.m_instruction == dTreeAdressStmt::m_goto) && (stmt.m_arg0 == D_RETURN_LABEL))  {
				stmt.m_jmpTarget = retLabelNode;
			}
		}
	}

	dCIL::dListNode* const retNode = cil.NewStatement();
	dTreeAdressStmt& ret = retNode->GetInfo();
	ret.m_instruction = dTreeAdressStmt::m_ret;
	dTRACE_INTRUCTION (&ret);

	cil.NewStatement();
	cil.Optimize(blockStart->GetNext());
}

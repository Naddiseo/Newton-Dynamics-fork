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
#include "dDAGClassNode.h"
#include "dDAGFunctionNode.h"
#include "dDAGParameterNode.h"

dInitRtti(dDAGClassNode);

dDAGClassNode::dDAGClassNode(dList<dDAG*>& allNodes)
	:dDAG(allNodes)
	,m_isPublic(true)
	,m_baseClass (NULL)
	,m_variables()
	,m_functionList()
{
}


dDAGClassNode::~dDAGClassNode(void)
{
	for (dList<dDAGParameterNode*>::dListNode* node = m_variables.GetFirst(); node; node = node->GetNext()) {
		dDAGParameterNode* const variable = node->GetInfo();
		variable->Release();
	}

	for (dList<dDAGFunctionNode*>::dListNode* node = m_functionList.GetFirst(); node; node = node->GetNext()) {
		dDAGFunctionNode* const function = node->GetInfo();
		function->Release();
	}
}

void dDAGClassNode::FinalizeImplementation (const char* const visibility, const char* const name, dDAGClassNode* const baseClass)
{
	m_isPublic = (string (visibility) == "") ? true : false;
	m_name = name;
	m_baseClass = baseClass;
	_ASSERTE (!m_baseClass);
}


void dDAGClassNode::AddFunction (dDAGFunctionNode* const function)
{
	m_functionList.Append(function);
	function->AddRef();
}


void dDAGClassNode::AddVariable (dDAGParameterNode* const variable)
{
	m_variables.Append(variable);
	variable->AddRef();
}


void dDAGClassNode::ConnectParent(dDAG* const parent)  
{
	m_parent = parent;
	for (dList<dDAGParameterNode*>::dListNode* node = m_variables.GetFirst(); node; node = node->GetNext()) {
		dDAGParameterNode* const variable = node->GetInfo();
		variable->ConnectParent(this);
	}

	for (dList<dDAGFunctionNode*>::dListNode* node = m_functionList.GetFirst(); node; node = node->GetNext()) {
		dDAGFunctionNode* const function = node->GetInfo();
		function->ConnectParent(this);
	}
}

void dDAGClassNode::CompileCIL(dCIL& cil)  
{
	DTRACE (("emit the class variables here\n"));

	for (dList<dDAGFunctionNode*>::dListNode* node = m_functionList.GetFirst(); node; node = node->GetNext()) {
		dDAGFunctionNode* const function = node->GetInfo();
		function->CompileCIL(cil);
	}
}
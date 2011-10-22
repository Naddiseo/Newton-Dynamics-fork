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
#include "dDAGFunctionNode.h"
#include "dDAGScopeBlockNode.h"

dRttiRootClassSupportImplement(dDAG);

dDAG::dDAG(dList<dDAG*>& allNodes)
	:m_name ("")
	,m_result("")
	,m_parent (NULL)
{
	allNodes.Append(this);
}

dDAG::~dDAG(void)
{
}



dDAGScopeBlockNode* dDAG::GetScope() const
{
	for (const dDAG* node = this; node; node = node->m_parent) {
		if (node->GetTypeId() == dDAGScopeBlockNode::GetRttiType()) {
			return (dDAGScopeBlockNode*) node;
		}
	}
	return NULL;
}

dDAGFunctionNode* dDAG::GetFunction() const
{
	for (const dDAG* node = this; node; node = node->m_parent) {
		if (node->GetTypeId() == dDAGFunctionNode::GetRttiType()) {
			return (dDAGFunctionNode*) node;
		}
	}
	_ASSERTE (0);
	return NULL;
}

bool dDAG::RenameLocalVariable(string& variable) const
{
	for (dDAGScopeBlockNode* scope = GetScope(); scope; scope = (dDAGScopeBlockNode*)scope->m_parent->GetScope()) {
		char text[256];
		sprintf (text, "%s%d%s", D_SCOPE_PREFIX, scope->m_scopeLayer, variable.c_str());

		if (scope->m_localVariablesFilter.FindVariable(text)) {
			variable = text;
			return true;
		}
	}

	dDAGFunctionNode* const function = GetFunction();
	if (function->FindArgumentVariable(variable.c_str())) {
		return true;
	}


	return false;
}
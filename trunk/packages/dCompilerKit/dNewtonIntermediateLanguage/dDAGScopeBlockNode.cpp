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
#include "dDAGParameterNode.h"
#include "dDAGScopeBlockNode.h"


dInitRtti(dDAGScopeBlockNode);



dDAGScopeBlockNode::dDAGScopeBlockNode()
	:dDirectAcyclicgraphNode()
	,m_subScopeBlocks()
{
}


dDAGScopeBlockNode::~dDAGScopeBlockNode()
{
	for (dList<dDAGParameterNode*>::dListNode* node = m_localVariables.GetFirst(); node; node = node->GetNext()) {
		dDAGParameterNode* const variable = node->GetInfo();
		variable->Release();
	}

	for (dList<dDAGScopeBlockNode*>::dListNode* node = m_subScopeBlocks.GetFirst(); node; node = node->GetNext()) {
		dDAGScopeBlockNode* const block = node->GetInfo();
		block->Release();
	}
}


void dDAGScopeBlockNode::AddLocalVariable (dDAGParameterNode* const variable)
{
	m_localVariables.Append(variable);
	variable->AddRef();
}



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

#include "dLittleScriptCompiler.h"
#include "dDAG.h"
#include "dDAGTypeNode.h"
#include "dDAGDimensionNode.h"

dInitRtti(dDAGTypeNode);

dDAGTypeNode::dDAGTypeNode(dList<dDAG*>& allNodes, const char* const type, const char* const modifier)
	:dDAG(allNodes)
	,m_dimensions()
{
	m_name = string (modifier) + string (type);
	CalculateKey();		
}


dDAGTypeNode::~dDAGTypeNode(void)
{
	for (dList<dDAGDimensionNode*>::dListNode* node = m_dimensions.GetFirst(); node; node = node->GetNext()) {
		dDAGDimensionNode* const dim = node->GetInfo();
		dim->Release();
	}
}


void dDAGTypeNode::CalculateKey() 
{
	m_key = dCRC64 (m_name.c_str());
}

void dDAGTypeNode::AddDimensions (dDAGDimensionNode* dimList)
{
	dDAGDimensionNode* next;
	for (dDAGDimensionNode* node = dimList; node; node = next) {
		next = node->m_next;
		node->m_next = NULL;
		_ASSERTE (node->IsType(dDAGDimensionNode::GetRttiType()));
		m_dimensions.Append(node);
		node->AddRef();
	}
}
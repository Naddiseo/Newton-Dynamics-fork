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
#include "dDirectAcyclicgraphNode.h"
#include "dDAGTypeNode.h"
#include "dDAGParameterNode.h"
#include "dDAGExpressionNode.h"


dInitRtti(dDAGParameterNode);

dDAGParameterNode::dDAGParameterNode(dList<dDirectAcyclicgraphNode*>& allNodes, dDAGTypeNode* const type, const char* const identifier)
	:dDAGFunctionStatement(allNodes)
	,m_isPublic(true)
	,m_type(type)
	,m_next(NULL)
	,m_initializationExp(NULL)
{
	type->AddRef();
	m_name = string (identifier);
	CalculateKey();		
}


dDAGParameterNode::~dDAGParameterNode(void)
{
	m_type->Release();
	if (m_initializationExp) {
		m_initializationExp->Release();
	}
}

void dDAGParameterNode::SetInitializationExpression(dDAGExpressionNode* exp)
{
	m_initializationExp = exp;
	m_initializationExp->AddRef();
}

void dDAGParameterNode::CalculateKey() 
{
	m_key = dCRC64 (m_name.c_str(), m_type->GetKey());
}


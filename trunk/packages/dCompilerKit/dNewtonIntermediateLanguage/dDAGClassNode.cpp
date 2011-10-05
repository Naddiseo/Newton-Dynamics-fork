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
#include "dDAGClassNode.h"
#include "dDAGFunctionNode.h"

dInitRtti(dDAGClassNode);

dDAGClassNode::dDAGClassNode()
	:dDirectAcyclicgraphNode()
	,m_isPublic(true)
	,m_name ("")
	,m_baseClass (NULL)
{
}


dDAGClassNode::~dDAGClassNode(void)
{
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
	CalculateKey() ;
}

void dDAGClassNode::CalculateKey() 
{
	if (m_baseClass) {
		m_key = m_baseClass->GetKey();
	}
	if (!m_isPublic) {
		m_key = dCRC64 ("private", m_key);
	}
	m_key = dCRC64 (m_name.c_str(), m_key);
}

void dDAGClassNode::AddFunction (dDAGFunctionNode* const function)
{
	m_functionList.Append(function);
}
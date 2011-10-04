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
#include "dDAGTypeNode.h"

dInitRtti(dDAGParameterNode);

dDAGParameterNode::dDAGParameterNode(dDAGTypeNode* const type, const char* const identifier)
	:dDirectAcyclicgraphNode()
	,m_type(type)
{
	m_name = string (identifier);
	CalculateKey();		
}


dDAGParameterNode::~dDAGParameterNode(void)
{
	_ASSERTE (0);
}


void dDAGParameterNode::CalculateKey() 
{
	m_key = dCRC64 (m_name.c_str(), m_type->GetKey());
}


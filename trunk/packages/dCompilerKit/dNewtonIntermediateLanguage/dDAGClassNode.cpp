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

dInitRtti(dDAGClassNode);

dDAGClassNode::dDAGClassNode()
	:dDirectAcyclicgraphNode()
	,m_name ("")
	,m_baseClass (NULL)
{
}


dDAGClassNode::~dDAGClassNode(void)
{
	_ASSERTE (0);
}


void dDAGClassNode::CalculateKey() 
{
	_ASSERTE (0);
//	m_key = dCRC64 (m_name.c_str(), m_baseClass ? m_baseClass->GetKey() : 0);
}


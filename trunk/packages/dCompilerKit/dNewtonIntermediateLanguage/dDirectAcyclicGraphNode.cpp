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

dRttiRootClassSupportImplement(dDirectAcyclicgraphNode);

dDirectAcyclicgraphNode::dDirectAcyclicgraphNode(dList<dDirectAcyclicgraphNode*>& allNodes)
	:m_name ("")
	,m_key(0)
{
	allNodes.Append(this);
}

dDirectAcyclicgraphNode::~dDirectAcyclicgraphNode(void)
{
}


dCRCTYPE dDirectAcyclicgraphNode::GetKey() const
{
	return m_key;
}

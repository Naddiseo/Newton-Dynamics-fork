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
#include "dDAGScopeBlockNode.h"

dInitRtti(dDAGScopeBlockNode);

dDAGScopeBlockNode::dDAGScopeBlockNode()
	:dDirectAcyclicgraphNode()
	,m_subScopeBlocks()
{
}


dDAGScopeBlockNode::~dDAGScopeBlockNode(void)
{
	_ASSERTE (0);
}




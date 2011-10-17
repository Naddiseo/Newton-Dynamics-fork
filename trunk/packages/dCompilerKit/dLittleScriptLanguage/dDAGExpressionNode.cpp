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
#include "dDAGExpressionNode.h"


dInitRtti(dDAGExpressionNode);

dDAGExpressionNode::dDAGExpressionNode(dList<dDirectAcyclicgraphNode*>& allNodes)
	:dDirectAcyclicgraphNode(allNodes)
	,m_argumentListNext(NULL)
{
}


dDAGExpressionNode::~dDAGExpressionNode(void)
{
}



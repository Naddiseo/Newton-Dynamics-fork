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

#ifndef __dDAGDimensionNode_H__
#define __dDAGDimensionNode_H__

#include "dDirectAcyclicgraphNode.h"

class dDAGExpressionNode;

class dDAGDimensionNode: public dDirectAcyclicgraphNode
{
	public:
	dDAGDimensionNode(dList<dDirectAcyclicgraphNode*>& allNodes, dDAGExpressionNode* const size);
	~dDAGDimensionNode(void);


	dDAGExpressionNode* m_size;
	dDAGDimensionNode* m_next;

	dAddRtti(dDirectAcyclicgraphNode);
};


#endif
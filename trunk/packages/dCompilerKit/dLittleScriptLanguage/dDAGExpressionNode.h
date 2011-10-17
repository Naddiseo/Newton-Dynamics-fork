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

#ifndef __dDAGExpressionNode_H__
#define __dDAGExpressionNode_H__

#include "dDirectAcyclicgraphNode.h"
#include "dLittleScriptCompiler.h"


class dDAGExpressionNode: public dDirectAcyclicgraphNode
{
	public:
	dDAGExpressionNode (dList<dDirectAcyclicgraphNode*>& allNodes);
	~dDAGExpressionNode(void);


	dDAGExpressionNode* m_argumentListNext;
	dAddRtti(dDirectAcyclicgraphNode);
};


#endif
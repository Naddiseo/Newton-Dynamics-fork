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

#ifndef __dDAGExpressionFunctionCall_H__
#define __dDAGExpressionFunctionCall_H__

#include "dDAGExpressionNode.h"


class dDAGExpressionFunctionCall: public dDAGExpressionNode
{
	public:
	dDAGExpressionFunctionCall (dList<dDirectAcyclicgraphNode*>& allNodes, const char* const identifier, dDAGExpressionNode* const argumentList);
	~dDAGExpressionFunctionCall(void);

	dList<dDAGExpressionNode*> m_argumentList;
	dAddRtti(dDAGExpressionNode);
};


#endif
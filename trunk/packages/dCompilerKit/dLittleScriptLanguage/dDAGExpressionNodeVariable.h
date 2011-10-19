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

#ifndef __dDAGExpressionNodeVariable_H__
#define __dDAGExpressionNodeVariable_H__

#include "dDAG.h"
#include "dDAGExpressionNode.h"

class dDAGDimensionNode;

class dDAGExpressionNodeVariable: public dDAGExpressionNode
{
	public:
	dDAGExpressionNodeVariable (dList<dDAG*>& allNodes, const char* const identifier, dDAGDimensionNode* const expressionDimIndex);
	~dDAGExpressionNodeVariable(void);

	virtual void CompileCIL(dCIL& cil);
	virtual void ConnectParents(dDAG* const parent)  {_ASSERTE (0);}

	dDAGDimensionNode* m_expressionDimIndex;
	dAddRtti(dDAGExpressionNode);
};


#endif
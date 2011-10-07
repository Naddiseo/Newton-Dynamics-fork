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

#ifndef __dDAGScopeBlockNode_H__
#define __dDAGScopeBlockNode_H__

#include "dDirectAcyclicgraphNode.h"
#include "dDAGExpressionNodeConstant.h"
#include "dDAGExpressionNodeBinaryOperator.h"


class dDAGParameterNode;
class dDAGExpressionNode;
class dDAGExpressionNodeVariable;

class dDAGScopeBlockNode: public dDirectAcyclicgraphNode
{
	public:
	dDAGScopeBlockNode();
	~dDAGScopeBlockNode(void);

	void AddStatement (dDirectAcyclicgraphNode* const statement);
	

	dDAGExpressionNodeVariable* CreatedVariableNode (const char* const identifier);
	dDAGExpressionNodeConstant* CreatedConstantNode (dDAGExpressionNodeConstant::dType type, const char* const value);
	dDAGExpressionNodeBinaryOperator* CreateBinaryOperatorNode (dDAGExpressionNodeBinaryOperator::dBinaryOperator binaryOperator, dDAGExpressionNode* const epresionA, dDAGExpressionNode* const epresionB);

	

	dAddRtti(dDirectAcyclicgraphNode);
	
	dList<dDAGScopeBlockNode*> m_subScopeBlocks;
	dList<dDirectAcyclicgraphNode*> m_statementList;
	dTree<dDAGExpressionNode*, dCRCTYPE> m_expresionNodesCashe;
};


#endif
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
#include "dDAGFunctionStatement.h"
#include "dDAGExpressionNodeConstant.h"
#include "dDAGExpressionNodeUnuaryOperator.h"
#include "dDAGExpressionNodeBinaryOperator.h"


class dDAGParameterNode;
class dDAGExpressionNode;
class dDAGExpressionNodeVariable;

class dDAGScopeBlockNode: public dDAGFunctionStatement
{
	public:
	dDAGScopeBlockNode(dList<dDirectAcyclicgraphNode*>& allNodes);
	~dDAGScopeBlockNode(void);

	void AddStatement (dDAGFunctionStatement* const statement);

	dDAGExpressionNodeVariable* CreatedVariableNode (dList<dDirectAcyclicgraphNode*>& allNodes, const char* const identifier);
	dDAGExpressionNodeConstant* CreatedConstantNode (dList<dDirectAcyclicgraphNode*>& allNodes, dDAGExpressionNodeConstant::dType type, const char* const value);
	dDAGExpressionNodeUnuaryOperator* CreateUnuaryOperatorNode (dList<dDirectAcyclicgraphNode*>& allNodes, dDAGExpressionNodeUnuaryOperator::dUnuaryOperator unuaryOperator, dDAGExpressionNode* const expression);
	dDAGExpressionNodeBinaryOperator* CreateBinaryOperatorNode (dList<dDirectAcyclicgraphNode*>& allNodes, dDAGExpressionNodeBinaryOperator::dBinaryOperator binaryOperator, dDAGExpressionNode* const expressionA, dDAGExpressionNode* const expressionB);

	dAddRtti(dDAGFunctionStatement);

	dList<dDAGFunctionStatement*> m_statementList;
	dTree<dDAGExpressionNode*, dCRCTYPE> m_expresionNodesCashe;
};


#endif
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

#include "dDAG.h"
#include "dDAGFunctionStatement.h"
#include "dDAGExpressionNodeConstant.h"
#include "dDAGExpressionNodeUnuaryOperator.h"
#include "dDAGExpressionNodeBinaryOperator.h"

class dDAGDimensionNode;
class dDAGParameterNode;
class dDAGExpressionNode;
class dDAGExpressionNodeVariable;

class dDAGScopeBlockNode: public dDAGFunctionStatement
{
	public:
	dDAGScopeBlockNode(dList<dDAG*>& allNodes);
	~dDAGScopeBlockNode(void);

	void AddStatement (dDAGFunctionStatement* const statement);

	virtual void CompileCIL(dCIL& cil);
	virtual void ConnectParents(dDAG* const parent)  {_ASSERTE (0);}

	dDAGExpressionNodeVariable* CreatedVariableNode (dList<dDAG*>& allNodes, const char* const identifier, dDAGDimensionNode* const expressionDimIndex);
	dDAGExpressionNodeConstant* CreatedConstantNode (dList<dDAG*>& allNodes, dDAGExpressionNodeConstant::dType type, const char* const value);
	dDAGExpressionNodeUnuaryOperator* CreateUnuaryOperatorNode (dList<dDAG*>& allNodes, dDAGExpressionNodeUnuaryOperator::dUnuaryOperator unuaryOperator, dDAGExpressionNode* const expression);
	dDAGExpressionNodeBinaryOperator* CreateBinaryOperatorNode (dList<dDAG*>& allNodes, dDAGExpressionNodeBinaryOperator::dBinaryOperator binaryOperator, dDAGExpressionNode* const expressionA, dDAGExpressionNode* const expressionB);

	dAddRtti(dDAGFunctionStatement);

	dList<dDAGFunctionStatement*> m_statementList;
	dTree<dDAGExpressionNode*, dCRCTYPE> m_expressionNodesCache;
};


#endif
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

#include "dLSCstdafx.h"
#include "dDAG.h"
#include "dDAGFunctionStatement.h"
#include "dDAGParameterNode.h"
#include "dDAGScopeBlockNode.h"
#include "dDAGExpressionNodeVariable.h"
#include "dDAGExpressionNodeBinaryOperator.h"

dInitRtti(dDAGScopeBlockNode);


dDAGScopeBlockNode::dDAGScopeBlockNode(dList<dDAG*>& allNodes)
	:dDAGFunctionStatement(allNodes)
	,m_statementList()
	,m_expressionNodesCache()
{
}


dDAGScopeBlockNode::~dDAGScopeBlockNode()
{
	dTree<dDAGExpressionNode*, dCRCTYPE>::Iterator iter (m_expressionNodesCache);
	for (iter.Begin(); iter; iter ++) {
		dDAGExpressionNode* const node = iter.GetNode()->GetInfo();
		node->Release();
	}

	for (dList<dDAGFunctionStatement*>::dListNode* node = m_statementList.GetFirst(); node; node = node->GetNext()) {
		dDAGFunctionStatement* const stmnt = node->GetInfo();
		stmnt->Release();
	}
}


void dDAGScopeBlockNode::AddStatement (dDAGFunctionStatement* const statement)
{
	m_statementList.Append(statement);
	statement->AddRef();
}

dDAGExpressionNodeBinaryOperator* dDAGScopeBlockNode::CreateBinaryOperatorNode (dList<dDAG*>& allNodes, dDAGExpressionNodeBinaryOperator::dBinaryOperator binaryOperator, dDAGExpressionNode* const expressionA, dDAGExpressionNode* const expressionB)
{
	dCRCTYPE key = dDAGExpressionNodeBinaryOperator::CalculateKey (binaryOperator, expressionA, expressionB);
	
	dTree<dDAGExpressionNode*, dCRCTYPE>::dTreeNode* node = m_expressionNodesCache.Find(key);
	if (!node) {
		dDAGExpressionNodeBinaryOperator* const expresionNode = new dDAGExpressionNodeBinaryOperator (allNodes, binaryOperator, expressionA, expressionB);
		_ASSERTE (expresionNode->GetKey() == key);
		node = m_expressionNodesCache.Insert(expresionNode, key);
		expresionNode->AddRef();
	}
	dDAGExpressionNodeBinaryOperator* const expresionNode = (dDAGExpressionNodeBinaryOperator*)node->GetInfo();
	return expresionNode;
}


dDAGExpressionNodeUnuaryOperator* dDAGScopeBlockNode::CreateUnuaryOperatorNode (dList<dDAG*>& allNodes, dDAGExpressionNodeUnuaryOperator::dUnuaryOperator unuaryOperator, dDAGExpressionNode* const expression)
{
	dCRCTYPE key = dDAGExpressionNodeUnuaryOperator::CalculateKey (unuaryOperator, expression);

	dTree<dDAGExpressionNode*, dCRCTYPE>::dTreeNode* node = m_expressionNodesCache.Find(key);
	if (!node) {
		dDAGExpressionNodeUnuaryOperator* const expresionNode = new dDAGExpressionNodeUnuaryOperator (allNodes, unuaryOperator, expression);
		_ASSERTE (expresionNode->GetKey() == key);
		node = m_expressionNodesCache.Insert(expresionNode, key);
		expresionNode->AddRef();
	}
	dDAGExpressionNodeUnuaryOperator* const expresionNode = (dDAGExpressionNodeUnuaryOperator*)node->GetInfo();
	return expresionNode;
}


dDAGExpressionNodeVariable* dDAGScopeBlockNode::CreatedVariableNode (dList<dDAG*>& allNodes, const char* const identifier, dDAGDimensionNode* const expressionDimIndex)
{
	dCRCTYPE key = dCRC64 (identifier);
	dTree<dDAGExpressionNode*, dCRCTYPE>::dTreeNode* node = m_expressionNodesCache.Find(key);
	if (!node) {
		dDAGExpressionNodeVariable* const expresionNode = new dDAGExpressionNodeVariable (allNodes, identifier, expressionDimIndex);
		_ASSERTE (expresionNode->GetKey() == key);
		node = m_expressionNodesCache.Insert(expresionNode, key);
		expresionNode->AddRef();
	}
	dDAGExpressionNodeVariable* const expresionNode = (dDAGExpressionNodeVariable*)node->GetInfo();
	return expresionNode;
}

dDAGExpressionNodeConstant* dDAGScopeBlockNode::CreatedConstantNode (dList<dDAG*>& allNodes, dDAGExpressionNodeConstant::dType type, const char* const value)
{
	dCRCTYPE key = dCRC64 (value);

	dTree<dDAGExpressionNode*, dCRCTYPE>::dTreeNode* node = m_expressionNodesCache.Find(key);
	if (!node) {
		dDAGExpressionNodeConstant* const expresionNode = new dDAGExpressionNodeConstant (allNodes, type, value);
		_ASSERTE (expresionNode->GetKey() == key);
		node = m_expressionNodesCache.Insert(expresionNode, key);
		expresionNode->AddRef();
	}
	dDAGExpressionNodeConstant* const expresionNode = (dDAGExpressionNodeConstant*)node->GetInfo();
	return expresionNode;
}


void dDAGScopeBlockNode::CompileCIL(dCIL& cil)  
{
	for (dList<dDAGFunctionStatement*>::dListNode* node = m_statementList.GetFirst(); node; node = node->GetNext()) {
		dDAGFunctionStatement* const stmt = node->GetInfo();
		stmt->CompileCIL(cil);
	}
}
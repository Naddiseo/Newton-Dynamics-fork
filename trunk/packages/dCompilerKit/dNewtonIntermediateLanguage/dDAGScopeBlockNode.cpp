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
#include "dDAGFunctionStatement.h"
#include "dDAGParameterNode.h"
#include "dDAGScopeBlockNode.h"
#include "dDAGExpressionNodeVariable.h"
#include "dDAGExpressionNodeBinaryOperator.h"

dInitRtti(dDAGScopeBlockNode);


dDAGScopeBlockNode::dDAGScopeBlockNode(dList<dDirectAcyclicgraphNode*>& allNodes, dDAGFunctionStatement* const statementList)
	:dDAGFunctionStatement(allNodes)
	,m_statementList()
	,m_expresionNodesCashe()
{
	for (dDAGFunctionStatement* node = statementList; node; node = node->m_next) {
		_ASSERTE (0);
	}
}


dDAGScopeBlockNode::~dDAGScopeBlockNode()
{
	dTree<dDAGExpressionNode*, dCRCTYPE>::Iterator iter (m_expresionNodesCashe);
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

dDAGExpressionNodeBinaryOperator* dDAGScopeBlockNode::CreateBinaryOperatorNode (dList<dDirectAcyclicgraphNode*>& allNodes, dDAGExpressionNodeBinaryOperator::dBinaryOperator binaryOperator, dDAGExpressionNode* const expressionA, dDAGExpressionNode* const expressionB)
{
	dCRCTYPE key = dDAGExpressionNodeBinaryOperator::CalculateKey (binaryOperator, expressionA, expressionB);
	
	dTree<dDAGExpressionNode*, dCRCTYPE>::dTreeNode* node = m_expresionNodesCashe.Find(key);
	if (!node) {
		dDAGExpressionNodeBinaryOperator* const expresionNode = new dDAGExpressionNodeBinaryOperator (allNodes, binaryOperator, expressionA, expressionB);
		_ASSERTE (expresionNode->GetKey() == key);
		node = m_expresionNodesCashe.Insert(expresionNode, key);
	}
	dDAGExpressionNodeBinaryOperator* const expresionNode = (dDAGExpressionNodeBinaryOperator*)node->GetInfo();
	expresionNode->AddRef();
	return expresionNode;
}


dDAGExpressionNodeVariable* dDAGScopeBlockNode::CreatedVariableNode (dList<dDirectAcyclicgraphNode*>& allNodes, const char* const identifier)
{
	dCRCTYPE key = dCRC64 (identifier);
	dTree<dDAGExpressionNode*, dCRCTYPE>::dTreeNode* node = m_expresionNodesCashe.Find(key);
	if (!node) {
		dDAGExpressionNodeVariable* const expresionNode = new dDAGExpressionNodeVariable (allNodes, identifier);
		_ASSERTE (expresionNode->GetKey() == key);
		node = m_expresionNodesCashe.Insert(expresionNode, key);
	}
	dDAGExpressionNodeVariable* const expresionNode = (dDAGExpressionNodeVariable*)node->GetInfo();
	expresionNode->AddRef();
	return expresionNode;
}

dDAGExpressionNodeConstant* dDAGScopeBlockNode::CreatedConstantNode (dList<dDirectAcyclicgraphNode*>& allNodes, dDAGExpressionNodeConstant::dType type, const char* const value)
{
	dCRCTYPE key = dCRC64 (value);

	dTree<dDAGExpressionNode*, dCRCTYPE>::dTreeNode* node = m_expresionNodesCashe.Find(key);
	if (!node) {
		dDAGExpressionNodeConstant* const expresionNode = new dDAGExpressionNodeConstant (allNodes, type, value);
		_ASSERTE (expresionNode->GetKey() == key);
		node = m_expresionNodesCashe.Insert(expresionNode, key);
	}
	dDAGExpressionNodeConstant* const expresionNode = (dDAGExpressionNodeConstant*)node->GetInfo();
	expresionNode->AddRef();
	return expresionNode;
}
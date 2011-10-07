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
#include "dDAGParameterNode.h"
#include "dDAGScopeBlockNode.h"
#include "dDAGExpressionNodeVariable.h"
#include "dDAGExpressionNodeBinaryOperator.h"

dInitRtti(dDAGScopeBlockNode);



dDAGScopeBlockNode::dDAGScopeBlockNode()
	:dDirectAcyclicgraphNode()
	,m_localVariables()
	,m_subScopeBlocks()
	,m_statementList()
	,m_expresionNodesCashe()
{
}


dDAGScopeBlockNode::~dDAGScopeBlockNode()
{
	dTree<dDAGExpressionNode*, dCRCTYPE>::Iterator iter (m_expresionNodesCashe);
	for (iter.Begin(); iter; iter ++) {
		dDAGExpressionNode* const node = iter.GetNode()->GetInfo();
		node->Release();
	}

	for (dList<dDirectAcyclicgraphNode*>::dListNode* node = m_statementList.GetFirst(); node; node = node->GetNext()) {
		dDirectAcyclicgraphNode* const stmnt = node->GetInfo();
		stmnt->Release();
	}


	for (dList<dDAGParameterNode*>::dListNode* node = m_localVariables.GetFirst(); node; node = node->GetNext()) {
		dDAGParameterNode* const variable = node->GetInfo();
		variable->Release();
	}

	for (dList<dDAGScopeBlockNode*>::dListNode* node = m_subScopeBlocks.GetFirst(); node; node = node->GetNext()) {
		dDAGScopeBlockNode* const block = node->GetInfo();
		block->Release();
	}
}


void dDAGScopeBlockNode::AddLocalVariable (dDAGParameterNode* const variable)
{
	m_localVariables.Append(variable);
	variable->AddRef();
}


dDAGExpressionNodeBinaryOperator* dDAGScopeBlockNode::CreateBinaryOperatorNode (dDAGExpressionNodeBinaryOperator::dBinaryOperator binaryOperator, dDAGExpressionNode* const expressionA, dDAGExpressionNode* const expressionB)
{
	dCRCTYPE key = dDAGExpressionNodeBinaryOperator::CalculateKey (binaryOperator, expressionA, expressionB);
	
	dTree<dDAGExpressionNode*, dCRCTYPE>::dTreeNode* node = m_expresionNodesCashe.Find(key);
	if (!node) {
		dDAGExpressionNodeBinaryOperator* const expresionNode = new dDAGExpressionNodeBinaryOperator (binaryOperator, expressionA, expressionB);
		_ASSERTE (expresionNode->GetKey() == key);
		node = m_expresionNodesCashe.Insert(expresionNode, key);
	}
	dDAGExpressionNodeBinaryOperator* const expresionNode = (dDAGExpressionNodeBinaryOperator*)node->GetInfo();
	expresionNode->AddRef();
	return expresionNode;
}


dDAGExpressionNodeVariable* dDAGScopeBlockNode::CreatedVariableNode (const char* const identifier)
{
	dCRCTYPE key = dCRC64 (identifier);
	dTree<dDAGExpressionNode*, dCRCTYPE>::dTreeNode* node = m_expresionNodesCashe.Find(key);
	if (!node) {
		dDAGExpressionNodeVariable* const expresionNode = new dDAGExpressionNodeVariable (identifier);
		_ASSERTE (expresionNode->GetKey() == key);
		node = m_expresionNodesCashe.Insert(expresionNode, key);
	}
	dDAGExpressionNodeVariable* const expresionNode = (dDAGExpressionNodeVariable*)node->GetInfo();
	expresionNode->AddRef();
	return expresionNode;
}

dDAGExpressionNodeConstant* dDAGScopeBlockNode::CreatedConstantNode (dDAGExpressionNodeConstant::dType type, const char* const value)
{
	dCRCTYPE key = dCRC64 (value);

	dTree<dDAGExpressionNode*, dCRCTYPE>::dTreeNode* node = m_expresionNodesCashe.Find(key);
	if (!node) {
		dDAGExpressionNodeConstant* const expresionNode = new dDAGExpressionNodeConstant (type, value);
		_ASSERTE (expresionNode->GetKey() == key);
		node = m_expresionNodesCashe.Insert(expresionNode, key);
	}
	dDAGExpressionNodeConstant* const expresionNode = (dDAGExpressionNodeConstant*)node->GetInfo();
	expresionNode->AddRef();
	return expresionNode;
}
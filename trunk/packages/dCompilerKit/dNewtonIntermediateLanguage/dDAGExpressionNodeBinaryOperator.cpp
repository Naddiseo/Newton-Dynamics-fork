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
#include "dDAGTypeNode.h"
#include "dDAGExpressionNodeBinaryOperator.h"


dInitRtti(dDAGExpressionNodeBinaryOperator);

dDAGExpressionNodeBinaryOperator::dDAGExpressionNodeBinaryOperator(
	dList<dDirectAcyclicgraphNode*>& allNodes,
	dBinaryOperator binaryOperator, dDAGExpressionNode* const expressionA, dDAGExpressionNode* const expressionB)
	:dDAGExpressionNode(allNodes)
	,m_operator (binaryOperator)
	,m_expressionA (expressionA)
	,m_expressionB (expressionB)
{
	m_expressionA->AddRef();
	m_expressionB->AddRef();

	m_key = CalculateKey (binaryOperator, expressionA, expressionB);
}


dDAGExpressionNodeBinaryOperator::~dDAGExpressionNodeBinaryOperator(void)
{
	m_expressionA->Release();
	m_expressionB->Release();
}


dCRCTYPE dDAGExpressionNodeBinaryOperator::CalculateKey (dBinaryOperator binaryOperator, dDAGExpressionNode* const expressionA, dDAGExpressionNode* const expressionB)
{
	int tmp = binaryOperator;
	return dCRC64(&tmp, sizeof (tmp), dCombineCRC (expressionA->GetKey(), expressionB->GetKey()));
}
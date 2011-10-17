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

#include "dLittleScriptCompiler.h"
#include "dDirectAcyclicgraphNode.h"
#include "dDAGTypeNode.h"
#include "dDAGExpressionNodeUnuaryOperator.h"


dInitRtti(dDAGExpressionNodeUnuaryOperator);

dDAGExpressionNodeUnuaryOperator::dDAGExpressionNodeUnuaryOperator(dList<dDirectAcyclicgraphNode*>& allNodes, dUnuaryOperator unuaryOperator, dDAGExpressionNode* const expression)
	:dDAGExpressionNode(allNodes)
	,m_operator (unuaryOperator)
	,m_expression (expression)
{
	m_expression->AddRef();

	m_key = CalculateKey (m_operator, expression);
}


dDAGExpressionNodeUnuaryOperator::~dDAGExpressionNodeUnuaryOperator(void)
{
	m_expression->Release();
}


dCRCTYPE dDAGExpressionNodeUnuaryOperator::CalculateKey (dUnuaryOperator unuaryOperator, dDAGExpressionNode* const expression)
{
	int tmp = unuaryOperator;
	return dCRC64(&tmp, sizeof (tmp), expression->GetKey());
}
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
#include "dDAGTypeNode.h"
#include "dDAGExpressionNodeBinaryOperator.h"


dInitRtti(dDAGExpressionNodeBinaryOperator);

dDAGExpressionNodeBinaryOperator::dDAGExpressionNodeBinaryOperator(
	dList<dDAG*>& allNodes,
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


void dDAGExpressionNodeBinaryOperator::CompileCIL(dCIL& cil)  
{
	if (m_name == "") {
		dTreeAdressStmt& stmnt = cil.NewStatement()->GetInfo();
		m_name = cil.NewTemp ();		

		stmnt.m_instrution = dTreeAdressStmt::m_assigment;
		stmnt.m_arg0 = m_name;
		stmnt.m_arg1 = m_expressionA->m_name;
		stmnt.m_arg2 = m_expressionB->m_name;

		switch (m_operator) 
		{
			case m_lessEqual:
			{
				stmnt.m_operator = dTreeAdressStmt::m_lessEqual;
				break;
			}
			


			
			default:;
			_ASSERTE (0);
		}
	}
}
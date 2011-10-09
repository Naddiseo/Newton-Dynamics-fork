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

#ifndef __dDAGFunctionStatementAssigment_H__
#define __dDAGFunctionStatementAssigment_H__

#include "dDAGFunctionStatement.h"
#include "dDAGExpressionNodeVariable.h"
#include "dDAGExpressionNodeBinaryOperator.h"

class dDAGFunctionStatementAssigment: public dDAGFunctionStatement
{
	public:
	dDAGFunctionStatementAssigment(dList<dDirectAcyclicgraphNode*>& allNodes, dDAGExpressionNodeVariable* const leftVariable, dDAGExpressionNode* const expression);
	~dDAGFunctionStatementAssigment();

	dDAGExpressionNode* m_expression;
	dDAGExpressionNodeVariable* m_leftVariable;
	


	dAddRtti(dDAGFunctionStatement);
};


#endif
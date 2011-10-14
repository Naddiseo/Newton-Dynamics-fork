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

#ifndef __dDAGFunctionStatementReturn_H__
#define __dDAGFunctionStatementReturn_H__

#include "dDAGFunctionStatement.h"
#include "dDAGExpressionNode.h"


class dDAGFunctionStatementReturn: public dDAGFunctionStatement
{
	public:
	dDAGFunctionStatementReturn(dList<dDirectAcyclicgraphNode*>& allNodes, dDAGExpressionNode* const expression);
	~dDAGFunctionStatementReturn();

	dDAGExpressionNode* m_expression;
	


	dAddRtti(dDAGFunctionStatement);
};


#endif
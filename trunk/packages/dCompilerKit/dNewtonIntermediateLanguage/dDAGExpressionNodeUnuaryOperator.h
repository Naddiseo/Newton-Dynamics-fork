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

#ifndef __dDAGExpressionNodeUnuaryOperator_H__
#define __dDAGExpressionNodeUnuaryOperator_H__

#include "dDAGExpressionNode.h"

class dDAGExpressionNodeUnuaryOperator: public dDAGExpressionNode
{
	public:

	enum dUnuaryOperator
	{
		m_minus = '-',
		m_notBinay,
		m_notLogical
	};
	
	dDAGExpressionNodeUnuaryOperator (dList<dDirectAcyclicgraphNode*>& allNodes, dUnuaryOperator unuaryOperator, dDAGExpressionNode* const expression);
	~dDAGExpressionNodeUnuaryOperator(void);


	static dCRCTYPE CalculateKey (dUnuaryOperator unuaryOperator, dDAGExpressionNode* const expression);

	dUnuaryOperator m_operator;
	dDAGExpressionNode* m_expression;

	dAddRtti(dDAGExpressionNode);
};


#endif
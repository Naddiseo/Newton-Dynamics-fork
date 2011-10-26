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

#include "dDAG.h"
#include "dDAG.h"
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
	
	dDAGExpressionNodeUnuaryOperator (dList<dDAG*>& allNodes, dUnuaryOperator unuaryOperator, dDAGExpressionNode* const expression);
	~dDAGExpressionNodeUnuaryOperator(void);

	virtual void CompileCIL(dCIL& cil)  {_ASSERTE (0);}
	virtual void ConnectParent(dDAG* const parent)  {_ASSERTE (0);}

	dUnuaryOperator m_operator;
	dDAGExpressionNode* m_expression;

	dAddRtti(dDAGExpressionNode);
};


#endif
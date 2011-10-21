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

#ifndef __dDAGFunctionStatementDO_H__
#define __dDAGFunctionStatementDO_H__

#include "dDAG.h"
#include "dDAGFunctionStatement.h"

class dDAGExpressionNode;

class dDAGFunctionStatementDO: public dDAGFunctionStatement
{
	public:
	dDAGFunctionStatementDO(dList<dDAG*>& allNodes, dDAGExpressionNode* const expression, dDAGFunctionStatement* const stmt);
	~dDAGFunctionStatementDO();

	virtual void CompileCIL(dCIL& cil)  {_ASSERTE (0);}
	virtual void ConnectParent(dDAG* const parent)  {_ASSERTE (0);}

	dAddRtti(dDAGFunctionStatement);
	dDAGExpressionNode* m_expression;
	dDAGFunctionStatement* m_stmt;
};


#endif
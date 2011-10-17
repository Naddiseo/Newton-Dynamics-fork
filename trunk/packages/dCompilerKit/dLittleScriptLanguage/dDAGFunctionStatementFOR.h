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

#ifndef __dDAGFunctionStatementFOR_H__
#define __dDAGFunctionStatementFOR_H__

#include "dDAG.h"
#include "dDAGFunctionStatement.h"


class dDAGExpressionNode;

class dDAGFunctionStatementFOR: public dDAGFunctionStatement
{
	public:
	dDAGFunctionStatementFOR(dList<dDAG*>& allNodes, dDAGFunctionStatement* const beginStmt, dDAGExpressionNode* const expression, dDAGFunctionStatement* const endStmt, dDAGFunctionStatement* const stmt);
	~dDAGFunctionStatementFOR();

	virtual void ConnectParents(dDAG* const parent)  {_ASSERTE (0);}

	dAddRtti(dDAGFunctionStatement);
	dDAGFunctionStatement* m_initialStmt;
	dDAGExpressionNode* m_expression;
	dDAGFunctionStatement* m_endStmt;
	dDAGFunctionStatement* m_stmt;
};


#endif
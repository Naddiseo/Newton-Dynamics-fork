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

#ifndef __dDAGFunctionStatementWHILE_H__
#define __dDAGFunctionStatementWHILE_H__

#include "dDAG.h"
#include "dDAGFunctionStatement.h"

class dDAGExpressionNode;

class dDAGFunctionStatementWHILE: public dDAGFunctionStatement
{
	public:
	dDAGFunctionStatementWHILE(dList<dDAG*>& allNodes, dDAGExpressionNode* const expression, dDAGFunctionStatement* const stmt);
	~dDAGFunctionStatementWHILE();

	virtual void CompileCIL(dCIL& cil);
	virtual void ConnectParent(dDAG* const parent);

	dAddRtti(dDAGFunctionStatement);

	dDAGExpressionNode* m_expression;
	dDAGFunctionStatement* m_stmt;
};


#endif
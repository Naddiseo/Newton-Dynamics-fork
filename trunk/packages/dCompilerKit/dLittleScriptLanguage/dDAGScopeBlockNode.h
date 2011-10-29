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

#ifndef __dDAGScopeBlockNode_H__
#define __dDAGScopeBlockNode_H__

#include "dDAG.h"
#include "dDAGFunctionStatement.h"
#include "dDAGExpressionNodeConstant.h"
#include "dDAGExpressionNodeUnuaryOperator.h"
#include "dDAGExpressionNodeBinaryOperator.h"

class dDAGDimensionNode;
class dDAGParameterNode;
class dDAGExpressionNode;
class dDAGExpressionNodeVariable;

#define D_SCOPE_PREFIX		"_scope" 
#define D_RETURN_REGISTER	"t10000" 

class dDAGScopeBlockNode: public dDAGFunctionStatement
{
	public:
	class dLocalVariables: public dList<string>
	{
		public:
		dLocalVariables()
		{
		}

		bool FindVariable (const char* const name) const
		{
			for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
				if (node->GetInfo() == name) {
					return true;
				}
			}
			return false;
		}

	};

	dDAGScopeBlockNode(dList<dDAG*>& allNodes);
	~dDAGScopeBlockNode(void);

	void AddStatement (dDAGFunctionStatement* const statement);

	virtual void CompileCIL(dCIL& cil);
	virtual void ConnectParent(dDAG* const parent);

	dAddRtti(dDAGFunctionStatement);

	int m_scopeLayer;
	dList<dDAGFunctionStatement*> m_statementList;
	dLocalVariables m_localVariablesFilter;
};


#endif
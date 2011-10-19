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

#ifndef __dDAGParameterNode_H__
#define __dDAGParameterNode_H__

#include "dDAG.h"
#include "dDAGFunctionStatement.h"

class dDAGTypeNode;
class dDAGExpressionNode;
class dDAGParameterNode: public dDAGFunctionStatement
{
	public:
	dDAGParameterNode (dList<dDAG*>& allNodes, dDAGTypeNode* const type, const char* const identifier);
	~dDAGParameterNode(void);

	void SetInitializationExpression(dDAGExpressionNode* exp);

	virtual void CompileCIL(dCIL& cil); 
	virtual void ConnectParents(dDAG* const parent)  {_ASSERTE (0);}

	virtual void CalculateKey();

	bool m_isPublic;
	dDAGTypeNode* m_type;
	dDAGParameterNode* m_next;
	dDAGExpressionNode* m_initializationExp;
	dAddRtti(dDAGFunctionStatement);
};


#endif
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

#ifndef __dDAGFunctionNode_H__
#define __dDAGFunctionNode_H__

#include "dDirectAcyclicgraphNode.h"

class dDAGTypeNode;
class dDAGParameterNode;
class dDAGScopeBlockNode;

class dDAGFunctionNode: public dDirectAcyclicgraphNode
{
	public:
	dDAGFunctionNode();
	~dDAGFunctionNode(void);

	void FinalizeFunctionPrototypeNode (dDAGTypeNode* const type, const char* const name, const char* const isConst);

	virtual void CalculateKey();
	void AddParameter (const dDAGParameterNode* const parameter);

	void PushScope (dDAGScopeBlockNode* const scope);
	void PopScope ();

	bool m_isconst;
	dDAGTypeNode* m_returnType;
	dDAGScopeBlockNode* m_functionScopeBlock;
	dList<const dDAGParameterNode*> m_parameters; 
	
	dList<dDAGScopeBlockNode*> m_scopeStack;

	dAddRtti(dDirectAcyclicgraphNode);
};


#endif
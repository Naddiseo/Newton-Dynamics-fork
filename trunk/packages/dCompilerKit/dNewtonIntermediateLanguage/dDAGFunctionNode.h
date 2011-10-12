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
	dDAGFunctionNode(dList<dDirectAcyclicgraphNode*>& allNodes, dDAGTypeNode* const type, const char* const name, dDAGParameterNode* const parameterList, const char* const isConst);
	~dDAGFunctionNode(void);

	virtual void CalculateKey();

	void SetBody(dDAGScopeBlockNode* const body);

	bool m_isConst;
	bool m_isPrivate;
	dDAGTypeNode* m_returnType;
	dDAGScopeBlockNode* m_body;
	dList<dDAGParameterNode*> m_parameters; 
	


	dAddRtti(dDirectAcyclicgraphNode);
};


#endif
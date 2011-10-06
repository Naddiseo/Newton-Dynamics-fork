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

#include "dDirectAcyclicgraphNode.h"

class dDAGParameterNode;

class dDAGScopeBlockNode: public dDirectAcyclicgraphNode
{
	public:
	dDAGScopeBlockNode();
	~dDAGScopeBlockNode(void);

	void AddLocalVariable (dDAGParameterNode* const variable);

	dAddRtti(dDirectAcyclicgraphNode);

	dList<dDAGParameterNode*> m_localVariables;
	dList<dDAGScopeBlockNode*> m_subScopeBlocks;
};


#endif
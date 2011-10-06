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

#include "dDirectAcyclicgraphNode.h"

class dDAGTypeNode;
class dDAGParameterNode: public dDirectAcyclicgraphNode
{
	public:
	dDAGParameterNode (dDAGTypeNode* const type, const char* const identifier);
	~dDAGParameterNode(void);

	virtual void CalculateKey();

	bool m_isPublic;
	dDAGTypeNode* m_type;
	dAddRtti(dDirectAcyclicgraphNode);
};


#endif
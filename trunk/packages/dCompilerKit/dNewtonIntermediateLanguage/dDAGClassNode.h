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

#ifndef __dDAGClassNode_H__
#define __dDAGClassNode_H__

#include "dDirectAcyclicgraphNode.h"

class dDAGClassNode: public dDirectAcyclicgraphNode
{
	public:
	dDAGClassNode();
	~dDAGClassNode(void);

	virtual void CalculateKey();

	string m_name;
	const dDAGClassNode* m_baseClass;

	dAddRtti(dDirectAcyclicgraphNode);
};


#endif
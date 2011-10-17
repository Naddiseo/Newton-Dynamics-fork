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

#ifndef __dDAGFunctionStatement_H__
#define __dDAGFunctionStatement_H__

#include "dDirectAcyclicgraphNode.h"
#include "dLittleScriptCompiler.h"


class dDAGFunctionStatement: public dDirectAcyclicgraphNode
{
	public:
	dDAGFunctionStatement(dList<dDirectAcyclicgraphNode*>& allNodes);
	~dDAGFunctionStatement();

	dAddRtti(dDirectAcyclicgraphNode);
};


#endif
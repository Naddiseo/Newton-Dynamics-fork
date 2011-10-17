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

#ifndef __dDAGTypeNode_H__
#define __dDAGTypeNode_H__

#include "dDAG.h"
#include "dLittleScriptCompiler.h"

class dDAGDimensionNode;

class dDAGTypeNode: public dDAG
{
	public:
	dDAGTypeNode(dList<dDAG*>& allNodes, const char* const type, const char* const modifier);
	~dDAGTypeNode(void);

	void AddDimensions (dDAGDimensionNode* dimList);

	virtual void CalculateKey();

	dList<dDAGDimensionNode*> m_dimensions;

	dAddRtti(dDAG);
};


#endif
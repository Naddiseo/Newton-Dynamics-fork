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

#include "dDirectAcyclicgraphNode.h"
#include "dSyntaxTreeCode.h"

dSyntaxTreeCode::dSyntaxTreeCode(void)
	:dTree<dDirectAcyclicgraphNode*, dCRCTYPE> ()
{
}

dSyntaxTreeCode::~dSyntaxTreeCode(void)
{
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		dDirectAcyclicgraphNode* const node = iter.GetNode()->GetInfo();
		delete node;
	}
}

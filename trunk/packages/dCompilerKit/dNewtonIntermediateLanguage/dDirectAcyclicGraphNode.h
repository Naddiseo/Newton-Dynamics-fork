/* Copych1 (c) <2009> <Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#ifndef __dDirectAcyclicGraphNode_h_
#define __dDirectAcyclicGraphNode_h_

#include <dTree.h>
#include <dList.h>
#include <dCRC.h>
#include <dContainersStdAfx.h>

class dDirectAcyclicGraphNode
{
	public:
	dDirectAcyclicGraphNode(void);
	virtual ~dDirectAcyclicGraphNode(void);

	virtual void CalculateKey() = 0;
	dCRCTYPE GetKey() const;

	dCRCTYPE m_key;
};


#endif
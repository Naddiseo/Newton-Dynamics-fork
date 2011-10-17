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

#ifndef __dDirectAcyclicgraphNode_h_
#define __dDirectAcyclicgraphNode_h_

#include <dTree.h>
#include <dList.h>
#include <dCRC.h>
#include <dRtti.h>
#include <dRefCounter.h>

#include <string>
using namespace std;

class dDirectAcyclicgraphNode: public dRefCounter
{
	public:
	dDirectAcyclicgraphNode(dList<dDirectAcyclicgraphNode*>& allNodes);
	virtual ~dDirectAcyclicgraphNode(void);

	dCRCTYPE GetKey() const;
	virtual void CalculateKey() {m_key = 0;}
	
	
	string m_name;
	dCRCTYPE m_key;

	dRttiRootClassSupportDeclare(dDirectAcyclicgraphNode);
	
};


#endif
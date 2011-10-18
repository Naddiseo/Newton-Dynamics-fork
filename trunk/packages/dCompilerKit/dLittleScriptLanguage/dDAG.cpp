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

#include "dLSCstdafx.h"
#include "dDAG.h"

dRttiRootClassSupportImplement(dDAG);

dDAG::dDAG(dList<dDAG*>& allNodes)
	:m_name ("")
	,m_key(0)
	,m_parent (NULL)
{
	allNodes.Append(this);
}

dDAG::~dDAG(void)
{
}


dCRCTYPE dDAG::GetKey() const
{
	return m_key;
}

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

#ifndef __dDAG_h_
#define __dDAG_h_

#include "dLSCstdafx.h"

#ifdef _MSC_VER
#pragma warning (disable: 4100) // warning C4100: unreferenced formal parameter
#endif


class dDAG: public dRefCounter
{
	public:
	dDAG(dList<dDAG*>& allNodes);
	virtual ~dDAG(void);
	
	virtual void CompileCIL(dCIL& cil)  {_ASSERTE (0);}
	virtual void ConnectParent(dDAG* const parent) = 0;
	
	string m_name;
	string m_result;
	dDAG* m_parent;

	dRttiRootClassSupportDeclare(dDAG);
	
};


#endif
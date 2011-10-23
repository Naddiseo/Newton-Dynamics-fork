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

#include "dCILstdafx.h"
#include "dCIL.h"

dCIL::dCIL(void)
	:dList()
	,m_tempIndex (0)
	,m_labelIndex (0)
{
}

dCIL::~dCIL(void)
{
}

void dCIL::ResetTemporaries()
{
	m_tempIndex = 0;
	m_labelIndex = 0;
}

string dCIL::NewTemp ()
{
	char tmp[256];
	sprintf (tmp, "t%0d", m_tempIndex);
	m_tempIndex ++;
	return string (tmp);
}

string dCIL::NewLabel ()
{
	char tmp[256];
	sprintf (tmp, "label%d", m_labelIndex);
	m_labelIndex ++;
	return string (tmp);
}

dCIL::dListNode* dCIL::NewStatement()
{
	return Append();
}


void dCIL::Optimize(dListNode* const scopeSegment)
{

}
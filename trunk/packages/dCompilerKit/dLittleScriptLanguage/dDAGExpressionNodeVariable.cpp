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
#include "dDAGTypeNode.h"
#include "dDAGDimensionNode.h"
#include "dDAGExpressionNodeVariable.h"


dInitRtti(dDAGExpressionNodeVariable);

dDAGExpressionNodeVariable::dDAGExpressionNodeVariable(dList<dDAG*>& allNodes, const char* const identifier, dDAGDimensionNode* const expressionDimIndex)
	:dDAGExpressionNode(allNodes)
	,m_expressionDimIndex (expressionDimIndex)
{
	m_name = identifier;
	
	dCRCTYPE crc = 0;
	if (m_expressionDimIndex) {
		m_expressionDimIndex->AddRef();
		crc = m_expressionDimIndex->GetKey();
	}
	m_key = dCRC64 (m_name.c_str(), crc);
}


dDAGExpressionNodeVariable::~dDAGExpressionNodeVariable(void)
{
	if (m_expressionDimIndex) {
		m_expressionDimIndex->Release();
	}
}

void dDAGExpressionNodeVariable::CompileCIL(dCIL& cil)
{
	
}

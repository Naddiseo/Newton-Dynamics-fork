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

#include "dLittleScriptCompiler.h"
#include "dDAG.h"
#include "dDAGExpressionNodeFunctionCall.h"
#include "dDAGFunctionStatementFunctionCall.h"

dInitRtti(dDAGFunctionStatementFunctionCall);

dDAGFunctionStatementFunctionCall::dDAGFunctionStatementFunctionCall(dList<dDAG*>& allNodes, dDAGExpressionNodeFunctionCall* const function)
	:dDAGFunctionStatement(allNodes)
	,m_function(function)
{
	if (m_function) {
		m_function->AddRef();
	}
}


dDAGFunctionStatementFunctionCall::~dDAGFunctionStatementFunctionCall()
{
	if (m_function) {
		m_function->Release();
	}
}



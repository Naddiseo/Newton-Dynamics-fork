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
#include "dDAGFunctionNode.h"
#include "dDAGFunctionStatementFlow.h"
#include "dDAGFunctionStatementBREAK.h"

dInitRtti(dDAGFunctionStatementBREAK);

dDAGFunctionStatementBREAK::dDAGFunctionStatementBREAK(dList<dDAG*>& allNodes)
	:dDAGFunctionStatement(allNodes)
{
}


dDAGFunctionStatementBREAK::~dDAGFunctionStatementBREAK()
{
}

void dDAGFunctionStatementBREAK::ConnectParent(dDAG* const parent)
{
	m_parent = parent;
}

void dDAGFunctionStatementBREAK::CompileCIL(dCIL& cil)  
{
	for (dDAG* node = m_parent; node && (node->GetTypeId() != dDAGFunctionNode::GetRttiType()); node = node->m_parent) {
		if (node->IsType(dDAGFunctionStatementFlow::GetRttiType())) {
			dDAGFunctionStatementFlow* const flowControl = (dDAGFunctionStatementFlow*) node;
			dTreeAdressStmt& stmt = cil.NewStatement()->GetInfo();
			stmt.m_instruction = dTreeAdressStmt::m_goto;
			stmt.m_arg0.m_label = flowControl->m_currentExitLabel;
			dTRACE_INTRUCTION (&stmt);
			return ;
		} 
	}

	// warning break instruction outside of a flow control statement
	_ASSERTE (0);
}

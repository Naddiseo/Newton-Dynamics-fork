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
#include "dDAGExpressionNode.h"
#include "dDAGFunctionStatementFlow.h"


dInitRtti(dDAGFunctionStatementFlow);

dDAGFunctionStatementFlow::dDAGFunctionStatementFlow(dList<dDAG*>& allNodes)
	:dDAGFunctionStatement(allNodes)
	,m_currentExitLabel()
	,m_backPatchStart (NULL)
{
}


dDAGFunctionStatementFlow::~dDAGFunctionStatementFlow()
{
}


void dDAGFunctionStatementFlow::ConnectParent(dDAG* const parent)
{
	_ASSERTE (0);
}


void dDAGFunctionStatementFlow::CompileCIL(dCIL& cil)  
{
	m_backPatchStart = cil.GetLast();
	m_currentExitLabel = cil.NewLabel();
}

void dDAGFunctionStatementFlow::BackPatch (dCIL& cil)
{
	bool flowBreak = false;
	if (!m_backPatchStart) {
		m_backPatchStart = cil.GetFirst();
	}
	
	for (dCIL::dListNode* node = m_backPatchStart->GetNext(); node; node = node->GetNext()) {
		dTreeAdressStmt& stmt = node->GetInfo();
		if ((stmt.m_instruction == dTreeAdressStmt::m_goto) && !stmt.m_jmpTarget)  {
			flowBreak = true;
			break;
		}
	}

	if (flowBreak) {
		dCIL::dListNode* const target = cil.NewStatement();
		dTreeAdressStmt& jmpTarget = target->GetInfo();
		jmpTarget.m_instruction = dTreeAdressStmt::m_label;
		jmpTarget.m_arg0.m_label = m_currentExitLabel;
		dTRACE_INTRUCTION (&jmpTarget);

		for (dCIL::dListNode* node = m_backPatchStart->GetNext(); node; node = node->GetNext()) {
			dTreeAdressStmt& stmt = node->GetInfo();
			if ((stmt.m_instruction == dTreeAdressStmt::m_goto) && !stmt.m_jmpTarget)  {
				stmt.m_jmpTarget = target;
			}
		}
	}
}
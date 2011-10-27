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
#include "dFlowControlBlock.h"

dCIL::dCIL(void)
	:dList()
	,m_tempIndex (0)
	,m_labelIndex (0)
{
	memset (m_conditinals, 0, sizeof (m_conditinals));
	m_conditinals[dTreeAdressStmt::m_equal] = dTreeAdressStmt::m_equal;
	m_conditinals[dTreeAdressStmt::m_different] = dTreeAdressStmt::m_different;
	m_conditinals[dTreeAdressStmt::m_less] = dTreeAdressStmt::m_less;
	m_conditinals[dTreeAdressStmt::m_lessEqual] = dTreeAdressStmt::m_lessEqual;
	m_conditinals[dTreeAdressStmt::m_greather] = dTreeAdressStmt::m_greather;
	m_conditinals[dTreeAdressStmt::m_greatherEqual] = dTreeAdressStmt::m_greatherEqual;


	memset (m_operatorComplement, 0, sizeof (m_operatorComplement));
	m_operatorComplement[dTreeAdressStmt::m_equal] = dTreeAdressStmt::m_different;
	m_operatorComplement[dTreeAdressStmt::m_different] = dTreeAdressStmt::m_equal;
	m_operatorComplement[dTreeAdressStmt::m_less] = dTreeAdressStmt::m_greatherEqual;
	m_operatorComplement[dTreeAdressStmt::m_lessEqual] = dTreeAdressStmt::m_greather;
	m_operatorComplement[dTreeAdressStmt::m_greather] = dTreeAdressStmt::m_lessEqual;
	m_operatorComplement[dTreeAdressStmt::m_greatherEqual] = dTreeAdressStmt::m_less;
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


void dCIL::Trace()
{
	for (dCIL::dListNode* node = GetFirst(); node; node = node->GetNext()) {
		const dTreeAdressStmt& stmt = node->GetInfo();
		dTRACE_INTRUCTION(&stmt);
	}
	DTRACE(("\n"));
}



void dCIL::Optimize(dListNode* const function)
{
	dTree<dFlowControlBlock*, dCIL::dListNode*> blocksMap;
	dFlowControlBlock* const flowDiagramRoot = new dFlowControlBlock(function);
	blocksMap.Insert(flowDiagramRoot, flowDiagramRoot->m_leader);

	dFlowControlBlock* last = flowDiagramRoot;
	for (dCIL::dListNode* node = function->GetNext(); node; node = node->GetNext()) {
		const dTreeAdressStmt& stmt = node->GetInfo();
		if (stmt.m_instruction == dTreeAdressStmt::m_target) {
			last->m_end = node->GetPrev();
			last->m_nextBlock = new dFlowControlBlock(node);
			blocksMap.Insert(last->m_nextBlock, last->m_nextBlock->m_leader);
			last = last->m_nextBlock;
		} else if (stmt.m_instruction == dTreeAdressStmt::m_if) {
			dCIL::dListNode* const next = node->GetNext();
			const dTreeAdressStmt& stmt = next->GetInfo();
			if (stmt.m_instruction != dTreeAdressStmt::m_target) {
				last->m_end = next->GetPrev();
				last->m_nextBlock = new dFlowControlBlock(next);
				blocksMap.Insert(last->m_nextBlock, last->m_nextBlock->m_leader);
				last = last->m_nextBlock;
			}
		}
	}
	last->m_end =  GetLast()->GetPrev();

	for (dFlowControlBlock* block = flowDiagramRoot; block; block = block->m_nextBlock) {
		_ASSERTE (block->m_end);
		const dTreeAdressStmt& stmt = block->m_end->GetInfo();
		if ((stmt.m_instruction == dTreeAdressStmt::m_if) || (stmt.m_instruction == dTreeAdressStmt::m_goto)) {
			_ASSERTE (blocksMap.Find(stmt.m_jmpTarget));
			dFlowControlBlock* const targetBlock = blocksMap.Find(stmt.m_jmpTarget)->GetInfo();
			block->m_branchTarget = targetBlock;
		}
	}


	// eliminate local common sub expression
	DTRACE(("\n"));
	for (dFlowControlBlock* block = flowDiagramRoot; block; block = block->m_nextBlock) {
		block->ApplyLocalOptimizations(*this);
//		block->Trace();
	}


	dFlowControlBlock* next;
	for (dFlowControlBlock* block = flowDiagramRoot; block; block = next) {
		next = block->m_nextBlock;
		delete block;
	}

	Trace();
}

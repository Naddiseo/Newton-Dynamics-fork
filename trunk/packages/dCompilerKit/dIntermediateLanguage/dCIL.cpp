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


void dCIL::GetFlowControlBlockList (dFlowControlBlock* const root, dList<dFlowControlBlock*>& list)
{
	int stack = 1;
	dFlowControlBlock* pool[256];

	int mark = root->m_mark + 1;
	pool[0] = root;
	while (stack) {
		stack --;
		dFlowControlBlock* const block = pool[stack];
		if (block->m_mark != mark) {
			list.Append(block);
			block->m_mark = mark; 
		}

		if (block->m_branchBlock && (block->m_branchBlock->m_mark < mark)) {
			pool[stack] = block->m_branchBlock;
			stack ++;
		}
		if (block->m_nextBlock && (block->m_nextBlock->m_mark < mark)) {
			pool[stack] = block->m_nextBlock;
			stack ++;
		}
	}

}

void dCIL::Optimize(dListNode* const function)
{
	dFlowControlBlock* const flowDiagramRoot = new dFlowControlBlock(function);

	dList<dFlowControlBlock*> flowBlockList;
	GetFlowControlBlockList (flowDiagramRoot, flowBlockList);

	// eliminate local common sub expression
DTRACE(("\n"));
	for (dList<dFlowControlBlock*>::dListNode* node = flowBlockList.GetFirst(); node; node = node->GetNext()) {
		dFlowControlBlock* const flowBlock = node->GetInfo();


		flowBlock->ApplyLocalOptimizations();
//		flowBlock->Trace();
	}
		

	for (dList<dFlowControlBlock*>::dListNode* node = flowBlockList.GetFirst(); node; node = node->GetNext()) {
		dFlowControlBlock* const flowBlock = node->GetInfo();
		delete flowBlock;
	}
}
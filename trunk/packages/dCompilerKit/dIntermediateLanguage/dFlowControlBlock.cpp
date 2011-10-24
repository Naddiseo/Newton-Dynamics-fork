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
#include "dTreeAdressStmt.h"
#include "dFlowControlBlock.h"


dFlowControlBlock::dFlowControlBlock(dCIL::dListNode* const root)
	:m_mark(0)
	,m_begin(root)
	,m_end(NULL)
	,m_nextBlock(NULL)
	,m_branchBlock(NULL)
{
	dTree<dFlowControlBlock*, dCIL::dListNode*> filter;
	AddBlock(root, filter);
}

dFlowControlBlock::dFlowControlBlock(dCIL::dListNode* const root, dTree<dFlowControlBlock*, dCIL::dListNode*>& filter)
	:m_mark(0)
	,m_begin(root)
	,m_end(NULL)
	,m_nextBlock(NULL)
	,m_branchBlock(NULL)
{
	AddBlock(root, filter);
}

dFlowControlBlock::~dFlowControlBlock(void)
{
}



void dFlowControlBlock::Trace() const
{

	dCIL::dListNode* node = m_begin;
	const dTreeAdressStmt& stmt = node->GetInfo();
	dTRACE_INTRUCTION(&stmt);
	while (node != m_end) {
		node = node->GetNext();
		const dTreeAdressStmt& stmt = node->GetInfo();
		dTRACE_INTRUCTION(&stmt);
	}
	DTRACE(("\n"));
}




void dFlowControlBlock::AddBlock(dCIL::dListNode* const root, dTree<dFlowControlBlock*, dCIL::dListNode*>& filter)
{
	_ASSERTE (!filter.Find(root));
	filter.Insert(this, root);

	m_begin = root;

	for (dCIL::dListNode* node = m_begin; node; node = node->GetNext()) {
		m_end = node;
		const dTreeAdressStmt& stmt = node->GetInfo();
		switch (stmt.m_instruction)
		{
			case dTreeAdressStmt::m_target:
			{
				dTree<dFlowControlBlock*, dCIL::dListNode*>::dTreeNode* const block = filter.Find(node->GetNext()); 
				if (block) {
					m_nextBlock = block->GetInfo();
				} else {
					m_nextBlock = new dFlowControlBlock (node->GetNext(), filter);
				}
				return;
			}

			case dTreeAdressStmt::m_if:
			{
				dTree<dFlowControlBlock*, dCIL::dListNode*>::dTreeNode* const block = filter.Find(node->GetNext()); 
				if (block) {
					m_nextBlock = block->GetInfo();
				} else {
					m_nextBlock = new dFlowControlBlock (node->GetNext(), filter);
				}

				dTree<dFlowControlBlock*, dCIL::dListNode*>::dTreeNode* const branchBlock = filter.Find(stmt.m_jmpTarget->GetNext()); 
				if (branchBlock) {
					m_branchBlock = branchBlock->GetInfo();
				} else {
					m_branchBlock = new dFlowControlBlock (stmt.m_jmpTarget->GetNext(), filter);
				}
				return;
			}

			case dTreeAdressStmt::m_goto:
			{
				dTree<dFlowControlBlock*, dCIL::dListNode*>::dTreeNode* const branchBlock = filter.Find(stmt.m_jmpTarget->GetNext()); 
				if (branchBlock) {
					m_branchBlock = branchBlock->GetInfo();
				} else {
					m_branchBlock = new dFlowControlBlock (stmt.m_jmpTarget->GetNext(), filter);
				}

				return;
			}
		}
	}
}


void dFlowControlBlock::OptimizeSubexpression()
{

}


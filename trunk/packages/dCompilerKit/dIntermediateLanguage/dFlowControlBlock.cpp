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
	:m_begin(root)
	,m_end(NULL)
	,m_nextBlock(NULL)
	,m_branchBlock(NULL)
{
/*
	for (dCIL::dListNode* node = m_begin; node; node = node->GetNext()) {
		const dTreeAdressStmt& stmt = node->GetInfo();
		switch (stmt.m_instruction)
		{
			case dTreeAdressStmt::m_target:
			{
				m_end = node;
				m_nextBlock = new dFlowControlBlock (node->GetNext());
				return;
			}

			case dTreeAdressStmt::m_if:
			case dTreeAdressStmt::m_ifnot:
			{
				m_end = node;
				m_nextBlock = new dFlowControlBlock (node->GetNext());
			}


			case dTreeAdressStmt::m_goto:
				_ASSERTE (0);
				break;
		}
	}
*/
}

dFlowControlBlock::~dFlowControlBlock(void)
{
	if (m_branchBlock) {
		delete m_branchBlock;
	}
	if (m_nextBlock) {
		delete m_nextBlock;
	}
}

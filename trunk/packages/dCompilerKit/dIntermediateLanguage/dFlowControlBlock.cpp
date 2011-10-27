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
		const dTreeAdressStmt& stmt = node->GetInfo();
		if (stmt.m_instruction != dTreeAdressStmt::m_nop) {
			m_end = node;

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
}


void dFlowControlBlock::ApplyLocalOptimizations(dCIL& program)
{
static int xxx = 0;
xxx ++;
if(xxx < 4)
return;

	Trace();
	RemoveSubExpressions_1(program);
	RemoveSubExpressions_2(program);

Trace();
}


void dFlowControlBlock::ApplyCopyProgation(dCIL& program)
{

}


bool dFlowControlBlock::RemoveSubExpressions_1(dCIL& program)
{
	bool ret = false;
	const dCIL::dListNode* const lastNode = m_end->GetNext();
	_ASSERTE (lastNode);
	for (dCIL::dListNode* node = m_begin; node != lastNode; node = node->GetNext()) {
		dTreeAdressStmt& stmt = node->GetInfo();
		if (stmt.m_instruction == dTreeAdressStmt::m_assigment) {
			dCIL::dListNode* next = NULL;
			for (dCIL::dListNode* node1 = node->GetNext(); node1 != lastNode; node1 = next) {
				dTreeAdressStmt& stmt1 = node1->GetInfo();
				next = node1->GetNext();
				if (stmt.m_arg0 == stmt1.m_arg0) {
					break;
				}
				if ((stmt.m_instruction == stmt1.m_instruction) && (stmt.m_operator == stmt1.m_operator) &&	(stmt.m_arg1 == stmt1.m_arg1) && (stmt.m_arg2 == stmt1.m_arg2)) {
					for (dCIL::dListNode* node2 = node1->GetNext(); node2 != lastNode; node2 = node2->GetNext()) {
						dTreeAdressStmt& stmt2 = node2->GetInfo();
						if (stmt2.m_arg0 == stmt1.m_arg0) {
							stmt2.m_arg0 = stmt.m_arg0;
						}
						if (stmt2.m_arg1 == stmt1.m_arg0) {
							stmt2.m_arg1 = stmt.m_arg0;
						}
						if (stmt2.m_arg2 == stmt1.m_arg0) {
							stmt2.m_arg2 = stmt.m_arg0;
						}
					}

					ret = true;
					if (stmt1.m_arg0[0] == 't') {
						program.Remove(node1);
Trace();
					}
				} 
			}
		}
	}
	return ret;
}

bool dFlowControlBlock::RemoveSubExpressions_2(dCIL& program)
{
	int b[10];
	int a[10];
	for (int i = 0; i < 10; i = i + 1) {
		b[i] = a[i];
	}	



	bool ret = false;
	const dCIL::dListNode* const lastNode = m_end->GetNext();
	_ASSERTE (lastNode);
	dCIL::dListNode* next = NULL;
	for (dCIL::dListNode* node = m_begin; node != lastNode; node = next) {
		dTreeAdressStmt& stmt = node->GetInfo();
		next = node->GetNext();
		if (stmt.m_instruction == dTreeAdressStmt::m_assigment) {
			bool alive = true;
			if (stmt.m_operator == dTreeAdressStmt::m_nothing) {
				for (dCIL::dListNode* node1 = node->GetNext(); node1 != lastNode; node1 = node1->GetNext()) {
					dTreeAdressStmt& stmt1 = node1->GetInfo();
					switch (stmt1.m_instruction) 
					{
						case dTreeAdressStmt::m_assigment:
						{
							if (stmt1.m_arg1 == stmt.m_arg0) {
								alive = false;
								stmt1.m_arg1 = stmt.m_arg1;
							}
							if (stmt1.m_arg2 == stmt.m_arg0) {
								alive = false;
								stmt1.m_arg2 = stmt.m_arg1;
							}
							break;
						}
						case dTreeAdressStmt::m_if:
							_ASSERTE (0);
					}
				}
			} else {
				for (dCIL::dListNode* node1 = node->GetNext(); node1 != lastNode; node1 = node1->GetNext()) {
					dTreeAdressStmt& stmt1 = node1->GetInfo();
					switch (stmt1.m_instruction) 
					{
						case dTreeAdressStmt::m_assigment:
						{
							if (stmt1.m_operator == dTreeAdressStmt::m_nothing) {
								if (stmt1.m_arg1 == stmt.m_arg0) {
									alive = false;
									stmt1.m_operator = stmt.m_operator;
									stmt1.m_arg1 = stmt.m_arg1;
									stmt1.m_arg2 = stmt.m_arg2;
								}
							}
							break;
						}


						case dTreeAdressStmt::m_if:
						{
							switch (stmt1.m_operator) 
							{
								case dTreeAdressStmt::m_equal:
								{
									if (stmt1.m_arg1 == "0") {
										alive = false;
										stmt1.m_operator = program.m_operatorComplement[stmt.m_operator];
										stmt1.m_arg0 = stmt.m_arg1;
										stmt1.m_arg1 = stmt.m_arg2;
									}
									break;
								}

								case dTreeAdressStmt::m_different:
								{
									if (stmt1.m_arg1 == "0") {
										alive = false;
										stmt1.m_operator = stmt.m_operator;
										stmt1.m_arg0 = stmt.m_arg1;
										stmt1.m_arg1 = stmt.m_arg2;
									}
									break;
								}


								default:
									break;
							}
						}

					}
				}
			}

			if (!alive) {
				if (m_begin == node) {
					m_begin = node->GetNext();
				}
				program.Remove(node);
Trace();
			}
		}
	}
	return ret;
}


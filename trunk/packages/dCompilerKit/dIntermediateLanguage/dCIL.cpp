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
	,m_mark(1)
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






void dCIL::MakeFlowControlGraph(dFlowControlBlock* const root, dTree<dFlowControlBlock*, dCIL::dListNode*>& blocksMap)
{
	int stack = 1;
	dFlowControlBlock* pool[32];

	m_mark ++;

	pool[0] = root;
	while (stack) {
		stack --;
		dFlowControlBlock* const node = pool[stack];
		if (node->m_mark < m_mark) {

			node->m_mark = m_mark;

			const dTreeAdressStmt& stmt = node->m_end->GetInfo();
			if (stmt.m_instruction == dTreeAdressStmt::m_if) {

				_ASSERTE (blocksMap.Find(stmt.m_jmpTarget));
				dFlowControlBlock* const child0 = blocksMap.Find(stmt.m_jmpTarget)->GetInfo();
				node->AddGraphEdge(child0);
				pool[stack] = child0;
				stack ++;

				_ASSERTE (blocksMap.Find(node->m_end->GetNext()));
				dFlowControlBlock* const child1 = blocksMap.Find(node->m_end->GetNext())->GetInfo();
				node->AddGraphEdge(child1);
				pool[stack] = child1;
				stack ++;

			} else if (stmt.m_instruction == dTreeAdressStmt::m_goto) {
				_ASSERTE (blocksMap.Find(stmt.m_jmpTarget));
				dFlowControlBlock* const child = blocksMap.Find(stmt.m_jmpTarget)->GetInfo();
				node->AddGraphEdge(child);
				pool[stack] = child;
				stack ++;
			} else if (node->m_end->GetNext()->GetNext()) {
				_ASSERTE (blocksMap.Find(node->m_end->GetNext()));
				dFlowControlBlock* const child = blocksMap.Find(node->m_end->GetNext())->GetInfo();
				node->AddGraphEdge(child);
				pool[stack] = child;
				stack ++;
			}
		}
	}

#ifdef _DEBUG
	for (dFlowControlBlock* block = root; block; block = block->m_nextBlock) {
		_ASSERTE (block->m_mark == m_mark);
	}
#endif
}





void dCIL::RemoveRedundantJumps(dListNode* const function)
{
	dTree<int, dListNode*> jumpMap;

	// create jump and label map;
	for (dListNode* node = function; node; node = node->GetNext()) {
		const dTreeAdressStmt& stmt = node->GetInfo();
		switch (stmt.m_instruction) 
		{
			case dTreeAdressStmt::m_if:
			case dTreeAdressStmt::m_label:
			case dTreeAdressStmt::m_goto:
				jumpMap.Insert(0, node);
		}
	}

	// remove redundant adjacent labels
	dTree<int, dListNode*>::Iterator iter (jumpMap);
	for (iter.Begin(); iter; iter ++) {
		dListNode* const node = iter.GetKey();
		const dTreeAdressStmt& stmt = node->GetInfo();
		if (stmt.m_instruction == dTreeAdressStmt::m_label) {
			dListNode* const labelNode = node->GetNext();
			if (labelNode && (labelNode->GetInfo().m_instruction == dTreeAdressStmt::m_label)) {
				dTree<int, dListNode*>::Iterator iter1 (jumpMap);
				for (iter1.Begin(); iter1; iter1 ++) {
					dListNode* const node1 = iter1.GetKey();
					dTreeAdressStmt& stmt1 = node1->GetInfo();
					if (stmt1.m_instruction == dTreeAdressStmt::m_goto) {
						if (stmt1.m_jmpTarget == labelNode)	{
							stmt1.m_jmpTarget = node;
							stmt1.m_arg0 = stmt.m_arg0;
						}
					} else if (stmt1.m_instruction == dTreeAdressStmt::m_if) { 
						if (stmt1.m_jmpTarget == labelNode)	{
							stmt1.m_jmpTarget = node;	
							stmt1.m_arg2 = stmt.m_arg0;
						}
					}
				}
				Remove(labelNode);
				jumpMap.Remove(labelNode);
			}
		}
	}

	// redirect double indirect goto
	for (iter.Begin(); iter; iter ++) {
		dListNode* const node = iter.GetKey();
		dTreeAdressStmt& stmt = node->GetInfo();
		if (stmt.m_instruction == dTreeAdressStmt::m_goto) {
			_ASSERTE (jumpMap.Find (stmt.m_jmpTarget));
			dListNode* const targetNode = jumpMap.Find (stmt.m_jmpTarget)->GetKey();
			dTreeAdressStmt& stmt1 = targetNode->GetInfo();
			dListNode* const nextGotoNode = targetNode->GetNext();
			if ((stmt1.m_instruction == dTreeAdressStmt::m_label) && (nextGotoNode->GetInfo().m_instruction == dTreeAdressStmt::m_goto)) {
				const dTreeAdressStmt& stmt2 = nextGotoNode->GetInfo();
				stmt.m_arg0 = stmt2.m_arg0;
				stmt.m_jmpTarget = stmt2.m_jmpTarget;
			}
		}
	}

	// remove jumps over jumps
	for (iter.Begin(); iter; iter ++) {
		dListNode* const node = iter.GetKey();
		dTreeAdressStmt& stmt = node->GetInfo();
		if (stmt.m_instruction == dTreeAdressStmt::m_if) {
			dListNode* const gotoNode = node->GetNext();
			dTreeAdressStmt& gotoStmt = gotoNode->GetInfo();
			if (gotoStmt.m_instruction == dTreeAdressStmt::m_goto) {
				dListNode* const target = gotoNode->GetNext();
				if (stmt.m_jmpTarget == target) {
					dTreeAdressStmt& gotoStmt = gotoNode->GetInfo();
					stmt.m_operator = m_operatorComplement[stmt.m_operator];
					stmt.m_jmpTarget = gotoStmt.m_jmpTarget;
					stmt.m_arg2 = gotoStmt.m_arg0;
					Remove(gotoNode);
				}
			}
		}
	}


	// remove goto to immediate labels
	for (iter.Begin(); iter; ) {
		dListNode* const node = iter.GetKey();
		dTreeAdressStmt& stmt = node->GetInfo();
		iter ++;
		dListNode* const nextNode = node->GetNext();
		if ((stmt.m_instruction == dTreeAdressStmt::m_goto) && (stmt.m_jmpTarget == nextNode)) {
			Remove(node);
			jumpMap.Remove(node);
		}
	}


	// delete unreferenced labels
	for (iter.Begin(); iter; ) {
		dListNode* const node = iter.GetKey();
		dTreeAdressStmt& stmt = node->GetInfo();
		iter ++;
		if (stmt.m_instruction == dTreeAdressStmt::m_label) {		
			dTree<int, dListNode*>::Iterator iter1 (jumpMap);
			bool isReferenced = false;
			for (iter1.Begin(); iter1; iter1 ++) {
				dListNode* const node1 = iter1.GetKey();
				dTreeAdressStmt& stmt1 = node1->GetInfo();
				if ((stmt1.m_instruction == dTreeAdressStmt::m_goto) || (stmt1.m_instruction == dTreeAdressStmt::m_if)){
					if (stmt1.m_jmpTarget == node) {
						isReferenced = true;
						break;
					}
				}
			}
			if (!isReferenced) {
				Remove(node);
				jumpMap.Remove(node);
			}
		}
	}

	// delete dead code labels
	for (iter.Begin(); iter; ) {
		dListNode* const node = iter.GetKey();
		dTreeAdressStmt& stmt = node->GetInfo();
		iter ++;
		if (stmt.m_instruction == dTreeAdressStmt::m_goto) {
			for (dListNode* deadNode = node->GetNext(); deadNode && (deadNode->GetInfo().m_instruction != dTreeAdressStmt::m_label); deadNode = node->GetNext()) {
				Remove(deadNode);
			}
		}
	}

	Trace();
}



void dCIL::Optimize(dListNode* const function)
{
	// eliminate redundant jumps
	RemoveRedundantJumps(function);

	// build leading block map table
	dTree<dFlowControlBlock*, dCIL::dListNode*> blocksMap;
	dFlowControlBlock* const flowDiagramRoot = new dFlowControlBlock(function);
	blocksMap.Insert(flowDiagramRoot, flowDiagramRoot->m_leader);

	dFlowControlBlock* last = flowDiagramRoot;
	for (dCIL::dListNode* node = function->GetNext(); node; node = node->GetNext()) {
		const dTreeAdressStmt& stmt = node->GetInfo();
		if (stmt.m_instruction == dTreeAdressStmt::m_label) {
			last->m_end = node->GetPrev();
			last->m_nextBlock = new dFlowControlBlock(node);
			blocksMap.Insert(last->m_nextBlock, last->m_nextBlock->m_leader);
			last = last->m_nextBlock;
		} else if (stmt.m_instruction == dTreeAdressStmt::m_if) {
			dCIL::dListNode* const next = node->GetNext();
			const dTreeAdressStmt& stmt = next->GetInfo();
			if (stmt.m_instruction != dTreeAdressStmt::m_label) {
				last->m_end = next->GetPrev();
				last->m_nextBlock = new dFlowControlBlock(next);
				blocksMap.Insert(last->m_nextBlock, last->m_nextBlock->m_leader);
				last = last->m_nextBlock;
			}
		} else {
			const dTreeAdressStmt& prevInst = node->GetPrev()->GetInfo();
			if ((prevInst.m_instruction == dTreeAdressStmt::m_if) && !blocksMap.Find(node)) {
				last->m_end = node->GetPrev();
				last->m_nextBlock = new dFlowControlBlock(node);
				blocksMap.Insert(last->m_nextBlock, last->m_nextBlock->m_leader);
				last = last->m_nextBlock;
			}
		}
	}
	last->m_end =  GetLast()->GetPrev();

	// create float control for inteBlock optimization
	MakeFlowControlGraph(flowDiagramRoot, blocksMap);

	// Apply local internal blocks optimizations
	DTRACE(("\n"));
	for (dFlowControlBlock* block = flowDiagramRoot; block; block = block->m_nextBlock) {
		block->ApplyLocalOptimizations(*this);
		block->Trace();
	}

	// apply global optimization
	ApplyGlobalOptimization (flowDiagramRoot);

	// destroy all flow control blocks
	dFlowControlBlock* next;
	for (dFlowControlBlock* block = flowDiagramRoot; block; block = next) {
		next = block->m_nextBlock;
		delete block;
	}
	Trace();
}


void dCIL::ApplyGlobalOptimization (dFlowControlBlock* const root)
{
	PackTmpVariables(root);
}


void dCIL::PackTmpVariables(dFlowControlBlock* const root)
{
	int id = 0;
	dTree<int, int> tmpMap;
	for (dCIL::dListNode* node = root->m_leader; node; node = node->GetNext()) {	
		dTreeAdressStmt& stmt = node->GetInfo();		
		if (stmt.m_arg0[0] == 't') {
			int index = atoi (&stmt.m_arg0[1]);
			if (!tmpMap.Find(index)) {
				tmpMap.Insert(id, index);
				id ++;
			}
		}
		if (stmt.m_arg1[0] == 't') {
			int index = atoi (&stmt.m_arg1[1]);
			if (!tmpMap.Find(index)) {
				tmpMap.Insert(id, index);
				id ++;
			}
		}
		if (stmt.m_arg2[0] == 't') {
			int index = atoi (&stmt.m_arg2[1]);
			if (!tmpMap.Find(index)) {
				tmpMap.Insert(id, index);
				id ++;
			}
		}
	}

	for (dCIL::dListNode* node = root->m_leader; node; node = node->GetNext()) {	
		dTreeAdressStmt& stmt = node->GetInfo();		
		if (stmt.m_arg0[0] == 't') {
			int index = atoi (&stmt.m_arg0[1]);
			dTree<int, int>::dTreeNode* const node = tmpMap.Find(index);
			_ASSERTE (node);
			char text[128];
			sprintf (text, "t%d", node->GetInfo());
			stmt.m_arg0 = text;
		}
		if (stmt.m_arg1[0] == 't') {
			int index = atoi (&stmt.m_arg1[1]);
			dTree<int, int>::dTreeNode* const node = tmpMap.Find(index);
			_ASSERTE (node);
			char text[128];
			sprintf (text, "t%d", node->GetInfo());
			stmt.m_arg1 = text;
		}
		if (stmt.m_arg2[0] == 't') {
			int index = atoi (&stmt.m_arg2[1]);
			dTree<int, int>::dTreeNode* const node = tmpMap.Find(index);
			_ASSERTE (node);
			char text[128];
			sprintf (text, "t%d", node->GetInfo());
			stmt.m_arg2 = text;
		}
	}

//for (dFlowControlBlock* block = root; block; block = block->m_nextBlock) {
//block->Trace();
//}

}
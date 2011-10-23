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

#ifndef __dFlowControlBlock_H_
#define __dFlowControlBlock_H_

class dCIL;
class dTreeAdressStmt;

class dFlowControlBlock
{
	public:
	dFlowControlBlock(dCIL::dListNode* const root);
	virtual ~dFlowControlBlock(void);

	dCIL::dListNode* m_begin;
	dCIL::dListNode* m_end;
	dFlowControlBlock* m_nextBlock;
	dFlowControlBlock* m_branchBlock;
};


#endif
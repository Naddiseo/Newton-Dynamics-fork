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

#ifndef __dCIL_H_
#define __dCIL_H_

#include "dCILstdafx.h"
#include "dTreeAdressStmt.h"

class dFlowControlBlock;

class dCIL: public dList<dTreeAdressStmt>
{
	public:
	
	dCIL(void);
	virtual ~dCIL(void);

	string NewTemp (); 
	string NewLabel (); 
	void ResetTemporaries();
	dListNode* NewStatement();

	void Trace();
	void Optimize(dListNode* const function);
	void RemoveRedundantJumps(dListNode* const function);

	private:
	void PackTmpVariables(dFlowControlBlock* const root);
	void ApplyGlobalOptimization (dFlowControlBlock* const root);
	void MakeFlowControlGraph(dFlowControlBlock* const root, dTree<dFlowControlBlock*, dCIL::dListNode*>& blocksMap);


	int m_tempIndex;
	int m_labelIndex;
	dTreeAdressStmt::dOperator m_conditinals[dTreeAdressStmt::m_operatorsCount];
	dTreeAdressStmt::dOperator m_operatorComplement[dTreeAdressStmt::m_operatorsCount];

	friend dFlowControlBlock;
};


#endif
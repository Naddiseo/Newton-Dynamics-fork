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
#include "dDAGTypeNode.h"
#include "dDAGDimensionNode.h"
#include "dDAGScopeBlockNode.h"
#include "dDAGExpressionNodeVariable.h"
#include "dDAGFunctionStatementAssigment.h"

dInitRtti(dDAGExpressionNodeVariable);

dDAGExpressionNodeVariable::dDAGExpressionNodeVariable(dList<dDAG*>& allNodes, const char* const identifier, dDAGDimensionNode* const expressionDimIndex)
	:dDAGExpressionNode(allNodes)
	,m_dimExpressions ()
{
	m_name = identifier;


	dDAGDimensionNode* next;
	for (dDAGDimensionNode* node = expressionDimIndex; node; node = next) {
		next = node->m_next;
		node->m_next = NULL;
		_ASSERTE (node->IsType(dDAGDimensionNode::GetRttiType()));
		m_dimExpressions.Append(node);
		node->AddRef();
	}
}



dDAGExpressionNodeVariable::~dDAGExpressionNodeVariable(void)
{
	for (dList<dDAGDimensionNode*>::dListNode* node = m_dimExpressions.GetFirst(); node; node = node->GetNext()) {
		dDAGDimensionNode* const dim = node->GetInfo();
		dim->Release();
	}
}

void dDAGExpressionNodeVariable::ConnectParent(dDAG* const parent)  
{
	m_parent = parent;
	for (dList<dDAGDimensionNode*>::dListNode* node = m_dimExpressions.GetFirst(); node; node = node->GetNext()) {
		dDAGDimensionNode* const dim = node->GetInfo();
		dim->ConnectParent(this);
	}
}


void dDAGExpressionNodeVariable::CompileCIL(dCIL& cil)
{
	size_t pos =  m_name.find (D_SCOPE_PREFIX, 0, strlen (D_SCOPE_PREFIX));
	if (pos != 0) {
		bool state = RenameLocalVariable(m_name);
		if (!state) {
			DTRACE (("undefined local variable\n"));
			_ASSERTE (0);
		}
	}


	if (m_dimExpressions.GetCount()) {
		dDAGDimensionNode* const dim = m_dimExpressions.GetFirst()->GetInfo();
		dim->CompileCIL(cil);
		dCIL::dListNode* const dimInstruction = cil.NewStatement();
		dTreeAdressStmt& addressIndex = dimInstruction->GetInfo();
		addressIndex.m_instruction = dTreeAdressStmt::m_assigment;
		addressIndex.m_arg0 = cil.NewTemp();
		addressIndex.m_arg1 = dim->m_result; 

		string result = addressIndex.m_arg0;
		dTRACE_INTRUCTION (&addressIndex);

		for (dList<dDAGDimensionNode*>::dListNode* node = m_dimExpressions.GetFirst()->GetNext(); node; node = node->GetNext()) {
			dDAGDimensionNode* const dim = node->GetInfo();
			dim->CompileCIL(cil);
			
			dTreeAdressStmt& stmtMul = cil.NewStatement()->GetInfo();
			stmtMul.m_instruction = dTreeAdressStmt::m_assigment;
			stmtMul.m_operator = dTreeAdressStmt::m_mul;
			stmtMul.m_arg0 = cil.NewTemp();
			stmtMul.m_arg1 = result;
			stmtMul.m_arg2 = dim->m_arraySize;

			dTRACE_INTRUCTION (&stmtMul);

			dTreeAdressStmt& stmtAdd = cil.NewStatement()->GetInfo();
			stmtAdd.m_instruction = dTreeAdressStmt::m_assigment;
			stmtAdd.m_operator = dTreeAdressStmt::m_add;
			stmtAdd.m_arg0 = cil.NewTemp();
			stmtAdd.m_arg1 = stmtMul.m_arg0;
			stmtAdd.m_arg2 = dim->m_result;

			result = stmtAdd.m_arg0;

			dTRACE_INTRUCTION (&stmtAdd);
		}

		dTreeAdressStmt& dimSize = cil.NewStatement()->GetInfo();
		dimSize.m_instruction = dTreeAdressStmt::m_assigment;
		dimSize.m_operator = dTreeAdressStmt::m_mul;
		dimSize.m_arg0 = cil.NewTemp();
		dimSize.m_arg1 = result; 
		dimSize.m_arg2 = "4"; 
		dTRACE_INTRUCTION (&dimSize);

		result = dimSize.m_arg0;

		_ASSERTE (m_parent);
		if (m_parent->GetTypeId() == dDAGFunctionStatementAssigment::GetRttiType()) {
			dDAGFunctionStatementAssigment* const asmt = (dDAGFunctionStatementAssigment*) m_parent;
			if (asmt->m_leftVariable == this) {
				m_result = m_name + '[' + result + ']';
			} else {
				// emit an indirect addressing mode
				dTreeAdressStmt& tmp = cil.NewStatement()->GetInfo();
				m_result = m_name + '[' + addressIndex.m_arg0 + ']';
				tmp.m_instruction = dTreeAdressStmt::m_assigment;
				tmp.m_arg0 = cil.NewTemp();
				tmp.m_arg1 = m_name + '[' + result + ']';
				dTRACE_INTRUCTION (&tmp);
				m_result = tmp.m_arg0; 
			}
			
		} else {
			// emit an indirect addressing mode
			dTreeAdressStmt& tmp = cil.NewStatement()->GetInfo();
			m_result = m_name + '[' + addressIndex.m_arg0 + ']';
			tmp.m_instruction = dTreeAdressStmt::m_assigment;
			tmp.m_arg0 = cil.NewTemp();;
			tmp.m_arg1 = m_name + '[' + result + ']';
			dTRACE_INTRUCTION (&tmp);
			m_result = tmp.m_arg0; 
		}
	} else {
		m_result = m_name;
	}

}

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

#ifndef __dDAGFunctionNode_H__
#define __dDAGFunctionNode_H__

#include "dDAG.h"
#include "dLSCstdafx.h"

class dDAGTypeNode;
class dDAGParameterNode;
class dDAGScopeBlockNode;
class dDAGFunctionModifier;

class dDAGFunctionNode: public dDAG
{
	public:
	dDAGFunctionNode(dList<dDAG*>& allNodes, dDAGTypeNode* const type, const char* const name, dDAGParameterNode* const parameterList, const char* const isConst);
	~dDAGFunctionNode(void);


	void SetBody(dDAGScopeBlockNode* const body);
	void SetModifier(dDAGFunctionModifier* const modifier);

	virtual void CompileCIL(dCIL& cil);
	virtual void ConnectParent(dDAG* const parent);

	bool FindArgumentVariable(const char* name) const;

	bool m_isConst;
	bool m_isPrivate;
	bool m_isConstructor;
	dDAGTypeNode* m_returnType;
	dDAGScopeBlockNode* m_body;
	dDAGFunctionModifier* m_modifier;
	dList<dDAGParameterNode*> m_parameters; 
	dAddRtti(dDAG);
};


#endif
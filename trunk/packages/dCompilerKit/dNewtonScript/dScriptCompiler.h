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

//
//Auto generated Parser Generator class: dScriptCompiler.h
//

#ifndef __dScriptCompiler_h__
#define __dScriptCompiler_h__

#include <dTree.h>
#include <dList.h>
#include <dCRC.h>
#include <dContainersStdAfx.h>

#include <dDAGTypeNode.h>
#include <dDAGClassNode.h>
#include <dSyntaxTreeCode.h>
#include <dDAGFunctionNode.h>
#include <dDAGParameterNode.h>
#include <dDAGScopeBlockNode.h>
#include <dDAGExpressionNode.h>
#include <dDAGFunctionStatement.h>
#include "dDAGFunctionStatementIF.h"
#include <dDirectAcyclicgraphNode.h>
#include <dDAGExpressionNodeConstant.h>
#include <dDAGExpressionNodeVariable.h>
#include <dDAGExpressionNodeBinaryOperator.h>

#include "dNewtonScriptParser.h"


//class dVirtualMachine;
class dSyntaxTreeCode;
class dDirectAcyclicgraphNode;

class dScriptCompiler: public dNewtonScriptParser
{
	public:
	dScriptCompiler(const char* const sourceFileName);
	virtual ~dScriptCompiler();
	int CompileSource (const char* const source);

	protected:
	virtual bool Parse(dNewtonScriptLexical& scanner);


	void DisplayError (const char* format, ...) const;
	void SyntaxError (const dNewtonScriptLexical& scanner, const dUserVariable& errorToken, const dUserVariable& errorTokenMarker);

	
	dUserVariable BeginClassNode ();
	dUserVariable FinalizeClassNode (const dUserVariable& classNode, const dUserVariable& visibility, const dUserVariable& name, const dUserVariable& baseClass, const dUserVariable& guiInterface);

	dUserVariable AddClassFunction (const dUserVariable& returnType, const dUserVariable& function, const dUserVariable& functionBlock);
	
//	dUserVariable BeginBeginFunctionPrototypeNode ();
//	dUserVariable FinalizePrototype (const dUserVariable& returnType, const dUserVariable& funtionName, const dUserVariable& isConst);
	dUserVariable NewFunctionPrototype (const dUserVariable& returnType, const dUserVariable& funtionName, const dUserVariable& parameterList, const dUserVariable& isConst);

//	dUserVariable BeginScopeBlock ();
//	dUserVariable FinalizeScopeBlock (const dUserVariable& scope);

	dUserVariable NewScopeBlock (const dUserVariable& statementsList);

	dUserVariable NewParameterNode (const dUserVariable& primitiveType, const dUserVariable& identifier);
	dUserVariable EmitTypeNode (const dUserVariable& type, const dUserVariable& modifier = dUserVariable());
	dUserVariable LinkParameters(const dUserVariable& parameterA, const dUserVariable& parameterB);
	
	
	dUserVariable NewExpressionNodeConstant (const dUserVariable& identifier);
	dUserVariable NewExpressionNodeVariable (const dUserVariable& identifier);
	dUserVariable NewExpresionNodeAssigment (const dUserVariable& leftVariable, const dUserVariable& expression);
	dUserVariable NewExpressionNodeBinaryOperator (const dUserVariable& binaryOperator, const dUserVariable& expressionA, const dUserVariable& expressionB);
	
	void AddClassVariable(const dUserVariable& variable);
	void SetParamameterAsPrivateVariable(const dUserVariable& variable);
	
	void AddStatementToCurrentBlock(const dUserVariable& statement);
	void AddLocalVaribleToCurrentBlock(const dUserVariable& variable, const dUserVariable& initExpression);
	void AddStatementIFToCurrentBlock(const dUserVariable& expression, const dUserVariable& thenBlock, const dUserVariable& elseBlock);

	dDAGClassNode* GetCurrentClass() const;


	const char* m_fileName;
	dDAGFunctionNode* m_currentFunction;
	dList<dDAGClassNode*> m_classList;
	dList<dDirectAcyclicgraphNode*> m_allNodes;

	friend class dNewtonScriptParser;
};

#endif

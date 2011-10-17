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
//Auto generated Parser Generator class: dLittleScriptCompiler.h
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
#include <dDAGDimensionNode.h>
#include <dDAGScopeBlockNode.h>
#include <dDAGExpressionNode.h>
#include <dDAGFunctionModifier.h>
#include <dDAGFunctionStatement.h>
#include <dDirectAcyclicgraphNode.h>
#include <dDAGFunctionStatementIF.h>
#include <dDAGFunctionStatementDO.h>
#include <dDAGFunctionStatementFOR.h>
#include <dDAGFunctionStatementBREAK.h>
#include <dDAGFunctionStatementWHILE.h>
#include <dDAGExpressionNodeFunctionCall.h>
#include <dDAGExpressionNodeConstant.h>
#include <dDAGExpressionNodeVariable.h>
#include <dDAGFunctionStatementReturn.h>
#include <dDAGFunctionStatementAssigment.h>
#include <dDAGExpressionNodeBinaryOperator.h>
#include <dDAGExpressionNodeUnuaryOperator.h>
#include <dDAGFunctionStatementFunctionCall.h>

#include "dLittleScriptParser.h"


//class dVirtualMachine;
class dSyntaxTreeCode;
class dLittleScriptLexical;
class dDirectAcyclicgraphNode;

class dScriptCompiler: public dLittleScriptParser
{
	public:
	dScriptCompiler(const char* const sourceFileName);
	virtual ~dScriptCompiler();
	int CompileSource (const char* const source);

	protected:
	virtual bool Parse(dLittleScriptLexical& scanner);


	void DisplayError (const char* format, ...) const;
	void SyntaxError (const dLittleScriptLexical& scanner, const dUserVariable& errorToken, const dUserVariable& errorTokenMarker);

	void AddClassFunction(const dUserVariable& function);
	void AddClassVariable(const dUserVariable& variable);
	void AddStatementToCurrentBlock(const dUserVariable& statement);

	
	dUserVariable BeginClassNode ();
	dUserVariable FinalizeClassNode (const dUserVariable& classNode, const dUserVariable& visibility, const dUserVariable& name, const dUserVariable& baseClass, const dUserVariable& guiInterface);

	dUserVariable NewClassFunction (const dUserVariable& modifier, const dUserVariable& functionProtype, const dUserVariable& functionBody);
	dUserVariable NewConstructorPrototype (const dUserVariable& functionName, const dUserVariable& parameterList, const dUserVariable& initBaseClass);
	dUserVariable NewFunctionPrototype (const dUserVariable& returnType, const dUserVariable& functionName, const dUserVariable& parameterList, const dUserVariable& isConst);
	dUserVariable NewFunctionModifier (const dUserVariable& modifirer, const dUserVariable& appendModifirer);
	dUserVariable BeginScopeBlock ();
	dUserVariable EndScopeBlock (const dUserVariable& block);
	dUserVariable NewParameterNode (const dUserVariable& primitiveType, const dUserVariable& identifier);
	dUserVariable EmitTypeNode (const dUserVariable& type, const dUserVariable& modifier = dUserVariable());
	dUserVariable LinkParameters(const dUserVariable& parameterA, const dUserVariable& parameterB);
	dUserVariable LinkExpressions(const dUserVariable& ExpressionA, const dUserVariable& ExpressionB);
	dUserVariable NewExpressionNodeConstant (const dUserVariable& identifier);
	dUserVariable NewExpressionNodeVariable (const dUserVariable& identifier, const dUserVariable& dimArray);
	dUserVariable NewExpresionNodeAssigment (const dUserVariable& leftVariable, const dUserVariable& dimArray, const dUserVariable& expression);
	dUserVariable NewExpressionNodeUnuaryOperator (const dUserVariable& unuaryOperator, const dUserVariable& expression);
	dUserVariable NewExpressionNodeBinaryOperator (const dUserVariable& binaryOperator, const dUserVariable& expressionA, const dUserVariable& expressionB);
	
	dUserVariable NewExpressionFunctionCall (const dUserVariable& functionName, const dUserVariable& argumnetList);

	dUserVariable NewDimensionNode(const dUserVariable& expression);
	dUserVariable TypeAddDimention(const dUserVariable& type, const dUserVariable& dimension);
	
	dUserVariable NewLocalVariableStatement(const dUserVariable& variable, const dUserVariable& initializationExpression);
	dUserVariable NewBreakStatement();
	dUserVariable NewDoStatement(const dUserVariable& expression, const dUserVariable& statement);
	dUserVariable NewWhileStatement(const dUserVariable& expression, const dUserVariable& statement);
	dUserVariable NewIFStatement(const dUserVariable& expression, const dUserVariable& thenBlock, const dUserVariable& elseBlock);
	
	dUserVariable NewForStatement(const dUserVariable& forScope, const dUserVariable& init_exp, const dUserVariable& conditional, const dUserVariable& step_Exp, const dUserVariable& statement);
	dUserVariable NewReturnStatement(const dUserVariable& expression);
	dUserVariable NewFunctionCallStatement(const dUserVariable& functionExpression);
	

	dDAGClassNode* GetCurrentClass() const;
	dDAGScopeBlockNode* GetCurrentScope() const;

	const char* m_fileName;
	dDAGFunctionNode* m_currentFunction;
	dList<dDAGClassNode*> m_classList;
	dList<dDAGScopeBlockNode*> m_scopeStack;
	dList<dDirectAcyclicgraphNode*> m_allNodes;

	friend class dLittleScriptParser;
};

#endif

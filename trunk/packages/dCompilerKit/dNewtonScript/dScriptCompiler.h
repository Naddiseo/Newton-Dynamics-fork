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
#include <dDirectAcyclicgraphNode.h>

#include "dNewtonScriptParser.h"


//class dVirtualMachine;
class dSyntaxTreeCode;
class dDirectAcyclicgraphNode;

class dScriptCompiler: public dNewtonScriptParser
{
	public:
	enum dPasses
	{
		m_first,
		m_secund,
	};
	dScriptCompiler(const char* const sourceFileName);
	virtual ~dScriptCompiler();
	int CompileSource (const char* const source);

	protected:
	virtual bool Parse(dNewtonScriptLexical& scanner);


	void DisplayError (const char* format, ...) const;
	void SyntaxError (const dNewtonScriptLexical& scanner, const dUserVariable& errorToken, const dUserVariable& errorTokenMarker);


	dDirectAcyclicgraphNode* AddNode (dDirectAcyclicgraphNode* const node);

	dUserVariable NewClassDefinitionNode ();

	dUserVariable BeginBeginFunctionPrototypeNode ();
	dUserVariable FinalizeFunctionPrototypeNode (const dUserVariable& reurnType, const dUserVariable& funtionName, const dUserVariable& isConst);

	dUserVariable BeginScopeBlock ();
	dUserVariable FinalizeScopeBlock (const dUserVariable& scope);

	dUserVariable NewParameterNode (const dUserVariable& primitiveType, const dUserVariable& identifier);

	dUserVariable EmitTypeNode (const dUserVariable& type, const dUserVariable& modifier = dUserVariable());
	

	dPasses m_pass;
	const char* m_fileName;
	dSyntaxTreeCode* m_syntaxTree;

	dDAGClassNode* m_currentClass;
	dDAGFunctionNode* m_currentFunction;

	friend class dNewtonScriptParser;
};

#endif

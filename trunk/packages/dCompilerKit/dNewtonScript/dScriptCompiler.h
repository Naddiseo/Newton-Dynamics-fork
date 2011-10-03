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

#include <dDAGClass.h>
#include <dSyntaxTreeCode.h>
#include <dDirectAcyclicGraphNode.h>

#include "dNewtonScriptParser.h"


//class dVirtualMachine;
class dSyntaxTreeCode;
class dDirectAcyclicGraphNode;

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
	dUserVariable NewClassDefinition (const dUserVariable& visibility, const dUserVariable& name, const dUserVariable& deriveFrom);

	dDirectAcyclicGraphNode* AddNode (dDirectAcyclicGraphNode* const node);

	const char* m_fileName;
	dSyntaxTreeCode* m_syntaxTree;

	dDAGClass* m_currentClass;

	friend class dNewtonScriptParser;
};

#endif

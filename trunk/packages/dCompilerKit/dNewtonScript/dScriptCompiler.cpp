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
//Auto generated Parser Generator class: dScriptCompiler.cpp
//

#include "dScriptCompiler.h"
#include "dNewtonScriptLexical.h"




dScriptCompiler::dScriptCompiler(const char* const sourceFileName)
	:dNewtonScriptParser ()
	,m_fileName(sourceFileName)
	,m_syntaxTree (NULL)
	,m_currentClass(NULL)
{
}

dScriptCompiler::~dScriptCompiler()
{
	if (m_syntaxTree) {
		delete m_syntaxTree;
	}	
}

//inline void dExpandTraceMessage (const char *fmt, ...)
void dScriptCompiler::DisplayError (const char* format, ...) const
{
	va_list v_args;
	char* const text = (char*) malloc (strlen (format) + 2048);

	text[0] = 0;
	va_start (v_args, format);     
	vsprintf(text, format, v_args);
	va_end (v_args);            

	fprintf (stderr, text);
	OutputDebugStringA (text);

	free (text);
}


//int dScriptCompiler::CompileSource (dVirtualMachine* const virtualMachine, const char* const source)
int dScriptCompiler::CompileSource (const char* const source)
{
	dNewtonScriptLexical scanner (source);

	if (m_syntaxTree) {
		delete m_syntaxTree;
	}	
	
	m_syntaxTree = new dSyntaxTreeCode;

//	m_virtualMachine = virtualMachine;
	Parse(scanner);
	return 0;
}


bool dScriptCompiler::Parse(dNewtonScriptLexical& scanner)
{
//	m_codeSegmentSize = 0;
//	m_codeSegment = new short[D_CODE_SEGMNET_ALLOC_SHUNK_SIZE];


	dNewtonScriptParser::Parse(scanner);

//	scanner.ReStartScanner();
//	dAssemblerParser::Parse(scanner);

	return true;
}


void dScriptCompiler::SyntaxError (const dNewtonScriptLexical& scanner, const dUserVariable& errorToken, const dUserVariable& errorTokenMarker)
{
	const char* const data = scanner.GetData();
	int start = errorToken.m_scannerIndex;
	int lineNumber = errorToken.m_scannerLine + 1;
	while (data[start] && isspace (data[start])) {
		if (data[start] == '\n') {
			lineNumber ++;
		}
		start ++;
	}

	int end = errorTokenMarker.m_scannerIndex;
	while (data[end] && isspace (data[end])) {
		end --;
	}
	_ASSERTE (end >= start);

	int length = end - start + 1;
	string errorLine (&data[start], length);
//	fprintf (stderr, "%s (%d) : syntax error on line: %s\n", m_fileName, lineNumber, errorLine.c_str());
//	DTRACE (("%s (%d) : syntax error on line: %s\n", m_fileName, lineNumber, errorLine.c_str()));
	DisplayError ("%s (%d) : syntax error on line: %s\n", m_fileName, lineNumber, errorLine.c_str());
}



dScriptCompiler::dUserVariable dScriptCompiler::NewClassDefinition (const dUserVariable& visibility, const dUserVariable& name, const dUserVariable& deriveFrom)
{
	dUserVariable returnNode;
	dDAGClass* classNode = new dDAGClass (name.m_data.c_str(), (dDAGClass*) deriveFrom.m_node);

	dSyntaxTreeCode::dTreeNode* node = m_syntaxTree->Find(classNode->GetKey());
	if (node) {
		DisplayError ("error class \"%s\" : already defined", name.m_data.c_str());
		m_syntaxTree->Remove(node);
		node = m_syntaxTree->Insert(classNode, classNode->GetKey());
	} else {
		node = m_syntaxTree->Insert(classNode, classNode->GetKey());
	}

	m_currentClass = classNode;
	returnNode.m_node = classNode;
	return returnNode ;
}
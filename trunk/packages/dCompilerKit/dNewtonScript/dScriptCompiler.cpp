/* Copych1 (c) <2009> <Newton Game Dynamics>
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


//#define D_CODE_SEGMNET_ALLOC_SHUNK_SIZE (1024 * 32)

dScriptCompiler::dScriptCompiler(const char* const sourceFileName)
	:dNewtonScriptParser ()
	,m_virtualMachine (NULL)
	,m_fileName(sourceFileName)
//	,m_globalSymbols ()
//	,m_codeSegmentSize(0)
//	,m_codeSegment (NULL)
//	,m_currentFunction(NULL)
{
}

dScriptCompiler::~dScriptCompiler()
{
//	if (m_codeSegment) {
//		delete[] m_codeSegment;
//	}
}

int dScriptCompiler::CompileSource (dVirtualMachine* const virtualMachine, const char* const source)
{
	dNewtonScriptLexical scanner (source);

	m_virtualMachine = virtualMachine;
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
	fprintf (stderr, "%s (%d) : syntax error on line: %s\n", m_fileName, lineNumber, errorLine.c_str());
	DTRACE (("%s (%d) : syntax error on line: %s\n", m_fileName, lineNumber, errorLine.c_str()));
}
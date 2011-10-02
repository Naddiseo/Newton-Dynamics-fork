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
//Auto generated Parser Generator class: dScriptCompiler.h
//

#ifndef __dScriptCompiler_h__
#define __dScriptCompiler_h__

#include <dTree.h>
#include <dList.h>
#include <dCRC.h>
#include <dContainersStdAfx.h>
#include "dNewtonScriptParser.h"


//class dVirtualMachine;

class dScriptCompiler: public dNewtonScriptParser
{
	public:
	dScriptCompiler(const char* const sourceFileName);
	virtual ~dScriptCompiler();
	int CompileSource (const char* const source);

	protected:
	virtual bool Parse(dNewtonScriptLexical& scanner);

	void SyntaxError (const dNewtonScriptLexical& scanner, const dUserVariable& errorToken, const dUserVariable& errorTokenMarker);

	const char* m_fileName;
//	dVirtualMachine* m_virtualMachine;

	friend class dNewtonScriptParser;
};

#endif

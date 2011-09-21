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
//Auto generated Parser Generator class: dAssemblerCompiler.cpp
//


#include <dVirtualMachine.h>
#include "dAssemblerCompiler.h"

dAssemblerCompiler::dAssemblerCompiler(dVirtualMachine* const virtualMachine)
	:dAssemblerParser ()
	,m_virtualMachine (virtualMachine)
{
}

dAssemblerCompiler::~dAssemblerCompiler()
{
}

bool dAssemblerCompiler::Parse(dAssemblerLexical& scanner)
{
	return dAssemblerParser::Parse(scanner);
}


dAssemblerCompiler::dUserVariable dAssemblerCompiler::EmitDataType (const dUserVariable& dataType) const
{
	return dataType;
}

dAssemblerCompiler::dUserVariable dAssemblerCompiler::EmitSymbol (const dUserVariable& symbol) const
{
	return symbol;
}

dAssemblerCompiler::dUserVariable dAssemblerCompiler::EmitIntegerConst (const dUserVariable& integerConst) const
{
	return integerConst;
}


void dAssemblerCompiler::EmitUnInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id) const
{

}

void dAssemblerCompiler::EmitInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id, const dUserVariable& initialValue) const
{

}

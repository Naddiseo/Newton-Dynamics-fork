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
#include "dAssemblerLexical.h"
#include "dAssemblerCompiler.h"

#define D_CODE_SEGMNET_ALLOC_SHUNK_SIZE (1024 * 32)

dAssemblerCompiler::dAssemblerCompiler(dVirtualMachine* const virtualMachine)
	:dAssemblerParser ()
	,m_virtualMachine (virtualMachine)
	,m_currentPass(firstPass)
	,m_globalSymbols ()
	,m_codeSegmentSize(0)
	,m_codeSegment (NULL)
	,m_currentFuntion(NULL)
{
}

dAssemblerCompiler::~dAssemblerCompiler()
{
	if (m_codeSegment) {
		delete[] m_codeSegment;
	}
}

bool dAssemblerCompiler::Parse(dAssemblerLexical& scanner)
{
	m_codeSegmentSize = 0;
	m_codeSegment = new short[D_CODE_SEGMNET_ALLOC_SHUNK_SIZE];
	m_currentPass = firstPass;
	dAssemblerParser::Parse(scanner);

	scanner.ReStartScanner();
	m_currentPass = secundPass;
	dAssemblerParser::Parse(scanner);

	return true;
}

void dAssemblerCompiler::EmitByteCode (int count, const dVirtualMachine::dOpCode* const code)
{
	if ((m_codeSegmentSize + count) > D_CODE_SEGMNET_ALLOC_SHUNK_SIZE) {
		// realloc the code segment
		_ASSERTE (0);
	}

	for (int i = 0; i < count; i++) {
		m_codeSegment[m_codeSegmentSize] = code[i].m_bytecode;
		m_codeSegmentSize ++;
	}
}


dAssemblerCompiler::dUserVariable dAssemblerCompiler::EmitDataType (const dUserVariable& dataType) const
{
	return dataType;
}

dAssemblerCompiler::dUserVariable dAssemblerCompiler::EmitSymbol (const dUserVariable& symbol) const
{
	return symbol;
}



void dAssemblerCompiler::EmitUnInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id) const
{

}

void dAssemblerCompiler::EmitInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id, const dUserVariable& initialValue) const
{
}


void dAssemblerCompiler::RegisterFunction (const dUserVariable& name, const dUserVariable& functionScope)
{
	if (m_globalSymbols.Find(name.m_data)) {
		// WARNING FUNTION DUPLICATION;
		_ASSERTE (0);
	} 

	// add new symbol to the global symbol table; 
	dGlobalSymbol symbol;
	symbol.m_isPublic = (functionScope.m_data == "private") ? false : true;
	symbol.m_type = funtionName;
	symbol.m_symbolLocationInByte = m_codeSegmentSize;

	dSymbolTable::dTreeNode* const symbolNode = m_globalSymbols.Insert (symbol, name.m_data);
	m_currentFuntion = &symbolNode->GetInfo();
}

void dAssemblerCompiler::BeginFunctionLinking (const dUserVariable& name)
{
	dSymbolTable::dTreeNode* const symbolNode = m_globalSymbols.Find(name.m_data);
	_ASSERTE (symbolNode);
	m_currentFuntion = &symbolNode->GetInfo();
}

void dAssemblerCompiler::EmitJumpDestLabel (const dUserVariable& symbol) const
{
	_ASSERTE (m_currentFuntion);
	if (m_currentFuntion->m_localSymbols.Find(symbol.m_data)) {
		// ERROR local symbol duplication
	}

	dLocalSymbol localSymbol;
	localSymbol.m_symbolLocationInByte = m_codeSegmentSize;
	localSymbol.m_type = localJumpLabel;
	string label (symbol.m_data);
	label.replace(label.size()-1, 1, "");
	m_currentFuntion->m_localSymbols.Insert(localSymbol, label);
}

void dAssemblerCompiler::EmitADDIConstantExpresion (const dUserVariable& dstRegister, const dUserVariable& srcRegister, const dUserVariable& constValue)
{
	dVirtualMachine::dOpCode bytecode[3];
	
	bytecode[0].m_opcode = dVirtualMachine::addi;
	bytecode[0].m_reg0 = dstRegister.m_semanticValue;
	bytecode[0].m_reg1 = srcRegister.m_semanticValue;
	bytecode[1].m_bytecode = short (constValue.m_semanticValue & 0xffff);
	bytecode[2].m_bytecode = short (constValue.m_semanticValue >> 16);
	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}

dAssemblerCompiler::dUserVariable dAssemblerCompiler::TypeCheckRegister (const dUserVariable& symbol)
{
	dAssemblerCompiler::dUserVariable reg (symbol);
//	if (reg.m_registerNumber)
	_ASSERTE (0);

	reg.m_semanticValue = 1;
	return reg;
}

dAssemblerCompiler::dUserVariable dAssemblerCompiler::EmitIntegerConst (const dUserVariable& integerConst) const
{
	dAssemblerCompiler::dUserVariable constantValue (integerConst);
	constantValue.m_semanticValue = 111;
	return constantValue;
}

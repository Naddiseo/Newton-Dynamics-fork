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
	,m_globalSymbols ()
	,m_codeSegmentSize(0)
	,m_codeSegment (NULL)
	,m_currentFunction(NULL)
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
	dAssemblerParser::Parse(scanner);

//	scanner.ReStartScanner();
//	dAssemblerParser::Parse(scanner);

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


/*
void dAssemblerCompiler::EmitJumpDestLabel (const dUserVariable& symbol) const
{
	_ASSERTE (m_currentFunction);
	if (m_currentFunction->m_localSymbols.Find(symbol.m_data)) {
		// ERROR local symbol duplication
	}

	dLocalSymbol localSymbol;
	localSymbol.m_symbolLocationInByte = m_codeSegmentSize;
	localSymbol.m_type = localJumpLabel;
	string label (symbol.m_data);
	label.replace(label.size()-1, 1, "");
	m_currentFunction->m_localSymbols.Insert(localSymbol, label);
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


void dAssemblerCompiler::EmitCALL (const dUserVariable& regStack, const dUserVariable& symbol)
{
	dReference& reference = m_globalReferences.Append()->GetInfo();
	reference.m_location = m_codeSegmentSize + 1;
	reference.m_symbol = symbol.m_data;

	dVirtualMachine::dOpCode bytecode[3];
	bytecode[0].m_opcode = dVirtualMachine::call;
	bytecode[0].m_reg0 = regStack.m_semanticValue;
	bytecode[0].m_reg1 = 0;
	bytecode[1].m_bytecode = 0;
	bytecode[2].m_bytecode = 0;
	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}

void dAssemblerCompiler::EmitRET (const dUserVariable& regStack)
{
	dVirtualMachine::dOpCode bytecode[1];
	bytecode[0].m_opcode = dVirtualMachine::ret;
	bytecode[0].m_reg0 = regStack.m_semanticValue;
	bytecode[0].m_reg1 = 0;
	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}

void dAssemblerCompiler::EmitCompareAndJumpConstOffset (const dUserVariable& instruction, const dUserVariable& reg0, const dUserVariable& reg1, const dUserVariable& constOffset)
{
	dVirtualMachine::dOpCode bytecode[3];
	bytecode[0].m_opcode = instruction.m_semanticValue;
	bytecode[0].m_reg0 = reg0.m_semanticValue;
	bytecode[0].m_reg1 = reg1.m_semanticValue;
	bytecode[1].m_bytecode = constOffset.m_semanticValue & 0xffff;
	bytecode[2].m_bytecode = constOffset.m_semanticValue >> 16;
	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}

void dAssemblerCompiler::EmitCompareAndJumpLocalLabel (const dUserVariable& instruction, const dUserVariable& reg0, const dUserVariable& reg1, const dUserVariable& label)
{
	dReference& reference = m_globalReferences.Append()->GetInfo();
	reference.m_location = m_codeSegmentSize + 1;
	reference.m_symbol = label.m_data;

	dVirtualMachine::dOpCode bytecode[3];
	bytecode[0].m_opcode = instruction.m_semanticValue;
	bytecode[0].m_reg0 = reg0.m_semanticValue;
	bytecode[0].m_reg1 = reg1.m_semanticValue;
	bytecode[1].m_bytecode = 0;
	bytecode[2].m_bytecode = 0;
	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}


void dAssemblerCompiler::EmitArithmeticInstrution (const dUserVariable& instruction, const dUserVariable& dst, const dUserVariable& src)
{
	dVirtualMachine::dOpCode bytecode[1];
	bytecode[0].m_opcode = instruction.m_semanticValue;
	bytecode[0].m_reg0 = dst.m_semanticValue;
	bytecode[0].m_reg1 = src.m_semanticValue;
	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}

void dAssemblerCompiler::EmitPushPopRegisterList (const dUserVariable& instruction, const dUserVariable& stackReg, const dUserVariable& registerMask)
{
	// check that the register mask is no making reference to non existing registers;
	dVirtualMachine::dOpCode bytecode[1];
	bytecode[0].m_opcode = instruction.m_semanticValue;
	bytecode[0].m_reg0 = stackReg.m_semanticValue;
	bytecode[0].m_reg1 = 0;
	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}
*/

dAssemblerCompiler::dUserVariable dAssemblerCompiler::TypeCheckRegister (const dUserVariable& symbol)
{
	dAssemblerCompiler::dUserVariable reg (symbol);

	int registerIndex = atoi (symbol.m_data.c_str() + 1);
	if ((registerIndex < 0) || (registerIndex >= m_virtualMachine->GetRegisterCount())) {
		// error unknowns Register
		_ASSERTE (0);
	}

	reg.m_semanticValue = registerIndex;
	return reg;
}


void dAssemblerCompiler::EmitBeginFunction (const dUserVariable& name, const dUserVariable& functionScope)
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
	m_currentFunction = &symbolNode->GetInfo();
}


void dAssemblerCompiler::EmitEndFunction ()
{
	_ASSERTE (m_currentFunction);
	for (dList<dReference>::dListNode* node = m_currentFunction->m_localReferences.GetFirst(); node; node = node->GetNext()) {
		_ASSERTE (0);
	}
}

dAssemblerCompiler::dUserVariable dAssemblerCompiler::EmitIntegerConst (const dUserVariable& integerConst) const
{
	dAssemblerCompiler::dUserVariable constantValue (integerConst);
	constantValue.m_semanticValue = atoi (integerConst.m_data.c_str());
	return constantValue;
}


void dAssemblerCompiler::EmitInstrutionType1 (const dUserVariable& instruction, const dUserVariable& reg, const dUserVariable& immediate)
{
	dVirtualMachine::dOpCode bytecode[3];
	bytecode[0].m_opcode = instruction.m_semanticValue;
	bytecode[0].m_reg0 = reg.m_semanticValue;
	bytecode[0].m_reg1 = 0;
	bytecode[1].m_bytecode = immediate.m_semanticValue & 0xffff;
	bytecode[2].m_bytecode = immediate.m_semanticValue >> 16;
	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}

void dAssemblerCompiler::EmitInstrutionType2 (const dUserVariable& instruction, const dUserVariable& dst, const dUserVariable& src, const dUserVariable& immediate)
{

}
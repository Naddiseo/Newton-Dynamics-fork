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

dScriptCompiler::dScriptCompiler()
	:dNewtonScriptParser ()
//	,m_virtualMachine (virtualMachine)
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

/*
int dScriptCompiler::CompileSouce (const char* const source)
{
	dAssemblerLexical scanner (source);
	Parse(scanner);
	return 0;
}

bool dScriptCompiler::Parse(dAssemblerLexical& scanner)
{
	m_codeSegmentSize = 0;
	m_codeSegment = new short[D_CODE_SEGMNET_ALLOC_SHUNK_SIZE];
	dAssemblerParser::Parse(scanner);

//	scanner.ReStartScanner();
//	dAssemblerParser::Parse(scanner);

	return true;
}

void dScriptCompiler::EmitByteCode (int count, const dVirtualMachine::dOpCode* const code)
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


dScriptCompiler::dUserVariable dScriptCompiler::EmitDataType (const dUserVariable& dataType) const
{
	return dataType;
}

dScriptCompiler::dUserVariable dScriptCompiler::EmitSymbol (const dUserVariable& symbol) const
{
	return symbol;
}



void dScriptCompiler::EmitUnInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id) const
{

}

void dScriptCompiler::EmitInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id, const dUserVariable& initialValue) const
{

}



void dScriptCompiler::EmitLocalLabel (const dUserVariable& symbol) const
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



dScriptCompiler::dUserVariable dScriptCompiler::TypeCheckRegister (const dUserVariable& symbol)
{
	dScriptCompiler::dUserVariable reg (symbol);

	int registerIndex = atoi (symbol.m_data.c_str() + 1);
	if ((registerIndex < 0) || (registerIndex >= m_virtualMachine->GetRegisterCount())) {
		// error unknowns Register
		_ASSERTE (0);
	}

	reg.m_semanticValue = registerIndex;
	return reg;
}



dScriptCompiler::dUserVariable dScriptCompiler::EmitIntegerConst (const dUserVariable& integerConst) const
{
	dScriptCompiler::dUserVariable constantValue (integerConst);
	constantValue.m_semanticValue = atoi (integerConst.m_data.c_str());
	return constantValue;
}



void dScriptCompiler::EmitPushAndPop (const dUserVariable& instruction, const dUserVariable& registerMask)
{
	// check that the register mask is no making reference to non existing registers;
	int mask = registerMask.m_semanticValue;
	if (instruction.m_semanticValue == dVirtualMachine::push) {
		int reg = m_virtualMachine->GetRegisterCount() - 1;
		do {
			if (mask & (1<<31)) {
				dUserVariable register1;
				register1.m_semanticValue = reg; 
				EmitInstructionType2 (instruction, register1);
			}
			reg --;
			mask <<= 1;
		} while (mask);
	} else {
		int reg = 0;
		do {
			if (mask & 1) {
				dUserVariable register1;
				register1.m_semanticValue = reg; 
				EmitInstructionType2 (instruction, register1);
			}
			reg ++;
			mask >>= 1;
		} while (mask);
	}
}


void dScriptCompiler::EmitInstructionType1_saveLocalAdress (const dUserVariable& instruction, const dUserVariable& symbol)
{
	_ASSERTE (0);
}

void dScriptCompiler::EmitInstructionType4_saveLocalAdress (const dUserVariable& instruction, const dUserVariable& reg0, const dUserVariable& reg1, const dUserVariable& symbol)
{
	dReference& reference = m_currentFunction->m_localReferences.Append()->GetInfo();

	reference.m_location = m_codeSegmentSize + 1;
	reference.m_symbol = symbol.m_data;

	EmitInstructionType4 (instruction, reg0, reg1, symbol);
}

void dScriptCompiler::EmitInstructionType1_saveGlobalAdress (const dUserVariable& instruction, const dUserVariable& symbol)
{
	dReference& reference = m_globalReferences.Append()->GetInfo();
	reference.m_location = m_codeSegmentSize + 1;
	reference.m_symbol = symbol.m_data;

	EmitInstructionType1 (instruction, symbol);
}

void dScriptCompiler::EmitInstructionType0 (const dUserVariable& instruction)
{
	dVirtualMachine::dOpCode bytecode[1];

	bytecode[0].m_opcode = instruction.m_semanticValue;
	bytecode[0].m_reg0 = 0;
	bytecode[0].m_reg1 = 0;
	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}


void dScriptCompiler::EmitInstructionType1 (const dUserVariable& instruction, const dUserVariable& immediate)
{
	dVirtualMachine::dOpCode bytecode[3];

	bytecode[0].m_opcode = instruction.m_semanticValue;
	bytecode[0].m_reg0 = 0;
	bytecode[0].m_reg1 = 0;

	bytecode[1].m_bytecode = immediate.m_semanticValue & 0xffff;
	bytecode[2].m_bytecode = immediate.m_semanticValue >> 16;

	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}


void dScriptCompiler::EmitInstructionType2 (const dUserVariable& instruction, const dUserVariable& reg)
{
	dVirtualMachine::dOpCode bytecode[1];
	bytecode[0].m_opcode = instruction.m_semanticValue;
	bytecode[0].m_reg0 = reg.m_semanticValue;
	bytecode[0].m_reg1 = 0;

	if (reg.m_semanticValue >= m_virtualMachine->GetRegisterCount()) {
		// error using illegal register
	}

	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}

void dScriptCompiler::EmitInstructionType3 (const dUserVariable& instruction, const dUserVariable& dst, const dUserVariable& src)
{
	dVirtualMachine::dOpCode bytecode[1];
	bytecode[0].m_opcode = instruction.m_semanticValue;
	bytecode[0].m_reg0 = dst.m_semanticValue;
	bytecode[0].m_reg1 = src.m_semanticValue;

	if (dst.m_semanticValue >= m_virtualMachine->GetRegisterCount()) {
		// error using illegal register
	}
	if (src.m_semanticValue >= m_virtualMachine->GetRegisterCount()) {
		// error using illegal register
	}

	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}

void dScriptCompiler::EmitInstructionType4 (const dUserVariable& instruction, const dUserVariable& dst, const dUserVariable& src, const dUserVariable& immediate)
{
	dVirtualMachine::dOpCode bytecode[3];
	bytecode[0].m_opcode = instruction.m_semanticValue;
	bytecode[0].m_reg0 = dst.m_semanticValue;
	bytecode[0].m_reg1 = src.m_semanticValue;

	if (dst.m_semanticValue >= m_virtualMachine->GetRegisterCount()) {
		// error using illegal register
	}
	if (src.m_semanticValue >= m_virtualMachine->GetRegisterCount()) {
		// error using illegal register
	}

	bytecode[1].m_bytecode = immediate.m_semanticValue & 0xffff;
	bytecode[2].m_bytecode = immediate.m_semanticValue >> 16;
	EmitByteCode (sizeof (bytecode) / sizeof (bytecode[0]), bytecode);
}


void dScriptCompiler::EmitBeginFunction (const dUserVariable& name, const dUserVariable& functionScope)
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


void dScriptCompiler::EmitEndFunction ()
{
	_ASSERTE (m_currentFunction);
	for (dList<dReference>::dListNode* node = m_currentFunction->m_localReferences.GetFirst(); node; node = node->GetNext()) {
		_ASSERTE (0);
	}
}
*/
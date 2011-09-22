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
//Auto generated Parser Generator class: dAssemblerCompiler.h
//

#ifndef __dAssemblerCompiler_h__
#define __dAssemblerCompiler_h__

#include "dAssemblerParser.h"

class dVirtualMachine;

class dAssemblerCompiler: public dAssemblerParser
{
	public:
	enum dCompilerPasses
	{
		firstPass,
		secundPass
	};

	enum dSymbolType
	{
		funtionName,
		localJumpLabel,
	};

	class dSymbol
	{
		public:
		dSymbol ()
			:m_type(funtionName), m_symbolLocationInByte(0)
		{
		}

		dSymbolType m_type;
		int m_symbolLocationInByte;
	};

	class dLocalSymbol: public dSymbol
	{
		public:
	};

	class dGlobalSymbol: public dSymbol
	{
		public:
		dGlobalSymbol ()
			:dSymbol (), m_isPublic(true)
		{
		}

		bool m_isPublic;
		dTree <dLocalSymbol, string> m_localSymbols;
	};

	class dSymbolTable: public dTree <dGlobalSymbol, string>
	{
	};

	dAssemblerCompiler(dVirtualMachine* const m_virtualMachine);
	virtual ~dAssemblerCompiler();
	virtual bool Parse(dAssemblerLexical& scanner);

	void EmitByteCode (int count, const dVirtualMachine::dOpCode* const code);

	// first past, emit code and create global and local symbol table
	void RegisterFunction (const dUserVariable& name, const dUserVariable& functionScope);
	void EmitJumpDestLabel (const dUserVariable& symbol) const;
	void EmitADDIConstantExpresion (const dUserVariable& dstRegister, const dUserVariable& srcRegister, const dUserVariable& constValue);
	
	// second pass is linker linker 
	void BeginFunctionLinking (const dUserVariable& name);



	dUserVariable TypeCheckRegister (const dUserVariable& symbol);
	
	dUserVariable EmitSymbol (const dUserVariable& symbol) const;
	dUserVariable EmitDataType (const dUserVariable& dataType) const;
	dUserVariable EmitIntegerConst (const dUserVariable& symbol) const;
	void EmitUnInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id) const;
	void EmitInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id, const dUserVariable& initialValue) const;
	
	dVirtualMachine* m_virtualMachine;

	dCompilerPasses m_currentPass;

	dSymbolTable m_globalSymbols;

	int m_codeSegmentSize;
	short* m_codeSegment;

	dGlobalSymbol* m_currentFuntion;

};

#endif

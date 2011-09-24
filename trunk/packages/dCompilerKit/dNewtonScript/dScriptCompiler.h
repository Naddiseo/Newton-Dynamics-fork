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
#include <dVirtualMachine.h>
#include "dNewtonScriptParser.h"

#ifdef _MSC_VER
#include <windows.h>
#include <crtdbg.h>
#endif


class dScriptCompiler: public dNewtonScriptParser
{
	public:
/*
	enum dSymbolType
	{
		funtionName,
		localJumpLabel,
	};

	class dReference
	{
		public:
		int m_location;
		string m_symbol;
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
		dList<dReference> m_localReferences;
		dTree <dLocalSymbol, string> m_localSymbols;
	};

	class dSymbolTable: public dTree <dGlobalSymbol, string>
	{
	};


	int CompileSouce (const char* const source);

	protected:
	virtual bool Parse(dAssemblerLexical& scanner);
	void EmitByteCode (int count, const dVirtualMachine::dOpCode* const code);

	void EmitBeginFunction (const dUserVariable& name, const dUserVariable& functionScope);
	void EmitEndFunction ();



	void EmitInstructionType0 (const dUserVariable& instruction);
	void EmitInstructionType1 (const dUserVariable& instruction, const dUserVariable& immediate);
	void EmitInstructionType2 (const dUserVariable& instruction, const dUserVariable& reg);
	void EmitInstructionType3 (const dUserVariable& instruction, const dUserVariable& dst, const dUserVariable& src);
	void EmitInstructionType4 (const dUserVariable& instruction, const dUserVariable& dst, const dUserVariable& src, const dUserVariable& immediate);

	void EmitInstructionType1_saveGlobalAdress (const dUserVariable& instruction, const dUserVariable& symbol);

	void EmitInstructionType1_saveLocalAdress (const dUserVariable& instruction, const dUserVariable& symbol);
	void EmitInstructionType4_saveLocalAdress (const dUserVariable& instruction, const dUserVariable& reg0, const dUserVariable& reg1, const dUserVariable& symbol);


	void EmitLocalLabel (const dUserVariable& symbol) const;
	dUserVariable TypeCheckRegister (const dUserVariable& symbol);
	dUserVariable EmitSymbol (const dUserVariable& symbol) const;
	dUserVariable EmitDataType (const dUserVariable& dataType) const;
	dUserVariable EmitIntegerConst (const dUserVariable& symbol) const;
	void EmitUnInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id) const;
	void EmitInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id, const dUserVariable& initialValue) const;


	// pseudo instructions
	void EmitPushAndPop (const dUserVariable& instruction, const dUserVariable& regMask);



	
	dVirtualMachine* m_virtualMachine;


	dSymbolTable m_globalSymbols;
	dList<dReference> m_globalReferences;

	int m_codeSegmentSize;
	short* m_codeSegment;

	dGlobalSymbol* m_currentFunction;

	friend dAssemblerParser;
*/

	dScriptCompiler();
	virtual ~dScriptCompiler();

//	dVirtualMachine* const m_virtualMachine

};

#endif

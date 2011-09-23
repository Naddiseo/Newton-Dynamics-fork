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

	dAssemblerCompiler(dVirtualMachine* const m_virtualMachine);
	virtual ~dAssemblerCompiler();
	virtual bool Parse(dAssemblerLexical& scanner);

	void EmitByteCode (int count, const dVirtualMachine::dOpCode* const code);

	void EmitBeginFunction (const dUserVariable& name, const dUserVariable& functionScope);
	void EmitEndFunction ();



	void EmitInstrutionType0 (const dUserVariable& instruction, const dUserVariable& dst, const dUserVariable& src);
	void EmitInstrutionType1 (const dUserVariable& instruction, const dUserVariable& reg, const dUserVariable& immediate);
	void EmitInstrutionType2 (const dUserVariable& instruction, const dUserVariable& dst, const dUserVariable& src, const dUserVariable& immediate);
//	void EmitInstrutionType0 (const dUserVariable& instruction, const dUserVariable& dst, const dUserVariable& src) const;

//	void EmitJumpDestLabel (const dUserVariable& symbol) const;
//	void EmitADDIConstantExpresion (const dUserVariable& dstRegister, const dUserVariable& srcRegister, const dUserVariable& constValue);
//	void EmitArithmeticInstrution (const dUserVariable& instruction, const dUserVariable& dst, const dUserVariable& src);
//	void EmitCompareAndJumpLocalLabel (const dUserVariable& instruction, const dUserVariable& reg0, const dUserVariable& reg1, const dUserVariable& label);
//	void EmitCompareAndJumpConstOffset (const dUserVariable& instruction, const dUserVariable& reg0, const dUserVariable& reg1, const dUserVariable& offset);
//	void EmitCALL (const dUserVariable& regStack, const dUserVariable& symbol);
//	void EmitRET (const dUserVariable& regStack);
//	void EmitPushPopRegisterList (const dUserVariable& instruction, const dUserVariable& stackReg, const dUserVariable& registerMask);


	dUserVariable TypeCheckRegister (const dUserVariable& symbol);
	
	dUserVariable EmitSymbol (const dUserVariable& symbol) const;
	dUserVariable EmitDataType (const dUserVariable& dataType) const;
	dUserVariable EmitIntegerConst (const dUserVariable& symbol) const;


	void EmitUnInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id) const;
	void EmitInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id, const dUserVariable& initialValue) const;
	
	dVirtualMachine* m_virtualMachine;


	dSymbolTable m_globalSymbols;
	dList<dReference> m_globalReferences;

	int m_codeSegmentSize;
	short* m_codeSegment;

	dGlobalSymbol* m_currentFunction;

};

#endif

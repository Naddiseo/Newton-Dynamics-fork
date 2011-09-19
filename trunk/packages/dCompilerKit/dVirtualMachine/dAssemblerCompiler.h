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
	dAssemblerCompiler(dVirtualMachine* const m_virtualMachine);
	virtual ~dAssemblerCompiler();
	virtual bool Parce(dAssemblerLexical& scanner);

	
	dUserVariable EmitSymbol (const dUserVariable& symbol) const;
	dUserVariable EmitDataType (const dUserVariable& dataType) const;
	dUserVariable EmitIntegerConst (const dUserVariable& symbol) const;

	void EmitUnInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id) const;
	void EmitInitilizedDataDeclaration (const dUserVariable& type, const dUserVariable& id, const dUserVariable& initialValue) const;
	
	dVirtualMachine* m_virtualMachine;
};

#endif

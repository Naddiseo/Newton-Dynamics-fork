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
//Auto generated Parcer Generator class: dAssemblerCompiler.h
//

#ifndef __dAssemblerCompiler_h__
#define __dAssemblerCompiler_h__

#include "dAssemblerParcer.h"

class dVirtualMachine;

class dAssemblerCompiler: public dAssemblerParcer
{
	public:
	dAssemblerCompiler(dVirtualMachine* const m_virtualMachine);
	virtual ~dAssemblerCompiler();
	virtual int Parce(dAssemblerLexical& scanner);


	dVirtualMachine* m_virtualMachine;
};

#endif

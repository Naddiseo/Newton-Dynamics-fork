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
//Auto generated Parcer Generator class: dAssemblerParcer.cpp
//


#include <dVirtualMachine.h>

#include "dAssemblerParcer.h"

#define dStackSize 1024

dAssemblerParcer::dAssemblerParcer()
{
}

dAssemblerParcer::~dAssemblerParcer()
{
}

int dAssemblerParcer::Parce(dAssemblerLexical& scanner)
{
	for (Token token = Token (scanner.NextToken()); token != -1; token = Token (scanner.NextToken())) {

	}

	return 0;
}


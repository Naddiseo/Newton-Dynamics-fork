#include "dVirtualMachine.h"
#include "dAssemblerParcer.h"
#include "dAssemblerLexical.h"


dVirtualMachine::dVirtualMachine(void)
{
}

dVirtualMachine::~dVirtualMachine(void)
{
}


bool dVirtualMachine::CompileAssembly (const char* const code)
{
	dAssemblerParcer parcer;
	dAssemblerLexical scanner (code);

	parcer.Parce(scanner);

	return true;
}
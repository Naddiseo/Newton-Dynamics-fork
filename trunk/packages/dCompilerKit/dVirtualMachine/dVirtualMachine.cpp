#include "dVirtualMachine.h"
#include "dAssemblerParcer.h"
#include "dAssemblerLexical.h"
#include "dAssemblerCompiler.h"

dVirtualMachine::dVirtualMachine(void)
{
}

dVirtualMachine::~dVirtualMachine(void)
{
}


bool dVirtualMachine::CompileAssembly (const char* const code)
{
	dAssemblerCompiler compiler(this);
	dAssemblerLexical scanner (code);

	compiler.Parce(scanner);

	return true;
}
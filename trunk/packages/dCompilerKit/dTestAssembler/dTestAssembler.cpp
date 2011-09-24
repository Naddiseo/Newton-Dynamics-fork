// dTestAssembler.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tchar.h>
#include <dVirtualMachine.h>
#include <dAssemblerCompiler.h>

#ifdef _MSC_VER
#include <windows.h>
#include <crtdbg.h>
#endif


int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _MSC_VER
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	if (argc < 2) {
		fprintf (stdout, "usage: dTestAssembly filename\n");
		exit (0);
	}


	const char* const source = argv[1];

	FILE* const rules = fopen (source, "r");
	if (!rules) {
		fprintf (stdout, "Rule file \"%s\" not found\n", source);
		exit (0);
	}

	fseek (rules, 0, SEEK_END);
	int size = ftell (rules);
	fseek (rules, 0, SEEK_SET);

	char* const buffer = new char [size + 1];
	memset (buffer, 0, size + 1);
	fread (buffer, 1, size, rules);
	fclose (rules);

	dVirtualMachine virtualMachine;
	dAssemblerCompiler compiler (&virtualMachine);
	compiler.CompileSouce (buffer);

	delete buffer;
	return 0;
}


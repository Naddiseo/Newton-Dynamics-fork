// dParcerGenerator.cpp : Defines the entry point for the console application.
//

#include "dParcelCompiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tchar.h>
#include <crtdbg.h>


int main(int argc, char* argv[])
{
	#ifdef _MSC_VER
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	if (argc < 3) {
		fprintf (stdout, "usage: dParcelGenerator [GrammarFile] [OutputFileName]\n");
		fprintf (stdout, "[GrammarFile] name of the file containing a TYacc like Grammar file\n");
		fprintf (stdout, "[OutputFileName] name of the file cpp output file\n");
		exit (0);
	}

	const char* const inputRulesFileName = argv[1];
	const char* const outputFileName = argv[2];

	FILE* const rules = fopen (inputRulesFileName, "r");
	if (!rules) {
		fprintf (stdout, "Rule file \"%s\" not found\n",  inputRulesFileName);
		exit (0);
	}

	fseek (rules, 0, SEEK_END);
	int size = ftell (rules);
	fseek (rules, 0, SEEK_SET);

	char* const buffer = new char [size + 1];
	memset (buffer, 0, size + 1);
	fread (buffer, 1, size, rules);
	fclose (rules);

	dParcelCompiler parcel (buffer, outputFileName);

	delete buffer;

	return 0;
}


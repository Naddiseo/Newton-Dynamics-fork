/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/


#include "dLexCompiler.h"


int main(int argc, char* argv[])
{
	if (argc < 3) {
		fprintf (stdout, "usage: dLexCompiler [inputRulesFileName] [outputFileName]\n");
		fprintf (stdout, "[inputRulesFileName] name of the file containing a Lex or Flex rules\n");
		fprintf (stdout, "[outputFileName] name of the file cpp output file\n");
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

	dLexCompiler lexical (buffer, outputFileName);

	delete buffer;
	return 0;
}


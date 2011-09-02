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

#ifndef __dLexCompiler_h__
#define __dLexCompiler_h__


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tchar.h>
#include <crtdbg.h>
#include <direct.h>

#include <dTree.h>
#include <dList.h>
#include <dCRC.h>

#include <string>
using namespace std;


#include "dAutomataState.h"
#include "dChatertSetMap.h"
#include "dDeterministicFiniteAutonataCompiler.h"
#include "dNonDeterministicFiniteAutonataCompiler.h"


class dLexCompiler
{
	enum dToken;
	class dTokenData;
	class dExpandedNFA;
	class dConvertDFAtoCode;

	class dTokenDataList: public dList<dTokenData*>
	{
		public:
		dTokenDataList ();
		~dTokenDataList ();
		void DeleteAll();
		void AddTokenData (dToken token, const char* const regulatExpresion);
	};


	class dDefinitionsMap: public dTree<string,unsigned>
	{
		public:
		dDefinitionsMap ();
		~dDefinitionsMap ();
		void PreProcessDefinitions (string& regularExpresionWithDefinitions);
		void AddDefinition (string& regularExpresionWithMacros, string& key);
	};


	public:
	dLexCompiler(const char* const inputRules, const char* const outputFileName);
	~dLexCompiler();

	private:
	void NextToken ();
	void MatchToken (dToken token);
	void ParseDefinitionExpression (string& preheaderCode);
	void ParseDefinitionBlock (string& preheaderCode);


	void ParseDefinitions (string& userPreheaderCode, string& automataCode);

	int ParseDefinitions (string& userPreheaderCode, string& nextCodeCases, string& automataCode, dChatertSetMap& characterSet, 
						   dTree<int, int>& transitionsCountMap, dTree<dTree<int, int>, int>& nextState,
						   dTree<dTree<char, int>, int>& characterTestMap, dTree<dTree<int, int>, int>& testSetArrayIndexMap);

	
	void CreateHeaderFile (const char* const fileName, const char* const className) const;
	void CreateCodeFile (const char* const fileName, const char* const className, int stateCount,
						 string& userPreheaderCode, string& nextCodeCases, string& automataCode, dChatertSetMap& characterSet,
						 dTree<int, int>& transitionsCountMap, dTree<dTree<int, int>, int>& nextState,
						 dTree<dTree<char, int>, int>& characterTestMap, dTree<dTree<int, int>, int>& testSetArrayIndexMap) const;


	dToken m_token;
	int m_grammarTokenStart;
	int m_grammarTokenLength;
	const char* m_grammar;
	dTokenDataList m_tokenList;
	dDefinitionsMap m_defintions;


};


#endif

// TODO: reference additional headers your program requires here

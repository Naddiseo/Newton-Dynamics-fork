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

#ifndef __dParcerCompiler_h__
#define __dParcerCompiler_h__


#include <dTree.h>
#include <dList.h>
#include <dCRC.h>
#include <dCompilerCore.h>

#include <string>
using namespace std;

class dGrammarLexical;

class dParcerCompiler
{
	public:
	enum Token
	{
		OR = 256,
		COLOM,
		SIMICOLOM,
		TOKEN,
		UNION,
		LEFT,
		RIGHT,
		START,
		LITERAL,
		CODE_BLOCK,
		USER_ACTION,
		GRAMMAR_SEGEMENT,
	};

	enum TokenType;
	enum ActionType;
	
	class dItem;
	class dState;
	class dGoto;
	class dAction;
	class dSymbol;
	class dTransition;
	class dSentenceSymbol;
	class dRuleInfo;
	class dProductionRule;

	class dActionEntry;

	
	dParcerCompiler(const char* const inputRules, const char* const outputFileName, const char* const scannerClassName);
	~dParcerCompiler();

	protected:
	void ScanGrammarFile(const char* const inputRules, dProductionRule& rules, dTree<TokenType, string>& symbolList, dTree<int, string>& terminalTokens, string& userCodeBlock);
	Token ScanGrammarRule(dGrammarLexical& lexical, dProductionRule& rules, dTree<TokenType, string>& symbolList, int& ruleNumber, dTree<int, string>& tokenEnumarationMap, int& tokenEnumeration);

	
	void CanonicalItemSets (dTree<dState*,int>& states, dProductionRule& rules, dTree<TokenType, string>& symbolList);
	dState* Goto (dProductionRule& rulesList, dState* const state, const string& symbol);
	dState* Closure (dProductionRule& rulesList, dList<dItem>& itemSet);

	void BuildParcingTable (dTree<dState*,int>& stateList, dTree<TokenType, string>& symbolList);

	void GenerateHeaderFile (const char* const className, const char* const scannerClassName, const char* const outputFileName, dProductionRule& rules, dTree<int, string>& tokenEnumerationMap);
	void GenerateParcerCode (const char* const className, const char* const scannerClassName, const char* const outputFileName, const string& userCode, 
							 const string& userVariable, const string& userVariableClass, 
							 dTree<dState*,int>& stateList, dTree<TokenType, string>& symbolList);

};


#endif

// TODO: reference additional headers your program requires here
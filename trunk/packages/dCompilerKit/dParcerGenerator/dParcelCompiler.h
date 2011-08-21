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

#ifndef __dParcelCompiler_h__
#define __dParcelCompiler_h__


#include <dTree.h>
#include <dList.h>
#include <dCRC.h>
#include <dCompilerCore.h>

#include <string>
using namespace std;

class dGrammarLexical;

class dParcelCompiler
{
	public:
	enum Token
	{
		OR = 256,
		COLOM,
		SIMICOLOM,
		TOKEN,
		LEFT,
		RIGHT,
		START,
		LITERAL,
		CODE_BLOCK,
		USER_ACTION,
		GRAMMAR_SEGEMENT,
	};

	class dItem;
	class dState;
	class dSymbol;
	class dSentenceSymbol;
	class dRuleInfo;
	class dProductionRule;

	
	

	dParcelCompiler(const char* const inputRules, const char* const outputFileName);
	~dParcelCompiler();

	protected:
	void ScanGrammarFile(const char* const inputRules, dProductionRule& rules, dTree<void*, string>& symbolList);
	Token ScanGrammarRule(dGrammarLexical& lexical, dProductionRule& rules, dTree<void*, string>& symbolList);

	
	void GenerateDFAStates (dList<dState*>& states, dProductionRule& rules, dTree<void*, string>& symbolList);
	dState* Goto (dProductionRule& rulesList, dState* const state, const string& symbol);
	dState* Closure (dProductionRule& rulesList, dList<dItem>& itemSet);
};


#endif

// TODO: reference additional headers your program requires here

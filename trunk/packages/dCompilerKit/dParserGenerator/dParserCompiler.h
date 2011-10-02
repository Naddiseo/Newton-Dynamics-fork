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

#ifndef __dParserCompiler_h__
#define __dParserCompiler_h__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tchar.h>
#include <crtdbg.h>

#include <dTree.h>
#include <dList.h>
#include <dCRC.h>
#include <dContainersStdAfx.h>




#include <string>
using namespace std;

class dParserLexical;

class dParserCompiler
{
	public:
	enum dToken
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
		SEMANTIC_ACTION,
		GRAMMAR_SEGMENT,
		PARCEL_ERROR,
	};

	enum dTokenType;
	enum ActionType;
	
	class dItem;
	class dState;
	class dGoto;
	class dAction;
	class dSymbol;
	class dRuleInfo;
	class dTokenInfo;
	class dGotoEntry;
	class ActionEntry;
	class dTransition;
	class dActionEntry;
	class dSentenceSymbol;
	class dProductionRule;
	class dTokenStringPair;
	class dOperatorsPrecedence;
	class dOperatorsAssociation;

	
	dParserCompiler(const string& inputRules, const char* const outputFileName, const char* const scannerClassName);
	~dParserCompiler();

	protected:
	string GetClassName(const char* const fileName) const;
	void LoadTemplateFile(const char* const fileName, string& output) const;
	void SaveFile(const char* const fileName, const char* const extention, const string& input) const;

	void ReplaceMacro (string& data, const string& newName, const string& macro) const;
	void ReplaceAllMacros (string& data, const string& newName, const string& macro) const;

	void ScanGrammarFile(const string& inputRules, dProductionRule& rules, dTree<dTokenInfo, dCRCTYPE>& symbolList, dOperatorsPrecedence& operatorPrecence,
						 string& userCodeBlock, string& userVariableClass, string& endUserCode, int& lastTokenEnum);
	dToken ScanGrammarRule(dParserLexical& lexical, dProductionRule& rules, dTree<dTokenInfo, dCRCTYPE>& symbolList, int& ruleNumber, int& tokenEnumeration);

	
	bool DoesSymbolDeriveEmpty (dCRCTYPE symbol, const dTree<dList<void*>, dCRCTYPE>& ruleMap) const ;
	void First (dCRCTYPE symbol, dTree<int, dCRCTYPE>& symbolListMark, const dTree<dTokenInfo, dCRCTYPE>& symbolList, const dTree<dList<void*>, dCRCTYPE>& ruleMap, dTree<int, dCRCTYPE>& firstSetOut) const;
	void First (const dList<dCRCTYPE>& symbolSet, const dTree<dTokenInfo, dCRCTYPE>& symbolList, const dTree<dList<void*>, dCRCTYPE>& ruleMap, dTree<int, dCRCTYPE>& firstSetOut) const;
	dState* Goto (const dState* const state, dCRCTYPE symbol, const dTree<dTokenInfo, dCRCTYPE>& symbolList, const dTree<dList<void*>, dCRCTYPE>& ruleMap) const;
	dState* Closure (const dList<dItem>& itemSet, const dTree<dTokenInfo, dCRCTYPE>& symbolList, const dTree<dList<void*>, dCRCTYPE>& ruleMap) const;
	void BuildParsingTable (const dTree<dState*, dCRCTYPE>& stateList, dCRCTYPE startSymbol, const dOperatorsPrecedence& operatorPrecence) const;
	void CanonicalItemSets (dTree<dState*, dCRCTYPE>& states, const dProductionRule& rules, const dTree<dTokenInfo, dCRCTYPE>& symbolList, const dOperatorsPrecedence& operatorPrecence, FILE* const debugFile);

	void GenerateHeaderFile (const string& className, const string& scannerClassName, const char* const outputFileName, 
							 const dTree<dTokenInfo, dCRCTYPE>& symbolList, const string& userVariableClass); 
	void GenerateParserCode (const string& className, const string& scannerClassName, const char* const outputFileName, 
							 const dTree<dTokenInfo, dCRCTYPE>& symbolList, dTree<dState*, dCRCTYPE>& stateList, const string& userCode, string& endUserCode, int lastTerminalTokenEnum);

};


#endif

// TODO: reference additional headers your program requires here

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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tchar.h>
#include <crtdbg.h>


#include <dTree.h>
#include <dList.h>
#include <dCRC.h>


#ifndef DTRACE
	#ifdef _DEBUG
		inline void dExpandTraceMessage (const char *fmt, ...)
		{
			va_list v_args;
			char* const text = (char*) malloc (strlen (fmt) + 2048);

			text[0] = 0;
			va_start (v_args, fmt);     
			vsprintf(text, fmt, v_args);
			va_end (v_args);            

			OutputDebugStringA (text);

			free (text);
		}

		#define DTRACE(x)										\
		{														\
			dExpandTraceMessage x;								\
		}
	#else
		#define DTRACE(x)
	#endif
#endif


#include <string>
using namespace std;

class dParcerLexical;

class dParcerCompiler
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
	class dGotoEntry;
	class ActionEntry;
	class dTransition;
	class dActionEntry;
	class dSentenceSymbol;
	class dProductionRule;
	class dTokenStringPair;
	class dOperatorsPrecedence;
	class dOperatorsAssociation;

	
	dParcerCompiler(const string& inputRules, const char* const outputFileName, const char* const scannerClassName);
	~dParcerCompiler();

	protected:
	string GetClassName(const char* const fileName) const;
	void LoadTemplateFile(const char* const fileName, string& output) const;
	void SaveFile(const char* const fileName, const char* const extention, const string& input) const;

	void ReplaceMacro (string& data, const string& newName, const string& macro) const;
	void ReplaceAllMacros (string& data, const string& newName, const string& macro) const;

	void ScanGrammarFile(const string& inputRules, dProductionRule& rules, dTree<dTokenType, string>& symbolList, dOperatorsPrecedence& operatorPrecence,
						 dTree<int, string>& terminaldTokens, string& userCodeBlock, string& userVariableClass, string& endUserCode, int& lastTokenEnum);
	dToken ScanGrammarRule(dParcerLexical& lexical, dProductionRule& rules, dTree<dTokenType, string>& symbolList, int& ruleNumber, dTree<int, string>& tokenEnumarationMap, int& tokenEnumeration);

	
	bool DoesSymbolDeriveEmpty (const string& symbol, const dProductionRule& ruleList) const ;
	void First (const string& symbol, const dTree<dTokenType, string>& symbolList, const dProductionRule& ruleList, dTree<int, string>& firstSetOut) const;
	void First (const dList<string>& symbolSet, const dTree<dTokenType, string>& symbolList, const dProductionRule& ruleList, dTree<int, string>& firstSetOut) const;
	dState* Goto (const dProductionRule& ruleList, const dState* const state, const string& symbol, const dTree<dTokenType, string>& symbolList) const;
	dState* Closure (const dProductionRule& ruleList, const dList<dItem>& itemSet, const dTree<dTokenType, string>& symbolList) const;
	void BuildParcingTable (const dTree<dState*,int>& stateList, const dTree<dTokenType, string>& symbolList, const string& startSymbol, const dOperatorsPrecedence& operatorPrecence) const;
	void CanonicalItemSets (dTree<dState*,int>& states, const dProductionRule& rules, const dTree<dTokenType, string>& symbolList);

	void GenerateHeaderFile (const string& className, const string& scannerClassName, const char* const outputFileName, dProductionRule& rules, dTree<int, string>& tokenEnumerationMap); 
	void GenerateParcerCode (const string& className, const string& scannerClassName, const char* const outputFileName, const string& userCode, 
							 const string& userVariableClass, dTree<dState*,int>& stateList, dTree<dTokenType, string>& symbolList, dTree<int, string>& tokenEnumerationMap,
							 string& endUserCode, int lastTokenEnum);

};


#endif

// TODO: reference additional headers your program requires here

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

	void ScanGrammarFile(const string& inputRules, dProductionRule& rules, dTree<dTokenInfo, unsigned>& symbolList, dOperatorsPrecedence& operatorPrecence,
						 string& userCodeBlock, string& userVariableClass, string& endUserCode, int& lastTokenEnum);
	dToken ScanGrammarRule(dParserLexical& lexical, dProductionRule& rules, dTree<dTokenInfo, unsigned>& symbolList, int& ruleNumber, int& tokenEnumeration);

	
	bool DoesSymbolDeriveEmpty (unsigned symbol, const dTree<dList<void*>, unsigned>& ruleMap) const ;
	void First (unsigned symbol, dTree<int, unsigned>& symbolListMark, const dTree<dTokenInfo, unsigned>& symbolList, const dTree<dList<void*>, unsigned>& ruleMap, dTree<int, unsigned>& firstSetOut) const;
	void First (const dList<unsigned>& symbolSet, const dTree<dTokenInfo, unsigned>& symbolList, const dTree<dList<void*>, unsigned>& ruleMap, dTree<int, unsigned>& firstSetOut) const;
	dState* Goto (const dState* const state, unsigned symbol, const dTree<dTokenInfo, unsigned>& symbolList, const dTree<dList<void*>, unsigned>& ruleMap) const;
	dState* Closure (const dList<dItem>& itemSet, const dTree<dTokenInfo, unsigned>& symbolList, const dTree<dList<void*>, unsigned>& ruleMap) const;
	void BuildParcingTable (const dTree<dState*,int>& stateList, unsigned startSymbol, const dOperatorsPrecedence& operatorPrecence) const;
	void CanonicalItemSets (dTree<dState*,int>& states, const dProductionRule& rules, const dTree<dTokenInfo, unsigned>& symbolList, const dOperatorsPrecedence& operatorPrecence, FILE* const debugFile);

	void GenerateHeaderFile (const string& className, const string& scannerClassName, const char* const outputFileName, 
							 const dTree<dTokenInfo, unsigned>& symbolList, const string& userVariableClass); 
	void GenerateParserCode (const string& className, const string& scannerClassName, const char* const outputFileName, 
							 const dTree<dTokenInfo, unsigned>& symbolList, dTree<dState*,int>& stateList, const string& userCode, string& endUserCode, int lastTerminalTokenEnum);

};


#endif

// TODO: reference additional headers your program requires here

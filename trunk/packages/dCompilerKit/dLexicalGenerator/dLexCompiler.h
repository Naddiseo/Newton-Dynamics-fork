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


#include "dAutomataState.h"
#include "dChatertSetMap.h"
#include "dDeterministicFiniteAutonataCompiler.h"
#include "dNonDeterministicFiniteAutonataCompiler.h"


class dLexCompiler
{
	enum Token
	{
		m_whiteSpace,
		m_action,
		m_comment,
		m_delimiter,
		m_codeBlock,
		m_intenalSize,
		m_number,
		m_quatedString,
		m_literal,
		m_extendedRegularExpresion,
		m_curlyBrace,
		m_end,
	};

	class TokenData: public dDeterministicFiniteAutonataCompiler
	{
		public:
		TokenData (Token token, const char* const regulatExpresion)
			:dDeterministicFiniteAutonataCompiler (regulatExpresion)
			,m_token(token)
		{
		}

		~TokenData()
		{

		}
		Token m_token;
	};

	class TokenDataList: public dList<TokenData*>
	{
		public:
		TokenDataList ()
			:dList<TokenData*>()
		{
		}

		~TokenDataList ()
		{
			DeleteAll();
		}

		void AddTokenData (Token token, const char* const regulatExpresion)
		{
			TokenData* const data = new TokenData (token, regulatExpresion);
			Append (data);
		}

		void DeleteAll()
		{
			for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
				delete node->GetInfo();
			}
			dList<TokenData*>::RemoveAll();
		}
	};

	class DefinitionsMap;
	class ExpandedNFA: public dNonDeterministicFiniteAutonataCompiler
	{
		public:
		ExpandedNFA (const char* const regulatExpresion, DefinitionsMap* const map);

		virtual void ShiftID();
		virtual bool IsOperator (int charater) const;
		virtual bool CheckInsertConcatenation (int left, int right) const;
		virtual void PreProcessExpression (const char* const regularExpression);

		void PushNFA (ExpandedNFA* const nfa, const char* const label);

		DefinitionsMap* m_map;

	};


	class DefinitionsMap: public dTree<ExpandedNFA*,int>
	{
		public:
		DefinitionsMap ();
		~DefinitionsMap ();
		void AddDefinition (const char* const label, const char* const regulatExpresion);
		ExpandedNFA* FindNDAExpresion (const char* const label) const;
	};


	public:
	dLexCompiler(const char* const inputRules, const char* const outputFileName);
	~dLexCompiler();

	private:
	void NextToken ();
	void MatchToken (Token token);
	void CopyTokenStream (char* const buffer) const;
	void ParseDefinitionExpression (string& preheaderCode);
	void ParseDefinitionBlock (string& preheaderCode);
	int ParseDefinitions (string& userPreheaderCode, string& nextCodeCases, string& automataCode, dChatertSetMap& characterSet, 
						   dTree<int, int>& transitionsCountMap, dTree<dTree<int, int>, int>& nextState,
						   dTree<dTree<char, int>, int>& characterTestMap, dTree<dTree<int, int>, int>& testSetArrayIndexMap);

	
	void CreateHeaderFile (const char* const fileName, const char* const className) const;
	void CreateCodeFile (const char* const fileName, const char* const className, int stateCount,
						 string& userPreheaderCode, string& nextCodeCases, string& automataCode, dChatertSetMap& characterSet,
						 dTree<int, int>& transitionsCountMap, dTree<dTree<int, int>, int>& nextState,
						 dTree<dTree<char, int>, int>& characterTestMap, dTree<dTree<int, int>, int>& testSetArrayIndexMap) const;


	Token m_token;
	int m_grammarTokenStart;
	int m_grammarTokenLength;
	const char* m_grammar;
	TokenDataList m_tokenList;
	DefinitionsMap m_defintions;
	
};


#endif

// TODO: reference additional headers your program requires here

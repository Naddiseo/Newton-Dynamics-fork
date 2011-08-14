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
		GRAMMAR_SEGEMENT,
	};
/*
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
		DefinitionsMap ()
			:dTree<ExpandedNFA*,int>()
		{
		}

		
		void AddDefinition (const char* const label, const char* const regulatExpresion);
		ExpandedNFA* FindNDAExpresion (const char* const label) const;
	};
*/


	dParcelCompiler(const char* const inputRules, const char* const outputFileName);
	~dParcelCompiler();

	private:
/*
	void NextToken ();
	void MatchToken (Token token);
	void CopyTokenStream (char* const buffer) const;
	void ParseDefinitions (FILE* const file, const char* const className);
	void ParseDefinitionExpression (string& preheaderCode);
	void ParseDefinitionBlock (string& preheaderCode);


	Token m_token;
	int m_grammarTokenStart;
	int m_grammarTokenLength;
	const char* m_grammar;
	TokenDataList m_tokenList;
	DefinitionsMap m_defintions;
*/	
};


#endif

// TODO: reference additional headers your program requires here

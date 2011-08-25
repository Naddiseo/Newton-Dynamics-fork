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


#ifndef __dNonDeterministicFiniteAutonataCompiler_h_
#define __dNonDeterministicFiniteAutonataCompiler_h_

#include "dChatertSetMap.h"

#include <string>
using namespace std;


#define D_ASCII_SET_MAX_SIZE	1024



class dAutomataState;

int GetScapeChar (int symbol);

class dNonDeterministicFiniteAutonataCompiler
{
	public:
	dNonDeterministicFiniteAutonataCompiler();
	dNonDeterministicFiniteAutonataCompiler(const char* const regularExpression);
//	dNonDeterministicFiniteAutonataCompiler(const dNonDeterministicFiniteAutonataCompiler& nfa);
	virtual ~dNonDeterministicFiniteAutonataCompiler();

	bool IsValid() const;
	const dChatertSetMap& GetChatertSetMap() const;
	dAutomataState* dNonDeterministicFiniteAutonataCompiler::CreateDeterministicFiniteAutomaton () const;

	protected:
	enum Operations
	{
		m_union = '|',						// a|b	
		m_concatenation = 0x7f,				// ab
		m_zeroOrMore = '*',					// a* 
		m_oneOrMore  = '+',					// a+ 
		m_zeroOrOne  = '?',					// a? 
		m_openParentesis = '(',				// (a)
		m_closeParentesis = ')',			// (a)  
		m_openSquareBrakect = '[',			// [a] 
		m_closeSquareBrakect = ']',			// [a] 
	};
	

	class StateConstructPair
	{
		public: 
		StateConstructPair ()
		{
		}

		StateConstructPair (dAutomataState* start, dAutomataState* accepting)
			:m_start(start)
			,m_accepting(accepting)
		{
		}

		dAutomataState* GetStart() const { return m_start;}
		dAutomataState* GetAccepting() const { return m_accepting;}

		private:
		dAutomataState* m_start;
		dAutomataState* m_accepting;
	};

	class dAutomataStateConstructStack
	{
		public:
		dAutomataStateConstructStack ()
			:m_index(0)
		{
		}

		bool IsEmpty() const
		{
			return m_index == 0;
		}

		StateConstructPair Pop ()
		{
			_ASSERTE (m_index);
			return m_pool[--m_index];
		}

		void Push (dAutomataState* const start, dAutomataState* const accepting)
		{
			m_pool[m_index++] = StateConstructPair (start, accepting);
			_ASSERTE (m_index <= sizeof (m_pool)/sizeof (m_pool[0]));
		}

		private:
		int m_index;
		StateConstructPair m_pool[1024];
	};

	virtual void ShiftID();
	virtual bool IsOperator (int charater) const;
	virtual bool CheckInsertConcatenation (int left, int right) const;

	virtual void PreProcessExpression (const char* const regularExpression);
	void CompileExpression(const char* const regularExpression);	
	int GetChar();
	void ParseExpresionToNFA ();
	void DeleteNFA (dAutomataState* const startdAutomataState);
	

	void Match (int token);
	void PushId (int charater);
	void PushSet (const char* const set, int size);
	void ReduceUnionDiagram();
	void ReduceConcatenationDiagram();
	void ReduceZeroOrMoreDiagram ();
	void ReduceOneOrMoreDiagram();
	void ReduceZeroOrOneDiagram();

	void UnionExpression ();
	void ConcatenationExpression ();
	void UnuaryExpression ();
	int BracketedExpression (char* const set, int size);

	bool CompareSets (dList<dAutomataState*>& setA, dTree<dAutomataState*,dAutomataState*>& setB) const;
	void MoveSymbol (int symbol, const dAutomataState* const state, dTree<dAutomataState*,dAutomataState*>& ouput) const;
	void EmptyTransitionClosure (const dTree<dAutomataState*,dAutomataState*>& set, dTree<dAutomataState*,dAutomataState*>& closureStates) const;

	static int SortStates (const void *ptr0, const void *ptr1);

	bool m_error;
	int m_token;
	int m_stateID;
//	int m_stateMark;
	int m_regularExpressionIndex;	
	char m_regularExpression[256];	
	dAutomataState* m_startState; 
	dAutomataState* m_acceptingState; 
	dAutomataStateConstructStack m_stack;

	dChatertSetMap m_charaterSetMap;

	static char m_asciiSet[128];
	static char m_asciiSetButNewLine[128];
};


#endif
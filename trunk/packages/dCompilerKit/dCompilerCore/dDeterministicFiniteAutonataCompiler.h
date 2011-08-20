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

#ifndef __dDeterministicFiniteAutonataCompiler_h_
#define __dDeterministicFiniteAutonataCompiler_h_


#include "dNonDeterministicFiniteAutonataCompiler.h"


class dDeterministicFiniteAutonataCompiler
{
	public:
	dDeterministicFiniteAutonataCompiler(const char* const regularExpression);
	dDeterministicFiniteAutonataCompiler(const dNonDeterministicFiniteAutonataCompiler& expression);
	
	virtual ~dDeterministicFiniteAutonataCompiler(void);

	int FindMatch(const char* const text) const;
	int ConvertSwitchCaseStatements (string& parseTokenOutput, string& nextTokenOutput, int initialState, 
									dChatertSetMap& characterSet, const string& action,
									dTree<int, int>& transitionsCountMap,
									dTree<dTree<int, int>, int>& nextState,
									dTree<dTree<char, int>, int>& characterTestMap,
									dTree<dTree<int, int>, int>& testSetArrayIndexMap) const;

	private:
	void CopySet (const dNonDeterministicFiniteAutonataCompiler& nfa);
	void ParseExpresionToNFA ();

	void AddText (string& output, const char* const fmt, ...) const;

	dAutomataState* m_deterministicFiniteAutomata; 
	dChatertSetMap m_charaterSetMap;

};


#endif
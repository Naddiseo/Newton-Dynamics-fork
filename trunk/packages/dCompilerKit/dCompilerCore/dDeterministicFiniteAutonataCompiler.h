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
	dDeterministicFiniteAutonataCompiler();
	dDeterministicFiniteAutonataCompiler(const char* const regularExpression);
	dDeterministicFiniteAutonataCompiler(const dNonDeterministicFiniteAutonataCompiler& nfa);
	
	virtual ~dDeterministicFiniteAutonataCompiler(void);

	virtual int FindMatch(const char* const text) const;

	protected:
		
	virtual dAutomataState* CreateTargetState (dTree<dAutomataState*,dAutomataState*>& subSet, int id);
	void CreateDeterministicFiniteAutomaton (const dNonDeterministicFiniteAutonataCompiler& nfa);

	private:
	void CopySet (const dNonDeterministicFiniteAutonataCompiler& nfa);
	bool CompareSets (dList<dAutomataState*>& setA, dTree<dAutomataState*,dAutomataState*>& setB) const;
	void MoveSymbol (int symbol, const dAutomataState* const state, dTree<dAutomataState*,dAutomataState*>& ouput) const;
	void EmptyTransitionClosure (const dTree<dAutomataState*,dAutomataState*>& set, dTree<dAutomataState*,dAutomataState*>& closureStates) const;

	protected:
	dAutomataState* m_deterministicFiniteAutomata; 
	dChatertSetMap m_charaterSetMap;
};


#endif
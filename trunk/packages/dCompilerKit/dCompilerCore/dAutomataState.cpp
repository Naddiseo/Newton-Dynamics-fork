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


#include "dCompilerCore.h"
#include "dAutomataState.h"


dAutomataState::Transition::Transition (int character, dAutomataState* const targetdAutomataState)
	:m_character(character)
	,m_targetdAutomataState(targetdAutomataState)
{
}


int dAutomataState::Transition::GetCharater () const 
{	return m_character;
}

dAutomataState* dAutomataState::Transition::GetState() const 
{ 
	return m_targetdAutomataState;
}





dAutomataState::dAutomataState (int id)
	:m_id (id)
	,m_mark (0)
	,m_exitState(false)
{
}

dAutomataState::dAutomataState (dTree<dAutomataState*,dAutomataState*>& subSet, int id)
	:m_id (id)
	,m_mark (0)
	,m_exitState(false)
{
	dTree<dAutomataState*,dAutomataState*>::Iterator iter (subSet);
	for (iter.Begin(); iter; iter ++) {
		dAutomataState* const dAutomataState = iter.GetNode()->GetInfo();
		m_myNFANullStates.Append(dAutomataState);
		if (dAutomataState->m_exitState) {
			m_exitState = true;
		}
	}
}

dAutomataState::~dAutomataState ()
{
}


void dAutomataState::GetStateArray (dList<dAutomataState*>& statesList)
{
	dTree<dAutomataState*, dAutomataState*> filter;

	int stack = 1;
	dAutomataState* pool[256];
	pool[0] = this;
	filter.Insert(this, this);

	while (stack) {
		stack --;
		dAutomataState* const state = pool[stack];
		statesList.Append(state);

		for (dList<dAutomataState::Transition>::dListNode* node = state->m_transtions.GetFirst(); node; node = node->GetNext()) {
			dAutomataState* const state = node->GetInfo().GetState();
			if (!filter.Find (state)) {
				pool[stack] = state;
				filter.Insert(state, state);
				stack ++;
				_ASSERTE (stack < sizeof (pool)/sizeof (pool[0]));
			}
		}
	}
}


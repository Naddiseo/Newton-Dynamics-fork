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
#include "dDeterministicFiniteAutonataCompiler.h"
#include "dNonDeterministicFiniteAutonataCompiler.h"

dDeterministicFiniteAutonataCompiler::dDeterministicFiniteAutonataCompiler(const char* const regularExpression)
	:m_deterministicFiniteAutomata(NULL)
{
	dNonDeterministicFiniteAutonataCompiler nfa (regularExpression);
	if (nfa.IsValid()) {
		
		m_deterministicFiniteAutomata = nfa.CreateDeterministicFiniteAutomaton();
		CopySet (nfa);
	}
}

dDeterministicFiniteAutonataCompiler::dDeterministicFiniteAutonataCompiler(const dNonDeterministicFiniteAutonataCompiler& nfa)
	:m_deterministicFiniteAutomata(NULL)
{
	if (nfa.IsValid()) {
		m_deterministicFiniteAutomata = nfa.CreateDeterministicFiniteAutomaton();
		CopySet (nfa);
	}
}

dDeterministicFiniteAutonataCompiler::~dDeterministicFiniteAutonataCompiler(void)
{
	if (m_deterministicFiniteAutomata) {
		dList<dAutomataState*> statesList;
		m_deterministicFiniteAutomata->GetStateArray (statesList);
		for (dList<dAutomataState*>::dListNode* node = statesList.GetFirst(); node; node = node->GetNext()) {
			dAutomataState* const state = node->GetInfo();
			delete state;
		}
	}

}


void dDeterministicFiniteAutonataCompiler::CopySet (const dNonDeterministicFiniteAutonataCompiler& nfa)
{
	dList<dAutomataState*> stateList;
	m_deterministicFiniteAutomata->GetStateArray (stateList);

	dTree<int, int> setFilter;
	for (dList<dAutomataState*>::dListNode* node = stateList.GetFirst(); node; node = node->GetNext()) {
		dAutomataState* const state = node->GetInfo();
		for (dList<dAutomataState::dTransition>::dListNode* transitionNode = state->m_transtions.GetFirst(); transitionNode; transitionNode = transitionNode->GetNext()) {
			dAutomataState::dTransition& transition = transitionNode->GetInfo();
			dAutomataState::dCharacter ch (transition.GetCharater());
//			if (ch & (1<<15)) {
			if (ch.m_type == dAutomataState::CHARACTER_SET) {
				setFilter.Insert (ch.m_symbol, ch.m_symbol);
			}
		}
	}

	const dChatertSetMap& charaterSet = nfa.GetChatertSetMap();
	dTree<int, int>::Iterator iter (setFilter);
	for (iter.Begin(); iter; iter ++) {
		dAutomataState::dCharacter ch (iter.GetNode()->GetInfo());
		const dChatertSetMap::ChatertSet* const set = charaterSet.FindSet (ch.m_info);
		_ASSERTE (set);
		int ch1 = m_charaterSetMap.AddSet(set->GetSet(), set->GetLength());
		_ASSERTE (ch1 == ch.m_info);
	}
}


int dDeterministicFiniteAutonataCompiler::FindMatch(const char* const text) const
{
	int count = -1;
	if (text[0]) {
		count = 0;
		dAutomataState* state = m_deterministicFiniteAutomata;
		for(int i = 0; text[i]; i ++) {
			int extendChar = 0;
			int ch = text[i];
			if (ch == '\\') {
				i ++;
				extendChar = 1;
				_ASSERTE (text[i]);
				ch = (ch << 8) + text[i];
				ch = GetScapeChar (ch);
			}


			dAutomataState::dTransition* transition = NULL;
			for (dList<dAutomataState::dTransition>::dListNode* node = state->m_transtions.GetFirst(); node; node = node->GetNext()) {
				dAutomataState::dTransition*const trans = &node->GetInfo();
				dAutomataState::dCharacter symbol (trans->GetCharater());

				if (symbol.m_type == dAutomataState::CHARACTER) {
					if (symbol.m_info == ch) {
						transition = trans;
						break;
					}
//				} else if (symbol & (1<<15)) {
				} else if (symbol.m_type == dAutomataState::CHARACTER_SET) {
					const dChatertSetMap::ChatertSet* const characterSet = m_charaterSetMap.FindSet(symbol.m_info);
					if (characterSet) {
						if (characterSet->IsCharAMatch (GetScapeChar (ch))) {
							transition = trans;
							break;
						}
					}
				} else {
					_ASSERTE (0);
				}
			}

			if(transition) {
				count ++;
				count += extendChar;
				state = transition->GetState();
			} else {
				if (!state->m_exitState) {
					count = -1;
				}
				break;
			}
		}
	}

	return count;
}

void dDeterministicFiniteAutonataCompiler::AddText (string& output, const char* const fmt, ...) const
{
	va_list v_args;
	char text[2048];
	
	text[0] = '\t';
	text[1] = '\t';
	text[2] = '\t';
	text[3] = 0;
	va_start (v_args, fmt);     
	vsprintf(&text[3], fmt, v_args);
	va_end (v_args);            

	output += text;
}

#if 0
int dDeterministicFiniteAutonataCompiler::ConvertSwitchCaseStatements (string& parseTokenOutput, string& nextTokenOutput, int stateIdBase, dChatertSetMap& characterSet, const string& action) const 
{
	dTree<dAutomataState*,dAutomataState*> filter;

	int stack = 1;
	dAutomataState* pool[128];

	pool[0] = m_deterministicFiniteAutomata;
	filter.Insert(pool[0], pool[0]);

	int lastState = m_deterministicFiniteAutomata->m_id;

	while (stack) {
		stack --;

		dAutomataState* const state = pool[stack];
		_ASSERTE (filter.Find(state));

		 AddText (parseTokenOutput, "case %d:\n", state->m_id + stateIdBase);
		 AddText (parseTokenOutput, "{\n");

		if (!state->m_exitState || state->m_transtions.GetCount()) {
			AddText (parseTokenOutput, "\tchar ch = NextChar();\n");
		}

		if ((state->m_id) > lastState) {
			lastState = state->m_id;
		}

		char condition[126];
		condition[0] = 0;
		for (dList<dAutomataState::dTransition>::dListNode* node = state->m_transtions.GetFirst(); node; node = node->GetNext()) {
			dAutomataState::dTransition& sourceTransition = node->GetInfo();

			dAutomataState* const targetState = sourceTransition.GetState();
			dTree<dAutomataState*,dAutomataState*>::dTreeNode* const mapMode = filter.Find(targetState);

			int ch = sourceTransition.GetCharater();

			if (mapMode) {
				if (ch & (1<<15)) {
					const dChatertSetMap::ChatertSet* const charSet = m_charaterSetMap.FindSet (ch);
					_ASSERTE (charSet);
					ch = characterSet.AddSet (charSet->GetSet(), charSet->GetLength());
					AddText (parseTokenOutput, "\t%sif (IsCharInSet (ch, text_%d)) m_state = %d;\n", condition, ch & 0x7fff, targetState->m_id + stateIdBase);
				} else {
					AddText (parseTokenOutput, "\t%sif (ch == %d) m_state = %d;\n", condition, GetScapeChar(ch), targetState->m_id + stateIdBase);
				}

			} else {
				pool[stack] = targetState;
				filter.Insert(targetState, targetState);
				stack ++;
				_ASSERTE (stack < sizeof (pool)/sizeof (pool[0]));

				if (ch & (1<<15)) {
					const dChatertSetMap::ChatertSet* const charSet = m_charaterSetMap.FindSet (ch);
					_ASSERTE (charSet);
					ch = characterSet.AddSet (charSet->GetSet(), charSet->GetLength());

					AddText (parseTokenOutput, "\t%sif (IsCharInSet (ch, text_%d)) m_state = %d;\n", condition, ch & 0x7fff, targetState->m_id + stateIdBase);

				} else {
					AddText (parseTokenOutput, "\t%sif (ch == %d) m_state = %d;\n", condition, GetScapeChar(ch), targetState->m_id + stateIdBase);
				}
			}
			sprintf (condition, "else ");
		}

		if (state->m_exitState) {
			// this is the exit;

			AddText (parseTokenOutput, "\t%s{\n", condition);

			//AddText (parseTokenOutput, "\t\tm_state = %d;\n", condition, state->m_id + stateIdBase);
			if (state->m_transtions.GetCount()) {
				AddText (parseTokenOutput, "\t\tm_index --;\n");
			}
			AddText (parseTokenOutput, "\t\tGetLexString ();\n");
			
			//AddText (parseTokenOutput, "\t\t{\n");
				AddText (parseTokenOutput, "\t\t//user specified action\n");
				AddText (parseTokenOutput, "\t\tm_state = 0;\n");
				AddText (parseTokenOutput, "\t\tm_startState = 0;\n");
				AddText (parseTokenOutput, "\t\t%s\n", action.c_str());
				
			//AddText (parseTokenOutput, "\t\t}\n");

			

			AddText (parseTokenOutput, "\t}\n");

		} else {
			 AddText (parseTokenOutput, "\t%sm_state = NextPattern();\n", condition);
		}

		AddText (parseTokenOutput, "\tbreak;\n");

		AddText (parseTokenOutput, "}\n");
	}

//	AddText (nextTokenOutput, "case %d: m_startState = %d; break;\n", m_deterministicFiniteAutomata->m_id + stateIdBase, lastState + stateIdBase + 1);
	char nextJump[256];
	static string filler ("0, ");
	sprintf (nextJump, "%d, ", lastState + stateIdBase + 1);
	nextTokenOutput += nextJump;
	for (int i = 0; i < lastState; i ++) {
		nextTokenOutput += filler;
	}
	

//	DTRACE ((parseTokenOutput.c_str()));
	return stateIdBase + lastState + 1;
}
#endif


int dDeterministicFiniteAutonataCompiler::ConvertSwitchCaseStatements (
	string& parseTokenOutput, 
	string& nextTokenOutput, 
	int stateIdBase, 
	dChatertSetMap& characterSet, 
	const string& userAction,
	dTree<int, int>& transitionsCountMap,
	dTree<dTree<int, int>, int>& nextStateMap,
	dTree<dTree<char, int>, int>& characterTestMap,
	dTree<dTree<int, int>, int>& testSetArrayIndexMap) const 
{
	dTree<dAutomataState*,dAutomataState*> filter;

	int stack = 1;
	dAutomataState* pool[128];

	pool[0] = m_deterministicFiniteAutomata;
	filter.Insert(pool[0], pool[0]);

	int lastState = m_deterministicFiniteAutomata->m_id;

	while (stack) {
		stack --;

		dAutomataState* const state = pool[stack];
		_ASSERTE (filter.Find(state));


		if ((state->m_id) > lastState) {
			lastState = state->m_id;
		}

		char condition[126];
		condition[0] = 0;

		if (state->m_exitState) {
			AddText (parseTokenOutput, "case %d:\n", state->m_id + stateIdBase);
			AddText (parseTokenOutput, "{\n");
			if (!state->m_exitState || state->m_transtions.GetCount()) {
				AddText (parseTokenOutput, "\tchar ch = NextChar();\n");
			}

			for (dList<dAutomataState::dTransition>::dListNode* node = state->m_transtions.GetFirst(); node; node = node->GetNext()) {


				dAutomataState::dTransition& sourceTransition = node->GetInfo();
				dAutomataState* const targetState = sourceTransition.GetState();

				if (!filter.Find(targetState)) {
					pool[stack] = targetState;
					filter.Insert(targetState, targetState);
					stack ++;
					_ASSERTE (stack < sizeof (pool)/sizeof (pool[0]));
				}

				dAutomataState::dCharacter ch (sourceTransition.GetCharater());
//				if (ch & (1<<15)) {
				if (ch.m_type == dAutomataState::CHARACTER_SET) {
					const dChatertSetMap::ChatertSet* const charSet = m_charaterSetMap.FindSet (ch.m_info);
					_ASSERTE (charSet);
					int ch = characterSet.AddSet (charSet->GetSet(), charSet->GetLength());
					AddText (parseTokenOutput, "\t%sif (IsCharInSet (ch, text_%d)) m_state = %d;\n", condition, ch, targetState->m_id + stateIdBase);
					
				} else if (ch.m_type == dAutomataState::CHARACTER) {
					AddText (parseTokenOutput, "\t%sif (ch == %d) m_state = %d;\n", condition, GetScapeChar(ch.m_info), targetState->m_id + stateIdBase);
				} else {
					_ASSERTE (0);
				}


				sprintf (condition, "else ");
			}

			AddText (parseTokenOutput, "\t%s{\n", condition);

			//AddText (parseTokenOutput, "\t\tm_state = %d;\n", condition, state->m_id + stateIdBase);
			if (state->m_transtions.GetCount()) {
				AddText (parseTokenOutput, "\t\tm_index --;\n");
			}
			AddText (parseTokenOutput, "\t\tGetLexString ();\n");

			//AddText (parseTokenOutput, "\t\t{\n");
			AddText (parseTokenOutput, "\t\t//user specified action\n");
			AddText (parseTokenOutput, "\t\tm_state = 0;\n");
			AddText (parseTokenOutput, "\t\tm_startState = 0;\n");
			AddText (parseTokenOutput, "\t\t%s\n", userAction.c_str());

			//AddText (parseTokenOutput, "\t\t}\n");

			AddText (parseTokenOutput, "\t}\n");

			AddText (parseTokenOutput, "\tbreak;\n");

			AddText (parseTokenOutput, "}\n");

		} else {

			transitionsCountMap.Insert(state->m_transtions.GetCount(), state->m_id + stateIdBase);
			for (int i = 0; i < state->m_transtions.GetCount(); i ++) {
				if (!nextStateMap.Find(i)) {
					nextStateMap.Insert(i);
					characterTestMap.Insert(i);
					testSetArrayIndexMap.Insert(i);
				}
			}

			int transitionIndex = 0;
			for (dList<dAutomataState::dTransition>::dListNode* node = state->m_transtions.GetFirst(); node; node = node->GetNext()) {

				dTree<int, int>& nextState = nextStateMap.Find(transitionIndex)->GetInfo();
				dTree<char, int>& charatestTest = characterTestMap.Find(transitionIndex)->GetInfo();
				dTree<int, int>& testSetArrayIndex = testSetArrayIndexMap.Find(transitionIndex)->GetInfo();

				dAutomataState::dTransition& sourceTransition = node->GetInfo();
				dAutomataState* const targetState = sourceTransition.GetState();

				nextState.Insert(targetState->m_id + stateIdBase, state->m_id + stateIdBase);
				
				if (!filter.Find(targetState)) {
					pool[stack] = targetState;
					filter.Insert(targetState, targetState);
					stack ++;
					_ASSERTE (stack < sizeof (pool)/sizeof (pool[0]));
				}

				//int ch = sourceTransition.GetCharater();
				dAutomataState::dCharacter ch (sourceTransition.GetCharater());
				//if (ch & (1<<15)) {
				if (ch.m_type == dAutomataState::CHARACTER_SET) {
					const dChatertSetMap::ChatertSet* const charSet = m_charaterSetMap.FindSet (ch.m_info);
					_ASSERTE (charSet);
					int ch = characterSet.AddSet (charSet->GetSet(), charSet->GetLength());
					testSetArrayIndex.Insert(ch, state->m_id + stateIdBase);
				} else if (ch.m_type == dAutomataState::CHARACTER) {
					charatestTest.Insert(char(GetScapeChar(ch.m_info)), state->m_id + stateIdBase);
				} else {
					_ASSERTE (0);
				}

				transitionIndex ++;
			}
		}
	}

	char nextJump[256];
	static string filler ("0, ");
	sprintf (nextJump, "%d, ", lastState + stateIdBase + 1);
	nextTokenOutput += nextJump;
	for (int i = 0; i < lastState; i ++) {
		nextTokenOutput += filler;
	}


	//	DTRACE ((parseTokenOutput.c_str()));
	return stateIdBase + lastState + 1;
}
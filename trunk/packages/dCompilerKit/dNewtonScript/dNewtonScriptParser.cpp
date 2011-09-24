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

//
//Auto generated Parser Generator class: dNewtonScriptParser.cpp
//


#include <dScriptCompiler.h>
#include "dNewtonScriptLexical.h"

//
// Newton Script parcer  
// loosely based on a subset of Java and C sharp
//

#include "dNewtonScriptParser.h"
#include <dList.h>

#define MAX_USER_PARAM	64

enum dNewtonScriptParser::ActionType
{
	dSHIFT = 0,
	dREDUCE,
	dACCEPT,
	dERROR
};


class dNewtonScriptParser::dActionEntry
{
	public:
	dActionEntry (short token, short stateType,	short nextState, short ruleSymbols, short ruleIndex)
		:m_token(token), m_stateType(stateType), m_nextState(nextState), m_ruleSymbols(ruleSymbols), m_ruleIndex(ruleIndex)
	{
	}

	short m_token;
	short m_stateType;// 0 = shift, 1 = reduce, 2 = accept
	short m_nextState;
	short m_ruleSymbols;
	short m_ruleIndex;
};

class dNewtonScriptParser::dGotoEntry
{
	public:
	dGotoEntry (short token, short nextState)
		:m_token(token), m_nextState(nextState)
	{
	}

	short  m_token;
	short  m_nextState;
};



class dNewtonScriptParser::dStackPair
{
	public:
	dStackPair()
		:m_state(0), m_token(dToken (0)), m_value()
	{
	}

	int m_state;
	dToken m_token;
	dUserVariable m_value;
};


dNewtonScriptParser::dNewtonScriptParser()
{
}

dNewtonScriptParser::~dNewtonScriptParser()
{
}


bool dNewtonScriptParser::ErrorHandler (const string& line) const
{
	line;
	return false;
}

const dNewtonScriptParser::dActionEntry* dNewtonScriptParser::FindAction (const dActionEntry* const actionList, int count, dToken token) const
{
	int i0 = 0;
	int i1 = count - 1;
	while ((i1 - i0) >= 4) {
		int i = (i1 + i0 + 1)>>1;

		const dActionEntry& action = actionList[i];
		dToken actionToken (dToken(action.m_token));
		if (token <= actionToken) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		const dActionEntry& action = actionList[i];
		dToken actionToken (dToken(action.m_token));
		if (token == actionToken) {
			return& action;
		}
	}

	_ASSERT (0);
	return NULL;
}

const dNewtonScriptParser::dGotoEntry* dNewtonScriptParser::FindGoto (const dGotoEntry* const gotoList, int count, dToken token) const
{
	int i0 = 0;
	int i1 = count - 1;
	while ((i1 - i0) >= 4) {
		int i = (i1 + i0 + 1)>>1;

		const dGotoEntry& action = gotoList[i];
		dToken actionToken (dToken(action.m_token));
		if (token <= actionToken) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		const dGotoEntry& action = gotoList[i];
		dToken actionToken (dToken(action.m_token));
		if (token == actionToken) {
			return &action;
		}
	}

	_ASSERT (0);
	return NULL;
}


bool dNewtonScriptParser::Parse(dNewtonScriptLexical& scanner)
{
	dList<dStackPair> stack;
	static short actionsCount[] = {
			4, 1, 1, 3, 1, 1, 3, 3, 4, 3, 1, 3, 2, 1, 2, 1, 1, 1, 1, 2, 2, 2, 2, 1, 
			2, 3, 2, 2, 2, 1, 1, 1, 2, 0, 2, 2, 1, 2, 2, 2, 2, 1, 2, 2, 1, 2, 2, 2, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2};
	static short actionsStart[] = {
			0, 4, 5, 6, 9, 10, 11, 14, 17, 21, 24, 25, 28, 30, 31, 33, 34, 35, 36, 37, 39, 41, 43, 45, 
			46, 48, 51, 53, 55, 57, 58, 59, 51, 60, 60, 62, 64, 65, 67, 69, 71, 59, 73, 75, 58, 77, 79, 81, 
			83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 104};
	static short gotoCount[] = {
			6, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 1, 0, 1, 0, 0, 4, 0, 0, 3, 0, 0, 3, 0, 0, 1, 0, 0, 1, 2, 1, 
			1, 2, 0, 0, 0, 0, 0, 0, 2, 1, 0, 2, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0};
	static short gotoStart[] = {
			0, 6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10, 11, 11, 12, 12, 12, 16, 16, 16, 19, 19, 19, 22, 22, 22, 23, 23, 23, 24, 26, 
			27, 28, 30, 30, 30, 30, 30, 30, 30, 32, 33, 33, 35, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 38};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (272, 5), dGotoEntry (273, 8), dGotoEntry (274, 7), dGotoEntry (275, 6), dGotoEntry (276, 3), 
			dGotoEntry (277, 4), dGotoEntry (274, 11), dGotoEntry (275, 6), dGotoEntry (276, 3), dGotoEntry (277, 4), 
			dGotoEntry (278, 14), dGotoEntry (279, 16), dGotoEntry (280, 20), dGotoEntry (281, 21), dGotoEntry (282, 19), 
			dGotoEntry (283, 22), dGotoEntry (281, 24), dGotoEntry (282, 19), dGotoEntry (283, 22), dGotoEntry (286, 28), 
			dGotoEntry (287, 27), dGotoEntry (288, 29), dGotoEntry (284, 31), dGotoEntry (289, 35), dGotoEntry (291, 37), 
			dGotoEntry (292, 38), dGotoEntry (285, 39), dGotoEntry (284, 41), dGotoEntry (287, 42), dGotoEntry (288, 29), 
			dGotoEntry (295, 47), dGotoEntry (296, 46), dGotoEntry (285, 49), dGotoEntry (293, 53), dGotoEntry (294, 52), 
			dGotoEntry (292, 54), dGotoEntry (296, 55), dGotoEntry (294, 59)};
	static dActionEntry actionTable[] = {
			dActionEntry (255, 1, 0, 0, 1), dActionEntry (257, 0, 1, 0, 0), dActionEntry (259, 0, 2, 0, 0), dActionEntry (260, 1, 5, 0, 13), 
			dActionEntry (258, 0, 9, 0, 0), dActionEntry (260, 1, 5, 1, 14), dActionEntry (255, 1, 2, 1, 6), dActionEntry (257, 1, 2, 1, 6), 
			dActionEntry (260, 1, 2, 1, 6), dActionEntry (260, 0, 10, 0, 0), dActionEntry (255, 2, 0, 0, 0), dActionEntry (255, 1, 2, 1, 5), 
			dActionEntry (257, 1, 2, 1, 5), dActionEntry (260, 1, 2, 1, 5), dActionEntry (255, 1, 1, 1, 3), dActionEntry (257, 1, 1, 1, 3), 
			dActionEntry (260, 1, 1, 1, 3), dActionEntry (255, 1, 0, 1, 2), dActionEntry (257, 0, 1, 0, 0), dActionEntry (259, 0, 2, 0, 0), 
			dActionEntry (260, 1, 5, 0, 13), dActionEntry (255, 1, 3, 2, 7), dActionEntry (257, 1, 3, 2, 7), dActionEntry (260, 1, 3, 2, 7), 
			dActionEntry (261, 0, 12, 0, 0), dActionEntry (255, 1, 1, 2, 4), dActionEntry (257, 1, 1, 2, 4), dActionEntry (260, 1, 1, 2, 4), 
			dActionEntry (123, 1, 6, 0, 11), dActionEntry (262, 0, 13, 0, 0), dActionEntry (123, 1, 6, 1, 12), dActionEntry (123, 1, 7, 0, 9), 
			dActionEntry (263, 0, 15, 0, 0), dActionEntry (123, 1, 7, 1, 10), dActionEntry (123, 0, 17, 0, 0), dActionEntry (261, 0, 18, 0, 0), 
			dActionEntry (40, 0, 23, 0, 0), dActionEntry (125, 1, 9, 1, 17), dActionEntry (261, 1, 9, 1, 17), dActionEntry (125, 0, 25, 0, 0), 
			dActionEntry (261, 0, 18, 0, 0), dActionEntry (125, 1, 8, 1, 15), dActionEntry (261, 1, 8, 1, 15), dActionEntry (125, 1, 9, 1, 18), 
			dActionEntry (261, 1, 9, 1, 18), dActionEntry (41, 0, 26, 0, 0), dActionEntry (125, 1, 8, 2, 16), dActionEntry (261, 1, 8, 2, 16), 
			dActionEntry (255, 1, 4, 8, 8), dActionEntry (257, 1, 4, 8, 8), dActionEntry (260, 1, 4, 8, 8), dActionEntry (58, 0, 30, 0, 0), 
			dActionEntry (123, 1, 12, 0, 37), dActionEntry (41, 1, 14, 1, 21), dActionEntry (44, 1, 14, 1, 21), dActionEntry (41, 0, 32, 0, 0), 
			dActionEntry (44, 0, 33, 0, 0), dActionEntry (261, 0, 34, 0, 0), dActionEntry (261, 0, 36, 0, 0), dActionEntry (123, 0, 40, 0, 0), 
			dActionEntry (41, 1, 17, 1, 36), dActionEntry (44, 1, 17, 1, 36), dActionEntry (41, 1, 15, 2, 23), dActionEntry (44, 1, 15, 2, 23), 
			dActionEntry (40, 0, 43, 0, 0), dActionEntry (44, 0, 44, 0, 0), dActionEntry (123, 1, 12, 2, 38), dActionEntry (44, 1, 19, 1, 39), 
			dActionEntry (123, 1, 19, 1, 39), dActionEntry (125, 1, 10, 5, 19), dActionEntry (261, 1, 10, 5, 19), dActionEntry (125, 0, 48, 0, 0), 
			dActionEntry (267, 0, 45, 0, 0), dActionEntry (41, 1, 14, 3, 22), dActionEntry (44, 1, 14, 3, 22), dActionEntry (41, 0, 50, 0, 0), 
			dActionEntry (256, 0, 51, 0, 0), dActionEntry (125, 1, 24, 1, 50), dActionEntry (267, 1, 24, 1, 50), dActionEntry (125, 1, 23, 1, 48), 
			dActionEntry (267, 1, 23, 1, 48), dActionEntry (125, 0, 56, 0, 0), dActionEntry (267, 0, 45, 0, 0), dActionEntry (125, 1, 13, 2, 46), 
			dActionEntry (261, 1, 13, 2, 46), dActionEntry (125, 1, 10, 6, 20), dActionEntry (261, 1, 10, 6, 20), dActionEntry (44, 1, 20, 3, 41), 
			dActionEntry (123, 1, 20, 3, 41), dActionEntry (41, 1, 22, 1, 45), dActionEntry (44, 1, 22, 1, 45), dActionEntry (41, 1, 21, 1, 43), 
			dActionEntry (44, 1, 21, 1, 43), dActionEntry (41, 0, 57, 0, 0), dActionEntry (44, 0, 58, 0, 0), dActionEntry (44, 1, 19, 3, 40), 
			dActionEntry (123, 1, 19, 3, 40), dActionEntry (125, 1, 23, 2, 49), dActionEntry (267, 1, 23, 2, 49), dActionEntry (125, 1, 13, 3, 47), 
			dActionEntry (261, 1, 13, 3, 47), dActionEntry (44, 1, 20, 4, 42), dActionEntry (123, 1, 20, 4, 42), dActionEntry (256, 0, 51, 0, 0), 
			dActionEntry (41, 1, 21, 3, 44), dActionEntry (44, 1, 21, 3, 44)};

	const int lastToken = 272;

	stack.Append ();
	dToken token = dToken (scanner.NextToken());
	for (;;) {
		const dStackPair& stackTop = stack.GetLast()->GetInfo();
		int start = actionsStart[stackTop.m_state];
		int count = actionsCount[stackTop.m_state];
		const dActionEntry* const action (FindAction (&actionTable[start], count, token));
		_ASSERTE (action);

		switch (action->m_stateType) 
		{
			case dSHIFT: 
			{
				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_state = action->m_nextState;
				entry.m_value = dUserVariable (entry.m_token, scanner.GetTokenString());
				token = dToken (scanner.NextToken());
				entry.m_token = token;
				if (token == -1) {
					token = ACCEPTING_TOKEN;
				}

				break;
			}

			case dREDUCE: 
			{
				dStackPair parameter[MAX_USER_PARAM];

				int reduceCount = action->m_ruleSymbols;
				_ASSERTE (reduceCount < sizeof (parameter) / sizeof (parameter[0]));

				for (int i = 0; i < reduceCount; i ++) {
					parameter[reduceCount - i - 1] = stack.GetLast()->GetInfo();
					stack.Remove (stack.GetLast());
				}

				const dStackPair& stackTop = stack.GetLast()->GetInfo();
				int start = gotoStart[stackTop.m_state];
				int count = gotoCount[stackTop.m_state];
				const dGotoEntry* const gotoEntry = FindGoto (&gotoTable[start], count, dToken (action->m_nextState + lastToken));

				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_state = gotoEntry->m_nextState;
				entry.m_token = dToken (gotoEntry->m_token);
				
				switch (action->m_ruleIndex) 
				{
					//do user semantic Actions

					default:;
				}

				break;

			}
	
			case dACCEPT: // 2 = accept
			{
				// program parsed successfully, exit with successful code
				return true;
			}
			
			default:  
			{
				_ASSERTE (0);
				// syntax error parsing program
				//if (!ErrorHandler ("error")) {
				//}
				break;
			}
		}
	}
	return false;
}



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
			4, 1, 1, 3, 1, 3, 3, 4, 1, 3, 3, 1, 2, 1, 2, 1, 1, 2, 2, 3, 2, 2, 1, 2, 
			3, 1, 9, 2, 1, 1, 1, 1, 1, 1, 1, 4, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 8, 
			2, 2, 1, 2, 2, 2, 10, 1, 2, 2, 1, 1, 10, 10, 10, 10, 10, 1, 2, 2, 2, 2, 2, 2, 
			2, 1, 10, 2, 11, 11, 2, 1, 10, 10, 1, 2, 10};
	static short actionsStart[] = {
			0, 4, 5, 6, 9, 10, 13, 16, 20, 21, 24, 27, 28, 30, 31, 33, 34, 35, 37, 39, 42, 44, 46, 47, 
			49, 52, 53, 62, 64, 65, 66, 67, 68, 69, 70, 71, 75, 77, 79, 80, 81, 82, 83, 84, 85, 86, 62, 87, 
			95, 97, 99, 100, 102, 104, 106, 82, 116, 118, 81, 120, 121, 131, 141, 151, 161, 171, 172, 174, 176, 178, 180, 182, 
			184, 186, 187, 197, 199, 210, 221, 223, 224, 234, 186, 244, 246};
	static short gotoCount[] = {
			6, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 1, 0, 1, 0, 0, 5, 0, 4, 0, 0, 0, 0, 0, 0, 4, 1, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 7, 1, 0, 2, 1, 0, 0, 0, 6, 0, 
			0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0};
	static short gotoStart[] = {
			0, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10, 10, 11, 11, 12, 12, 12, 17, 17, 21, 21, 21, 21, 21, 21, 21, 25, 26, 26, 26, 26, 
			26, 26, 26, 26, 26, 26, 26, 26, 27, 29, 30, 30, 30, 30, 30, 31, 34, 34, 34, 34, 34, 34, 34, 41, 42, 42, 44, 45, 45, 45, 45, 51, 
			51, 51, 52, 52, 52, 52, 52, 52, 52, 52, 53, 53, 53, 53, 53, 53, 54, 54, 54, 55, 55};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (272, 4), dGotoEntry (273, 7), dGotoEntry (274, 6), dGotoEntry (275, 5), dGotoEntry (276, 3), 
			dGotoEntry (277, 8), dGotoEntry (274, 10), dGotoEntry (275, 5), dGotoEntry (276, 3), dGotoEntry (277, 8), 
			dGotoEntry (278, 14), dGotoEntry (279, 16), dGotoEntry (277, 22), dGotoEntry (280, 19), dGotoEntry (281, 20), 
			dGotoEntry (282, 18), dGotoEntry (283, 21), dGotoEntry (277, 22), dGotoEntry (281, 23), dGotoEntry (282, 18), 
			dGotoEntry (283, 21), dGotoEntry (286, 37), dGotoEntry (287, 36), dGotoEntry (288, 39), dGotoEntry (290, 38), 
			dGotoEntry (284, 41), dGotoEntry (289, 49), dGotoEntry (291, 51), dGotoEntry (292, 52), dGotoEntry (285, 53), 
			dGotoEntry (284, 55), dGotoEntry (287, 56), dGotoEntry (288, 39), dGotoEntry (290, 38), dGotoEntry (288, 65), 
			dGotoEntry (290, 38), dGotoEntry (295, 62), dGotoEntry (296, 61), dGotoEntry (297, 63), dGotoEntry (298, 64), 
			dGotoEntry (301, 60), dGotoEntry (285, 67), dGotoEntry (293, 71), dGotoEntry (294, 70), dGotoEntry (292, 72), 
			dGotoEntry (288, 65), dGotoEntry (290, 38), dGotoEntry (296, 74), dGotoEntry (297, 63), dGotoEntry (298, 64), 
			dGotoEntry (301, 60), dGotoEntry (299, 77), dGotoEntry (300, 81), dGotoEntry (294, 83), dGotoEntry (300, 84)};
	static dActionEntry actionTable[] = {
			dActionEntry (255, 1, 0, 0, 1), dActionEntry (257, 0, 1, 0, 0), dActionEntry (259, 0, 2, 0, 0), dActionEntry (260, 1, 5, 0, 13), 
			dActionEntry (258, 0, 9, 0, 0), dActionEntry (260, 1, 5, 1, 14), dActionEntry (255, 1, 2, 1, 6), dActionEntry (257, 1, 2, 1, 6), 
			dActionEntry (260, 1, 2, 1, 6), dActionEntry (255, 2, 0, 0, 0), dActionEntry (255, 1, 2, 1, 5), dActionEntry (257, 1, 2, 1, 5), 
			dActionEntry (260, 1, 2, 1, 5), dActionEntry (255, 1, 1, 1, 3), dActionEntry (257, 1, 1, 1, 3), dActionEntry (260, 1, 1, 1, 3), 
			dActionEntry (255, 1, 0, 1, 2), dActionEntry (257, 0, 1, 0, 0), dActionEntry (259, 0, 2, 0, 0), dActionEntry (260, 1, 5, 0, 13), 
			dActionEntry (260, 0, 11, 0, 0), dActionEntry (255, 1, 3, 2, 7), dActionEntry (257, 1, 3, 2, 7), dActionEntry (260, 1, 3, 2, 7), 
			dActionEntry (255, 1, 1, 2, 4), dActionEntry (257, 1, 1, 2, 4), dActionEntry (260, 1, 1, 2, 4), dActionEntry (261, 0, 12, 0, 0), 
			dActionEntry (123, 1, 6, 0, 11), dActionEntry (262, 0, 13, 0, 0), dActionEntry (123, 1, 6, 1, 12), dActionEntry (123, 1, 7, 0, 9), 
			dActionEntry (263, 0, 15, 0, 0), dActionEntry (123, 1, 7, 1, 10), dActionEntry (123, 0, 17, 0, 0), dActionEntry (259, 0, 2, 0, 0), 
			dActionEntry (261, 1, 5, 0, 13), dActionEntry (125, 1, 9, 1, 17), dActionEntry (261, 1, 9, 1, 17), dActionEntry (125, 0, 24, 0, 0), 
			dActionEntry (259, 0, 2, 0, 0), dActionEntry (261, 1, 5, 0, 13), dActionEntry (125, 1, 8, 1, 15), dActionEntry (261, 1, 8, 1, 15), 
			dActionEntry (125, 1, 9, 1, 18), dActionEntry (261, 1, 9, 1, 18), dActionEntry (261, 0, 25, 0, 0), dActionEntry (125, 1, 8, 2, 16), 
			dActionEntry (261, 1, 8, 2, 16), dActionEntry (255, 1, 4, 8, 8), dActionEntry (257, 1, 4, 8, 8), dActionEntry (260, 1, 4, 8, 8), 
			dActionEntry (40, 0, 26, 0, 0), dActionEntry (41, 0, 27, 0, 0), dActionEntry (264, 0, 28, 0, 0), dActionEntry (265, 0, 29, 0, 0), 
			dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 32, 0, 0), dActionEntry (268, 0, 33, 0, 0), dActionEntry (269, 0, 35, 0, 0), 
			dActionEntry (270, 0, 31, 0, 0), dActionEntry (271, 0, 30, 0, 0), dActionEntry (58, 0, 40, 0, 0), dActionEntry (123, 1, 12, 0, 37), 
			dActionEntry (261, 1, 18, 1, 25), dActionEntry (261, 1, 18, 1, 26), dActionEntry (261, 1, 18, 1, 35), dActionEntry (261, 1, 18, 1, 34), 
			dActionEntry (261, 1, 18, 1, 28), dActionEntry (261, 1, 18, 1, 29), dActionEntry (261, 1, 18, 1, 27), dActionEntry (265, 0, 42, 0, 0), 
			dActionEntry (266, 0, 45, 0, 0), dActionEntry (267, 0, 43, 0, 0), dActionEntry (268, 0, 44, 0, 0), dActionEntry (41, 1, 14, 1, 21), 
			dActionEntry (44, 1, 14, 1, 21), dActionEntry (41, 0, 46, 0, 0), dActionEntry (44, 0, 47, 0, 0), dActionEntry (261, 1, 16, 1, 24), 
			dActionEntry (261, 0, 48, 0, 0), dActionEntry (261, 0, 50, 0, 0), dActionEntry (123, 0, 54, 0, 0), dActionEntry (261, 1, 18, 2, 30), 
			dActionEntry (261, 1, 18, 2, 32), dActionEntry (261, 1, 18, 2, 33), dActionEntry (261, 1, 18, 2, 31), dActionEntry (264, 0, 28, 0, 0), 
			dActionEntry (265, 0, 29, 0, 0), dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 32, 0, 0), dActionEntry (268, 0, 33, 0, 0), 
			dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 31, 0, 0), dActionEntry (271, 0, 30, 0, 0), dActionEntry (41, 1, 17, 1, 36), 
			dActionEntry (44, 1, 17, 1, 36), dActionEntry (41, 1, 15, 2, 23), dActionEntry (44, 1, 15, 2, 23), dActionEntry (40, 0, 57, 0, 0), 
			dActionEntry (44, 0, 58, 0, 0), dActionEntry (123, 1, 12, 2, 38), dActionEntry (44, 1, 19, 1, 39), dActionEntry (123, 1, 19, 1, 39), 
			dActionEntry (125, 1, 10, 6, 19), dActionEntry (261, 1, 10, 6, 19), dActionEntry (125, 0, 66, 0, 0), dActionEntry (261, 0, 59, 0, 0), 
			dActionEntry (264, 0, 28, 0, 0), dActionEntry (265, 0, 29, 0, 0), dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 32, 0, 0), 
			dActionEntry (268, 0, 33, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 31, 0, 0), dActionEntry (271, 0, 30, 0, 0), 
			dActionEntry (41, 1, 14, 3, 22), dActionEntry (44, 1, 14, 3, 22), dActionEntry (41, 0, 68, 0, 0), dActionEntry (256, 0, 69, 0, 0), 
			dActionEntry (61, 0, 73, 0, 0), dActionEntry (125, 1, 26, 1, 56), dActionEntry (261, 1, 26, 1, 56), dActionEntry (264, 1, 26, 1, 56), 
			dActionEntry (265, 1, 26, 1, 56), dActionEntry (266, 1, 26, 1, 56), dActionEntry (267, 1, 26, 1, 56), dActionEntry (268, 1, 26, 1, 56), 
			dActionEntry (269, 1, 26, 1, 56), dActionEntry (270, 1, 26, 1, 56), dActionEntry (271, 1, 26, 1, 56), dActionEntry (125, 1, 23, 1, 48), 
			dActionEntry (261, 1, 23, 1, 48), dActionEntry (264, 1, 23, 1, 48), dActionEntry (265, 1, 23, 1, 48), dActionEntry (266, 1, 23, 1, 48), 
			dActionEntry (267, 1, 23, 1, 48), dActionEntry (268, 1, 23, 1, 48), dActionEntry (269, 1, 23, 1, 48), dActionEntry (270, 1, 23, 1, 48), 
			dActionEntry (271, 1, 23, 1, 48), dActionEntry (125, 0, 75, 0, 0), dActionEntry (261, 0, 59, 0, 0), dActionEntry (264, 0, 28, 0, 0), 
			dActionEntry (265, 0, 29, 0, 0), dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 32, 0, 0), dActionEntry (268, 0, 33, 0, 0), 
			dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 31, 0, 0), dActionEntry (271, 0, 30, 0, 0), dActionEntry (125, 1, 24, 1, 50), 
			dActionEntry (261, 1, 24, 1, 50), dActionEntry (264, 1, 24, 1, 50), dActionEntry (265, 1, 24, 1, 50), dActionEntry (266, 1, 24, 1, 50), 
			dActionEntry (267, 1, 24, 1, 50), dActionEntry (268, 1, 24, 1, 50), dActionEntry (269, 1, 24, 1, 50), dActionEntry (270, 1, 24, 1, 50), 
			dActionEntry (271, 1, 24, 1, 50), dActionEntry (125, 1, 24, 1, 51), dActionEntry (261, 1, 24, 1, 51), dActionEntry (264, 1, 24, 1, 51), 
			dActionEntry (265, 1, 24, 1, 51), dActionEntry (266, 1, 24, 1, 51), dActionEntry (267, 1, 24, 1, 51), dActionEntry (268, 1, 24, 1, 51), 
			dActionEntry (269, 1, 24, 1, 51), dActionEntry (270, 1, 24, 1, 51), dActionEntry (271, 1, 24, 1, 51), dActionEntry (261, 0, 76, 0, 0), 
			dActionEntry (125, 1, 13, 2, 46), dActionEntry (261, 1, 13, 2, 46), dActionEntry (125, 1, 10, 7, 20), dActionEntry (261, 1, 10, 7, 20), 
			dActionEntry (44, 1, 20, 3, 41), dActionEntry (123, 1, 20, 3, 41), dActionEntry (41, 1, 22, 1, 45), dActionEntry (44, 1, 22, 1, 45), 
			dActionEntry (41, 1, 21, 1, 43), dActionEntry (44, 1, 21, 1, 43), dActionEntry (41, 0, 78, 0, 0), dActionEntry (44, 0, 79, 0, 0), 
			dActionEntry (44, 1, 19, 3, 40), dActionEntry (123, 1, 19, 3, 40), dActionEntry (256, 0, 80, 0, 0), dActionEntry (125, 1, 23, 2, 49), 
			dActionEntry (261, 1, 23, 2, 49), dActionEntry (264, 1, 23, 2, 49), dActionEntry (265, 1, 23, 2, 49), dActionEntry (266, 1, 23, 2, 49), 
			dActionEntry (267, 1, 23, 2, 49), dActionEntry (268, 1, 23, 2, 49), dActionEntry (269, 1, 23, 2, 49), dActionEntry (270, 1, 23, 2, 49), 
			dActionEntry (271, 1, 23, 2, 49), dActionEntry (125, 1, 13, 3, 47), dActionEntry (261, 1, 13, 3, 47), dActionEntry (61, 1, 27, 1, 54), 
			dActionEntry (125, 1, 27, 1, 54), dActionEntry (261, 1, 27, 1, 54), dActionEntry (264, 1, 27, 1, 54), dActionEntry (265, 1, 27, 1, 54), 
			dActionEntry (266, 1, 27, 1, 54), dActionEntry (267, 1, 27, 1, 54), dActionEntry (268, 1, 27, 1, 54), dActionEntry (269, 1, 27, 1, 54), 
			dActionEntry (270, 1, 27, 1, 54), dActionEntry (271, 1, 27, 1, 54), dActionEntry (61, 0, 82, 0, 0), dActionEntry (125, 1, 25, 2, 52), 
			dActionEntry (261, 1, 25, 2, 52), dActionEntry (264, 1, 25, 2, 52), dActionEntry (265, 1, 25, 2, 52), dActionEntry (266, 1, 25, 2, 52), 
			dActionEntry (267, 1, 25, 2, 52), dActionEntry (268, 1, 25, 2, 52), dActionEntry (269, 1, 25, 2, 52), dActionEntry (270, 1, 25, 2, 52), 
			dActionEntry (271, 1, 25, 2, 52), dActionEntry (44, 1, 20, 4, 42), dActionEntry (123, 1, 20, 4, 42), dActionEntry (256, 0, 69, 0, 0), 
			dActionEntry (125, 1, 28, 1, 55), dActionEntry (261, 1, 28, 1, 55), dActionEntry (264, 1, 28, 1, 55), dActionEntry (265, 1, 28, 1, 55), 
			dActionEntry (266, 1, 28, 1, 55), dActionEntry (267, 1, 28, 1, 55), dActionEntry (268, 1, 28, 1, 55), dActionEntry (269, 1, 28, 1, 55), 
			dActionEntry (270, 1, 28, 1, 55), dActionEntry (271, 1, 28, 1, 55), dActionEntry (125, 1, 29, 3, 57), dActionEntry (261, 1, 29, 3, 57), 
			dActionEntry (264, 1, 29, 3, 57), dActionEntry (265, 1, 29, 3, 57), dActionEntry (266, 1, 29, 3, 57), dActionEntry (267, 1, 29, 3, 57), 
			dActionEntry (268, 1, 29, 3, 57), dActionEntry (269, 1, 29, 3, 57), dActionEntry (270, 1, 29, 3, 57), dActionEntry (271, 1, 29, 3, 57), 
			dActionEntry (41, 1, 21, 3, 44), dActionEntry (44, 1, 21, 3, 44), dActionEntry (125, 1, 25, 4, 53), dActionEntry (261, 1, 25, 4, 53), 
			dActionEntry (264, 1, 25, 4, 53), dActionEntry (265, 1, 25, 4, 53), dActionEntry (266, 1, 25, 4, 53), dActionEntry (267, 1, 25, 4, 53), 
			dActionEntry (268, 1, 25, 4, 53), dActionEntry (269, 1, 25, 4, 53), dActionEntry (270, 1, 25, 4, 53), dActionEntry (271, 1, 25, 4, 53)};

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



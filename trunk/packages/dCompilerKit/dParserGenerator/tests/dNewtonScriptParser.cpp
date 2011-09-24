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


bool dNewtonScriptParser::Parse(xxx& scanner)
{
	dList<dStackPair> stack;
	static short actionsCount[] = {
			4, 1, 1, 3, 1, 1, 3, 3, 4, 3, 1, 3, 2, 1, 2, 1, 1, 1, 1, 2, 2, 2, 2, 9, 
			2, 3, 2, 1, 1, 1, 1, 1, 1, 1, 4, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 8, 2, 
			2, 1, 2, 2, 2, 10, 1, 2, 2, 1, 1, 10, 10, 10, 10, 10, 1, 2, 2, 2, 2, 2, 2, 2, 
			1, 10, 2, 11, 11, 2, 1, 10, 10, 1, 2, 10};
	static short actionsStart[] = {
			0, 4, 5, 6, 9, 10, 11, 14, 17, 21, 24, 25, 28, 30, 31, 33, 34, 35, 36, 37, 39, 41, 43, 45, 
			54, 56, 59, 61, 62, 63, 64, 65, 66, 67, 68, 72, 74, 76, 77, 78, 79, 80, 81, 82, 83, 59, 84, 92, 
			94, 96, 97, 99, 101, 103, 79, 113, 115, 78, 117, 118, 128, 138, 148, 158, 168, 169, 171, 173, 175, 177, 179, 181, 
			183, 184, 194, 196, 207, 218, 220, 221, 231, 183, 241, 243};
	static short gotoCount[] = {
			6, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 1, 0, 1, 0, 0, 4, 0, 0, 3, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 7, 1, 0, 2, 1, 0, 0, 0, 6, 0, 0, 
			1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0};
	static short gotoStart[] = {
			0, 6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10, 11, 11, 12, 12, 12, 16, 16, 16, 19, 19, 19, 23, 23, 23, 24, 24, 24, 24, 24, 
			24, 24, 24, 24, 24, 24, 24, 25, 27, 28, 28, 28, 28, 28, 29, 32, 32, 32, 32, 32, 32, 32, 39, 40, 40, 42, 43, 43, 43, 43, 49, 49, 
			49, 50, 50, 50, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 51, 52, 52, 52, 53, 53};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (272, 5), dGotoEntry (273, 8), dGotoEntry (274, 7), dGotoEntry (275, 6), dGotoEntry (276, 3), 
			dGotoEntry (277, 4), dGotoEntry (274, 11), dGotoEntry (275, 6), dGotoEntry (276, 3), dGotoEntry (277, 4), 
			dGotoEntry (278, 14), dGotoEntry (279, 16), dGotoEntry (280, 20), dGotoEntry (281, 21), dGotoEntry (282, 19), 
			dGotoEntry (283, 22), dGotoEntry (281, 24), dGotoEntry (282, 19), dGotoEntry (283, 22), dGotoEntry (286, 36), 
			dGotoEntry (287, 35), dGotoEntry (288, 38), dGotoEntry (290, 37), dGotoEntry (284, 40), dGotoEntry (289, 48), 
			dGotoEntry (291, 50), dGotoEntry (292, 51), dGotoEntry (285, 52), dGotoEntry (284, 54), dGotoEntry (287, 55), 
			dGotoEntry (288, 38), dGotoEntry (290, 37), dGotoEntry (288, 64), dGotoEntry (290, 37), dGotoEntry (295, 61), 
			dGotoEntry (296, 60), dGotoEntry (297, 62), dGotoEntry (298, 63), dGotoEntry (301, 59), dGotoEntry (285, 66), 
			dGotoEntry (293, 70), dGotoEntry (294, 69), dGotoEntry (292, 71), dGotoEntry (288, 64), dGotoEntry (290, 37), 
			dGotoEntry (296, 73), dGotoEntry (297, 62), dGotoEntry (298, 63), dGotoEntry (301, 59), dGotoEntry (299, 76), 
			dGotoEntry (300, 80), dGotoEntry (294, 82), dGotoEntry (300, 83)};
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
			dActionEntry (261, 1, 9, 1, 18), dActionEntry (41, 0, 26, 0, 0), dActionEntry (264, 0, 27, 0, 0), dActionEntry (265, 0, 28, 0, 0), 
			dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 31, 0, 0), dActionEntry (268, 0, 32, 0, 0), dActionEntry (269, 0, 34, 0, 0), 
			dActionEntry (270, 0, 30, 0, 0), dActionEntry (271, 0, 29, 0, 0), dActionEntry (125, 1, 8, 2, 16), dActionEntry (261, 1, 8, 2, 16), 
			dActionEntry (255, 1, 4, 8, 8), dActionEntry (257, 1, 4, 8, 8), dActionEntry (260, 1, 4, 8, 8), dActionEntry (58, 0, 39, 0, 0), 
			dActionEntry (123, 1, 12, 0, 37), dActionEntry (261, 1, 18, 1, 25), dActionEntry (261, 1, 18, 1, 26), dActionEntry (261, 1, 18, 1, 35), 
			dActionEntry (261, 1, 18, 1, 34), dActionEntry (261, 1, 18, 1, 28), dActionEntry (261, 1, 18, 1, 29), dActionEntry (261, 1, 18, 1, 27), 
			dActionEntry (265, 0, 41, 0, 0), dActionEntry (266, 0, 44, 0, 0), dActionEntry (267, 0, 42, 0, 0), dActionEntry (268, 0, 43, 0, 0), 
			dActionEntry (41, 1, 14, 1, 21), dActionEntry (44, 1, 14, 1, 21), dActionEntry (41, 0, 45, 0, 0), dActionEntry (44, 0, 46, 0, 0), 
			dActionEntry (261, 1, 16, 1, 24), dActionEntry (261, 0, 47, 0, 0), dActionEntry (261, 0, 49, 0, 0), dActionEntry (123, 0, 53, 0, 0), 
			dActionEntry (261, 1, 18, 2, 30), dActionEntry (261, 1, 18, 2, 32), dActionEntry (261, 1, 18, 2, 33), dActionEntry (261, 1, 18, 2, 31), 
			dActionEntry (264, 0, 27, 0, 0), dActionEntry (265, 0, 28, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 31, 0, 0), 
			dActionEntry (268, 0, 32, 0, 0), dActionEntry (269, 0, 34, 0, 0), dActionEntry (270, 0, 30, 0, 0), dActionEntry (271, 0, 29, 0, 0), 
			dActionEntry (41, 1, 17, 1, 36), dActionEntry (44, 1, 17, 1, 36), dActionEntry (41, 1, 15, 2, 23), dActionEntry (44, 1, 15, 2, 23), 
			dActionEntry (40, 0, 56, 0, 0), dActionEntry (44, 0, 57, 0, 0), dActionEntry (123, 1, 12, 2, 38), dActionEntry (44, 1, 19, 1, 39), 
			dActionEntry (123, 1, 19, 1, 39), dActionEntry (125, 1, 10, 5, 19), dActionEntry (261, 1, 10, 5, 19), dActionEntry (125, 0, 65, 0, 0), 
			dActionEntry (261, 0, 58, 0, 0), dActionEntry (264, 0, 27, 0, 0), dActionEntry (265, 0, 28, 0, 0), dActionEntry (266, 0, 33, 0, 0), 
			dActionEntry (267, 0, 31, 0, 0), dActionEntry (268, 0, 32, 0, 0), dActionEntry (269, 0, 34, 0, 0), dActionEntry (270, 0, 30, 0, 0), 
			dActionEntry (271, 0, 29, 0, 0), dActionEntry (41, 1, 14, 3, 22), dActionEntry (44, 1, 14, 3, 22), dActionEntry (41, 0, 67, 0, 0), 
			dActionEntry (256, 0, 68, 0, 0), dActionEntry (61, 0, 72, 0, 0), dActionEntry (125, 1, 26, 1, 56), dActionEntry (261, 1, 26, 1, 56), 
			dActionEntry (264, 1, 26, 1, 56), dActionEntry (265, 1, 26, 1, 56), dActionEntry (266, 1, 26, 1, 56), dActionEntry (267, 1, 26, 1, 56), 
			dActionEntry (268, 1, 26, 1, 56), dActionEntry (269, 1, 26, 1, 56), dActionEntry (270, 1, 26, 1, 56), dActionEntry (271, 1, 26, 1, 56), 
			dActionEntry (125, 1, 23, 1, 48), dActionEntry (261, 1, 23, 1, 48), dActionEntry (264, 1, 23, 1, 48), dActionEntry (265, 1, 23, 1, 48), 
			dActionEntry (266, 1, 23, 1, 48), dActionEntry (267, 1, 23, 1, 48), dActionEntry (268, 1, 23, 1, 48), dActionEntry (269, 1, 23, 1, 48), 
			dActionEntry (270, 1, 23, 1, 48), dActionEntry (271, 1, 23, 1, 48), dActionEntry (125, 0, 74, 0, 0), dActionEntry (261, 0, 58, 0, 0), 
			dActionEntry (264, 0, 27, 0, 0), dActionEntry (265, 0, 28, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 31, 0, 0), 
			dActionEntry (268, 0, 32, 0, 0), dActionEntry (269, 0, 34, 0, 0), dActionEntry (270, 0, 30, 0, 0), dActionEntry (271, 0, 29, 0, 0), 
			dActionEntry (125, 1, 24, 1, 50), dActionEntry (261, 1, 24, 1, 50), dActionEntry (264, 1, 24, 1, 50), dActionEntry (265, 1, 24, 1, 50), 
			dActionEntry (266, 1, 24, 1, 50), dActionEntry (267, 1, 24, 1, 50), dActionEntry (268, 1, 24, 1, 50), dActionEntry (269, 1, 24, 1, 50), 
			dActionEntry (270, 1, 24, 1, 50), dActionEntry (271, 1, 24, 1, 50), dActionEntry (125, 1, 24, 1, 51), dActionEntry (261, 1, 24, 1, 51), 
			dActionEntry (264, 1, 24, 1, 51), dActionEntry (265, 1, 24, 1, 51), dActionEntry (266, 1, 24, 1, 51), dActionEntry (267, 1, 24, 1, 51), 
			dActionEntry (268, 1, 24, 1, 51), dActionEntry (269, 1, 24, 1, 51), dActionEntry (270, 1, 24, 1, 51), dActionEntry (271, 1, 24, 1, 51), 
			dActionEntry (261, 0, 75, 0, 0), dActionEntry (125, 1, 13, 2, 46), dActionEntry (261, 1, 13, 2, 46), dActionEntry (125, 1, 10, 6, 20), 
			dActionEntry (261, 1, 10, 6, 20), dActionEntry (44, 1, 20, 3, 41), dActionEntry (123, 1, 20, 3, 41), dActionEntry (41, 1, 22, 1, 45), 
			dActionEntry (44, 1, 22, 1, 45), dActionEntry (41, 1, 21, 1, 43), dActionEntry (44, 1, 21, 1, 43), dActionEntry (41, 0, 77, 0, 0), 
			dActionEntry (44, 0, 78, 0, 0), dActionEntry (44, 1, 19, 3, 40), dActionEntry (123, 1, 19, 3, 40), dActionEntry (256, 0, 79, 0, 0), 
			dActionEntry (125, 1, 23, 2, 49), dActionEntry (261, 1, 23, 2, 49), dActionEntry (264, 1, 23, 2, 49), dActionEntry (265, 1, 23, 2, 49), 
			dActionEntry (266, 1, 23, 2, 49), dActionEntry (267, 1, 23, 2, 49), dActionEntry (268, 1, 23, 2, 49), dActionEntry (269, 1, 23, 2, 49), 
			dActionEntry (270, 1, 23, 2, 49), dActionEntry (271, 1, 23, 2, 49), dActionEntry (125, 1, 13, 3, 47), dActionEntry (261, 1, 13, 3, 47), 
			dActionEntry (61, 1, 27, 1, 54), dActionEntry (125, 1, 27, 1, 54), dActionEntry (261, 1, 27, 1, 54), dActionEntry (264, 1, 27, 1, 54), 
			dActionEntry (265, 1, 27, 1, 54), dActionEntry (266, 1, 27, 1, 54), dActionEntry (267, 1, 27, 1, 54), dActionEntry (268, 1, 27, 1, 54), 
			dActionEntry (269, 1, 27, 1, 54), dActionEntry (270, 1, 27, 1, 54), dActionEntry (271, 1, 27, 1, 54), dActionEntry (61, 0, 81, 0, 0), 
			dActionEntry (125, 1, 25, 2, 52), dActionEntry (261, 1, 25, 2, 52), dActionEntry (264, 1, 25, 2, 52), dActionEntry (265, 1, 25, 2, 52), 
			dActionEntry (266, 1, 25, 2, 52), dActionEntry (267, 1, 25, 2, 52), dActionEntry (268, 1, 25, 2, 52), dActionEntry (269, 1, 25, 2, 52), 
			dActionEntry (270, 1, 25, 2, 52), dActionEntry (271, 1, 25, 2, 52), dActionEntry (44, 1, 20, 4, 42), dActionEntry (123, 1, 20, 4, 42), 
			dActionEntry (256, 0, 68, 0, 0), dActionEntry (125, 1, 28, 1, 55), dActionEntry (261, 1, 28, 1, 55), dActionEntry (264, 1, 28, 1, 55), 
			dActionEntry (265, 1, 28, 1, 55), dActionEntry (266, 1, 28, 1, 55), dActionEntry (267, 1, 28, 1, 55), dActionEntry (268, 1, 28, 1, 55), 
			dActionEntry (269, 1, 28, 1, 55), dActionEntry (270, 1, 28, 1, 55), dActionEntry (271, 1, 28, 1, 55), dActionEntry (125, 1, 29, 3, 57), 
			dActionEntry (261, 1, 29, 3, 57), dActionEntry (264, 1, 29, 3, 57), dActionEntry (265, 1, 29, 3, 57), dActionEntry (266, 1, 29, 3, 57), 
			dActionEntry (267, 1, 29, 3, 57), dActionEntry (268, 1, 29, 3, 57), dActionEntry (269, 1, 29, 3, 57), dActionEntry (270, 1, 29, 3, 57), 
			dActionEntry (271, 1, 29, 3, 57), dActionEntry (41, 1, 21, 3, 44), dActionEntry (44, 1, 21, 3, 44), dActionEntry (125, 1, 25, 4, 53), 
			dActionEntry (261, 1, 25, 4, 53), dActionEntry (264, 1, 25, 4, 53), dActionEntry (265, 1, 25, 4, 53), dActionEntry (266, 1, 25, 4, 53), 
			dActionEntry (267, 1, 25, 4, 53), dActionEntry (268, 1, 25, 4, 53), dActionEntry (269, 1, 25, 4, 53), dActionEntry (270, 1, 25, 4, 53), 
			dActionEntry (271, 1, 25, 4, 53)};

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



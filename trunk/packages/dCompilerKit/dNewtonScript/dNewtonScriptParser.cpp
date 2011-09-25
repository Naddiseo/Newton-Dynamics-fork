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
			4, 1, 1, 3, 1, 1, 3, 3, 4, 3, 1, 3, 2, 1, 2, 1, 1, 1, 12, 2, 2, 2, 2, 2, 
			2, 10, 10, 2, 4, 12, 12, 12, 12, 1, 12, 1, 1, 9, 2, 1, 12, 9, 9, 1, 1, 2, 2, 2, 
			2, 10, 12, 3, 1, 12, 11, 1, 12, 1, 2, 2, 2, 13, 12, 13, 9, 1, 8, 11, 11, 11, 11, 11, 
			11, 1, 12, 12, 9, 1, 1, 2, 1, 2, 9, 1, 2, 2, 1, 1, 1, 8, 11, 1, 12, 12, 12, 1, 
			1, 9, 2, 1, 2, 8, 2, 11, 11, 12, 1, 12, 11, 12, 1, 2, 1, 1, 2, 12, 13, 1, 1, 1, 
			2, 2, 12, 12, 12, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2};
	static short actionsStart[] = {
			0, 4, 5, 6, 9, 10, 11, 14, 17, 21, 24, 25, 28, 30, 31, 33, 34, 35, 36, 48, 50, 52, 54, 56, 
			58, 60, 70, 80, 82, 86, 98, 110, 122, 134, 135, 147, 148, 149, 158, 160, 161, 173, 182, 148, 191, 192, 194, 196, 
			198, 70, 200, 212, 215, 216, 228, 239, 240, 252, 253, 255, 257, 259, 272, 284, 297, 306, 307, 315, 326, 337, 348, 359, 
			370, 381, 382, 394, 297, 406, 407, 408, 410, 411, 297, 413, 414, 416, 418, 419, 420, 307, 421, 432, 382, 433, 445, 419, 
			457, 297, 458, 460, 461, 307, 463, 465, 476, 487, 499, 445, 500, 511, 523, 524, 526, 148, 527, 487, 529, 542, 543, 544, 
			545, 547, 549, 561, 573, 585, 526, 587, 589, 591, 593, 595, 597, 599, 600};
	static short gotoCount[] = {
			6, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 1, 0, 1, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 
			10, 0, 0, 0, 1, 3, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 9, 0, 0, 0, 1, 0, 1, 0, 0, 0, 
			5, 0, 2, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 1, 
			0, 5, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 
			0, 0, 0, 0, 0, 1, 0};
	static short gotoStart[] = {
			0, 6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10, 11, 11, 12, 12, 12, 12, 24, 24, 24, 24, 24, 24, 24, 24, 27, 27, 27, 27, 27, 
			27, 37, 37, 37, 37, 38, 41, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 47, 47, 47, 47, 47, 56, 56, 56, 56, 57, 57, 58, 58, 58, 
			58, 63, 63, 65, 65, 65, 73, 73, 73, 73, 73, 73, 73, 78, 78, 78, 78, 78, 78, 83, 83, 83, 83, 83, 84, 84, 86, 86, 86, 86, 86, 86, 
			87, 87, 92, 92, 92, 93, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 97, 99, 100, 100, 100, 101, 101, 101, 101, 101, 101, 101, 101, 101, 103, 104, 
			104, 104, 104, 104, 104, 104, 105};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (274, 5), dGotoEntry (275, 8), dGotoEntry (276, 7), dGotoEntry (277, 6), dGotoEntry (278, 3), 
			dGotoEntry (279, 4), dGotoEntry (276, 11), dGotoEntry (277, 6), dGotoEntry (278, 3), dGotoEntry (279, 4), 
			dGotoEntry (280, 14), dGotoEntry (281, 17), dGotoEntry (279, 35), dGotoEntry (282, 33), dGotoEntry (283, 32), 
			dGotoEntry (284, 31), dGotoEntry (285, 29), dGotoEntry (286, 34), dGotoEntry (287, 30), dGotoEntry (288, 37), 
			dGotoEntry (295, 36), dGotoEntry (296, 39), dGotoEntry (297, 40), dGotoEntry (300, 38), dGotoEntry (295, 43), 
			dGotoEntry (296, 44), dGotoEntry (300, 38), dGotoEntry (279, 35), dGotoEntry (284, 50), dGotoEntry (285, 29), 
			dGotoEntry (286, 34), dGotoEntry (287, 30), dGotoEntry (288, 37), dGotoEntry (295, 36), dGotoEntry (296, 39), 
			dGotoEntry (297, 40), dGotoEntry (300, 38), dGotoEntry (294, 53), dGotoEntry (295, 56), dGotoEntry (296, 57), 
			dGotoEntry (300, 38), dGotoEntry (301, 60), dGotoEntry (302, 59), dGotoEntry (294, 62), dGotoEntry (295, 43), 
			dGotoEntry (296, 44), dGotoEntry (300, 38), dGotoEntry (296, 73), dGotoEntry (297, 74), dGotoEntry (298, 70), 
			dGotoEntry (300, 38), dGotoEntry (307, 69), dGotoEntry (308, 68), dGotoEntry (309, 71), dGotoEntry (310, 72), 
			dGotoEntry (311, 67), dGotoEntry (299, 80), dGotoEntry (302, 81), dGotoEntry (289, 85), dGotoEntry (290, 84), 
			dGotoEntry (291, 83), dGotoEntry (296, 86), dGotoEntry (300, 38), dGotoEntry (296, 88), dGotoEntry (300, 38), 
			dGotoEntry (296, 91), dGotoEntry (297, 92), dGotoEntry (298, 70), dGotoEntry (300, 38), dGotoEntry (308, 90), 
			dGotoEntry (309, 71), dGotoEntry (310, 72), dGotoEntry (311, 67), dGotoEntry (289, 85), dGotoEntry (290, 84), 
			dGotoEntry (291, 96), dGotoEntry (296, 86), dGotoEntry (300, 38), dGotoEntry (289, 85), dGotoEntry (290, 84), 
			dGotoEntry (291, 99), dGotoEntry (296, 86), dGotoEntry (300, 38), dGotoEntry (299, 104), dGotoEntry (296, 106), 
			dGotoEntry (300, 38), dGotoEntry (299, 108), dGotoEntry (289, 85), dGotoEntry (290, 84), dGotoEntry (291, 110), 
			dGotoEntry (296, 86), dGotoEntry (300, 38), dGotoEntry (293, 113), dGotoEntry (290, 114), dGotoEntry (296, 86), 
			dGotoEntry (300, 38), dGotoEntry (292, 118), dGotoEntry (303, 120), dGotoEntry (304, 121), dGotoEntry (294, 122), 
			dGotoEntry (292, 124), dGotoEntry (305, 130), dGotoEntry (306, 129), dGotoEntry (304, 131), dGotoEntry (306, 134)};
	static dActionEntry actionTable[] = {
			dActionEntry (255, 1, 0, 0, 1), dActionEntry (259, 0, 1, 0, 0), dActionEntry (261, 0, 2, 0, 0), dActionEntry (262, 1, 5, 0, 9), 
			dActionEntry (260, 0, 9, 0, 0), dActionEntry (262, 1, 5, 1, 10), dActionEntry (255, 1, 2, 1, 6), dActionEntry (259, 1, 2, 1, 6), 
			dActionEntry (262, 1, 2, 1, 6), dActionEntry (262, 0, 10, 0, 0), dActionEntry (255, 2, 0, 0, 0), dActionEntry (255, 1, 2, 1, 5), 
			dActionEntry (259, 1, 2, 1, 5), dActionEntry (262, 1, 2, 1, 5), dActionEntry (255, 1, 1, 1, 3), dActionEntry (259, 1, 1, 1, 3), 
			dActionEntry (262, 1, 1, 1, 3), dActionEntry (255, 1, 0, 1, 2), dActionEntry (259, 0, 1, 0, 0), dActionEntry (261, 0, 2, 0, 0), 
			dActionEntry (262, 1, 5, 0, 9), dActionEntry (255, 1, 3, 2, 7), dActionEntry (259, 1, 3, 2, 7), dActionEntry (262, 1, 3, 2, 7), 
			dActionEntry (263, 0, 12, 0, 0), dActionEntry (255, 1, 1, 2, 4), dActionEntry (259, 1, 1, 2, 4), dActionEntry (262, 1, 1, 2, 4), 
			dActionEntry (123, 1, 6, 0, 13), dActionEntry (264, 0, 13, 0, 0), dActionEntry (263, 0, 15, 0, 0), dActionEntry (123, 1, 7, 0, 11), 
			dActionEntry (265, 0, 16, 0, 0), dActionEntry (123, 1, 6, 2, 14), dActionEntry (123, 1, 7, 1, 12), dActionEntry (123, 0, 18, 0, 0), 
			dActionEntry (125, 1, 8, 0, 15), dActionEntry (256, 0, 25, 0, 0), dActionEntry (261, 0, 26, 0, 0), dActionEntry (263, 1, 5, 0, 9), 
			dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), 
			dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), 
			dActionEntry (91, 1, 26, 1, 44), dActionEntry (263, 1, 26, 1, 44), dActionEntry (91, 1, 26, 1, 45), dActionEntry (263, 1, 26, 1, 45), 
			dActionEntry (91, 1, 26, 1, 54), dActionEntry (263, 1, 26, 1, 54), dActionEntry (91, 1, 26, 1, 53), dActionEntry (263, 1, 26, 1, 53), 
			dActionEntry (91, 1, 26, 1, 47), dActionEntry (263, 1, 26, 1, 47), dActionEntry (91, 1, 26, 1, 48), dActionEntry (263, 1, 26, 1, 48), 
			dActionEntry (261, 0, 41, 0, 0), dActionEntry (263, 1, 14, 1, 22), dActionEntry (266, 1, 14, 1, 22), dActionEntry (267, 1, 14, 1, 22), 
			dActionEntry (268, 1, 14, 1, 22), dActionEntry (269, 1, 14, 1, 22), dActionEntry (270, 1, 14, 1, 22), dActionEntry (271, 1, 14, 1, 22), 
			dActionEntry (272, 1, 14, 1, 22), dActionEntry (273, 1, 14, 1, 22), dActionEntry (256, 0, 42, 0, 0), dActionEntry (263, 1, 5, 1, 10), 
			dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), 
			dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), 
			dActionEntry (91, 1, 26, 1, 46), dActionEntry (263, 1, 26, 1, 46), dActionEntry (267, 0, 45, 0, 0), dActionEntry (268, 0, 48, 0, 0), 
			dActionEntry (269, 0, 46, 0, 0), dActionEntry (270, 0, 47, 0, 0), dActionEntry (125, 1, 10, 1, 19), dActionEntry (256, 1, 10, 1, 19), 
			dActionEntry (261, 1, 10, 1, 19), dActionEntry (263, 1, 10, 1, 19), dActionEntry (266, 1, 10, 1, 19), dActionEntry (267, 1, 10, 1, 19), 
			dActionEntry (268, 1, 10, 1, 19), dActionEntry (269, 1, 10, 1, 19), dActionEntry (270, 1, 10, 1, 19), dActionEntry (271, 1, 10, 1, 19), 
			dActionEntry (272, 1, 10, 1, 19), dActionEntry (273, 1, 10, 1, 19), dActionEntry (125, 1, 10, 1, 21), dActionEntry (256, 1, 10, 1, 21), 
			dActionEntry (261, 1, 10, 1, 21), dActionEntry (263, 1, 10, 1, 21), dActionEntry (266, 1, 10, 1, 21), dActionEntry (267, 1, 10, 1, 21), 
			dActionEntry (268, 1, 10, 1, 21), dActionEntry (269, 1, 10, 1, 21), dActionEntry (270, 1, 10, 1, 21), dActionEntry (271, 1, 10, 1, 21), 
			dActionEntry (272, 1, 10, 1, 21), dActionEntry (273, 1, 10, 1, 21), dActionEntry (125, 1, 9, 1, 17), dActionEntry (256, 1, 9, 1, 17), 
			dActionEntry (261, 1, 9, 1, 17), dActionEntry (263, 1, 9, 1, 17), dActionEntry (266, 1, 9, 1, 17), dActionEntry (267, 1, 9, 1, 17), 
			dActionEntry (268, 1, 9, 1, 17), dActionEntry (269, 1, 9, 1, 17), dActionEntry (270, 1, 9, 1, 17), dActionEntry (271, 1, 9, 1, 17), 
			dActionEntry (272, 1, 9, 1, 17), dActionEntry (273, 1, 9, 1, 17), dActionEntry (125, 1, 8, 1, 16), dActionEntry (256, 0, 25, 0, 0), 
			dActionEntry (261, 0, 49, 0, 0), dActionEntry (263, 1, 5, 0, 9), dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), 
			dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), dActionEntry (125, 0, 51, 0, 0), dActionEntry (125, 1, 10, 1, 20), 
			dActionEntry (256, 1, 10, 1, 20), dActionEntry (261, 1, 10, 1, 20), dActionEntry (263, 1, 10, 1, 20), dActionEntry (266, 1, 10, 1, 20), 
			dActionEntry (267, 1, 10, 1, 20), dActionEntry (268, 1, 10, 1, 20), dActionEntry (269, 1, 10, 1, 20), dActionEntry (270, 1, 10, 1, 20), 
			dActionEntry (271, 1, 10, 1, 20), dActionEntry (272, 1, 10, 1, 20), dActionEntry (273, 1, 10, 1, 20), dActionEntry (263, 0, 52, 0, 0), 
			dActionEntry (123, 0, 54, 0, 0), dActionEntry (263, 0, 55, 0, 0), dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), 
			dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), dActionEntry (91, 0, 58, 0, 0), dActionEntry (263, 1, 22, 1, 42), 
			dActionEntry (263, 0, 61, 0, 0), dActionEntry (125, 1, 12, 1, 40), dActionEntry (256, 1, 12, 1, 40), dActionEntry (261, 1, 12, 1, 40), 
			dActionEntry (263, 1, 12, 1, 40), dActionEntry (266, 1, 12, 1, 40), dActionEntry (267, 1, 12, 1, 40), dActionEntry (268, 1, 12, 1, 40), 
			dActionEntry (269, 1, 12, 1, 40), dActionEntry (270, 1, 12, 1, 40), dActionEntry (271, 1, 12, 1, 40), dActionEntry (272, 1, 12, 1, 40), 
			dActionEntry (273, 1, 12, 1, 40), dActionEntry (263, 1, 14, 2, 23), dActionEntry (266, 1, 14, 2, 23), dActionEntry (267, 1, 14, 2, 23), 
			dActionEntry (268, 1, 14, 2, 23), dActionEntry (269, 1, 14, 2, 23), dActionEntry (270, 1, 14, 2, 23), dActionEntry (271, 1, 14, 2, 23), 
			dActionEntry (272, 1, 14, 2, 23), dActionEntry (273, 1, 14, 2, 23), dActionEntry (263, 1, 14, 2, 24), dActionEntry (266, 1, 14, 2, 24), 
			dActionEntry (267, 1, 14, 2, 24), dActionEntry (268, 1, 14, 2, 24), dActionEntry (269, 1, 14, 2, 24), dActionEntry (270, 1, 14, 2, 24), 
			dActionEntry (271, 1, 14, 2, 24), dActionEntry (272, 1, 14, 2, 24), dActionEntry (273, 1, 14, 2, 24), dActionEntry (263, 0, 63, 0, 0), 
			dActionEntry (91, 1, 26, 2, 49), dActionEntry (263, 1, 26, 2, 49), dActionEntry (91, 1, 26, 2, 51), dActionEntry (263, 1, 26, 2, 51), 
			dActionEntry (91, 1, 26, 2, 52), dActionEntry (263, 1, 26, 2, 52), dActionEntry (91, 1, 26, 2, 50), dActionEntry (263, 1, 26, 2, 50), 
			dActionEntry (125, 1, 9, 2, 18), dActionEntry (256, 1, 9, 2, 18), dActionEntry (261, 1, 9, 2, 18), dActionEntry (263, 1, 9, 2, 18), 
			dActionEntry (266, 1, 9, 2, 18), dActionEntry (267, 1, 9, 2, 18), dActionEntry (268, 1, 9, 2, 18), dActionEntry (269, 1, 9, 2, 18), 
			dActionEntry (270, 1, 9, 2, 18), dActionEntry (271, 1, 9, 2, 18), dActionEntry (272, 1, 9, 2, 18), dActionEntry (273, 1, 9, 2, 18), 
			dActionEntry (255, 1, 4, 8, 8), dActionEntry (259, 1, 4, 8, 8), dActionEntry (262, 1, 4, 8, 8), dActionEntry (40, 0, 64, 0, 0), 
			dActionEntry (125, 1, 13, 2, 34), dActionEntry (256, 1, 13, 2, 34), dActionEntry (261, 1, 13, 2, 34), dActionEntry (263, 1, 13, 2, 34), 
			dActionEntry (266, 1, 13, 2, 34), dActionEntry (267, 1, 13, 2, 34), dActionEntry (268, 1, 13, 2, 34), dActionEntry (269, 1, 13, 2, 34), 
			dActionEntry (270, 1, 13, 2, 34), dActionEntry (271, 1, 13, 2, 34), dActionEntry (272, 1, 13, 2, 34), dActionEntry (273, 1, 13, 2, 34), 
			dActionEntry (125, 0, 75, 0, 0), dActionEntry (261, 0, 66, 0, 0), dActionEntry (263, 0, 65, 0, 0), dActionEntry (266, 0, 19, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), 
			dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), dActionEntry (40, 0, 76, 0, 0), 
			dActionEntry (125, 1, 13, 2, 33), dActionEntry (256, 1, 13, 2, 33), dActionEntry (261, 1, 13, 2, 33), dActionEntry (263, 1, 13, 2, 33), 
			dActionEntry (266, 1, 13, 2, 33), dActionEntry (267, 1, 13, 2, 33), dActionEntry (268, 1, 13, 2, 33), dActionEntry (269, 1, 13, 2, 33), 
			dActionEntry (270, 1, 13, 2, 33), dActionEntry (271, 1, 13, 2, 33), dActionEntry (272, 1, 13, 2, 33), dActionEntry (273, 1, 13, 2, 33), 
			dActionEntry (263, 0, 77, 0, 0), dActionEntry (93, 0, 79, 0, 0), dActionEntry (258, 0, 78, 0, 0), dActionEntry (91, 1, 27, 1, 55), 
			dActionEntry (263, 1, 27, 1, 55), dActionEntry (91, 0, 58, 0, 0), dActionEntry (263, 1, 22, 2, 43), dActionEntry (40, 0, 82, 0, 0), 
			dActionEntry (125, 1, 23, 2, 37), dActionEntry (256, 1, 23, 2, 37), dActionEntry (261, 1, 23, 2, 37), dActionEntry (263, 1, 23, 2, 37), 
			dActionEntry (266, 1, 23, 2, 37), dActionEntry (267, 1, 23, 2, 37), dActionEntry (268, 1, 23, 2, 37), dActionEntry (269, 1, 23, 2, 37), 
			dActionEntry (270, 1, 23, 2, 37), dActionEntry (271, 1, 23, 2, 37), dActionEntry (272, 1, 23, 2, 37), dActionEntry (273, 1, 23, 2, 37), 
			dActionEntry (125, 1, 13, 3, 35), dActionEntry (256, 1, 13, 3, 35), dActionEntry (261, 1, 13, 3, 35), dActionEntry (263, 1, 13, 3, 35), 
			dActionEntry (266, 1, 13, 3, 35), dActionEntry (267, 1, 13, 3, 35), dActionEntry (268, 1, 13, 3, 35), dActionEntry (269, 1, 13, 3, 35), 
			dActionEntry (270, 1, 13, 3, 35), dActionEntry (271, 1, 13, 3, 35), dActionEntry (272, 1, 13, 3, 35), dActionEntry (273, 1, 13, 3, 35), 
			dActionEntry (40, 0, 82, 0, 0), dActionEntry (125, 1, 23, 3, 38), dActionEntry (256, 1, 23, 3, 38), dActionEntry (261, 1, 23, 3, 38), 
			dActionEntry (263, 1, 23, 3, 38), dActionEntry (266, 1, 23, 3, 38), dActionEntry (267, 1, 23, 3, 38), dActionEntry (268, 1, 23, 3, 38), 
			dActionEntry (269, 1, 23, 3, 38), dActionEntry (270, 1, 23, 3, 38), dActionEntry (271, 1, 23, 3, 38), dActionEntry (272, 1, 23, 3, 38), 
			dActionEntry (273, 1, 23, 3, 38), dActionEntry (41, 1, 17, 0, 27), dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), 
			dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), dActionEntry (61, 0, 87, 0, 0), dActionEntry (266, 0, 19, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), 
			dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), dActionEntry (125, 1, 36, 1, 77), 
			dActionEntry (261, 1, 36, 1, 77), dActionEntry (263, 1, 36, 1, 77), dActionEntry (266, 1, 36, 1, 77), dActionEntry (267, 1, 36, 1, 77), 
			dActionEntry (268, 1, 36, 1, 77), dActionEntry (269, 1, 36, 1, 77), dActionEntry (270, 1, 36, 1, 77), dActionEntry (271, 1, 36, 1, 77), 
			dActionEntry (272, 1, 36, 1, 77), dActionEntry (273, 1, 36, 1, 77), dActionEntry (125, 1, 33, 1, 70), dActionEntry (261, 1, 33, 1, 70), 
			dActionEntry (263, 1, 33, 1, 70), dActionEntry (266, 1, 33, 1, 70), dActionEntry (267, 1, 33, 1, 70), dActionEntry (268, 1, 33, 1, 70), 
			dActionEntry (269, 1, 33, 1, 70), dActionEntry (270, 1, 33, 1, 70), dActionEntry (271, 1, 33, 1, 70), dActionEntry (272, 1, 33, 1, 70), 
			dActionEntry (273, 1, 33, 1, 70), dActionEntry (125, 0, 93, 0, 0), dActionEntry (261, 0, 89, 0, 0), dActionEntry (263, 0, 65, 0, 0), 
			dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), 
			dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), 
			dActionEntry (125, 1, 35, 1, 75), dActionEntry (261, 1, 35, 1, 75), dActionEntry (263, 1, 35, 1, 75), dActionEntry (266, 1, 35, 1, 75), 
			dActionEntry (267, 1, 35, 1, 75), dActionEntry (268, 1, 35, 1, 75), dActionEntry (269, 1, 35, 1, 75), dActionEntry (270, 1, 35, 1, 75), 
			dActionEntry (271, 1, 35, 1, 75), dActionEntry (272, 1, 35, 1, 75), dActionEntry (273, 1, 35, 1, 75), dActionEntry (125, 1, 34, 1, 72), 
			dActionEntry (261, 1, 34, 1, 72), dActionEntry (263, 1, 34, 1, 72), dActionEntry (266, 1, 34, 1, 72), dActionEntry (267, 1, 34, 1, 72), 
			dActionEntry (268, 1, 34, 1, 72), dActionEntry (269, 1, 34, 1, 72), dActionEntry (270, 1, 34, 1, 72), dActionEntry (271, 1, 34, 1, 72), 
			dActionEntry (272, 1, 34, 1, 72), dActionEntry (273, 1, 34, 1, 72), dActionEntry (125, 1, 34, 1, 73), dActionEntry (261, 1, 34, 1, 73), 
			dActionEntry (263, 1, 34, 1, 73), dActionEntry (266, 1, 34, 1, 73), dActionEntry (267, 1, 34, 1, 73), dActionEntry (268, 1, 34, 1, 73), 
			dActionEntry (269, 1, 34, 1, 73), dActionEntry (270, 1, 34, 1, 73), dActionEntry (271, 1, 34, 1, 73), dActionEntry (272, 1, 34, 1, 73), 
			dActionEntry (273, 1, 34, 1, 73), dActionEntry (263, 0, 94, 0, 0), dActionEntry (61, 0, 95, 0, 0), dActionEntry (125, 1, 35, 1, 74), 
			dActionEntry (261, 1, 35, 1, 74), dActionEntry (263, 1, 35, 1, 74), dActionEntry (266, 1, 35, 1, 74), dActionEntry (267, 1, 35, 1, 74), 
			dActionEntry (268, 1, 35, 1, 74), dActionEntry (269, 1, 35, 1, 74), dActionEntry (270, 1, 35, 1, 74), dActionEntry (271, 1, 35, 1, 74), 
			dActionEntry (272, 1, 35, 1, 74), dActionEntry (273, 1, 35, 1, 74), dActionEntry (125, 1, 20, 2, 68), dActionEntry (256, 1, 20, 2, 68), 
			dActionEntry (261, 1, 20, 2, 68), dActionEntry (263, 1, 20, 2, 68), dActionEntry (266, 1, 20, 2, 68), dActionEntry (267, 1, 20, 2, 68), 
			dActionEntry (268, 1, 20, 2, 68), dActionEntry (269, 1, 20, 2, 68), dActionEntry (270, 1, 20, 2, 68), dActionEntry (271, 1, 20, 2, 68), 
			dActionEntry (272, 1, 20, 2, 68), dActionEntry (273, 1, 20, 2, 68), dActionEntry (40, 0, 97, 0, 0), dActionEntry (93, 1, 25, 1, 76), 
			dActionEntry (91, 1, 28, 2, 57), dActionEntry (263, 1, 28, 2, 57), dActionEntry (93, 0, 98, 0, 0), dActionEntry (91, 1, 27, 2, 56), 
			dActionEntry (263, 1, 27, 2, 56), dActionEntry (41, 0, 100, 0, 0), dActionEntry (41, 1, 15, 1, 25), dActionEntry (44, 1, 15, 1, 25), 
			dActionEntry (41, 1, 17, 1, 28), dActionEntry (44, 0, 101, 0, 0), dActionEntry (263, 0, 102, 0, 0), dActionEntry (258, 0, 103, 0, 0), 
			dActionEntry (263, 0, 105, 0, 0), dActionEntry (125, 1, 33, 2, 71), dActionEntry (261, 1, 33, 2, 71), dActionEntry (263, 1, 33, 2, 71), 
			dActionEntry (266, 1, 33, 2, 71), dActionEntry (267, 1, 33, 2, 71), dActionEntry (268, 1, 33, 2, 71), dActionEntry (269, 1, 33, 2, 71), 
			dActionEntry (270, 1, 33, 2, 71), dActionEntry (271, 1, 33, 2, 71), dActionEntry (272, 1, 33, 2, 71), dActionEntry (273, 1, 33, 2, 71), 
			dActionEntry (263, 0, 107, 0, 0), dActionEntry (125, 1, 20, 3, 69), dActionEntry (256, 1, 20, 3, 69), dActionEntry (261, 1, 20, 3, 69), 
			dActionEntry (263, 1, 20, 3, 69), dActionEntry (266, 1, 20, 3, 69), dActionEntry (267, 1, 20, 3, 69), dActionEntry (268, 1, 20, 3, 69), 
			dActionEntry (269, 1, 20, 3, 69), dActionEntry (270, 1, 20, 3, 69), dActionEntry (271, 1, 20, 3, 69), dActionEntry (272, 1, 20, 3, 69), 
			dActionEntry (273, 1, 20, 3, 69), dActionEntry (61, 1, 23, 2, 37), dActionEntry (125, 1, 23, 2, 37), dActionEntry (261, 1, 23, 2, 37), 
			dActionEntry (263, 1, 23, 2, 37), dActionEntry (266, 1, 23, 2, 37), dActionEntry (267, 1, 23, 2, 37), dActionEntry (268, 1, 23, 2, 37), 
			dActionEntry (269, 1, 23, 2, 37), dActionEntry (270, 1, 23, 2, 37), dActionEntry (271, 1, 23, 2, 37), dActionEntry (272, 1, 23, 2, 37), 
			dActionEntry (273, 1, 23, 2, 37), dActionEntry (41, 0, 109, 0, 0), dActionEntry (91, 1, 28, 3, 58), dActionEntry (263, 1, 28, 3, 58), 
			dActionEntry (41, 0, 111, 0, 0), dActionEntry (58, 0, 112, 0, 0), dActionEntry (123, 1, 19, 0, 59), dActionEntry (41, 1, 16, 2, 41), 
			dActionEntry (44, 1, 16, 2, 41), dActionEntry (125, 1, 25, 1, 76), dActionEntry (261, 1, 25, 1, 76), dActionEntry (263, 1, 25, 1, 76), 
			dActionEntry (266, 1, 25, 1, 76), dActionEntry (267, 1, 25, 1, 76), dActionEntry (268, 1, 25, 1, 76), dActionEntry (269, 1, 25, 1, 76), 
			dActionEntry (270, 1, 25, 1, 76), dActionEntry (271, 1, 25, 1, 76), dActionEntry (272, 1, 25, 1, 76), dActionEntry (273, 1, 25, 1, 76), 
			dActionEntry (125, 1, 37, 3, 78), dActionEntry (261, 1, 37, 3, 78), dActionEntry (263, 1, 37, 3, 78), dActionEntry (266, 1, 37, 3, 78), 
			dActionEntry (267, 1, 37, 3, 78), dActionEntry (268, 1, 37, 3, 78), dActionEntry (269, 1, 37, 3, 78), dActionEntry (270, 1, 37, 3, 78), 
			dActionEntry (271, 1, 37, 3, 78), dActionEntry (272, 1, 37, 3, 78), dActionEntry (273, 1, 37, 3, 78), dActionEntry (61, 1, 23, 3, 38), 
			dActionEntry (125, 1, 23, 3, 38), dActionEntry (261, 1, 23, 3, 38), dActionEntry (263, 1, 23, 3, 38), dActionEntry (266, 1, 23, 3, 38), 
			dActionEntry (267, 1, 23, 3, 38), dActionEntry (268, 1, 23, 3, 38), dActionEntry (269, 1, 23, 3, 38), dActionEntry (270, 1, 23, 3, 38), 
			dActionEntry (271, 1, 23, 3, 38), dActionEntry (272, 1, 23, 3, 38), dActionEntry (273, 1, 23, 3, 38), dActionEntry (263, 0, 115, 0, 0), 
			dActionEntry (125, 1, 24, 3, 39), dActionEntry (261, 1, 24, 3, 39), dActionEntry (263, 1, 24, 3, 39), dActionEntry (266, 1, 24, 3, 39), 
			dActionEntry (267, 1, 24, 3, 39), dActionEntry (268, 1, 24, 3, 39), dActionEntry (269, 1, 24, 3, 39), dActionEntry (270, 1, 24, 3, 39), 
			dActionEntry (271, 1, 24, 3, 39), dActionEntry (272, 1, 24, 3, 39), dActionEntry (273, 1, 24, 3, 39), dActionEntry (125, 1, 11, 5, 31), 
			dActionEntry (256, 1, 11, 5, 31), dActionEntry (261, 1, 11, 5, 31), dActionEntry (263, 1, 11, 5, 31), dActionEntry (266, 1, 11, 5, 31), 
			dActionEntry (267, 1, 11, 5, 31), dActionEntry (268, 1, 11, 5, 31), dActionEntry (269, 1, 11, 5, 31), dActionEntry (270, 1, 11, 5, 31), 
			dActionEntry (271, 1, 11, 5, 31), dActionEntry (272, 1, 11, 5, 31), dActionEntry (273, 1, 11, 5, 31), dActionEntry (41, 0, 116, 0, 0), 
			dActionEntry (123, 1, 18, 0, 29), dActionEntry (257, 0, 117, 0, 0), dActionEntry (263, 0, 119, 0, 0), dActionEntry (41, 1, 15, 3, 26), 
			dActionEntry (44, 1, 15, 3, 26), dActionEntry (125, 1, 18, 0, 29), dActionEntry (256, 1, 18, 0, 29), dActionEntry (257, 0, 123, 0, 0), 
			dActionEntry (261, 1, 18, 0, 29), dActionEntry (263, 1, 18, 0, 29), dActionEntry (266, 1, 18, 0, 29), dActionEntry (267, 1, 18, 0, 29), 
			dActionEntry (268, 1, 18, 0, 29), dActionEntry (269, 1, 18, 0, 29), dActionEntry (270, 1, 18, 0, 29), dActionEntry (271, 1, 18, 0, 29), 
			dActionEntry (272, 1, 18, 0, 29), dActionEntry (273, 1, 18, 0, 29), dActionEntry (123, 1, 18, 1, 30), dActionEntry (123, 1, 21, 6, 36), 
			dActionEntry (40, 0, 125, 0, 0), dActionEntry (44, 0, 126, 0, 0), dActionEntry (123, 1, 19, 2, 60), dActionEntry (44, 1, 29, 1, 61), 
			dActionEntry (123, 1, 29, 1, 61), dActionEntry (125, 1, 11, 7, 32), dActionEntry (256, 1, 11, 7, 32), dActionEntry (261, 1, 11, 7, 32), 
			dActionEntry (263, 1, 11, 7, 32), dActionEntry (266, 1, 11, 7, 32), dActionEntry (267, 1, 11, 7, 32), dActionEntry (268, 1, 11, 7, 32), 
			dActionEntry (269, 1, 11, 7, 32), dActionEntry (270, 1, 11, 7, 32), dActionEntry (271, 1, 11, 7, 32), dActionEntry (272, 1, 11, 7, 32), 
			dActionEntry (273, 1, 11, 7, 32), dActionEntry (125, 1, 18, 1, 30), dActionEntry (256, 1, 18, 1, 30), dActionEntry (261, 1, 18, 1, 30), 
			dActionEntry (263, 1, 18, 1, 30), dActionEntry (266, 1, 18, 1, 30), dActionEntry (267, 1, 18, 1, 30), dActionEntry (268, 1, 18, 1, 30), 
			dActionEntry (269, 1, 18, 1, 30), dActionEntry (270, 1, 18, 1, 30), dActionEntry (271, 1, 18, 1, 30), dActionEntry (272, 1, 18, 1, 30), 
			dActionEntry (273, 1, 18, 1, 30), dActionEntry (125, 1, 21, 6, 36), dActionEntry (256, 1, 21, 6, 36), dActionEntry (261, 1, 21, 6, 36), 
			dActionEntry (263, 1, 21, 6, 36), dActionEntry (266, 1, 21, 6, 36), dActionEntry (267, 1, 21, 6, 36), dActionEntry (268, 1, 21, 6, 36), 
			dActionEntry (269, 1, 21, 6, 36), dActionEntry (270, 1, 21, 6, 36), dActionEntry (271, 1, 21, 6, 36), dActionEntry (272, 1, 21, 6, 36), 
			dActionEntry (273, 1, 21, 6, 36), dActionEntry (41, 0, 127, 0, 0), dActionEntry (258, 0, 128, 0, 0), dActionEntry (44, 1, 30, 3, 63), 
			dActionEntry (123, 1, 30, 3, 63), dActionEntry (41, 1, 32, 1, 67), dActionEntry (44, 1, 32, 1, 67), dActionEntry (41, 1, 31, 1, 65), 
			dActionEntry (44, 1, 31, 1, 65), dActionEntry (41, 0, 132, 0, 0), dActionEntry (44, 0, 133, 0, 0), dActionEntry (44, 1, 29, 3, 62), 
			dActionEntry (123, 1, 29, 3, 62), dActionEntry (44, 1, 30, 4, 64), dActionEntry (123, 1, 30, 4, 64), dActionEntry (258, 0, 128, 0, 0), 
			dActionEntry (41, 1, 31, 3, 66), dActionEntry (44, 1, 31, 3, 66)};

	const int lastToken = 274;

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



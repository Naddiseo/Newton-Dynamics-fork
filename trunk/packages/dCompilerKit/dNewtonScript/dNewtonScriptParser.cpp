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
			4, 1, 1, 1, 4, 1, 4, 4, 4, 2, 4, 1, 4, 1, 2, 2, 1, 1, 1, 2, 12, 1, 2, 2, 
			2, 2, 1, 2, 2, 2, 10, 2, 4, 12, 12, 12, 12, 1, 2, 1, 12, 4, 12, 9, 1, 1, 1, 1, 
			2, 2, 2, 2, 10, 12, 4, 1, 2, 2, 2, 12, 12, 4, 1, 2, 2, 1, 9, 12, 9, 1, 2, 1, 
			2, 4, 2, 8, 2, 1, 1, 2, 1, 1, 2, 2, 12, 1, 2, 2, 12, 11, 1, 2, 1, 1, 8, 11, 
			11, 11, 11, 11, 11, 12, 12, 12, 2, 2, 2, 2, 2, 1, 11, 12, 12, 1, 2, 1, 11, 11, 11, 2};
	static short actionsStart[] = {
			0, 4, 5, 6, 7, 11, 12, 16, 20, 24, 26, 30, 31, 35, 36, 24, 38, 39, 40, 36, 41, 53, 54, 56, 
			58, 60, 62, 63, 65, 67, 69, 79, 81, 85, 97, 109, 121, 133, 134, 136, 137, 149, 153, 165, 174, 175, 176, 177, 
			178, 180, 182, 184, 69, 186, 198, 202, 203, 205, 207, 209, 221, 233, 237, 238, 240, 242, 165, 243, 165, 255, 256, 258, 
			259, 261, 265, 267, 275, 277, 278, 279, 281, 282, 283, 265, 285, 297, 298, 300, 302, 314, 282, 325, 281, 327, 267, 328, 
			339, 350, 361, 372, 383, 394, 406, 418, 430, 432, 434, 436, 438, 440, 441, 394, 452, 440, 464, 466, 467, 478, 489, 500};
	static short gotoCount[] = {
			5, 0, 0, 0, 0, 0, 0, 0, 3, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 
			0, 0, 7, 0, 0, 0, 2, 0, 0, 0, 8, 5, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 1, 0, 7, 0, 0, 0, 
			0, 0, 5, 0, 5, 0, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 2, 1, 0, 1, 0, 0, 0, 0, 0, 9, 1, 2, 1, 0, 2, 0, 
			0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0};
	static short gotoStart[] = {
			0, 5, 5, 5, 5, 5, 5, 5, 5, 8, 9, 9, 9, 9, 9, 10, 11, 11, 11, 11, 12, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 22, 
			22, 22, 22, 29, 29, 29, 29, 31, 31, 31, 31, 39, 44, 44, 44, 44, 44, 44, 44, 44, 44, 46, 46, 46, 46, 47, 47, 48, 48, 55, 55, 55, 
			55, 55, 55, 60, 60, 65, 65, 65, 65, 65, 65, 66, 69, 69, 69, 69, 69, 71, 72, 72, 73, 73, 73, 73, 73, 73, 82, 83, 85, 86, 86, 88, 
			88, 88, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 97, 97, 97, 97, 98, 98, 99, 99, 99, 99};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (273, 5), dGotoEntry (274, 8), dGotoEntry (275, 7), dGotoEntry (276, 6), dGotoEntry (277, 4), 
			dGotoEntry (275, 12), dGotoEntry (276, 6), dGotoEntry (277, 4), dGotoEntry (278, 14), dGotoEntry (279, 18), 
			dGotoEntry (278, 19), dGotoEntry (279, 21), dGotoEntry (280, 34), dGotoEntry (282, 35), dGotoEntry (283, 33), 
			dGotoEntry (284, 36), dGotoEntry (285, 37), dGotoEntry (291, 40), dGotoEntry (292, 39), dGotoEntry (295, 38), 
			dGotoEntry (292, 47), dGotoEntry (295, 38), dGotoEntry (282, 53), dGotoEntry (283, 33), dGotoEntry (284, 36), 
			dGotoEntry (285, 37), dGotoEntry (291, 40), dGotoEntry (292, 39), dGotoEntry (295, 38), dGotoEntry (296, 58), 
			dGotoEntry (297, 57), dGotoEntry (280, 60), dGotoEntry (282, 35), dGotoEntry (283, 33), dGotoEntry (284, 36), 
			dGotoEntry (285, 37), dGotoEntry (291, 40), dGotoEntry (292, 39), dGotoEntry (295, 38), dGotoEntry (286, 64), 
			dGotoEntry (287, 63), dGotoEntry (288, 62), dGotoEntry (292, 65), dGotoEntry (295, 38), dGotoEntry (292, 47), 
			dGotoEntry (295, 38), dGotoEntry (294, 71), dGotoEntry (297, 72), dGotoEntry (282, 53), dGotoEntry (283, 33), 
			dGotoEntry (284, 36), dGotoEntry (285, 37), dGotoEntry (291, 40), dGotoEntry (292, 39), dGotoEntry (295, 38), 
			dGotoEntry (286, 64), dGotoEntry (287, 63), dGotoEntry (288, 77), dGotoEntry (292, 65), dGotoEntry (295, 38), 
			dGotoEntry (286, 64), dGotoEntry (287, 63), dGotoEntry (288, 78), dGotoEntry (292, 65), dGotoEntry (295, 38), 
			dGotoEntry (289, 81), dGotoEntry (287, 82), dGotoEntry (292, 65), dGotoEntry (295, 38), dGotoEntry (298, 86), 
			dGotoEntry (299, 87), dGotoEntry (290, 88), dGotoEntry (289, 90), dGotoEntry (291, 101), dGotoEntry (292, 39), 
			dGotoEntry (293, 98), dGotoEntry (295, 38), dGotoEntry (302, 97), dGotoEntry (303, 96), dGotoEntry (304, 99), 
			dGotoEntry (305, 100), dGotoEntry (306, 95), dGotoEntry (290, 103), dGotoEntry (300, 107), dGotoEntry (301, 106), 
			dGotoEntry (299, 108), dGotoEntry (292, 47), dGotoEntry (295, 38), dGotoEntry (291, 111), dGotoEntry (292, 39), 
			dGotoEntry (293, 98), dGotoEntry (295, 38), dGotoEntry (303, 110), dGotoEntry (304, 99), dGotoEntry (305, 100), 
			dGotoEntry (306, 95), dGotoEntry (294, 117), dGotoEntry (294, 118), dGotoEntry (301, 119)};
	static dActionEntry actionTable[] = {
			dActionEntry (255, 1, 0, 0, 1), dActionEntry (258, 0, 2, 0, 0), dActionEntry (260, 0, 3, 0, 0), dActionEntry (261, 0, 1, 0, 0), 
			dActionEntry (262, 0, 9, 0, 0), dActionEntry (259, 0, 10, 0, 0), dActionEntry (261, 0, 11, 0, 0), dActionEntry (255, 1, 2, 1, 6), 
			dActionEntry (258, 1, 2, 1, 6), dActionEntry (260, 1, 2, 1, 6), dActionEntry (261, 1, 2, 1, 6), dActionEntry (255, 2, 0, 0, 0), 
			dActionEntry (255, 1, 2, 1, 5), dActionEntry (258, 1, 2, 1, 5), dActionEntry (260, 1, 2, 1, 5), dActionEntry (261, 1, 2, 1, 5), 
			dActionEntry (255, 1, 1, 1, 3), dActionEntry (258, 1, 1, 1, 3), dActionEntry (260, 1, 1, 1, 3), dActionEntry (261, 1, 1, 1, 3), 
			dActionEntry (255, 1, 0, 1, 2), dActionEntry (258, 0, 2, 0, 0), dActionEntry (260, 0, 3, 0, 0), dActionEntry (261, 0, 1, 0, 0), 
			dActionEntry (123, 1, 5, 0, 14), dActionEntry (263, 0, 13, 0, 0), dActionEntry (255, 1, 3, 2, 7), dActionEntry (258, 1, 3, 2, 7), 
			dActionEntry (260, 1, 3, 2, 7), dActionEntry (261, 1, 3, 2, 7), dActionEntry (262, 0, 15, 0, 0), dActionEntry (255, 1, 1, 2, 4), 
			dActionEntry (258, 1, 1, 2, 4), dActionEntry (260, 1, 1, 2, 4), dActionEntry (261, 1, 1, 2, 4), dActionEntry (262, 0, 16, 0, 0), 
			dActionEntry (123, 1, 6, 0, 12), dActionEntry (264, 0, 17, 0, 0), dActionEntry (123, 1, 5, 2, 15), dActionEntry (123, 1, 6, 1, 13), 
			dActionEntry (123, 0, 20, 0, 0), dActionEntry (125, 0, 41, 0, 0), dActionEntry (256, 0, 29, 0, 0), dActionEntry (260, 0, 30, 0, 0), 
			dActionEntry (262, 0, 26, 0, 0), dActionEntry (265, 0, 22, 0, 0), dActionEntry (266, 0, 23, 0, 0), dActionEntry (267, 0, 31, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 28, 0, 0), dActionEntry (270, 0, 32, 0, 0), dActionEntry (271, 0, 25, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (123, 0, 42, 0, 0), dActionEntry (91, 1, 22, 1, 41), dActionEntry (262, 1, 22, 1, 41), 
			dActionEntry (91, 1, 22, 1, 42), dActionEntry (262, 1, 22, 1, 42), dActionEntry (91, 1, 22, 1, 51), dActionEntry (262, 1, 22, 1, 51), 
			dActionEntry (91, 1, 22, 1, 50), dActionEntry (262, 1, 22, 1, 50), dActionEntry (40, 0, 43, 0, 0), dActionEntry (91, 1, 22, 1, 44), 
			dActionEntry (262, 1, 22, 1, 44), dActionEntry (91, 1, 22, 1, 45), dActionEntry (262, 1, 22, 1, 45), dActionEntry (260, 0, 44, 0, 0), 
			dActionEntry (262, 1, 12, 1, 24), dActionEntry (256, 0, 46, 0, 0), dActionEntry (262, 0, 45, 0, 0), dActionEntry (265, 0, 22, 0, 0), 
			dActionEntry (266, 0, 23, 0, 0), dActionEntry (267, 0, 31, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 28, 0, 0), 
			dActionEntry (270, 0, 32, 0, 0), dActionEntry (271, 0, 25, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (91, 1, 22, 1, 43), 
			dActionEntry (262, 1, 22, 1, 43), dActionEntry (266, 0, 48, 0, 0), dActionEntry (267, 0, 51, 0, 0), dActionEntry (268, 0, 49, 0, 0), 
			dActionEntry (269, 0, 50, 0, 0), dActionEntry (125, 1, 9, 1, 22), dActionEntry (256, 1, 9, 1, 22), dActionEntry (260, 1, 9, 1, 22), 
			dActionEntry (262, 1, 9, 1, 22), dActionEntry (265, 1, 9, 1, 22), dActionEntry (266, 1, 9, 1, 22), dActionEntry (267, 1, 9, 1, 22), 
			dActionEntry (268, 1, 9, 1, 22), dActionEntry (269, 1, 9, 1, 22), dActionEntry (270, 1, 9, 1, 22), dActionEntry (271, 1, 9, 1, 22), 
			dActionEntry (272, 1, 9, 1, 22), dActionEntry (125, 0, 54, 0, 0), dActionEntry (256, 0, 29, 0, 0), dActionEntry (260, 0, 52, 0, 0), 
			dActionEntry (262, 0, 26, 0, 0), dActionEntry (265, 0, 22, 0, 0), dActionEntry (266, 0, 23, 0, 0), dActionEntry (267, 0, 31, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 28, 0, 0), dActionEntry (270, 0, 32, 0, 0), dActionEntry (271, 0, 25, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (125, 1, 7, 1, 20), dActionEntry (256, 1, 7, 1, 20), dActionEntry (260, 1, 7, 1, 20), 
			dActionEntry (262, 1, 7, 1, 20), dActionEntry (265, 1, 7, 1, 20), dActionEntry (266, 1, 7, 1, 20), dActionEntry (267, 1, 7, 1, 20), 
			dActionEntry (268, 1, 7, 1, 20), dActionEntry (269, 1, 7, 1, 20), dActionEntry (270, 1, 7, 1, 20), dActionEntry (271, 1, 7, 1, 20), 
			dActionEntry (272, 1, 7, 1, 20), dActionEntry (125, 1, 9, 1, 23), dActionEntry (256, 1, 9, 1, 23), dActionEntry (260, 1, 9, 1, 23), 
			dActionEntry (262, 1, 9, 1, 23), dActionEntry (265, 1, 9, 1, 23), dActionEntry (266, 1, 9, 1, 23), dActionEntry (267, 1, 9, 1, 23), 
			dActionEntry (268, 1, 9, 1, 23), dActionEntry (269, 1, 9, 1, 23), dActionEntry (270, 1, 9, 1, 23), dActionEntry (271, 1, 9, 1, 23), 
			dActionEntry (272, 1, 9, 1, 23), dActionEntry (262, 0, 55, 0, 0), dActionEntry (91, 0, 56, 0, 0), dActionEntry (262, 1, 19, 1, 39), 
			dActionEntry (262, 0, 59, 0, 0), dActionEntry (125, 1, 11, 1, 37), dActionEntry (256, 1, 11, 1, 37), dActionEntry (260, 1, 11, 1, 37), 
			dActionEntry (262, 1, 11, 1, 37), dActionEntry (265, 1, 11, 1, 37), dActionEntry (266, 1, 11, 1, 37), dActionEntry (267, 1, 11, 1, 37), 
			dActionEntry (268, 1, 11, 1, 37), dActionEntry (269, 1, 11, 1, 37), dActionEntry (270, 1, 11, 1, 37), dActionEntry (271, 1, 11, 1, 37), 
			dActionEntry (272, 1, 11, 1, 37), dActionEntry (255, 1, 4, 6, 8), dActionEntry (258, 1, 4, 6, 8), dActionEntry (260, 1, 4, 6, 8), 
			dActionEntry (261, 1, 4, 6, 8), dActionEntry (125, 0, 61, 0, 0), dActionEntry (256, 0, 29, 0, 0), dActionEntry (260, 0, 30, 0, 0), 
			dActionEntry (262, 0, 26, 0, 0), dActionEntry (265, 0, 22, 0, 0), dActionEntry (266, 0, 23, 0, 0), dActionEntry (267, 0, 31, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 28, 0, 0), dActionEntry (270, 0, 32, 0, 0), dActionEntry (271, 0, 25, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (41, 1, 15, 0, 29), dActionEntry (265, 0, 22, 0, 0), dActionEntry (266, 0, 23, 0, 0), 
			dActionEntry (267, 0, 31, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 28, 0, 0), dActionEntry (270, 0, 32, 0, 0), 
			dActionEntry (271, 0, 25, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (262, 1, 12, 2, 25), dActionEntry (40, 0, 66, 0, 0), 
			dActionEntry (262, 1, 12, 2, 26), dActionEntry (262, 0, 67, 0, 0), dActionEntry (91, 1, 22, 2, 46), dActionEntry (262, 1, 22, 2, 46), 
			dActionEntry (91, 1, 22, 2, 48), dActionEntry (262, 1, 22, 2, 48), dActionEntry (91, 1, 22, 2, 49), dActionEntry (262, 1, 22, 2, 49), 
			dActionEntry (91, 1, 22, 2, 47), dActionEntry (262, 1, 22, 2, 47), dActionEntry (125, 1, 7, 2, 21), dActionEntry (256, 1, 7, 2, 21), 
			dActionEntry (260, 1, 7, 2, 21), dActionEntry (262, 1, 7, 2, 21), dActionEntry (265, 1, 7, 2, 21), dActionEntry (266, 1, 7, 2, 21), 
			dActionEntry (267, 1, 7, 2, 21), dActionEntry (268, 1, 7, 2, 21), dActionEntry (269, 1, 7, 2, 21), dActionEntry (270, 1, 7, 2, 21), 
			dActionEntry (271, 1, 7, 2, 21), dActionEntry (272, 1, 7, 2, 21), dActionEntry (255, 1, 4, 7, 9), dActionEntry (258, 1, 4, 7, 9), 
			dActionEntry (260, 1, 4, 7, 9), dActionEntry (261, 1, 4, 7, 9), dActionEntry (40, 0, 68, 0, 0), dActionEntry (93, 0, 70, 0, 0), 
			dActionEntry (257, 0, 69, 0, 0), dActionEntry (91, 1, 23, 1, 52), dActionEntry (262, 1, 23, 1, 52), dActionEntry (91, 0, 56, 0, 0), 
			dActionEntry (262, 1, 19, 2, 40), dActionEntry (125, 1, 18, 2, 34), dActionEntry (256, 1, 18, 2, 34), dActionEntry (260, 1, 18, 2, 34), 
			dActionEntry (262, 1, 18, 2, 34), dActionEntry (265, 1, 18, 2, 34), dActionEntry (266, 1, 18, 2, 34), dActionEntry (267, 1, 18, 2, 34), 
			dActionEntry (268, 1, 18, 2, 34), dActionEntry (269, 1, 18, 2, 34), dActionEntry (270, 1, 18, 2, 34), dActionEntry (271, 1, 18, 2, 34), 
			dActionEntry (272, 1, 18, 2, 34), dActionEntry (125, 0, 73, 0, 0), dActionEntry (256, 0, 29, 0, 0), dActionEntry (260, 0, 52, 0, 0), 
			dActionEntry (262, 0, 26, 0, 0), dActionEntry (265, 0, 22, 0, 0), dActionEntry (266, 0, 23, 0, 0), dActionEntry (267, 0, 31, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 28, 0, 0), dActionEntry (270, 0, 32, 0, 0), dActionEntry (271, 0, 25, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (255, 1, 4, 7, 10), dActionEntry (258, 1, 4, 7, 10), dActionEntry (260, 1, 4, 7, 10), 
			dActionEntry (261, 1, 4, 7, 10), dActionEntry (41, 0, 74, 0, 0), dActionEntry (41, 1, 13, 1, 27), dActionEntry (44, 1, 13, 1, 27), 
			dActionEntry (41, 1, 15, 1, 30), dActionEntry (44, 0, 75, 0, 0), dActionEntry (262, 0, 76, 0, 0), dActionEntry (125, 1, 18, 3, 35), 
			dActionEntry (256, 1, 18, 3, 35), dActionEntry (260, 1, 18, 3, 35), dActionEntry (262, 1, 18, 3, 35), dActionEntry (265, 1, 18, 3, 35), 
			dActionEntry (266, 1, 18, 3, 35), dActionEntry (267, 1, 18, 3, 35), dActionEntry (268, 1, 18, 3, 35), dActionEntry (269, 1, 18, 3, 35), 
			dActionEntry (270, 1, 18, 3, 35), dActionEntry (271, 1, 18, 3, 35), dActionEntry (272, 1, 18, 3, 35), dActionEntry (93, 1, 21, 1, 73), 
			dActionEntry (91, 1, 24, 2, 54), dActionEntry (262, 1, 24, 2, 54), dActionEntry (93, 0, 79, 0, 0), dActionEntry (91, 1, 23, 2, 53), 
			dActionEntry (262, 1, 23, 2, 53), dActionEntry (255, 1, 4, 8, 11), dActionEntry (258, 1, 4, 8, 11), dActionEntry (260, 1, 4, 8, 11), 
			dActionEntry (261, 1, 4, 8, 11), dActionEntry (58, 0, 80, 0, 0), dActionEntry (123, 1, 16, 0, 56), dActionEntry (265, 0, 22, 0, 0), 
			dActionEntry (266, 0, 23, 0, 0), dActionEntry (267, 0, 31, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 28, 0, 0), 
			dActionEntry (270, 0, 32, 0, 0), dActionEntry (271, 0, 25, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (41, 1, 14, 2, 38), 
			dActionEntry (44, 1, 14, 2, 38), dActionEntry (41, 0, 83, 0, 0), dActionEntry (41, 0, 84, 0, 0), dActionEntry (91, 1, 24, 3, 55), 
			dActionEntry (262, 1, 24, 3, 55), dActionEntry (262, 0, 85, 0, 0), dActionEntry (123, 0, 89, 0, 0), dActionEntry (41, 1, 13, 3, 28), 
			dActionEntry (44, 1, 13, 3, 28), dActionEntry (125, 1, 10, 5, 31), dActionEntry (256, 1, 10, 5, 31), dActionEntry (260, 1, 10, 5, 31), 
			dActionEntry (262, 1, 10, 5, 31), dActionEntry (265, 1, 10, 5, 31), dActionEntry (266, 1, 10, 5, 31), dActionEntry (267, 1, 10, 5, 31), 
			dActionEntry (268, 1, 10, 5, 31), dActionEntry (269, 1, 10, 5, 31), dActionEntry (270, 1, 10, 5, 31), dActionEntry (271, 1, 10, 5, 31), 
			dActionEntry (272, 1, 10, 5, 31), dActionEntry (40, 0, 91, 0, 0), dActionEntry (44, 0, 92, 0, 0), dActionEntry (123, 1, 16, 2, 57), 
			dActionEntry (44, 1, 25, 1, 58), dActionEntry (123, 1, 25, 1, 58), dActionEntry (125, 1, 10, 6, 32), dActionEntry (256, 1, 10, 6, 32), 
			dActionEntry (260, 1, 10, 6, 32), dActionEntry (262, 1, 10, 6, 32), dActionEntry (265, 1, 10, 6, 32), dActionEntry (266, 1, 10, 6, 32), 
			dActionEntry (267, 1, 10, 6, 32), dActionEntry (268, 1, 10, 6, 32), dActionEntry (269, 1, 10, 6, 32), dActionEntry (270, 1, 10, 6, 32), 
			dActionEntry (271, 1, 10, 6, 32), dActionEntry (272, 1, 10, 6, 32), dActionEntry (125, 0, 102, 0, 0), dActionEntry (260, 0, 94, 0, 0), 
			dActionEntry (262, 0, 93, 0, 0), dActionEntry (265, 0, 22, 0, 0), dActionEntry (266, 0, 23, 0, 0), dActionEntry (267, 0, 31, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 28, 0, 0), dActionEntry (270, 0, 32, 0, 0), dActionEntry (271, 0, 25, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (41, 0, 104, 0, 0), dActionEntry (257, 0, 105, 0, 0), dActionEntry (61, 0, 109, 0, 0), 
			dActionEntry (125, 1, 32, 1, 74), dActionEntry (260, 1, 32, 1, 74), dActionEntry (262, 1, 32, 1, 74), dActionEntry (265, 1, 32, 1, 74), 
			dActionEntry (266, 1, 32, 1, 74), dActionEntry (267, 1, 32, 1, 74), dActionEntry (268, 1, 32, 1, 74), dActionEntry (269, 1, 32, 1, 74), 
			dActionEntry (270, 1, 32, 1, 74), dActionEntry (271, 1, 32, 1, 74), dActionEntry (272, 1, 32, 1, 74), dActionEntry (125, 1, 29, 1, 67), 
			dActionEntry (260, 1, 29, 1, 67), dActionEntry (262, 1, 29, 1, 67), dActionEntry (265, 1, 29, 1, 67), dActionEntry (266, 1, 29, 1, 67), 
			dActionEntry (267, 1, 29, 1, 67), dActionEntry (268, 1, 29, 1, 67), dActionEntry (269, 1, 29, 1, 67), dActionEntry (270, 1, 29, 1, 67), 
			dActionEntry (271, 1, 29, 1, 67), dActionEntry (272, 1, 29, 1, 67), dActionEntry (125, 0, 112, 0, 0), dActionEntry (260, 0, 94, 0, 0), 
			dActionEntry (262, 0, 93, 0, 0), dActionEntry (265, 0, 22, 0, 0), dActionEntry (266, 0, 23, 0, 0), dActionEntry (267, 0, 31, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 28, 0, 0), dActionEntry (270, 0, 32, 0, 0), dActionEntry (271, 0, 25, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (125, 1, 31, 1, 72), dActionEntry (260, 1, 31, 1, 72), dActionEntry (262, 1, 31, 1, 72), 
			dActionEntry (265, 1, 31, 1, 72), dActionEntry (266, 1, 31, 1, 72), dActionEntry (267, 1, 31, 1, 72), dActionEntry (268, 1, 31, 1, 72), 
			dActionEntry (269, 1, 31, 1, 72), dActionEntry (270, 1, 31, 1, 72), dActionEntry (271, 1, 31, 1, 72), dActionEntry (272, 1, 31, 1, 72), 
			dActionEntry (125, 1, 30, 1, 69), dActionEntry (260, 1, 30, 1, 69), dActionEntry (262, 1, 30, 1, 69), dActionEntry (265, 1, 30, 1, 69), 
			dActionEntry (266, 1, 30, 1, 69), dActionEntry (267, 1, 30, 1, 69), dActionEntry (268, 1, 30, 1, 69), dActionEntry (269, 1, 30, 1, 69), 
			dActionEntry (270, 1, 30, 1, 69), dActionEntry (271, 1, 30, 1, 69), dActionEntry (272, 1, 30, 1, 69), dActionEntry (125, 1, 30, 1, 70), 
			dActionEntry (260, 1, 30, 1, 70), dActionEntry (262, 1, 30, 1, 70), dActionEntry (265, 1, 30, 1, 70), dActionEntry (266, 1, 30, 1, 70), 
			dActionEntry (267, 1, 30, 1, 70), dActionEntry (268, 1, 30, 1, 70), dActionEntry (269, 1, 30, 1, 70), dActionEntry (270, 1, 30, 1, 70), 
			dActionEntry (271, 1, 30, 1, 70), dActionEntry (272, 1, 30, 1, 70), dActionEntry (61, 0, 113, 0, 0), dActionEntry (125, 1, 31, 1, 71), 
			dActionEntry (260, 1, 31, 1, 71), dActionEntry (262, 1, 31, 1, 71), dActionEntry (265, 1, 31, 1, 71), dActionEntry (266, 1, 31, 1, 71), 
			dActionEntry (267, 1, 31, 1, 71), dActionEntry (268, 1, 31, 1, 71), dActionEntry (269, 1, 31, 1, 71), dActionEntry (270, 1, 31, 1, 71), 
			dActionEntry (271, 1, 31, 1, 71), dActionEntry (272, 1, 31, 1, 71), dActionEntry (125, 1, 17, 2, 65), dActionEntry (256, 1, 17, 2, 65), 
			dActionEntry (260, 1, 17, 2, 65), dActionEntry (262, 1, 17, 2, 65), dActionEntry (265, 1, 17, 2, 65), dActionEntry (266, 1, 17, 2, 65), 
			dActionEntry (267, 1, 17, 2, 65), dActionEntry (268, 1, 17, 2, 65), dActionEntry (269, 1, 17, 2, 65), dActionEntry (270, 1, 17, 2, 65), 
			dActionEntry (271, 1, 17, 2, 65), dActionEntry (272, 1, 17, 2, 65), dActionEntry (125, 1, 10, 7, 33), dActionEntry (256, 1, 10, 7, 33), 
			dActionEntry (260, 1, 10, 7, 33), dActionEntry (262, 1, 10, 7, 33), dActionEntry (265, 1, 10, 7, 33), dActionEntry (266, 1, 10, 7, 33), 
			dActionEntry (267, 1, 10, 7, 33), dActionEntry (268, 1, 10, 7, 33), dActionEntry (269, 1, 10, 7, 33), dActionEntry (270, 1, 10, 7, 33), 
			dActionEntry (271, 1, 10, 7, 33), dActionEntry (272, 1, 10, 7, 33), dActionEntry (44, 1, 26, 3, 60), dActionEntry (123, 1, 26, 3, 60), 
			dActionEntry (41, 1, 28, 1, 64), dActionEntry (44, 1, 28, 1, 64), dActionEntry (41, 1, 27, 1, 62), dActionEntry (44, 1, 27, 1, 62), 
			dActionEntry (41, 0, 114, 0, 0), dActionEntry (44, 0, 115, 0, 0), dActionEntry (44, 1, 25, 3, 59), dActionEntry (123, 1, 25, 3, 59), 
			dActionEntry (257, 0, 116, 0, 0), dActionEntry (125, 1, 29, 2, 68), dActionEntry (260, 1, 29, 2, 68), dActionEntry (262, 1, 29, 2, 68), 
			dActionEntry (265, 1, 29, 2, 68), dActionEntry (266, 1, 29, 2, 68), dActionEntry (267, 1, 29, 2, 68), dActionEntry (268, 1, 29, 2, 68), 
			dActionEntry (269, 1, 29, 2, 68), dActionEntry (270, 1, 29, 2, 68), dActionEntry (271, 1, 29, 2, 68), dActionEntry (272, 1, 29, 2, 68), 
			dActionEntry (125, 1, 17, 3, 66), dActionEntry (256, 1, 17, 3, 66), dActionEntry (260, 1, 17, 3, 66), dActionEntry (262, 1, 17, 3, 66), 
			dActionEntry (265, 1, 17, 3, 66), dActionEntry (266, 1, 17, 3, 66), dActionEntry (267, 1, 17, 3, 66), dActionEntry (268, 1, 17, 3, 66), 
			dActionEntry (269, 1, 17, 3, 66), dActionEntry (270, 1, 17, 3, 66), dActionEntry (271, 1, 17, 3, 66), dActionEntry (272, 1, 17, 3, 66), 
			dActionEntry (44, 1, 26, 4, 61), dActionEntry (123, 1, 26, 4, 61), dActionEntry (257, 0, 105, 0, 0), dActionEntry (125, 1, 21, 1, 73), 
			dActionEntry (260, 1, 21, 1, 73), dActionEntry (262, 1, 21, 1, 73), dActionEntry (265, 1, 21, 1, 73), dActionEntry (266, 1, 21, 1, 73), 
			dActionEntry (267, 1, 21, 1, 73), dActionEntry (268, 1, 21, 1, 73), dActionEntry (269, 1, 21, 1, 73), dActionEntry (270, 1, 21, 1, 73), 
			dActionEntry (271, 1, 21, 1, 73), dActionEntry (272, 1, 21, 1, 73), dActionEntry (125, 1, 33, 3, 75), dActionEntry (260, 1, 33, 3, 75), 
			dActionEntry (262, 1, 33, 3, 75), dActionEntry (265, 1, 33, 3, 75), dActionEntry (266, 1, 33, 3, 75), dActionEntry (267, 1, 33, 3, 75), 
			dActionEntry (268, 1, 33, 3, 75), dActionEntry (269, 1, 33, 3, 75), dActionEntry (270, 1, 33, 3, 75), dActionEntry (271, 1, 33, 3, 75), 
			dActionEntry (272, 1, 33, 3, 75), dActionEntry (125, 1, 20, 3, 36), dActionEntry (260, 1, 20, 3, 36), dActionEntry (262, 1, 20, 3, 36), 
			dActionEntry (265, 1, 20, 3, 36), dActionEntry (266, 1, 20, 3, 36), dActionEntry (267, 1, 20, 3, 36), dActionEntry (268, 1, 20, 3, 36), 
			dActionEntry (269, 1, 20, 3, 36), dActionEntry (270, 1, 20, 3, 36), dActionEntry (271, 1, 20, 3, 36), dActionEntry (272, 1, 20, 3, 36), 
			dActionEntry (41, 1, 27, 3, 63), dActionEntry (44, 1, 27, 3, 63)};

	const int lastToken = 273;

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



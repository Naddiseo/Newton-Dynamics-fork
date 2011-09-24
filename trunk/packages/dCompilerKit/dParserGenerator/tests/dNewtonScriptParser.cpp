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
			4, 1, 1, 1, 4, 1, 4, 4, 4, 2, 4, 1, 4, 1, 2, 2, 1, 1, 1, 2, 12, 1, 2, 2, 
			2, 2, 1, 2, 2, 10, 10, 2, 4, 12, 12, 12, 12, 12, 9, 2, 1, 12, 4, 12, 9, 9, 1, 9, 
			1, 2, 2, 2, 2, 10, 12, 1, 4, 1, 1, 2, 2, 2, 13, 12, 4, 1, 2, 2, 1, 9, 13, 1, 
			13, 9, 2, 1, 2, 1, 2, 9, 4, 2, 8, 2, 1, 9, 13, 1, 1, 1, 2, 1, 1, 1, 2, 2, 
			1, 12, 9, 2, 1, 2, 2, 12, 11, 1, 2, 1, 1, 2, 1, 1, 8, 11, 11, 11, 11, 11, 11, 1, 
			12, 12, 12, 1, 12, 12, 2, 2, 2, 2, 2, 1, 1, 11, 12, 12, 12, 1, 12, 2, 1, 11, 11, 12, 
			11, 2};
	static short actionsStart[] = {
			0, 4, 5, 6, 7, 11, 12, 16, 20, 24, 26, 30, 31, 35, 36, 24, 38, 39, 40, 36, 41, 53, 54, 56, 
			58, 60, 62, 63, 65, 67, 77, 87, 89, 93, 105, 117, 129, 141, 153, 162, 164, 165, 177, 181, 193, 202, 211, 212, 
			221, 222, 224, 226, 228, 77, 230, 242, 243, 247, 248, 249, 251, 253, 255, 268, 280, 284, 285, 287, 289, 193, 290, 303, 
			255, 193, 304, 306, 307, 309, 310, 193, 312, 316, 318, 326, 328, 193, 290, 329, 330, 331, 332, 334, 335, 336, 337, 316, 
			339, 340, 193, 352, 354, 355, 357, 359, 371, 336, 352, 382, 336, 383, 335, 385, 318, 386, 397, 408, 419, 430, 441, 452, 
			453, 465, 477, 336, 489, 501, 513, 515, 517, 519, 521, 523, 524, 525, 453, 536, 548, 523, 560, 572, 574, 575, 586, 597, 
			609, 620};
	static short gotoCount[] = {
			5, 0, 0, 0, 0, 0, 0, 0, 3, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 
			0, 0, 0, 8, 0, 0, 2, 2, 0, 0, 0, 9, 5, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 1, 0, 8, 
			0, 0, 0, 0, 0, 5, 0, 0, 0, 5, 1, 0, 0, 0, 0, 5, 0, 1, 3, 0, 0, 5, 0, 0, 0, 0, 0, 0, 2, 1, 0, 1, 
			0, 0, 5, 1, 0, 0, 0, 0, 9, 1, 1, 0, 1, 2, 1, 0, 2, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 
			0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0};
	static short gotoStart[] = {
			0, 5, 5, 5, 5, 5, 5, 5, 5, 8, 9, 9, 9, 9, 9, 10, 11, 11, 11, 11, 12, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 23, 
			23, 23, 23, 23, 31, 31, 31, 33, 35, 35, 35, 35, 44, 49, 49, 49, 49, 49, 49, 49, 49, 49, 51, 51, 51, 51, 51, 51, 52, 52, 53, 53, 
			61, 61, 61, 61, 61, 61, 66, 66, 66, 66, 71, 72, 72, 72, 72, 72, 77, 77, 78, 81, 81, 81, 86, 86, 86, 86, 86, 86, 86, 88, 89, 89, 
			90, 90, 90, 95, 96, 96, 96, 96, 96, 105, 106, 107, 107, 108, 110, 111, 111, 113, 113, 113, 121, 121, 121, 121, 121, 121, 121, 121, 122, 122, 122, 122, 
			122, 122, 122, 122, 123, 123, 123, 123, 123, 123, 124, 124, 124, 125, 125, 125, 125, 125};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (274, 5), dGotoEntry (275, 8), dGotoEntry (276, 7), dGotoEntry (277, 6), dGotoEntry (278, 4), 
			dGotoEntry (276, 12), dGotoEntry (277, 6), dGotoEntry (278, 4), dGotoEntry (279, 14), dGotoEntry (280, 18), 
			dGotoEntry (279, 19), dGotoEntry (280, 21), dGotoEntry (281, 35), dGotoEntry (282, 36), dGotoEntry (283, 33), 
			dGotoEntry (284, 37), dGotoEntry (285, 34), dGotoEntry (286, 38), dGotoEntry (293, 40), dGotoEntry (294, 41), 
			dGotoEntry (297, 39), dGotoEntry (293, 48), dGotoEntry (297, 39), dGotoEntry (282, 54), dGotoEntry (283, 33), 
			dGotoEntry (284, 37), dGotoEntry (285, 34), dGotoEntry (286, 38), dGotoEntry (293, 55), dGotoEntry (294, 41), 
			dGotoEntry (297, 39), dGotoEntry (293, 58), dGotoEntry (297, 39), dGotoEntry (298, 61), dGotoEntry (299, 60), 
			dGotoEntry (281, 63), dGotoEntry (282, 36), dGotoEntry (283, 33), dGotoEntry (284, 37), dGotoEntry (285, 34), 
			dGotoEntry (286, 38), dGotoEntry (293, 40), dGotoEntry (294, 41), dGotoEntry (297, 39), dGotoEntry (287, 67), 
			dGotoEntry (288, 66), dGotoEntry (289, 65), dGotoEntry (293, 68), dGotoEntry (297, 39), dGotoEntry (293, 71), 
			dGotoEntry (297, 39), dGotoEntry (296, 77), dGotoEntry (299, 78), dGotoEntry (282, 54), dGotoEntry (283, 33), 
			dGotoEntry (284, 37), dGotoEntry (285, 34), dGotoEntry (286, 38), dGotoEntry (293, 55), dGotoEntry (294, 41), 
			dGotoEntry (297, 39), dGotoEntry (287, 67), dGotoEntry (288, 66), dGotoEntry (289, 84), dGotoEntry (293, 68), 
			dGotoEntry (297, 39), dGotoEntry (287, 67), dGotoEntry (288, 66), dGotoEntry (289, 87), dGotoEntry (293, 68), 
			dGotoEntry (297, 39), dGotoEntry (290, 89), dGotoEntry (287, 67), dGotoEntry (288, 66), dGotoEntry (289, 91), 
			dGotoEntry (293, 68), dGotoEntry (297, 39), dGotoEntry (291, 93), dGotoEntry (288, 94), dGotoEntry (293, 68), 
			dGotoEntry (297, 39), dGotoEntry (287, 67), dGotoEntry (288, 66), dGotoEntry (289, 96), dGotoEntry (293, 68), 
			dGotoEntry (297, 39), dGotoEntry (300, 101), dGotoEntry (301, 102), dGotoEntry (292, 103), dGotoEntry (291, 105), 
			dGotoEntry (287, 67), dGotoEntry (288, 66), dGotoEntry (289, 107), dGotoEntry (293, 68), dGotoEntry (297, 39), 
			dGotoEntry (290, 108), dGotoEntry (293, 119), dGotoEntry (294, 120), dGotoEntry (295, 116), dGotoEntry (297, 39), 
			dGotoEntry (304, 115), dGotoEntry (305, 114), dGotoEntry (306, 117), dGotoEntry (307, 118), dGotoEntry (308, 113), 
			dGotoEntry (292, 122), dGotoEntry (290, 123), dGotoEntry (292, 125), dGotoEntry (302, 129), dGotoEntry (303, 128), 
			dGotoEntry (301, 130), dGotoEntry (293, 132), dGotoEntry (297, 39), dGotoEntry (293, 119), dGotoEntry (294, 134), 
			dGotoEntry (295, 116), dGotoEntry (297, 39), dGotoEntry (305, 133), dGotoEntry (306, 117), dGotoEntry (307, 118), 
			dGotoEntry (308, 113), dGotoEntry (292, 138), dGotoEntry (296, 142), dGotoEntry (296, 144), dGotoEntry (303, 145)};
	static dActionEntry actionTable[] = {
			dActionEntry (255, 1, 0, 0, 1), dActionEntry (259, 0, 2, 0, 0), dActionEntry (261, 0, 3, 0, 0), dActionEntry (262, 0, 1, 0, 0), 
			dActionEntry (263, 0, 9, 0, 0), dActionEntry (260, 0, 10, 0, 0), dActionEntry (262, 0, 11, 0, 0), dActionEntry (255, 1, 2, 1, 6), 
			dActionEntry (259, 1, 2, 1, 6), dActionEntry (261, 1, 2, 1, 6), dActionEntry (262, 1, 2, 1, 6), dActionEntry (255, 2, 0, 0, 0), 
			dActionEntry (255, 1, 2, 1, 5), dActionEntry (259, 1, 2, 1, 5), dActionEntry (261, 1, 2, 1, 5), dActionEntry (262, 1, 2, 1, 5), 
			dActionEntry (255, 1, 1, 1, 3), dActionEntry (259, 1, 1, 1, 3), dActionEntry (261, 1, 1, 1, 3), dActionEntry (262, 1, 1, 1, 3), 
			dActionEntry (255, 1, 0, 1, 2), dActionEntry (259, 0, 2, 0, 0), dActionEntry (261, 0, 3, 0, 0), dActionEntry (262, 0, 1, 0, 0), 
			dActionEntry (123, 1, 5, 0, 14), dActionEntry (264, 0, 13, 0, 0), dActionEntry (255, 1, 3, 2, 7), dActionEntry (259, 1, 3, 2, 7), 
			dActionEntry (261, 1, 3, 2, 7), dActionEntry (262, 1, 3, 2, 7), dActionEntry (263, 0, 15, 0, 0), dActionEntry (255, 1, 1, 2, 4), 
			dActionEntry (259, 1, 1, 2, 4), dActionEntry (261, 1, 1, 2, 4), dActionEntry (262, 1, 1, 2, 4), dActionEntry (263, 0, 16, 0, 0), 
			dActionEntry (123, 1, 6, 0, 12), dActionEntry (265, 0, 17, 0, 0), dActionEntry (123, 1, 5, 2, 15), dActionEntry (123, 1, 6, 1, 13), 
			dActionEntry (123, 0, 20, 0, 0), dActionEntry (125, 0, 42, 0, 0), dActionEntry (256, 0, 29, 0, 0), dActionEntry (261, 0, 30, 0, 0), 
			dActionEntry (263, 0, 26, 0, 0), dActionEntry (266, 0, 22, 0, 0), dActionEntry (267, 0, 23, 0, 0), dActionEntry (268, 0, 31, 0, 0), 
			dActionEntry (269, 0, 27, 0, 0), dActionEntry (270, 0, 28, 0, 0), dActionEntry (271, 0, 32, 0, 0), dActionEntry (272, 0, 25, 0, 0), 
			dActionEntry (273, 0, 24, 0, 0), dActionEntry (123, 0, 43, 0, 0), dActionEntry (91, 1, 23, 1, 43), dActionEntry (263, 1, 23, 1, 43), 
			dActionEntry (91, 1, 23, 1, 44), dActionEntry (263, 1, 23, 1, 44), dActionEntry (91, 1, 23, 1, 53), dActionEntry (263, 1, 23, 1, 53), 
			dActionEntry (91, 1, 23, 1, 52), dActionEntry (263, 1, 23, 1, 52), dActionEntry (40, 0, 44, 0, 0), dActionEntry (91, 1, 23, 1, 46), 
			dActionEntry (263, 1, 23, 1, 46), dActionEntry (91, 1, 23, 1, 47), dActionEntry (263, 1, 23, 1, 47), dActionEntry (261, 0, 45, 0, 0), 
			dActionEntry (263, 1, 12, 1, 21), dActionEntry (266, 1, 12, 1, 21), dActionEntry (267, 1, 12, 1, 21), dActionEntry (268, 1, 12, 1, 21), 
			dActionEntry (269, 1, 12, 1, 21), dActionEntry (270, 1, 12, 1, 21), dActionEntry (271, 1, 12, 1, 21), dActionEntry (272, 1, 12, 1, 21), 
			dActionEntry (273, 1, 12, 1, 21), dActionEntry (256, 0, 47, 0, 0), dActionEntry (263, 0, 46, 0, 0), dActionEntry (266, 0, 22, 0, 0), 
			dActionEntry (267, 0, 23, 0, 0), dActionEntry (268, 0, 31, 0, 0), dActionEntry (269, 0, 27, 0, 0), dActionEntry (270, 0, 28, 0, 0), 
			dActionEntry (271, 0, 32, 0, 0), dActionEntry (272, 0, 25, 0, 0), dActionEntry (273, 0, 24, 0, 0), dActionEntry (91, 1, 23, 1, 45), 
			dActionEntry (263, 1, 23, 1, 45), dActionEntry (267, 0, 49, 0, 0), dActionEntry (268, 0, 52, 0, 0), dActionEntry (269, 0, 50, 0, 0), 
			dActionEntry (270, 0, 51, 0, 0), dActionEntry (125, 1, 8, 1, 18), dActionEntry (256, 1, 8, 1, 18), dActionEntry (261, 1, 8, 1, 18), 
			dActionEntry (263, 1, 8, 1, 18), dActionEntry (266, 1, 8, 1, 18), dActionEntry (267, 1, 8, 1, 18), dActionEntry (268, 1, 8, 1, 18), 
			dActionEntry (269, 1, 8, 1, 18), dActionEntry (270, 1, 8, 1, 18), dActionEntry (271, 1, 8, 1, 18), dActionEntry (272, 1, 8, 1, 18), 
			dActionEntry (273, 1, 8, 1, 18), dActionEntry (125, 1, 8, 1, 20), dActionEntry (256, 1, 8, 1, 20), dActionEntry (261, 1, 8, 1, 20), 
			dActionEntry (263, 1, 8, 1, 20), dActionEntry (266, 1, 8, 1, 20), dActionEntry (267, 1, 8, 1, 20), dActionEntry (268, 1, 8, 1, 20), 
			dActionEntry (269, 1, 8, 1, 20), dActionEntry (270, 1, 8, 1, 20), dActionEntry (271, 1, 8, 1, 20), dActionEntry (272, 1, 8, 1, 20), 
			dActionEntry (273, 1, 8, 1, 20), dActionEntry (125, 0, 56, 0, 0), dActionEntry (256, 0, 29, 0, 0), dActionEntry (261, 0, 53, 0, 0), 
			dActionEntry (263, 0, 26, 0, 0), dActionEntry (266, 0, 22, 0, 0), dActionEntry (267, 0, 23, 0, 0), dActionEntry (268, 0, 31, 0, 0), 
			dActionEntry (269, 0, 27, 0, 0), dActionEntry (270, 0, 28, 0, 0), dActionEntry (271, 0, 32, 0, 0), dActionEntry (272, 0, 25, 0, 0), 
			dActionEntry (273, 0, 24, 0, 0), dActionEntry (125, 1, 7, 1, 16), dActionEntry (256, 1, 7, 1, 16), dActionEntry (261, 1, 7, 1, 16), 
			dActionEntry (263, 1, 7, 1, 16), dActionEntry (266, 1, 7, 1, 16), dActionEntry (267, 1, 7, 1, 16), dActionEntry (268, 1, 7, 1, 16), 
			dActionEntry (269, 1, 7, 1, 16), dActionEntry (270, 1, 7, 1, 16), dActionEntry (271, 1, 7, 1, 16), dActionEntry (272, 1, 7, 1, 16), 
			dActionEntry (273, 1, 7, 1, 16), dActionEntry (125, 1, 8, 1, 19), dActionEntry (256, 1, 8, 1, 19), dActionEntry (261, 1, 8, 1, 19), 
			dActionEntry (263, 1, 8, 1, 19), dActionEntry (266, 1, 8, 1, 19), dActionEntry (267, 1, 8, 1, 19), dActionEntry (268, 1, 8, 1, 19), 
			dActionEntry (269, 1, 8, 1, 19), dActionEntry (270, 1, 8, 1, 19), dActionEntry (271, 1, 8, 1, 19), dActionEntry (272, 1, 8, 1, 19), 
			dActionEntry (273, 1, 8, 1, 19), dActionEntry (263, 0, 57, 0, 0), dActionEntry (266, 0, 22, 0, 0), dActionEntry (267, 0, 23, 0, 0), 
			dActionEntry (268, 0, 31, 0, 0), dActionEntry (269, 0, 27, 0, 0), dActionEntry (270, 0, 28, 0, 0), dActionEntry (271, 0, 32, 0, 0), 
			dActionEntry (272, 0, 25, 0, 0), dActionEntry (273, 0, 24, 0, 0), dActionEntry (91, 0, 59, 0, 0), dActionEntry (263, 1, 19, 1, 41), 
			dActionEntry (263, 0, 62, 0, 0), dActionEntry (125, 1, 10, 1, 39), dActionEntry (256, 1, 10, 1, 39), dActionEntry (261, 1, 10, 1, 39), 
			dActionEntry (263, 1, 10, 1, 39), dActionEntry (266, 1, 10, 1, 39), dActionEntry (267, 1, 10, 1, 39), dActionEntry (268, 1, 10, 1, 39), 
			dActionEntry (269, 1, 10, 1, 39), dActionEntry (270, 1, 10, 1, 39), dActionEntry (271, 1, 10, 1, 39), dActionEntry (272, 1, 10, 1, 39), 
			dActionEntry (273, 1, 10, 1, 39), dActionEntry (255, 1, 4, 6, 8), dActionEntry (259, 1, 4, 6, 8), dActionEntry (261, 1, 4, 6, 8), 
			dActionEntry (262, 1, 4, 6, 8), dActionEntry (125, 0, 64, 0, 0), dActionEntry (256, 0, 29, 0, 0), dActionEntry (261, 0, 30, 0, 0), 
			dActionEntry (263, 0, 26, 0, 0), dActionEntry (266, 0, 22, 0, 0), dActionEntry (267, 0, 23, 0, 0), dActionEntry (268, 0, 31, 0, 0), 
			dActionEntry (269, 0, 27, 0, 0), dActionEntry (270, 0, 28, 0, 0), dActionEntry (271, 0, 32, 0, 0), dActionEntry (272, 0, 25, 0, 0), 
			dActionEntry (273, 0, 24, 0, 0), dActionEntry (41, 1, 15, 0, 26), dActionEntry (266, 0, 22, 0, 0), dActionEntry (267, 0, 23, 0, 0), 
			dActionEntry (268, 0, 31, 0, 0), dActionEntry (269, 0, 27, 0, 0), dActionEntry (270, 0, 28, 0, 0), dActionEntry (271, 0, 32, 0, 0), 
			dActionEntry (272, 0, 25, 0, 0), dActionEntry (273, 0, 24, 0, 0), dActionEntry (263, 1, 12, 2, 22), dActionEntry (266, 1, 12, 2, 22), 
			dActionEntry (267, 1, 12, 2, 22), dActionEntry (268, 1, 12, 2, 22), dActionEntry (269, 1, 12, 2, 22), dActionEntry (270, 1, 12, 2, 22), 
			dActionEntry (271, 1, 12, 2, 22), dActionEntry (272, 1, 12, 2, 22), dActionEntry (273, 1, 12, 2, 22), dActionEntry (40, 0, 69, 0, 0), 
			dActionEntry (263, 1, 12, 2, 23), dActionEntry (266, 1, 12, 2, 23), dActionEntry (267, 1, 12, 2, 23), dActionEntry (268, 1, 12, 2, 23), 
			dActionEntry (269, 1, 12, 2, 23), dActionEntry (270, 1, 12, 2, 23), dActionEntry (271, 1, 12, 2, 23), dActionEntry (272, 1, 12, 2, 23), 
			dActionEntry (273, 1, 12, 2, 23), dActionEntry (263, 0, 70, 0, 0), dActionEntry (91, 1, 23, 2, 48), dActionEntry (263, 1, 23, 2, 48), 
			dActionEntry (91, 1, 23, 2, 50), dActionEntry (263, 1, 23, 2, 50), dActionEntry (91, 1, 23, 2, 51), dActionEntry (263, 1, 23, 2, 51), 
			dActionEntry (91, 1, 23, 2, 49), dActionEntry (263, 1, 23, 2, 49), dActionEntry (125, 1, 7, 2, 17), dActionEntry (256, 1, 7, 2, 17), 
			dActionEntry (261, 1, 7, 2, 17), dActionEntry (263, 1, 7, 2, 17), dActionEntry (266, 1, 7, 2, 17), dActionEntry (267, 1, 7, 2, 17), 
			dActionEntry (268, 1, 7, 2, 17), dActionEntry (269, 1, 7, 2, 17), dActionEntry (270, 1, 7, 2, 17), dActionEntry (271, 1, 7, 2, 17), 
			dActionEntry (272, 1, 7, 2, 17), dActionEntry (273, 1, 7, 2, 17), dActionEntry (263, 0, 72, 0, 0), dActionEntry (255, 1, 4, 7, 9), 
			dActionEntry (259, 1, 4, 7, 9), dActionEntry (261, 1, 4, 7, 9), dActionEntry (262, 1, 4, 7, 9), dActionEntry (40, 0, 73, 0, 0), 
			dActionEntry (263, 0, 74, 0, 0), dActionEntry (93, 0, 76, 0, 0), dActionEntry (258, 0, 75, 0, 0), dActionEntry (91, 1, 24, 1, 54), 
			dActionEntry (263, 1, 24, 1, 54), dActionEntry (91, 0, 59, 0, 0), dActionEntry (263, 1, 19, 2, 42), dActionEntry (40, 0, 79, 0, 0), 
			dActionEntry (125, 1, 20, 2, 36), dActionEntry (256, 1, 20, 2, 36), dActionEntry (261, 1, 20, 2, 36), dActionEntry (263, 1, 20, 2, 36), 
			dActionEntry (266, 1, 20, 2, 36), dActionEntry (267, 1, 20, 2, 36), dActionEntry (268, 1, 20, 2, 36), dActionEntry (269, 1, 20, 2, 36), 
			dActionEntry (270, 1, 20, 2, 36), dActionEntry (271, 1, 20, 2, 36), dActionEntry (272, 1, 20, 2, 36), dActionEntry (273, 1, 20, 2, 36), 
			dActionEntry (125, 0, 80, 0, 0), dActionEntry (256, 0, 29, 0, 0), dActionEntry (261, 0, 53, 0, 0), dActionEntry (263, 0, 26, 0, 0), 
			dActionEntry (266, 0, 22, 0, 0), dActionEntry (267, 0, 23, 0, 0), dActionEntry (268, 0, 31, 0, 0), dActionEntry (269, 0, 27, 0, 0), 
			dActionEntry (270, 0, 28, 0, 0), dActionEntry (271, 0, 32, 0, 0), dActionEntry (272, 0, 25, 0, 0), dActionEntry (273, 0, 24, 0, 0), 
			dActionEntry (255, 1, 4, 7, 10), dActionEntry (259, 1, 4, 7, 10), dActionEntry (261, 1, 4, 7, 10), dActionEntry (262, 1, 4, 7, 10), 
			dActionEntry (41, 0, 81, 0, 0), dActionEntry (41, 1, 13, 1, 24), dActionEntry (44, 1, 13, 1, 24), dActionEntry (41, 1, 15, 1, 27), 
			dActionEntry (44, 0, 82, 0, 0), dActionEntry (263, 0, 83, 0, 0), dActionEntry (40, 0, 85, 0, 0), dActionEntry (125, 1, 20, 3, 37), 
			dActionEntry (256, 1, 20, 3, 37), dActionEntry (261, 1, 20, 3, 37), dActionEntry (263, 1, 20, 3, 37), dActionEntry (266, 1, 20, 3, 37), 
			dActionEntry (267, 1, 20, 3, 37), dActionEntry (268, 1, 20, 3, 37), dActionEntry (269, 1, 20, 3, 37), dActionEntry (270, 1, 20, 3, 37), 
			dActionEntry (271, 1, 20, 3, 37), dActionEntry (272, 1, 20, 3, 37), dActionEntry (273, 1, 20, 3, 37), dActionEntry (263, 0, 86, 0, 0), 
			dActionEntry (40, 1, 16, 0, 28), dActionEntry (257, 0, 88, 0, 0), dActionEntry (93, 1, 22, 1, 75), dActionEntry (91, 1, 25, 2, 56), 
			dActionEntry (263, 1, 25, 2, 56), dActionEntry (93, 0, 90, 0, 0), dActionEntry (91, 1, 24, 2, 55), dActionEntry (263, 1, 24, 2, 55), 
			dActionEntry (255, 1, 4, 8, 11), dActionEntry (259, 1, 4, 8, 11), dActionEntry (261, 1, 4, 8, 11), dActionEntry (262, 1, 4, 8, 11), 
			dActionEntry (58, 0, 92, 0, 0), dActionEntry (123, 1, 17, 0, 58), dActionEntry (266, 0, 22, 0, 0), dActionEntry (267, 0, 23, 0, 0), 
			dActionEntry (268, 0, 31, 0, 0), dActionEntry (269, 0, 27, 0, 0), dActionEntry (270, 0, 28, 0, 0), dActionEntry (271, 0, 32, 0, 0), 
			dActionEntry (272, 0, 25, 0, 0), dActionEntry (273, 0, 24, 0, 0), dActionEntry (41, 1, 14, 2, 40), dActionEntry (44, 1, 14, 2, 40), 
			dActionEntry (41, 0, 95, 0, 0), dActionEntry (41, 0, 97, 0, 0), dActionEntry (40, 1, 16, 1, 29), dActionEntry (40, 0, 98, 0, 0), 
			dActionEntry (91, 1, 25, 3, 57), dActionEntry (263, 1, 25, 3, 57), dActionEntry (41, 0, 99, 0, 0), dActionEntry (263, 0, 100, 0, 0), 
			dActionEntry (123, 0, 104, 0, 0), dActionEntry (41, 1, 13, 3, 25), dActionEntry (44, 1, 13, 3, 25), dActionEntry (41, 0, 106, 0, 0), 
			dActionEntry (125, 1, 9, 5, 30), dActionEntry (256, 1, 9, 5, 30), dActionEntry (261, 1, 9, 5, 30), dActionEntry (263, 1, 9, 5, 30), 
			dActionEntry (266, 1, 9, 5, 30), dActionEntry (267, 1, 9, 5, 30), dActionEntry (268, 1, 9, 5, 30), dActionEntry (269, 1, 9, 5, 30), 
			dActionEntry (270, 1, 9, 5, 30), dActionEntry (271, 1, 9, 5, 30), dActionEntry (272, 1, 9, 5, 30), dActionEntry (273, 1, 9, 5, 30), 
			dActionEntry (123, 1, 16, 0, 28), dActionEntry (257, 0, 88, 0, 0), dActionEntry (40, 0, 109, 0, 0), dActionEntry (44, 0, 110, 0, 0), 
			dActionEntry (123, 1, 17, 2, 59), dActionEntry (44, 1, 26, 1, 60), dActionEntry (123, 1, 26, 1, 60), dActionEntry (125, 1, 9, 6, 31), 
			dActionEntry (256, 1, 9, 6, 31), dActionEntry (261, 1, 9, 6, 31), dActionEntry (263, 1, 9, 6, 31), dActionEntry (266, 1, 9, 6, 31), 
			dActionEntry (267, 1, 9, 6, 31), dActionEntry (268, 1, 9, 6, 31), dActionEntry (269, 1, 9, 6, 31), dActionEntry (270, 1, 9, 6, 31), 
			dActionEntry (271, 1, 9, 6, 31), dActionEntry (272, 1, 9, 6, 31), dActionEntry (273, 1, 9, 6, 31), dActionEntry (125, 0, 121, 0, 0), 
			dActionEntry (261, 0, 112, 0, 0), dActionEntry (263, 0, 111, 0, 0), dActionEntry (266, 0, 22, 0, 0), dActionEntry (267, 0, 23, 0, 0), 
			dActionEntry (268, 0, 31, 0, 0), dActionEntry (269, 0, 27, 0, 0), dActionEntry (270, 0, 28, 0, 0), dActionEntry (271, 0, 32, 0, 0), 
			dActionEntry (272, 0, 25, 0, 0), dActionEntry (273, 0, 24, 0, 0), dActionEntry (41, 0, 124, 0, 0), dActionEntry (41, 0, 126, 0, 0), 
			dActionEntry (258, 0, 127, 0, 0), dActionEntry (61, 0, 131, 0, 0), dActionEntry (125, 1, 33, 1, 76), dActionEntry (261, 1, 33, 1, 76), 
			dActionEntry (263, 1, 33, 1, 76), dActionEntry (266, 1, 33, 1, 76), dActionEntry (267, 1, 33, 1, 76), dActionEntry (268, 1, 33, 1, 76), 
			dActionEntry (269, 1, 33, 1, 76), dActionEntry (270, 1, 33, 1, 76), dActionEntry (271, 1, 33, 1, 76), dActionEntry (272, 1, 33, 1, 76), 
			dActionEntry (273, 1, 33, 1, 76), dActionEntry (125, 1, 30, 1, 69), dActionEntry (261, 1, 30, 1, 69), dActionEntry (263, 1, 30, 1, 69), 
			dActionEntry (266, 1, 30, 1, 69), dActionEntry (267, 1, 30, 1, 69), dActionEntry (268, 1, 30, 1, 69), dActionEntry (269, 1, 30, 1, 69), 
			dActionEntry (270, 1, 30, 1, 69), dActionEntry (271, 1, 30, 1, 69), dActionEntry (272, 1, 30, 1, 69), dActionEntry (273, 1, 30, 1, 69), 
			dActionEntry (125, 0, 135, 0, 0), dActionEntry (261, 0, 112, 0, 0), dActionEntry (263, 0, 111, 0, 0), dActionEntry (266, 0, 22, 0, 0), 
			dActionEntry (267, 0, 23, 0, 0), dActionEntry (268, 0, 31, 0, 0), dActionEntry (269, 0, 27, 0, 0), dActionEntry (270, 0, 28, 0, 0), 
			dActionEntry (271, 0, 32, 0, 0), dActionEntry (272, 0, 25, 0, 0), dActionEntry (273, 0, 24, 0, 0), dActionEntry (125, 1, 32, 1, 74), 
			dActionEntry (261, 1, 32, 1, 74), dActionEntry (263, 1, 32, 1, 74), dActionEntry (266, 1, 32, 1, 74), dActionEntry (267, 1, 32, 1, 74), 
			dActionEntry (268, 1, 32, 1, 74), dActionEntry (269, 1, 32, 1, 74), dActionEntry (270, 1, 32, 1, 74), dActionEntry (271, 1, 32, 1, 74), 
			dActionEntry (272, 1, 32, 1, 74), dActionEntry (273, 1, 32, 1, 74), dActionEntry (125, 1, 31, 1, 71), dActionEntry (261, 1, 31, 1, 71), 
			dActionEntry (263, 1, 31, 1, 71), dActionEntry (266, 1, 31, 1, 71), dActionEntry (267, 1, 31, 1, 71), dActionEntry (268, 1, 31, 1, 71), 
			dActionEntry (269, 1, 31, 1, 71), dActionEntry (270, 1, 31, 1, 71), dActionEntry (271, 1, 31, 1, 71), dActionEntry (272, 1, 31, 1, 71), 
			dActionEntry (273, 1, 31, 1, 71), dActionEntry (125, 1, 31, 1, 72), dActionEntry (261, 1, 31, 1, 72), dActionEntry (263, 1, 31, 1, 72), 
			dActionEntry (266, 1, 31, 1, 72), dActionEntry (267, 1, 31, 1, 72), dActionEntry (268, 1, 31, 1, 72), dActionEntry (269, 1, 31, 1, 72), 
			dActionEntry (270, 1, 31, 1, 72), dActionEntry (271, 1, 31, 1, 72), dActionEntry (272, 1, 31, 1, 72), dActionEntry (273, 1, 31, 1, 72), 
			dActionEntry (263, 0, 136, 0, 0), dActionEntry (61, 0, 137, 0, 0), dActionEntry (125, 1, 32, 1, 73), dActionEntry (261, 1, 32, 1, 73), 
			dActionEntry (263, 1, 32, 1, 73), dActionEntry (266, 1, 32, 1, 73), dActionEntry (267, 1, 32, 1, 73), dActionEntry (268, 1, 32, 1, 73), 
			dActionEntry (269, 1, 32, 1, 73), dActionEntry (270, 1, 32, 1, 73), dActionEntry (271, 1, 32, 1, 73), dActionEntry (272, 1, 32, 1, 73), 
			dActionEntry (273, 1, 32, 1, 73), dActionEntry (125, 1, 18, 2, 67), dActionEntry (256, 1, 18, 2, 67), dActionEntry (261, 1, 18, 2, 67), 
			dActionEntry (263, 1, 18, 2, 67), dActionEntry (266, 1, 18, 2, 67), dActionEntry (267, 1, 18, 2, 67), dActionEntry (268, 1, 18, 2, 67), 
			dActionEntry (269, 1, 18, 2, 67), dActionEntry (270, 1, 18, 2, 67), dActionEntry (271, 1, 18, 2, 67), dActionEntry (272, 1, 18, 2, 67), 
			dActionEntry (273, 1, 18, 2, 67), dActionEntry (125, 1, 9, 7, 32), dActionEntry (256, 1, 9, 7, 32), dActionEntry (261, 1, 9, 7, 32), 
			dActionEntry (263, 1, 9, 7, 32), dActionEntry (266, 1, 9, 7, 32), dActionEntry (267, 1, 9, 7, 32), dActionEntry (268, 1, 9, 7, 32), 
			dActionEntry (269, 1, 9, 7, 32), dActionEntry (270, 1, 9, 7, 32), dActionEntry (271, 1, 9, 7, 32), dActionEntry (272, 1, 9, 7, 32), 
			dActionEntry (273, 1, 9, 7, 32), dActionEntry (125, 1, 11, 7, 33), dActionEntry (256, 1, 11, 7, 33), dActionEntry (261, 1, 11, 7, 33), 
			dActionEntry (263, 1, 11, 7, 33), dActionEntry (266, 1, 11, 7, 33), dActionEntry (267, 1, 11, 7, 33), dActionEntry (268, 1, 11, 7, 33), 
			dActionEntry (269, 1, 11, 7, 33), dActionEntry (270, 1, 11, 7, 33), dActionEntry (271, 1, 11, 7, 33), dActionEntry (272, 1, 11, 7, 33), 
			dActionEntry (273, 1, 11, 7, 33), dActionEntry (125, 1, 11, 7, 34), dActionEntry (256, 1, 11, 7, 34), dActionEntry (261, 1, 11, 7, 34), 
			dActionEntry (263, 1, 11, 7, 34), dActionEntry (266, 1, 11, 7, 34), dActionEntry (267, 1, 11, 7, 34), dActionEntry (268, 1, 11, 7, 34), 
			dActionEntry (269, 1, 11, 7, 34), dActionEntry (270, 1, 11, 7, 34), dActionEntry (271, 1, 11, 7, 34), dActionEntry (272, 1, 11, 7, 34), 
			dActionEntry (273, 1, 11, 7, 34), dActionEntry (44, 1, 27, 3, 62), dActionEntry (123, 1, 27, 3, 62), dActionEntry (41, 1, 29, 1, 66), 
			dActionEntry (44, 1, 29, 1, 66), dActionEntry (41, 1, 28, 1, 64), dActionEntry (44, 1, 28, 1, 64), dActionEntry (41, 0, 139, 0, 0), 
			dActionEntry (44, 0, 140, 0, 0), dActionEntry (44, 1, 26, 3, 61), dActionEntry (123, 1, 26, 3, 61), dActionEntry (258, 0, 141, 0, 0), 
			dActionEntry (263, 0, 143, 0, 0), dActionEntry (125, 1, 30, 2, 70), dActionEntry (261, 1, 30, 2, 70), dActionEntry (263, 1, 30, 2, 70), 
			dActionEntry (266, 1, 30, 2, 70), dActionEntry (267, 1, 30, 2, 70), dActionEntry (268, 1, 30, 2, 70), dActionEntry (269, 1, 30, 2, 70), 
			dActionEntry (270, 1, 30, 2, 70), dActionEntry (271, 1, 30, 2, 70), dActionEntry (272, 1, 30, 2, 70), dActionEntry (273, 1, 30, 2, 70), 
			dActionEntry (125, 1, 18, 3, 68), dActionEntry (256, 1, 18, 3, 68), dActionEntry (261, 1, 18, 3, 68), dActionEntry (263, 1, 18, 3, 68), 
			dActionEntry (266, 1, 18, 3, 68), dActionEntry (267, 1, 18, 3, 68), dActionEntry (268, 1, 18, 3, 68), dActionEntry (269, 1, 18, 3, 68), 
			dActionEntry (270, 1, 18, 3, 68), dActionEntry (271, 1, 18, 3, 68), dActionEntry (272, 1, 18, 3, 68), dActionEntry (273, 1, 18, 3, 68), 
			dActionEntry (61, 1, 20, 2, 36), dActionEntry (125, 1, 20, 2, 36), dActionEntry (261, 1, 20, 2, 36), dActionEntry (263, 1, 20, 2, 36), 
			dActionEntry (266, 1, 20, 2, 36), dActionEntry (267, 1, 20, 2, 36), dActionEntry (268, 1, 20, 2, 36), dActionEntry (269, 1, 20, 2, 36), 
			dActionEntry (270, 1, 20, 2, 36), dActionEntry (271, 1, 20, 2, 36), dActionEntry (272, 1, 20, 2, 36), dActionEntry (273, 1, 20, 2, 36), 
			dActionEntry (125, 1, 11, 8, 35), dActionEntry (256, 1, 11, 8, 35), dActionEntry (261, 1, 11, 8, 35), dActionEntry (263, 1, 11, 8, 35), 
			dActionEntry (266, 1, 11, 8, 35), dActionEntry (267, 1, 11, 8, 35), dActionEntry (268, 1, 11, 8, 35), dActionEntry (269, 1, 11, 8, 35), 
			dActionEntry (270, 1, 11, 8, 35), dActionEntry (271, 1, 11, 8, 35), dActionEntry (272, 1, 11, 8, 35), dActionEntry (273, 1, 11, 8, 35), 
			dActionEntry (44, 1, 27, 4, 63), dActionEntry (123, 1, 27, 4, 63), dActionEntry (258, 0, 127, 0, 0), dActionEntry (125, 1, 22, 1, 75), 
			dActionEntry (261, 1, 22, 1, 75), dActionEntry (263, 1, 22, 1, 75), dActionEntry (266, 1, 22, 1, 75), dActionEntry (267, 1, 22, 1, 75), 
			dActionEntry (268, 1, 22, 1, 75), dActionEntry (269, 1, 22, 1, 75), dActionEntry (270, 1, 22, 1, 75), dActionEntry (271, 1, 22, 1, 75), 
			dActionEntry (272, 1, 22, 1, 75), dActionEntry (273, 1, 22, 1, 75), dActionEntry (125, 1, 34, 3, 77), dActionEntry (261, 1, 34, 3, 77), 
			dActionEntry (263, 1, 34, 3, 77), dActionEntry (266, 1, 34, 3, 77), dActionEntry (267, 1, 34, 3, 77), dActionEntry (268, 1, 34, 3, 77), 
			dActionEntry (269, 1, 34, 3, 77), dActionEntry (270, 1, 34, 3, 77), dActionEntry (271, 1, 34, 3, 77), dActionEntry (272, 1, 34, 3, 77), 
			dActionEntry (273, 1, 34, 3, 77), dActionEntry (61, 1, 20, 3, 37), dActionEntry (125, 1, 20, 3, 37), dActionEntry (261, 1, 20, 3, 37), 
			dActionEntry (263, 1, 20, 3, 37), dActionEntry (266, 1, 20, 3, 37), dActionEntry (267, 1, 20, 3, 37), dActionEntry (268, 1, 20, 3, 37), 
			dActionEntry (269, 1, 20, 3, 37), dActionEntry (270, 1, 20, 3, 37), dActionEntry (271, 1, 20, 3, 37), dActionEntry (272, 1, 20, 3, 37), 
			dActionEntry (273, 1, 20, 3, 37), dActionEntry (125, 1, 21, 3, 38), dActionEntry (261, 1, 21, 3, 38), dActionEntry (263, 1, 21, 3, 38), 
			dActionEntry (266, 1, 21, 3, 38), dActionEntry (267, 1, 21, 3, 38), dActionEntry (268, 1, 21, 3, 38), dActionEntry (269, 1, 21, 3, 38), 
			dActionEntry (270, 1, 21, 3, 38), dActionEntry (271, 1, 21, 3, 38), dActionEntry (272, 1, 21, 3, 38), dActionEntry (273, 1, 21, 3, 38), 
			dActionEntry (41, 1, 28, 3, 65), dActionEntry (44, 1, 28, 3, 65)};

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



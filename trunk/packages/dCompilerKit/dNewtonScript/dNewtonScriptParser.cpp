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
			2, 10, 12, 3, 1, 12, 18, 1, 12, 1, 2, 2, 2, 13, 12, 13, 9, 18, 18, 1, 1, 1, 1, 8, 
			1, 1, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 1, 19, 12, 9, 1, 1, 2, 1, 2, 9, 1, 
			2, 2, 1, 1, 18, 8, 1, 1, 1, 1, 1, 8, 18, 1, 19, 12, 19, 1, 1, 9, 2, 1, 2, 8, 
			2, 1, 18, 1, 1, 18, 18, 1, 1, 19, 1, 1, 1, 19, 18, 12, 1, 2, 1, 1, 2, 1, 1, 1, 
			1, 1, 1, 1, 19, 13, 1, 1, 1, 2, 2, 12, 1, 1, 1, 1, 3, 18, 18, 12, 12, 2, 1, 18, 
			1, 1, 1, 19, 18, 1, 1, 3, 3, 1, 18, 18, 2, 2, 2, 2, 2, 1, 1, 18, 19, 1, 1, 3, 
			18, 18, 2, 1, 1, 1, 18, 19, 1, 3, 18, 2, 1, 1, 3, 18, 3, 1, 18, 3};
	static short actionsStart[] = {
			0, 4, 5, 6, 9, 10, 11, 14, 17, 21, 24, 25, 28, 30, 31, 33, 34, 35, 36, 48, 50, 52, 54, 56, 
			58, 60, 70, 80, 82, 86, 98, 110, 122, 134, 135, 147, 148, 149, 158, 160, 161, 173, 182, 148, 191, 192, 194, 196, 
			198, 70, 200, 212, 215, 216, 228, 246, 247, 259, 260, 262, 264, 266, 279, 291, 304, 313, 331, 349, 350, 351, 352, 353, 
			361, 362, 363, 381, 399, 417, 435, 453, 471, 489, 507, 525, 543, 561, 562, 581, 304, 593, 594, 595, 597, 598, 304, 600, 
			601, 603, 605, 606, 607, 353, 625, 626, 627, 626, 626, 353, 628, 646, 562, 647, 659, 625, 678, 304, 679, 681, 682, 353, 
			684, 686, 687, 705, 706, 707, 725, 743, 744, 745, 764, 765, 766, 659, 767, 785, 797, 798, 800, 148, 801, 626, 803, 804, 
			805, 806, 807, 808, 745, 809, 822, 823, 824, 825, 827, 829, 841, 842, 842, 843, 844, 847, 865, 883, 895, 907, 800, 909, 
			927, 928, 929, 930, 949, 967, 968, 969, 972, 975, 976, 994, 1012, 1014, 1016, 1018, 1020, 1022, 808, 1023, 1041, 1060, 1061, 1062, 
			1065, 1083, 1101, 1103, 1104, 1105, 1106, 1124, 1061, 1143, 1146, 1164, 626, 808, 1166, 1169, 1187, 1190, 1191, 1209};
	static short gotoCount[] = {
			6, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 1, 0, 1, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 
			10, 0, 0, 0, 1, 3, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 14, 0, 0, 0, 1, 0, 1, 0, 0, 0, 
			5, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 5, 0, 
			0, 0, 0, 0, 14, 2, 1, 1, 0, 1, 1, 2, 0, 0, 0, 0, 0, 1, 0, 5, 0, 0, 1, 3, 0, 0, 13, 0, 1, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 
			3, 0, 14, 0, 0, 2, 1, 0, 0, 0, 0, 0, 14, 0, 0, 0, 1, 0, 13, 0, 0, 0, 0, 0, 0, 1, 1, 13, 0, 0, 1, 0, 
			0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 14, 0, 1, 1, 0, 13, 0, 0, 0, 0};
	static short gotoStart[] = {
			0, 6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 10, 11, 11, 12, 12, 12, 12, 24, 24, 24, 24, 24, 24, 24, 24, 27, 27, 27, 27, 27, 
			27, 37, 37, 37, 37, 38, 41, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 47, 47, 47, 47, 47, 61, 61, 61, 61, 62, 62, 63, 63, 63, 
			63, 68, 68, 68, 69, 69, 69, 69, 71, 71, 71, 71, 71, 71, 71, 71, 84, 84, 84, 84, 84, 84, 84, 84, 84, 89, 89, 89, 89, 89, 89, 94, 
			94, 94, 94, 94, 94, 108, 110, 111, 112, 112, 113, 114, 116, 116, 116, 116, 116, 116, 117, 117, 122, 122, 122, 123, 126, 126, 126, 139, 139, 140, 140, 140, 
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 141, 143, 144, 144, 145, 145, 145, 145, 145, 145, 146, 146, 147, 147, 147, 147, 147, 147, 147, 147, 148, 149, 
			150, 153, 153, 167, 167, 167, 169, 170, 170, 170, 170, 170, 170, 184, 184, 184, 184, 185, 185, 198, 198, 198, 198, 198, 198, 198, 199, 200, 213, 213, 213, 214, 
			214, 214, 214, 214, 215, 215, 215, 215, 215, 216, 216, 230, 230, 231, 232, 232, 245, 245, 245, 245};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (285, 5), dGotoEntry (286, 8), dGotoEntry (287, 7), dGotoEntry (288, 6), dGotoEntry (289, 3), 
			dGotoEntry (290, 4), dGotoEntry (287, 11), dGotoEntry (288, 6), dGotoEntry (289, 3), dGotoEntry (290, 4), 
			dGotoEntry (291, 14), dGotoEntry (292, 17), dGotoEntry (290, 35), dGotoEntry (293, 33), dGotoEntry (294, 32), 
			dGotoEntry (295, 31), dGotoEntry (296, 29), dGotoEntry (297, 34), dGotoEntry (298, 30), dGotoEntry (299, 37), 
			dGotoEntry (306, 36), dGotoEntry (307, 39), dGotoEntry (308, 40), dGotoEntry (311, 38), dGotoEntry (306, 43), 
			dGotoEntry (307, 44), dGotoEntry (311, 38), dGotoEntry (290, 35), dGotoEntry (295, 50), dGotoEntry (296, 29), 
			dGotoEntry (297, 34), dGotoEntry (298, 30), dGotoEntry (299, 37), dGotoEntry (306, 36), dGotoEntry (307, 39), 
			dGotoEntry (308, 40), dGotoEntry (311, 38), dGotoEntry (305, 53), dGotoEntry (306, 56), dGotoEntry (307, 57), 
			dGotoEntry (311, 38), dGotoEntry (312, 60), dGotoEntry (313, 59), dGotoEntry (305, 62), dGotoEntry (306, 43), 
			dGotoEntry (307, 44), dGotoEntry (311, 38), dGotoEntry (307, 85), dGotoEntry (308, 86), dGotoEntry (309, 81), 
			dGotoEntry (311, 38), dGotoEntry (318, 79), dGotoEntry (319, 78), dGotoEntry (320, 82), dGotoEntry (321, 83), 
			dGotoEntry (322, 74), dGotoEntry (323, 80), dGotoEntry (324, 77), dGotoEntry (325, 84), dGotoEntry (326, 76), 
			dGotoEntry (327, 75), dGotoEntry (310, 92), dGotoEntry (313, 93), dGotoEntry (300, 97), dGotoEntry (301, 96), 
			dGotoEntry (302, 95), dGotoEntry (307, 98), dGotoEntry (311, 38), dGotoEntry (305, 99), dGotoEntry (307, 104), 
			dGotoEntry (311, 38), dGotoEntry (307, 109), dGotoEntry (308, 110), dGotoEntry (309, 81), dGotoEntry (311, 38), 
			dGotoEntry (319, 108), dGotoEntry (320, 82), dGotoEntry (321, 83), dGotoEntry (322, 74), dGotoEntry (323, 80), 
			dGotoEntry (324, 77), dGotoEntry (325, 84), dGotoEntry (326, 76), dGotoEntry (327, 75), dGotoEntry (300, 97), 
			dGotoEntry (301, 96), dGotoEntry (302, 114), dGotoEntry (307, 98), dGotoEntry (311, 38), dGotoEntry (300, 97), 
			dGotoEntry (301, 96), dGotoEntry (302, 117), dGotoEntry (307, 98), dGotoEntry (311, 38), dGotoEntry (307, 85), 
			dGotoEntry (308, 86), dGotoEntry (309, 81), dGotoEntry (311, 38), dGotoEntry (318, 122), dGotoEntry (319, 78), 
			dGotoEntry (320, 82), dGotoEntry (321, 83), dGotoEntry (322, 74), dGotoEntry (323, 80), dGotoEntry (324, 77), 
			dGotoEntry (325, 84), dGotoEntry (326, 76), dGotoEntry (327, 75), dGotoEntry (307, 124), dGotoEntry (311, 38), 
			dGotoEntry (310, 126), dGotoEntry (310, 128), dGotoEntry (310, 130), dGotoEntry (310, 131), dGotoEntry (307, 132), 
			dGotoEntry (311, 38), dGotoEntry (310, 134), dGotoEntry (300, 97), dGotoEntry (301, 96), dGotoEntry (302, 136), 
			dGotoEntry (307, 98), dGotoEntry (311, 38), dGotoEntry (304, 139), dGotoEntry (301, 140), dGotoEntry (307, 98), 
			dGotoEntry (311, 38), dGotoEntry (307, 109), dGotoEntry (308, 110), dGotoEntry (309, 81), dGotoEntry (311, 38), 
			dGotoEntry (319, 108), dGotoEntry (320, 82), dGotoEntry (321, 83), dGotoEntry (322, 74), dGotoEntry (323, 80), 
			dGotoEntry (324, 77), dGotoEntry (325, 84), dGotoEntry (326, 76), dGotoEntry (327, 75), dGotoEntry (322, 144), 
			dGotoEntry (303, 151), dGotoEntry (314, 153), dGotoEntry (315, 154), dGotoEntry (305, 155), dGotoEntry (310, 156), 
			dGotoEntry (305, 161), dGotoEntry (303, 164), dGotoEntry (310, 169), dGotoEntry (310, 170), dGotoEntry (305, 171), 
			dGotoEntry (328, 177), dGotoEntry (329, 176), dGotoEntry (330, 175), dGotoEntry (307, 85), dGotoEntry (308, 86), 
			dGotoEntry (309, 81), dGotoEntry (311, 38), dGotoEntry (318, 178), dGotoEntry (319, 78), dGotoEntry (320, 82), 
			dGotoEntry (321, 83), dGotoEntry (322, 74), dGotoEntry (323, 80), dGotoEntry (324, 77), dGotoEntry (325, 84), 
			dGotoEntry (326, 76), dGotoEntry (327, 75), dGotoEntry (316, 183), dGotoEntry (317, 182), dGotoEntry (315, 184), 
			dGotoEntry (307, 85), dGotoEntry (308, 86), dGotoEntry (309, 81), dGotoEntry (311, 38), dGotoEntry (318, 187), 
			dGotoEntry (319, 78), dGotoEntry (320, 82), dGotoEntry (321, 83), dGotoEntry (322, 74), dGotoEntry (323, 80), 
			dGotoEntry (324, 77), dGotoEntry (325, 84), dGotoEntry (326, 76), dGotoEntry (327, 75), dGotoEntry (330, 191), 
			dGotoEntry (307, 109), dGotoEntry (308, 110), dGotoEntry (309, 81), dGotoEntry (311, 38), dGotoEntry (319, 108), 
			dGotoEntry (320, 82), dGotoEntry (321, 83), dGotoEntry (322, 74), dGotoEntry (323, 80), dGotoEntry (324, 77), 
			dGotoEntry (325, 84), dGotoEntry (326, 76), dGotoEntry (327, 75), dGotoEntry (322, 197), dGotoEntry (305, 198), 
			dGotoEntry (307, 109), dGotoEntry (308, 110), dGotoEntry (309, 81), dGotoEntry (311, 38), dGotoEntry (319, 108), 
			dGotoEntry (320, 82), dGotoEntry (321, 83), dGotoEntry (322, 74), dGotoEntry (323, 80), dGotoEntry (324, 77), 
			dGotoEntry (325, 84), dGotoEntry (326, 76), dGotoEntry (327, 75), dGotoEntry (305, 201), dGotoEntry (317, 203), 
			dGotoEntry (305, 206), dGotoEntry (307, 85), dGotoEntry (308, 86), dGotoEntry (309, 81), dGotoEntry (311, 38), 
			dGotoEntry (318, 207), dGotoEntry (319, 78), dGotoEntry (320, 82), dGotoEntry (321, 83), dGotoEntry (322, 74), 
			dGotoEntry (323, 80), dGotoEntry (324, 77), dGotoEntry (325, 84), dGotoEntry (326, 76), dGotoEntry (327, 75), 
			dGotoEntry (310, 209), dGotoEntry (305, 210), dGotoEntry (307, 109), dGotoEntry (308, 110), dGotoEntry (309, 81), 
			dGotoEntry (311, 38), dGotoEntry (319, 108), dGotoEntry (320, 82), dGotoEntry (321, 83), dGotoEntry (322, 74), 
			dGotoEntry (323, 80), dGotoEntry (324, 77), dGotoEntry (325, 84), dGotoEntry (326, 76), dGotoEntry (327, 75)};
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
			dActionEntry (125, 0, 87, 0, 0), dActionEntry (261, 0, 71, 0, 0), dActionEntry (263, 0, 69, 0, 0), dActionEntry (266, 0, 19, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), 
			dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), dActionEntry (274, 0, 68, 0, 0), 
			dActionEntry (275, 0, 73, 0, 0), dActionEntry (276, 0, 67, 0, 0), dActionEntry (277, 0, 70, 0, 0), dActionEntry (280, 0, 72, 0, 0), 
			dActionEntry (283, 0, 66, 0, 0), dActionEntry (284, 0, 65, 0, 0), dActionEntry (40, 0, 88, 0, 0), dActionEntry (125, 1, 13, 2, 33), 
			dActionEntry (256, 1, 13, 2, 33), dActionEntry (261, 1, 13, 2, 33), dActionEntry (263, 1, 13, 2, 33), dActionEntry (266, 1, 13, 2, 33), 
			dActionEntry (267, 1, 13, 2, 33), dActionEntry (268, 1, 13, 2, 33), dActionEntry (269, 1, 13, 2, 33), dActionEntry (270, 1, 13, 2, 33), 
			dActionEntry (271, 1, 13, 2, 33), dActionEntry (272, 1, 13, 2, 33), dActionEntry (273, 1, 13, 2, 33), dActionEntry (263, 0, 89, 0, 0), 
			dActionEntry (93, 0, 91, 0, 0), dActionEntry (258, 0, 90, 0, 0), dActionEntry (91, 1, 27, 1, 55), dActionEntry (263, 1, 27, 1, 55), 
			dActionEntry (91, 0, 58, 0, 0), dActionEntry (263, 1, 22, 2, 43), dActionEntry (40, 0, 94, 0, 0), dActionEntry (125, 1, 23, 2, 37), 
			dActionEntry (256, 1, 23, 2, 37), dActionEntry (261, 1, 23, 2, 37), dActionEntry (263, 1, 23, 2, 37), dActionEntry (266, 1, 23, 2, 37), 
			dActionEntry (267, 1, 23, 2, 37), dActionEntry (268, 1, 23, 2, 37), dActionEntry (269, 1, 23, 2, 37), dActionEntry (270, 1, 23, 2, 37), 
			dActionEntry (271, 1, 23, 2, 37), dActionEntry (272, 1, 23, 2, 37), dActionEntry (273, 1, 23, 2, 37), dActionEntry (125, 1, 13, 3, 35), 
			dActionEntry (256, 1, 13, 3, 35), dActionEntry (261, 1, 13, 3, 35), dActionEntry (263, 1, 13, 3, 35), dActionEntry (266, 1, 13, 3, 35), 
			dActionEntry (267, 1, 13, 3, 35), dActionEntry (268, 1, 13, 3, 35), dActionEntry (269, 1, 13, 3, 35), dActionEntry (270, 1, 13, 3, 35), 
			dActionEntry (271, 1, 13, 3, 35), dActionEntry (272, 1, 13, 3, 35), dActionEntry (273, 1, 13, 3, 35), dActionEntry (40, 0, 94, 0, 0), 
			dActionEntry (125, 1, 23, 3, 38), dActionEntry (256, 1, 23, 3, 38), dActionEntry (261, 1, 23, 3, 38), dActionEntry (263, 1, 23, 3, 38), 
			dActionEntry (266, 1, 23, 3, 38), dActionEntry (267, 1, 23, 3, 38), dActionEntry (268, 1, 23, 3, 38), dActionEntry (269, 1, 23, 3, 38), 
			dActionEntry (270, 1, 23, 3, 38), dActionEntry (271, 1, 23, 3, 38), dActionEntry (272, 1, 23, 3, 38), dActionEntry (273, 1, 23, 3, 38), 
			dActionEntry (41, 1, 17, 0, 27), dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), 
			dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), 
			dActionEntry (273, 0, 21, 0, 0), dActionEntry (125, 1, 42, 1, 91), dActionEntry (261, 1, 42, 1, 91), dActionEntry (263, 1, 42, 1, 91), 
			dActionEntry (266, 1, 42, 1, 91), dActionEntry (267, 1, 42, 1, 91), dActionEntry (268, 1, 42, 1, 91), dActionEntry (269, 1, 42, 1, 91), 
			dActionEntry (270, 1, 42, 1, 91), dActionEntry (271, 1, 42, 1, 91), dActionEntry (272, 1, 42, 1, 91), dActionEntry (273, 1, 42, 1, 91), 
			dActionEntry (274, 1, 42, 1, 91), dActionEntry (275, 1, 42, 1, 91), dActionEntry (276, 1, 42, 1, 91), dActionEntry (277, 1, 42, 1, 91), 
			dActionEntry (280, 1, 42, 1, 91), dActionEntry (283, 1, 42, 1, 91), dActionEntry (284, 1, 42, 1, 91), dActionEntry (125, 1, 41, 1, 90), 
			dActionEntry (261, 1, 41, 1, 90), dActionEntry (263, 1, 41, 1, 90), dActionEntry (266, 1, 41, 1, 90), dActionEntry (267, 1, 41, 1, 90), 
			dActionEntry (268, 1, 41, 1, 90), dActionEntry (269, 1, 41, 1, 90), dActionEntry (270, 1, 41, 1, 90), dActionEntry (271, 1, 41, 1, 90), 
			dActionEntry (272, 1, 41, 1, 90), dActionEntry (273, 1, 41, 1, 90), dActionEntry (274, 1, 41, 1, 90), dActionEntry (275, 1, 41, 1, 90), 
			dActionEntry (276, 1, 41, 1, 90), dActionEntry (277, 1, 41, 1, 90), dActionEntry (280, 1, 41, 1, 90), dActionEntry (283, 1, 41, 1, 90), 
			dActionEntry (284, 1, 41, 1, 90), dActionEntry (123, 0, 100, 0, 0), dActionEntry (40, 0, 101, 0, 0), dActionEntry (61, 0, 102, 0, 0), 
			dActionEntry (40, 0, 103, 0, 0), dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), 
			dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), 
			dActionEntry (273, 0, 21, 0, 0), dActionEntry (40, 0, 105, 0, 0), dActionEntry (40, 0, 106, 0, 0), dActionEntry (125, 1, 36, 1, 77), 
			dActionEntry (261, 1, 36, 1, 77), dActionEntry (263, 1, 36, 1, 77), dActionEntry (266, 1, 36, 1, 77), dActionEntry (267, 1, 36, 1, 77), 
			dActionEntry (268, 1, 36, 1, 77), dActionEntry (269, 1, 36, 1, 77), dActionEntry (270, 1, 36, 1, 77), dActionEntry (271, 1, 36, 1, 77), 
			dActionEntry (272, 1, 36, 1, 77), dActionEntry (273, 1, 36, 1, 77), dActionEntry (274, 1, 36, 1, 77), dActionEntry (275, 1, 36, 1, 77), 
			dActionEntry (276, 1, 36, 1, 77), dActionEntry (277, 1, 36, 1, 77), dActionEntry (280, 1, 36, 1, 77), dActionEntry (283, 1, 36, 1, 77), 
			dActionEntry (284, 1, 36, 1, 77), dActionEntry (125, 1, 36, 1, 82), dActionEntry (261, 1, 36, 1, 82), dActionEntry (263, 1, 36, 1, 82), 
			dActionEntry (266, 1, 36, 1, 82), dActionEntry (267, 1, 36, 1, 82), dActionEntry (268, 1, 36, 1, 82), dActionEntry (269, 1, 36, 1, 82), 
			dActionEntry (270, 1, 36, 1, 82), dActionEntry (271, 1, 36, 1, 82), dActionEntry (272, 1, 36, 1, 82), dActionEntry (273, 1, 36, 1, 82), 
			dActionEntry (274, 1, 36, 1, 82), dActionEntry (275, 1, 36, 1, 82), dActionEntry (276, 1, 36, 1, 82), dActionEntry (277, 1, 36, 1, 82), 
			dActionEntry (280, 1, 36, 1, 82), dActionEntry (283, 1, 36, 1, 82), dActionEntry (284, 1, 36, 1, 82), dActionEntry (125, 1, 36, 1, 81), 
			dActionEntry (261, 1, 36, 1, 81), dActionEntry (263, 1, 36, 1, 81), dActionEntry (266, 1, 36, 1, 81), dActionEntry (267, 1, 36, 1, 81), 
			dActionEntry (268, 1, 36, 1, 81), dActionEntry (269, 1, 36, 1, 81), dActionEntry (270, 1, 36, 1, 81), dActionEntry (271, 1, 36, 1, 81), 
			dActionEntry (272, 1, 36, 1, 81), dActionEntry (273, 1, 36, 1, 81), dActionEntry (274, 1, 36, 1, 81), dActionEntry (275, 1, 36, 1, 81), 
			dActionEntry (276, 1, 36, 1, 81), dActionEntry (277, 1, 36, 1, 81), dActionEntry (280, 1, 36, 1, 81), dActionEntry (283, 1, 36, 1, 81), 
			dActionEntry (284, 1, 36, 1, 81), dActionEntry (125, 1, 36, 1, 79), dActionEntry (261, 1, 36, 1, 79), dActionEntry (263, 1, 36, 1, 79), 
			dActionEntry (266, 1, 36, 1, 79), dActionEntry (267, 1, 36, 1, 79), dActionEntry (268, 1, 36, 1, 79), dActionEntry (269, 1, 36, 1, 79), 
			dActionEntry (270, 1, 36, 1, 79), dActionEntry (271, 1, 36, 1, 79), dActionEntry (272, 1, 36, 1, 79), dActionEntry (273, 1, 36, 1, 79), 
			dActionEntry (274, 1, 36, 1, 79), dActionEntry (275, 1, 36, 1, 79), dActionEntry (276, 1, 36, 1, 79), dActionEntry (277, 1, 36, 1, 79), 
			dActionEntry (280, 1, 36, 1, 79), dActionEntry (283, 1, 36, 1, 79), dActionEntry (284, 1, 36, 1, 79), dActionEntry (125, 1, 33, 1, 70), 
			dActionEntry (261, 1, 33, 1, 70), dActionEntry (263, 1, 33, 1, 70), dActionEntry (266, 1, 33, 1, 70), dActionEntry (267, 1, 33, 1, 70), 
			dActionEntry (268, 1, 33, 1, 70), dActionEntry (269, 1, 33, 1, 70), dActionEntry (270, 1, 33, 1, 70), dActionEntry (271, 1, 33, 1, 70), 
			dActionEntry (272, 1, 33, 1, 70), dActionEntry (273, 1, 33, 1, 70), dActionEntry (274, 1, 33, 1, 70), dActionEntry (275, 1, 33, 1, 70), 
			dActionEntry (276, 1, 33, 1, 70), dActionEntry (277, 1, 33, 1, 70), dActionEntry (280, 1, 33, 1, 70), dActionEntry (283, 1, 33, 1, 70), 
			dActionEntry (284, 1, 33, 1, 70), dActionEntry (125, 0, 111, 0, 0), dActionEntry (261, 0, 107, 0, 0), dActionEntry (263, 0, 69, 0, 0), 
			dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), 
			dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), 
			dActionEntry (274, 0, 68, 0, 0), dActionEntry (275, 0, 73, 0, 0), dActionEntry (276, 0, 67, 0, 0), dActionEntry (277, 0, 70, 0, 0), 
			dActionEntry (280, 0, 72, 0, 0), dActionEntry (283, 0, 66, 0, 0), dActionEntry (284, 0, 65, 0, 0), dActionEntry (125, 1, 36, 1, 78), 
			dActionEntry (261, 1, 36, 1, 78), dActionEntry (263, 1, 36, 1, 78), dActionEntry (266, 1, 36, 1, 78), dActionEntry (267, 1, 36, 1, 78), 
			dActionEntry (268, 1, 36, 1, 78), dActionEntry (269, 1, 36, 1, 78), dActionEntry (270, 1, 36, 1, 78), dActionEntry (271, 1, 36, 1, 78), 
			dActionEntry (272, 1, 36, 1, 78), dActionEntry (273, 1, 36, 1, 78), dActionEntry (274, 1, 36, 1, 78), dActionEntry (275, 1, 36, 1, 78), 
			dActionEntry (276, 1, 36, 1, 78), dActionEntry (277, 1, 36, 1, 78), dActionEntry (280, 1, 36, 1, 78), dActionEntry (283, 1, 36, 1, 78), 
			dActionEntry (284, 1, 36, 1, 78), dActionEntry (125, 1, 35, 1, 75), dActionEntry (261, 1, 35, 1, 75), dActionEntry (263, 1, 35, 1, 75), 
			dActionEntry (266, 1, 35, 1, 75), dActionEntry (267, 1, 35, 1, 75), dActionEntry (268, 1, 35, 1, 75), dActionEntry (269, 1, 35, 1, 75), 
			dActionEntry (270, 1, 35, 1, 75), dActionEntry (271, 1, 35, 1, 75), dActionEntry (272, 1, 35, 1, 75), dActionEntry (273, 1, 35, 1, 75), 
			dActionEntry (274, 1, 35, 1, 75), dActionEntry (275, 1, 35, 1, 75), dActionEntry (276, 1, 35, 1, 75), dActionEntry (277, 1, 35, 1, 75), 
			dActionEntry (280, 1, 35, 1, 75), dActionEntry (283, 1, 35, 1, 75), dActionEntry (284, 1, 35, 1, 75), dActionEntry (125, 1, 34, 1, 72), 
			dActionEntry (261, 1, 34, 1, 72), dActionEntry (263, 1, 34, 1, 72), dActionEntry (266, 1, 34, 1, 72), dActionEntry (267, 1, 34, 1, 72), 
			dActionEntry (268, 1, 34, 1, 72), dActionEntry (269, 1, 34, 1, 72), dActionEntry (270, 1, 34, 1, 72), dActionEntry (271, 1, 34, 1, 72), 
			dActionEntry (272, 1, 34, 1, 72), dActionEntry (273, 1, 34, 1, 72), dActionEntry (274, 1, 34, 1, 72), dActionEntry (275, 1, 34, 1, 72), 
			dActionEntry (276, 1, 34, 1, 72), dActionEntry (277, 1, 34, 1, 72), dActionEntry (280, 1, 34, 1, 72), dActionEntry (283, 1, 34, 1, 72), 
			dActionEntry (284, 1, 34, 1, 72), dActionEntry (125, 1, 34, 1, 73), dActionEntry (261, 1, 34, 1, 73), dActionEntry (263, 1, 34, 1, 73), 
			dActionEntry (266, 1, 34, 1, 73), dActionEntry (267, 1, 34, 1, 73), dActionEntry (268, 1, 34, 1, 73), dActionEntry (269, 1, 34, 1, 73), 
			dActionEntry (270, 1, 34, 1, 73), dActionEntry (271, 1, 34, 1, 73), dActionEntry (272, 1, 34, 1, 73), dActionEntry (273, 1, 34, 1, 73), 
			dActionEntry (274, 1, 34, 1, 73), dActionEntry (275, 1, 34, 1, 73), dActionEntry (276, 1, 34, 1, 73), dActionEntry (277, 1, 34, 1, 73), 
			dActionEntry (280, 1, 34, 1, 73), dActionEntry (283, 1, 34, 1, 73), dActionEntry (284, 1, 34, 1, 73), dActionEntry (125, 1, 36, 1, 80), 
			dActionEntry (261, 1, 36, 1, 80), dActionEntry (263, 1, 36, 1, 80), dActionEntry (266, 1, 36, 1, 80), dActionEntry (267, 1, 36, 1, 80), 
			dActionEntry (268, 1, 36, 1, 80), dActionEntry (269, 1, 36, 1, 80), dActionEntry (270, 1, 36, 1, 80), dActionEntry (271, 1, 36, 1, 80), 
			dActionEntry (272, 1, 36, 1, 80), dActionEntry (273, 1, 36, 1, 80), dActionEntry (274, 1, 36, 1, 80), dActionEntry (275, 1, 36, 1, 80), 
			dActionEntry (276, 1, 36, 1, 80), dActionEntry (277, 1, 36, 1, 80), dActionEntry (280, 1, 36, 1, 80), dActionEntry (283, 1, 36, 1, 80), 
			dActionEntry (284, 1, 36, 1, 80), dActionEntry (263, 0, 112, 0, 0), dActionEntry (61, 0, 113, 0, 0), dActionEntry (125, 1, 35, 1, 74), 
			dActionEntry (261, 1, 35, 1, 74), dActionEntry (263, 1, 35, 1, 74), dActionEntry (266, 1, 35, 1, 74), dActionEntry (267, 1, 35, 1, 74), 
			dActionEntry (268, 1, 35, 1, 74), dActionEntry (269, 1, 35, 1, 74), dActionEntry (270, 1, 35, 1, 74), dActionEntry (271, 1, 35, 1, 74), 
			dActionEntry (272, 1, 35, 1, 74), dActionEntry (273, 1, 35, 1, 74), dActionEntry (274, 1, 35, 1, 74), dActionEntry (275, 1, 35, 1, 74), 
			dActionEntry (276, 1, 35, 1, 74), dActionEntry (277, 1, 35, 1, 74), dActionEntry (280, 1, 35, 1, 74), dActionEntry (283, 1, 35, 1, 74), 
			dActionEntry (284, 1, 35, 1, 74), dActionEntry (125, 1, 20, 2, 68), dActionEntry (256, 1, 20, 2, 68), dActionEntry (261, 1, 20, 2, 68), 
			dActionEntry (263, 1, 20, 2, 68), dActionEntry (266, 1, 20, 2, 68), dActionEntry (267, 1, 20, 2, 68), dActionEntry (268, 1, 20, 2, 68), 
			dActionEntry (269, 1, 20, 2, 68), dActionEntry (270, 1, 20, 2, 68), dActionEntry (271, 1, 20, 2, 68), dActionEntry (272, 1, 20, 2, 68), 
			dActionEntry (273, 1, 20, 2, 68), dActionEntry (40, 0, 115, 0, 0), dActionEntry (93, 1, 25, 1, 76), dActionEntry (91, 1, 28, 2, 57), 
			dActionEntry (263, 1, 28, 2, 57), dActionEntry (93, 0, 116, 0, 0), dActionEntry (91, 1, 27, 2, 56), dActionEntry (263, 1, 27, 2, 56), 
			dActionEntry (41, 0, 118, 0, 0), dActionEntry (41, 1, 15, 1, 25), dActionEntry (44, 1, 15, 1, 25), dActionEntry (41, 1, 17, 1, 28), 
			dActionEntry (44, 0, 119, 0, 0), dActionEntry (263, 0, 120, 0, 0), dActionEntry (275, 0, 121, 0, 0), dActionEntry (125, 0, 123, 0, 0), 
			dActionEntry (261, 0, 71, 0, 0), dActionEntry (263, 0, 69, 0, 0), dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), 
			dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), dActionEntry (274, 0, 68, 0, 0), dActionEntry (275, 0, 73, 0, 0), 
			dActionEntry (276, 0, 67, 0, 0), dActionEntry (277, 0, 70, 0, 0), dActionEntry (280, 0, 72, 0, 0), dActionEntry (283, 0, 66, 0, 0), 
			dActionEntry (284, 0, 65, 0, 0), dActionEntry (258, 0, 125, 0, 0), dActionEntry (258, 0, 127, 0, 0), dActionEntry (263, 0, 129, 0, 0), 
			dActionEntry (125, 1, 33, 2, 71), dActionEntry (261, 1, 33, 2, 71), dActionEntry (263, 1, 33, 2, 71), dActionEntry (266, 1, 33, 2, 71), 
			dActionEntry (267, 1, 33, 2, 71), dActionEntry (268, 1, 33, 2, 71), dActionEntry (269, 1, 33, 2, 71), dActionEntry (270, 1, 33, 2, 71), 
			dActionEntry (271, 1, 33, 2, 71), dActionEntry (272, 1, 33, 2, 71), dActionEntry (273, 1, 33, 2, 71), dActionEntry (274, 1, 33, 2, 71), 
			dActionEntry (275, 1, 33, 2, 71), dActionEntry (276, 1, 33, 2, 71), dActionEntry (277, 1, 33, 2, 71), dActionEntry (280, 1, 33, 2, 71), 
			dActionEntry (283, 1, 33, 2, 71), dActionEntry (284, 1, 33, 2, 71), dActionEntry (263, 0, 133, 0, 0), dActionEntry (125, 1, 20, 3, 69), 
			dActionEntry (256, 1, 20, 3, 69), dActionEntry (261, 1, 20, 3, 69), dActionEntry (263, 1, 20, 3, 69), dActionEntry (266, 1, 20, 3, 69), 
			dActionEntry (267, 1, 20, 3, 69), dActionEntry (268, 1, 20, 3, 69), dActionEntry (269, 1, 20, 3, 69), dActionEntry (270, 1, 20, 3, 69), 
			dActionEntry (271, 1, 20, 3, 69), dActionEntry (272, 1, 20, 3, 69), dActionEntry (273, 1, 20, 3, 69), dActionEntry (61, 1, 23, 2, 37), 
			dActionEntry (125, 1, 23, 2, 37), dActionEntry (261, 1, 23, 2, 37), dActionEntry (263, 1, 23, 2, 37), dActionEntry (266, 1, 23, 2, 37), 
			dActionEntry (267, 1, 23, 2, 37), dActionEntry (268, 1, 23, 2, 37), dActionEntry (269, 1, 23, 2, 37), dActionEntry (270, 1, 23, 2, 37), 
			dActionEntry (271, 1, 23, 2, 37), dActionEntry (272, 1, 23, 2, 37), dActionEntry (273, 1, 23, 2, 37), dActionEntry (274, 1, 23, 2, 37), 
			dActionEntry (275, 1, 23, 2, 37), dActionEntry (276, 1, 23, 2, 37), dActionEntry (277, 1, 23, 2, 37), dActionEntry (280, 1, 23, 2, 37), 
			dActionEntry (283, 1, 23, 2, 37), dActionEntry (284, 1, 23, 2, 37), dActionEntry (41, 0, 135, 0, 0), dActionEntry (91, 1, 28, 3, 58), 
			dActionEntry (263, 1, 28, 3, 58), dActionEntry (41, 0, 137, 0, 0), dActionEntry (58, 0, 138, 0, 0), dActionEntry (123, 1, 19, 0, 59), 
			dActionEntry (41, 1, 16, 2, 41), dActionEntry (44, 1, 16, 2, 41), dActionEntry (40, 0, 141, 0, 0), dActionEntry (125, 0, 142, 0, 0), 
			dActionEntry (261, 0, 107, 0, 0), dActionEntry (263, 0, 69, 0, 0), dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), 
			dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), dActionEntry (274, 0, 68, 0, 0), dActionEntry (275, 0, 73, 0, 0), 
			dActionEntry (276, 0, 67, 0, 0), dActionEntry (277, 0, 70, 0, 0), dActionEntry (280, 0, 72, 0, 0), dActionEntry (283, 0, 66, 0, 0), 
			dActionEntry (284, 0, 65, 0, 0), dActionEntry (275, 1, 20, 2, 68), dActionEntry (263, 0, 143, 0, 0), dActionEntry (125, 1, 25, 1, 76), 
			dActionEntry (261, 1, 25, 1, 76), dActionEntry (263, 1, 25, 1, 76), dActionEntry (266, 1, 25, 1, 76), dActionEntry (267, 1, 25, 1, 76), 
			dActionEntry (268, 1, 25, 1, 76), dActionEntry (269, 1, 25, 1, 76), dActionEntry (270, 1, 25, 1, 76), dActionEntry (271, 1, 25, 1, 76), 
			dActionEntry (272, 1, 25, 1, 76), dActionEntry (273, 1, 25, 1, 76), dActionEntry (274, 1, 25, 1, 76), dActionEntry (275, 1, 25, 1, 76), 
			dActionEntry (276, 1, 25, 1, 76), dActionEntry (277, 1, 25, 1, 76), dActionEntry (280, 1, 25, 1, 76), dActionEntry (283, 1, 25, 1, 76), 
			dActionEntry (284, 1, 25, 1, 76), dActionEntry (125, 1, 37, 3, 83), dActionEntry (261, 1, 37, 3, 83), dActionEntry (263, 1, 37, 3, 83), 
			dActionEntry (266, 1, 37, 3, 83), dActionEntry (267, 1, 37, 3, 83), dActionEntry (268, 1, 37, 3, 83), dActionEntry (269, 1, 37, 3, 83), 
			dActionEntry (270, 1, 37, 3, 83), dActionEntry (271, 1, 37, 3, 83), dActionEntry (272, 1, 37, 3, 83), dActionEntry (273, 1, 37, 3, 83), 
			dActionEntry (274, 1, 37, 3, 83), dActionEntry (275, 1, 37, 3, 83), dActionEntry (276, 1, 37, 3, 83), dActionEntry (277, 1, 37, 3, 83), 
			dActionEntry (280, 1, 37, 3, 83), dActionEntry (283, 1, 37, 3, 83), dActionEntry (284, 1, 37, 3, 83), dActionEntry (41, 1, 25, 1, 76), 
			dActionEntry (41, 0, 145, 0, 0), dActionEntry (61, 1, 23, 3, 38), dActionEntry (125, 1, 23, 3, 38), dActionEntry (261, 1, 23, 3, 38), 
			dActionEntry (263, 1, 23, 3, 38), dActionEntry (266, 1, 23, 3, 38), dActionEntry (267, 1, 23, 3, 38), dActionEntry (268, 1, 23, 3, 38), 
			dActionEntry (269, 1, 23, 3, 38), dActionEntry (270, 1, 23, 3, 38), dActionEntry (271, 1, 23, 3, 38), dActionEntry (272, 1, 23, 3, 38), 
			dActionEntry (273, 1, 23, 3, 38), dActionEntry (274, 1, 23, 3, 38), dActionEntry (275, 1, 23, 3, 38), dActionEntry (276, 1, 23, 3, 38), 
			dActionEntry (277, 1, 23, 3, 38), dActionEntry (280, 1, 23, 3, 38), dActionEntry (283, 1, 23, 3, 38), dActionEntry (284, 1, 23, 3, 38), 
			dActionEntry (41, 0, 146, 0, 0), dActionEntry (41, 0, 147, 0, 0), dActionEntry (263, 0, 148, 0, 0), dActionEntry (125, 1, 24, 3, 39), 
			dActionEntry (261, 1, 24, 3, 39), dActionEntry (263, 1, 24, 3, 39), dActionEntry (266, 1, 24, 3, 39), dActionEntry (267, 1, 24, 3, 39), 
			dActionEntry (268, 1, 24, 3, 39), dActionEntry (269, 1, 24, 3, 39), dActionEntry (270, 1, 24, 3, 39), dActionEntry (271, 1, 24, 3, 39), 
			dActionEntry (272, 1, 24, 3, 39), dActionEntry (273, 1, 24, 3, 39), dActionEntry (274, 1, 24, 3, 39), dActionEntry (275, 1, 24, 3, 39), 
			dActionEntry (276, 1, 24, 3, 39), dActionEntry (277, 1, 24, 3, 39), dActionEntry (280, 1, 24, 3, 39), dActionEntry (283, 1, 24, 3, 39), 
			dActionEntry (284, 1, 24, 3, 39), dActionEntry (125, 1, 11, 5, 31), dActionEntry (256, 1, 11, 5, 31), dActionEntry (261, 1, 11, 5, 31), 
			dActionEntry (263, 1, 11, 5, 31), dActionEntry (266, 1, 11, 5, 31), dActionEntry (267, 1, 11, 5, 31), dActionEntry (268, 1, 11, 5, 31), 
			dActionEntry (269, 1, 11, 5, 31), dActionEntry (270, 1, 11, 5, 31), dActionEntry (271, 1, 11, 5, 31), dActionEntry (272, 1, 11, 5, 31), 
			dActionEntry (273, 1, 11, 5, 31), dActionEntry (41, 0, 149, 0, 0), dActionEntry (123, 1, 18, 0, 29), dActionEntry (257, 0, 150, 0, 0), 
			dActionEntry (263, 0, 152, 0, 0), dActionEntry (41, 1, 15, 3, 26), dActionEntry (44, 1, 15, 3, 26), dActionEntry (275, 1, 20, 3, 69), 
			dActionEntry (61, 0, 157, 0, 0), dActionEntry (59, 0, 158, 0, 0), dActionEntry (278, 0, 159, 0, 0), dActionEntry (123, 0, 160, 0, 0), 
			dActionEntry (123, 0, 162, 0, 0), dActionEntry (125, 1, 18, 0, 29), dActionEntry (256, 1, 18, 0, 29), dActionEntry (257, 0, 163, 0, 0), 
			dActionEntry (261, 1, 18, 0, 29), dActionEntry (263, 1, 18, 0, 29), dActionEntry (266, 1, 18, 0, 29), dActionEntry (267, 1, 18, 0, 29), 
			dActionEntry (268, 1, 18, 0, 29), dActionEntry (269, 1, 18, 0, 29), dActionEntry (270, 1, 18, 0, 29), dActionEntry (271, 1, 18, 0, 29), 
			dActionEntry (272, 1, 18, 0, 29), dActionEntry (273, 1, 18, 0, 29), dActionEntry (123, 1, 18, 1, 30), dActionEntry (123, 1, 21, 6, 36), 
			dActionEntry (40, 0, 165, 0, 0), dActionEntry (44, 0, 166, 0, 0), dActionEntry (123, 1, 19, 2, 60), dActionEntry (44, 1, 29, 1, 61), 
			dActionEntry (123, 1, 29, 1, 61), dActionEntry (125, 1, 11, 7, 32), dActionEntry (256, 1, 11, 7, 32), dActionEntry (261, 1, 11, 7, 32), 
			dActionEntry (263, 1, 11, 7, 32), dActionEntry (266, 1, 11, 7, 32), dActionEntry (267, 1, 11, 7, 32), dActionEntry (268, 1, 11, 7, 32), 
			dActionEntry (269, 1, 11, 7, 32), dActionEntry (270, 1, 11, 7, 32), dActionEntry (271, 1, 11, 7, 32), dActionEntry (272, 1, 11, 7, 32), 
			dActionEntry (273, 1, 11, 7, 32), dActionEntry (41, 0, 167, 0, 0), dActionEntry (258, 0, 168, 0, 0), dActionEntry (123, 0, 172, 0, 0), 
			dActionEntry (125, 1, 43, 0, 92), dActionEntry (281, 0, 173, 0, 0), dActionEntry (282, 0, 174, 0, 0), dActionEntry (125, 1, 39, 5, 87), 
			dActionEntry (261, 1, 39, 5, 87), dActionEntry (263, 1, 39, 5, 87), dActionEntry (266, 1, 39, 5, 87), dActionEntry (267, 1, 39, 5, 87), 
			dActionEntry (268, 1, 39, 5, 87), dActionEntry (269, 1, 39, 5, 87), dActionEntry (270, 1, 39, 5, 87), dActionEntry (271, 1, 39, 5, 87), 
			dActionEntry (272, 1, 39, 5, 87), dActionEntry (273, 1, 39, 5, 87), dActionEntry (274, 1, 39, 5, 87), dActionEntry (275, 1, 39, 5, 87), 
			dActionEntry (276, 1, 39, 5, 87), dActionEntry (277, 1, 39, 5, 87), dActionEntry (280, 1, 39, 5, 87), dActionEntry (283, 1, 39, 5, 87), 
			dActionEntry (284, 1, 39, 5, 87), dActionEntry (125, 0, 179, 0, 0), dActionEntry (261, 0, 71, 0, 0), dActionEntry (263, 0, 69, 0, 0), 
			dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), 
			dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), 
			dActionEntry (274, 0, 68, 0, 0), dActionEntry (275, 0, 73, 0, 0), dActionEntry (276, 0, 67, 0, 0), dActionEntry (277, 0, 70, 0, 0), 
			dActionEntry (280, 0, 72, 0, 0), dActionEntry (283, 0, 66, 0, 0), dActionEntry (284, 0, 65, 0, 0), dActionEntry (125, 1, 18, 1, 30), 
			dActionEntry (256, 1, 18, 1, 30), dActionEntry (261, 1, 18, 1, 30), dActionEntry (263, 1, 18, 1, 30), dActionEntry (266, 1, 18, 1, 30), 
			dActionEntry (267, 1, 18, 1, 30), dActionEntry (268, 1, 18, 1, 30), dActionEntry (269, 1, 18, 1, 30), dActionEntry (270, 1, 18, 1, 30), 
			dActionEntry (271, 1, 18, 1, 30), dActionEntry (272, 1, 18, 1, 30), dActionEntry (273, 1, 18, 1, 30), dActionEntry (125, 1, 21, 6, 36), 
			dActionEntry (256, 1, 21, 6, 36), dActionEntry (261, 1, 21, 6, 36), dActionEntry (263, 1, 21, 6, 36), dActionEntry (266, 1, 21, 6, 36), 
			dActionEntry (267, 1, 21, 6, 36), dActionEntry (268, 1, 21, 6, 36), dActionEntry (269, 1, 21, 6, 36), dActionEntry (270, 1, 21, 6, 36), 
			dActionEntry (271, 1, 21, 6, 36), dActionEntry (272, 1, 21, 6, 36), dActionEntry (273, 1, 21, 6, 36), dActionEntry (41, 0, 180, 0, 0), 
			dActionEntry (258, 0, 181, 0, 0), dActionEntry (125, 1, 39, 6, 88), dActionEntry (261, 1, 39, 6, 88), dActionEntry (263, 1, 39, 6, 88), 
			dActionEntry (266, 1, 39, 6, 88), dActionEntry (267, 1, 39, 6, 88), dActionEntry (268, 1, 39, 6, 88), dActionEntry (269, 1, 39, 6, 88), 
			dActionEntry (270, 1, 39, 6, 88), dActionEntry (271, 1, 39, 6, 88), dActionEntry (272, 1, 39, 6, 88), dActionEntry (273, 1, 39, 6, 88), 
			dActionEntry (274, 1, 39, 6, 88), dActionEntry (275, 1, 39, 6, 88), dActionEntry (276, 1, 39, 6, 88), dActionEntry (277, 1, 39, 6, 88), 
			dActionEntry (280, 1, 39, 6, 88), dActionEntry (283, 1, 39, 6, 88), dActionEntry (284, 1, 39, 6, 88), dActionEntry (59, 1, 25, 1, 76), 
			dActionEntry (59, 1, 37, 3, 83), dActionEntry (59, 0, 185, 0, 0), dActionEntry (125, 1, 38, 6, 84), dActionEntry (261, 1, 38, 6, 84), 
			dActionEntry (263, 1, 38, 6, 84), dActionEntry (266, 1, 38, 6, 84), dActionEntry (267, 1, 38, 6, 84), dActionEntry (268, 1, 38, 6, 84), 
			dActionEntry (269, 1, 38, 6, 84), dActionEntry (270, 1, 38, 6, 84), dActionEntry (271, 1, 38, 6, 84), dActionEntry (272, 1, 38, 6, 84), 
			dActionEntry (273, 1, 38, 6, 84), dActionEntry (274, 1, 38, 6, 84), dActionEntry (275, 1, 38, 6, 84), dActionEntry (276, 1, 38, 6, 84), 
			dActionEntry (277, 1, 38, 6, 84), dActionEntry (279, 0, 186, 0, 0), dActionEntry (280, 1, 38, 6, 84), dActionEntry (283, 1, 38, 6, 84), 
			dActionEntry (284, 1, 38, 6, 84), dActionEntry (125, 0, 188, 0, 0), dActionEntry (261, 0, 71, 0, 0), dActionEntry (263, 0, 69, 0, 0), 
			dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), 
			dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), 
			dActionEntry (274, 0, 68, 0, 0), dActionEntry (275, 0, 73, 0, 0), dActionEntry (276, 0, 67, 0, 0), dActionEntry (277, 0, 70, 0, 0), 
			dActionEntry (280, 0, 72, 0, 0), dActionEntry (283, 0, 66, 0, 0), dActionEntry (284, 0, 65, 0, 0), dActionEntry (258, 0, 189, 0, 0), 
			dActionEntry (58, 0, 190, 0, 0), dActionEntry (125, 1, 44, 1, 94), dActionEntry (281, 1, 44, 1, 94), dActionEntry (282, 1, 44, 1, 94), 
			dActionEntry (125, 1, 43, 1, 93), dActionEntry (281, 0, 173, 0, 0), dActionEntry (282, 0, 174, 0, 0), dActionEntry (125, 0, 192, 0, 0), 
			dActionEntry (125, 0, 193, 0, 0), dActionEntry (261, 0, 107, 0, 0), dActionEntry (263, 0, 69, 0, 0), dActionEntry (266, 0, 19, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), 
			dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), dActionEntry (274, 0, 68, 0, 0), 
			dActionEntry (275, 0, 73, 0, 0), dActionEntry (276, 0, 67, 0, 0), dActionEntry (277, 0, 70, 0, 0), dActionEntry (280, 0, 72, 0, 0), 
			dActionEntry (283, 0, 66, 0, 0), dActionEntry (284, 0, 65, 0, 0), dActionEntry (125, 1, 20, 2, 68), dActionEntry (261, 1, 20, 2, 68), 
			dActionEntry (263, 1, 20, 2, 68), dActionEntry (266, 1, 20, 2, 68), dActionEntry (267, 1, 20, 2, 68), dActionEntry (268, 1, 20, 2, 68), 
			dActionEntry (269, 1, 20, 2, 68), dActionEntry (270, 1, 20, 2, 68), dActionEntry (271, 1, 20, 2, 68), dActionEntry (272, 1, 20, 2, 68), 
			dActionEntry (273, 1, 20, 2, 68), dActionEntry (274, 1, 20, 2, 68), dActionEntry (275, 1, 20, 2, 68), dActionEntry (276, 1, 20, 2, 68), 
			dActionEntry (277, 1, 20, 2, 68), dActionEntry (280, 1, 20, 2, 68), dActionEntry (283, 1, 20, 2, 68), dActionEntry (284, 1, 20, 2, 68), 
			dActionEntry (44, 1, 30, 3, 63), dActionEntry (123, 1, 30, 3, 63), dActionEntry (41, 1, 32, 1, 67), dActionEntry (44, 1, 32, 1, 67), 
			dActionEntry (41, 1, 31, 1, 65), dActionEntry (44, 1, 31, 1, 65), dActionEntry (41, 0, 194, 0, 0), dActionEntry (44, 0, 195, 0, 0), 
			dActionEntry (44, 1, 29, 3, 62), dActionEntry (123, 1, 29, 3, 62), dActionEntry (263, 0, 196, 0, 0), dActionEntry (125, 0, 199, 0, 0), 
			dActionEntry (261, 0, 107, 0, 0), dActionEntry (263, 0, 69, 0, 0), dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), 
			dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), dActionEntry (274, 0, 68, 0, 0), dActionEntry (275, 0, 73, 0, 0), 
			dActionEntry (276, 0, 67, 0, 0), dActionEntry (277, 0, 70, 0, 0), dActionEntry (280, 0, 72, 0, 0), dActionEntry (283, 0, 66, 0, 0), 
			dActionEntry (284, 0, 65, 0, 0), dActionEntry (125, 1, 20, 2, 68), dActionEntry (261, 1, 20, 2, 68), dActionEntry (263, 1, 20, 2, 68), 
			dActionEntry (266, 1, 20, 2, 68), dActionEntry (267, 1, 20, 2, 68), dActionEntry (268, 1, 20, 2, 68), dActionEntry (269, 1, 20, 2, 68), 
			dActionEntry (270, 1, 20, 2, 68), dActionEntry (271, 1, 20, 2, 68), dActionEntry (272, 1, 20, 2, 68), dActionEntry (273, 1, 20, 2, 68), 
			dActionEntry (274, 1, 20, 2, 68), dActionEntry (275, 1, 20, 2, 68), dActionEntry (276, 1, 20, 2, 68), dActionEntry (277, 1, 20, 2, 68), 
			dActionEntry (279, 1, 20, 2, 68), dActionEntry (280, 1, 20, 2, 68), dActionEntry (283, 1, 20, 2, 68), dActionEntry (284, 1, 20, 2, 68), 
			dActionEntry (58, 0, 200, 0, 0), dActionEntry (123, 0, 202, 0, 0), dActionEntry (125, 1, 44, 2, 95), dActionEntry (281, 1, 44, 2, 95), 
			dActionEntry (282, 1, 44, 2, 95), dActionEntry (125, 1, 38, 7, 86), dActionEntry (261, 1, 38, 7, 86), dActionEntry (263, 1, 38, 7, 86), 
			dActionEntry (266, 1, 38, 7, 86), dActionEntry (267, 1, 38, 7, 86), dActionEntry (268, 1, 38, 7, 86), dActionEntry (269, 1, 38, 7, 86), 
			dActionEntry (270, 1, 38, 7, 86), dActionEntry (271, 1, 38, 7, 86), dActionEntry (272, 1, 38, 7, 86), dActionEntry (273, 1, 38, 7, 86), 
			dActionEntry (274, 1, 38, 7, 86), dActionEntry (275, 1, 38, 7, 86), dActionEntry (276, 1, 38, 7, 86), dActionEntry (277, 1, 38, 7, 86), 
			dActionEntry (280, 1, 38, 7, 86), dActionEntry (283, 1, 38, 7, 86), dActionEntry (284, 1, 38, 7, 86), dActionEntry (125, 1, 20, 3, 69), 
			dActionEntry (261, 1, 20, 3, 69), dActionEntry (263, 1, 20, 3, 69), dActionEntry (266, 1, 20, 3, 69), dActionEntry (267, 1, 20, 3, 69), 
			dActionEntry (268, 1, 20, 3, 69), dActionEntry (269, 1, 20, 3, 69), dActionEntry (270, 1, 20, 3, 69), dActionEntry (271, 1, 20, 3, 69), 
			dActionEntry (272, 1, 20, 3, 69), dActionEntry (273, 1, 20, 3, 69), dActionEntry (274, 1, 20, 3, 69), dActionEntry (275, 1, 20, 3, 69), 
			dActionEntry (276, 1, 20, 3, 69), dActionEntry (277, 1, 20, 3, 69), dActionEntry (280, 1, 20, 3, 69), dActionEntry (283, 1, 20, 3, 69), 
			dActionEntry (284, 1, 20, 3, 69), dActionEntry (44, 1, 30, 4, 64), dActionEntry (123, 1, 30, 4, 64), dActionEntry (258, 0, 181, 0, 0), 
			dActionEntry (61, 0, 204, 0, 0), dActionEntry (41, 0, 205, 0, 0), dActionEntry (125, 1, 38, 8, 85), dActionEntry (261, 1, 38, 8, 85), 
			dActionEntry (263, 1, 38, 8, 85), dActionEntry (266, 1, 38, 8, 85), dActionEntry (267, 1, 38, 8, 85), dActionEntry (268, 1, 38, 8, 85), 
			dActionEntry (269, 1, 38, 8, 85), dActionEntry (270, 1, 38, 8, 85), dActionEntry (271, 1, 38, 8, 85), dActionEntry (272, 1, 38, 8, 85), 
			dActionEntry (273, 1, 38, 8, 85), dActionEntry (274, 1, 38, 8, 85), dActionEntry (275, 1, 38, 8, 85), dActionEntry (276, 1, 38, 8, 85), 
			dActionEntry (277, 1, 38, 8, 85), dActionEntry (280, 1, 38, 8, 85), dActionEntry (283, 1, 38, 8, 85), dActionEntry (284, 1, 38, 8, 85), 
			dActionEntry (125, 1, 20, 3, 69), dActionEntry (261, 1, 20, 3, 69), dActionEntry (263, 1, 20, 3, 69), dActionEntry (266, 1, 20, 3, 69), 
			dActionEntry (267, 1, 20, 3, 69), dActionEntry (268, 1, 20, 3, 69), dActionEntry (269, 1, 20, 3, 69), dActionEntry (270, 1, 20, 3, 69), 
			dActionEntry (271, 1, 20, 3, 69), dActionEntry (272, 1, 20, 3, 69), dActionEntry (273, 1, 20, 3, 69), dActionEntry (274, 1, 20, 3, 69), 
			dActionEntry (275, 1, 20, 3, 69), dActionEntry (276, 1, 20, 3, 69), dActionEntry (277, 1, 20, 3, 69), dActionEntry (279, 1, 20, 3, 69), 
			dActionEntry (280, 1, 20, 3, 69), dActionEntry (283, 1, 20, 3, 69), dActionEntry (284, 1, 20, 3, 69), dActionEntry (125, 1, 45, 3, 97), 
			dActionEntry (281, 1, 45, 3, 97), dActionEntry (282, 1, 45, 3, 97), dActionEntry (125, 0, 208, 0, 0), dActionEntry (261, 0, 71, 0, 0), 
			dActionEntry (263, 0, 69, 0, 0), dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), 
			dActionEntry (269, 0, 23, 0, 0), dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), 
			dActionEntry (273, 0, 21, 0, 0), dActionEntry (274, 0, 68, 0, 0), dActionEntry (275, 0, 73, 0, 0), dActionEntry (276, 0, 67, 0, 0), 
			dActionEntry (277, 0, 70, 0, 0), dActionEntry (280, 0, 72, 0, 0), dActionEntry (283, 0, 66, 0, 0), dActionEntry (284, 0, 65, 0, 0), 
			dActionEntry (41, 1, 31, 3, 66), dActionEntry (44, 1, 31, 3, 66), dActionEntry (125, 1, 45, 4, 96), dActionEntry (281, 1, 45, 4, 96), 
			dActionEntry (282, 1, 45, 4, 96), dActionEntry (125, 0, 211, 0, 0), dActionEntry (261, 0, 107, 0, 0), dActionEntry (263, 0, 69, 0, 0), 
			dActionEntry (266, 0, 19, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 27, 0, 0), dActionEntry (269, 0, 23, 0, 0), 
			dActionEntry (270, 0, 24, 0, 0), dActionEntry (271, 0, 28, 0, 0), dActionEntry (272, 0, 22, 0, 0), dActionEntry (273, 0, 21, 0, 0), 
			dActionEntry (274, 0, 68, 0, 0), dActionEntry (275, 0, 73, 0, 0), dActionEntry (276, 0, 67, 0, 0), dActionEntry (277, 0, 70, 0, 0), 
			dActionEntry (280, 0, 72, 0, 0), dActionEntry (283, 0, 66, 0, 0), dActionEntry (284, 0, 65, 0, 0), dActionEntry (125, 1, 20, 2, 68), 
			dActionEntry (281, 1, 20, 2, 68), dActionEntry (282, 1, 20, 2, 68), dActionEntry (41, 1, 37, 3, 83), dActionEntry (125, 1, 39, 10, 89), 
			dActionEntry (261, 1, 39, 10, 89), dActionEntry (263, 1, 39, 10, 89), dActionEntry (266, 1, 39, 10, 89), dActionEntry (267, 1, 39, 10, 89), 
			dActionEntry (268, 1, 39, 10, 89), dActionEntry (269, 1, 39, 10, 89), dActionEntry (270, 1, 39, 10, 89), dActionEntry (271, 1, 39, 10, 89), 
			dActionEntry (272, 1, 39, 10, 89), dActionEntry (273, 1, 39, 10, 89), dActionEntry (274, 1, 39, 10, 89), dActionEntry (275, 1, 39, 10, 89), 
			dActionEntry (276, 1, 39, 10, 89), dActionEntry (277, 1, 39, 10, 89), dActionEntry (280, 1, 39, 10, 89), dActionEntry (283, 1, 39, 10, 89), 
			dActionEntry (284, 1, 39, 10, 89), dActionEntry (125, 1, 20, 3, 69), dActionEntry (281, 1, 20, 3, 69), dActionEntry (282, 1, 20, 3, 69)};

	const int lastToken = 285;

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



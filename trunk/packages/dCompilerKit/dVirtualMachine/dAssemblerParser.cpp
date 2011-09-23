/* Copych1 (c) <2009> <Newton Game Dynamics>
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
//Auto generated Parser Generator class: dAssemblerParser.cpp
//

#include <dVirtualMachine.h>
#include "dAssemblerLexical.h"
#include "dAssemblerCompiler.h"
//
// Newton virtual machine assembler grammar
// based loosely on a subset and a hybrid between the MIPS R3000 and the Intel 386 instructions set 
//

#include "dAssemblerParser.h"
#include <dList.h>

#define MAX_USER_PARAM	64

enum dAssemblerParser::ActionType
{
	dSHIFT = 0,
	dREDUCE,
	dACCEPT,
	dERROR
};


class dAssemblerParser::dActionEntry
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

class dAssemblerParser::dGotoEntry
{
	public:
	dGotoEntry (short token, short nextState)
		:m_token(token), m_nextState(nextState)
	{
	}

	short  m_token;
	short  m_nextState;
};



class dAssemblerParser::dStackPair
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


dAssemblerParser::dAssemblerParser()
{
}

dAssemblerParser::~dAssemblerParser()
{
}


bool dAssemblerParser::ErrorHandler (const string& line) const
{
	line;
	return false;
}

const dAssemblerParser::dActionEntry* dAssemblerParser::FindAction (const dActionEntry* const actionList, int count, dToken token) const
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

const dAssemblerParser::dGotoEntry* dAssemblerParser::FindGoto (const dGotoEntry* const gotoList, int count, dToken token) const
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


bool dAssemblerParser::Parse(dAssemblerLexical& scanner)
{
	dList<dStackPair> stack;
	static short actionsCount[] = {
			4, 1, 1, 1, 37, 4, 1, 4, 4, 1, 4, 4, 37, 38, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			1, 2, 1, 1, 1, 1, 1, 38, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 39, 1, 1, 1, 1, 
			1, 1, 1, 1, 38, 1, 38, 38, 1, 38, 38, 38, 38, 38, 1, 2, 38, 38, 1, 5, 5, 4, 37, 37, 
			2, 2, 2, 1, 1, 38, 38, 1, 39, 39, 1, 1, 38, 38, 38, 38, 1, 1, 1, 4, 38, 1, 40, 40, 
			1, 1, 1, 1, 4, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 4, 4, 2, 38, 
			38, 38, 1, 38, 38, 1, 38, 1, 38, 1, 38, 1, 38, 1, 38, 38, 2, 1, 1, 2, 2, 2, 38, 38, 
			38, 38, 38, 38, 38};
	static short actionsStart[] = {
			0, 4, 5, 6, 7, 44, 48, 49, 53, 57, 58, 62, 66, 103, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 
			151, 152, 154, 155, 144, 144, 156, 157, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 206, 208, 247, 248, 249, 250, 
			251, 252, 144, 253, 254, 144, 292, 330, 144, 368, 406, 444, 482, 520, 144, 558, 560, 598, 144, 636, 641, 646, 650, 687, 
			724, 726, 728, 730, 731, 732, 770, 808, 809, 848, 887, 888, 889, 927, 965, 1003, 1041, 1042, 1043, 1044, 1048, 1086, 1087, 1127, 
			1167, 1168, 1169, 141, 1170, 1174, 195, 156, 144, 156, 144, 156, 144, 195, 1177, 156, 1178, 1180, 1182, 195, 1184, 1188, 1192, 1194, 
			1232, 1270, 1308, 1309, 1347, 1385, 1386, 1424, 1425, 1463, 1464, 1167, 1502, 1167, 1540, 1578, 1616, 1618, 195, 1616, 1619, 1621, 1623, 1661, 
			1699, 1737, 1775, 1813, 1851};
	static short gotoCount[] = {
			8, 1, 0, 0, 15, 0, 1, 0, 0, 0, 0, 6, 0, 1, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 1, 1, 1, 0, 
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 14, 0, 0, 1, 1, 
			0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
			2, 0, 1, 1, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 2, 0, 1, 0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	static short gotoStart[] = {
			0, 8, 9, 9, 9, 24, 24, 25, 25, 25, 25, 25, 31, 31, 32, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35, 35, 37, 38, 38, 39, 40, 41, 
			41, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 60, 60, 60, 61, 
			62, 62, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 
			64, 66, 66, 67, 68, 68, 71, 72, 73, 74, 75, 76, 77, 78, 79, 79, 80, 80, 80, 80, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 
			81, 81, 81, 81, 83, 83, 84, 84, 84, 86, 87, 88, 90, 90, 90, 90, 90, 90, 90, 90, 90};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (301, 9), dGotoEntry (302, 11), dGotoEntry (303, 10), dGotoEntry (304, 8), dGotoEntry (305, 5), 
			dGotoEntry (306, 7), dGotoEntry (309, 6), dGotoEntry (328, 4), dGotoEntry (308, 13), dGotoEntry (311, 59), 
			dGotoEntry (312, 58), dGotoEntry (313, 61), dGotoEntry (314, 65), dGotoEntry (315, 57), dGotoEntry (316, 52), 
			dGotoEntry (317, 55), dGotoEntry (318, 54), dGotoEntry (319, 64), dGotoEntry (320, 60), dGotoEntry (321, 62), 
			dGotoEntry (323, 66), dGotoEntry (325, 63), dGotoEntry (326, 53), dGotoEntry (327, 56), dGotoEntry (308, 68), 
			dGotoEntry (303, 69), dGotoEntry (304, 8), dGotoEntry (305, 5), dGotoEntry (306, 7), dGotoEntry (309, 6), 
			dGotoEntry (328, 4), dGotoEntry (324, 71), dGotoEntry (307, 73), dGotoEntry (308, 74), dGotoEntry (322, 76), 
			dGotoEntry (308, 78), dGotoEntry (322, 79), dGotoEntry (322, 81), dGotoEntry (322, 82), dGotoEntry (322, 83), 
			dGotoEntry (322, 85), dGotoEntry (308, 86), dGotoEntry (322, 87), dGotoEntry (322, 88), dGotoEntry (322, 89), 
			dGotoEntry (322, 90), dGotoEntry (312, 92), dGotoEntry (313, 61), dGotoEntry (314, 65), dGotoEntry (315, 57), 
			dGotoEntry (316, 52), dGotoEntry (317, 55), dGotoEntry (318, 54), dGotoEntry (319, 64), dGotoEntry (320, 60), 
			dGotoEntry (321, 62), dGotoEntry (323, 66), dGotoEntry (325, 63), dGotoEntry (326, 53), dGotoEntry (327, 56), 
			dGotoEntry (322, 93), dGotoEntry (322, 95), dGotoEntry (322, 97), dGotoEntry (322, 111), dGotoEntry (322, 113), 
			dGotoEntry (329, 114), dGotoEntry (310, 117), dGotoEntry (308, 118), dGotoEntry (308, 121), dGotoEntry (310, 120), 
			dGotoEntry (322, 122), dGotoEntry (308, 123), dGotoEntry (322, 124), dGotoEntry (322, 125), dGotoEntry (322, 126), 
			dGotoEntry (322, 127), dGotoEntry (322, 128), dGotoEntry (322, 129), dGotoEntry (308, 130), dGotoEntry (322, 132), 
			dGotoEntry (308, 135), dGotoEntry (322, 113), dGotoEntry (329, 140), dGotoEntry (322, 141), dGotoEntry (308, 143), 
			dGotoEntry (310, 142), dGotoEntry (310, 144), dGotoEntry (308, 145), dGotoEntry (308, 147), dGotoEntry (310, 146)};
	static dActionEntry actionTable[] = {
			dActionEntry (255, 1, 0, 0, 2), dActionEntry (256, 0, 2, 0, 0), dActionEntry (259, 0, 1, 0, 0), dActionEntry (261, 0, 3, 0, 0), 
			dActionEntry (257, 0, 12, 0, 0), dActionEntry (60, 0, 14, 0, 0), dActionEntry (257, 1, 8, 1, 13), dActionEntry (264, 0, 36, 0, 0), 
			dActionEntry (265, 0, 33, 0, 0), dActionEntry (266, 0, 35, 0, 0), dActionEntry (267, 0, 34, 0, 0), dActionEntry (268, 0, 46, 0, 0), 
			dActionEntry (269, 0, 48, 0, 0), dActionEntry (270, 0, 47, 0, 0), dActionEntry (271, 0, 44, 0, 0), dActionEntry (272, 0, 45, 0, 0), 
			dActionEntry (273, 0, 18, 0, 0), dActionEntry (274, 0, 40, 0, 0), dActionEntry (275, 0, 51, 0, 0), dActionEntry (276, 0, 39, 0, 0), 
			dActionEntry (277, 0, 17, 0, 0), dActionEntry (278, 0, 16, 0, 0), dActionEntry (279, 0, 49, 0, 0), dActionEntry (280, 0, 37, 0, 0), 
			dActionEntry (281, 0, 27, 0, 0), dActionEntry (282, 0, 15, 0, 0), dActionEntry (283, 0, 38, 0, 0), dActionEntry (284, 0, 19, 0, 0), 
			dActionEntry (285, 0, 24, 0, 0), dActionEntry (286, 0, 23, 0, 0), dActionEntry (287, 0, 22, 0, 0), dActionEntry (288, 0, 21, 0, 0), 
			dActionEntry (289, 0, 20, 0, 0), dActionEntry (290, 0, 25, 0, 0), dActionEntry (291, 0, 26, 0, 0), dActionEntry (292, 0, 43, 0, 0), 
			dActionEntry (293, 0, 50, 0, 0), dActionEntry (294, 0, 30, 0, 0), dActionEntry (295, 0, 32, 0, 0), dActionEntry (296, 0, 28, 0, 0), 
			dActionEntry (297, 0, 29, 0, 0), dActionEntry (298, 0, 42, 0, 0), dActionEntry (299, 0, 41, 0, 0), dActionEntry (300, 0, 31, 0, 0), 
			dActionEntry (255, 1, 2, 1, 6), dActionEntry (256, 1, 2, 1, 6), dActionEntry (259, 1, 2, 1, 6), dActionEntry (261, 1, 2, 1, 6), 
			dActionEntry (257, 0, 67, 0, 0), dActionEntry (255, 1, 2, 1, 7), dActionEntry (256, 1, 2, 1, 7), dActionEntry (259, 1, 2, 1, 7), 
			dActionEntry (261, 1, 2, 1, 7), dActionEntry (255, 1, 2, 1, 5), dActionEntry (256, 1, 2, 1, 5), dActionEntry (259, 1, 2, 1, 5), 
			dActionEntry (261, 1, 2, 1, 5), dActionEntry (255, 2, 0, 0, 0), dActionEntry (255, 1, 1, 1, 3), dActionEntry (256, 1, 1, 1, 3), 
			dActionEntry (259, 1, 1, 1, 3), dActionEntry (261, 1, 1, 1, 3), dActionEntry (255, 1, 0, 1, 1), dActionEntry (256, 0, 2, 0, 0), 
			dActionEntry (259, 0, 1, 0, 0), dActionEntry (261, 0, 3, 0, 0), dActionEntry (264, 1, 7, 1, 14), dActionEntry (265, 1, 7, 1, 14), 
			dActionEntry (266, 1, 7, 1, 14), dActionEntry (267, 1, 7, 1, 14), dActionEntry (268, 1, 7, 1, 14), dActionEntry (269, 1, 7, 1, 14), 
			dActionEntry (270, 1, 7, 1, 14), dActionEntry (271, 1, 7, 1, 14), dActionEntry (272, 1, 7, 1, 14), dActionEntry (273, 1, 7, 1, 14), 
			dActionEntry (274, 1, 7, 1, 14), dActionEntry (275, 1, 7, 1, 14), dActionEntry (276, 1, 7, 1, 14), dActionEntry (277, 1, 7, 1, 14), 
			dActionEntry (278, 1, 7, 1, 14), dActionEntry (279, 1, 7, 1, 14), dActionEntry (280, 1, 7, 1, 14), dActionEntry (281, 1, 7, 1, 14), 
			dActionEntry (282, 1, 7, 1, 14), dActionEntry (283, 1, 7, 1, 14), dActionEntry (284, 1, 7, 1, 14), dActionEntry (285, 1, 7, 1, 14), 
			dActionEntry (286, 1, 7, 1, 14), dActionEntry (287, 1, 7, 1, 14), dActionEntry (288, 1, 7, 1, 14), dActionEntry (289, 1, 7, 1, 14), 
			dActionEntry (290, 1, 7, 1, 14), dActionEntry (291, 1, 7, 1, 14), dActionEntry (292, 1, 7, 1, 14), dActionEntry (293, 1, 7, 1, 14), 
			dActionEntry (294, 1, 7, 1, 14), dActionEntry (295, 1, 7, 1, 14), dActionEntry (296, 1, 7, 1, 14), dActionEntry (297, 1, 7, 1, 14), 
			dActionEntry (298, 1, 7, 1, 14), dActionEntry (299, 1, 7, 1, 14), dActionEntry (300, 1, 7, 1, 14), dActionEntry (258, 0, 70, 0, 0), 
			dActionEntry (264, 1, 23, 0, 34), dActionEntry (265, 1, 23, 0, 34), dActionEntry (266, 1, 23, 0, 34), dActionEntry (267, 1, 23, 0, 34), 
			dActionEntry (268, 1, 23, 0, 34), dActionEntry (269, 1, 23, 0, 34), dActionEntry (270, 1, 23, 0, 34), dActionEntry (271, 1, 23, 0, 34), 
			dActionEntry (272, 1, 23, 0, 34), dActionEntry (273, 1, 23, 0, 34), dActionEntry (274, 1, 23, 0, 34), dActionEntry (275, 1, 23, 0, 34), 
			dActionEntry (276, 1, 23, 0, 34), dActionEntry (277, 1, 23, 0, 34), dActionEntry (278, 1, 23, 0, 34), dActionEntry (279, 1, 23, 0, 34), 
			dActionEntry (280, 1, 23, 0, 34), dActionEntry (281, 1, 23, 0, 34), dActionEntry (282, 1, 23, 0, 34), dActionEntry (283, 1, 23, 0, 34), 
			dActionEntry (284, 1, 23, 0, 34), dActionEntry (285, 1, 23, 0, 34), dActionEntry (286, 1, 23, 0, 34), dActionEntry (287, 1, 23, 0, 34), 
			dActionEntry (288, 1, 23, 0, 34), dActionEntry (289, 1, 23, 0, 34), dActionEntry (290, 1, 23, 0, 34), dActionEntry (291, 1, 23, 0, 34), 
			dActionEntry (292, 1, 23, 0, 34), dActionEntry (293, 1, 23, 0, 34), dActionEntry (294, 1, 23, 0, 34), dActionEntry (295, 1, 23, 0, 34), 
			dActionEntry (296, 1, 23, 0, 34), dActionEntry (297, 1, 23, 0, 34), dActionEntry (298, 1, 23, 0, 34), dActionEntry (299, 1, 23, 0, 34), 
			dActionEntry (300, 1, 23, 0, 34), dActionEntry (257, 0, 72, 0, 0), dActionEntry (263, 1, 25, 1, 41), dActionEntry (263, 1, 25, 1, 37), 
			dActionEntry (263, 0, 75, 0, 0), dActionEntry (263, 1, 25, 1, 43), dActionEntry (263, 1, 26, 1, 50), dActionEntry (263, 1, 26, 1, 55), 
			dActionEntry (263, 1, 26, 1, 54), dActionEntry (263, 1, 26, 1, 53), dActionEntry (263, 1, 26, 1, 52), dActionEntry (263, 1, 26, 1, 51), 
			dActionEntry (257, 0, 77, 0, 0), dActionEntry (263, 0, 75, 0, 0), dActionEntry (263, 0, 80, 0, 0), dActionEntry (263, 1, 25, 1, 40), 
			dActionEntry (263, 0, 84, 0, 0), dActionEntry (260, 1, 19, 1, 60), dActionEntry (264, 1, 19, 1, 60), dActionEntry (265, 1, 19, 1, 60), 
			dActionEntry (266, 1, 19, 1, 60), dActionEntry (267, 1, 19, 1, 60), dActionEntry (268, 1, 19, 1, 60), dActionEntry (269, 1, 19, 1, 60), 
			dActionEntry (270, 1, 19, 1, 60), dActionEntry (271, 1, 19, 1, 60), dActionEntry (272, 1, 19, 1, 60), dActionEntry (273, 1, 19, 1, 60), 
			dActionEntry (274, 1, 19, 1, 60), dActionEntry (275, 1, 19, 1, 60), dActionEntry (276, 1, 19, 1, 60), dActionEntry (277, 1, 19, 1, 60), 
			dActionEntry (278, 1, 19, 1, 60), dActionEntry (279, 1, 19, 1, 60), dActionEntry (280, 1, 19, 1, 60), dActionEntry (281, 1, 19, 1, 60), 
			dActionEntry (282, 1, 19, 1, 60), dActionEntry (283, 1, 19, 1, 60), dActionEntry (284, 1, 19, 1, 60), dActionEntry (285, 1, 19, 1, 60), 
			dActionEntry (286, 1, 19, 1, 60), dActionEntry (287, 1, 19, 1, 60), dActionEntry (288, 1, 19, 1, 60), dActionEntry (289, 1, 19, 1, 60), 
			dActionEntry (290, 1, 19, 1, 60), dActionEntry (291, 1, 19, 1, 60), dActionEntry (292, 1, 19, 1, 60), dActionEntry (293, 1, 19, 1, 60), 
			dActionEntry (294, 1, 19, 1, 60), dActionEntry (295, 1, 19, 1, 60), dActionEntry (296, 1, 19, 1, 60), dActionEntry (297, 1, 19, 1, 60), 
			dActionEntry (298, 1, 19, 1, 60), dActionEntry (299, 1, 19, 1, 60), dActionEntry (300, 1, 19, 1, 60), dActionEntry (257, 0, 77, 0, 0), 
			dActionEntry (263, 1, 20, 1, 27), dActionEntry (263, 1, 20, 1, 29), dActionEntry (263, 1, 20, 1, 28), dActionEntry (263, 1, 25, 1, 49), 
			dActionEntry (263, 1, 25, 1, 39), dActionEntry (263, 1, 25, 1, 42), dActionEntry (263, 1, 25, 1, 46), dActionEntry (263, 1, 25, 1, 44), 
			dActionEntry (123, 1, 24, 1, 36), dActionEntry (263, 1, 24, 1, 36), dActionEntry (123, 1, 24, 1, 35), dActionEntry (263, 1, 24, 1, 35), 
			dActionEntry (260, 1, 17, 1, 71), dActionEntry (263, 0, 84, 0, 0), dActionEntry (264, 1, 17, 1, 71), dActionEntry (265, 1, 17, 1, 71), 
			dActionEntry (266, 1, 17, 1, 71), dActionEntry (267, 1, 17, 1, 71), dActionEntry (268, 1, 17, 1, 71), dActionEntry (269, 1, 17, 1, 71), 
			dActionEntry (270, 1, 17, 1, 71), dActionEntry (271, 1, 17, 1, 71), dActionEntry (272, 1, 17, 1, 71), dActionEntry (273, 1, 17, 1, 71), 
			dActionEntry (274, 1, 17, 1, 71), dActionEntry (275, 1, 17, 1, 71), dActionEntry (276, 1, 17, 1, 71), dActionEntry (277, 1, 17, 1, 71), 
			dActionEntry (278, 1, 17, 1, 71), dActionEntry (279, 1, 17, 1, 71), dActionEntry (280, 1, 17, 1, 71), dActionEntry (281, 1, 17, 1, 71), 
			dActionEntry (282, 1, 17, 1, 71), dActionEntry (283, 1, 17, 1, 71), dActionEntry (284, 1, 17, 1, 71), dActionEntry (285, 1, 17, 1, 71), 
			dActionEntry (286, 1, 17, 1, 71), dActionEntry (287, 1, 17, 1, 71), dActionEntry (288, 1, 17, 1, 71), dActionEntry (289, 1, 17, 1, 71), 
			dActionEntry (290, 1, 17, 1, 71), dActionEntry (291, 1, 17, 1, 71), dActionEntry (292, 1, 17, 1, 71), dActionEntry (293, 1, 17, 1, 71), 
			dActionEntry (294, 1, 17, 1, 71), dActionEntry (295, 1, 17, 1, 71), dActionEntry (296, 1, 17, 1, 71), dActionEntry (297, 1, 17, 1, 71), 
			dActionEntry (298, 1, 17, 1, 71), dActionEntry (299, 1, 17, 1, 71), dActionEntry (300, 1, 17, 1, 71), dActionEntry (263, 1, 25, 1, 47), 
			dActionEntry (263, 1, 25, 1, 48), dActionEntry (263, 1, 22, 1, 30), dActionEntry (263, 1, 22, 1, 32), dActionEntry (263, 1, 22, 1, 31), 
			dActionEntry (263, 1, 25, 1, 38), dActionEntry (263, 1, 25, 1, 45), dActionEntry (260, 1, 11, 1, 20), dActionEntry (264, 1, 11, 1, 20), 
			dActionEntry (265, 1, 11, 1, 20), dActionEntry (266, 1, 11, 1, 20), dActionEntry (267, 1, 11, 1, 20), dActionEntry (268, 1, 11, 1, 20), 
			dActionEntry (269, 1, 11, 1, 20), dActionEntry (270, 1, 11, 1, 20), dActionEntry (271, 1, 11, 1, 20), dActionEntry (272, 1, 11, 1, 20), 
			dActionEntry (273, 1, 11, 1, 20), dActionEntry (274, 1, 11, 1, 20), dActionEntry (275, 1, 11, 1, 20), dActionEntry (276, 1, 11, 1, 20), 
			dActionEntry (277, 1, 11, 1, 20), dActionEntry (278, 1, 11, 1, 20), dActionEntry (279, 1, 11, 1, 20), dActionEntry (280, 1, 11, 1, 20), 
			dActionEntry (281, 1, 11, 1, 20), dActionEntry (282, 1, 11, 1, 20), dActionEntry (283, 1, 11, 1, 20), dActionEntry (284, 1, 11, 1, 20), 
			dActionEntry (285, 1, 11, 1, 20), dActionEntry (286, 1, 11, 1, 20), dActionEntry (287, 1, 11, 1, 20), dActionEntry (288, 1, 11, 1, 20), 
			dActionEntry (289, 1, 11, 1, 20), dActionEntry (290, 1, 11, 1, 20), dActionEntry (291, 1, 11, 1, 20), dActionEntry (292, 1, 11, 1, 20), 
			dActionEntry (293, 1, 11, 1, 20), dActionEntry (294, 1, 11, 1, 20), dActionEntry (295, 1, 11, 1, 20), dActionEntry (296, 1, 11, 1, 20), 
			dActionEntry (297, 1, 11, 1, 20), dActionEntry (298, 1, 11, 1, 20), dActionEntry (299, 1, 11, 1, 20), dActionEntry (300, 1, 11, 1, 20), 
			dActionEntry (260, 1, 11, 1, 22), dActionEntry (264, 1, 11, 1, 22), dActionEntry (265, 1, 11, 1, 22), dActionEntry (266, 1, 11, 1, 22), 
			dActionEntry (267, 1, 11, 1, 22), dActionEntry (268, 1, 11, 1, 22), dActionEntry (269, 1, 11, 1, 22), dActionEntry (270, 1, 11, 1, 22), 
			dActionEntry (271, 1, 11, 1, 22), dActionEntry (272, 1, 11, 1, 22), dActionEntry (273, 1, 11, 1, 22), dActionEntry (274, 1, 11, 1, 22), 
			dActionEntry (275, 1, 11, 1, 22), dActionEntry (276, 1, 11, 1, 22), dActionEntry (277, 1, 11, 1, 22), dActionEntry (278, 1, 11, 1, 22), 
			dActionEntry (279, 1, 11, 1, 22), dActionEntry (280, 1, 11, 1, 22), dActionEntry (281, 1, 11, 1, 22), dActionEntry (282, 1, 11, 1, 22), 
			dActionEntry (283, 1, 11, 1, 22), dActionEntry (284, 1, 11, 1, 22), dActionEntry (285, 1, 11, 1, 22), dActionEntry (286, 1, 11, 1, 22), 
			dActionEntry (287, 1, 11, 1, 22), dActionEntry (288, 1, 11, 1, 22), dActionEntry (289, 1, 11, 1, 22), dActionEntry (290, 1, 11, 1, 22), 
			dActionEntry (291, 1, 11, 1, 22), dActionEntry (292, 1, 11, 1, 22), dActionEntry (293, 1, 11, 1, 22), dActionEntry (294, 1, 11, 1, 22), 
			dActionEntry (295, 1, 11, 1, 22), dActionEntry (296, 1, 11, 1, 22), dActionEntry (297, 1, 11, 1, 22), dActionEntry (298, 1, 11, 1, 22), 
			dActionEntry (299, 1, 11, 1, 22), dActionEntry (300, 1, 11, 1, 22), dActionEntry (260, 1, 11, 1, 21), dActionEntry (264, 1, 11, 1, 21), 
			dActionEntry (265, 1, 11, 1, 21), dActionEntry (266, 1, 11, 1, 21), dActionEntry (267, 1, 11, 1, 21), dActionEntry (268, 1, 11, 1, 21), 
			dActionEntry (269, 1, 11, 1, 21), dActionEntry (270, 1, 11, 1, 21), dActionEntry (271, 1, 11, 1, 21), dActionEntry (272, 1, 11, 1, 21), 
			dActionEntry (273, 1, 11, 1, 21), dActionEntry (274, 1, 11, 1, 21), dActionEntry (275, 1, 11, 1, 21), dActionEntry (276, 1, 11, 1, 21), 
			dActionEntry (277, 1, 11, 1, 21), dActionEntry (278, 1, 11, 1, 21), dActionEntry (279, 1, 11, 1, 21), dActionEntry (280, 1, 11, 1, 21), 
			dActionEntry (281, 1, 11, 1, 21), dActionEntry (282, 1, 11, 1, 21), dActionEntry (283, 1, 11, 1, 21), dActionEntry (284, 1, 11, 1, 21), 
			dActionEntry (285, 1, 11, 1, 21), dActionEntry (286, 1, 11, 1, 21), dActionEntry (287, 1, 11, 1, 21), dActionEntry (288, 1, 11, 1, 21), 
			dActionEntry (289, 1, 11, 1, 21), dActionEntry (290, 1, 11, 1, 21), dActionEntry (291, 1, 11, 1, 21), dActionEntry (292, 1, 11, 1, 21), 
			dActionEntry (293, 1, 11, 1, 21), dActionEntry (294, 1, 11, 1, 21), dActionEntry (295, 1, 11, 1, 21), dActionEntry (296, 1, 11, 1, 21), 
			dActionEntry (297, 1, 11, 1, 21), dActionEntry (298, 1, 11, 1, 21), dActionEntry (299, 1, 11, 1, 21), dActionEntry (300, 1, 11, 1, 21), 
			dActionEntry (260, 1, 11, 1, 19), dActionEntry (264, 1, 11, 1, 19), dActionEntry (265, 1, 11, 1, 19), dActionEntry (266, 1, 11, 1, 19), 
			dActionEntry (267, 1, 11, 1, 19), dActionEntry (268, 1, 11, 1, 19), dActionEntry (269, 1, 11, 1, 19), dActionEntry (270, 1, 11, 1, 19), 
			dActionEntry (271, 1, 11, 1, 19), dActionEntry (272, 1, 11, 1, 19), dActionEntry (273, 1, 11, 1, 19), dActionEntry (274, 1, 11, 1, 19), 
			dActionEntry (275, 1, 11, 1, 19), dActionEntry (276, 1, 11, 1, 19), dActionEntry (277, 1, 11, 1, 19), dActionEntry (278, 1, 11, 1, 19), 
			dActionEntry (279, 1, 11, 1, 19), dActionEntry (280, 1, 11, 1, 19), dActionEntry (281, 1, 11, 1, 19), dActionEntry (282, 1, 11, 1, 19), 
			dActionEntry (283, 1, 11, 1, 19), dActionEntry (284, 1, 11, 1, 19), dActionEntry (285, 1, 11, 1, 19), dActionEntry (286, 1, 11, 1, 19), 
			dActionEntry (287, 1, 11, 1, 19), dActionEntry (288, 1, 11, 1, 19), dActionEntry (289, 1, 11, 1, 19), dActionEntry (290, 1, 11, 1, 19), 
			dActionEntry (291, 1, 11, 1, 19), dActionEntry (292, 1, 11, 1, 19), dActionEntry (293, 1, 11, 1, 19), dActionEntry (294, 1, 11, 1, 19), 
			dActionEntry (295, 1, 11, 1, 19), dActionEntry (296, 1, 11, 1, 19), dActionEntry (297, 1, 11, 1, 19), dActionEntry (298, 1, 11, 1, 19), 
			dActionEntry (299, 1, 11, 1, 19), dActionEntry (300, 1, 11, 1, 19), dActionEntry (260, 1, 10, 1, 15), dActionEntry (264, 1, 10, 1, 15), 
			dActionEntry (265, 1, 10, 1, 15), dActionEntry (266, 1, 10, 1, 15), dActionEntry (267, 1, 10, 1, 15), dActionEntry (268, 1, 10, 1, 15), 
			dActionEntry (269, 1, 10, 1, 15), dActionEntry (270, 1, 10, 1, 15), dActionEntry (271, 1, 10, 1, 15), dActionEntry (272, 1, 10, 1, 15), 
			dActionEntry (273, 1, 10, 1, 15), dActionEntry (274, 1, 10, 1, 15), dActionEntry (275, 1, 10, 1, 15), dActionEntry (276, 1, 10, 1, 15), 
			dActionEntry (277, 1, 10, 1, 15), dActionEntry (278, 1, 10, 1, 15), dActionEntry (279, 1, 10, 1, 15), dActionEntry (280, 1, 10, 1, 15), 
			dActionEntry (281, 1, 10, 1, 15), dActionEntry (282, 1, 10, 1, 15), dActionEntry (283, 1, 10, 1, 15), dActionEntry (284, 1, 10, 1, 15), 
			dActionEntry (285, 1, 10, 1, 15), dActionEntry (286, 1, 10, 1, 15), dActionEntry (287, 1, 10, 1, 15), dActionEntry (288, 1, 10, 1, 15), 
			dActionEntry (289, 1, 10, 1, 15), dActionEntry (290, 1, 10, 1, 15), dActionEntry (291, 1, 10, 1, 15), dActionEntry (292, 1, 10, 1, 15), 
			dActionEntry (293, 1, 10, 1, 15), dActionEntry (294, 1, 10, 1, 15), dActionEntry (295, 1, 10, 1, 15), dActionEntry (296, 1, 10, 1, 15), 
			dActionEntry (297, 1, 10, 1, 15), dActionEntry (298, 1, 10, 1, 15), dActionEntry (299, 1, 10, 1, 15), dActionEntry (300, 1, 10, 1, 15), 
			dActionEntry (260, 0, 91, 0, 0), dActionEntry (264, 0, 36, 0, 0), dActionEntry (265, 0, 33, 0, 0), dActionEntry (266, 0, 35, 0, 0), 
			dActionEntry (267, 0, 34, 0, 0), dActionEntry (268, 0, 46, 0, 0), dActionEntry (269, 0, 48, 0, 0), dActionEntry (270, 0, 47, 0, 0), 
			dActionEntry (271, 0, 44, 0, 0), dActionEntry (272, 0, 45, 0, 0), dActionEntry (273, 0, 18, 0, 0), dActionEntry (274, 0, 40, 0, 0), 
			dActionEntry (275, 0, 51, 0, 0), dActionEntry (276, 0, 39, 0, 0), dActionEntry (277, 0, 17, 0, 0), dActionEntry (278, 0, 16, 0, 0), 
			dActionEntry (279, 0, 49, 0, 0), dActionEntry (280, 0, 37, 0, 0), dActionEntry (281, 0, 27, 0, 0), dActionEntry (282, 0, 15, 0, 0), 
			dActionEntry (283, 0, 38, 0, 0), dActionEntry (284, 0, 19, 0, 0), dActionEntry (285, 0, 24, 0, 0), dActionEntry (286, 0, 23, 0, 0), 
			dActionEntry (287, 0, 22, 0, 0), dActionEntry (288, 0, 21, 0, 0), dActionEntry (289, 0, 20, 0, 0), dActionEntry (290, 0, 25, 0, 0), 
			dActionEntry (291, 0, 26, 0, 0), dActionEntry (292, 0, 43, 0, 0), dActionEntry (293, 0, 50, 0, 0), dActionEntry (294, 0, 30, 0, 0), 
			dActionEntry (295, 0, 32, 0, 0), dActionEntry (296, 0, 28, 0, 0), dActionEntry (297, 0, 29, 0, 0), dActionEntry (298, 0, 42, 0, 0), 
			dActionEntry (299, 0, 41, 0, 0), dActionEntry (300, 0, 31, 0, 0), dActionEntry (260, 1, 11, 1, 24), dActionEntry (264, 1, 11, 1, 24), 
			dActionEntry (265, 1, 11, 1, 24), dActionEntry (266, 1, 11, 1, 24), dActionEntry (267, 1, 11, 1, 24), dActionEntry (268, 1, 11, 1, 24), 
			dActionEntry (269, 1, 11, 1, 24), dActionEntry (270, 1, 11, 1, 24), dActionEntry (271, 1, 11, 1, 24), dActionEntry (272, 1, 11, 1, 24), 
			dActionEntry (273, 1, 11, 1, 24), dActionEntry (274, 1, 11, 1, 24), dActionEntry (275, 1, 11, 1, 24), dActionEntry (276, 1, 11, 1, 24), 
			dActionEntry (277, 1, 11, 1, 24), dActionEntry (278, 1, 11, 1, 24), dActionEntry (279, 1, 11, 1, 24), dActionEntry (280, 1, 11, 1, 24), 
			dActionEntry (281, 1, 11, 1, 24), dActionEntry (282, 1, 11, 1, 24), dActionEntry (283, 1, 11, 1, 24), dActionEntry (284, 1, 11, 1, 24), 
			dActionEntry (285, 1, 11, 1, 24), dActionEntry (286, 1, 11, 1, 24), dActionEntry (287, 1, 11, 1, 24), dActionEntry (288, 1, 11, 1, 24), 
			dActionEntry (289, 1, 11, 1, 24), dActionEntry (290, 1, 11, 1, 24), dActionEntry (291, 1, 11, 1, 24), dActionEntry (292, 1, 11, 1, 24), 
			dActionEntry (293, 1, 11, 1, 24), dActionEntry (294, 1, 11, 1, 24), dActionEntry (295, 1, 11, 1, 24), dActionEntry (296, 1, 11, 1, 24), 
			dActionEntry (297, 1, 11, 1, 24), dActionEntry (298, 1, 11, 1, 24), dActionEntry (299, 1, 11, 1, 24), dActionEntry (300, 1, 11, 1, 24), 
			dActionEntry (260, 1, 11, 1, 17), dActionEntry (264, 1, 11, 1, 17), dActionEntry (265, 1, 11, 1, 17), dActionEntry (266, 1, 11, 1, 17), 
			dActionEntry (267, 1, 11, 1, 17), dActionEntry (268, 1, 11, 1, 17), dActionEntry (269, 1, 11, 1, 17), dActionEntry (270, 1, 11, 1, 17), 
			dActionEntry (271, 1, 11, 1, 17), dActionEntry (272, 1, 11, 1, 17), dActionEntry (273, 1, 11, 1, 17), dActionEntry (274, 1, 11, 1, 17), 
			dActionEntry (275, 1, 11, 1, 17), dActionEntry (276, 1, 11, 1, 17), dActionEntry (277, 1, 11, 1, 17), dActionEntry (278, 1, 11, 1, 17), 
			dActionEntry (279, 1, 11, 1, 17), dActionEntry (280, 1, 11, 1, 17), dActionEntry (281, 1, 11, 1, 17), dActionEntry (282, 1, 11, 1, 17), 
			dActionEntry (283, 1, 11, 1, 17), dActionEntry (284, 1, 11, 1, 17), dActionEntry (285, 1, 11, 1, 17), dActionEntry (286, 1, 11, 1, 17), 
			dActionEntry (287, 1, 11, 1, 17), dActionEntry (288, 1, 11, 1, 17), dActionEntry (289, 1, 11, 1, 17), dActionEntry (290, 1, 11, 1, 17), 
			dActionEntry (291, 1, 11, 1, 17), dActionEntry (292, 1, 11, 1, 17), dActionEntry (293, 1, 11, 1, 17), dActionEntry (294, 1, 11, 1, 17), 
			dActionEntry (295, 1, 11, 1, 17), dActionEntry (296, 1, 11, 1, 17), dActionEntry (297, 1, 11, 1, 17), dActionEntry (298, 1, 11, 1, 17), 
			dActionEntry (299, 1, 11, 1, 17), dActionEntry (300, 1, 11, 1, 17), dActionEntry (123, 0, 96, 0, 0), dActionEntry (263, 0, 94, 0, 0), 
			dActionEntry (260, 1, 11, 1, 23), dActionEntry (264, 1, 11, 1, 23), dActionEntry (265, 1, 11, 1, 23), dActionEntry (266, 1, 11, 1, 23), 
			dActionEntry (267, 1, 11, 1, 23), dActionEntry (268, 1, 11, 1, 23), dActionEntry (269, 1, 11, 1, 23), dActionEntry (270, 1, 11, 1, 23), 
			dActionEntry (271, 1, 11, 1, 23), dActionEntry (272, 1, 11, 1, 23), dActionEntry (273, 1, 11, 1, 23), dActionEntry (274, 1, 11, 1, 23), 
			dActionEntry (275, 1, 11, 1, 23), dActionEntry (276, 1, 11, 1, 23), dActionEntry (277, 1, 11, 1, 23), dActionEntry (278, 1, 11, 1, 23), 
			dActionEntry (279, 1, 11, 1, 23), dActionEntry (280, 1, 11, 1, 23), dActionEntry (281, 1, 11, 1, 23), dActionEntry (282, 1, 11, 1, 23), 
			dActionEntry (283, 1, 11, 1, 23), dActionEntry (284, 1, 11, 1, 23), dActionEntry (285, 1, 11, 1, 23), dActionEntry (286, 1, 11, 1, 23), 
			dActionEntry (287, 1, 11, 1, 23), dActionEntry (288, 1, 11, 1, 23), dActionEntry (289, 1, 11, 1, 23), dActionEntry (290, 1, 11, 1, 23), 
			dActionEntry (291, 1, 11, 1, 23), dActionEntry (292, 1, 11, 1, 23), dActionEntry (293, 1, 11, 1, 23), dActionEntry (294, 1, 11, 1, 23), 
			dActionEntry (295, 1, 11, 1, 23), dActionEntry (296, 1, 11, 1, 23), dActionEntry (297, 1, 11, 1, 23), dActionEntry (298, 1, 11, 1, 23), 
			dActionEntry (299, 1, 11, 1, 23), dActionEntry (300, 1, 11, 1, 23), dActionEntry (260, 1, 11, 1, 18), dActionEntry (264, 1, 11, 1, 18), 
			dActionEntry (265, 1, 11, 1, 18), dActionEntry (266, 1, 11, 1, 18), dActionEntry (267, 1, 11, 1, 18), dActionEntry (268, 1, 11, 1, 18), 
			dActionEntry (269, 1, 11, 1, 18), dActionEntry (270, 1, 11, 1, 18), dActionEntry (271, 1, 11, 1, 18), dActionEntry (272, 1, 11, 1, 18), 
			dActionEntry (273, 1, 11, 1, 18), dActionEntry (274, 1, 11, 1, 18), dActionEntry (275, 1, 11, 1, 18), dActionEntry (276, 1, 11, 1, 18), 
			dActionEntry (277, 1, 11, 1, 18), dActionEntry (278, 1, 11, 1, 18), dActionEntry (279, 1, 11, 1, 18), dActionEntry (280, 1, 11, 1, 18), 
			dActionEntry (281, 1, 11, 1, 18), dActionEntry (282, 1, 11, 1, 18), dActionEntry (283, 1, 11, 1, 18), dActionEntry (284, 1, 11, 1, 18), 
			dActionEntry (285, 1, 11, 1, 18), dActionEntry (286, 1, 11, 1, 18), dActionEntry (287, 1, 11, 1, 18), dActionEntry (288, 1, 11, 1, 18), 
			dActionEntry (289, 1, 11, 1, 18), dActionEntry (290, 1, 11, 1, 18), dActionEntry (291, 1, 11, 1, 18), dActionEntry (292, 1, 11, 1, 18), 
			dActionEntry (293, 1, 11, 1, 18), dActionEntry (294, 1, 11, 1, 18), dActionEntry (295, 1, 11, 1, 18), dActionEntry (296, 1, 11, 1, 18), 
			dActionEntry (297, 1, 11, 1, 18), dActionEntry (298, 1, 11, 1, 18), dActionEntry (299, 1, 11, 1, 18), dActionEntry (300, 1, 11, 1, 18), 
			dActionEntry (61, 1, 7, 1, 14), dActionEntry (255, 1, 7, 1, 14), dActionEntry (256, 1, 7, 1, 14), dActionEntry (259, 1, 7, 1, 14), 
			dActionEntry (261, 1, 7, 1, 14), dActionEntry (61, 0, 98, 0, 0), dActionEntry (255, 1, 4, 2, 11), dActionEntry (256, 1, 4, 2, 11), 
			dActionEntry (259, 1, 4, 2, 11), dActionEntry (261, 1, 4, 2, 11), dActionEntry (255, 1, 1, 2, 4), dActionEntry (256, 1, 1, 2, 4), 
			dActionEntry (259, 1, 1, 2, 4), dActionEntry (261, 1, 1, 2, 4), dActionEntry (264, 1, 23, 1, 33), dActionEntry (265, 1, 23, 1, 33), 
			dActionEntry (266, 1, 23, 1, 33), dActionEntry (267, 1, 23, 1, 33), dActionEntry (268, 1, 23, 1, 33), dActionEntry (269, 1, 23, 1, 33), 
			dActionEntry (270, 1, 23, 1, 33), dActionEntry (271, 1, 23, 1, 33), dActionEntry (272, 1, 23, 1, 33), dActionEntry (273, 1, 23, 1, 33), 
			dActionEntry (274, 1, 23, 1, 33), dActionEntry (275, 1, 23, 1, 33), dActionEntry (276, 1, 23, 1, 33), dActionEntry (277, 1, 23, 1, 33), 
			dActionEntry (278, 1, 23, 1, 33), dActionEntry (279, 1, 23, 1, 33), dActionEntry (280, 1, 23, 1, 33), dActionEntry (281, 1, 23, 1, 33), 
			dActionEntry (282, 1, 23, 1, 33), dActionEntry (283, 1, 23, 1, 33), dActionEntry (284, 1, 23, 1, 33), dActionEntry (285, 1, 23, 1, 33), 
			dActionEntry (286, 1, 23, 1, 33), dActionEntry (287, 1, 23, 1, 33), dActionEntry (288, 1, 23, 1, 33), dActionEntry (289, 1, 23, 1, 33), 
			dActionEntry (290, 1, 23, 1, 33), dActionEntry (291, 1, 23, 1, 33), dActionEntry (292, 1, 23, 1, 33), dActionEntry (293, 1, 23, 1, 33), 
			dActionEntry (294, 1, 23, 1, 33), dActionEntry (295, 1, 23, 1, 33), dActionEntry (296, 1, 23, 1, 33), dActionEntry (297, 1, 23, 1, 33), 
			dActionEntry (298, 1, 23, 1, 33), dActionEntry (299, 1, 23, 1, 33), dActionEntry (300, 1, 23, 1, 33), dActionEntry (264, 1, 27, 3, 56), 
			dActionEntry (265, 1, 27, 3, 56), dActionEntry (266, 1, 27, 3, 56), dActionEntry (267, 1, 27, 3, 56), dActionEntry (268, 1, 27, 3, 56), 
			dActionEntry (269, 1, 27, 3, 56), dActionEntry (270, 1, 27, 3, 56), dActionEntry (271, 1, 27, 3, 56), dActionEntry (272, 1, 27, 3, 56), 
			dActionEntry (273, 1, 27, 3, 56), dActionEntry (274, 1, 27, 3, 56), dActionEntry (275, 1, 27, 3, 56), dActionEntry (276, 1, 27, 3, 56), 
			dActionEntry (277, 1, 27, 3, 56), dActionEntry (278, 1, 27, 3, 56), dActionEntry (279, 1, 27, 3, 56), dActionEntry (280, 1, 27, 3, 56), 
			dActionEntry (281, 1, 27, 3, 56), dActionEntry (282, 1, 27, 3, 56), dActionEntry (283, 1, 27, 3, 56), dActionEntry (284, 1, 27, 3, 56), 
			dActionEntry (285, 1, 27, 3, 56), dActionEntry (286, 1, 27, 3, 56), dActionEntry (287, 1, 27, 3, 56), dActionEntry (288, 1, 27, 3, 56), 
			dActionEntry (289, 1, 27, 3, 56), dActionEntry (290, 1, 27, 3, 56), dActionEntry (291, 1, 27, 3, 56), dActionEntry (292, 1, 27, 3, 56), 
			dActionEntry (293, 1, 27, 3, 56), dActionEntry (294, 1, 27, 3, 56), dActionEntry (295, 1, 27, 3, 56), dActionEntry (296, 1, 27, 3, 56), 
			dActionEntry (297, 1, 27, 3, 56), dActionEntry (298, 1, 27, 3, 56), dActionEntry (299, 1, 27, 3, 56), dActionEntry (300, 1, 27, 3, 56), 
			dActionEntry (46, 1, 7, 1, 14), dActionEntry (62, 1, 7, 1, 14), dActionEntry (46, 0, 99, 0, 0), dActionEntry (62, 0, 100, 0, 0), 
			dActionEntry (46, 1, 6, 1, 9), dActionEntry (62, 1, 6, 1, 9), dActionEntry (44, 1, 21, 1, 65), dActionEntry (44, 0, 101, 0, 0), 
			dActionEntry (260, 1, 7, 1, 14), dActionEntry (264, 1, 7, 1, 14), dActionEntry (265, 1, 7, 1, 14), dActionEntry (266, 1, 7, 1, 14), 
			dActionEntry (267, 1, 7, 1, 14), dActionEntry (268, 1, 7, 1, 14), dActionEntry (269, 1, 7, 1, 14), dActionEntry (270, 1, 7, 1, 14), 
			dActionEntry (271, 1, 7, 1, 14), dActionEntry (272, 1, 7, 1, 14), dActionEntry (273, 1, 7, 1, 14), dActionEntry (274, 1, 7, 1, 14), 
			dActionEntry (275, 1, 7, 1, 14), dActionEntry (276, 1, 7, 1, 14), dActionEntry (277, 1, 7, 1, 14), dActionEntry (278, 1, 7, 1, 14), 
			dActionEntry (279, 1, 7, 1, 14), dActionEntry (280, 1, 7, 1, 14), dActionEntry (281, 1, 7, 1, 14), dActionEntry (282, 1, 7, 1, 14), 
			dActionEntry (283, 1, 7, 1, 14), dActionEntry (284, 1, 7, 1, 14), dActionEntry (285, 1, 7, 1, 14), dActionEntry (286, 1, 7, 1, 14), 
			dActionEntry (287, 1, 7, 1, 14), dActionEntry (288, 1, 7, 1, 14), dActionEntry (289, 1, 7, 1, 14), dActionEntry (290, 1, 7, 1, 14), 
			dActionEntry (291, 1, 7, 1, 14), dActionEntry (292, 1, 7, 1, 14), dActionEntry (293, 1, 7, 1, 14), dActionEntry (294, 1, 7, 1, 14), 
			dActionEntry (295, 1, 7, 1, 14), dActionEntry (296, 1, 7, 1, 14), dActionEntry (297, 1, 7, 1, 14), dActionEntry (298, 1, 7, 1, 14), 
			dActionEntry (299, 1, 7, 1, 14), dActionEntry (300, 1, 7, 1, 14), dActionEntry (260, 1, 17, 2, 67), dActionEntry (264, 1, 17, 2, 67), 
			dActionEntry (265, 1, 17, 2, 67), dActionEntry (266, 1, 17, 2, 67), dActionEntry (267, 1, 17, 2, 67), dActionEntry (268, 1, 17, 2, 67), 
			dActionEntry (269, 1, 17, 2, 67), dActionEntry (270, 1, 17, 2, 67), dActionEntry (271, 1, 17, 2, 67), dActionEntry (272, 1, 17, 2, 67), 
			dActionEntry (273, 1, 17, 2, 67), dActionEntry (274, 1, 17, 2, 67), dActionEntry (275, 1, 17, 2, 67), dActionEntry (276, 1, 17, 2, 67), 
			dActionEntry (277, 1, 17, 2, 67), dActionEntry (278, 1, 17, 2, 67), dActionEntry (279, 1, 17, 2, 67), dActionEntry (280, 1, 17, 2, 67), 
			dActionEntry (281, 1, 17, 2, 67), dActionEntry (282, 1, 17, 2, 67), dActionEntry (283, 1, 17, 2, 67), dActionEntry (284, 1, 17, 2, 67), 
			dActionEntry (285, 1, 17, 2, 67), dActionEntry (286, 1, 17, 2, 67), dActionEntry (287, 1, 17, 2, 67), dActionEntry (288, 1, 17, 2, 67), 
			dActionEntry (289, 1, 17, 2, 67), dActionEntry (290, 1, 17, 2, 67), dActionEntry (291, 1, 17, 2, 67), dActionEntry (292, 1, 17, 2, 67), 
			dActionEntry (293, 1, 17, 2, 67), dActionEntry (294, 1, 17, 2, 67), dActionEntry (295, 1, 17, 2, 67), dActionEntry (296, 1, 17, 2, 67), 
			dActionEntry (297, 1, 17, 2, 67), dActionEntry (298, 1, 17, 2, 67), dActionEntry (299, 1, 17, 2, 67), dActionEntry (300, 1, 17, 2, 67), 
			dActionEntry (44, 0, 102, 0, 0), dActionEntry (44, 1, 21, 1, 65), dActionEntry (260, 1, 21, 1, 65), dActionEntry (264, 1, 21, 1, 65), 
			dActionEntry (265, 1, 21, 1, 65), dActionEntry (266, 1, 21, 1, 65), dActionEntry (267, 1, 21, 1, 65), dActionEntry (268, 1, 21, 1, 65), 
			dActionEntry (269, 1, 21, 1, 65), dActionEntry (270, 1, 21, 1, 65), dActionEntry (271, 1, 21, 1, 65), dActionEntry (272, 1, 21, 1, 65), 
			dActionEntry (273, 1, 21, 1, 65), dActionEntry (274, 1, 21, 1, 65), dActionEntry (275, 1, 21, 1, 65), dActionEntry (276, 1, 21, 1, 65), 
			dActionEntry (277, 1, 21, 1, 65), dActionEntry (278, 1, 21, 1, 65), dActionEntry (279, 1, 21, 1, 65), dActionEntry (280, 1, 21, 1, 65), 
			dActionEntry (281, 1, 21, 1, 65), dActionEntry (282, 1, 21, 1, 65), dActionEntry (283, 1, 21, 1, 65), dActionEntry (284, 1, 21, 1, 65), 
			dActionEntry (285, 1, 21, 1, 65), dActionEntry (286, 1, 21, 1, 65), dActionEntry (287, 1, 21, 1, 65), dActionEntry (288, 1, 21, 1, 65), 
			dActionEntry (289, 1, 21, 1, 65), dActionEntry (290, 1, 21, 1, 65), dActionEntry (291, 1, 21, 1, 65), dActionEntry (292, 1, 21, 1, 65), 
			dActionEntry (293, 1, 21, 1, 65), dActionEntry (294, 1, 21, 1, 65), dActionEntry (295, 1, 21, 1, 65), dActionEntry (296, 1, 21, 1, 65), 
			dActionEntry (297, 1, 21, 1, 65), dActionEntry (298, 1, 21, 1, 65), dActionEntry (299, 1, 21, 1, 65), dActionEntry (300, 1, 21, 1, 65), 
			dActionEntry (44, 0, 103, 0, 0), dActionEntry (260, 1, 17, 2, 69), dActionEntry (264, 1, 17, 2, 69), dActionEntry (265, 1, 17, 2, 69), 
			dActionEntry (266, 1, 17, 2, 69), dActionEntry (267, 1, 17, 2, 69), dActionEntry (268, 1, 17, 2, 69), dActionEntry (269, 1, 17, 2, 69), 
			dActionEntry (270, 1, 17, 2, 69), dActionEntry (271, 1, 17, 2, 69), dActionEntry (272, 1, 17, 2, 69), dActionEntry (273, 1, 17, 2, 69), 
			dActionEntry (274, 1, 17, 2, 69), dActionEntry (275, 1, 17, 2, 69), dActionEntry (276, 1, 17, 2, 69), dActionEntry (277, 1, 17, 2, 69), 
			dActionEntry (278, 1, 17, 2, 69), dActionEntry (279, 1, 17, 2, 69), dActionEntry (280, 1, 17, 2, 69), dActionEntry (281, 1, 17, 2, 69), 
			dActionEntry (282, 1, 17, 2, 69), dActionEntry (283, 1, 17, 2, 69), dActionEntry (284, 1, 17, 2, 69), dActionEntry (285, 1, 17, 2, 69), 
			dActionEntry (286, 1, 17, 2, 69), dActionEntry (287, 1, 17, 2, 69), dActionEntry (288, 1, 17, 2, 69), dActionEntry (289, 1, 17, 2, 69), 
			dActionEntry (290, 1, 17, 2, 69), dActionEntry (291, 1, 17, 2, 69), dActionEntry (292, 1, 17, 2, 69), dActionEntry (293, 1, 17, 2, 69), 
			dActionEntry (294, 1, 17, 2, 69), dActionEntry (295, 1, 17, 2, 69), dActionEntry (296, 1, 17, 2, 69), dActionEntry (297, 1, 17, 2, 69), 
			dActionEntry (298, 1, 17, 2, 69), dActionEntry (299, 1, 17, 2, 69), dActionEntry (300, 1, 17, 2, 69), dActionEntry (44, 0, 104, 0, 0), 
			dActionEntry (44, 0, 105, 0, 0), dActionEntry (260, 1, 21, 1, 65), dActionEntry (264, 1, 21, 1, 65), dActionEntry (265, 1, 21, 1, 65), 
			dActionEntry (266, 1, 21, 1, 65), dActionEntry (267, 1, 21, 1, 65), dActionEntry (268, 1, 21, 1, 65), dActionEntry (269, 1, 21, 1, 65), 
			dActionEntry (270, 1, 21, 1, 65), dActionEntry (271, 1, 21, 1, 65), dActionEntry (272, 1, 21, 1, 65), dActionEntry (273, 1, 21, 1, 65), 
			dActionEntry (274, 1, 21, 1, 65), dActionEntry (275, 1, 21, 1, 65), dActionEntry (276, 1, 21, 1, 65), dActionEntry (277, 1, 21, 1, 65), 
			dActionEntry (278, 1, 21, 1, 65), dActionEntry (279, 1, 21, 1, 65), dActionEntry (280, 1, 21, 1, 65), dActionEntry (281, 1, 21, 1, 65), 
			dActionEntry (282, 1, 21, 1, 65), dActionEntry (283, 1, 21, 1, 65), dActionEntry (284, 1, 21, 1, 65), dActionEntry (285, 1, 21, 1, 65), 
			dActionEntry (286, 1, 21, 1, 65), dActionEntry (287, 1, 21, 1, 65), dActionEntry (288, 1, 21, 1, 65), dActionEntry (289, 1, 21, 1, 65), 
			dActionEntry (290, 1, 21, 1, 65), dActionEntry (291, 1, 21, 1, 65), dActionEntry (292, 1, 21, 1, 65), dActionEntry (293, 1, 21, 1, 65), 
			dActionEntry (294, 1, 21, 1, 65), dActionEntry (295, 1, 21, 1, 65), dActionEntry (296, 1, 21, 1, 65), dActionEntry (297, 1, 21, 1, 65), 
			dActionEntry (298, 1, 21, 1, 65), dActionEntry (299, 1, 21, 1, 65), dActionEntry (300, 1, 21, 1, 65), dActionEntry (260, 1, 17, 2, 74), 
			dActionEntry (264, 1, 17, 2, 74), dActionEntry (265, 1, 17, 2, 74), dActionEntry (266, 1, 17, 2, 74), dActionEntry (267, 1, 17, 2, 74), 
			dActionEntry (268, 1, 17, 2, 74), dActionEntry (269, 1, 17, 2, 74), dActionEntry (270, 1, 17, 2, 74), dActionEntry (271, 1, 17, 2, 74), 
			dActionEntry (272, 1, 17, 2, 74), dActionEntry (273, 1, 17, 2, 74), dActionEntry (274, 1, 17, 2, 74), dActionEntry (275, 1, 17, 2, 74), 
			dActionEntry (276, 1, 17, 2, 74), dActionEntry (277, 1, 17, 2, 74), dActionEntry (278, 1, 17, 2, 74), dActionEntry (279, 1, 17, 2, 74), 
			dActionEntry (280, 1, 17, 2, 74), dActionEntry (281, 1, 17, 2, 74), dActionEntry (282, 1, 17, 2, 74), dActionEntry (283, 1, 17, 2, 74), 
			dActionEntry (284, 1, 17, 2, 74), dActionEntry (285, 1, 17, 2, 74), dActionEntry (286, 1, 17, 2, 74), dActionEntry (287, 1, 17, 2, 74), 
			dActionEntry (288, 1, 17, 2, 74), dActionEntry (289, 1, 17, 2, 74), dActionEntry (290, 1, 17, 2, 74), dActionEntry (291, 1, 17, 2, 74), 
			dActionEntry (292, 1, 17, 2, 74), dActionEntry (293, 1, 17, 2, 74), dActionEntry (294, 1, 17, 2, 74), dActionEntry (295, 1, 17, 2, 74), 
			dActionEntry (296, 1, 17, 2, 74), dActionEntry (297, 1, 17, 2, 74), dActionEntry (298, 1, 17, 2, 74), dActionEntry (299, 1, 17, 2, 74), 
			dActionEntry (300, 1, 17, 2, 74), dActionEntry (260, 1, 17, 2, 75), dActionEntry (264, 1, 17, 2, 75), dActionEntry (265, 1, 17, 2, 75), 
			dActionEntry (266, 1, 17, 2, 75), dActionEntry (267, 1, 17, 2, 75), dActionEntry (268, 1, 17, 2, 75), dActionEntry (269, 1, 17, 2, 75), 
			dActionEntry (270, 1, 17, 2, 75), dActionEntry (271, 1, 17, 2, 75), dActionEntry (272, 1, 17, 2, 75), dActionEntry (273, 1, 17, 2, 75), 
			dActionEntry (274, 1, 17, 2, 75), dActionEntry (275, 1, 17, 2, 75), dActionEntry (276, 1, 17, 2, 75), dActionEntry (277, 1, 17, 2, 75), 
			dActionEntry (278, 1, 17, 2, 75), dActionEntry (279, 1, 17, 2, 75), dActionEntry (280, 1, 17, 2, 75), dActionEntry (281, 1, 17, 2, 75), 
			dActionEntry (282, 1, 17, 2, 75), dActionEntry (283, 1, 17, 2, 75), dActionEntry (284, 1, 17, 2, 75), dActionEntry (285, 1, 17, 2, 75), 
			dActionEntry (286, 1, 17, 2, 75), dActionEntry (287, 1, 17, 2, 75), dActionEntry (288, 1, 17, 2, 75), dActionEntry (289, 1, 17, 2, 75), 
			dActionEntry (290, 1, 17, 2, 75), dActionEntry (291, 1, 17, 2, 75), dActionEntry (292, 1, 17, 2, 75), dActionEntry (293, 1, 17, 2, 75), 
			dActionEntry (294, 1, 17, 2, 75), dActionEntry (295, 1, 17, 2, 75), dActionEntry (296, 1, 17, 2, 75), dActionEntry (297, 1, 17, 2, 75), 
			dActionEntry (298, 1, 17, 2, 75), dActionEntry (299, 1, 17, 2, 75), dActionEntry (300, 1, 17, 2, 75), dActionEntry (260, 1, 17, 2, 72), 
			dActionEntry (264, 1, 17, 2, 72), dActionEntry (265, 1, 17, 2, 72), dActionEntry (266, 1, 17, 2, 72), dActionEntry (267, 1, 17, 2, 72), 
			dActionEntry (268, 1, 17, 2, 72), dActionEntry (269, 1, 17, 2, 72), dActionEntry (270, 1, 17, 2, 72), dActionEntry (271, 1, 17, 2, 72), 
			dActionEntry (272, 1, 17, 2, 72), dActionEntry (273, 1, 17, 2, 72), dActionEntry (274, 1, 17, 2, 72), dActionEntry (275, 1, 17, 2, 72), 
			dActionEntry (276, 1, 17, 2, 72), dActionEntry (277, 1, 17, 2, 72), dActionEntry (278, 1, 17, 2, 72), dActionEntry (279, 1, 17, 2, 72), 
			dActionEntry (280, 1, 17, 2, 72), dActionEntry (281, 1, 17, 2, 72), dActionEntry (282, 1, 17, 2, 72), dActionEntry (283, 1, 17, 2, 72), 
			dActionEntry (284, 1, 17, 2, 72), dActionEntry (285, 1, 17, 2, 72), dActionEntry (286, 1, 17, 2, 72), dActionEntry (287, 1, 17, 2, 72), 
			dActionEntry (288, 1, 17, 2, 72), dActionEntry (289, 1, 17, 2, 72), dActionEntry (290, 1, 17, 2, 72), dActionEntry (291, 1, 17, 2, 72), 
			dActionEntry (292, 1, 17, 2, 72), dActionEntry (293, 1, 17, 2, 72), dActionEntry (294, 1, 17, 2, 72), dActionEntry (295, 1, 17, 2, 72), 
			dActionEntry (296, 1, 17, 2, 72), dActionEntry (297, 1, 17, 2, 72), dActionEntry (298, 1, 17, 2, 72), dActionEntry (299, 1, 17, 2, 72), 
			dActionEntry (300, 1, 17, 2, 72), dActionEntry (44, 0, 106, 0, 0), dActionEntry (44, 0, 107, 0, 0), dActionEntry (44, 0, 108, 0, 0), 
			dActionEntry (255, 1, 5, 3, 59), dActionEntry (256, 1, 5, 3, 59), dActionEntry (259, 1, 5, 3, 59), dActionEntry (261, 1, 5, 3, 59), 
			dActionEntry (260, 1, 10, 2, 16), dActionEntry (264, 1, 10, 2, 16), dActionEntry (265, 1, 10, 2, 16), dActionEntry (266, 1, 10, 2, 16), 
			dActionEntry (267, 1, 10, 2, 16), dActionEntry (268, 1, 10, 2, 16), dActionEntry (269, 1, 10, 2, 16), dActionEntry (270, 1, 10, 2, 16), 
			dActionEntry (271, 1, 10, 2, 16), dActionEntry (272, 1, 10, 2, 16), dActionEntry (273, 1, 10, 2, 16), dActionEntry (274, 1, 10, 2, 16), 
			dActionEntry (275, 1, 10, 2, 16), dActionEntry (276, 1, 10, 2, 16), dActionEntry (277, 1, 10, 2, 16), dActionEntry (278, 1, 10, 2, 16), 
			dActionEntry (279, 1, 10, 2, 16), dActionEntry (280, 1, 10, 2, 16), dActionEntry (281, 1, 10, 2, 16), dActionEntry (282, 1, 10, 2, 16), 
			dActionEntry (283, 1, 10, 2, 16), dActionEntry (284, 1, 10, 2, 16), dActionEntry (285, 1, 10, 2, 16), dActionEntry (286, 1, 10, 2, 16), 
			dActionEntry (287, 1, 10, 2, 16), dActionEntry (288, 1, 10, 2, 16), dActionEntry (289, 1, 10, 2, 16), dActionEntry (290, 1, 10, 2, 16), 
			dActionEntry (291, 1, 10, 2, 16), dActionEntry (292, 1, 10, 2, 16), dActionEntry (293, 1, 10, 2, 16), dActionEntry (294, 1, 10, 2, 16), 
			dActionEntry (295, 1, 10, 2, 16), dActionEntry (296, 1, 10, 2, 16), dActionEntry (297, 1, 10, 2, 16), dActionEntry (298, 1, 10, 2, 16), 
			dActionEntry (299, 1, 10, 2, 16), dActionEntry (300, 1, 10, 2, 16), dActionEntry (44, 0, 109, 0, 0), dActionEntry (44, 1, 21, 1, 65), 
			dActionEntry (260, 1, 21, 1, 65), dActionEntry (263, 1, 21, 1, 65), dActionEntry (264, 1, 21, 1, 65), dActionEntry (265, 1, 21, 1, 65), 
			dActionEntry (266, 1, 21, 1, 65), dActionEntry (267, 1, 21, 1, 65), dActionEntry (268, 1, 21, 1, 65), dActionEntry (269, 1, 21, 1, 65), 
			dActionEntry (270, 1, 21, 1, 65), dActionEntry (271, 1, 21, 1, 65), dActionEntry (272, 1, 21, 1, 65), dActionEntry (273, 1, 21, 1, 65), 
			dActionEntry (274, 1, 21, 1, 65), dActionEntry (275, 1, 21, 1, 65), dActionEntry (276, 1, 21, 1, 65), dActionEntry (277, 1, 21, 1, 65), 
			dActionEntry (278, 1, 21, 1, 65), dActionEntry (279, 1, 21, 1, 65), dActionEntry (280, 1, 21, 1, 65), dActionEntry (281, 1, 21, 1, 65), 
			dActionEntry (282, 1, 21, 1, 65), dActionEntry (283, 1, 21, 1, 65), dActionEntry (284, 1, 21, 1, 65), dActionEntry (285, 1, 21, 1, 65), 
			dActionEntry (286, 1, 21, 1, 65), dActionEntry (287, 1, 21, 1, 65), dActionEntry (288, 1, 21, 1, 65), dActionEntry (289, 1, 21, 1, 65), 
			dActionEntry (290, 1, 21, 1, 65), dActionEntry (291, 1, 21, 1, 65), dActionEntry (292, 1, 21, 1, 65), dActionEntry (293, 1, 21, 1, 65), 
			dActionEntry (294, 1, 21, 1, 65), dActionEntry (295, 1, 21, 1, 65), dActionEntry (296, 1, 21, 1, 65), dActionEntry (297, 1, 21, 1, 65), 
			dActionEntry (298, 1, 21, 1, 65), dActionEntry (299, 1, 21, 1, 65), dActionEntry (300, 1, 21, 1, 65), dActionEntry (44, 0, 110, 0, 0), 
			dActionEntry (260, 1, 18, 2, 81), dActionEntry (263, 0, 75, 0, 0), dActionEntry (264, 1, 18, 2, 81), dActionEntry (265, 1, 18, 2, 81), 
			dActionEntry (266, 1, 18, 2, 81), dActionEntry (267, 1, 18, 2, 81), dActionEntry (268, 1, 18, 2, 81), dActionEntry (269, 1, 18, 2, 81), 
			dActionEntry (270, 1, 18, 2, 81), dActionEntry (271, 1, 18, 2, 81), dActionEntry (272, 1, 18, 2, 81), dActionEntry (273, 1, 18, 2, 81), 
			dActionEntry (274, 1, 18, 2, 81), dActionEntry (275, 1, 18, 2, 81), dActionEntry (276, 1, 18, 2, 81), dActionEntry (277, 1, 18, 2, 81), 
			dActionEntry (278, 1, 18, 2, 81), dActionEntry (279, 1, 18, 2, 81), dActionEntry (280, 1, 18, 2, 81), dActionEntry (281, 1, 18, 2, 81), 
			dActionEntry (282, 1, 18, 2, 81), dActionEntry (283, 1, 18, 2, 81), dActionEntry (284, 1, 18, 2, 81), dActionEntry (285, 1, 18, 2, 81), 
			dActionEntry (286, 1, 18, 2, 81), dActionEntry (287, 1, 18, 2, 81), dActionEntry (288, 1, 18, 2, 81), dActionEntry (289, 1, 18, 2, 81), 
			dActionEntry (290, 1, 18, 2, 81), dActionEntry (291, 1, 18, 2, 81), dActionEntry (292, 1, 18, 2, 81), dActionEntry (293, 1, 18, 2, 81), 
			dActionEntry (294, 1, 18, 2, 81), dActionEntry (295, 1, 18, 2, 81), dActionEntry (296, 1, 18, 2, 81), dActionEntry (297, 1, 18, 2, 81), 
			dActionEntry (298, 1, 18, 2, 81), dActionEntry (299, 1, 18, 2, 81), dActionEntry (300, 1, 18, 2, 81), dActionEntry (263, 0, 112, 0, 0), 
			dActionEntry (44, 0, 115, 0, 0), dActionEntry (262, 0, 116, 0, 0), dActionEntry (255, 1, 3, 4, 8), dActionEntry (256, 1, 3, 4, 8), 
			dActionEntry (259, 1, 3, 4, 8), dActionEntry (261, 1, 3, 4, 8), dActionEntry (257, 0, 77, 0, 0), dActionEntry (262, 0, 119, 0, 0), 
			dActionEntry (263, 0, 75, 0, 0), dActionEntry (123, 0, 131, 0, 0), dActionEntry (44, 1, 21, 1, 65), dActionEntry (125, 1, 21, 1, 65), 
			dActionEntry (44, 1, 28, 1, 57), dActionEntry (125, 1, 28, 1, 57), dActionEntry (44, 0, 133, 0, 0), dActionEntry (125, 0, 134, 0, 0), 
			dActionEntry (255, 1, 9, 1, 66), dActionEntry (256, 1, 9, 1, 66), dActionEntry (259, 1, 9, 1, 66), dActionEntry (261, 1, 9, 1, 66), 
			dActionEntry (255, 1, 4, 4, 12), dActionEntry (256, 1, 4, 4, 12), dActionEntry (259, 1, 4, 4, 12), dActionEntry (261, 1, 4, 4, 12), 
			dActionEntry (46, 1, 6, 3, 10), dActionEntry (62, 1, 6, 3, 10), dActionEntry (260, 1, 9, 1, 66), dActionEntry (264, 1, 9, 1, 66), 
			dActionEntry (265, 1, 9, 1, 66), dActionEntry (266, 1, 9, 1, 66), dActionEntry (267, 1, 9, 1, 66), dActionEntry (268, 1, 9, 1, 66), 
			dActionEntry (269, 1, 9, 1, 66), dActionEntry (270, 1, 9, 1, 66), dActionEntry (271, 1, 9, 1, 66), dActionEntry (272, 1, 9, 1, 66), 
			dActionEntry (273, 1, 9, 1, 66), dActionEntry (274, 1, 9, 1, 66), dActionEntry (275, 1, 9, 1, 66), dActionEntry (276, 1, 9, 1, 66), 
			dActionEntry (277, 1, 9, 1, 66), dActionEntry (278, 1, 9, 1, 66), dActionEntry (279, 1, 9, 1, 66), dActionEntry (280, 1, 9, 1, 66), 
			dActionEntry (281, 1, 9, 1, 66), dActionEntry (282, 1, 9, 1, 66), dActionEntry (283, 1, 9, 1, 66), dActionEntry (284, 1, 9, 1, 66), 
			dActionEntry (285, 1, 9, 1, 66), dActionEntry (286, 1, 9, 1, 66), dActionEntry (287, 1, 9, 1, 66), dActionEntry (288, 1, 9, 1, 66), 
			dActionEntry (289, 1, 9, 1, 66), dActionEntry (290, 1, 9, 1, 66), dActionEntry (291, 1, 9, 1, 66), dActionEntry (292, 1, 9, 1, 66), 
			dActionEntry (293, 1, 9, 1, 66), dActionEntry (294, 1, 9, 1, 66), dActionEntry (295, 1, 9, 1, 66), dActionEntry (296, 1, 9, 1, 66), 
			dActionEntry (297, 1, 9, 1, 66), dActionEntry (298, 1, 9, 1, 66), dActionEntry (299, 1, 9, 1, 66), dActionEntry (300, 1, 9, 1, 66), 
			dActionEntry (260, 1, 14, 4, 61), dActionEntry (264, 1, 14, 4, 61), dActionEntry (265, 1, 14, 4, 61), dActionEntry (266, 1, 14, 4, 61), 
			dActionEntry (267, 1, 14, 4, 61), dActionEntry (268, 1, 14, 4, 61), dActionEntry (269, 1, 14, 4, 61), dActionEntry (270, 1, 14, 4, 61), 
			dActionEntry (271, 1, 14, 4, 61), dActionEntry (272, 1, 14, 4, 61), dActionEntry (273, 1, 14, 4, 61), dActionEntry (274, 1, 14, 4, 61), 
			dActionEntry (275, 1, 14, 4, 61), dActionEntry (276, 1, 14, 4, 61), dActionEntry (277, 1, 14, 4, 61), dActionEntry (278, 1, 14, 4, 61), 
			dActionEntry (279, 1, 14, 4, 61), dActionEntry (280, 1, 14, 4, 61), dActionEntry (281, 1, 14, 4, 61), dActionEntry (282, 1, 14, 4, 61), 
			dActionEntry (283, 1, 14, 4, 61), dActionEntry (284, 1, 14, 4, 61), dActionEntry (285, 1, 14, 4, 61), dActionEntry (286, 1, 14, 4, 61), 
			dActionEntry (287, 1, 14, 4, 61), dActionEntry (288, 1, 14, 4, 61), dActionEntry (289, 1, 14, 4, 61), dActionEntry (290, 1, 14, 4, 61), 
			dActionEntry (291, 1, 14, 4, 61), dActionEntry (292, 1, 14, 4, 61), dActionEntry (293, 1, 14, 4, 61), dActionEntry (294, 1, 14, 4, 61), 
			dActionEntry (295, 1, 14, 4, 61), dActionEntry (296, 1, 14, 4, 61), dActionEntry (297, 1, 14, 4, 61), dActionEntry (298, 1, 14, 4, 61), 
			dActionEntry (299, 1, 14, 4, 61), dActionEntry (300, 1, 14, 4, 61), dActionEntry (260, 1, 14, 4, 63), dActionEntry (264, 1, 14, 4, 63), 
			dActionEntry (265, 1, 14, 4, 63), dActionEntry (266, 1, 14, 4, 63), dActionEntry (267, 1, 14, 4, 63), dActionEntry (268, 1, 14, 4, 63), 
			dActionEntry (269, 1, 14, 4, 63), dActionEntry (270, 1, 14, 4, 63), dActionEntry (271, 1, 14, 4, 63), dActionEntry (272, 1, 14, 4, 63), 
			dActionEntry (273, 1, 14, 4, 63), dActionEntry (274, 1, 14, 4, 63), dActionEntry (275, 1, 14, 4, 63), dActionEntry (276, 1, 14, 4, 63), 
			dActionEntry (277, 1, 14, 4, 63), dActionEntry (278, 1, 14, 4, 63), dActionEntry (279, 1, 14, 4, 63), dActionEntry (280, 1, 14, 4, 63), 
			dActionEntry (281, 1, 14, 4, 63), dActionEntry (282, 1, 14, 4, 63), dActionEntry (283, 1, 14, 4, 63), dActionEntry (284, 1, 14, 4, 63), 
			dActionEntry (285, 1, 14, 4, 63), dActionEntry (286, 1, 14, 4, 63), dActionEntry (287, 1, 14, 4, 63), dActionEntry (288, 1, 14, 4, 63), 
			dActionEntry (289, 1, 14, 4, 63), dActionEntry (290, 1, 14, 4, 63), dActionEntry (291, 1, 14, 4, 63), dActionEntry (292, 1, 14, 4, 63), 
			dActionEntry (293, 1, 14, 4, 63), dActionEntry (294, 1, 14, 4, 63), dActionEntry (295, 1, 14, 4, 63), dActionEntry (296, 1, 14, 4, 63), 
			dActionEntry (297, 1, 14, 4, 63), dActionEntry (298, 1, 14, 4, 63), dActionEntry (299, 1, 14, 4, 63), dActionEntry (300, 1, 14, 4, 63), 
			dActionEntry (44, 0, 136, 0, 0), dActionEntry (260, 1, 17, 4, 68), dActionEntry (264, 1, 17, 4, 68), dActionEntry (265, 1, 17, 4, 68), 
			dActionEntry (266, 1, 17, 4, 68), dActionEntry (267, 1, 17, 4, 68), dActionEntry (268, 1, 17, 4, 68), dActionEntry (269, 1, 17, 4, 68), 
			dActionEntry (270, 1, 17, 4, 68), dActionEntry (271, 1, 17, 4, 68), dActionEntry (272, 1, 17, 4, 68), dActionEntry (273, 1, 17, 4, 68), 
			dActionEntry (274, 1, 17, 4, 68), dActionEntry (275, 1, 17, 4, 68), dActionEntry (276, 1, 17, 4, 68), dActionEntry (277, 1, 17, 4, 68), 
			dActionEntry (278, 1, 17, 4, 68), dActionEntry (279, 1, 17, 4, 68), dActionEntry (280, 1, 17, 4, 68), dActionEntry (281, 1, 17, 4, 68), 
			dActionEntry (282, 1, 17, 4, 68), dActionEntry (283, 1, 17, 4, 68), dActionEntry (284, 1, 17, 4, 68), dActionEntry (285, 1, 17, 4, 68), 
			dActionEntry (286, 1, 17, 4, 68), dActionEntry (287, 1, 17, 4, 68), dActionEntry (288, 1, 17, 4, 68), dActionEntry (289, 1, 17, 4, 68), 
			dActionEntry (290, 1, 17, 4, 68), dActionEntry (291, 1, 17, 4, 68), dActionEntry (292, 1, 17, 4, 68), dActionEntry (293, 1, 17, 4, 68), 
			dActionEntry (294, 1, 17, 4, 68), dActionEntry (295, 1, 17, 4, 68), dActionEntry (296, 1, 17, 4, 68), dActionEntry (297, 1, 17, 4, 68), 
			dActionEntry (298, 1, 17, 4, 68), dActionEntry (299, 1, 17, 4, 68), dActionEntry (300, 1, 17, 4, 68), dActionEntry (260, 1, 17, 4, 70), 
			dActionEntry (264, 1, 17, 4, 70), dActionEntry (265, 1, 17, 4, 70), dActionEntry (266, 1, 17, 4, 70), dActionEntry (267, 1, 17, 4, 70), 
			dActionEntry (268, 1, 17, 4, 70), dActionEntry (269, 1, 17, 4, 70), dActionEntry (270, 1, 17, 4, 70), dActionEntry (271, 1, 17, 4, 70), 
			dActionEntry (272, 1, 17, 4, 70), dActionEntry (273, 1, 17, 4, 70), dActionEntry (274, 1, 17, 4, 70), dActionEntry (275, 1, 17, 4, 70), 
			dActionEntry (276, 1, 17, 4, 70), dActionEntry (277, 1, 17, 4, 70), dActionEntry (278, 1, 17, 4, 70), dActionEntry (279, 1, 17, 4, 70), 
			dActionEntry (280, 1, 17, 4, 70), dActionEntry (281, 1, 17, 4, 70), dActionEntry (282, 1, 17, 4, 70), dActionEntry (283, 1, 17, 4, 70), 
			dActionEntry (284, 1, 17, 4, 70), dActionEntry (285, 1, 17, 4, 70), dActionEntry (286, 1, 17, 4, 70), dActionEntry (287, 1, 17, 4, 70), 
			dActionEntry (288, 1, 17, 4, 70), dActionEntry (289, 1, 17, 4, 70), dActionEntry (290, 1, 17, 4, 70), dActionEntry (291, 1, 17, 4, 70), 
			dActionEntry (292, 1, 17, 4, 70), dActionEntry (293, 1, 17, 4, 70), dActionEntry (294, 1, 17, 4, 70), dActionEntry (295, 1, 17, 4, 70), 
			dActionEntry (296, 1, 17, 4, 70), dActionEntry (297, 1, 17, 4, 70), dActionEntry (298, 1, 17, 4, 70), dActionEntry (299, 1, 17, 4, 70), 
			dActionEntry (300, 1, 17, 4, 70), dActionEntry (44, 0, 137, 0, 0), dActionEntry (260, 1, 18, 4, 80), dActionEntry (264, 1, 18, 4, 80), 
			dActionEntry (265, 1, 18, 4, 80), dActionEntry (266, 1, 18, 4, 80), dActionEntry (267, 1, 18, 4, 80), dActionEntry (268, 1, 18, 4, 80), 
			dActionEntry (269, 1, 18, 4, 80), dActionEntry (270, 1, 18, 4, 80), dActionEntry (271, 1, 18, 4, 80), dActionEntry (272, 1, 18, 4, 80), 
			dActionEntry (273, 1, 18, 4, 80), dActionEntry (274, 1, 18, 4, 80), dActionEntry (275, 1, 18, 4, 80), dActionEntry (276, 1, 18, 4, 80), 
			dActionEntry (277, 1, 18, 4, 80), dActionEntry (278, 1, 18, 4, 80), dActionEntry (279, 1, 18, 4, 80), dActionEntry (280, 1, 18, 4, 80), 
			dActionEntry (281, 1, 18, 4, 80), dActionEntry (282, 1, 18, 4, 80), dActionEntry (283, 1, 18, 4, 80), dActionEntry (284, 1, 18, 4, 80), 
			dActionEntry (285, 1, 18, 4, 80), dActionEntry (286, 1, 18, 4, 80), dActionEntry (287, 1, 18, 4, 80), dActionEntry (288, 1, 18, 4, 80), 
			dActionEntry (289, 1, 18, 4, 80), dActionEntry (290, 1, 18, 4, 80), dActionEntry (291, 1, 18, 4, 80), dActionEntry (292, 1, 18, 4, 80), 
			dActionEntry (293, 1, 18, 4, 80), dActionEntry (294, 1, 18, 4, 80), dActionEntry (295, 1, 18, 4, 80), dActionEntry (296, 1, 18, 4, 80), 
			dActionEntry (297, 1, 18, 4, 80), dActionEntry (298, 1, 18, 4, 80), dActionEntry (299, 1, 18, 4, 80), dActionEntry (300, 1, 18, 4, 80), 
			dActionEntry (44, 0, 138, 0, 0), dActionEntry (260, 1, 15, 4, 78), dActionEntry (264, 1, 15, 4, 78), dActionEntry (265, 1, 15, 4, 78), 
			dActionEntry (266, 1, 15, 4, 78), dActionEntry (267, 1, 15, 4, 78), dActionEntry (268, 1, 15, 4, 78), dActionEntry (269, 1, 15, 4, 78), 
			dActionEntry (270, 1, 15, 4, 78), dActionEntry (271, 1, 15, 4, 78), dActionEntry (272, 1, 15, 4, 78), dActionEntry (273, 1, 15, 4, 78), 
			dActionEntry (274, 1, 15, 4, 78), dActionEntry (275, 1, 15, 4, 78), dActionEntry (276, 1, 15, 4, 78), dActionEntry (277, 1, 15, 4, 78), 
			dActionEntry (278, 1, 15, 4, 78), dActionEntry (279, 1, 15, 4, 78), dActionEntry (280, 1, 15, 4, 78), dActionEntry (281, 1, 15, 4, 78), 
			dActionEntry (282, 1, 15, 4, 78), dActionEntry (283, 1, 15, 4, 78), dActionEntry (284, 1, 15, 4, 78), dActionEntry (285, 1, 15, 4, 78), 
			dActionEntry (286, 1, 15, 4, 78), dActionEntry (287, 1, 15, 4, 78), dActionEntry (288, 1, 15, 4, 78), dActionEntry (289, 1, 15, 4, 78), 
			dActionEntry (290, 1, 15, 4, 78), dActionEntry (291, 1, 15, 4, 78), dActionEntry (292, 1, 15, 4, 78), dActionEntry (293, 1, 15, 4, 78), 
			dActionEntry (294, 1, 15, 4, 78), dActionEntry (295, 1, 15, 4, 78), dActionEntry (296, 1, 15, 4, 78), dActionEntry (297, 1, 15, 4, 78), 
			dActionEntry (298, 1, 15, 4, 78), dActionEntry (299, 1, 15, 4, 78), dActionEntry (300, 1, 15, 4, 78), dActionEntry (44, 0, 139, 0, 0), 
			dActionEntry (260, 1, 12, 4, 25), dActionEntry (264, 1, 12, 4, 25), dActionEntry (265, 1, 12, 4, 25), dActionEntry (266, 1, 12, 4, 25), 
			dActionEntry (267, 1, 12, 4, 25), dActionEntry (268, 1, 12, 4, 25), dActionEntry (269, 1, 12, 4, 25), dActionEntry (270, 1, 12, 4, 25), 
			dActionEntry (271, 1, 12, 4, 25), dActionEntry (272, 1, 12, 4, 25), dActionEntry (273, 1, 12, 4, 25), dActionEntry (274, 1, 12, 4, 25), 
			dActionEntry (275, 1, 12, 4, 25), dActionEntry (276, 1, 12, 4, 25), dActionEntry (277, 1, 12, 4, 25), dActionEntry (278, 1, 12, 4, 25), 
			dActionEntry (279, 1, 12, 4, 25), dActionEntry (280, 1, 12, 4, 25), dActionEntry (281, 1, 12, 4, 25), dActionEntry (282, 1, 12, 4, 25), 
			dActionEntry (283, 1, 12, 4, 25), dActionEntry (284, 1, 12, 4, 25), dActionEntry (285, 1, 12, 4, 25), dActionEntry (286, 1, 12, 4, 25), 
			dActionEntry (287, 1, 12, 4, 25), dActionEntry (288, 1, 12, 4, 25), dActionEntry (289, 1, 12, 4, 25), dActionEntry (290, 1, 12, 4, 25), 
			dActionEntry (291, 1, 12, 4, 25), dActionEntry (292, 1, 12, 4, 25), dActionEntry (293, 1, 12, 4, 25), dActionEntry (294, 1, 12, 4, 25), 
			dActionEntry (295, 1, 12, 4, 25), dActionEntry (296, 1, 12, 4, 25), dActionEntry (297, 1, 12, 4, 25), dActionEntry (298, 1, 12, 4, 25), 
			dActionEntry (299, 1, 12, 4, 25), dActionEntry (300, 1, 12, 4, 25), dActionEntry (260, 1, 18, 4, 82), dActionEntry (264, 1, 18, 4, 82), 
			dActionEntry (265, 1, 18, 4, 82), dActionEntry (266, 1, 18, 4, 82), dActionEntry (267, 1, 18, 4, 82), dActionEntry (268, 1, 18, 4, 82), 
			dActionEntry (269, 1, 18, 4, 82), dActionEntry (270, 1, 18, 4, 82), dActionEntry (271, 1, 18, 4, 82), dActionEntry (272, 1, 18, 4, 82), 
			dActionEntry (273, 1, 18, 4, 82), dActionEntry (274, 1, 18, 4, 82), dActionEntry (275, 1, 18, 4, 82), dActionEntry (276, 1, 18, 4, 82), 
			dActionEntry (277, 1, 18, 4, 82), dActionEntry (278, 1, 18, 4, 82), dActionEntry (279, 1, 18, 4, 82), dActionEntry (280, 1, 18, 4, 82), 
			dActionEntry (281, 1, 18, 4, 82), dActionEntry (282, 1, 18, 4, 82), dActionEntry (283, 1, 18, 4, 82), dActionEntry (284, 1, 18, 4, 82), 
			dActionEntry (285, 1, 18, 4, 82), dActionEntry (286, 1, 18, 4, 82), dActionEntry (287, 1, 18, 4, 82), dActionEntry (288, 1, 18, 4, 82), 
			dActionEntry (289, 1, 18, 4, 82), dActionEntry (290, 1, 18, 4, 82), dActionEntry (291, 1, 18, 4, 82), dActionEntry (292, 1, 18, 4, 82), 
			dActionEntry (293, 1, 18, 4, 82), dActionEntry (294, 1, 18, 4, 82), dActionEntry (295, 1, 18, 4, 82), dActionEntry (296, 1, 18, 4, 82), 
			dActionEntry (297, 1, 18, 4, 82), dActionEntry (298, 1, 18, 4, 82), dActionEntry (299, 1, 18, 4, 82), dActionEntry (300, 1, 18, 4, 82), 
			dActionEntry (260, 1, 18, 4, 83), dActionEntry (264, 1, 18, 4, 83), dActionEntry (265, 1, 18, 4, 83), dActionEntry (266, 1, 18, 4, 83), 
			dActionEntry (267, 1, 18, 4, 83), dActionEntry (268, 1, 18, 4, 83), dActionEntry (269, 1, 18, 4, 83), dActionEntry (270, 1, 18, 4, 83), 
			dActionEntry (271, 1, 18, 4, 83), dActionEntry (272, 1, 18, 4, 83), dActionEntry (273, 1, 18, 4, 83), dActionEntry (274, 1, 18, 4, 83), 
			dActionEntry (275, 1, 18, 4, 83), dActionEntry (276, 1, 18, 4, 83), dActionEntry (277, 1, 18, 4, 83), dActionEntry (278, 1, 18, 4, 83), 
			dActionEntry (279, 1, 18, 4, 83), dActionEntry (280, 1, 18, 4, 83), dActionEntry (281, 1, 18, 4, 83), dActionEntry (282, 1, 18, 4, 83), 
			dActionEntry (283, 1, 18, 4, 83), dActionEntry (284, 1, 18, 4, 83), dActionEntry (285, 1, 18, 4, 83), dActionEntry (286, 1, 18, 4, 83), 
			dActionEntry (287, 1, 18, 4, 83), dActionEntry (288, 1, 18, 4, 83), dActionEntry (289, 1, 18, 4, 83), dActionEntry (290, 1, 18, 4, 83), 
			dActionEntry (291, 1, 18, 4, 83), dActionEntry (292, 1, 18, 4, 83), dActionEntry (293, 1, 18, 4, 83), dActionEntry (294, 1, 18, 4, 83), 
			dActionEntry (295, 1, 18, 4, 83), dActionEntry (296, 1, 18, 4, 83), dActionEntry (297, 1, 18, 4, 83), dActionEntry (298, 1, 18, 4, 83), 
			dActionEntry (299, 1, 18, 4, 83), dActionEntry (300, 1, 18, 4, 83), dActionEntry (260, 1, 13, 4, 26), dActionEntry (264, 1, 13, 4, 26), 
			dActionEntry (265, 1, 13, 4, 26), dActionEntry (266, 1, 13, 4, 26), dActionEntry (267, 1, 13, 4, 26), dActionEntry (268, 1, 13, 4, 26), 
			dActionEntry (269, 1, 13, 4, 26), dActionEntry (270, 1, 13, 4, 26), dActionEntry (271, 1, 13, 4, 26), dActionEntry (272, 1, 13, 4, 26), 
			dActionEntry (273, 1, 13, 4, 26), dActionEntry (274, 1, 13, 4, 26), dActionEntry (275, 1, 13, 4, 26), dActionEntry (276, 1, 13, 4, 26), 
			dActionEntry (277, 1, 13, 4, 26), dActionEntry (278, 1, 13, 4, 26), dActionEntry (279, 1, 13, 4, 26), dActionEntry (280, 1, 13, 4, 26), 
			dActionEntry (281, 1, 13, 4, 26), dActionEntry (282, 1, 13, 4, 26), dActionEntry (283, 1, 13, 4, 26), dActionEntry (284, 1, 13, 4, 26), 
			dActionEntry (285, 1, 13, 4, 26), dActionEntry (286, 1, 13, 4, 26), dActionEntry (287, 1, 13, 4, 26), dActionEntry (288, 1, 13, 4, 26), 
			dActionEntry (289, 1, 13, 4, 26), dActionEntry (290, 1, 13, 4, 26), dActionEntry (291, 1, 13, 4, 26), dActionEntry (292, 1, 13, 4, 26), 
			dActionEntry (293, 1, 13, 4, 26), dActionEntry (294, 1, 13, 4, 26), dActionEntry (295, 1, 13, 4, 26), dActionEntry (296, 1, 13, 4, 26), 
			dActionEntry (297, 1, 13, 4, 26), dActionEntry (298, 1, 13, 4, 26), dActionEntry (299, 1, 13, 4, 26), dActionEntry (300, 1, 13, 4, 26), 
			dActionEntry (257, 0, 77, 0, 0), dActionEntry (262, 0, 119, 0, 0), dActionEntry (262, 0, 119, 0, 0), dActionEntry (44, 0, 133, 0, 0), 
			dActionEntry (125, 0, 148, 0, 0), dActionEntry (44, 1, 28, 3, 58), dActionEntry (125, 1, 28, 3, 58), dActionEntry (260, 1, 14, 6, 62), 
			dActionEntry (264, 1, 14, 6, 62), dActionEntry (265, 1, 14, 6, 62), dActionEntry (266, 1, 14, 6, 62), dActionEntry (267, 1, 14, 6, 62), 
			dActionEntry (268, 1, 14, 6, 62), dActionEntry (269, 1, 14, 6, 62), dActionEntry (270, 1, 14, 6, 62), dActionEntry (271, 1, 14, 6, 62), 
			dActionEntry (272, 1, 14, 6, 62), dActionEntry (273, 1, 14, 6, 62), dActionEntry (274, 1, 14, 6, 62), dActionEntry (275, 1, 14, 6, 62), 
			dActionEntry (276, 1, 14, 6, 62), dActionEntry (277, 1, 14, 6, 62), dActionEntry (278, 1, 14, 6, 62), dActionEntry (279, 1, 14, 6, 62), 
			dActionEntry (280, 1, 14, 6, 62), dActionEntry (281, 1, 14, 6, 62), dActionEntry (282, 1, 14, 6, 62), dActionEntry (283, 1, 14, 6, 62), 
			dActionEntry (284, 1, 14, 6, 62), dActionEntry (285, 1, 14, 6, 62), dActionEntry (286, 1, 14, 6, 62), dActionEntry (287, 1, 14, 6, 62), 
			dActionEntry (288, 1, 14, 6, 62), dActionEntry (289, 1, 14, 6, 62), dActionEntry (290, 1, 14, 6, 62), dActionEntry (291, 1, 14, 6, 62), 
			dActionEntry (292, 1, 14, 6, 62), dActionEntry (293, 1, 14, 6, 62), dActionEntry (294, 1, 14, 6, 62), dActionEntry (295, 1, 14, 6, 62), 
			dActionEntry (296, 1, 14, 6, 62), dActionEntry (297, 1, 14, 6, 62), dActionEntry (298, 1, 14, 6, 62), dActionEntry (299, 1, 14, 6, 62), 
			dActionEntry (300, 1, 14, 6, 62), dActionEntry (260, 1, 14, 6, 64), dActionEntry (264, 1, 14, 6, 64), dActionEntry (265, 1, 14, 6, 64), 
			dActionEntry (266, 1, 14, 6, 64), dActionEntry (267, 1, 14, 6, 64), dActionEntry (268, 1, 14, 6, 64), dActionEntry (269, 1, 14, 6, 64), 
			dActionEntry (270, 1, 14, 6, 64), dActionEntry (271, 1, 14, 6, 64), dActionEntry (272, 1, 14, 6, 64), dActionEntry (273, 1, 14, 6, 64), 
			dActionEntry (274, 1, 14, 6, 64), dActionEntry (275, 1, 14, 6, 64), dActionEntry (276, 1, 14, 6, 64), dActionEntry (277, 1, 14, 6, 64), 
			dActionEntry (278, 1, 14, 6, 64), dActionEntry (279, 1, 14, 6, 64), dActionEntry (280, 1, 14, 6, 64), dActionEntry (281, 1, 14, 6, 64), 
			dActionEntry (282, 1, 14, 6, 64), dActionEntry (283, 1, 14, 6, 64), dActionEntry (284, 1, 14, 6, 64), dActionEntry (285, 1, 14, 6, 64), 
			dActionEntry (286, 1, 14, 6, 64), dActionEntry (287, 1, 14, 6, 64), dActionEntry (288, 1, 14, 6, 64), dActionEntry (289, 1, 14, 6, 64), 
			dActionEntry (290, 1, 14, 6, 64), dActionEntry (291, 1, 14, 6, 64), dActionEntry (292, 1, 14, 6, 64), dActionEntry (293, 1, 14, 6, 64), 
			dActionEntry (294, 1, 14, 6, 64), dActionEntry (295, 1, 14, 6, 64), dActionEntry (296, 1, 14, 6, 64), dActionEntry (297, 1, 14, 6, 64), 
			dActionEntry (298, 1, 14, 6, 64), dActionEntry (299, 1, 14, 6, 64), dActionEntry (300, 1, 14, 6, 64), dActionEntry (260, 1, 18, 6, 79), 
			dActionEntry (264, 1, 18, 6, 79), dActionEntry (265, 1, 18, 6, 79), dActionEntry (266, 1, 18, 6, 79), dActionEntry (267, 1, 18, 6, 79), 
			dActionEntry (268, 1, 18, 6, 79), dActionEntry (269, 1, 18, 6, 79), dActionEntry (270, 1, 18, 6, 79), dActionEntry (271, 1, 18, 6, 79), 
			dActionEntry (272, 1, 18, 6, 79), dActionEntry (273, 1, 18, 6, 79), dActionEntry (274, 1, 18, 6, 79), dActionEntry (275, 1, 18, 6, 79), 
			dActionEntry (276, 1, 18, 6, 79), dActionEntry (277, 1, 18, 6, 79), dActionEntry (278, 1, 18, 6, 79), dActionEntry (279, 1, 18, 6, 79), 
			dActionEntry (280, 1, 18, 6, 79), dActionEntry (281, 1, 18, 6, 79), dActionEntry (282, 1, 18, 6, 79), dActionEntry (283, 1, 18, 6, 79), 
			dActionEntry (284, 1, 18, 6, 79), dActionEntry (285, 1, 18, 6, 79), dActionEntry (286, 1, 18, 6, 79), dActionEntry (287, 1, 18, 6, 79), 
			dActionEntry (288, 1, 18, 6, 79), dActionEntry (289, 1, 18, 6, 79), dActionEntry (290, 1, 18, 6, 79), dActionEntry (291, 1, 18, 6, 79), 
			dActionEntry (292, 1, 18, 6, 79), dActionEntry (293, 1, 18, 6, 79), dActionEntry (294, 1, 18, 6, 79), dActionEntry (295, 1, 18, 6, 79), 
			dActionEntry (296, 1, 18, 6, 79), dActionEntry (297, 1, 18, 6, 79), dActionEntry (298, 1, 18, 6, 79), dActionEntry (299, 1, 18, 6, 79), 
			dActionEntry (300, 1, 18, 6, 79), dActionEntry (260, 1, 17, 6, 73), dActionEntry (264, 1, 17, 6, 73), dActionEntry (265, 1, 17, 6, 73), 
			dActionEntry (266, 1, 17, 6, 73), dActionEntry (267, 1, 17, 6, 73), dActionEntry (268, 1, 17, 6, 73), dActionEntry (269, 1, 17, 6, 73), 
			dActionEntry (270, 1, 17, 6, 73), dActionEntry (271, 1, 17, 6, 73), dActionEntry (272, 1, 17, 6, 73), dActionEntry (273, 1, 17, 6, 73), 
			dActionEntry (274, 1, 17, 6, 73), dActionEntry (275, 1, 17, 6, 73), dActionEntry (276, 1, 17, 6, 73), dActionEntry (277, 1, 17, 6, 73), 
			dActionEntry (278, 1, 17, 6, 73), dActionEntry (279, 1, 17, 6, 73), dActionEntry (280, 1, 17, 6, 73), dActionEntry (281, 1, 17, 6, 73), 
			dActionEntry (282, 1, 17, 6, 73), dActionEntry (283, 1, 17, 6, 73), dActionEntry (284, 1, 17, 6, 73), dActionEntry (285, 1, 17, 6, 73), 
			dActionEntry (286, 1, 17, 6, 73), dActionEntry (287, 1, 17, 6, 73), dActionEntry (288, 1, 17, 6, 73), dActionEntry (289, 1, 17, 6, 73), 
			dActionEntry (290, 1, 17, 6, 73), dActionEntry (291, 1, 17, 6, 73), dActionEntry (292, 1, 17, 6, 73), dActionEntry (293, 1, 17, 6, 73), 
			dActionEntry (294, 1, 17, 6, 73), dActionEntry (295, 1, 17, 6, 73), dActionEntry (296, 1, 17, 6, 73), dActionEntry (297, 1, 17, 6, 73), 
			dActionEntry (298, 1, 17, 6, 73), dActionEntry (299, 1, 17, 6, 73), dActionEntry (300, 1, 17, 6, 73), dActionEntry (260, 1, 16, 6, 77), 
			dActionEntry (264, 1, 16, 6, 77), dActionEntry (265, 1, 16, 6, 77), dActionEntry (266, 1, 16, 6, 77), dActionEntry (267, 1, 16, 6, 77), 
			dActionEntry (268, 1, 16, 6, 77), dActionEntry (269, 1, 16, 6, 77), dActionEntry (270, 1, 16, 6, 77), dActionEntry (271, 1, 16, 6, 77), 
			dActionEntry (272, 1, 16, 6, 77), dActionEntry (273, 1, 16, 6, 77), dActionEntry (274, 1, 16, 6, 77), dActionEntry (275, 1, 16, 6, 77), 
			dActionEntry (276, 1, 16, 6, 77), dActionEntry (277, 1, 16, 6, 77), dActionEntry (278, 1, 16, 6, 77), dActionEntry (279, 1, 16, 6, 77), 
			dActionEntry (280, 1, 16, 6, 77), dActionEntry (281, 1, 16, 6, 77), dActionEntry (282, 1, 16, 6, 77), dActionEntry (283, 1, 16, 6, 77), 
			dActionEntry (284, 1, 16, 6, 77), dActionEntry (285, 1, 16, 6, 77), dActionEntry (286, 1, 16, 6, 77), dActionEntry (287, 1, 16, 6, 77), 
			dActionEntry (288, 1, 16, 6, 77), dActionEntry (289, 1, 16, 6, 77), dActionEntry (290, 1, 16, 6, 77), dActionEntry (291, 1, 16, 6, 77), 
			dActionEntry (292, 1, 16, 6, 77), dActionEntry (293, 1, 16, 6, 77), dActionEntry (294, 1, 16, 6, 77), dActionEntry (295, 1, 16, 6, 77), 
			dActionEntry (296, 1, 16, 6, 77), dActionEntry (297, 1, 16, 6, 77), dActionEntry (298, 1, 16, 6, 77), dActionEntry (299, 1, 16, 6, 77), 
			dActionEntry (300, 1, 16, 6, 77), dActionEntry (260, 1, 16, 6, 76), dActionEntry (264, 1, 16, 6, 76), dActionEntry (265, 1, 16, 6, 76), 
			dActionEntry (266, 1, 16, 6, 76), dActionEntry (267, 1, 16, 6, 76), dActionEntry (268, 1, 16, 6, 76), dActionEntry (269, 1, 16, 6, 76), 
			dActionEntry (270, 1, 16, 6, 76), dActionEntry (271, 1, 16, 6, 76), dActionEntry (272, 1, 16, 6, 76), dActionEntry (273, 1, 16, 6, 76), 
			dActionEntry (274, 1, 16, 6, 76), dActionEntry (275, 1, 16, 6, 76), dActionEntry (276, 1, 16, 6, 76), dActionEntry (277, 1, 16, 6, 76), 
			dActionEntry (278, 1, 16, 6, 76), dActionEntry (279, 1, 16, 6, 76), dActionEntry (280, 1, 16, 6, 76), dActionEntry (281, 1, 16, 6, 76), 
			dActionEntry (282, 1, 16, 6, 76), dActionEntry (283, 1, 16, 6, 76), dActionEntry (284, 1, 16, 6, 76), dActionEntry (285, 1, 16, 6, 76), 
			dActionEntry (286, 1, 16, 6, 76), dActionEntry (287, 1, 16, 6, 76), dActionEntry (288, 1, 16, 6, 76), dActionEntry (289, 1, 16, 6, 76), 
			dActionEntry (290, 1, 16, 6, 76), dActionEntry (291, 1, 16, 6, 76), dActionEntry (292, 1, 16, 6, 76), dActionEntry (293, 1, 16, 6, 76), 
			dActionEntry (294, 1, 16, 6, 76), dActionEntry (295, 1, 16, 6, 76), dActionEntry (296, 1, 16, 6, 76), dActionEntry (297, 1, 16, 6, 76), 
			dActionEntry (298, 1, 16, 6, 76), dActionEntry (299, 1, 16, 6, 76), dActionEntry (300, 1, 16, 6, 76), dActionEntry (260, 1, 18, 6, 84), 
			dActionEntry (264, 1, 18, 6, 84), dActionEntry (265, 1, 18, 6, 84), dActionEntry (266, 1, 18, 6, 84), dActionEntry (267, 1, 18, 6, 84), 
			dActionEntry (268, 1, 18, 6, 84), dActionEntry (269, 1, 18, 6, 84), dActionEntry (270, 1, 18, 6, 84), dActionEntry (271, 1, 18, 6, 84), 
			dActionEntry (272, 1, 18, 6, 84), dActionEntry (273, 1, 18, 6, 84), dActionEntry (274, 1, 18, 6, 84), dActionEntry (275, 1, 18, 6, 84), 
			dActionEntry (276, 1, 18, 6, 84), dActionEntry (277, 1, 18, 6, 84), dActionEntry (278, 1, 18, 6, 84), dActionEntry (279, 1, 18, 6, 84), 
			dActionEntry (280, 1, 18, 6, 84), dActionEntry (281, 1, 18, 6, 84), dActionEntry (282, 1, 18, 6, 84), dActionEntry (283, 1, 18, 6, 84), 
			dActionEntry (284, 1, 18, 6, 84), dActionEntry (285, 1, 18, 6, 84), dActionEntry (286, 1, 18, 6, 84), dActionEntry (287, 1, 18, 6, 84), 
			dActionEntry (288, 1, 18, 6, 84), dActionEntry (289, 1, 18, 6, 84), dActionEntry (290, 1, 18, 6, 84), dActionEntry (291, 1, 18, 6, 84), 
			dActionEntry (292, 1, 18, 6, 84), dActionEntry (293, 1, 18, 6, 84), dActionEntry (294, 1, 18, 6, 84), dActionEntry (295, 1, 18, 6, 84), 
			dActionEntry (296, 1, 18, 6, 84), dActionEntry (297, 1, 18, 6, 84), dActionEntry (298, 1, 18, 6, 84), dActionEntry (299, 1, 18, 6, 84), 
			dActionEntry (300, 1, 18, 6, 84)};

	const int lastToken = 301;

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
					case 13:// rule dataType : INT 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitDataType (parameter[0].m_value);}
						break;
					case 14:// rule literal : LITERAL 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitSymbol (parameter[0].m_value);}
						break;
					case 41:// rule arithmeticOpcode : ABS 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::abs;}
						break;
					case 37:// rule arithmeticOpcode : ADD 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::add;}
						break;
					case 43:// rule arithmeticOpcode : AND 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::and;}
						break;
					case 50:// rule conditionalOpcode : BEQ 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::beq;}
						break;
					case 55:// rule conditionalOpcode : BGET 
						{entry.m_value = parameter[0].m_value; entry.m_value.m_semanticValue = dVirtualMachine::bget;}
						break;
					case 54:// rule conditionalOpcode : BGT 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::bgt;}
						break;
					case 53:// rule conditionalOpcode : BLE 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::ble;}
						break;
					case 52:// rule conditionalOpcode : BLT 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::blt;}
						break;
					case 51:// rule conditionalOpcode : BNE 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::bne;}
						break;
					case 40:// rule arithmeticOpcode : DIV 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::div;}
						break;
					case 60:// rule jumpLabel : JUMPLABEL 
						{ 
				dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
//				me->EmitJumpDestLabel (parameter[0].m_value);
			}
						break;
					case 49:// rule arithmeticOpcode : MOVE 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::mov;}
						break;
					case 39:// rule arithmeticOpcode : MUL 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::mul;}
						break;
					case 42:// rule arithmeticOpcode : NEG 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::neg;}
						break;
					case 46:// rule arithmeticOpcode : NOT 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::not;}
						break;
					case 44:// rule arithmeticOpcode : OR 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::or;}
						break;
					case 36:// rule pushPop : POP 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::pop;}
						break;
					case 35:// rule pushPop : PUSH 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::push;}
						break;
					case 71:// rule callJumpRet : RET 
						{
					dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
					dUserVariable register0 (REGISTER, "r0");
//					me->EmitRET (register0);
				}
						break;
					case 47:// rule arithmeticOpcode : SLL 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::sll;}
						break;
					case 48:// rule arithmeticOpcode : SRL 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::srl;}
						break;
					case 38:// rule arithmeticOpcode : SUB 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::sub;}
						break;
					case 45:// rule arithmeticOpcode : XOR 
						{entry.m_value = parameter[0].m_value;  entry.m_value.m_semanticValue = dVirtualMachine::xor;}
						break;
					case 11:// rule dataDeclaration : dataType literal 
						{((dAssemblerCompiler*)this)->EmitUnInitilizedDataDeclaration (parameter[0].m_value, parameter[1].m_value);}
						break;
					case 56:// rule beginFunction : BEGIN literal visibility 
						{ 
					dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
					me->EmitBeginFunction (parameter[1].m_value, parameter[2].m_value);
				}
						break;
					case 65:// rule register : REGISTER 
						{
				dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
				entry.m_value = me->TypeCheckRegister (parameter[0].m_value);
			}
						break;
					case 67:// rule callJumpRet : CALL literal 
						{
					dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
					dUserVariable register0 (REGISTER, "r0");
//					me->EmitCALL (register0, parameter[1].m_value);
				}
						break;
					case 72:// rule callJumpRet : RET register 
						{
					dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
//					me->EmitRET (parameter[1].m_value);
				}
						break;
					case 59:// rule functionDeclaration : beginFunction instructionList END 
						{
							dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
							me->EmitEndFunction ();
						}
						break;
					case 57:// rule registerMask : register 
						{entry.m_value = parameter[0].m_value; entry.m_value.m_semanticValue = 1 << parameter[0].m_value.m_semanticValue;}
						break;
					case 66:// rule constantExpression : INTEGER 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitIntegerConst (parameter[0].m_value);}
						break;
					case 12:// rule dataDeclaration : dataType literal = constantExpression 
						{((dAssemblerCompiler*)this)->EmitInitilizedDataDeclaration (parameter[0].m_value, parameter[1].m_value, parameter[3].m_value);}
						break;
					case 61:// rule constAdd : ADDI register , constantExpression 
						{ 
				dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
				dUserVariable register0 (REGISTER, "r0");
//				me->EmitADDIConstantExpresion (parameter[1].m_value, register0, parameter[3].m_value);
			}
						break;
					case 68:// rule callJumpRet : CALL register , literal 
						{
					dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
//					me->EmitCALL (parameter[1].m_value, parameter[3].m_value);
				}
						break;
					case 78:// rule arithmetic : arithmeticOpcode register , register 
						{
					dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
//					me->EmitArithmeticInstrution (parameter[0].m_value, parameter[1].m_value, parameter[3].m_value);
				}
						break;
					case 83:// rule stack : pushPop { registerMask } 
						{
			dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
			char text[64];
			sprintf (text, "r%d", me->m_virtualMachine->GetRegisterCount() - 1);
			dUserVariable stackRegister (REGISTER, text);
			stackRegister.m_semanticValue = me->m_virtualMachine->GetRegisterCount() - 1;
			me->EmitInstrutionType1 (parameter[0].m_value, stackRegister, parameter[2].m_value);
		}
						break;
					case 58:// rule registerMask : registerMask , register 
						{entry.m_value = parameter[0].m_value; entry.m_value.m_semanticValue = parameter[0].m_value.m_semanticValue | (1 << parameter[2].m_value.m_semanticValue);}
						break;
					case 62:// rule constAdd : ADDI register , register , constantExpression 
						{ 
				dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
//				me->EmitADDIConstantExpresion (parameter[1].m_value, parameter[3].m_value, parameter[5].m_value);
			}
						break;
					case 77:// rule conditional : conditionalOpcode register , register , constantExpression 
						{
					dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
//					me->EmitCompareAndJumpConstOffset (parameter[0].m_value, parameter[1].m_value, parameter[3].m_value, parameter[5].m_value);
				}
						break;
					case 76:// rule conditional : conditionalOpcode register , register , literal 
						{
					dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
//					me->EmitCompareAndJumpLocalLabel (parameter[0].m_value, parameter[1].m_value, parameter[3].m_value, parameter[5].m_value);
				}
						break;
					case 84:// rule stack : pushPop register , { registerMask } 
						{
				dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
				me->EmitInstrutionType1 (parameter[0].m_value, parameter[1].m_value, parameter[4].m_value);
			}
						break;

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





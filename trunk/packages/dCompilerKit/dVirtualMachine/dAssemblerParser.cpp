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
			1, 1, 1, 1, 38, 1, 38, 38, 1, 38, 38, 38, 38, 38, 1, 38, 1, 38, 38, 1, 38, 38, 1, 5, 
			5, 4, 37, 37, 2, 2, 2, 1, 1, 38, 38, 1, 39, 39, 1, 1, 38, 38, 38, 1, 38, 38, 39, 38, 
			39, 38, 1, 1, 1, 4, 38, 1, 1, 1, 1, 1, 1, 4, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 1, 1, 1, 4, 4, 2, 38, 38, 1, 38, 38, 1, 38, 38, 38, 38, 1, 38, 1, 38, 38, 38, 
			38, 2, 1, 1, 2, 38, 38, 38, 38, 38, 38};
	static short actionsStart[] = {
			0, 4, 5, 6, 7, 44, 48, 49, 53, 57, 58, 62, 66, 103, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 
			151, 152, 154, 155, 144, 144, 156, 157, 195, 196, 197, 198, 144, 199, 200, 201, 202, 203, 203, 205, 244, 245, 246, 247, 
			248, 249, 144, 250, 251, 144, 289, 327, 144, 365, 403, 441, 479, 517, 144, 555, 144, 593, 631, 144, 669, 707, 144, 745, 
			750, 755, 759, 796, 833, 835, 837, 839, 840, 841, 879, 917, 918, 957, 996, 997, 998, 1036, 1074, 1112, 1113, 1151, 1189, 1228, 
			1266, 1305, 1343, 1344, 1345, 1346, 1350, 1388, 1389, 1390, 1391, 1392, 141, 1393, 1397, 195, 156, 144, 156, 156, 1400, 1400, 144, 156, 
			144, 195, 156, 156, 195, 1401, 1405, 1409, 1411, 1449, 1487, 1488, 1526, 1564, 1565, 1603, 1641, 1679, 1717, 1718, 1756, 1757, 1795, 1833, 
			1871, 1909, 1400, 195, 1909, 1911, 1949, 1987, 2025, 2063, 2101};
	static short gotoCount[] = {
			8, 1, 0, 0, 19, 0, 1, 0, 0, 0, 0, 6, 0, 1, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 1, 1, 1, 0, 
			1, 0, 0, 0, 1, 0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 18, 0, 0, 1, 0, 
			1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0};
	static short gotoStart[] = {
			0, 8, 9, 9, 9, 28, 28, 29, 29, 29, 29, 29, 35, 35, 36, 38, 38, 38, 39, 39, 39, 39, 39, 39, 39, 39, 41, 42, 42, 43, 44, 45, 
			45, 46, 46, 46, 46, 47, 47, 47, 47, 47, 49, 51, 52, 52, 52, 52, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 73, 73, 73, 74, 
			74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 
			77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 78, 79, 79, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 96, 96, 
			96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 98, 99, 100, 102, 102, 102, 102, 102, 102};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (301, 9), dGotoEntry (302, 11), dGotoEntry (303, 10), dGotoEntry (304, 8), dGotoEntry (305, 5), 
			dGotoEntry (306, 7), dGotoEntry (309, 6), dGotoEntry (332, 4), dGotoEntry (308, 13), dGotoEntry (311, 59), 
			dGotoEntry (312, 58), dGotoEntry (313, 65), dGotoEntry (314, 61), dGotoEntry (315, 69), dGotoEntry (316, 66), 
			dGotoEntry (317, 63), dGotoEntry (318, 57), dGotoEntry (319, 52), dGotoEntry (320, 55), dGotoEntry (321, 54), 
			dGotoEntry (322, 68), dGotoEntry (323, 60), dGotoEntry (325, 62), dGotoEntry (326, 70), dGotoEntry (327, 67), 
			dGotoEntry (328, 64), dGotoEntry (329, 53), dGotoEntry (330, 56), dGotoEntry (308, 72), dGotoEntry (303, 73), 
			dGotoEntry (304, 8), dGotoEntry (305, 5), dGotoEntry (306, 7), dGotoEntry (309, 6), dGotoEntry (332, 4), 
			dGotoEntry (331, 75), dGotoEntry (307, 77), dGotoEntry (308, 78), dGotoEntry (324, 80), dGotoEntry (308, 82), 
			dGotoEntry (324, 83), dGotoEntry (324, 85), dGotoEntry (324, 86), dGotoEntry (324, 87), dGotoEntry (324, 89), 
			dGotoEntry (308, 90), dGotoEntry (324, 91), dGotoEntry (310, 93), dGotoEntry (324, 94), dGotoEntry (310, 95), 
			dGotoEntry (324, 96), dGotoEntry (324, 97), dGotoEntry (324, 98), dGotoEntry (324, 99), dGotoEntry (324, 100), 
			dGotoEntry (312, 102), dGotoEntry (313, 65), dGotoEntry (314, 61), dGotoEntry (315, 69), dGotoEntry (316, 66), 
			dGotoEntry (317, 63), dGotoEntry (318, 57), dGotoEntry (319, 52), dGotoEntry (320, 55), dGotoEntry (321, 54), 
			dGotoEntry (322, 68), dGotoEntry (323, 60), dGotoEntry (325, 62), dGotoEntry (326, 70), dGotoEntry (327, 67), 
			dGotoEntry (328, 64), dGotoEntry (329, 53), dGotoEntry (330, 56), dGotoEntry (324, 103), dGotoEntry (324, 104), 
			dGotoEntry (324, 105), dGotoEntry (324, 106), dGotoEntry (310, 126), dGotoEntry (308, 127), dGotoEntry (308, 129), 
			dGotoEntry (310, 128), dGotoEntry (324, 130), dGotoEntry (308, 131), dGotoEntry (324, 132), dGotoEntry (324, 133), 
			dGotoEntry (324, 134), dGotoEntry (324, 135), dGotoEntry (310, 136), dGotoEntry (310, 137), dGotoEntry (324, 138), 
			dGotoEntry (324, 139), dGotoEntry (324, 140), dGotoEntry (308, 141), dGotoEntry (324, 142), dGotoEntry (324, 143), 
			dGotoEntry (308, 144), dGotoEntry (308, 150), dGotoEntry (310, 149), dGotoEntry (310, 151), dGotoEntry (308, 152), 
			dGotoEntry (308, 154), dGotoEntry (310, 153)};
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
			dActionEntry (257, 0, 71, 0, 0), dActionEntry (255, 1, 2, 1, 7), dActionEntry (256, 1, 2, 1, 7), dActionEntry (259, 1, 2, 1, 7), 
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
			dActionEntry (298, 1, 7, 1, 14), dActionEntry (299, 1, 7, 1, 14), dActionEntry (300, 1, 7, 1, 14), dActionEntry (258, 0, 74, 0, 0), 
			dActionEntry (264, 1, 30, 0, 78), dActionEntry (265, 1, 30, 0, 78), dActionEntry (266, 1, 30, 0, 78), dActionEntry (267, 1, 30, 0, 78), 
			dActionEntry (268, 1, 30, 0, 78), dActionEntry (269, 1, 30, 0, 78), dActionEntry (270, 1, 30, 0, 78), dActionEntry (271, 1, 30, 0, 78), 
			dActionEntry (272, 1, 30, 0, 78), dActionEntry (273, 1, 30, 0, 78), dActionEntry (274, 1, 30, 0, 78), dActionEntry (275, 1, 30, 0, 78), 
			dActionEntry (276, 1, 30, 0, 78), dActionEntry (277, 1, 30, 0, 78), dActionEntry (278, 1, 30, 0, 78), dActionEntry (279, 1, 30, 0, 78), 
			dActionEntry (280, 1, 30, 0, 78), dActionEntry (281, 1, 30, 0, 78), dActionEntry (282, 1, 30, 0, 78), dActionEntry (283, 1, 30, 0, 78), 
			dActionEntry (284, 1, 30, 0, 78), dActionEntry (285, 1, 30, 0, 78), dActionEntry (286, 1, 30, 0, 78), dActionEntry (287, 1, 30, 0, 78), 
			dActionEntry (288, 1, 30, 0, 78), dActionEntry (289, 1, 30, 0, 78), dActionEntry (290, 1, 30, 0, 78), dActionEntry (291, 1, 30, 0, 78), 
			dActionEntry (292, 1, 30, 0, 78), dActionEntry (293, 1, 30, 0, 78), dActionEntry (294, 1, 30, 0, 78), dActionEntry (295, 1, 30, 0, 78), 
			dActionEntry (296, 1, 30, 0, 78), dActionEntry (297, 1, 30, 0, 78), dActionEntry (298, 1, 30, 0, 78), dActionEntry (299, 1, 30, 0, 78), 
			dActionEntry (300, 1, 30, 0, 78), dActionEntry (257, 0, 76, 0, 0), dActionEntry (263, 1, 28, 1, 69), dActionEntry (263, 1, 28, 1, 65), 
			dActionEntry (263, 0, 79, 0, 0), dActionEntry (263, 1, 27, 1, 61), dActionEntry (263, 1, 29, 1, 71), dActionEntry (263, 1, 29, 1, 76), 
			dActionEntry (263, 1, 29, 1, 75), dActionEntry (263, 1, 29, 1, 74), dActionEntry (263, 1, 29, 1, 73), dActionEntry (263, 1, 29, 1, 72), 
			dActionEntry (257, 0, 81, 0, 0), dActionEntry (263, 0, 79, 0, 0), dActionEntry (263, 0, 84, 0, 0), dActionEntry (263, 1, 28, 1, 68), 
			dActionEntry (263, 0, 88, 0, 0), dActionEntry (260, 1, 22, 1, 81), dActionEntry (264, 1, 22, 1, 81), dActionEntry (265, 1, 22, 1, 81), 
			dActionEntry (266, 1, 22, 1, 81), dActionEntry (267, 1, 22, 1, 81), dActionEntry (268, 1, 22, 1, 81), dActionEntry (269, 1, 22, 1, 81), 
			dActionEntry (270, 1, 22, 1, 81), dActionEntry (271, 1, 22, 1, 81), dActionEntry (272, 1, 22, 1, 81), dActionEntry (273, 1, 22, 1, 81), 
			dActionEntry (274, 1, 22, 1, 81), dActionEntry (275, 1, 22, 1, 81), dActionEntry (276, 1, 22, 1, 81), dActionEntry (277, 1, 22, 1, 81), 
			dActionEntry (278, 1, 22, 1, 81), dActionEntry (279, 1, 22, 1, 81), dActionEntry (280, 1, 22, 1, 81), dActionEntry (281, 1, 22, 1, 81), 
			dActionEntry (282, 1, 22, 1, 81), dActionEntry (283, 1, 22, 1, 81), dActionEntry (284, 1, 22, 1, 81), dActionEntry (285, 1, 22, 1, 81), 
			dActionEntry (286, 1, 22, 1, 81), dActionEntry (287, 1, 22, 1, 81), dActionEntry (288, 1, 22, 1, 81), dActionEntry (289, 1, 22, 1, 81), 
			dActionEntry (290, 1, 22, 1, 81), dActionEntry (291, 1, 22, 1, 81), dActionEntry (292, 1, 22, 1, 81), dActionEntry (293, 1, 22, 1, 81), 
			dActionEntry (294, 1, 22, 1, 81), dActionEntry (295, 1, 22, 1, 81), dActionEntry (296, 1, 22, 1, 81), dActionEntry (297, 1, 22, 1, 81), 
			dActionEntry (298, 1, 22, 1, 81), dActionEntry (299, 1, 22, 1, 81), dActionEntry (300, 1, 22, 1, 81), dActionEntry (257, 0, 81, 0, 0), 
			dActionEntry (263, 1, 24, 1, 53), dActionEntry (263, 1, 24, 1, 55), dActionEntry (263, 1, 24, 1, 54), dActionEntry (263, 1, 28, 1, 67), 
			dActionEntry (263, 1, 28, 1, 70), dActionEntry (263, 1, 27, 1, 64), dActionEntry (263, 1, 27, 1, 62), dActionEntry (262, 0, 92, 0, 0), 
			dActionEntry (263, 0, 84, 0, 0), dActionEntry (260, 1, 20, 1, 40), dActionEntry (263, 0, 88, 0, 0), dActionEntry (264, 1, 20, 1, 40), 
			dActionEntry (265, 1, 20, 1, 40), dActionEntry (266, 1, 20, 1, 40), dActionEntry (267, 1, 20, 1, 40), dActionEntry (268, 1, 20, 1, 40), 
			dActionEntry (269, 1, 20, 1, 40), dActionEntry (270, 1, 20, 1, 40), dActionEntry (271, 1, 20, 1, 40), dActionEntry (272, 1, 20, 1, 40), 
			dActionEntry (273, 1, 20, 1, 40), dActionEntry (274, 1, 20, 1, 40), dActionEntry (275, 1, 20, 1, 40), dActionEntry (276, 1, 20, 1, 40), 
			dActionEntry (277, 1, 20, 1, 40), dActionEntry (278, 1, 20, 1, 40), dActionEntry (279, 1, 20, 1, 40), dActionEntry (280, 1, 20, 1, 40), 
			dActionEntry (281, 1, 20, 1, 40), dActionEntry (282, 1, 20, 1, 40), dActionEntry (283, 1, 20, 1, 40), dActionEntry (284, 1, 20, 1, 40), 
			dActionEntry (285, 1, 20, 1, 40), dActionEntry (286, 1, 20, 1, 40), dActionEntry (287, 1, 20, 1, 40), dActionEntry (288, 1, 20, 1, 40), 
			dActionEntry (289, 1, 20, 1, 40), dActionEntry (290, 1, 20, 1, 40), dActionEntry (291, 1, 20, 1, 40), dActionEntry (292, 1, 20, 1, 40), 
			dActionEntry (293, 1, 20, 1, 40), dActionEntry (294, 1, 20, 1, 40), dActionEntry (295, 1, 20, 1, 40), dActionEntry (296, 1, 20, 1, 40), 
			dActionEntry (297, 1, 20, 1, 40), dActionEntry (298, 1, 20, 1, 40), dActionEntry (299, 1, 20, 1, 40), dActionEntry (300, 1, 20, 1, 40), 
			dActionEntry (263, 1, 26, 1, 59), dActionEntry (263, 1, 26, 1, 60), dActionEntry (263, 1, 25, 1, 56), dActionEntry (263, 1, 25, 1, 58), 
			dActionEntry (263, 1, 25, 1, 57), dActionEntry (263, 1, 28, 1, 66), dActionEntry (263, 1, 27, 1, 63), dActionEntry (260, 1, 11, 1, 23), 
			dActionEntry (264, 1, 11, 1, 23), dActionEntry (265, 1, 11, 1, 23), dActionEntry (266, 1, 11, 1, 23), dActionEntry (267, 1, 11, 1, 23), 
			dActionEntry (268, 1, 11, 1, 23), dActionEntry (269, 1, 11, 1, 23), dActionEntry (270, 1, 11, 1, 23), dActionEntry (271, 1, 11, 1, 23), 
			dActionEntry (272, 1, 11, 1, 23), dActionEntry (273, 1, 11, 1, 23), dActionEntry (274, 1, 11, 1, 23), dActionEntry (275, 1, 11, 1, 23), 
			dActionEntry (276, 1, 11, 1, 23), dActionEntry (277, 1, 11, 1, 23), dActionEntry (278, 1, 11, 1, 23), dActionEntry (279, 1, 11, 1, 23), 
			dActionEntry (280, 1, 11, 1, 23), dActionEntry (281, 1, 11, 1, 23), dActionEntry (282, 1, 11, 1, 23), dActionEntry (283, 1, 11, 1, 23), 
			dActionEntry (284, 1, 11, 1, 23), dActionEntry (285, 1, 11, 1, 23), dActionEntry (286, 1, 11, 1, 23), dActionEntry (287, 1, 11, 1, 23), 
			dActionEntry (288, 1, 11, 1, 23), dActionEntry (289, 1, 11, 1, 23), dActionEntry (290, 1, 11, 1, 23), dActionEntry (291, 1, 11, 1, 23), 
			dActionEntry (292, 1, 11, 1, 23), dActionEntry (293, 1, 11, 1, 23), dActionEntry (294, 1, 11, 1, 23), dActionEntry (295, 1, 11, 1, 23), 
			dActionEntry (296, 1, 11, 1, 23), dActionEntry (297, 1, 11, 1, 23), dActionEntry (298, 1, 11, 1, 23), dActionEntry (299, 1, 11, 1, 23), 
			dActionEntry (300, 1, 11, 1, 23), dActionEntry (260, 1, 11, 1, 25), dActionEntry (264, 1, 11, 1, 25), dActionEntry (265, 1, 11, 1, 25), 
			dActionEntry (266, 1, 11, 1, 25), dActionEntry (267, 1, 11, 1, 25), dActionEntry (268, 1, 11, 1, 25), dActionEntry (269, 1, 11, 1, 25), 
			dActionEntry (270, 1, 11, 1, 25), dActionEntry (271, 1, 11, 1, 25), dActionEntry (272, 1, 11, 1, 25), dActionEntry (273, 1, 11, 1, 25), 
			dActionEntry (274, 1, 11, 1, 25), dActionEntry (275, 1, 11, 1, 25), dActionEntry (276, 1, 11, 1, 25), dActionEntry (277, 1, 11, 1, 25), 
			dActionEntry (278, 1, 11, 1, 25), dActionEntry (279, 1, 11, 1, 25), dActionEntry (280, 1, 11, 1, 25), dActionEntry (281, 1, 11, 1, 25), 
			dActionEntry (282, 1, 11, 1, 25), dActionEntry (283, 1, 11, 1, 25), dActionEntry (284, 1, 11, 1, 25), dActionEntry (285, 1, 11, 1, 25), 
			dActionEntry (286, 1, 11, 1, 25), dActionEntry (287, 1, 11, 1, 25), dActionEntry (288, 1, 11, 1, 25), dActionEntry (289, 1, 11, 1, 25), 
			dActionEntry (290, 1, 11, 1, 25), dActionEntry (291, 1, 11, 1, 25), dActionEntry (292, 1, 11, 1, 25), dActionEntry (293, 1, 11, 1, 25), 
			dActionEntry (294, 1, 11, 1, 25), dActionEntry (295, 1, 11, 1, 25), dActionEntry (296, 1, 11, 1, 25), dActionEntry (297, 1, 11, 1, 25), 
			dActionEntry (298, 1, 11, 1, 25), dActionEntry (299, 1, 11, 1, 25), dActionEntry (300, 1, 11, 1, 25), dActionEntry (260, 1, 11, 1, 24), 
			dActionEntry (264, 1, 11, 1, 24), dActionEntry (265, 1, 11, 1, 24), dActionEntry (266, 1, 11, 1, 24), dActionEntry (267, 1, 11, 1, 24), 
			dActionEntry (268, 1, 11, 1, 24), dActionEntry (269, 1, 11, 1, 24), dActionEntry (270, 1, 11, 1, 24), dActionEntry (271, 1, 11, 1, 24), 
			dActionEntry (272, 1, 11, 1, 24), dActionEntry (273, 1, 11, 1, 24), dActionEntry (274, 1, 11, 1, 24), dActionEntry (275, 1, 11, 1, 24), 
			dActionEntry (276, 1, 11, 1, 24), dActionEntry (277, 1, 11, 1, 24), dActionEntry (278, 1, 11, 1, 24), dActionEntry (279, 1, 11, 1, 24), 
			dActionEntry (280, 1, 11, 1, 24), dActionEntry (281, 1, 11, 1, 24), dActionEntry (282, 1, 11, 1, 24), dActionEntry (283, 1, 11, 1, 24), 
			dActionEntry (284, 1, 11, 1, 24), dActionEntry (285, 1, 11, 1, 24), dActionEntry (286, 1, 11, 1, 24), dActionEntry (287, 1, 11, 1, 24), 
			dActionEntry (288, 1, 11, 1, 24), dActionEntry (289, 1, 11, 1, 24), dActionEntry (290, 1, 11, 1, 24), dActionEntry (291, 1, 11, 1, 24), 
			dActionEntry (292, 1, 11, 1, 24), dActionEntry (293, 1, 11, 1, 24), dActionEntry (294, 1, 11, 1, 24), dActionEntry (295, 1, 11, 1, 24), 
			dActionEntry (296, 1, 11, 1, 24), dActionEntry (297, 1, 11, 1, 24), dActionEntry (298, 1, 11, 1, 24), dActionEntry (299, 1, 11, 1, 24), 
			dActionEntry (300, 1, 11, 1, 24), dActionEntry (260, 1, 11, 1, 22), dActionEntry (264, 1, 11, 1, 22), dActionEntry (265, 1, 11, 1, 22), 
			dActionEntry (266, 1, 11, 1, 22), dActionEntry (267, 1, 11, 1, 22), dActionEntry (268, 1, 11, 1, 22), dActionEntry (269, 1, 11, 1, 22), 
			dActionEntry (270, 1, 11, 1, 22), dActionEntry (271, 1, 11, 1, 22), dActionEntry (272, 1, 11, 1, 22), dActionEntry (273, 1, 11, 1, 22), 
			dActionEntry (274, 1, 11, 1, 22), dActionEntry (275, 1, 11, 1, 22), dActionEntry (276, 1, 11, 1, 22), dActionEntry (277, 1, 11, 1, 22), 
			dActionEntry (278, 1, 11, 1, 22), dActionEntry (279, 1, 11, 1, 22), dActionEntry (280, 1, 11, 1, 22), dActionEntry (281, 1, 11, 1, 22), 
			dActionEntry (282, 1, 11, 1, 22), dActionEntry (283, 1, 11, 1, 22), dActionEntry (284, 1, 11, 1, 22), dActionEntry (285, 1, 11, 1, 22), 
			dActionEntry (286, 1, 11, 1, 22), dActionEntry (287, 1, 11, 1, 22), dActionEntry (288, 1, 11, 1, 22), dActionEntry (289, 1, 11, 1, 22), 
			dActionEntry (290, 1, 11, 1, 22), dActionEntry (291, 1, 11, 1, 22), dActionEntry (292, 1, 11, 1, 22), dActionEntry (293, 1, 11, 1, 22), 
			dActionEntry (294, 1, 11, 1, 22), dActionEntry (295, 1, 11, 1, 22), dActionEntry (296, 1, 11, 1, 22), dActionEntry (297, 1, 11, 1, 22), 
			dActionEntry (298, 1, 11, 1, 22), dActionEntry (299, 1, 11, 1, 22), dActionEntry (300, 1, 11, 1, 22), dActionEntry (260, 1, 10, 1, 15), 
			dActionEntry (264, 1, 10, 1, 15), dActionEntry (265, 1, 10, 1, 15), dActionEntry (266, 1, 10, 1, 15), dActionEntry (267, 1, 10, 1, 15), 
			dActionEntry (268, 1, 10, 1, 15), dActionEntry (269, 1, 10, 1, 15), dActionEntry (270, 1, 10, 1, 15), dActionEntry (271, 1, 10, 1, 15), 
			dActionEntry (272, 1, 10, 1, 15), dActionEntry (273, 1, 10, 1, 15), dActionEntry (274, 1, 10, 1, 15), dActionEntry (275, 1, 10, 1, 15), 
			dActionEntry (276, 1, 10, 1, 15), dActionEntry (277, 1, 10, 1, 15), dActionEntry (278, 1, 10, 1, 15), dActionEntry (279, 1, 10, 1, 15), 
			dActionEntry (280, 1, 10, 1, 15), dActionEntry (281, 1, 10, 1, 15), dActionEntry (282, 1, 10, 1, 15), dActionEntry (283, 1, 10, 1, 15), 
			dActionEntry (284, 1, 10, 1, 15), dActionEntry (285, 1, 10, 1, 15), dActionEntry (286, 1, 10, 1, 15), dActionEntry (287, 1, 10, 1, 15), 
			dActionEntry (288, 1, 10, 1, 15), dActionEntry (289, 1, 10, 1, 15), dActionEntry (290, 1, 10, 1, 15), dActionEntry (291, 1, 10, 1, 15), 
			dActionEntry (292, 1, 10, 1, 15), dActionEntry (293, 1, 10, 1, 15), dActionEntry (294, 1, 10, 1, 15), dActionEntry (295, 1, 10, 1, 15), 
			dActionEntry (296, 1, 10, 1, 15), dActionEntry (297, 1, 10, 1, 15), dActionEntry (298, 1, 10, 1, 15), dActionEntry (299, 1, 10, 1, 15), 
			dActionEntry (300, 1, 10, 1, 15), dActionEntry (260, 0, 101, 0, 0), dActionEntry (264, 0, 36, 0, 0), dActionEntry (265, 0, 33, 0, 0), 
			dActionEntry (266, 0, 35, 0, 0), dActionEntry (267, 0, 34, 0, 0), dActionEntry (268, 0, 46, 0, 0), dActionEntry (269, 0, 48, 0, 0), 
			dActionEntry (270, 0, 47, 0, 0), dActionEntry (271, 0, 44, 0, 0), dActionEntry (272, 0, 45, 0, 0), dActionEntry (273, 0, 18, 0, 0), 
			dActionEntry (274, 0, 40, 0, 0), dActionEntry (275, 0, 51, 0, 0), dActionEntry (276, 0, 39, 0, 0), dActionEntry (277, 0, 17, 0, 0), 
			dActionEntry (278, 0, 16, 0, 0), dActionEntry (279, 0, 49, 0, 0), dActionEntry (280, 0, 37, 0, 0), dActionEntry (281, 0, 27, 0, 0), 
			dActionEntry (282, 0, 15, 0, 0), dActionEntry (283, 0, 38, 0, 0), dActionEntry (284, 0, 19, 0, 0), dActionEntry (285, 0, 24, 0, 0), 
			dActionEntry (286, 0, 23, 0, 0), dActionEntry (287, 0, 22, 0, 0), dActionEntry (288, 0, 21, 0, 0), dActionEntry (289, 0, 20, 0, 0), 
			dActionEntry (290, 0, 25, 0, 0), dActionEntry (291, 0, 26, 0, 0), dActionEntry (292, 0, 43, 0, 0), dActionEntry (293, 0, 50, 0, 0), 
			dActionEntry (294, 0, 30, 0, 0), dActionEntry (295, 0, 32, 0, 0), dActionEntry (296, 0, 28, 0, 0), dActionEntry (297, 0, 29, 0, 0), 
			dActionEntry (298, 0, 42, 0, 0), dActionEntry (299, 0, 41, 0, 0), dActionEntry (300, 0, 31, 0, 0), dActionEntry (260, 1, 11, 1, 27), 
			dActionEntry (264, 1, 11, 1, 27), dActionEntry (265, 1, 11, 1, 27), dActionEntry (266, 1, 11, 1, 27), dActionEntry (267, 1, 11, 1, 27), 
			dActionEntry (268, 1, 11, 1, 27), dActionEntry (269, 1, 11, 1, 27), dActionEntry (270, 1, 11, 1, 27), dActionEntry (271, 1, 11, 1, 27), 
			dActionEntry (272, 1, 11, 1, 27), dActionEntry (273, 1, 11, 1, 27), dActionEntry (274, 1, 11, 1, 27), dActionEntry (275, 1, 11, 1, 27), 
			dActionEntry (276, 1, 11, 1, 27), dActionEntry (277, 1, 11, 1, 27), dActionEntry (278, 1, 11, 1, 27), dActionEntry (279, 1, 11, 1, 27), 
			dActionEntry (280, 1, 11, 1, 27), dActionEntry (281, 1, 11, 1, 27), dActionEntry (282, 1, 11, 1, 27), dActionEntry (283, 1, 11, 1, 27), 
			dActionEntry (284, 1, 11, 1, 27), dActionEntry (285, 1, 11, 1, 27), dActionEntry (286, 1, 11, 1, 27), dActionEntry (287, 1, 11, 1, 27), 
			dActionEntry (288, 1, 11, 1, 27), dActionEntry (289, 1, 11, 1, 27), dActionEntry (290, 1, 11, 1, 27), dActionEntry (291, 1, 11, 1, 27), 
			dActionEntry (292, 1, 11, 1, 27), dActionEntry (293, 1, 11, 1, 27), dActionEntry (294, 1, 11, 1, 27), dActionEntry (295, 1, 11, 1, 27), 
			dActionEntry (296, 1, 11, 1, 27), dActionEntry (297, 1, 11, 1, 27), dActionEntry (298, 1, 11, 1, 27), dActionEntry (299, 1, 11, 1, 27), 
			dActionEntry (300, 1, 11, 1, 27), dActionEntry (260, 1, 11, 1, 18), dActionEntry (264, 1, 11, 1, 18), dActionEntry (265, 1, 11, 1, 18), 
			dActionEntry (266, 1, 11, 1, 18), dActionEntry (267, 1, 11, 1, 18), dActionEntry (268, 1, 11, 1, 18), dActionEntry (269, 1, 11, 1, 18), 
			dActionEntry (270, 1, 11, 1, 18), dActionEntry (271, 1, 11, 1, 18), dActionEntry (272, 1, 11, 1, 18), dActionEntry (273, 1, 11, 1, 18), 
			dActionEntry (274, 1, 11, 1, 18), dActionEntry (275, 1, 11, 1, 18), dActionEntry (276, 1, 11, 1, 18), dActionEntry (277, 1, 11, 1, 18), 
			dActionEntry (278, 1, 11, 1, 18), dActionEntry (279, 1, 11, 1, 18), dActionEntry (280, 1, 11, 1, 18), dActionEntry (281, 1, 11, 1, 18), 
			dActionEntry (282, 1, 11, 1, 18), dActionEntry (283, 1, 11, 1, 18), dActionEntry (284, 1, 11, 1, 18), dActionEntry (285, 1, 11, 1, 18), 
			dActionEntry (286, 1, 11, 1, 18), dActionEntry (287, 1, 11, 1, 18), dActionEntry (288, 1, 11, 1, 18), dActionEntry (289, 1, 11, 1, 18), 
			dActionEntry (290, 1, 11, 1, 18), dActionEntry (291, 1, 11, 1, 18), dActionEntry (292, 1, 11, 1, 18), dActionEntry (293, 1, 11, 1, 18), 
			dActionEntry (294, 1, 11, 1, 18), dActionEntry (295, 1, 11, 1, 18), dActionEntry (296, 1, 11, 1, 18), dActionEntry (297, 1, 11, 1, 18), 
			dActionEntry (298, 1, 11, 1, 18), dActionEntry (299, 1, 11, 1, 18), dActionEntry (300, 1, 11, 1, 18), dActionEntry (260, 1, 11, 1, 21), 
			dActionEntry (264, 1, 11, 1, 21), dActionEntry (265, 1, 11, 1, 21), dActionEntry (266, 1, 11, 1, 21), dActionEntry (267, 1, 11, 1, 21), 
			dActionEntry (268, 1, 11, 1, 21), dActionEntry (269, 1, 11, 1, 21), dActionEntry (270, 1, 11, 1, 21), dActionEntry (271, 1, 11, 1, 21), 
			dActionEntry (272, 1, 11, 1, 21), dActionEntry (273, 1, 11, 1, 21), dActionEntry (274, 1, 11, 1, 21), dActionEntry (275, 1, 11, 1, 21), 
			dActionEntry (276, 1, 11, 1, 21), dActionEntry (277, 1, 11, 1, 21), dActionEntry (278, 1, 11, 1, 21), dActionEntry (279, 1, 11, 1, 21), 
			dActionEntry (280, 1, 11, 1, 21), dActionEntry (281, 1, 11, 1, 21), dActionEntry (282, 1, 11, 1, 21), dActionEntry (283, 1, 11, 1, 21), 
			dActionEntry (284, 1, 11, 1, 21), dActionEntry (285, 1, 11, 1, 21), dActionEntry (286, 1, 11, 1, 21), dActionEntry (287, 1, 11, 1, 21), 
			dActionEntry (288, 1, 11, 1, 21), dActionEntry (289, 1, 11, 1, 21), dActionEntry (290, 1, 11, 1, 21), dActionEntry (291, 1, 11, 1, 21), 
			dActionEntry (292, 1, 11, 1, 21), dActionEntry (293, 1, 11, 1, 21), dActionEntry (294, 1, 11, 1, 21), dActionEntry (295, 1, 11, 1, 21), 
			dActionEntry (296, 1, 11, 1, 21), dActionEntry (297, 1, 11, 1, 21), dActionEntry (298, 1, 11, 1, 21), dActionEntry (299, 1, 11, 1, 21), 
			dActionEntry (300, 1, 11, 1, 21), dActionEntry (260, 1, 11, 1, 17), dActionEntry (264, 1, 11, 1, 17), dActionEntry (265, 1, 11, 1, 17), 
			dActionEntry (266, 1, 11, 1, 17), dActionEntry (267, 1, 11, 1, 17), dActionEntry (268, 1, 11, 1, 17), dActionEntry (269, 1, 11, 1, 17), 
			dActionEntry (270, 1, 11, 1, 17), dActionEntry (271, 1, 11, 1, 17), dActionEntry (272, 1, 11, 1, 17), dActionEntry (273, 1, 11, 1, 17), 
			dActionEntry (274, 1, 11, 1, 17), dActionEntry (275, 1, 11, 1, 17), dActionEntry (276, 1, 11, 1, 17), dActionEntry (277, 1, 11, 1, 17), 
			dActionEntry (278, 1, 11, 1, 17), dActionEntry (279, 1, 11, 1, 17), dActionEntry (280, 1, 11, 1, 17), dActionEntry (281, 1, 11, 1, 17), 
			dActionEntry (282, 1, 11, 1, 17), dActionEntry (283, 1, 11, 1, 17), dActionEntry (284, 1, 11, 1, 17), dActionEntry (285, 1, 11, 1, 17), 
			dActionEntry (286, 1, 11, 1, 17), dActionEntry (287, 1, 11, 1, 17), dActionEntry (288, 1, 11, 1, 17), dActionEntry (289, 1, 11, 1, 17), 
			dActionEntry (290, 1, 11, 1, 17), dActionEntry (291, 1, 11, 1, 17), dActionEntry (292, 1, 11, 1, 17), dActionEntry (293, 1, 11, 1, 17), 
			dActionEntry (294, 1, 11, 1, 17), dActionEntry (295, 1, 11, 1, 17), dActionEntry (296, 1, 11, 1, 17), dActionEntry (297, 1, 11, 1, 17), 
			dActionEntry (298, 1, 11, 1, 17), dActionEntry (299, 1, 11, 1, 17), dActionEntry (300, 1, 11, 1, 17), dActionEntry (260, 1, 11, 1, 20), 
			dActionEntry (264, 1, 11, 1, 20), dActionEntry (265, 1, 11, 1, 20), dActionEntry (266, 1, 11, 1, 20), dActionEntry (267, 1, 11, 1, 20), 
			dActionEntry (268, 1, 11, 1, 20), dActionEntry (269, 1, 11, 1, 20), dActionEntry (270, 1, 11, 1, 20), dActionEntry (271, 1, 11, 1, 20), 
			dActionEntry (272, 1, 11, 1, 20), dActionEntry (273, 1, 11, 1, 20), dActionEntry (274, 1, 11, 1, 20), dActionEntry (275, 1, 11, 1, 20), 
			dActionEntry (276, 1, 11, 1, 20), dActionEntry (277, 1, 11, 1, 20), dActionEntry (278, 1, 11, 1, 20), dActionEntry (279, 1, 11, 1, 20), 
			dActionEntry (280, 1, 11, 1, 20), dActionEntry (281, 1, 11, 1, 20), dActionEntry (282, 1, 11, 1, 20), dActionEntry (283, 1, 11, 1, 20), 
			dActionEntry (284, 1, 11, 1, 20), dActionEntry (285, 1, 11, 1, 20), dActionEntry (286, 1, 11, 1, 20), dActionEntry (287, 1, 11, 1, 20), 
			dActionEntry (288, 1, 11, 1, 20), dActionEntry (289, 1, 11, 1, 20), dActionEntry (290, 1, 11, 1, 20), dActionEntry (291, 1, 11, 1, 20), 
			dActionEntry (292, 1, 11, 1, 20), dActionEntry (293, 1, 11, 1, 20), dActionEntry (294, 1, 11, 1, 20), dActionEntry (295, 1, 11, 1, 20), 
			dActionEntry (296, 1, 11, 1, 20), dActionEntry (297, 1, 11, 1, 20), dActionEntry (298, 1, 11, 1, 20), dActionEntry (299, 1, 11, 1, 20), 
			dActionEntry (300, 1, 11, 1, 20), dActionEntry (260, 1, 11, 1, 26), dActionEntry (264, 1, 11, 1, 26), dActionEntry (265, 1, 11, 1, 26), 
			dActionEntry (266, 1, 11, 1, 26), dActionEntry (267, 1, 11, 1, 26), dActionEntry (268, 1, 11, 1, 26), dActionEntry (269, 1, 11, 1, 26), 
			dActionEntry (270, 1, 11, 1, 26), dActionEntry (271, 1, 11, 1, 26), dActionEntry (272, 1, 11, 1, 26), dActionEntry (273, 1, 11, 1, 26), 
			dActionEntry (274, 1, 11, 1, 26), dActionEntry (275, 1, 11, 1, 26), dActionEntry (276, 1, 11, 1, 26), dActionEntry (277, 1, 11, 1, 26), 
			dActionEntry (278, 1, 11, 1, 26), dActionEntry (279, 1, 11, 1, 26), dActionEntry (280, 1, 11, 1, 26), dActionEntry (281, 1, 11, 1, 26), 
			dActionEntry (282, 1, 11, 1, 26), dActionEntry (283, 1, 11, 1, 26), dActionEntry (284, 1, 11, 1, 26), dActionEntry (285, 1, 11, 1, 26), 
			dActionEntry (286, 1, 11, 1, 26), dActionEntry (287, 1, 11, 1, 26), dActionEntry (288, 1, 11, 1, 26), dActionEntry (289, 1, 11, 1, 26), 
			dActionEntry (290, 1, 11, 1, 26), dActionEntry (291, 1, 11, 1, 26), dActionEntry (292, 1, 11, 1, 26), dActionEntry (293, 1, 11, 1, 26), 
			dActionEntry (294, 1, 11, 1, 26), dActionEntry (295, 1, 11, 1, 26), dActionEntry (296, 1, 11, 1, 26), dActionEntry (297, 1, 11, 1, 26), 
			dActionEntry (298, 1, 11, 1, 26), dActionEntry (299, 1, 11, 1, 26), dActionEntry (300, 1, 11, 1, 26), dActionEntry (260, 1, 11, 1, 19), 
			dActionEntry (264, 1, 11, 1, 19), dActionEntry (265, 1, 11, 1, 19), dActionEntry (266, 1, 11, 1, 19), dActionEntry (267, 1, 11, 1, 19), 
			dActionEntry (268, 1, 11, 1, 19), dActionEntry (269, 1, 11, 1, 19), dActionEntry (270, 1, 11, 1, 19), dActionEntry (271, 1, 11, 1, 19), 
			dActionEntry (272, 1, 11, 1, 19), dActionEntry (273, 1, 11, 1, 19), dActionEntry (274, 1, 11, 1, 19), dActionEntry (275, 1, 11, 1, 19), 
			dActionEntry (276, 1, 11, 1, 19), dActionEntry (277, 1, 11, 1, 19), dActionEntry (278, 1, 11, 1, 19), dActionEntry (279, 1, 11, 1, 19), 
			dActionEntry (280, 1, 11, 1, 19), dActionEntry (281, 1, 11, 1, 19), dActionEntry (282, 1, 11, 1, 19), dActionEntry (283, 1, 11, 1, 19), 
			dActionEntry (284, 1, 11, 1, 19), dActionEntry (285, 1, 11, 1, 19), dActionEntry (286, 1, 11, 1, 19), dActionEntry (287, 1, 11, 1, 19), 
			dActionEntry (288, 1, 11, 1, 19), dActionEntry (289, 1, 11, 1, 19), dActionEntry (290, 1, 11, 1, 19), dActionEntry (291, 1, 11, 1, 19), 
			dActionEntry (292, 1, 11, 1, 19), dActionEntry (293, 1, 11, 1, 19), dActionEntry (294, 1, 11, 1, 19), dActionEntry (295, 1, 11, 1, 19), 
			dActionEntry (296, 1, 11, 1, 19), dActionEntry (297, 1, 11, 1, 19), dActionEntry (298, 1, 11, 1, 19), dActionEntry (299, 1, 11, 1, 19), 
			dActionEntry (300, 1, 11, 1, 19), dActionEntry (61, 1, 7, 1, 14), dActionEntry (255, 1, 7, 1, 14), dActionEntry (256, 1, 7, 1, 14), 
			dActionEntry (259, 1, 7, 1, 14), dActionEntry (261, 1, 7, 1, 14), dActionEntry (61, 0, 107, 0, 0), dActionEntry (255, 1, 4, 2, 11), 
			dActionEntry (256, 1, 4, 2, 11), dActionEntry (259, 1, 4, 2, 11), dActionEntry (261, 1, 4, 2, 11), dActionEntry (255, 1, 1, 2, 4), 
			dActionEntry (256, 1, 1, 2, 4), dActionEntry (259, 1, 1, 2, 4), dActionEntry (261, 1, 1, 2, 4), dActionEntry (264, 1, 30, 1, 77), 
			dActionEntry (265, 1, 30, 1, 77), dActionEntry (266, 1, 30, 1, 77), dActionEntry (267, 1, 30, 1, 77), dActionEntry (268, 1, 30, 1, 77), 
			dActionEntry (269, 1, 30, 1, 77), dActionEntry (270, 1, 30, 1, 77), dActionEntry (271, 1, 30, 1, 77), dActionEntry (272, 1, 30, 1, 77), 
			dActionEntry (273, 1, 30, 1, 77), dActionEntry (274, 1, 30, 1, 77), dActionEntry (275, 1, 30, 1, 77), dActionEntry (276, 1, 30, 1, 77), 
			dActionEntry (277, 1, 30, 1, 77), dActionEntry (278, 1, 30, 1, 77), dActionEntry (279, 1, 30, 1, 77), dActionEntry (280, 1, 30, 1, 77), 
			dActionEntry (281, 1, 30, 1, 77), dActionEntry (282, 1, 30, 1, 77), dActionEntry (283, 1, 30, 1, 77), dActionEntry (284, 1, 30, 1, 77), 
			dActionEntry (285, 1, 30, 1, 77), dActionEntry (286, 1, 30, 1, 77), dActionEntry (287, 1, 30, 1, 77), dActionEntry (288, 1, 30, 1, 77), 
			dActionEntry (289, 1, 30, 1, 77), dActionEntry (290, 1, 30, 1, 77), dActionEntry (291, 1, 30, 1, 77), dActionEntry (292, 1, 30, 1, 77), 
			dActionEntry (293, 1, 30, 1, 77), dActionEntry (294, 1, 30, 1, 77), dActionEntry (295, 1, 30, 1, 77), dActionEntry (296, 1, 30, 1, 77), 
			dActionEntry (297, 1, 30, 1, 77), dActionEntry (298, 1, 30, 1, 77), dActionEntry (299, 1, 30, 1, 77), dActionEntry (300, 1, 30, 1, 77), 
			dActionEntry (264, 1, 31, 3, 79), dActionEntry (265, 1, 31, 3, 79), dActionEntry (266, 1, 31, 3, 79), dActionEntry (267, 1, 31, 3, 79), 
			dActionEntry (268, 1, 31, 3, 79), dActionEntry (269, 1, 31, 3, 79), dActionEntry (270, 1, 31, 3, 79), dActionEntry (271, 1, 31, 3, 79), 
			dActionEntry (272, 1, 31, 3, 79), dActionEntry (273, 1, 31, 3, 79), dActionEntry (274, 1, 31, 3, 79), dActionEntry (275, 1, 31, 3, 79), 
			dActionEntry (276, 1, 31, 3, 79), dActionEntry (277, 1, 31, 3, 79), dActionEntry (278, 1, 31, 3, 79), dActionEntry (279, 1, 31, 3, 79), 
			dActionEntry (280, 1, 31, 3, 79), dActionEntry (281, 1, 31, 3, 79), dActionEntry (282, 1, 31, 3, 79), dActionEntry (283, 1, 31, 3, 79), 
			dActionEntry (284, 1, 31, 3, 79), dActionEntry (285, 1, 31, 3, 79), dActionEntry (286, 1, 31, 3, 79), dActionEntry (287, 1, 31, 3, 79), 
			dActionEntry (288, 1, 31, 3, 79), dActionEntry (289, 1, 31, 3, 79), dActionEntry (290, 1, 31, 3, 79), dActionEntry (291, 1, 31, 3, 79), 
			dActionEntry (292, 1, 31, 3, 79), dActionEntry (293, 1, 31, 3, 79), dActionEntry (294, 1, 31, 3, 79), dActionEntry (295, 1, 31, 3, 79), 
			dActionEntry (296, 1, 31, 3, 79), dActionEntry (297, 1, 31, 3, 79), dActionEntry (298, 1, 31, 3, 79), dActionEntry (299, 1, 31, 3, 79), 
			dActionEntry (300, 1, 31, 3, 79), dActionEntry (46, 1, 7, 1, 14), dActionEntry (62, 1, 7, 1, 14), dActionEntry (46, 0, 108, 0, 0), 
			dActionEntry (62, 0, 109, 0, 0), dActionEntry (46, 1, 6, 1, 9), dActionEntry (62, 1, 6, 1, 9), dActionEntry (44, 1, 23, 1, 86), 
			dActionEntry (44, 0, 110, 0, 0), dActionEntry (260, 1, 7, 1, 14), dActionEntry (264, 1, 7, 1, 14), dActionEntry (265, 1, 7, 1, 14), 
			dActionEntry (266, 1, 7, 1, 14), dActionEntry (267, 1, 7, 1, 14), dActionEntry (268, 1, 7, 1, 14), dActionEntry (269, 1, 7, 1, 14), 
			dActionEntry (270, 1, 7, 1, 14), dActionEntry (271, 1, 7, 1, 14), dActionEntry (272, 1, 7, 1, 14), dActionEntry (273, 1, 7, 1, 14), 
			dActionEntry (274, 1, 7, 1, 14), dActionEntry (275, 1, 7, 1, 14), dActionEntry (276, 1, 7, 1, 14), dActionEntry (277, 1, 7, 1, 14), 
			dActionEntry (278, 1, 7, 1, 14), dActionEntry (279, 1, 7, 1, 14), dActionEntry (280, 1, 7, 1, 14), dActionEntry (281, 1, 7, 1, 14), 
			dActionEntry (282, 1, 7, 1, 14), dActionEntry (283, 1, 7, 1, 14), dActionEntry (284, 1, 7, 1, 14), dActionEntry (285, 1, 7, 1, 14), 
			dActionEntry (286, 1, 7, 1, 14), dActionEntry (287, 1, 7, 1, 14), dActionEntry (288, 1, 7, 1, 14), dActionEntry (289, 1, 7, 1, 14), 
			dActionEntry (290, 1, 7, 1, 14), dActionEntry (291, 1, 7, 1, 14), dActionEntry (292, 1, 7, 1, 14), dActionEntry (293, 1, 7, 1, 14), 
			dActionEntry (294, 1, 7, 1, 14), dActionEntry (295, 1, 7, 1, 14), dActionEntry (296, 1, 7, 1, 14), dActionEntry (297, 1, 7, 1, 14), 
			dActionEntry (298, 1, 7, 1, 14), dActionEntry (299, 1, 7, 1, 14), dActionEntry (300, 1, 7, 1, 14), dActionEntry (260, 1, 20, 2, 36), 
			dActionEntry (264, 1, 20, 2, 36), dActionEntry (265, 1, 20, 2, 36), dActionEntry (266, 1, 20, 2, 36), dActionEntry (267, 1, 20, 2, 36), 
			dActionEntry (268, 1, 20, 2, 36), dActionEntry (269, 1, 20, 2, 36), dActionEntry (270, 1, 20, 2, 36), dActionEntry (271, 1, 20, 2, 36), 
			dActionEntry (272, 1, 20, 2, 36), dActionEntry (273, 1, 20, 2, 36), dActionEntry (274, 1, 20, 2, 36), dActionEntry (275, 1, 20, 2, 36), 
			dActionEntry (276, 1, 20, 2, 36), dActionEntry (277, 1, 20, 2, 36), dActionEntry (278, 1, 20, 2, 36), dActionEntry (279, 1, 20, 2, 36), 
			dActionEntry (280, 1, 20, 2, 36), dActionEntry (281, 1, 20, 2, 36), dActionEntry (282, 1, 20, 2, 36), dActionEntry (283, 1, 20, 2, 36), 
			dActionEntry (284, 1, 20, 2, 36), dActionEntry (285, 1, 20, 2, 36), dActionEntry (286, 1, 20, 2, 36), dActionEntry (287, 1, 20, 2, 36), 
			dActionEntry (288, 1, 20, 2, 36), dActionEntry (289, 1, 20, 2, 36), dActionEntry (290, 1, 20, 2, 36), dActionEntry (291, 1, 20, 2, 36), 
			dActionEntry (292, 1, 20, 2, 36), dActionEntry (293, 1, 20, 2, 36), dActionEntry (294, 1, 20, 2, 36), dActionEntry (295, 1, 20, 2, 36), 
			dActionEntry (296, 1, 20, 2, 36), dActionEntry (297, 1, 20, 2, 36), dActionEntry (298, 1, 20, 2, 36), dActionEntry (299, 1, 20, 2, 36), 
			dActionEntry (300, 1, 20, 2, 36), dActionEntry (44, 0, 111, 0, 0), dActionEntry (44, 1, 23, 1, 86), dActionEntry (260, 1, 23, 1, 86), 
			dActionEntry (264, 1, 23, 1, 86), dActionEntry (265, 1, 23, 1, 86), dActionEntry (266, 1, 23, 1, 86), dActionEntry (267, 1, 23, 1, 86), 
			dActionEntry (268, 1, 23, 1, 86), dActionEntry (269, 1, 23, 1, 86), dActionEntry (270, 1, 23, 1, 86), dActionEntry (271, 1, 23, 1, 86), 
			dActionEntry (272, 1, 23, 1, 86), dActionEntry (273, 1, 23, 1, 86), dActionEntry (274, 1, 23, 1, 86), dActionEntry (275, 1, 23, 1, 86), 
			dActionEntry (276, 1, 23, 1, 86), dActionEntry (277, 1, 23, 1, 86), dActionEntry (278, 1, 23, 1, 86), dActionEntry (279, 1, 23, 1, 86), 
			dActionEntry (280, 1, 23, 1, 86), dActionEntry (281, 1, 23, 1, 86), dActionEntry (282, 1, 23, 1, 86), dActionEntry (283, 1, 23, 1, 86), 
			dActionEntry (284, 1, 23, 1, 86), dActionEntry (285, 1, 23, 1, 86), dActionEntry (286, 1, 23, 1, 86), dActionEntry (287, 1, 23, 1, 86), 
			dActionEntry (288, 1, 23, 1, 86), dActionEntry (289, 1, 23, 1, 86), dActionEntry (290, 1, 23, 1, 86), dActionEntry (291, 1, 23, 1, 86), 
			dActionEntry (292, 1, 23, 1, 86), dActionEntry (293, 1, 23, 1, 86), dActionEntry (294, 1, 23, 1, 86), dActionEntry (295, 1, 23, 1, 86), 
			dActionEntry (296, 1, 23, 1, 86), dActionEntry (297, 1, 23, 1, 86), dActionEntry (298, 1, 23, 1, 86), dActionEntry (299, 1, 23, 1, 86), 
			dActionEntry (300, 1, 23, 1, 86), dActionEntry (44, 0, 112, 0, 0), dActionEntry (260, 1, 20, 2, 38), dActionEntry (264, 1, 20, 2, 38), 
			dActionEntry (265, 1, 20, 2, 38), dActionEntry (266, 1, 20, 2, 38), dActionEntry (267, 1, 20, 2, 38), dActionEntry (268, 1, 20, 2, 38), 
			dActionEntry (269, 1, 20, 2, 38), dActionEntry (270, 1, 20, 2, 38), dActionEntry (271, 1, 20, 2, 38), dActionEntry (272, 1, 20, 2, 38), 
			dActionEntry (273, 1, 20, 2, 38), dActionEntry (274, 1, 20, 2, 38), dActionEntry (275, 1, 20, 2, 38), dActionEntry (276, 1, 20, 2, 38), 
			dActionEntry (277, 1, 20, 2, 38), dActionEntry (278, 1, 20, 2, 38), dActionEntry (279, 1, 20, 2, 38), dActionEntry (280, 1, 20, 2, 38), 
			dActionEntry (281, 1, 20, 2, 38), dActionEntry (282, 1, 20, 2, 38), dActionEntry (283, 1, 20, 2, 38), dActionEntry (284, 1, 20, 2, 38), 
			dActionEntry (285, 1, 20, 2, 38), dActionEntry (286, 1, 20, 2, 38), dActionEntry (287, 1, 20, 2, 38), dActionEntry (288, 1, 20, 2, 38), 
			dActionEntry (289, 1, 20, 2, 38), dActionEntry (290, 1, 20, 2, 38), dActionEntry (291, 1, 20, 2, 38), dActionEntry (292, 1, 20, 2, 38), 
			dActionEntry (293, 1, 20, 2, 38), dActionEntry (294, 1, 20, 2, 38), dActionEntry (295, 1, 20, 2, 38), dActionEntry (296, 1, 20, 2, 38), 
			dActionEntry (297, 1, 20, 2, 38), dActionEntry (298, 1, 20, 2, 38), dActionEntry (299, 1, 20, 2, 38), dActionEntry (300, 1, 20, 2, 38), 
			dActionEntry (44, 0, 113, 0, 0), dActionEntry (44, 0, 114, 0, 0), dActionEntry (260, 1, 23, 1, 86), dActionEntry (264, 1, 23, 1, 86), 
			dActionEntry (265, 1, 23, 1, 86), dActionEntry (266, 1, 23, 1, 86), dActionEntry (267, 1, 23, 1, 86), dActionEntry (268, 1, 23, 1, 86), 
			dActionEntry (269, 1, 23, 1, 86), dActionEntry (270, 1, 23, 1, 86), dActionEntry (271, 1, 23, 1, 86), dActionEntry (272, 1, 23, 1, 86), 
			dActionEntry (273, 1, 23, 1, 86), dActionEntry (274, 1, 23, 1, 86), dActionEntry (275, 1, 23, 1, 86), dActionEntry (276, 1, 23, 1, 86), 
			dActionEntry (277, 1, 23, 1, 86), dActionEntry (278, 1, 23, 1, 86), dActionEntry (279, 1, 23, 1, 86), dActionEntry (280, 1, 23, 1, 86), 
			dActionEntry (281, 1, 23, 1, 86), dActionEntry (282, 1, 23, 1, 86), dActionEntry (283, 1, 23, 1, 86), dActionEntry (284, 1, 23, 1, 86), 
			dActionEntry (285, 1, 23, 1, 86), dActionEntry (286, 1, 23, 1, 86), dActionEntry (287, 1, 23, 1, 86), dActionEntry (288, 1, 23, 1, 86), 
			dActionEntry (289, 1, 23, 1, 86), dActionEntry (290, 1, 23, 1, 86), dActionEntry (291, 1, 23, 1, 86), dActionEntry (292, 1, 23, 1, 86), 
			dActionEntry (293, 1, 23, 1, 86), dActionEntry (294, 1, 23, 1, 86), dActionEntry (295, 1, 23, 1, 86), dActionEntry (296, 1, 23, 1, 86), 
			dActionEntry (297, 1, 23, 1, 86), dActionEntry (298, 1, 23, 1, 86), dActionEntry (299, 1, 23, 1, 86), dActionEntry (300, 1, 23, 1, 86), 
			dActionEntry (260, 1, 20, 2, 43), dActionEntry (264, 1, 20, 2, 43), dActionEntry (265, 1, 20, 2, 43), dActionEntry (266, 1, 20, 2, 43), 
			dActionEntry (267, 1, 20, 2, 43), dActionEntry (268, 1, 20, 2, 43), dActionEntry (269, 1, 20, 2, 43), dActionEntry (270, 1, 20, 2, 43), 
			dActionEntry (271, 1, 20, 2, 43), dActionEntry (272, 1, 20, 2, 43), dActionEntry (273, 1, 20, 2, 43), dActionEntry (274, 1, 20, 2, 43), 
			dActionEntry (275, 1, 20, 2, 43), dActionEntry (276, 1, 20, 2, 43), dActionEntry (277, 1, 20, 2, 43), dActionEntry (278, 1, 20, 2, 43), 
			dActionEntry (279, 1, 20, 2, 43), dActionEntry (280, 1, 20, 2, 43), dActionEntry (281, 1, 20, 2, 43), dActionEntry (282, 1, 20, 2, 43), 
			dActionEntry (283, 1, 20, 2, 43), dActionEntry (284, 1, 20, 2, 43), dActionEntry (285, 1, 20, 2, 43), dActionEntry (286, 1, 20, 2, 43), 
			dActionEntry (287, 1, 20, 2, 43), dActionEntry (288, 1, 20, 2, 43), dActionEntry (289, 1, 20, 2, 43), dActionEntry (290, 1, 20, 2, 43), 
			dActionEntry (291, 1, 20, 2, 43), dActionEntry (292, 1, 20, 2, 43), dActionEntry (293, 1, 20, 2, 43), dActionEntry (294, 1, 20, 2, 43), 
			dActionEntry (295, 1, 20, 2, 43), dActionEntry (296, 1, 20, 2, 43), dActionEntry (297, 1, 20, 2, 43), dActionEntry (298, 1, 20, 2, 43), 
			dActionEntry (299, 1, 20, 2, 43), dActionEntry (300, 1, 20, 2, 43), dActionEntry (260, 1, 20, 2, 44), dActionEntry (264, 1, 20, 2, 44), 
			dActionEntry (265, 1, 20, 2, 44), dActionEntry (266, 1, 20, 2, 44), dActionEntry (267, 1, 20, 2, 44), dActionEntry (268, 1, 20, 2, 44), 
			dActionEntry (269, 1, 20, 2, 44), dActionEntry (270, 1, 20, 2, 44), dActionEntry (271, 1, 20, 2, 44), dActionEntry (272, 1, 20, 2, 44), 
			dActionEntry (273, 1, 20, 2, 44), dActionEntry (274, 1, 20, 2, 44), dActionEntry (275, 1, 20, 2, 44), dActionEntry (276, 1, 20, 2, 44), 
			dActionEntry (277, 1, 20, 2, 44), dActionEntry (278, 1, 20, 2, 44), dActionEntry (279, 1, 20, 2, 44), dActionEntry (280, 1, 20, 2, 44), 
			dActionEntry (281, 1, 20, 2, 44), dActionEntry (282, 1, 20, 2, 44), dActionEntry (283, 1, 20, 2, 44), dActionEntry (284, 1, 20, 2, 44), 
			dActionEntry (285, 1, 20, 2, 44), dActionEntry (286, 1, 20, 2, 44), dActionEntry (287, 1, 20, 2, 44), dActionEntry (288, 1, 20, 2, 44), 
			dActionEntry (289, 1, 20, 2, 44), dActionEntry (290, 1, 20, 2, 44), dActionEntry (291, 1, 20, 2, 44), dActionEntry (292, 1, 20, 2, 44), 
			dActionEntry (293, 1, 20, 2, 44), dActionEntry (294, 1, 20, 2, 44), dActionEntry (295, 1, 20, 2, 44), dActionEntry (296, 1, 20, 2, 44), 
			dActionEntry (297, 1, 20, 2, 44), dActionEntry (298, 1, 20, 2, 44), dActionEntry (299, 1, 20, 2, 44), dActionEntry (300, 1, 20, 2, 44), 
			dActionEntry (44, 0, 115, 0, 0), dActionEntry (260, 1, 9, 1, 87), dActionEntry (264, 1, 9, 1, 87), dActionEntry (265, 1, 9, 1, 87), 
			dActionEntry (266, 1, 9, 1, 87), dActionEntry (267, 1, 9, 1, 87), dActionEntry (268, 1, 9, 1, 87), dActionEntry (269, 1, 9, 1, 87), 
			dActionEntry (270, 1, 9, 1, 87), dActionEntry (271, 1, 9, 1, 87), dActionEntry (272, 1, 9, 1, 87), dActionEntry (273, 1, 9, 1, 87), 
			dActionEntry (274, 1, 9, 1, 87), dActionEntry (275, 1, 9, 1, 87), dActionEntry (276, 1, 9, 1, 87), dActionEntry (277, 1, 9, 1, 87), 
			dActionEntry (278, 1, 9, 1, 87), dActionEntry (279, 1, 9, 1, 87), dActionEntry (280, 1, 9, 1, 87), dActionEntry (281, 1, 9, 1, 87), 
			dActionEntry (282, 1, 9, 1, 87), dActionEntry (283, 1, 9, 1, 87), dActionEntry (284, 1, 9, 1, 87), dActionEntry (285, 1, 9, 1, 87), 
			dActionEntry (286, 1, 9, 1, 87), dActionEntry (287, 1, 9, 1, 87), dActionEntry (288, 1, 9, 1, 87), dActionEntry (289, 1, 9, 1, 87), 
			dActionEntry (290, 1, 9, 1, 87), dActionEntry (291, 1, 9, 1, 87), dActionEntry (292, 1, 9, 1, 87), dActionEntry (293, 1, 9, 1, 87), 
			dActionEntry (294, 1, 9, 1, 87), dActionEntry (295, 1, 9, 1, 87), dActionEntry (296, 1, 9, 1, 87), dActionEntry (297, 1, 9, 1, 87), 
			dActionEntry (298, 1, 9, 1, 87), dActionEntry (299, 1, 9, 1, 87), dActionEntry (300, 1, 9, 1, 87), dActionEntry (260, 1, 21, 2, 51), 
			dActionEntry (264, 1, 21, 2, 51), dActionEntry (265, 1, 21, 2, 51), dActionEntry (266, 1, 21, 2, 51), dActionEntry (267, 1, 21, 2, 51), 
			dActionEntry (268, 1, 21, 2, 51), dActionEntry (269, 1, 21, 2, 51), dActionEntry (270, 1, 21, 2, 51), dActionEntry (271, 1, 21, 2, 51), 
			dActionEntry (272, 1, 21, 2, 51), dActionEntry (273, 1, 21, 2, 51), dActionEntry (274, 1, 21, 2, 51), dActionEntry (275, 1, 21, 2, 51), 
			dActionEntry (276, 1, 21, 2, 51), dActionEntry (277, 1, 21, 2, 51), dActionEntry (278, 1, 21, 2, 51), dActionEntry (279, 1, 21, 2, 51), 
			dActionEntry (280, 1, 21, 2, 51), dActionEntry (281, 1, 21, 2, 51), dActionEntry (282, 1, 21, 2, 51), dActionEntry (283, 1, 21, 2, 51), 
			dActionEntry (284, 1, 21, 2, 51), dActionEntry (285, 1, 21, 2, 51), dActionEntry (286, 1, 21, 2, 51), dActionEntry (287, 1, 21, 2, 51), 
			dActionEntry (288, 1, 21, 2, 51), dActionEntry (289, 1, 21, 2, 51), dActionEntry (290, 1, 21, 2, 51), dActionEntry (291, 1, 21, 2, 51), 
			dActionEntry (292, 1, 21, 2, 51), dActionEntry (293, 1, 21, 2, 51), dActionEntry (294, 1, 21, 2, 51), dActionEntry (295, 1, 21, 2, 51), 
			dActionEntry (296, 1, 21, 2, 51), dActionEntry (297, 1, 21, 2, 51), dActionEntry (298, 1, 21, 2, 51), dActionEntry (299, 1, 21, 2, 51), 
			dActionEntry (300, 1, 21, 2, 51), dActionEntry (44, 0, 116, 0, 0), dActionEntry (260, 1, 21, 2, 50), dActionEntry (264, 1, 21, 2, 50), 
			dActionEntry (265, 1, 21, 2, 50), dActionEntry (266, 1, 21, 2, 50), dActionEntry (267, 1, 21, 2, 50), dActionEntry (268, 1, 21, 2, 50), 
			dActionEntry (269, 1, 21, 2, 50), dActionEntry (270, 1, 21, 2, 50), dActionEntry (271, 1, 21, 2, 50), dActionEntry (272, 1, 21, 2, 50), 
			dActionEntry (273, 1, 21, 2, 50), dActionEntry (274, 1, 21, 2, 50), dActionEntry (275, 1, 21, 2, 50), dActionEntry (276, 1, 21, 2, 50), 
			dActionEntry (277, 1, 21, 2, 50), dActionEntry (278, 1, 21, 2, 50), dActionEntry (279, 1, 21, 2, 50), dActionEntry (280, 1, 21, 2, 50), 
			dActionEntry (281, 1, 21, 2, 50), dActionEntry (282, 1, 21, 2, 50), dActionEntry (283, 1, 21, 2, 50), dActionEntry (284, 1, 21, 2, 50), 
			dActionEntry (285, 1, 21, 2, 50), dActionEntry (286, 1, 21, 2, 50), dActionEntry (287, 1, 21, 2, 50), dActionEntry (288, 1, 21, 2, 50), 
			dActionEntry (289, 1, 21, 2, 50), dActionEntry (290, 1, 21, 2, 50), dActionEntry (291, 1, 21, 2, 50), dActionEntry (292, 1, 21, 2, 50), 
			dActionEntry (293, 1, 21, 2, 50), dActionEntry (294, 1, 21, 2, 50), dActionEntry (295, 1, 21, 2, 50), dActionEntry (296, 1, 21, 2, 50), 
			dActionEntry (297, 1, 21, 2, 50), dActionEntry (298, 1, 21, 2, 50), dActionEntry (299, 1, 21, 2, 50), dActionEntry (300, 1, 21, 2, 50), 
			dActionEntry (260, 1, 21, 2, 48), dActionEntry (264, 1, 21, 2, 48), dActionEntry (265, 1, 21, 2, 48), dActionEntry (266, 1, 21, 2, 48), 
			dActionEntry (267, 1, 21, 2, 48), dActionEntry (268, 1, 21, 2, 48), dActionEntry (269, 1, 21, 2, 48), dActionEntry (270, 1, 21, 2, 48), 
			dActionEntry (271, 1, 21, 2, 48), dActionEntry (272, 1, 21, 2, 48), dActionEntry (273, 1, 21, 2, 48), dActionEntry (274, 1, 21, 2, 48), 
			dActionEntry (275, 1, 21, 2, 48), dActionEntry (276, 1, 21, 2, 48), dActionEntry (277, 1, 21, 2, 48), dActionEntry (278, 1, 21, 2, 48), 
			dActionEntry (279, 1, 21, 2, 48), dActionEntry (280, 1, 21, 2, 48), dActionEntry (281, 1, 21, 2, 48), dActionEntry (282, 1, 21, 2, 48), 
			dActionEntry (283, 1, 21, 2, 48), dActionEntry (284, 1, 21, 2, 48), dActionEntry (285, 1, 21, 2, 48), dActionEntry (286, 1, 21, 2, 48), 
			dActionEntry (287, 1, 21, 2, 48), dActionEntry (288, 1, 21, 2, 48), dActionEntry (289, 1, 21, 2, 48), dActionEntry (290, 1, 21, 2, 48), 
			dActionEntry (291, 1, 21, 2, 48), dActionEntry (292, 1, 21, 2, 48), dActionEntry (293, 1, 21, 2, 48), dActionEntry (294, 1, 21, 2, 48), 
			dActionEntry (295, 1, 21, 2, 48), dActionEntry (296, 1, 21, 2, 48), dActionEntry (297, 1, 21, 2, 48), dActionEntry (298, 1, 21, 2, 48), 
			dActionEntry (299, 1, 21, 2, 48), dActionEntry (300, 1, 21, 2, 48), dActionEntry (44, 0, 117, 0, 0), dActionEntry (260, 1, 21, 2, 47), 
			dActionEntry (264, 1, 21, 2, 47), dActionEntry (265, 1, 21, 2, 47), dActionEntry (266, 1, 21, 2, 47), dActionEntry (267, 1, 21, 2, 47), 
			dActionEntry (268, 1, 21, 2, 47), dActionEntry (269, 1, 21, 2, 47), dActionEntry (270, 1, 21, 2, 47), dActionEntry (271, 1, 21, 2, 47), 
			dActionEntry (272, 1, 21, 2, 47), dActionEntry (273, 1, 21, 2, 47), dActionEntry (274, 1, 21, 2, 47), dActionEntry (275, 1, 21, 2, 47), 
			dActionEntry (276, 1, 21, 2, 47), dActionEntry (277, 1, 21, 2, 47), dActionEntry (278, 1, 21, 2, 47), dActionEntry (279, 1, 21, 2, 47), 
			dActionEntry (280, 1, 21, 2, 47), dActionEntry (281, 1, 21, 2, 47), dActionEntry (282, 1, 21, 2, 47), dActionEntry (283, 1, 21, 2, 47), 
			dActionEntry (284, 1, 21, 2, 47), dActionEntry (285, 1, 21, 2, 47), dActionEntry (286, 1, 21, 2, 47), dActionEntry (287, 1, 21, 2, 47), 
			dActionEntry (288, 1, 21, 2, 47), dActionEntry (289, 1, 21, 2, 47), dActionEntry (290, 1, 21, 2, 47), dActionEntry (291, 1, 21, 2, 47), 
			dActionEntry (292, 1, 21, 2, 47), dActionEntry (293, 1, 21, 2, 47), dActionEntry (294, 1, 21, 2, 47), dActionEntry (295, 1, 21, 2, 47), 
			dActionEntry (296, 1, 21, 2, 47), dActionEntry (297, 1, 21, 2, 47), dActionEntry (298, 1, 21, 2, 47), dActionEntry (299, 1, 21, 2, 47), 
			dActionEntry (300, 1, 21, 2, 47), dActionEntry (260, 1, 20, 2, 41), dActionEntry (264, 1, 20, 2, 41), dActionEntry (265, 1, 20, 2, 41), 
			dActionEntry (266, 1, 20, 2, 41), dActionEntry (267, 1, 20, 2, 41), dActionEntry (268, 1, 20, 2, 41), dActionEntry (269, 1, 20, 2, 41), 
			dActionEntry (270, 1, 20, 2, 41), dActionEntry (271, 1, 20, 2, 41), dActionEntry (272, 1, 20, 2, 41), dActionEntry (273, 1, 20, 2, 41), 
			dActionEntry (274, 1, 20, 2, 41), dActionEntry (275, 1, 20, 2, 41), dActionEntry (276, 1, 20, 2, 41), dActionEntry (277, 1, 20, 2, 41), 
			dActionEntry (278, 1, 20, 2, 41), dActionEntry (279, 1, 20, 2, 41), dActionEntry (280, 1, 20, 2, 41), dActionEntry (281, 1, 20, 2, 41), 
			dActionEntry (282, 1, 20, 2, 41), dActionEntry (283, 1, 20, 2, 41), dActionEntry (284, 1, 20, 2, 41), dActionEntry (285, 1, 20, 2, 41), 
			dActionEntry (286, 1, 20, 2, 41), dActionEntry (287, 1, 20, 2, 41), dActionEntry (288, 1, 20, 2, 41), dActionEntry (289, 1, 20, 2, 41), 
			dActionEntry (290, 1, 20, 2, 41), dActionEntry (291, 1, 20, 2, 41), dActionEntry (292, 1, 20, 2, 41), dActionEntry (293, 1, 20, 2, 41), 
			dActionEntry (294, 1, 20, 2, 41), dActionEntry (295, 1, 20, 2, 41), dActionEntry (296, 1, 20, 2, 41), dActionEntry (297, 1, 20, 2, 41), 
			dActionEntry (298, 1, 20, 2, 41), dActionEntry (299, 1, 20, 2, 41), dActionEntry (300, 1, 20, 2, 41), dActionEntry (44, 0, 118, 0, 0), 
			dActionEntry (44, 0, 119, 0, 0), dActionEntry (44, 0, 120, 0, 0), dActionEntry (255, 1, 5, 3, 80), dActionEntry (256, 1, 5, 3, 80), 
			dActionEntry (259, 1, 5, 3, 80), dActionEntry (261, 1, 5, 3, 80), dActionEntry (260, 1, 10, 2, 16), dActionEntry (264, 1, 10, 2, 16), 
			dActionEntry (265, 1, 10, 2, 16), dActionEntry (266, 1, 10, 2, 16), dActionEntry (267, 1, 10, 2, 16), dActionEntry (268, 1, 10, 2, 16), 
			dActionEntry (269, 1, 10, 2, 16), dActionEntry (270, 1, 10, 2, 16), dActionEntry (271, 1, 10, 2, 16), dActionEntry (272, 1, 10, 2, 16), 
			dActionEntry (273, 1, 10, 2, 16), dActionEntry (274, 1, 10, 2, 16), dActionEntry (275, 1, 10, 2, 16), dActionEntry (276, 1, 10, 2, 16), 
			dActionEntry (277, 1, 10, 2, 16), dActionEntry (278, 1, 10, 2, 16), dActionEntry (279, 1, 10, 2, 16), dActionEntry (280, 1, 10, 2, 16), 
			dActionEntry (281, 1, 10, 2, 16), dActionEntry (282, 1, 10, 2, 16), dActionEntry (283, 1, 10, 2, 16), dActionEntry (284, 1, 10, 2, 16), 
			dActionEntry (285, 1, 10, 2, 16), dActionEntry (286, 1, 10, 2, 16), dActionEntry (287, 1, 10, 2, 16), dActionEntry (288, 1, 10, 2, 16), 
			dActionEntry (289, 1, 10, 2, 16), dActionEntry (290, 1, 10, 2, 16), dActionEntry (291, 1, 10, 2, 16), dActionEntry (292, 1, 10, 2, 16), 
			dActionEntry (293, 1, 10, 2, 16), dActionEntry (294, 1, 10, 2, 16), dActionEntry (295, 1, 10, 2, 16), dActionEntry (296, 1, 10, 2, 16), 
			dActionEntry (297, 1, 10, 2, 16), dActionEntry (298, 1, 10, 2, 16), dActionEntry (299, 1, 10, 2, 16), dActionEntry (300, 1, 10, 2, 16), 
			dActionEntry (44, 0, 121, 0, 0), dActionEntry (44, 0, 122, 0, 0), dActionEntry (44, 0, 123, 0, 0), dActionEntry (44, 0, 124, 0, 0), 
			dActionEntry (262, 0, 125, 0, 0), dActionEntry (255, 1, 3, 4, 8), dActionEntry (256, 1, 3, 4, 8), dActionEntry (259, 1, 3, 4, 8), 
			dActionEntry (261, 1, 3, 4, 8), dActionEntry (257, 0, 81, 0, 0), dActionEntry (262, 0, 92, 0, 0), dActionEntry (263, 0, 79, 0, 0), 
			dActionEntry (262, 0, 92, 0, 0), dActionEntry (255, 1, 9, 1, 87), dActionEntry (256, 1, 9, 1, 87), dActionEntry (259, 1, 9, 1, 87), 
			dActionEntry (261, 1, 9, 1, 87), dActionEntry (255, 1, 4, 4, 12), dActionEntry (256, 1, 4, 4, 12), dActionEntry (259, 1, 4, 4, 12), 
			dActionEntry (261, 1, 4, 4, 12), dActionEntry (46, 1, 6, 3, 10), dActionEntry (62, 1, 6, 3, 10), dActionEntry (260, 1, 17, 4, 82), 
			dActionEntry (264, 1, 17, 4, 82), dActionEntry (265, 1, 17, 4, 82), dActionEntry (266, 1, 17, 4, 82), dActionEntry (267, 1, 17, 4, 82), 
			dActionEntry (268, 1, 17, 4, 82), dActionEntry (269, 1, 17, 4, 82), dActionEntry (270, 1, 17, 4, 82), dActionEntry (271, 1, 17, 4, 82), 
			dActionEntry (272, 1, 17, 4, 82), dActionEntry (273, 1, 17, 4, 82), dActionEntry (274, 1, 17, 4, 82), dActionEntry (275, 1, 17, 4, 82), 
			dActionEntry (276, 1, 17, 4, 82), dActionEntry (277, 1, 17, 4, 82), dActionEntry (278, 1, 17, 4, 82), dActionEntry (279, 1, 17, 4, 82), 
			dActionEntry (280, 1, 17, 4, 82), dActionEntry (281, 1, 17, 4, 82), dActionEntry (282, 1, 17, 4, 82), dActionEntry (283, 1, 17, 4, 82), 
			dActionEntry (284, 1, 17, 4, 82), dActionEntry (285, 1, 17, 4, 82), dActionEntry (286, 1, 17, 4, 82), dActionEntry (287, 1, 17, 4, 82), 
			dActionEntry (288, 1, 17, 4, 82), dActionEntry (289, 1, 17, 4, 82), dActionEntry (290, 1, 17, 4, 82), dActionEntry (291, 1, 17, 4, 82), 
			dActionEntry (292, 1, 17, 4, 82), dActionEntry (293, 1, 17, 4, 82), dActionEntry (294, 1, 17, 4, 82), dActionEntry (295, 1, 17, 4, 82), 
			dActionEntry (296, 1, 17, 4, 82), dActionEntry (297, 1, 17, 4, 82), dActionEntry (298, 1, 17, 4, 82), dActionEntry (299, 1, 17, 4, 82), 
			dActionEntry (300, 1, 17, 4, 82), dActionEntry (260, 1, 17, 4, 84), dActionEntry (264, 1, 17, 4, 84), dActionEntry (265, 1, 17, 4, 84), 
			dActionEntry (266, 1, 17, 4, 84), dActionEntry (267, 1, 17, 4, 84), dActionEntry (268, 1, 17, 4, 84), dActionEntry (269, 1, 17, 4, 84), 
			dActionEntry (270, 1, 17, 4, 84), dActionEntry (271, 1, 17, 4, 84), dActionEntry (272, 1, 17, 4, 84), dActionEntry (273, 1, 17, 4, 84), 
			dActionEntry (274, 1, 17, 4, 84), dActionEntry (275, 1, 17, 4, 84), dActionEntry (276, 1, 17, 4, 84), dActionEntry (277, 1, 17, 4, 84), 
			dActionEntry (278, 1, 17, 4, 84), dActionEntry (279, 1, 17, 4, 84), dActionEntry (280, 1, 17, 4, 84), dActionEntry (281, 1, 17, 4, 84), 
			dActionEntry (282, 1, 17, 4, 84), dActionEntry (283, 1, 17, 4, 84), dActionEntry (284, 1, 17, 4, 84), dActionEntry (285, 1, 17, 4, 84), 
			dActionEntry (286, 1, 17, 4, 84), dActionEntry (287, 1, 17, 4, 84), dActionEntry (288, 1, 17, 4, 84), dActionEntry (289, 1, 17, 4, 84), 
			dActionEntry (290, 1, 17, 4, 84), dActionEntry (291, 1, 17, 4, 84), dActionEntry (292, 1, 17, 4, 84), dActionEntry (293, 1, 17, 4, 84), 
			dActionEntry (294, 1, 17, 4, 84), dActionEntry (295, 1, 17, 4, 84), dActionEntry (296, 1, 17, 4, 84), dActionEntry (297, 1, 17, 4, 84), 
			dActionEntry (298, 1, 17, 4, 84), dActionEntry (299, 1, 17, 4, 84), dActionEntry (300, 1, 17, 4, 84), dActionEntry (44, 0, 145, 0, 0), 
			dActionEntry (260, 1, 20, 4, 37), dActionEntry (264, 1, 20, 4, 37), dActionEntry (265, 1, 20, 4, 37), dActionEntry (266, 1, 20, 4, 37), 
			dActionEntry (267, 1, 20, 4, 37), dActionEntry (268, 1, 20, 4, 37), dActionEntry (269, 1, 20, 4, 37), dActionEntry (270, 1, 20, 4, 37), 
			dActionEntry (271, 1, 20, 4, 37), dActionEntry (272, 1, 20, 4, 37), dActionEntry (273, 1, 20, 4, 37), dActionEntry (274, 1, 20, 4, 37), 
			dActionEntry (275, 1, 20, 4, 37), dActionEntry (276, 1, 20, 4, 37), dActionEntry (277, 1, 20, 4, 37), dActionEntry (278, 1, 20, 4, 37), 
			dActionEntry (279, 1, 20, 4, 37), dActionEntry (280, 1, 20, 4, 37), dActionEntry (281, 1, 20, 4, 37), dActionEntry (282, 1, 20, 4, 37), 
			dActionEntry (283, 1, 20, 4, 37), dActionEntry (284, 1, 20, 4, 37), dActionEntry (285, 1, 20, 4, 37), dActionEntry (286, 1, 20, 4, 37), 
			dActionEntry (287, 1, 20, 4, 37), dActionEntry (288, 1, 20, 4, 37), dActionEntry (289, 1, 20, 4, 37), dActionEntry (290, 1, 20, 4, 37), 
			dActionEntry (291, 1, 20, 4, 37), dActionEntry (292, 1, 20, 4, 37), dActionEntry (293, 1, 20, 4, 37), dActionEntry (294, 1, 20, 4, 37), 
			dActionEntry (295, 1, 20, 4, 37), dActionEntry (296, 1, 20, 4, 37), dActionEntry (297, 1, 20, 4, 37), dActionEntry (298, 1, 20, 4, 37), 
			dActionEntry (299, 1, 20, 4, 37), dActionEntry (300, 1, 20, 4, 37), dActionEntry (260, 1, 20, 4, 39), dActionEntry (264, 1, 20, 4, 39), 
			dActionEntry (265, 1, 20, 4, 39), dActionEntry (266, 1, 20, 4, 39), dActionEntry (267, 1, 20, 4, 39), dActionEntry (268, 1, 20, 4, 39), 
			dActionEntry (269, 1, 20, 4, 39), dActionEntry (270, 1, 20, 4, 39), dActionEntry (271, 1, 20, 4, 39), dActionEntry (272, 1, 20, 4, 39), 
			dActionEntry (273, 1, 20, 4, 39), dActionEntry (274, 1, 20, 4, 39), dActionEntry (275, 1, 20, 4, 39), dActionEntry (276, 1, 20, 4, 39), 
			dActionEntry (277, 1, 20, 4, 39), dActionEntry (278, 1, 20, 4, 39), dActionEntry (279, 1, 20, 4, 39), dActionEntry (280, 1, 20, 4, 39), 
			dActionEntry (281, 1, 20, 4, 39), dActionEntry (282, 1, 20, 4, 39), dActionEntry (283, 1, 20, 4, 39), dActionEntry (284, 1, 20, 4, 39), 
			dActionEntry (285, 1, 20, 4, 39), dActionEntry (286, 1, 20, 4, 39), dActionEntry (287, 1, 20, 4, 39), dActionEntry (288, 1, 20, 4, 39), 
			dActionEntry (289, 1, 20, 4, 39), dActionEntry (290, 1, 20, 4, 39), dActionEntry (291, 1, 20, 4, 39), dActionEntry (292, 1, 20, 4, 39), 
			dActionEntry (293, 1, 20, 4, 39), dActionEntry (294, 1, 20, 4, 39), dActionEntry (295, 1, 20, 4, 39), dActionEntry (296, 1, 20, 4, 39), 
			dActionEntry (297, 1, 20, 4, 39), dActionEntry (298, 1, 20, 4, 39), dActionEntry (299, 1, 20, 4, 39), dActionEntry (300, 1, 20, 4, 39), 
			dActionEntry (44, 0, 146, 0, 0), dActionEntry (260, 1, 21, 4, 46), dActionEntry (264, 1, 21, 4, 46), dActionEntry (265, 1, 21, 4, 46), 
			dActionEntry (266, 1, 21, 4, 46), dActionEntry (267, 1, 21, 4, 46), dActionEntry (268, 1, 21, 4, 46), dActionEntry (269, 1, 21, 4, 46), 
			dActionEntry (270, 1, 21, 4, 46), dActionEntry (271, 1, 21, 4, 46), dActionEntry (272, 1, 21, 4, 46), dActionEntry (273, 1, 21, 4, 46), 
			dActionEntry (274, 1, 21, 4, 46), dActionEntry (275, 1, 21, 4, 46), dActionEntry (276, 1, 21, 4, 46), dActionEntry (277, 1, 21, 4, 46), 
			dActionEntry (278, 1, 21, 4, 46), dActionEntry (279, 1, 21, 4, 46), dActionEntry (280, 1, 21, 4, 46), dActionEntry (281, 1, 21, 4, 46), 
			dActionEntry (282, 1, 21, 4, 46), dActionEntry (283, 1, 21, 4, 46), dActionEntry (284, 1, 21, 4, 46), dActionEntry (285, 1, 21, 4, 46), 
			dActionEntry (286, 1, 21, 4, 46), dActionEntry (287, 1, 21, 4, 46), dActionEntry (288, 1, 21, 4, 46), dActionEntry (289, 1, 21, 4, 46), 
			dActionEntry (290, 1, 21, 4, 46), dActionEntry (291, 1, 21, 4, 46), dActionEntry (292, 1, 21, 4, 46), dActionEntry (293, 1, 21, 4, 46), 
			dActionEntry (294, 1, 21, 4, 46), dActionEntry (295, 1, 21, 4, 46), dActionEntry (296, 1, 21, 4, 46), dActionEntry (297, 1, 21, 4, 46), 
			dActionEntry (298, 1, 21, 4, 46), dActionEntry (299, 1, 21, 4, 46), dActionEntry (300, 1, 21, 4, 46), dActionEntry (260, 1, 12, 4, 28), 
			dActionEntry (264, 1, 12, 4, 28), dActionEntry (265, 1, 12, 4, 28), dActionEntry (266, 1, 12, 4, 28), dActionEntry (267, 1, 12, 4, 28), 
			dActionEntry (268, 1, 12, 4, 28), dActionEntry (269, 1, 12, 4, 28), dActionEntry (270, 1, 12, 4, 28), dActionEntry (271, 1, 12, 4, 28), 
			dActionEntry (272, 1, 12, 4, 28), dActionEntry (273, 1, 12, 4, 28), dActionEntry (274, 1, 12, 4, 28), dActionEntry (275, 1, 12, 4, 28), 
			dActionEntry (276, 1, 12, 4, 28), dActionEntry (277, 1, 12, 4, 28), dActionEntry (278, 1, 12, 4, 28), dActionEntry (279, 1, 12, 4, 28), 
			dActionEntry (280, 1, 12, 4, 28), dActionEntry (281, 1, 12, 4, 28), dActionEntry (282, 1, 12, 4, 28), dActionEntry (283, 1, 12, 4, 28), 
			dActionEntry (284, 1, 12, 4, 28), dActionEntry (285, 1, 12, 4, 28), dActionEntry (286, 1, 12, 4, 28), dActionEntry (287, 1, 12, 4, 28), 
			dActionEntry (288, 1, 12, 4, 28), dActionEntry (289, 1, 12, 4, 28), dActionEntry (290, 1, 12, 4, 28), dActionEntry (291, 1, 12, 4, 28), 
			dActionEntry (292, 1, 12, 4, 28), dActionEntry (293, 1, 12, 4, 28), dActionEntry (294, 1, 12, 4, 28), dActionEntry (295, 1, 12, 4, 28), 
			dActionEntry (296, 1, 12, 4, 28), dActionEntry (297, 1, 12, 4, 28), dActionEntry (298, 1, 12, 4, 28), dActionEntry (299, 1, 12, 4, 28), 
			dActionEntry (300, 1, 12, 4, 28), dActionEntry (260, 1, 21, 4, 52), dActionEntry (264, 1, 21, 4, 52), dActionEntry (265, 1, 21, 4, 52), 
			dActionEntry (266, 1, 21, 4, 52), dActionEntry (267, 1, 21, 4, 52), dActionEntry (268, 1, 21, 4, 52), dActionEntry (269, 1, 21, 4, 52), 
			dActionEntry (270, 1, 21, 4, 52), dActionEntry (271, 1, 21, 4, 52), dActionEntry (272, 1, 21, 4, 52), dActionEntry (273, 1, 21, 4, 52), 
			dActionEntry (274, 1, 21, 4, 52), dActionEntry (275, 1, 21, 4, 52), dActionEntry (276, 1, 21, 4, 52), dActionEntry (277, 1, 21, 4, 52), 
			dActionEntry (278, 1, 21, 4, 52), dActionEntry (279, 1, 21, 4, 52), dActionEntry (280, 1, 21, 4, 52), dActionEntry (281, 1, 21, 4, 52), 
			dActionEntry (282, 1, 21, 4, 52), dActionEntry (283, 1, 21, 4, 52), dActionEntry (284, 1, 21, 4, 52), dActionEntry (285, 1, 21, 4, 52), 
			dActionEntry (286, 1, 21, 4, 52), dActionEntry (287, 1, 21, 4, 52), dActionEntry (288, 1, 21, 4, 52), dActionEntry (289, 1, 21, 4, 52), 
			dActionEntry (290, 1, 21, 4, 52), dActionEntry (291, 1, 21, 4, 52), dActionEntry (292, 1, 21, 4, 52), dActionEntry (293, 1, 21, 4, 52), 
			dActionEntry (294, 1, 21, 4, 52), dActionEntry (295, 1, 21, 4, 52), dActionEntry (296, 1, 21, 4, 52), dActionEntry (297, 1, 21, 4, 52), 
			dActionEntry (298, 1, 21, 4, 52), dActionEntry (299, 1, 21, 4, 52), dActionEntry (300, 1, 21, 4, 52), dActionEntry (260, 1, 21, 4, 49), 
			dActionEntry (264, 1, 21, 4, 49), dActionEntry (265, 1, 21, 4, 49), dActionEntry (266, 1, 21, 4, 49), dActionEntry (267, 1, 21, 4, 49), 
			dActionEntry (268, 1, 21, 4, 49), dActionEntry (269, 1, 21, 4, 49), dActionEntry (270, 1, 21, 4, 49), dActionEntry (271, 1, 21, 4, 49), 
			dActionEntry (272, 1, 21, 4, 49), dActionEntry (273, 1, 21, 4, 49), dActionEntry (274, 1, 21, 4, 49), dActionEntry (275, 1, 21, 4, 49), 
			dActionEntry (276, 1, 21, 4, 49), dActionEntry (277, 1, 21, 4, 49), dActionEntry (278, 1, 21, 4, 49), dActionEntry (279, 1, 21, 4, 49), 
			dActionEntry (280, 1, 21, 4, 49), dActionEntry (281, 1, 21, 4, 49), dActionEntry (282, 1, 21, 4, 49), dActionEntry (283, 1, 21, 4, 49), 
			dActionEntry (284, 1, 21, 4, 49), dActionEntry (285, 1, 21, 4, 49), dActionEntry (286, 1, 21, 4, 49), dActionEntry (287, 1, 21, 4, 49), 
			dActionEntry (288, 1, 21, 4, 49), dActionEntry (289, 1, 21, 4, 49), dActionEntry (290, 1, 21, 4, 49), dActionEntry (291, 1, 21, 4, 49), 
			dActionEntry (292, 1, 21, 4, 49), dActionEntry (293, 1, 21, 4, 49), dActionEntry (294, 1, 21, 4, 49), dActionEntry (295, 1, 21, 4, 49), 
			dActionEntry (296, 1, 21, 4, 49), dActionEntry (297, 1, 21, 4, 49), dActionEntry (298, 1, 21, 4, 49), dActionEntry (299, 1, 21, 4, 49), 
			dActionEntry (300, 1, 21, 4, 49), dActionEntry (44, 0, 147, 0, 0), dActionEntry (260, 1, 18, 4, 33), dActionEntry (264, 1, 18, 4, 33), 
			dActionEntry (265, 1, 18, 4, 33), dActionEntry (266, 1, 18, 4, 33), dActionEntry (267, 1, 18, 4, 33), dActionEntry (268, 1, 18, 4, 33), 
			dActionEntry (269, 1, 18, 4, 33), dActionEntry (270, 1, 18, 4, 33), dActionEntry (271, 1, 18, 4, 33), dActionEntry (272, 1, 18, 4, 33), 
			dActionEntry (273, 1, 18, 4, 33), dActionEntry (274, 1, 18, 4, 33), dActionEntry (275, 1, 18, 4, 33), dActionEntry (276, 1, 18, 4, 33), 
			dActionEntry (277, 1, 18, 4, 33), dActionEntry (278, 1, 18, 4, 33), dActionEntry (279, 1, 18, 4, 33), dActionEntry (280, 1, 18, 4, 33), 
			dActionEntry (281, 1, 18, 4, 33), dActionEntry (282, 1, 18, 4, 33), dActionEntry (283, 1, 18, 4, 33), dActionEntry (284, 1, 18, 4, 33), 
			dActionEntry (285, 1, 18, 4, 33), dActionEntry (286, 1, 18, 4, 33), dActionEntry (287, 1, 18, 4, 33), dActionEntry (288, 1, 18, 4, 33), 
			dActionEntry (289, 1, 18, 4, 33), dActionEntry (290, 1, 18, 4, 33), dActionEntry (291, 1, 18, 4, 33), dActionEntry (292, 1, 18, 4, 33), 
			dActionEntry (293, 1, 18, 4, 33), dActionEntry (294, 1, 18, 4, 33), dActionEntry (295, 1, 18, 4, 33), dActionEntry (296, 1, 18, 4, 33), 
			dActionEntry (297, 1, 18, 4, 33), dActionEntry (298, 1, 18, 4, 33), dActionEntry (299, 1, 18, 4, 33), dActionEntry (300, 1, 18, 4, 33), 
			dActionEntry (44, 0, 148, 0, 0), dActionEntry (260, 1, 13, 4, 29), dActionEntry (264, 1, 13, 4, 29), dActionEntry (265, 1, 13, 4, 29), 
			dActionEntry (266, 1, 13, 4, 29), dActionEntry (267, 1, 13, 4, 29), dActionEntry (268, 1, 13, 4, 29), dActionEntry (269, 1, 13, 4, 29), 
			dActionEntry (270, 1, 13, 4, 29), dActionEntry (271, 1, 13, 4, 29), dActionEntry (272, 1, 13, 4, 29), dActionEntry (273, 1, 13, 4, 29), 
			dActionEntry (274, 1, 13, 4, 29), dActionEntry (275, 1, 13, 4, 29), dActionEntry (276, 1, 13, 4, 29), dActionEntry (277, 1, 13, 4, 29), 
			dActionEntry (278, 1, 13, 4, 29), dActionEntry (279, 1, 13, 4, 29), dActionEntry (280, 1, 13, 4, 29), dActionEntry (281, 1, 13, 4, 29), 
			dActionEntry (282, 1, 13, 4, 29), dActionEntry (283, 1, 13, 4, 29), dActionEntry (284, 1, 13, 4, 29), dActionEntry (285, 1, 13, 4, 29), 
			dActionEntry (286, 1, 13, 4, 29), dActionEntry (287, 1, 13, 4, 29), dActionEntry (288, 1, 13, 4, 29), dActionEntry (289, 1, 13, 4, 29), 
			dActionEntry (290, 1, 13, 4, 29), dActionEntry (291, 1, 13, 4, 29), dActionEntry (292, 1, 13, 4, 29), dActionEntry (293, 1, 13, 4, 29), 
			dActionEntry (294, 1, 13, 4, 29), dActionEntry (295, 1, 13, 4, 29), dActionEntry (296, 1, 13, 4, 29), dActionEntry (297, 1, 13, 4, 29), 
			dActionEntry (298, 1, 13, 4, 29), dActionEntry (299, 1, 13, 4, 29), dActionEntry (300, 1, 13, 4, 29), dActionEntry (260, 1, 16, 4, 32), 
			dActionEntry (264, 1, 16, 4, 32), dActionEntry (265, 1, 16, 4, 32), dActionEntry (266, 1, 16, 4, 32), dActionEntry (267, 1, 16, 4, 32), 
			dActionEntry (268, 1, 16, 4, 32), dActionEntry (269, 1, 16, 4, 32), dActionEntry (270, 1, 16, 4, 32), dActionEntry (271, 1, 16, 4, 32), 
			dActionEntry (272, 1, 16, 4, 32), dActionEntry (273, 1, 16, 4, 32), dActionEntry (274, 1, 16, 4, 32), dActionEntry (275, 1, 16, 4, 32), 
			dActionEntry (276, 1, 16, 4, 32), dActionEntry (277, 1, 16, 4, 32), dActionEntry (278, 1, 16, 4, 32), dActionEntry (279, 1, 16, 4, 32), 
			dActionEntry (280, 1, 16, 4, 32), dActionEntry (281, 1, 16, 4, 32), dActionEntry (282, 1, 16, 4, 32), dActionEntry (283, 1, 16, 4, 32), 
			dActionEntry (284, 1, 16, 4, 32), dActionEntry (285, 1, 16, 4, 32), dActionEntry (286, 1, 16, 4, 32), dActionEntry (287, 1, 16, 4, 32), 
			dActionEntry (288, 1, 16, 4, 32), dActionEntry (289, 1, 16, 4, 32), dActionEntry (290, 1, 16, 4, 32), dActionEntry (291, 1, 16, 4, 32), 
			dActionEntry (292, 1, 16, 4, 32), dActionEntry (293, 1, 16, 4, 32), dActionEntry (294, 1, 16, 4, 32), dActionEntry (295, 1, 16, 4, 32), 
			dActionEntry (296, 1, 16, 4, 32), dActionEntry (297, 1, 16, 4, 32), dActionEntry (298, 1, 16, 4, 32), dActionEntry (299, 1, 16, 4, 32), 
			dActionEntry (300, 1, 16, 4, 32), dActionEntry (260, 1, 15, 4, 31), dActionEntry (264, 1, 15, 4, 31), dActionEntry (265, 1, 15, 4, 31), 
			dActionEntry (266, 1, 15, 4, 31), dActionEntry (267, 1, 15, 4, 31), dActionEntry (268, 1, 15, 4, 31), dActionEntry (269, 1, 15, 4, 31), 
			dActionEntry (270, 1, 15, 4, 31), dActionEntry (271, 1, 15, 4, 31), dActionEntry (272, 1, 15, 4, 31), dActionEntry (273, 1, 15, 4, 31), 
			dActionEntry (274, 1, 15, 4, 31), dActionEntry (275, 1, 15, 4, 31), dActionEntry (276, 1, 15, 4, 31), dActionEntry (277, 1, 15, 4, 31), 
			dActionEntry (278, 1, 15, 4, 31), dActionEntry (279, 1, 15, 4, 31), dActionEntry (280, 1, 15, 4, 31), dActionEntry (281, 1, 15, 4, 31), 
			dActionEntry (282, 1, 15, 4, 31), dActionEntry (283, 1, 15, 4, 31), dActionEntry (284, 1, 15, 4, 31), dActionEntry (285, 1, 15, 4, 31), 
			dActionEntry (286, 1, 15, 4, 31), dActionEntry (287, 1, 15, 4, 31), dActionEntry (288, 1, 15, 4, 31), dActionEntry (289, 1, 15, 4, 31), 
			dActionEntry (290, 1, 15, 4, 31), dActionEntry (291, 1, 15, 4, 31), dActionEntry (292, 1, 15, 4, 31), dActionEntry (293, 1, 15, 4, 31), 
			dActionEntry (294, 1, 15, 4, 31), dActionEntry (295, 1, 15, 4, 31), dActionEntry (296, 1, 15, 4, 31), dActionEntry (297, 1, 15, 4, 31), 
			dActionEntry (298, 1, 15, 4, 31), dActionEntry (299, 1, 15, 4, 31), dActionEntry (300, 1, 15, 4, 31), dActionEntry (260, 1, 14, 4, 30), 
			dActionEntry (264, 1, 14, 4, 30), dActionEntry (265, 1, 14, 4, 30), dActionEntry (266, 1, 14, 4, 30), dActionEntry (267, 1, 14, 4, 30), 
			dActionEntry (268, 1, 14, 4, 30), dActionEntry (269, 1, 14, 4, 30), dActionEntry (270, 1, 14, 4, 30), dActionEntry (271, 1, 14, 4, 30), 
			dActionEntry (272, 1, 14, 4, 30), dActionEntry (273, 1, 14, 4, 30), dActionEntry (274, 1, 14, 4, 30), dActionEntry (275, 1, 14, 4, 30), 
			dActionEntry (276, 1, 14, 4, 30), dActionEntry (277, 1, 14, 4, 30), dActionEntry (278, 1, 14, 4, 30), dActionEntry (279, 1, 14, 4, 30), 
			dActionEntry (280, 1, 14, 4, 30), dActionEntry (281, 1, 14, 4, 30), dActionEntry (282, 1, 14, 4, 30), dActionEntry (283, 1, 14, 4, 30), 
			dActionEntry (284, 1, 14, 4, 30), dActionEntry (285, 1, 14, 4, 30), dActionEntry (286, 1, 14, 4, 30), dActionEntry (287, 1, 14, 4, 30), 
			dActionEntry (288, 1, 14, 4, 30), dActionEntry (289, 1, 14, 4, 30), dActionEntry (290, 1, 14, 4, 30), dActionEntry (291, 1, 14, 4, 30), 
			dActionEntry (292, 1, 14, 4, 30), dActionEntry (293, 1, 14, 4, 30), dActionEntry (294, 1, 14, 4, 30), dActionEntry (295, 1, 14, 4, 30), 
			dActionEntry (296, 1, 14, 4, 30), dActionEntry (297, 1, 14, 4, 30), dActionEntry (298, 1, 14, 4, 30), dActionEntry (299, 1, 14, 4, 30), 
			dActionEntry (300, 1, 14, 4, 30), dActionEntry (257, 0, 81, 0, 0), dActionEntry (262, 0, 92, 0, 0), dActionEntry (260, 1, 17, 6, 83), 
			dActionEntry (264, 1, 17, 6, 83), dActionEntry (265, 1, 17, 6, 83), dActionEntry (266, 1, 17, 6, 83), dActionEntry (267, 1, 17, 6, 83), 
			dActionEntry (268, 1, 17, 6, 83), dActionEntry (269, 1, 17, 6, 83), dActionEntry (270, 1, 17, 6, 83), dActionEntry (271, 1, 17, 6, 83), 
			dActionEntry (272, 1, 17, 6, 83), dActionEntry (273, 1, 17, 6, 83), dActionEntry (274, 1, 17, 6, 83), dActionEntry (275, 1, 17, 6, 83), 
			dActionEntry (276, 1, 17, 6, 83), dActionEntry (277, 1, 17, 6, 83), dActionEntry (278, 1, 17, 6, 83), dActionEntry (279, 1, 17, 6, 83), 
			dActionEntry (280, 1, 17, 6, 83), dActionEntry (281, 1, 17, 6, 83), dActionEntry (282, 1, 17, 6, 83), dActionEntry (283, 1, 17, 6, 83), 
			dActionEntry (284, 1, 17, 6, 83), dActionEntry (285, 1, 17, 6, 83), dActionEntry (286, 1, 17, 6, 83), dActionEntry (287, 1, 17, 6, 83), 
			dActionEntry (288, 1, 17, 6, 83), dActionEntry (289, 1, 17, 6, 83), dActionEntry (290, 1, 17, 6, 83), dActionEntry (291, 1, 17, 6, 83), 
			dActionEntry (292, 1, 17, 6, 83), dActionEntry (293, 1, 17, 6, 83), dActionEntry (294, 1, 17, 6, 83), dActionEntry (295, 1, 17, 6, 83), 
			dActionEntry (296, 1, 17, 6, 83), dActionEntry (297, 1, 17, 6, 83), dActionEntry (298, 1, 17, 6, 83), dActionEntry (299, 1, 17, 6, 83), 
			dActionEntry (300, 1, 17, 6, 83), dActionEntry (260, 1, 17, 6, 85), dActionEntry (264, 1, 17, 6, 85), dActionEntry (265, 1, 17, 6, 85), 
			dActionEntry (266, 1, 17, 6, 85), dActionEntry (267, 1, 17, 6, 85), dActionEntry (268, 1, 17, 6, 85), dActionEntry (269, 1, 17, 6, 85), 
			dActionEntry (270, 1, 17, 6, 85), dActionEntry (271, 1, 17, 6, 85), dActionEntry (272, 1, 17, 6, 85), dActionEntry (273, 1, 17, 6, 85), 
			dActionEntry (274, 1, 17, 6, 85), dActionEntry (275, 1, 17, 6, 85), dActionEntry (276, 1, 17, 6, 85), dActionEntry (277, 1, 17, 6, 85), 
			dActionEntry (278, 1, 17, 6, 85), dActionEntry (279, 1, 17, 6, 85), dActionEntry (280, 1, 17, 6, 85), dActionEntry (281, 1, 17, 6, 85), 
			dActionEntry (282, 1, 17, 6, 85), dActionEntry (283, 1, 17, 6, 85), dActionEntry (284, 1, 17, 6, 85), dActionEntry (285, 1, 17, 6, 85), 
			dActionEntry (286, 1, 17, 6, 85), dActionEntry (287, 1, 17, 6, 85), dActionEntry (288, 1, 17, 6, 85), dActionEntry (289, 1, 17, 6, 85), 
			dActionEntry (290, 1, 17, 6, 85), dActionEntry (291, 1, 17, 6, 85), dActionEntry (292, 1, 17, 6, 85), dActionEntry (293, 1, 17, 6, 85), 
			dActionEntry (294, 1, 17, 6, 85), dActionEntry (295, 1, 17, 6, 85), dActionEntry (296, 1, 17, 6, 85), dActionEntry (297, 1, 17, 6, 85), 
			dActionEntry (298, 1, 17, 6, 85), dActionEntry (299, 1, 17, 6, 85), dActionEntry (300, 1, 17, 6, 85), dActionEntry (260, 1, 21, 6, 45), 
			dActionEntry (264, 1, 21, 6, 45), dActionEntry (265, 1, 21, 6, 45), dActionEntry (266, 1, 21, 6, 45), dActionEntry (267, 1, 21, 6, 45), 
			dActionEntry (268, 1, 21, 6, 45), dActionEntry (269, 1, 21, 6, 45), dActionEntry (270, 1, 21, 6, 45), dActionEntry (271, 1, 21, 6, 45), 
			dActionEntry (272, 1, 21, 6, 45), dActionEntry (273, 1, 21, 6, 45), dActionEntry (274, 1, 21, 6, 45), dActionEntry (275, 1, 21, 6, 45), 
			dActionEntry (276, 1, 21, 6, 45), dActionEntry (277, 1, 21, 6, 45), dActionEntry (278, 1, 21, 6, 45), dActionEntry (279, 1, 21, 6, 45), 
			dActionEntry (280, 1, 21, 6, 45), dActionEntry (281, 1, 21, 6, 45), dActionEntry (282, 1, 21, 6, 45), dActionEntry (283, 1, 21, 6, 45), 
			dActionEntry (284, 1, 21, 6, 45), dActionEntry (285, 1, 21, 6, 45), dActionEntry (286, 1, 21, 6, 45), dActionEntry (287, 1, 21, 6, 45), 
			dActionEntry (288, 1, 21, 6, 45), dActionEntry (289, 1, 21, 6, 45), dActionEntry (290, 1, 21, 6, 45), dActionEntry (291, 1, 21, 6, 45), 
			dActionEntry (292, 1, 21, 6, 45), dActionEntry (293, 1, 21, 6, 45), dActionEntry (294, 1, 21, 6, 45), dActionEntry (295, 1, 21, 6, 45), 
			dActionEntry (296, 1, 21, 6, 45), dActionEntry (297, 1, 21, 6, 45), dActionEntry (298, 1, 21, 6, 45), dActionEntry (299, 1, 21, 6, 45), 
			dActionEntry (300, 1, 21, 6, 45), dActionEntry (260, 1, 20, 6, 42), dActionEntry (264, 1, 20, 6, 42), dActionEntry (265, 1, 20, 6, 42), 
			dActionEntry (266, 1, 20, 6, 42), dActionEntry (267, 1, 20, 6, 42), dActionEntry (268, 1, 20, 6, 42), dActionEntry (269, 1, 20, 6, 42), 
			dActionEntry (270, 1, 20, 6, 42), dActionEntry (271, 1, 20, 6, 42), dActionEntry (272, 1, 20, 6, 42), dActionEntry (273, 1, 20, 6, 42), 
			dActionEntry (274, 1, 20, 6, 42), dActionEntry (275, 1, 20, 6, 42), dActionEntry (276, 1, 20, 6, 42), dActionEntry (277, 1, 20, 6, 42), 
			dActionEntry (278, 1, 20, 6, 42), dActionEntry (279, 1, 20, 6, 42), dActionEntry (280, 1, 20, 6, 42), dActionEntry (281, 1, 20, 6, 42), 
			dActionEntry (282, 1, 20, 6, 42), dActionEntry (283, 1, 20, 6, 42), dActionEntry (284, 1, 20, 6, 42), dActionEntry (285, 1, 20, 6, 42), 
			dActionEntry (286, 1, 20, 6, 42), dActionEntry (287, 1, 20, 6, 42), dActionEntry (288, 1, 20, 6, 42), dActionEntry (289, 1, 20, 6, 42), 
			dActionEntry (290, 1, 20, 6, 42), dActionEntry (291, 1, 20, 6, 42), dActionEntry (292, 1, 20, 6, 42), dActionEntry (293, 1, 20, 6, 42), 
			dActionEntry (294, 1, 20, 6, 42), dActionEntry (295, 1, 20, 6, 42), dActionEntry (296, 1, 20, 6, 42), dActionEntry (297, 1, 20, 6, 42), 
			dActionEntry (298, 1, 20, 6, 42), dActionEntry (299, 1, 20, 6, 42), dActionEntry (300, 1, 20, 6, 42), dActionEntry (260, 1, 19, 6, 35), 
			dActionEntry (264, 1, 19, 6, 35), dActionEntry (265, 1, 19, 6, 35), dActionEntry (266, 1, 19, 6, 35), dActionEntry (267, 1, 19, 6, 35), 
			dActionEntry (268, 1, 19, 6, 35), dActionEntry (269, 1, 19, 6, 35), dActionEntry (270, 1, 19, 6, 35), dActionEntry (271, 1, 19, 6, 35), 
			dActionEntry (272, 1, 19, 6, 35), dActionEntry (273, 1, 19, 6, 35), dActionEntry (274, 1, 19, 6, 35), dActionEntry (275, 1, 19, 6, 35), 
			dActionEntry (276, 1, 19, 6, 35), dActionEntry (277, 1, 19, 6, 35), dActionEntry (278, 1, 19, 6, 35), dActionEntry (279, 1, 19, 6, 35), 
			dActionEntry (280, 1, 19, 6, 35), dActionEntry (281, 1, 19, 6, 35), dActionEntry (282, 1, 19, 6, 35), dActionEntry (283, 1, 19, 6, 35), 
			dActionEntry (284, 1, 19, 6, 35), dActionEntry (285, 1, 19, 6, 35), dActionEntry (286, 1, 19, 6, 35), dActionEntry (287, 1, 19, 6, 35), 
			dActionEntry (288, 1, 19, 6, 35), dActionEntry (289, 1, 19, 6, 35), dActionEntry (290, 1, 19, 6, 35), dActionEntry (291, 1, 19, 6, 35), 
			dActionEntry (292, 1, 19, 6, 35), dActionEntry (293, 1, 19, 6, 35), dActionEntry (294, 1, 19, 6, 35), dActionEntry (295, 1, 19, 6, 35), 
			dActionEntry (296, 1, 19, 6, 35), dActionEntry (297, 1, 19, 6, 35), dActionEntry (298, 1, 19, 6, 35), dActionEntry (299, 1, 19, 6, 35), 
			dActionEntry (300, 1, 19, 6, 35), dActionEntry (260, 1, 19, 6, 34), dActionEntry (264, 1, 19, 6, 34), dActionEntry (265, 1, 19, 6, 34), 
			dActionEntry (266, 1, 19, 6, 34), dActionEntry (267, 1, 19, 6, 34), dActionEntry (268, 1, 19, 6, 34), dActionEntry (269, 1, 19, 6, 34), 
			dActionEntry (270, 1, 19, 6, 34), dActionEntry (271, 1, 19, 6, 34), dActionEntry (272, 1, 19, 6, 34), dActionEntry (273, 1, 19, 6, 34), 
			dActionEntry (274, 1, 19, 6, 34), dActionEntry (275, 1, 19, 6, 34), dActionEntry (276, 1, 19, 6, 34), dActionEntry (277, 1, 19, 6, 34), 
			dActionEntry (278, 1, 19, 6, 34), dActionEntry (279, 1, 19, 6, 34), dActionEntry (280, 1, 19, 6, 34), dActionEntry (281, 1, 19, 6, 34), 
			dActionEntry (282, 1, 19, 6, 34), dActionEntry (283, 1, 19, 6, 34), dActionEntry (284, 1, 19, 6, 34), dActionEntry (285, 1, 19, 6, 34), 
			dActionEntry (286, 1, 19, 6, 34), dActionEntry (287, 1, 19, 6, 34), dActionEntry (288, 1, 19, 6, 34), dActionEntry (289, 1, 19, 6, 34), 
			dActionEntry (290, 1, 19, 6, 34), dActionEntry (291, 1, 19, 6, 34), dActionEntry (292, 1, 19, 6, 34), dActionEntry (293, 1, 19, 6, 34), 
			dActionEntry (294, 1, 19, 6, 34), dActionEntry (295, 1, 19, 6, 34), dActionEntry (296, 1, 19, 6, 34), dActionEntry (297, 1, 19, 6, 34), 
			dActionEntry (298, 1, 19, 6, 34), dActionEntry (299, 1, 19, 6, 34), dActionEntry (300, 1, 19, 6, 34)};

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
					case 81:// rule jumpLabel : JUMPLABEL 
						{ 
				dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
				switch (me->m_currentPass) 
				{
					case dAssemblerCompiler::firstPass:
						me->EmitJumpDestLabel (parameter[0].m_value);
						break;
				}
			}
						break;
					case 11:// rule dataDeclaration : dataType literal 
						{((dAssemblerCompiler*)this)->EmitUnInitilizedDataDeclaration (parameter[0].m_value, parameter[1].m_value);}
						break;
					case 79:// rule beginFunction : BEGIN literal visibility 
						{ 
						dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
						switch (me->m_currentPass) 
						{
							case dAssemblerCompiler::firstPass:
								me->RegisterFunction (parameter[1].m_value, parameter[2].m_value);
								break;
							case dAssemblerCompiler::secundPass:	
								me->BeginFunctionLinking (parameter[1].m_value);
								break;
						}
					}
						break;
					case 86:// rule register : REGISTER 
						{
				dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
				entry.m_value = me->TypeCheckRegister (parameter[0].m_value);
			}
						break;
					case 87:// rule constantExpression : INTEGER 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitIntegerConst (parameter[0].m_value);}
						break;
					case 12:// rule dataDeclaration : dataType literal = constantExpression 
						{((dAssemblerCompiler*)this)->EmitInitilizedDataDeclaration (parameter[0].m_value, parameter[1].m_value, parameter[3].m_value);}
						break;
					case 82:// rule constAdd : ADDI register , constantExpression 
						{ 
				dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
				if (me->m_currentPass == dAssemblerCompiler::firstPass) {
					dUserVariable register0 (REGISTER, "r0");
					me->EmitADDIConstantExpresion (parameter[1].m_value, register0, parameter[3].m_value);
				}
			}
						break;
					case 83:// rule constAdd : ADDI register , register , constantExpression 
						{ 
				dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
				if (me->m_currentPass == dAssemblerCompiler::firstPass) {
					me->EmitADDIConstantExpresion (parameter[1].m_value, parameter[3].m_value, parameter[5].m_value);
				}
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





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


bool dAssemblerParser::Parse(xxx& scanner)
{
	dList<dStackPair> stack;
	static short actionsCount[] = {4, 1, 1, 1, 4, 1, 4, 4, 1, 4, 4, 36, 37, 1, 5, 5, 4, 36, 36, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 37, 1, 37, 37, 1, 37, 37, 37, 1, 37, 1, 37, 37, 1, 37, 37, 1, 1, 4, 4, 4, 1, 1, 1, 1, 1, 1, 37, 37, 37, 1, 1, 1, 37, 1, 1, 1, 4, 37, 1, 1, 1, 1, 2, 3, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 37, 37, 1, 37, 37, 1, 37, 37, 37, 37, 1, 37, 1, 37, 37, 37, 37, 37, 2, 0, 0, 0, 37, 37, 37, 37, 37};
	static short actionsStart[] = {0, 4, 5, 6, 7, 11, 12, 16, 20, 21, 25, 29, 65, 102, 103, 108, 113, 117, 153, 189, 191, 193, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 198, 198, 206, 198, 198, 207, 208, 208, 209, 210, 198, 211, 212, 213, 214, 198, 198, 207, 215, 216, 217, 218, 219, 220, 198, 221, 222, 198, 259, 296, 198, 333, 370, 407, 198, 444, 198, 481, 518, 198, 555, 592, 198, 102, 629, 633, 637, 641, 642, 643, 644, 645, 646, 647, 684, 721, 758, 759, 760, 761, 798, 799, 800, 801, 805, 842, 843, 844, 845, 846, 848, 208, 207, 198, 207, 207, 208, 208, 198, 207, 198, 851, 207, 207, 851, 852, 889, 926, 927, 964, 1001, 1002, 1039, 1076, 1113, 1150, 1151, 1188, 1189, 1226, 1263, 1300, 1337, 1374, 208, 208, 208, 1376, 1413, 1450, 1487, 1524};
	static dActionEntry actionTable[] = {
					dActionEntry (255, 1, 0, 0, 2), dActionEntry (256, 0, 2, 0, 0), dActionEntry (259, 0, 1, 0, 0), dActionEntry (261, 0, 3, 0, 0), 
					dActionEntry (257, 0, 11, 0, 0), 
					dActionEntry (60, 0, 13, 0, 0), 
					dActionEntry (257, 1, 8, 1, 14), 
					dActionEntry (255, 1, 2, 1, 6), dActionEntry (256, 1, 2, 1, 6), dActionEntry (259, 1, 2, 1, 6), dActionEntry (261, 1, 2, 1, 6), 
					dActionEntry (257, 0, 14, 0, 0), 
					dActionEntry (255, 1, 2, 1, 7), dActionEntry (256, 1, 2, 1, 7), dActionEntry (259, 1, 2, 1, 7), dActionEntry (261, 1, 2, 1, 7), 
					dActionEntry (255, 1, 2, 1, 5), dActionEntry (256, 1, 2, 1, 5), dActionEntry (259, 1, 2, 1, 5), dActionEntry (261, 1, 2, 1, 5), 
					dActionEntry (255, 2, 0, 0, 0), 
					dActionEntry (255, 1, 1, 1, 3), dActionEntry (256, 1, 1, 1, 3), dActionEntry (259, 1, 1, 1, 3), dActionEntry (261, 1, 1, 1, 3), 
					dActionEntry (255, 1, 0, 1, 1), dActionEntry (256, 0, 2, 0, 0), dActionEntry (259, 0, 1, 0, 0), dActionEntry (261, 0, 3, 0, 0), 
					dActionEntry (264, 1, 7, 1, 15), dActionEntry (265, 1, 7, 1, 15), dActionEntry (266, 1, 7, 1, 15), dActionEntry (267, 1, 7, 1, 15), dActionEntry (268, 1, 7, 1, 15), dActionEntry (269, 1, 7, 1, 15), dActionEntry (270, 1, 7, 1, 15), dActionEntry (271, 1, 7, 1, 15), dActionEntry (272, 1, 7, 1, 15), dActionEntry (273, 1, 7, 1, 15), dActionEntry (274, 1, 7, 1, 15), dActionEntry (275, 1, 7, 1, 15), dActionEntry (276, 1, 7, 1, 15), dActionEntry (277, 1, 7, 1, 15), dActionEntry (278, 1, 7, 1, 15), dActionEntry (279, 1, 7, 1, 15), dActionEntry (280, 1, 7, 1, 15), dActionEntry (281, 1, 7, 1, 15), dActionEntry (282, 1, 7, 1, 15), dActionEntry (283, 1, 7, 1, 15), dActionEntry (284, 1, 7, 1, 15), dActionEntry (285, 1, 7, 1, 15), dActionEntry (286, 1, 7, 1, 15), dActionEntry (287, 1, 7, 1, 15), dActionEntry (288, 1, 7, 1, 15), dActionEntry (289, 1, 7, 1, 15), dActionEntry (290, 1, 7, 1, 15), dActionEntry (291, 1, 7, 1, 15), dActionEntry (292, 1, 7, 1, 15), dActionEntry (293, 1, 7, 1, 15), dActionEntry (294, 1, 7, 1, 15), dActionEntry (295, 1, 7, 1, 15), dActionEntry (296, 1, 7, 1, 15), dActionEntry (297, 1, 7, 1, 15), dActionEntry (298, 1, 7, 1, 15), dActionEntry (299, 1, 7, 1, 15), 
					dActionEntry (258, 0, 17, 0, 0), dActionEntry (264, 1, 10, 0, 18), dActionEntry (265, 1, 10, 0, 18), dActionEntry (266, 1, 10, 0, 18), dActionEntry (267, 1, 10, 0, 18), dActionEntry (268, 1, 10, 0, 18), dActionEntry (269, 1, 10, 0, 18), dActionEntry (270, 1, 10, 0, 18), dActionEntry (271, 1, 10, 0, 18), dActionEntry (272, 1, 10, 0, 18), dActionEntry (273, 1, 10, 0, 18), dActionEntry (274, 1, 10, 0, 18), dActionEntry (275, 1, 10, 0, 18), dActionEntry (276, 1, 10, 0, 18), dActionEntry (277, 1, 10, 0, 18), dActionEntry (278, 1, 10, 0, 18), dActionEntry (279, 1, 10, 0, 18), dActionEntry (280, 1, 10, 0, 18), dActionEntry (281, 1, 10, 0, 18), dActionEntry (282, 1, 10, 0, 18), dActionEntry (283, 1, 10, 0, 18), dActionEntry (284, 1, 10, 0, 18), dActionEntry (285, 1, 10, 0, 18), dActionEntry (286, 1, 10, 0, 18), dActionEntry (287, 1, 10, 0, 18), dActionEntry (288, 1, 10, 0, 18), dActionEntry (289, 1, 10, 0, 18), dActionEntry (290, 1, 10, 0, 18), dActionEntry (291, 1, 10, 0, 18), dActionEntry (292, 1, 10, 0, 18), dActionEntry (293, 1, 10, 0, 18), dActionEntry (294, 1, 10, 0, 18), dActionEntry (295, 1, 10, 0, 18), dActionEntry (296, 1, 10, 0, 18), dActionEntry (297, 1, 10, 0, 18), dActionEntry (298, 1, 10, 0, 18), dActionEntry (299, 1, 10, 0, 18), 
					dActionEntry (257, 0, 19, 0, 0), 
					dActionEntry (61, 1, 7, 1, 15), dActionEntry (255, 1, 7, 1, 15), dActionEntry (256, 1, 7, 1, 15), dActionEntry (259, 1, 7, 1, 15), dActionEntry (261, 1, 7, 1, 15), 
					dActionEntry (61, 0, 22, 0, 0), dActionEntry (255, 1, 4, 2, 11), dActionEntry (256, 1, 4, 2, 11), dActionEntry (259, 1, 4, 2, 11), dActionEntry (261, 1, 4, 2, 11), 
					dActionEntry (255, 1, 1, 2, 4), dActionEntry (256, 1, 1, 2, 4), dActionEntry (259, 1, 1, 2, 4), dActionEntry (261, 1, 1, 2, 4), 
					dActionEntry (264, 1, 10, 1, 17), dActionEntry (265, 1, 10, 1, 17), dActionEntry (266, 1, 10, 1, 17), dActionEntry (267, 1, 10, 1, 17), dActionEntry (268, 1, 10, 1, 17), dActionEntry (269, 1, 10, 1, 17), dActionEntry (270, 1, 10, 1, 17), dActionEntry (271, 1, 10, 1, 17), dActionEntry (272, 1, 10, 1, 17), dActionEntry (273, 1, 10, 1, 17), dActionEntry (274, 1, 10, 1, 17), dActionEntry (275, 1, 10, 1, 17), dActionEntry (276, 1, 10, 1, 17), dActionEntry (277, 1, 10, 1, 17), dActionEntry (278, 1, 10, 1, 17), dActionEntry (279, 1, 10, 1, 17), dActionEntry (280, 1, 10, 1, 17), dActionEntry (281, 1, 10, 1, 17), dActionEntry (282, 1, 10, 1, 17), dActionEntry (283, 1, 10, 1, 17), dActionEntry (284, 1, 10, 1, 17), dActionEntry (285, 1, 10, 1, 17), dActionEntry (286, 1, 10, 1, 17), dActionEntry (287, 1, 10, 1, 17), dActionEntry (288, 1, 10, 1, 17), dActionEntry (289, 1, 10, 1, 17), dActionEntry (290, 1, 10, 1, 17), dActionEntry (291, 1, 10, 1, 17), dActionEntry (292, 1, 10, 1, 17), dActionEntry (293, 1, 10, 1, 17), dActionEntry (294, 1, 10, 1, 17), dActionEntry (295, 1, 10, 1, 17), dActionEntry (296, 1, 10, 1, 17), dActionEntry (297, 1, 10, 1, 17), dActionEntry (298, 1, 10, 1, 17), dActionEntry (299, 1, 10, 1, 17), 
					dActionEntry (264, 0, 43, 0, 0), dActionEntry (265, 0, 40, 0, 0), dActionEntry (266, 0, 42, 0, 0), dActionEntry (267, 0, 41, 0, 0), dActionEntry (268, 0, 53, 0, 0), dActionEntry (269, 0, 55, 0, 0), dActionEntry (270, 0, 54, 0, 0), dActionEntry (271, 0, 51, 0, 0), dActionEntry (272, 0, 52, 0, 0), dActionEntry (273, 0, 26, 0, 0), dActionEntry (274, 0, 47, 0, 0), dActionEntry (275, 0, 58, 0, 0), dActionEntry (276, 0, 46, 0, 0), dActionEntry (277, 0, 25, 0, 0), dActionEntry (278, 0, 24, 0, 0), dActionEntry (279, 0, 56, 0, 0), dActionEntry (280, 0, 44, 0, 0), dActionEntry (281, 0, 35, 0, 0), dActionEntry (282, 0, 23, 0, 0), dActionEntry (283, 0, 45, 0, 0), dActionEntry (284, 0, 27, 0, 0), dActionEntry (285, 0, 32, 0, 0), dActionEntry (286, 0, 31, 0, 0), dActionEntry (287, 0, 30, 0, 0), dActionEntry (288, 0, 29, 0, 0), dActionEntry (289, 0, 28, 0, 0), dActionEntry (290, 0, 33, 0, 0), dActionEntry (291, 0, 34, 0, 0), dActionEntry (292, 0, 50, 0, 0), dActionEntry (293, 0, 57, 0, 0), dActionEntry (294, 0, 38, 0, 0), dActionEntry (295, 0, 39, 0, 0), dActionEntry (296, 0, 36, 0, 0), dActionEntry (297, 0, 37, 0, 0), dActionEntry (298, 0, 49, 0, 0), dActionEntry (299, 0, 48, 0, 0), 
					dActionEntry (46, 1, 7, 1, 15), dActionEntry (62, 1, 7, 1, 15), 
					dActionEntry (46, 0, 76, 0, 0), dActionEntry (62, 0, 77, 0, 0), 
					dActionEntry (46, 1, 6, 1, 9), dActionEntry (62, 1, 6, 1, 9), 
					dActionEntry (262, 0, 78, 0, 0), 
					dActionEntry (263, 1, 27, 1, 68), 
					dActionEntry (263, 1, 27, 1, 64), 
					dActionEntry (263, 0, 80, 0, 0), 
					dActionEntry (263, 1, 26, 1, 60), 
					dActionEntry (263, 1, 28, 1, 70), 
					dActionEntry (263, 1, 28, 1, 75), 
					dActionEntry (263, 1, 28, 1, 74), 
					dActionEntry (263, 1, 28, 1, 73), 
					dActionEntry (263, 1, 28, 1, 72), 
					dActionEntry (263, 1, 28, 1, 71), 
					dActionEntry (263, 1, 27, 1, 67), 
					dActionEntry (263, 0, 86, 0, 0), 
					
					dActionEntry (263, 1, 23, 1, 52), 
					dActionEntry (263, 1, 23, 1, 54), 
					dActionEntry (263, 1, 23, 1, 53), 
					dActionEntry (263, 1, 27, 1, 66), 
					dActionEntry (263, 1, 27, 1, 69), 
					dActionEntry (263, 1, 26, 1, 63), 
					dActionEntry (263, 1, 26, 1, 61), 
					dActionEntry (263, 1, 25, 1, 58), 
					dActionEntry (263, 1, 25, 1, 59), 
					dActionEntry (263, 1, 24, 1, 55), 
					dActionEntry (263, 1, 24, 1, 57), 
					dActionEntry (263, 1, 24, 1, 56), 
					dActionEntry (263, 1, 27, 1, 65), 
					dActionEntry (263, 1, 26, 1, 62), 
					dActionEntry (260, 1, 13, 1, 27), dActionEntry (264, 1, 13, 1, 27), dActionEntry (265, 1, 13, 1, 27), dActionEntry (266, 1, 13, 1, 27), dActionEntry (267, 1, 13, 1, 27), dActionEntry (268, 1, 13, 1, 27), dActionEntry (269, 1, 13, 1, 27), dActionEntry (270, 1, 13, 1, 27), dActionEntry (271, 1, 13, 1, 27), dActionEntry (272, 1, 13, 1, 27), dActionEntry (273, 1, 13, 1, 27), dActionEntry (274, 1, 13, 1, 27), dActionEntry (275, 1, 13, 1, 27), dActionEntry (276, 1, 13, 1, 27), dActionEntry (277, 1, 13, 1, 27), dActionEntry (278, 1, 13, 1, 27), dActionEntry (279, 1, 13, 1, 27), dActionEntry (280, 1, 13, 1, 27), dActionEntry (281, 1, 13, 1, 27), dActionEntry (282, 1, 13, 1, 27), dActionEntry (283, 1, 13, 1, 27), dActionEntry (284, 1, 13, 1, 27), dActionEntry (285, 1, 13, 1, 27), dActionEntry (286, 1, 13, 1, 27), dActionEntry (287, 1, 13, 1, 27), dActionEntry (288, 1, 13, 1, 27), dActionEntry (289, 1, 13, 1, 27), dActionEntry (290, 1, 13, 1, 27), dActionEntry (291, 1, 13, 1, 27), dActionEntry (292, 1, 13, 1, 27), dActionEntry (293, 1, 13, 1, 27), dActionEntry (294, 1, 13, 1, 27), dActionEntry (295, 1, 13, 1, 27), dActionEntry (296, 1, 13, 1, 27), dActionEntry (297, 1, 13, 1, 27), dActionEntry (298, 1, 13, 1, 27), dActionEntry (299, 1, 13, 1, 27), 
					dActionEntry (260, 1, 13, 1, 29), dActionEntry (264, 1, 13, 1, 29), dActionEntry (265, 1, 13, 1, 29), dActionEntry (266, 1, 13, 1, 29), dActionEntry (267, 1, 13, 1, 29), dActionEntry (268, 1, 13, 1, 29), dActionEntry (269, 1, 13, 1, 29), dActionEntry (270, 1, 13, 1, 29), dActionEntry (271, 1, 13, 1, 29), dActionEntry (272, 1, 13, 1, 29), dActionEntry (273, 1, 13, 1, 29), dActionEntry (274, 1, 13, 1, 29), dActionEntry (275, 1, 13, 1, 29), dActionEntry (276, 1, 13, 1, 29), dActionEntry (277, 1, 13, 1, 29), dActionEntry (278, 1, 13, 1, 29), dActionEntry (279, 1, 13, 1, 29), dActionEntry (280, 1, 13, 1, 29), dActionEntry (281, 1, 13, 1, 29), dActionEntry (282, 1, 13, 1, 29), dActionEntry (283, 1, 13, 1, 29), dActionEntry (284, 1, 13, 1, 29), dActionEntry (285, 1, 13, 1, 29), dActionEntry (286, 1, 13, 1, 29), dActionEntry (287, 1, 13, 1, 29), dActionEntry (288, 1, 13, 1, 29), dActionEntry (289, 1, 13, 1, 29), dActionEntry (290, 1, 13, 1, 29), dActionEntry (291, 1, 13, 1, 29), dActionEntry (292, 1, 13, 1, 29), dActionEntry (293, 1, 13, 1, 29), dActionEntry (294, 1, 13, 1, 29), dActionEntry (295, 1, 13, 1, 29), dActionEntry (296, 1, 13, 1, 29), dActionEntry (297, 1, 13, 1, 29), dActionEntry (298, 1, 13, 1, 29), dActionEntry (299, 1, 13, 1, 29), 
					dActionEntry (260, 1, 13, 1, 28), dActionEntry (264, 1, 13, 1, 28), dActionEntry (265, 1, 13, 1, 28), dActionEntry (266, 1, 13, 1, 28), dActionEntry (267, 1, 13, 1, 28), dActionEntry (268, 1, 13, 1, 28), dActionEntry (269, 1, 13, 1, 28), dActionEntry (270, 1, 13, 1, 28), dActionEntry (271, 1, 13, 1, 28), dActionEntry (272, 1, 13, 1, 28), dActionEntry (273, 1, 13, 1, 28), dActionEntry (274, 1, 13, 1, 28), dActionEntry (275, 1, 13, 1, 28), dActionEntry (276, 1, 13, 1, 28), dActionEntry (277, 1, 13, 1, 28), dActionEntry (278, 1, 13, 1, 28), dActionEntry (279, 1, 13, 1, 28), dActionEntry (280, 1, 13, 1, 28), dActionEntry (281, 1, 13, 1, 28), dActionEntry (282, 1, 13, 1, 28), dActionEntry (283, 1, 13, 1, 28), dActionEntry (284, 1, 13, 1, 28), dActionEntry (285, 1, 13, 1, 28), dActionEntry (286, 1, 13, 1, 28), dActionEntry (287, 1, 13, 1, 28), dActionEntry (288, 1, 13, 1, 28), dActionEntry (289, 1, 13, 1, 28), dActionEntry (290, 1, 13, 1, 28), dActionEntry (291, 1, 13, 1, 28), dActionEntry (292, 1, 13, 1, 28), dActionEntry (293, 1, 13, 1, 28), dActionEntry (294, 1, 13, 1, 28), dActionEntry (295, 1, 13, 1, 28), dActionEntry (296, 1, 13, 1, 28), dActionEntry (297, 1, 13, 1, 28), dActionEntry (298, 1, 13, 1, 28), dActionEntry (299, 1, 13, 1, 28), 
					dActionEntry (260, 1, 11, 1, 20), dActionEntry (264, 1, 11, 1, 20), dActionEntry (265, 1, 11, 1, 20), dActionEntry (266, 1, 11, 1, 20), dActionEntry (267, 1, 11, 1, 20), dActionEntry (268, 1, 11, 1, 20), dActionEntry (269, 1, 11, 1, 20), dActionEntry (270, 1, 11, 1, 20), dActionEntry (271, 1, 11, 1, 20), dActionEntry (272, 1, 11, 1, 20), dActionEntry (273, 1, 11, 1, 20), dActionEntry (274, 1, 11, 1, 20), dActionEntry (275, 1, 11, 1, 20), dActionEntry (276, 1, 11, 1, 20), dActionEntry (277, 1, 11, 1, 20), dActionEntry (278, 1, 11, 1, 20), dActionEntry (279, 1, 11, 1, 20), dActionEntry (280, 1, 11, 1, 20), dActionEntry (281, 1, 11, 1, 20), dActionEntry (282, 1, 11, 1, 20), dActionEntry (283, 1, 11, 1, 20), dActionEntry (284, 1, 11, 1, 20), dActionEntry (285, 1, 11, 1, 20), dActionEntry (286, 1, 11, 1, 20), dActionEntry (287, 1, 11, 1, 20), dActionEntry (288, 1, 11, 1, 20), dActionEntry (289, 1, 11, 1, 20), dActionEntry (290, 1, 11, 1, 20), dActionEntry (291, 1, 11, 1, 20), dActionEntry (292, 1, 11, 1, 20), dActionEntry (293, 1, 11, 1, 20), dActionEntry (294, 1, 11, 1, 20), dActionEntry (295, 1, 11, 1, 20), dActionEntry (296, 1, 11, 1, 20), dActionEntry (297, 1, 11, 1, 20), dActionEntry (298, 1, 11, 1, 20), dActionEntry (299, 1, 11, 1, 20), 
					dActionEntry (260, 0, 96, 0, 0), dActionEntry (264, 0, 43, 0, 0), dActionEntry (265, 0, 40, 0, 0), dActionEntry (266, 0, 42, 0, 0), dActionEntry (267, 0, 41, 0, 0), dActionEntry (268, 0, 53, 0, 0), dActionEntry (269, 0, 55, 0, 0), dActionEntry (270, 0, 54, 0, 0), dActionEntry (271, 0, 51, 0, 0), dActionEntry (272, 0, 52, 0, 0), dActionEntry (273, 0, 26, 0, 0), dActionEntry (274, 0, 47, 0, 0), dActionEntry (275, 0, 58, 0, 0), dActionEntry (276, 0, 46, 0, 0), dActionEntry (277, 0, 25, 0, 0), dActionEntry (278, 0, 24, 0, 0), dActionEntry (279, 0, 56, 0, 0), dActionEntry (280, 0, 44, 0, 0), dActionEntry (281, 0, 35, 0, 0), dActionEntry (282, 0, 23, 0, 0), dActionEntry (283, 0, 45, 0, 0), dActionEntry (284, 0, 27, 0, 0), dActionEntry (285, 0, 32, 0, 0), dActionEntry (286, 0, 31, 0, 0), dActionEntry (287, 0, 30, 0, 0), dActionEntry (288, 0, 29, 0, 0), dActionEntry (289, 0, 28, 0, 0), dActionEntry (290, 0, 33, 0, 0), dActionEntry (291, 0, 34, 0, 0), dActionEntry (292, 0, 50, 0, 0), dActionEntry (293, 0, 57, 0, 0), dActionEntry (294, 0, 38, 0, 0), dActionEntry (295, 0, 39, 0, 0), dActionEntry (296, 0, 36, 0, 0), dActionEntry (297, 0, 37, 0, 0), dActionEntry (298, 0, 49, 0, 0), dActionEntry (299, 0, 48, 0, 0), 
					dActionEntry (260, 1, 13, 1, 23), dActionEntry (264, 1, 13, 1, 23), dActionEntry (265, 1, 13, 1, 23), dActionEntry (266, 1, 13, 1, 23), dActionEntry (267, 1, 13, 1, 23), dActionEntry (268, 1, 13, 1, 23), dActionEntry (269, 1, 13, 1, 23), dActionEntry (270, 1, 13, 1, 23), dActionEntry (271, 1, 13, 1, 23), dActionEntry (272, 1, 13, 1, 23), dActionEntry (273, 1, 13, 1, 23), dActionEntry (274, 1, 13, 1, 23), dActionEntry (275, 1, 13, 1, 23), dActionEntry (276, 1, 13, 1, 23), dActionEntry (277, 1, 13, 1, 23), dActionEntry (278, 1, 13, 1, 23), dActionEntry (279, 1, 13, 1, 23), dActionEntry (280, 1, 13, 1, 23), dActionEntry (281, 1, 13, 1, 23), dActionEntry (282, 1, 13, 1, 23), dActionEntry (283, 1, 13, 1, 23), dActionEntry (284, 1, 13, 1, 23), dActionEntry (285, 1, 13, 1, 23), dActionEntry (286, 1, 13, 1, 23), dActionEntry (287, 1, 13, 1, 23), dActionEntry (288, 1, 13, 1, 23), dActionEntry (289, 1, 13, 1, 23), dActionEntry (290, 1, 13, 1, 23), dActionEntry (291, 1, 13, 1, 23), dActionEntry (292, 1, 13, 1, 23), dActionEntry (293, 1, 13, 1, 23), dActionEntry (294, 1, 13, 1, 23), dActionEntry (295, 1, 13, 1, 23), dActionEntry (296, 1, 13, 1, 23), dActionEntry (297, 1, 13, 1, 23), dActionEntry (298, 1, 13, 1, 23), dActionEntry (299, 1, 13, 1, 23), 
					dActionEntry (260, 1, 13, 1, 26), dActionEntry (264, 1, 13, 1, 26), dActionEntry (265, 1, 13, 1, 26), dActionEntry (266, 1, 13, 1, 26), dActionEntry (267, 1, 13, 1, 26), dActionEntry (268, 1, 13, 1, 26), dActionEntry (269, 1, 13, 1, 26), dActionEntry (270, 1, 13, 1, 26), dActionEntry (271, 1, 13, 1, 26), dActionEntry (272, 1, 13, 1, 26), dActionEntry (273, 1, 13, 1, 26), dActionEntry (274, 1, 13, 1, 26), dActionEntry (275, 1, 13, 1, 26), dActionEntry (276, 1, 13, 1, 26), dActionEntry (277, 1, 13, 1, 26), dActionEntry (278, 1, 13, 1, 26), dActionEntry (279, 1, 13, 1, 26), dActionEntry (280, 1, 13, 1, 26), dActionEntry (281, 1, 13, 1, 26), dActionEntry (282, 1, 13, 1, 26), dActionEntry (283, 1, 13, 1, 26), dActionEntry (284, 1, 13, 1, 26), dActionEntry (285, 1, 13, 1, 26), dActionEntry (286, 1, 13, 1, 26), dActionEntry (287, 1, 13, 1, 26), dActionEntry (288, 1, 13, 1, 26), dActionEntry (289, 1, 13, 1, 26), dActionEntry (290, 1, 13, 1, 26), dActionEntry (291, 1, 13, 1, 26), dActionEntry (292, 1, 13, 1, 26), dActionEntry (293, 1, 13, 1, 26), dActionEntry (294, 1, 13, 1, 26), dActionEntry (295, 1, 13, 1, 26), dActionEntry (296, 1, 13, 1, 26), dActionEntry (297, 1, 13, 1, 26), dActionEntry (298, 1, 13, 1, 26), dActionEntry (299, 1, 13, 1, 26), 
					dActionEntry (260, 1, 13, 1, 22), dActionEntry (264, 1, 13, 1, 22), dActionEntry (265, 1, 13, 1, 22), dActionEntry (266, 1, 13, 1, 22), dActionEntry (267, 1, 13, 1, 22), dActionEntry (268, 1, 13, 1, 22), dActionEntry (269, 1, 13, 1, 22), dActionEntry (270, 1, 13, 1, 22), dActionEntry (271, 1, 13, 1, 22), dActionEntry (272, 1, 13, 1, 22), dActionEntry (273, 1, 13, 1, 22), dActionEntry (274, 1, 13, 1, 22), dActionEntry (275, 1, 13, 1, 22), dActionEntry (276, 1, 13, 1, 22), dActionEntry (277, 1, 13, 1, 22), dActionEntry (278, 1, 13, 1, 22), dActionEntry (279, 1, 13, 1, 22), dActionEntry (280, 1, 13, 1, 22), dActionEntry (281, 1, 13, 1, 22), dActionEntry (282, 1, 13, 1, 22), dActionEntry (283, 1, 13, 1, 22), dActionEntry (284, 1, 13, 1, 22), dActionEntry (285, 1, 13, 1, 22), dActionEntry (286, 1, 13, 1, 22), dActionEntry (287, 1, 13, 1, 22), dActionEntry (288, 1, 13, 1, 22), dActionEntry (289, 1, 13, 1, 22), dActionEntry (290, 1, 13, 1, 22), dActionEntry (291, 1, 13, 1, 22), dActionEntry (292, 1, 13, 1, 22), dActionEntry (293, 1, 13, 1, 22), dActionEntry (294, 1, 13, 1, 22), dActionEntry (295, 1, 13, 1, 22), dActionEntry (296, 1, 13, 1, 22), dActionEntry (297, 1, 13, 1, 22), dActionEntry (298, 1, 13, 1, 22), dActionEntry (299, 1, 13, 1, 22), 
					dActionEntry (260, 1, 13, 1, 25), dActionEntry (264, 1, 13, 1, 25), dActionEntry (265, 1, 13, 1, 25), dActionEntry (266, 1, 13, 1, 25), dActionEntry (267, 1, 13, 1, 25), dActionEntry (268, 1, 13, 1, 25), dActionEntry (269, 1, 13, 1, 25), dActionEntry (270, 1, 13, 1, 25), dActionEntry (271, 1, 13, 1, 25), dActionEntry (272, 1, 13, 1, 25), dActionEntry (273, 1, 13, 1, 25), dActionEntry (274, 1, 13, 1, 25), dActionEntry (275, 1, 13, 1, 25), dActionEntry (276, 1, 13, 1, 25), dActionEntry (277, 1, 13, 1, 25), dActionEntry (278, 1, 13, 1, 25), dActionEntry (279, 1, 13, 1, 25), dActionEntry (280, 1, 13, 1, 25), dActionEntry (281, 1, 13, 1, 25), dActionEntry (282, 1, 13, 1, 25), dActionEntry (283, 1, 13, 1, 25), dActionEntry (284, 1, 13, 1, 25), dActionEntry (285, 1, 13, 1, 25), dActionEntry (286, 1, 13, 1, 25), dActionEntry (287, 1, 13, 1, 25), dActionEntry (288, 1, 13, 1, 25), dActionEntry (289, 1, 13, 1, 25), dActionEntry (290, 1, 13, 1, 25), dActionEntry (291, 1, 13, 1, 25), dActionEntry (292, 1, 13, 1, 25), dActionEntry (293, 1, 13, 1, 25), dActionEntry (294, 1, 13, 1, 25), dActionEntry (295, 1, 13, 1, 25), dActionEntry (296, 1, 13, 1, 25), dActionEntry (297, 1, 13, 1, 25), dActionEntry (298, 1, 13, 1, 25), dActionEntry (299, 1, 13, 1, 25), 
					dActionEntry (260, 1, 13, 1, 30), dActionEntry (264, 1, 13, 1, 30), dActionEntry (265, 1, 13, 1, 30), dActionEntry (266, 1, 13, 1, 30), dActionEntry (267, 1, 13, 1, 30), dActionEntry (268, 1, 13, 1, 30), dActionEntry (269, 1, 13, 1, 30), dActionEntry (270, 1, 13, 1, 30), dActionEntry (271, 1, 13, 1, 30), dActionEntry (272, 1, 13, 1, 30), dActionEntry (273, 1, 13, 1, 30), dActionEntry (274, 1, 13, 1, 30), dActionEntry (275, 1, 13, 1, 30), dActionEntry (276, 1, 13, 1, 30), dActionEntry (277, 1, 13, 1, 30), dActionEntry (278, 1, 13, 1, 30), dActionEntry (279, 1, 13, 1, 30), dActionEntry (280, 1, 13, 1, 30), dActionEntry (281, 1, 13, 1, 30), dActionEntry (282, 1, 13, 1, 30), dActionEntry (283, 1, 13, 1, 30), dActionEntry (284, 1, 13, 1, 30), dActionEntry (285, 1, 13, 1, 30), dActionEntry (286, 1, 13, 1, 30), dActionEntry (287, 1, 13, 1, 30), dActionEntry (288, 1, 13, 1, 30), dActionEntry (289, 1, 13, 1, 30), dActionEntry (290, 1, 13, 1, 30), dActionEntry (291, 1, 13, 1, 30), dActionEntry (292, 1, 13, 1, 30), dActionEntry (293, 1, 13, 1, 30), dActionEntry (294, 1, 13, 1, 30), dActionEntry (295, 1, 13, 1, 30), dActionEntry (296, 1, 13, 1, 30), dActionEntry (297, 1, 13, 1, 30), dActionEntry (298, 1, 13, 1, 30), dActionEntry (299, 1, 13, 1, 30), 
					dActionEntry (260, 1, 13, 1, 24), dActionEntry (264, 1, 13, 1, 24), dActionEntry (265, 1, 13, 1, 24), dActionEntry (266, 1, 13, 1, 24), dActionEntry (267, 1, 13, 1, 24), dActionEntry (268, 1, 13, 1, 24), dActionEntry (269, 1, 13, 1, 24), dActionEntry (270, 1, 13, 1, 24), dActionEntry (271, 1, 13, 1, 24), dActionEntry (272, 1, 13, 1, 24), dActionEntry (273, 1, 13, 1, 24), dActionEntry (274, 1, 13, 1, 24), dActionEntry (275, 1, 13, 1, 24), dActionEntry (276, 1, 13, 1, 24), dActionEntry (277, 1, 13, 1, 24), dActionEntry (278, 1, 13, 1, 24), dActionEntry (279, 1, 13, 1, 24), dActionEntry (280, 1, 13, 1, 24), dActionEntry (281, 1, 13, 1, 24), dActionEntry (282, 1, 13, 1, 24), dActionEntry (283, 1, 13, 1, 24), dActionEntry (284, 1, 13, 1, 24), dActionEntry (285, 1, 13, 1, 24), dActionEntry (286, 1, 13, 1, 24), dActionEntry (287, 1, 13, 1, 24), dActionEntry (288, 1, 13, 1, 24), dActionEntry (289, 1, 13, 1, 24), dActionEntry (290, 1, 13, 1, 24), dActionEntry (291, 1, 13, 1, 24), dActionEntry (292, 1, 13, 1, 24), dActionEntry (293, 1, 13, 1, 24), dActionEntry (294, 1, 13, 1, 24), dActionEntry (295, 1, 13, 1, 24), dActionEntry (296, 1, 13, 1, 24), dActionEntry (297, 1, 13, 1, 24), dActionEntry (298, 1, 13, 1, 24), dActionEntry (299, 1, 13, 1, 24), 
					dActionEntry (255, 1, 3, 4, 8), dActionEntry (256, 1, 3, 4, 8), dActionEntry (259, 1, 3, 4, 8), dActionEntry (261, 1, 3, 4, 8), 
					dActionEntry (255, 1, 9, 1, 13), dActionEntry (256, 1, 9, 1, 13), dActionEntry (259, 1, 9, 1, 13), dActionEntry (261, 1, 9, 1, 13), 
					dActionEntry (255, 1, 4, 4, 12), dActionEntry (256, 1, 4, 4, 12), dActionEntry (259, 1, 4, 4, 12), dActionEntry (261, 1, 4, 4, 12), 
					dActionEntry (44, 1, 12, 1, 19), 
					dActionEntry (44, 0, 103, 0, 0), 
					dActionEntry (44, 0, 104, 0, 0), 
					dActionEntry (44, 0, 105, 0, 0), 
					dActionEntry (44, 0, 106, 0, 0), 
					dActionEntry (44, 0, 107, 0, 0), 
					dActionEntry (260, 1, 12, 1, 19), dActionEntry (264, 1, 12, 1, 19), dActionEntry (265, 1, 12, 1, 19), dActionEntry (266, 1, 12, 1, 19), dActionEntry (267, 1, 12, 1, 19), dActionEntry (268, 1, 12, 1, 19), dActionEntry (269, 1, 12, 1, 19), dActionEntry (270, 1, 12, 1, 19), dActionEntry (271, 1, 12, 1, 19), dActionEntry (272, 1, 12, 1, 19), dActionEntry (273, 1, 12, 1, 19), dActionEntry (274, 1, 12, 1, 19), dActionEntry (275, 1, 12, 1, 19), dActionEntry (276, 1, 12, 1, 19), dActionEntry (277, 1, 12, 1, 19), dActionEntry (278, 1, 12, 1, 19), dActionEntry (279, 1, 12, 1, 19), dActionEntry (280, 1, 12, 1, 19), dActionEntry (281, 1, 12, 1, 19), dActionEntry (282, 1, 12, 1, 19), dActionEntry (283, 1, 12, 1, 19), dActionEntry (284, 1, 12, 1, 19), dActionEntry (285, 1, 12, 1, 19), dActionEntry (286, 1, 12, 1, 19), dActionEntry (287, 1, 12, 1, 19), dActionEntry (288, 1, 12, 1, 19), dActionEntry (289, 1, 12, 1, 19), dActionEntry (290, 1, 12, 1, 19), dActionEntry (291, 1, 12, 1, 19), dActionEntry (292, 1, 12, 1, 19), dActionEntry (293, 1, 12, 1, 19), dActionEntry (294, 1, 12, 1, 19), dActionEntry (295, 1, 12, 1, 19), dActionEntry (296, 1, 12, 1, 19), dActionEntry (297, 1, 12, 1, 19), dActionEntry (298, 1, 12, 1, 19), dActionEntry (299, 1, 12, 1, 19), 
					dActionEntry (260, 1, 21, 2, 46), dActionEntry (264, 1, 21, 2, 46), dActionEntry (265, 1, 21, 2, 46), dActionEntry (266, 1, 21, 2, 46), dActionEntry (267, 1, 21, 2, 46), dActionEntry (268, 1, 21, 2, 46), dActionEntry (269, 1, 21, 2, 46), dActionEntry (270, 1, 21, 2, 46), dActionEntry (271, 1, 21, 2, 46), dActionEntry (272, 1, 21, 2, 46), dActionEntry (273, 1, 21, 2, 46), dActionEntry (274, 1, 21, 2, 46), dActionEntry (275, 1, 21, 2, 46), dActionEntry (276, 1, 21, 2, 46), dActionEntry (277, 1, 21, 2, 46), dActionEntry (278, 1, 21, 2, 46), dActionEntry (279, 1, 21, 2, 46), dActionEntry (280, 1, 21, 2, 46), dActionEntry (281, 1, 21, 2, 46), dActionEntry (282, 1, 21, 2, 46), dActionEntry (283, 1, 21, 2, 46), dActionEntry (284, 1, 21, 2, 46), dActionEntry (285, 1, 21, 2, 46), dActionEntry (286, 1, 21, 2, 46), dActionEntry (287, 1, 21, 2, 46), dActionEntry (288, 1, 21, 2, 46), dActionEntry (289, 1, 21, 2, 46), dActionEntry (290, 1, 21, 2, 46), dActionEntry (291, 1, 21, 2, 46), dActionEntry (292, 1, 21, 2, 46), dActionEntry (293, 1, 21, 2, 46), dActionEntry (294, 1, 21, 2, 46), dActionEntry (295, 1, 21, 2, 46), dActionEntry (296, 1, 21, 2, 46), dActionEntry (297, 1, 21, 2, 46), dActionEntry (298, 1, 21, 2, 46), dActionEntry (299, 1, 21, 2, 46), 
					dActionEntry (260, 1, 21, 2, 47), dActionEntry (264, 1, 21, 2, 47), dActionEntry (265, 1, 21, 2, 47), dActionEntry (266, 1, 21, 2, 47), dActionEntry (267, 1, 21, 2, 47), dActionEntry (268, 1, 21, 2, 47), dActionEntry (269, 1, 21, 2, 47), dActionEntry (270, 1, 21, 2, 47), dActionEntry (271, 1, 21, 2, 47), dActionEntry (272, 1, 21, 2, 47), dActionEntry (273, 1, 21, 2, 47), dActionEntry (274, 1, 21, 2, 47), dActionEntry (275, 1, 21, 2, 47), dActionEntry (276, 1, 21, 2, 47), dActionEntry (277, 1, 21, 2, 47), dActionEntry (278, 1, 21, 2, 47), dActionEntry (279, 1, 21, 2, 47), dActionEntry (280, 1, 21, 2, 47), dActionEntry (281, 1, 21, 2, 47), dActionEntry (282, 1, 21, 2, 47), dActionEntry (283, 1, 21, 2, 47), dActionEntry (284, 1, 21, 2, 47), dActionEntry (285, 1, 21, 2, 47), dActionEntry (286, 1, 21, 2, 47), dActionEntry (287, 1, 21, 2, 47), dActionEntry (288, 1, 21, 2, 47), dActionEntry (289, 1, 21, 2, 47), dActionEntry (290, 1, 21, 2, 47), dActionEntry (291, 1, 21, 2, 47), dActionEntry (292, 1, 21, 2, 47), dActionEntry (293, 1, 21, 2, 47), dActionEntry (294, 1, 21, 2, 47), dActionEntry (295, 1, 21, 2, 47), dActionEntry (296, 1, 21, 2, 47), dActionEntry (297, 1, 21, 2, 47), dActionEntry (298, 1, 21, 2, 47), dActionEntry (299, 1, 21, 2, 47), 
					dActionEntry (44, 0, 108, 0, 0), 
					dActionEntry (44, 0, 109, 0, 0), 
					dActionEntry (44, 0, 110, 0, 0), 
					dActionEntry (260, 1, 21, 2, 44), dActionEntry (264, 1, 21, 2, 44), dActionEntry (265, 1, 21, 2, 44), dActionEntry (266, 1, 21, 2, 44), dActionEntry (267, 1, 21, 2, 44), dActionEntry (268, 1, 21, 2, 44), dActionEntry (269, 1, 21, 2, 44), dActionEntry (270, 1, 21, 2, 44), dActionEntry (271, 1, 21, 2, 44), dActionEntry (272, 1, 21, 2, 44), dActionEntry (273, 1, 21, 2, 44), dActionEntry (274, 1, 21, 2, 44), dActionEntry (275, 1, 21, 2, 44), dActionEntry (276, 1, 21, 2, 44), dActionEntry (277, 1, 21, 2, 44), dActionEntry (278, 1, 21, 2, 44), dActionEntry (279, 1, 21, 2, 44), dActionEntry (280, 1, 21, 2, 44), dActionEntry (281, 1, 21, 2, 44), dActionEntry (282, 1, 21, 2, 44), dActionEntry (283, 1, 21, 2, 44), dActionEntry (284, 1, 21, 2, 44), dActionEntry (285, 1, 21, 2, 44), dActionEntry (286, 1, 21, 2, 44), dActionEntry (287, 1, 21, 2, 44), dActionEntry (288, 1, 21, 2, 44), dActionEntry (289, 1, 21, 2, 44), dActionEntry (290, 1, 21, 2, 44), dActionEntry (291, 1, 21, 2, 44), dActionEntry (292, 1, 21, 2, 44), dActionEntry (293, 1, 21, 2, 44), dActionEntry (294, 1, 21, 2, 44), dActionEntry (295, 1, 21, 2, 44), dActionEntry (296, 1, 21, 2, 44), dActionEntry (297, 1, 21, 2, 44), dActionEntry (298, 1, 21, 2, 44), dActionEntry (299, 1, 21, 2, 44), 
					dActionEntry (44, 0, 111, 0, 0), 
					dActionEntry (44, 0, 112, 0, 0), 
					dActionEntry (44, 0, 113, 0, 0), 
					dActionEntry (255, 1, 5, 5, 16), dActionEntry (256, 1, 5, 5, 16), dActionEntry (259, 1, 5, 5, 16), dActionEntry (261, 1, 5, 5, 16), 
					dActionEntry (260, 1, 11, 2, 21), dActionEntry (264, 1, 11, 2, 21), dActionEntry (265, 1, 11, 2, 21), dActionEntry (266, 1, 11, 2, 21), dActionEntry (267, 1, 11, 2, 21), dActionEntry (268, 1, 11, 2, 21), dActionEntry (269, 1, 11, 2, 21), dActionEntry (270, 1, 11, 2, 21), dActionEntry (271, 1, 11, 2, 21), dActionEntry (272, 1, 11, 2, 21), dActionEntry (273, 1, 11, 2, 21), dActionEntry (274, 1, 11, 2, 21), dActionEntry (275, 1, 11, 2, 21), dActionEntry (276, 1, 11, 2, 21), dActionEntry (277, 1, 11, 2, 21), dActionEntry (278, 1, 11, 2, 21), dActionEntry (279, 1, 11, 2, 21), dActionEntry (280, 1, 11, 2, 21), dActionEntry (281, 1, 11, 2, 21), dActionEntry (282, 1, 11, 2, 21), dActionEntry (283, 1, 11, 2, 21), dActionEntry (284, 1, 11, 2, 21), dActionEntry (285, 1, 11, 2, 21), dActionEntry (286, 1, 11, 2, 21), dActionEntry (287, 1, 11, 2, 21), dActionEntry (288, 1, 11, 2, 21), dActionEntry (289, 1, 11, 2, 21), dActionEntry (290, 1, 11, 2, 21), dActionEntry (291, 1, 11, 2, 21), dActionEntry (292, 1, 11, 2, 21), dActionEntry (293, 1, 11, 2, 21), dActionEntry (294, 1, 11, 2, 21), dActionEntry (295, 1, 11, 2, 21), dActionEntry (296, 1, 11, 2, 21), dActionEntry (297, 1, 11, 2, 21), dActionEntry (298, 1, 11, 2, 21), dActionEntry (299, 1, 11, 2, 21), 
					dActionEntry (44, 0, 114, 0, 0), 
					dActionEntry (44, 0, 115, 0, 0), 
					dActionEntry (44, 0, 116, 0, 0), 
					dActionEntry (44, 0, 117, 0, 0), 
					dActionEntry (46, 1, 6, 3, 10), dActionEntry (62, 1, 6, 3, 10), 
					dActionEntry (257, 0, 119, 0, 0), dActionEntry (262, 0, 118, 0, 0), dActionEntry (263, 0, 80, 0, 0), 
					dActionEntry (257, 0, 131, 0, 0), 
					dActionEntry (260, 1, 19, 4, 37), dActionEntry (264, 1, 19, 4, 37), dActionEntry (265, 1, 19, 4, 37), dActionEntry (266, 1, 19, 4, 37), dActionEntry (267, 1, 19, 4, 37), dActionEntry (268, 1, 19, 4, 37), dActionEntry (269, 1, 19, 4, 37), dActionEntry (270, 1, 19, 4, 37), dActionEntry (271, 1, 19, 4, 37), dActionEntry (272, 1, 19, 4, 37), dActionEntry (273, 1, 19, 4, 37), dActionEntry (274, 1, 19, 4, 37), dActionEntry (275, 1, 19, 4, 37), dActionEntry (276, 1, 19, 4, 37), dActionEntry (277, 1, 19, 4, 37), dActionEntry (278, 1, 19, 4, 37), dActionEntry (279, 1, 19, 4, 37), dActionEntry (280, 1, 19, 4, 37), dActionEntry (281, 1, 19, 4, 37), dActionEntry (282, 1, 19, 4, 37), dActionEntry (283, 1, 19, 4, 37), dActionEntry (284, 1, 19, 4, 37), dActionEntry (285, 1, 19, 4, 37), dActionEntry (286, 1, 19, 4, 37), dActionEntry (287, 1, 19, 4, 37), dActionEntry (288, 1, 19, 4, 37), dActionEntry (289, 1, 19, 4, 37), dActionEntry (290, 1, 19, 4, 37), dActionEntry (291, 1, 19, 4, 37), dActionEntry (292, 1, 19, 4, 37), dActionEntry (293, 1, 19, 4, 37), dActionEntry (294, 1, 19, 4, 37), dActionEntry (295, 1, 19, 4, 37), dActionEntry (296, 1, 19, 4, 37), dActionEntry (297, 1, 19, 4, 37), dActionEntry (298, 1, 19, 4, 37), dActionEntry (299, 1, 19, 4, 37), 
					dActionEntry (260, 1, 19, 4, 38), dActionEntry (264, 1, 19, 4, 38), dActionEntry (265, 1, 19, 4, 38), dActionEntry (266, 1, 19, 4, 38), dActionEntry (267, 1, 19, 4, 38), dActionEntry (268, 1, 19, 4, 38), dActionEntry (269, 1, 19, 4, 38), dActionEntry (270, 1, 19, 4, 38), dActionEntry (271, 1, 19, 4, 38), dActionEntry (272, 1, 19, 4, 38), dActionEntry (273, 1, 19, 4, 38), dActionEntry (274, 1, 19, 4, 38), dActionEntry (275, 1, 19, 4, 38), dActionEntry (276, 1, 19, 4, 38), dActionEntry (277, 1, 19, 4, 38), dActionEntry (278, 1, 19, 4, 38), dActionEntry (279, 1, 19, 4, 38), dActionEntry (280, 1, 19, 4, 38), dActionEntry (281, 1, 19, 4, 38), dActionEntry (282, 1, 19, 4, 38), dActionEntry (283, 1, 19, 4, 38), dActionEntry (284, 1, 19, 4, 38), dActionEntry (285, 1, 19, 4, 38), dActionEntry (286, 1, 19, 4, 38), dActionEntry (287, 1, 19, 4, 38), dActionEntry (288, 1, 19, 4, 38), dActionEntry (289, 1, 19, 4, 38), dActionEntry (290, 1, 19, 4, 38), dActionEntry (291, 1, 19, 4, 38), dActionEntry (292, 1, 19, 4, 38), dActionEntry (293, 1, 19, 4, 38), dActionEntry (294, 1, 19, 4, 38), dActionEntry (295, 1, 19, 4, 38), dActionEntry (296, 1, 19, 4, 38), dActionEntry (297, 1, 19, 4, 38), dActionEntry (298, 1, 19, 4, 38), dActionEntry (299, 1, 19, 4, 38), 
					dActionEntry (44, 0, 136, 0, 0), 
					dActionEntry (260, 1, 21, 4, 42), dActionEntry (264, 1, 21, 4, 42), dActionEntry (265, 1, 21, 4, 42), dActionEntry (266, 1, 21, 4, 42), dActionEntry (267, 1, 21, 4, 42), dActionEntry (268, 1, 21, 4, 42), dActionEntry (269, 1, 21, 4, 42), dActionEntry (270, 1, 21, 4, 42), dActionEntry (271, 1, 21, 4, 42), dActionEntry (272, 1, 21, 4, 42), dActionEntry (273, 1, 21, 4, 42), dActionEntry (274, 1, 21, 4, 42), dActionEntry (275, 1, 21, 4, 42), dActionEntry (276, 1, 21, 4, 42), dActionEntry (277, 1, 21, 4, 42), dActionEntry (278, 1, 21, 4, 42), dActionEntry (279, 1, 21, 4, 42), dActionEntry (280, 1, 21, 4, 42), dActionEntry (281, 1, 21, 4, 42), dActionEntry (282, 1, 21, 4, 42), dActionEntry (283, 1, 21, 4, 42), dActionEntry (284, 1, 21, 4, 42), dActionEntry (285, 1, 21, 4, 42), dActionEntry (286, 1, 21, 4, 42), dActionEntry (287, 1, 21, 4, 42), dActionEntry (288, 1, 21, 4, 42), dActionEntry (289, 1, 21, 4, 42), dActionEntry (290, 1, 21, 4, 42), dActionEntry (291, 1, 21, 4, 42), dActionEntry (292, 1, 21, 4, 42), dActionEntry (293, 1, 21, 4, 42), dActionEntry (294, 1, 21, 4, 42), dActionEntry (295, 1, 21, 4, 42), dActionEntry (296, 1, 21, 4, 42), dActionEntry (297, 1, 21, 4, 42), dActionEntry (298, 1, 21, 4, 42), dActionEntry (299, 1, 21, 4, 42), 
					dActionEntry (260, 1, 21, 4, 43), dActionEntry (264, 1, 21, 4, 43), dActionEntry (265, 1, 21, 4, 43), dActionEntry (266, 1, 21, 4, 43), dActionEntry (267, 1, 21, 4, 43), dActionEntry (268, 1, 21, 4, 43), dActionEntry (269, 1, 21, 4, 43), dActionEntry (270, 1, 21, 4, 43), dActionEntry (271, 1, 21, 4, 43), dActionEntry (272, 1, 21, 4, 43), dActionEntry (273, 1, 21, 4, 43), dActionEntry (274, 1, 21, 4, 43), dActionEntry (275, 1, 21, 4, 43), dActionEntry (276, 1, 21, 4, 43), dActionEntry (277, 1, 21, 4, 43), dActionEntry (278, 1, 21, 4, 43), dActionEntry (279, 1, 21, 4, 43), dActionEntry (280, 1, 21, 4, 43), dActionEntry (281, 1, 21, 4, 43), dActionEntry (282, 1, 21, 4, 43), dActionEntry (283, 1, 21, 4, 43), dActionEntry (284, 1, 21, 4, 43), dActionEntry (285, 1, 21, 4, 43), dActionEntry (286, 1, 21, 4, 43), dActionEntry (287, 1, 21, 4, 43), dActionEntry (288, 1, 21, 4, 43), dActionEntry (289, 1, 21, 4, 43), dActionEntry (290, 1, 21, 4, 43), dActionEntry (291, 1, 21, 4, 43), dActionEntry (292, 1, 21, 4, 43), dActionEntry (293, 1, 21, 4, 43), dActionEntry (294, 1, 21, 4, 43), dActionEntry (295, 1, 21, 4, 43), dActionEntry (296, 1, 21, 4, 43), dActionEntry (297, 1, 21, 4, 43), dActionEntry (298, 1, 21, 4, 43), dActionEntry (299, 1, 21, 4, 43), 
					dActionEntry (44, 0, 137, 0, 0), 
					dActionEntry (260, 1, 22, 4, 49), dActionEntry (264, 1, 22, 4, 49), dActionEntry (265, 1, 22, 4, 49), dActionEntry (266, 1, 22, 4, 49), dActionEntry (267, 1, 22, 4, 49), dActionEntry (268, 1, 22, 4, 49), dActionEntry (269, 1, 22, 4, 49), dActionEntry (270, 1, 22, 4, 49), dActionEntry (271, 1, 22, 4, 49), dActionEntry (272, 1, 22, 4, 49), dActionEntry (273, 1, 22, 4, 49), dActionEntry (274, 1, 22, 4, 49), dActionEntry (275, 1, 22, 4, 49), dActionEntry (276, 1, 22, 4, 49), dActionEntry (277, 1, 22, 4, 49), dActionEntry (278, 1, 22, 4, 49), dActionEntry (279, 1, 22, 4, 49), dActionEntry (280, 1, 22, 4, 49), dActionEntry (281, 1, 22, 4, 49), dActionEntry (282, 1, 22, 4, 49), dActionEntry (283, 1, 22, 4, 49), dActionEntry (284, 1, 22, 4, 49), dActionEntry (285, 1, 22, 4, 49), dActionEntry (286, 1, 22, 4, 49), dActionEntry (287, 1, 22, 4, 49), dActionEntry (288, 1, 22, 4, 49), dActionEntry (289, 1, 22, 4, 49), dActionEntry (290, 1, 22, 4, 49), dActionEntry (291, 1, 22, 4, 49), dActionEntry (292, 1, 22, 4, 49), dActionEntry (293, 1, 22, 4, 49), dActionEntry (294, 1, 22, 4, 49), dActionEntry (295, 1, 22, 4, 49), dActionEntry (296, 1, 22, 4, 49), dActionEntry (297, 1, 22, 4, 49), dActionEntry (298, 1, 22, 4, 49), dActionEntry (299, 1, 22, 4, 49), 
					dActionEntry (260, 1, 14, 4, 31), dActionEntry (264, 1, 14, 4, 31), dActionEntry (265, 1, 14, 4, 31), dActionEntry (266, 1, 14, 4, 31), dActionEntry (267, 1, 14, 4, 31), dActionEntry (268, 1, 14, 4, 31), dActionEntry (269, 1, 14, 4, 31), dActionEntry (270, 1, 14, 4, 31), dActionEntry (271, 1, 14, 4, 31), dActionEntry (272, 1, 14, 4, 31), dActionEntry (273, 1, 14, 4, 31), dActionEntry (274, 1, 14, 4, 31), dActionEntry (275, 1, 14, 4, 31), dActionEntry (276, 1, 14, 4, 31), dActionEntry (277, 1, 14, 4, 31), dActionEntry (278, 1, 14, 4, 31), dActionEntry (279, 1, 14, 4, 31), dActionEntry (280, 1, 14, 4, 31), dActionEntry (281, 1, 14, 4, 31), dActionEntry (282, 1, 14, 4, 31), dActionEntry (283, 1, 14, 4, 31), dActionEntry (284, 1, 14, 4, 31), dActionEntry (285, 1, 14, 4, 31), dActionEntry (286, 1, 14, 4, 31), dActionEntry (287, 1, 14, 4, 31), dActionEntry (288, 1, 14, 4, 31), dActionEntry (289, 1, 14, 4, 31), dActionEntry (290, 1, 14, 4, 31), dActionEntry (291, 1, 14, 4, 31), dActionEntry (292, 1, 14, 4, 31), dActionEntry (293, 1, 14, 4, 31), dActionEntry (294, 1, 14, 4, 31), dActionEntry (295, 1, 14, 4, 31), dActionEntry (296, 1, 14, 4, 31), dActionEntry (297, 1, 14, 4, 31), dActionEntry (298, 1, 14, 4, 31), dActionEntry (299, 1, 14, 4, 31), 
					dActionEntry (260, 1, 22, 4, 51), dActionEntry (264, 1, 22, 4, 51), dActionEntry (265, 1, 22, 4, 51), dActionEntry (266, 1, 22, 4, 51), dActionEntry (267, 1, 22, 4, 51), dActionEntry (268, 1, 22, 4, 51), dActionEntry (269, 1, 22, 4, 51), dActionEntry (270, 1, 22, 4, 51), dActionEntry (271, 1, 22, 4, 51), dActionEntry (272, 1, 22, 4, 51), dActionEntry (273, 1, 22, 4, 51), dActionEntry (274, 1, 22, 4, 51), dActionEntry (275, 1, 22, 4, 51), dActionEntry (276, 1, 22, 4, 51), dActionEntry (277, 1, 22, 4, 51), dActionEntry (278, 1, 22, 4, 51), dActionEntry (279, 1, 22, 4, 51), dActionEntry (280, 1, 22, 4, 51), dActionEntry (281, 1, 22, 4, 51), dActionEntry (282, 1, 22, 4, 51), dActionEntry (283, 1, 22, 4, 51), dActionEntry (284, 1, 22, 4, 51), dActionEntry (285, 1, 22, 4, 51), dActionEntry (286, 1, 22, 4, 51), dActionEntry (287, 1, 22, 4, 51), dActionEntry (288, 1, 22, 4, 51), dActionEntry (289, 1, 22, 4, 51), dActionEntry (290, 1, 22, 4, 51), dActionEntry (291, 1, 22, 4, 51), dActionEntry (292, 1, 22, 4, 51), dActionEntry (293, 1, 22, 4, 51), dActionEntry (294, 1, 22, 4, 51), dActionEntry (295, 1, 22, 4, 51), dActionEntry (296, 1, 22, 4, 51), dActionEntry (297, 1, 22, 4, 51), dActionEntry (298, 1, 22, 4, 51), dActionEntry (299, 1, 22, 4, 51), 
					dActionEntry (260, 1, 22, 4, 50), dActionEntry (264, 1, 22, 4, 50), dActionEntry (265, 1, 22, 4, 50), dActionEntry (266, 1, 22, 4, 50), dActionEntry (267, 1, 22, 4, 50), dActionEntry (268, 1, 22, 4, 50), dActionEntry (269, 1, 22, 4, 50), dActionEntry (270, 1, 22, 4, 50), dActionEntry (271, 1, 22, 4, 50), dActionEntry (272, 1, 22, 4, 50), dActionEntry (273, 1, 22, 4, 50), dActionEntry (274, 1, 22, 4, 50), dActionEntry (275, 1, 22, 4, 50), dActionEntry (276, 1, 22, 4, 50), dActionEntry (277, 1, 22, 4, 50), dActionEntry (278, 1, 22, 4, 50), dActionEntry (279, 1, 22, 4, 50), dActionEntry (280, 1, 22, 4, 50), dActionEntry (281, 1, 22, 4, 50), dActionEntry (282, 1, 22, 4, 50), dActionEntry (283, 1, 22, 4, 50), dActionEntry (284, 1, 22, 4, 50), dActionEntry (285, 1, 22, 4, 50), dActionEntry (286, 1, 22, 4, 50), dActionEntry (287, 1, 22, 4, 50), dActionEntry (288, 1, 22, 4, 50), dActionEntry (289, 1, 22, 4, 50), dActionEntry (290, 1, 22, 4, 50), dActionEntry (291, 1, 22, 4, 50), dActionEntry (292, 1, 22, 4, 50), dActionEntry (293, 1, 22, 4, 50), dActionEntry (294, 1, 22, 4, 50), dActionEntry (295, 1, 22, 4, 50), dActionEntry (296, 1, 22, 4, 50), dActionEntry (297, 1, 22, 4, 50), dActionEntry (298, 1, 22, 4, 50), dActionEntry (299, 1, 22, 4, 50), 
					dActionEntry (44, 0, 138, 0, 0), 
					dActionEntry (260, 1, 19, 4, 36), dActionEntry (264, 1, 19, 4, 36), dActionEntry (265, 1, 19, 4, 36), dActionEntry (266, 1, 19, 4, 36), dActionEntry (267, 1, 19, 4, 36), dActionEntry (268, 1, 19, 4, 36), dActionEntry (269, 1, 19, 4, 36), dActionEntry (270, 1, 19, 4, 36), dActionEntry (271, 1, 19, 4, 36), dActionEntry (272, 1, 19, 4, 36), dActionEntry (273, 1, 19, 4, 36), dActionEntry (274, 1, 19, 4, 36), dActionEntry (275, 1, 19, 4, 36), dActionEntry (276, 1, 19, 4, 36), dActionEntry (277, 1, 19, 4, 36), dActionEntry (278, 1, 19, 4, 36), dActionEntry (279, 1, 19, 4, 36), dActionEntry (280, 1, 19, 4, 36), dActionEntry (281, 1, 19, 4, 36), dActionEntry (282, 1, 19, 4, 36), dActionEntry (283, 1, 19, 4, 36), dActionEntry (284, 1, 19, 4, 36), dActionEntry (285, 1, 19, 4, 36), dActionEntry (286, 1, 19, 4, 36), dActionEntry (287, 1, 19, 4, 36), dActionEntry (288, 1, 19, 4, 36), dActionEntry (289, 1, 19, 4, 36), dActionEntry (290, 1, 19, 4, 36), dActionEntry (291, 1, 19, 4, 36), dActionEntry (292, 1, 19, 4, 36), dActionEntry (293, 1, 19, 4, 36), dActionEntry (294, 1, 19, 4, 36), dActionEntry (295, 1, 19, 4, 36), dActionEntry (296, 1, 19, 4, 36), dActionEntry (297, 1, 19, 4, 36), dActionEntry (298, 1, 19, 4, 36), dActionEntry (299, 1, 19, 4, 36), 
					dActionEntry (44, 0, 139, 0, 0), 
					dActionEntry (260, 1, 7, 1, 15), dActionEntry (264, 1, 7, 1, 15), dActionEntry (265, 1, 7, 1, 15), dActionEntry (266, 1, 7, 1, 15), dActionEntry (267, 1, 7, 1, 15), dActionEntry (268, 1, 7, 1, 15), dActionEntry (269, 1, 7, 1, 15), dActionEntry (270, 1, 7, 1, 15), dActionEntry (271, 1, 7, 1, 15), dActionEntry (272, 1, 7, 1, 15), dActionEntry (273, 1, 7, 1, 15), dActionEntry (274, 1, 7, 1, 15), dActionEntry (275, 1, 7, 1, 15), dActionEntry (276, 1, 7, 1, 15), dActionEntry (277, 1, 7, 1, 15), dActionEntry (278, 1, 7, 1, 15), dActionEntry (279, 1, 7, 1, 15), dActionEntry (280, 1, 7, 1, 15), dActionEntry (281, 1, 7, 1, 15), dActionEntry (282, 1, 7, 1, 15), dActionEntry (283, 1, 7, 1, 15), dActionEntry (284, 1, 7, 1, 15), dActionEntry (285, 1, 7, 1, 15), dActionEntry (286, 1, 7, 1, 15), dActionEntry (287, 1, 7, 1, 15), dActionEntry (288, 1, 7, 1, 15), dActionEntry (289, 1, 7, 1, 15), dActionEntry (290, 1, 7, 1, 15), dActionEntry (291, 1, 7, 1, 15), dActionEntry (292, 1, 7, 1, 15), dActionEntry (293, 1, 7, 1, 15), dActionEntry (294, 1, 7, 1, 15), dActionEntry (295, 1, 7, 1, 15), dActionEntry (296, 1, 7, 1, 15), dActionEntry (297, 1, 7, 1, 15), dActionEntry (298, 1, 7, 1, 15), dActionEntry (299, 1, 7, 1, 15), 
					dActionEntry (260, 1, 15, 4, 32), dActionEntry (264, 1, 15, 4, 32), dActionEntry (265, 1, 15, 4, 32), dActionEntry (266, 1, 15, 4, 32), dActionEntry (267, 1, 15, 4, 32), dActionEntry (268, 1, 15, 4, 32), dActionEntry (269, 1, 15, 4, 32), dActionEntry (270, 1, 15, 4, 32), dActionEntry (271, 1, 15, 4, 32), dActionEntry (272, 1, 15, 4, 32), dActionEntry (273, 1, 15, 4, 32), dActionEntry (274, 1, 15, 4, 32), dActionEntry (275, 1, 15, 4, 32), dActionEntry (276, 1, 15, 4, 32), dActionEntry (277, 1, 15, 4, 32), dActionEntry (278, 1, 15, 4, 32), dActionEntry (279, 1, 15, 4, 32), dActionEntry (280, 1, 15, 4, 32), dActionEntry (281, 1, 15, 4, 32), dActionEntry (282, 1, 15, 4, 32), dActionEntry (283, 1, 15, 4, 32), dActionEntry (284, 1, 15, 4, 32), dActionEntry (285, 1, 15, 4, 32), dActionEntry (286, 1, 15, 4, 32), dActionEntry (287, 1, 15, 4, 32), dActionEntry (288, 1, 15, 4, 32), dActionEntry (289, 1, 15, 4, 32), dActionEntry (290, 1, 15, 4, 32), dActionEntry (291, 1, 15, 4, 32), dActionEntry (292, 1, 15, 4, 32), dActionEntry (293, 1, 15, 4, 32), dActionEntry (294, 1, 15, 4, 32), dActionEntry (295, 1, 15, 4, 32), dActionEntry (296, 1, 15, 4, 32), dActionEntry (297, 1, 15, 4, 32), dActionEntry (298, 1, 15, 4, 32), dActionEntry (299, 1, 15, 4, 32), 
					dActionEntry (260, 1, 18, 4, 35), dActionEntry (264, 1, 18, 4, 35), dActionEntry (265, 1, 18, 4, 35), dActionEntry (266, 1, 18, 4, 35), dActionEntry (267, 1, 18, 4, 35), dActionEntry (268, 1, 18, 4, 35), dActionEntry (269, 1, 18, 4, 35), dActionEntry (270, 1, 18, 4, 35), dActionEntry (271, 1, 18, 4, 35), dActionEntry (272, 1, 18, 4, 35), dActionEntry (273, 1, 18, 4, 35), dActionEntry (274, 1, 18, 4, 35), dActionEntry (275, 1, 18, 4, 35), dActionEntry (276, 1, 18, 4, 35), dActionEntry (277, 1, 18, 4, 35), dActionEntry (278, 1, 18, 4, 35), dActionEntry (279, 1, 18, 4, 35), dActionEntry (280, 1, 18, 4, 35), dActionEntry (281, 1, 18, 4, 35), dActionEntry (282, 1, 18, 4, 35), dActionEntry (283, 1, 18, 4, 35), dActionEntry (284, 1, 18, 4, 35), dActionEntry (285, 1, 18, 4, 35), dActionEntry (286, 1, 18, 4, 35), dActionEntry (287, 1, 18, 4, 35), dActionEntry (288, 1, 18, 4, 35), dActionEntry (289, 1, 18, 4, 35), dActionEntry (290, 1, 18, 4, 35), dActionEntry (291, 1, 18, 4, 35), dActionEntry (292, 1, 18, 4, 35), dActionEntry (293, 1, 18, 4, 35), dActionEntry (294, 1, 18, 4, 35), dActionEntry (295, 1, 18, 4, 35), dActionEntry (296, 1, 18, 4, 35), dActionEntry (297, 1, 18, 4, 35), dActionEntry (298, 1, 18, 4, 35), dActionEntry (299, 1, 18, 4, 35), 
					dActionEntry (260, 1, 17, 4, 34), dActionEntry (264, 1, 17, 4, 34), dActionEntry (265, 1, 17, 4, 34), dActionEntry (266, 1, 17, 4, 34), dActionEntry (267, 1, 17, 4, 34), dActionEntry (268, 1, 17, 4, 34), dActionEntry (269, 1, 17, 4, 34), dActionEntry (270, 1, 17, 4, 34), dActionEntry (271, 1, 17, 4, 34), dActionEntry (272, 1, 17, 4, 34), dActionEntry (273, 1, 17, 4, 34), dActionEntry (274, 1, 17, 4, 34), dActionEntry (275, 1, 17, 4, 34), dActionEntry (276, 1, 17, 4, 34), dActionEntry (277, 1, 17, 4, 34), dActionEntry (278, 1, 17, 4, 34), dActionEntry (279, 1, 17, 4, 34), dActionEntry (280, 1, 17, 4, 34), dActionEntry (281, 1, 17, 4, 34), dActionEntry (282, 1, 17, 4, 34), dActionEntry (283, 1, 17, 4, 34), dActionEntry (284, 1, 17, 4, 34), dActionEntry (285, 1, 17, 4, 34), dActionEntry (286, 1, 17, 4, 34), dActionEntry (287, 1, 17, 4, 34), dActionEntry (288, 1, 17, 4, 34), dActionEntry (289, 1, 17, 4, 34), dActionEntry (290, 1, 17, 4, 34), dActionEntry (291, 1, 17, 4, 34), dActionEntry (292, 1, 17, 4, 34), dActionEntry (293, 1, 17, 4, 34), dActionEntry (294, 1, 17, 4, 34), dActionEntry (295, 1, 17, 4, 34), dActionEntry (296, 1, 17, 4, 34), dActionEntry (297, 1, 17, 4, 34), dActionEntry (298, 1, 17, 4, 34), dActionEntry (299, 1, 17, 4, 34), 
					dActionEntry (260, 1, 16, 4, 33), dActionEntry (264, 1, 16, 4, 33), dActionEntry (265, 1, 16, 4, 33), dActionEntry (266, 1, 16, 4, 33), dActionEntry (267, 1, 16, 4, 33), dActionEntry (268, 1, 16, 4, 33), dActionEntry (269, 1, 16, 4, 33), dActionEntry (270, 1, 16, 4, 33), dActionEntry (271, 1, 16, 4, 33), dActionEntry (272, 1, 16, 4, 33), dActionEntry (273, 1, 16, 4, 33), dActionEntry (274, 1, 16, 4, 33), dActionEntry (275, 1, 16, 4, 33), dActionEntry (276, 1, 16, 4, 33), dActionEntry (277, 1, 16, 4, 33), dActionEntry (278, 1, 16, 4, 33), dActionEntry (279, 1, 16, 4, 33), dActionEntry (280, 1, 16, 4, 33), dActionEntry (281, 1, 16, 4, 33), dActionEntry (282, 1, 16, 4, 33), dActionEntry (283, 1, 16, 4, 33), dActionEntry (284, 1, 16, 4, 33), dActionEntry (285, 1, 16, 4, 33), dActionEntry (286, 1, 16, 4, 33), dActionEntry (287, 1, 16, 4, 33), dActionEntry (288, 1, 16, 4, 33), dActionEntry (289, 1, 16, 4, 33), dActionEntry (290, 1, 16, 4, 33), dActionEntry (291, 1, 16, 4, 33), dActionEntry (292, 1, 16, 4, 33), dActionEntry (293, 1, 16, 4, 33), dActionEntry (294, 1, 16, 4, 33), dActionEntry (295, 1, 16, 4, 33), dActionEntry (296, 1, 16, 4, 33), dActionEntry (297, 1, 16, 4, 33), dActionEntry (298, 1, 16, 4, 33), dActionEntry (299, 1, 16, 4, 33), 
					dActionEntry (257, 0, 141, 0, 0), dActionEntry (262, 0, 140, 0, 0), 
					dActionEntry (260, 1, 19, 6, 39), dActionEntry (264, 1, 19, 6, 39), dActionEntry (265, 1, 19, 6, 39), dActionEntry (266, 1, 19, 6, 39), dActionEntry (267, 1, 19, 6, 39), dActionEntry (268, 1, 19, 6, 39), dActionEntry (269, 1, 19, 6, 39), dActionEntry (270, 1, 19, 6, 39), dActionEntry (271, 1, 19, 6, 39), dActionEntry (272, 1, 19, 6, 39), dActionEntry (273, 1, 19, 6, 39), dActionEntry (274, 1, 19, 6, 39), dActionEntry (275, 1, 19, 6, 39), dActionEntry (276, 1, 19, 6, 39), dActionEntry (277, 1, 19, 6, 39), dActionEntry (278, 1, 19, 6, 39), dActionEntry (279, 1, 19, 6, 39), dActionEntry (280, 1, 19, 6, 39), dActionEntry (281, 1, 19, 6, 39), dActionEntry (282, 1, 19, 6, 39), dActionEntry (283, 1, 19, 6, 39), dActionEntry (284, 1, 19, 6, 39), dActionEntry (285, 1, 19, 6, 39), dActionEntry (286, 1, 19, 6, 39), dActionEntry (287, 1, 19, 6, 39), dActionEntry (288, 1, 19, 6, 39), dActionEntry (289, 1, 19, 6, 39), dActionEntry (290, 1, 19, 6, 39), dActionEntry (291, 1, 19, 6, 39), dActionEntry (292, 1, 19, 6, 39), dActionEntry (293, 1, 19, 6, 39), dActionEntry (294, 1, 19, 6, 39), dActionEntry (295, 1, 19, 6, 39), dActionEntry (296, 1, 19, 6, 39), dActionEntry (297, 1, 19, 6, 39), dActionEntry (298, 1, 19, 6, 39), dActionEntry (299, 1, 19, 6, 39), 
					dActionEntry (260, 1, 19, 6, 40), dActionEntry (264, 1, 19, 6, 40), dActionEntry (265, 1, 19, 6, 40), dActionEntry (266, 1, 19, 6, 40), dActionEntry (267, 1, 19, 6, 40), dActionEntry (268, 1, 19, 6, 40), dActionEntry (269, 1, 19, 6, 40), dActionEntry (270, 1, 19, 6, 40), dActionEntry (271, 1, 19, 6, 40), dActionEntry (272, 1, 19, 6, 40), dActionEntry (273, 1, 19, 6, 40), dActionEntry (274, 1, 19, 6, 40), dActionEntry (275, 1, 19, 6, 40), dActionEntry (276, 1, 19, 6, 40), dActionEntry (277, 1, 19, 6, 40), dActionEntry (278, 1, 19, 6, 40), dActionEntry (279, 1, 19, 6, 40), dActionEntry (280, 1, 19, 6, 40), dActionEntry (281, 1, 19, 6, 40), dActionEntry (282, 1, 19, 6, 40), dActionEntry (283, 1, 19, 6, 40), dActionEntry (284, 1, 19, 6, 40), dActionEntry (285, 1, 19, 6, 40), dActionEntry (286, 1, 19, 6, 40), dActionEntry (287, 1, 19, 6, 40), dActionEntry (288, 1, 19, 6, 40), dActionEntry (289, 1, 19, 6, 40), dActionEntry (290, 1, 19, 6, 40), dActionEntry (291, 1, 19, 6, 40), dActionEntry (292, 1, 19, 6, 40), dActionEntry (293, 1, 19, 6, 40), dActionEntry (294, 1, 19, 6, 40), dActionEntry (295, 1, 19, 6, 40), dActionEntry (296, 1, 19, 6, 40), dActionEntry (297, 1, 19, 6, 40), dActionEntry (298, 1, 19, 6, 40), dActionEntry (299, 1, 19, 6, 40), 
					dActionEntry (260, 1, 22, 6, 48), dActionEntry (264, 1, 22, 6, 48), dActionEntry (265, 1, 22, 6, 48), dActionEntry (266, 1, 22, 6, 48), dActionEntry (267, 1, 22, 6, 48), dActionEntry (268, 1, 22, 6, 48), dActionEntry (269, 1, 22, 6, 48), dActionEntry (270, 1, 22, 6, 48), dActionEntry (271, 1, 22, 6, 48), dActionEntry (272, 1, 22, 6, 48), dActionEntry (273, 1, 22, 6, 48), dActionEntry (274, 1, 22, 6, 48), dActionEntry (275, 1, 22, 6, 48), dActionEntry (276, 1, 22, 6, 48), dActionEntry (277, 1, 22, 6, 48), dActionEntry (278, 1, 22, 6, 48), dActionEntry (279, 1, 22, 6, 48), dActionEntry (280, 1, 22, 6, 48), dActionEntry (281, 1, 22, 6, 48), dActionEntry (282, 1, 22, 6, 48), dActionEntry (283, 1, 22, 6, 48), dActionEntry (284, 1, 22, 6, 48), dActionEntry (285, 1, 22, 6, 48), dActionEntry (286, 1, 22, 6, 48), dActionEntry (287, 1, 22, 6, 48), dActionEntry (288, 1, 22, 6, 48), dActionEntry (289, 1, 22, 6, 48), dActionEntry (290, 1, 22, 6, 48), dActionEntry (291, 1, 22, 6, 48), dActionEntry (292, 1, 22, 6, 48), dActionEntry (293, 1, 22, 6, 48), dActionEntry (294, 1, 22, 6, 48), dActionEntry (295, 1, 22, 6, 48), dActionEntry (296, 1, 22, 6, 48), dActionEntry (297, 1, 22, 6, 48), dActionEntry (298, 1, 22, 6, 48), dActionEntry (299, 1, 22, 6, 48), 
					dActionEntry (260, 1, 21, 6, 45), dActionEntry (264, 1, 21, 6, 45), dActionEntry (265, 1, 21, 6, 45), dActionEntry (266, 1, 21, 6, 45), dActionEntry (267, 1, 21, 6, 45), dActionEntry (268, 1, 21, 6, 45), dActionEntry (269, 1, 21, 6, 45), dActionEntry (270, 1, 21, 6, 45), dActionEntry (271, 1, 21, 6, 45), dActionEntry (272, 1, 21, 6, 45), dActionEntry (273, 1, 21, 6, 45), dActionEntry (274, 1, 21, 6, 45), dActionEntry (275, 1, 21, 6, 45), dActionEntry (276, 1, 21, 6, 45), dActionEntry (277, 1, 21, 6, 45), dActionEntry (278, 1, 21, 6, 45), dActionEntry (279, 1, 21, 6, 45), dActionEntry (280, 1, 21, 6, 45), dActionEntry (281, 1, 21, 6, 45), dActionEntry (282, 1, 21, 6, 45), dActionEntry (283, 1, 21, 6, 45), dActionEntry (284, 1, 21, 6, 45), dActionEntry (285, 1, 21, 6, 45), dActionEntry (286, 1, 21, 6, 45), dActionEntry (287, 1, 21, 6, 45), dActionEntry (288, 1, 21, 6, 45), dActionEntry (289, 1, 21, 6, 45), dActionEntry (290, 1, 21, 6, 45), dActionEntry (291, 1, 21, 6, 45), dActionEntry (292, 1, 21, 6, 45), dActionEntry (293, 1, 21, 6, 45), dActionEntry (294, 1, 21, 6, 45), dActionEntry (295, 1, 21, 6, 45), dActionEntry (296, 1, 21, 6, 45), dActionEntry (297, 1, 21, 6, 45), dActionEntry (298, 1, 21, 6, 45), dActionEntry (299, 1, 21, 6, 45), 
					dActionEntry (260, 1, 20, 6, 41), dActionEntry (264, 1, 20, 6, 41), dActionEntry (265, 1, 20, 6, 41), dActionEntry (266, 1, 20, 6, 41), dActionEntry (267, 1, 20, 6, 41), dActionEntry (268, 1, 20, 6, 41), dActionEntry (269, 1, 20, 6, 41), dActionEntry (270, 1, 20, 6, 41), dActionEntry (271, 1, 20, 6, 41), dActionEntry (272, 1, 20, 6, 41), dActionEntry (273, 1, 20, 6, 41), dActionEntry (274, 1, 20, 6, 41), dActionEntry (275, 1, 20, 6, 41), dActionEntry (276, 1, 20, 6, 41), dActionEntry (277, 1, 20, 6, 41), dActionEntry (278, 1, 20, 6, 41), dActionEntry (279, 1, 20, 6, 41), dActionEntry (280, 1, 20, 6, 41), dActionEntry (281, 1, 20, 6, 41), dActionEntry (282, 1, 20, 6, 41), dActionEntry (283, 1, 20, 6, 41), dActionEntry (284, 1, 20, 6, 41), dActionEntry (285, 1, 20, 6, 41), dActionEntry (286, 1, 20, 6, 41), dActionEntry (287, 1, 20, 6, 41), dActionEntry (288, 1, 20, 6, 41), dActionEntry (289, 1, 20, 6, 41), dActionEntry (290, 1, 20, 6, 41), dActionEntry (291, 1, 20, 6, 41), dActionEntry (292, 1, 20, 6, 41), dActionEntry (293, 1, 20, 6, 41), dActionEntry (294, 1, 20, 6, 41), dActionEntry (295, 1, 20, 6, 41), dActionEntry (296, 1, 20, 6, 41), dActionEntry (297, 1, 20, 6, 41), dActionEntry (298, 1, 20, 6, 41), dActionEntry (299, 1, 20, 6, 41), 
			};

	static short gotoCount[] = {7, 1, 0, 0, 0, 1, 0, 0, 0, 0, 5, 0, 1, 2, 0, 0, 0, 0, 17, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 16, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0};
	static short gotoStart[] = {0, 7, 8, 8, 8, 8, 9, 9, 9, 9, 9, 14, 14, 15, 17, 17, 17, 17, 17, 34, 34, 34, 34, 35, 35, 35, 36, 36, 36, 36, 36, 36, 36, 36, 37, 38, 38, 39, 40, 41, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 45, 46, 46, 46, 46, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 65, 65, 66, 66, 67, 67, 67, 68, 68, 68, 69, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 86, 87, 88, 88, 88, 88, 88};
	static dGotoEntry gotoTable[] = {
					dGotoEntry (300, 8), dGotoEntry (301, 10), dGotoEntry (302, 9), dGotoEntry (303, 7), dGotoEntry (304, 4), dGotoEntry (305, 6), dGotoEntry (308, 5), 
					dGotoEntry (307, 12), dGotoEntry (307, 15), dGotoEntry (302, 16), dGotoEntry (303, 7), dGotoEntry (304, 4), dGotoEntry (305, 6), dGotoEntry (308, 5), 
					dGotoEntry (310, 18), dGotoEntry (306, 20), dGotoEntry (307, 21), dGotoEntry (311, 65), dGotoEntry (313, 64), dGotoEntry (314, 70), dGotoEntry (315, 66), dGotoEntry (316, 74), dGotoEntry (317, 71), dGotoEntry (318, 68), dGotoEntry (319, 59), dGotoEntry (320, 62), dGotoEntry (321, 61), dGotoEntry (322, 73), dGotoEntry (323, 67), dGotoEntry (324, 75), dGotoEntry (325, 72), dGotoEntry (326, 69), dGotoEntry (327, 60), dGotoEntry (328, 63), 
					dGotoEntry (309, 79), dGotoEntry (312, 81), dGotoEntry (312, 82), dGotoEntry (312, 83), 
					dGotoEntry (312, 84), dGotoEntry (312, 85), dGotoEntry (312, 87), dGotoEntry (329, 88), 
					dGotoEntry (312, 89), dGotoEntry (312, 90), dGotoEntry (312, 91), dGotoEntry (312, 92), 
					dGotoEntry (312, 93), dGotoEntry (312, 94), dGotoEntry (312, 95), dGotoEntry (313, 97), dGotoEntry (314, 70), dGotoEntry (315, 66), dGotoEntry (316, 74), dGotoEntry (317, 71), dGotoEntry (318, 68), dGotoEntry (319, 59), dGotoEntry (320, 62), dGotoEntry (321, 61), dGotoEntry (322, 73), dGotoEntry (323, 67), dGotoEntry (324, 75), dGotoEntry (325, 72), dGotoEntry (326, 69), dGotoEntry (327, 60), dGotoEntry (328, 63), 
					dGotoEntry (312, 98), dGotoEntry (312, 99), dGotoEntry (312, 100), dGotoEntry (312, 101), 
					dGotoEntry (307, 102), dGotoEntry (312, 120), dGotoEntry (329, 121), dGotoEntry (312, 122), 
					dGotoEntry (312, 123), dGotoEntry (312, 124), dGotoEntry (312, 125), dGotoEntry (329, 126), 
					dGotoEntry (329, 127), dGotoEntry (312, 128), dGotoEntry (312, 129), dGotoEntry (312, 130), 
					dGotoEntry (307, 132), dGotoEntry (312, 133), dGotoEntry (312, 134), dGotoEntry (307, 135), 
					dGotoEntry (329, 142), dGotoEntry (329, 143), dGotoEntry (329, 144)};

	const int lastToken = 300;

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
					case 14:// rule dataType : INT 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitDataType (parameter[0].m_value);}
						break;
					case 15:// rule literal : LITERAL 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitSymbol (parameter[0].m_value);}
						break;
					case 11:// rule dataDeclaration : dataType literal 
						{((dAssemblerCompiler*)this)->EmitUnInitilizedDataDeclaration (parameter[0].m_value, parameter[1].m_value);}
						break;
					case 13:// rule constantExpression : INTEGER 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitIntegerConst (parameter[0].m_value);}
						break;
					case 12:// rule dataDeclaration : dataType literal = constantExpression 
						{((dAssemblerCompiler*)this)->EmitInitilizedDataDeclaration (parameter[0].m_value, parameter[1].m_value, parameter[3].m_value);}
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





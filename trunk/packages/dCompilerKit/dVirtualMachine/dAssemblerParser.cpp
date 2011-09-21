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


bool dAssemblerParser::Parce(dAssemblerLexical& scanner)
{
	dList<dStackPair> stack;
	static short actionsCount[] = {4, 1, 1, 1, 4, 1, 4, 4, 1, 4, 4, 36, 37, 1, 5, 5, 4, 36, 10, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 0, 37, 37, 0, 37, 11, 37, 0, 37, 0, 37, 0, 37, 0, 37, 37, 0, 1, 4, 4, 4, 37, 2};
	static short actionsStart[] = {0, 4, 5, 6, 7, 11, 12, 16, 20, 21, 25, 29, 65, 102, 103, 108, 113, 117, 153, 163, 165, 167, 169, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 207, 244, 170, 281, 318, 329, 170, 366, 170, 403, 170, 440, 170, 477, 514, 170, 102, 551, 555, 559, 563, 600};
	static dActionEntry actionTable[] = {
					dActionEntry (0, 1, 0, 0, 2), dActionEntry (256, 0, 2, 0, 0), dActionEntry (259, 0, 1, 0, 0), dActionEntry (261, 0, 3, 0, 0), 
					dActionEntry (257, 0, 11, 0, 0), 
					dActionEntry (60, 0, 13, 0, 0), 
					dActionEntry (257, 1, 8, 1, 14), 
					dActionEntry (0, 1, 2, 1, 6), dActionEntry (256, 1, 2, 1, 6), dActionEntry (259, 1, 2, 1, 6), dActionEntry (261, 1, 2, 1, 6), 
					dActionEntry (257, 0, 14, 0, 0), 
					dActionEntry (0, 1, 2, 1, 7), dActionEntry (256, 1, 2, 1, 7), dActionEntry (259, 1, 2, 1, 7), dActionEntry (261, 1, 2, 1, 7), 
					dActionEntry (0, 1, 2, 1, 5), dActionEntry (256, 1, 2, 1, 5), dActionEntry (259, 1, 2, 1, 5), dActionEntry (261, 1, 2, 1, 5), 
					dActionEntry (0, 2, 0, 0, 0), 
					dActionEntry (0, 1, 1, 1, 3), dActionEntry (256, 1, 1, 1, 3), dActionEntry (259, 1, 1, 1, 3), dActionEntry (261, 1, 1, 1, 3), 
					dActionEntry (0, 1, 0, 1, 1), dActionEntry (256, 0, 2, 0, 0), dActionEntry (259, 0, 1, 0, 0), dActionEntry (261, 0, 3, 0, 0), 
					dActionEntry (264, 1, 7, 1, 15), dActionEntry (265, 1, 7, 1, 15), dActionEntry (266, 1, 7, 1, 15), dActionEntry (267, 1, 7, 1, 15), dActionEntry (268, 1, 7, 1, 15), dActionEntry (269, 1, 7, 1, 15), dActionEntry (270, 1, 7, 1, 15), dActionEntry (271, 1, 7, 1, 15), dActionEntry (272, 1, 7, 1, 15), dActionEntry (273, 1, 7, 1, 15), dActionEntry (274, 1, 7, 1, 15), dActionEntry (275, 1, 7, 1, 15), dActionEntry (276, 1, 7, 1, 15), dActionEntry (277, 1, 7, 1, 15), dActionEntry (278, 1, 7, 1, 15), dActionEntry (279, 1, 7, 1, 15), dActionEntry (280, 1, 7, 1, 15), dActionEntry (281, 1, 7, 1, 15), dActionEntry (282, 1, 7, 1, 15), dActionEntry (283, 1, 7, 1, 15), dActionEntry (284, 1, 7, 1, 15), dActionEntry (285, 1, 7, 1, 15), dActionEntry (286, 1, 7, 1, 15), dActionEntry (287, 1, 7, 1, 15), dActionEntry (288, 1, 7, 1, 15), dActionEntry (289, 1, 7, 1, 15), dActionEntry (290, 1, 7, 1, 15), dActionEntry (291, 1, 7, 1, 15), dActionEntry (292, 1, 7, 1, 15), dActionEntry (293, 1, 7, 1, 15), dActionEntry (294, 1, 7, 1, 15), dActionEntry (295, 1, 7, 1, 15), dActionEntry (296, 1, 7, 1, 15), dActionEntry (297, 1, 7, 1, 15), dActionEntry (298, 1, 7, 1, 15), dActionEntry (299, 1, 7, 1, 15), 
					dActionEntry (258, 0, 17, 0, 0), dActionEntry (264, 1, 10, 0, 18), dActionEntry (265, 1, 10, 0, 18), dActionEntry (266, 1, 10, 0, 18), dActionEntry (267, 1, 10, 0, 18), dActionEntry (268, 1, 10, 0, 18), dActionEntry (269, 1, 10, 0, 18), dActionEntry (270, 1, 10, 0, 18), dActionEntry (271, 1, 10, 0, 18), dActionEntry (272, 1, 10, 0, 18), dActionEntry (273, 1, 10, 0, 18), dActionEntry (274, 1, 10, 0, 18), dActionEntry (275, 1, 10, 0, 18), dActionEntry (276, 1, 10, 0, 18), dActionEntry (277, 1, 10, 0, 18), dActionEntry (278, 1, 10, 0, 18), dActionEntry (279, 1, 10, 0, 18), dActionEntry (280, 1, 10, 0, 18), dActionEntry (281, 1, 10, 0, 18), dActionEntry (282, 1, 10, 0, 18), dActionEntry (283, 1, 10, 0, 18), dActionEntry (284, 1, 10, 0, 18), dActionEntry (285, 1, 10, 0, 18), dActionEntry (286, 1, 10, 0, 18), dActionEntry (287, 1, 10, 0, 18), dActionEntry (288, 1, 10, 0, 18), dActionEntry (289, 1, 10, 0, 18), dActionEntry (290, 1, 10, 0, 18), dActionEntry (291, 1, 10, 0, 18), dActionEntry (292, 1, 10, 0, 18), dActionEntry (293, 1, 10, 0, 18), dActionEntry (294, 1, 10, 0, 18), dActionEntry (295, 1, 10, 0, 18), dActionEntry (296, 1, 10, 0, 18), dActionEntry (297, 1, 10, 0, 18), dActionEntry (298, 1, 10, 0, 18), dActionEntry (299, 1, 10, 0, 18), 
					dActionEntry (257, 0, 19, 0, 0), 
					dActionEntry (0, 1, 7, 1, 15), dActionEntry (61, 1, 7, 1, 15), dActionEntry (256, 1, 7, 1, 15), dActionEntry (259, 1, 7, 1, 15), dActionEntry (261, 1, 7, 1, 15), 
					dActionEntry (0, 1, 4, 2, 11), dActionEntry (61, 0, 22, 0, 0), dActionEntry (256, 1, 4, 2, 11), dActionEntry (259, 1, 4, 2, 11), dActionEntry (261, 1, 4, 2, 11), 
					dActionEntry (0, 1, 1, 2, 4), dActionEntry (256, 1, 1, 2, 4), dActionEntry (259, 1, 1, 2, 4), dActionEntry (261, 1, 1, 2, 4), 
					dActionEntry (264, 1, 10, 1, 17), dActionEntry (265, 1, 10, 1, 17), dActionEntry (266, 1, 10, 1, 17), dActionEntry (267, 1, 10, 1, 17), dActionEntry (268, 1, 10, 1, 17), dActionEntry (269, 1, 10, 1, 17), dActionEntry (270, 1, 10, 1, 17), dActionEntry (271, 1, 10, 1, 17), dActionEntry (272, 1, 10, 1, 17), dActionEntry (273, 1, 10, 1, 17), dActionEntry (274, 1, 10, 1, 17), dActionEntry (275, 1, 10, 1, 17), dActionEntry (276, 1, 10, 1, 17), dActionEntry (277, 1, 10, 1, 17), dActionEntry (278, 1, 10, 1, 17), dActionEntry (279, 1, 10, 1, 17), dActionEntry (280, 1, 10, 1, 17), dActionEntry (281, 1, 10, 1, 17), dActionEntry (282, 1, 10, 1, 17), dActionEntry (283, 1, 10, 1, 17), dActionEntry (284, 1, 10, 1, 17), dActionEntry (285, 1, 10, 1, 17), dActionEntry (286, 1, 10, 1, 17), dActionEntry (287, 1, 10, 1, 17), dActionEntry (288, 1, 10, 1, 17), dActionEntry (289, 1, 10, 1, 17), dActionEntry (290, 1, 10, 1, 17), dActionEntry (291, 1, 10, 1, 17), dActionEntry (292, 1, 10, 1, 17), dActionEntry (293, 1, 10, 1, 17), dActionEntry (294, 1, 10, 1, 17), dActionEntry (295, 1, 10, 1, 17), dActionEntry (296, 1, 10, 1, 17), dActionEntry (297, 1, 10, 1, 17), dActionEntry (298, 1, 10, 1, 17), dActionEntry (299, 1, 10, 1, 17), 
					dActionEntry (290, 0, 23, 0, 0), dActionEntry (291, 0, 24, 0, 0), dActionEntry (292, 0, 31, 0, 0), dActionEntry (293, 0, 32, 0, 0), dActionEntry (294, 0, 27, 0, 0), dActionEntry (295, 0, 28, 0, 0), dActionEntry (296, 0, 25, 0, 0), dActionEntry (297, 0, 26, 0, 0), dActionEntry (298, 0, 30, 0, 0), dActionEntry (299, 0, 29, 0, 0), 
					dActionEntry (46, 1, 7, 1, 15), dActionEntry (62, 1, 7, 1, 15), 
					dActionEntry (46, 0, 51, 0, 0), dActionEntry (62, 0, 2, 0, 0), 
					dActionEntry (46, 1, 6, 1, 9), dActionEntry (62, 1, 6, 1, 9), 
					dActionEntry (262, 0, 52, 0, 0), 
					
					dActionEntry (260, 1, 13, 1, 27), dActionEntry (264, 1, 13, 1, 27), dActionEntry (265, 1, 13, 1, 27), dActionEntry (266, 1, 13, 1, 27), dActionEntry (267, 1, 13, 1, 27), dActionEntry (268, 1, 13, 1, 27), dActionEntry (269, 1, 13, 1, 27), dActionEntry (270, 1, 13, 1, 27), dActionEntry (271, 1, 13, 1, 27), dActionEntry (272, 1, 13, 1, 27), dActionEntry (273, 1, 13, 1, 27), dActionEntry (274, 1, 13, 1, 27), dActionEntry (275, 1, 13, 1, 27), dActionEntry (276, 1, 13, 1, 27), dActionEntry (277, 1, 13, 1, 27), dActionEntry (278, 1, 13, 1, 27), dActionEntry (279, 1, 13, 1, 27), dActionEntry (280, 1, 13, 1, 27), dActionEntry (281, 1, 13, 1, 27), dActionEntry (282, 1, 13, 1, 27), dActionEntry (283, 1, 13, 1, 27), dActionEntry (284, 1, 13, 1, 27), dActionEntry (285, 1, 13, 1, 27), dActionEntry (286, 1, 13, 1, 27), dActionEntry (287, 1, 13, 1, 27), dActionEntry (288, 1, 13, 1, 27), dActionEntry (289, 1, 13, 1, 27), dActionEntry (290, 1, 13, 1, 27), dActionEntry (291, 1, 13, 1, 27), dActionEntry (292, 1, 13, 1, 27), dActionEntry (293, 1, 13, 1, 27), dActionEntry (294, 1, 13, 1, 27), dActionEntry (295, 1, 13, 1, 27), dActionEntry (296, 1, 13, 1, 27), dActionEntry (297, 1, 13, 1, 27), dActionEntry (298, 1, 13, 1, 27), dActionEntry (299, 1, 13, 1, 27), 
					dActionEntry (260, 1, 13, 1, 29), dActionEntry (264, 1, 13, 1, 29), dActionEntry (265, 1, 13, 1, 29), dActionEntry (266, 1, 13, 1, 29), dActionEntry (267, 1, 13, 1, 29), dActionEntry (268, 1, 13, 1, 29), dActionEntry (269, 1, 13, 1, 29), dActionEntry (270, 1, 13, 1, 29), dActionEntry (271, 1, 13, 1, 29), dActionEntry (272, 1, 13, 1, 29), dActionEntry (273, 1, 13, 1, 29), dActionEntry (274, 1, 13, 1, 29), dActionEntry (275, 1, 13, 1, 29), dActionEntry (276, 1, 13, 1, 29), dActionEntry (277, 1, 13, 1, 29), dActionEntry (278, 1, 13, 1, 29), dActionEntry (279, 1, 13, 1, 29), dActionEntry (280, 1, 13, 1, 29), dActionEntry (281, 1, 13, 1, 29), dActionEntry (282, 1, 13, 1, 29), dActionEntry (283, 1, 13, 1, 29), dActionEntry (284, 1, 13, 1, 29), dActionEntry (285, 1, 13, 1, 29), dActionEntry (286, 1, 13, 1, 29), dActionEntry (287, 1, 13, 1, 29), dActionEntry (288, 1, 13, 1, 29), dActionEntry (289, 1, 13, 1, 29), dActionEntry (290, 1, 13, 1, 29), dActionEntry (291, 1, 13, 1, 29), dActionEntry (292, 1, 13, 1, 29), dActionEntry (293, 1, 13, 1, 29), dActionEntry (294, 1, 13, 1, 29), dActionEntry (295, 1, 13, 1, 29), dActionEntry (296, 1, 13, 1, 29), dActionEntry (297, 1, 13, 1, 29), dActionEntry (298, 1, 13, 1, 29), dActionEntry (299, 1, 13, 1, 29), 
					dActionEntry (260, 1, 13, 1, 28), dActionEntry (264, 1, 13, 1, 28), dActionEntry (265, 1, 13, 1, 28), dActionEntry (266, 1, 13, 1, 28), dActionEntry (267, 1, 13, 1, 28), dActionEntry (268, 1, 13, 1, 28), dActionEntry (269, 1, 13, 1, 28), dActionEntry (270, 1, 13, 1, 28), dActionEntry (271, 1, 13, 1, 28), dActionEntry (272, 1, 13, 1, 28), dActionEntry (273, 1, 13, 1, 28), dActionEntry (274, 1, 13, 1, 28), dActionEntry (275, 1, 13, 1, 28), dActionEntry (276, 1, 13, 1, 28), dActionEntry (277, 1, 13, 1, 28), dActionEntry (278, 1, 13, 1, 28), dActionEntry (279, 1, 13, 1, 28), dActionEntry (280, 1, 13, 1, 28), dActionEntry (281, 1, 13, 1, 28), dActionEntry (282, 1, 13, 1, 28), dActionEntry (283, 1, 13, 1, 28), dActionEntry (284, 1, 13, 1, 28), dActionEntry (285, 1, 13, 1, 28), dActionEntry (286, 1, 13, 1, 28), dActionEntry (287, 1, 13, 1, 28), dActionEntry (288, 1, 13, 1, 28), dActionEntry (289, 1, 13, 1, 28), dActionEntry (290, 1, 13, 1, 28), dActionEntry (291, 1, 13, 1, 28), dActionEntry (292, 1, 13, 1, 28), dActionEntry (293, 1, 13, 1, 28), dActionEntry (294, 1, 13, 1, 28), dActionEntry (295, 1, 13, 1, 28), dActionEntry (296, 1, 13, 1, 28), dActionEntry (297, 1, 13, 1, 28), dActionEntry (298, 1, 13, 1, 28), dActionEntry (299, 1, 13, 1, 28), 
					dActionEntry (260, 1, 11, 1, 20), dActionEntry (264, 1, 11, 1, 20), dActionEntry (265, 1, 11, 1, 20), dActionEntry (266, 1, 11, 1, 20), dActionEntry (267, 1, 11, 1, 20), dActionEntry (268, 1, 11, 1, 20), dActionEntry (269, 1, 11, 1, 20), dActionEntry (270, 1, 11, 1, 20), dActionEntry (271, 1, 11, 1, 20), dActionEntry (272, 1, 11, 1, 20), dActionEntry (273, 1, 11, 1, 20), dActionEntry (274, 1, 11, 1, 20), dActionEntry (275, 1, 11, 1, 20), dActionEntry (276, 1, 11, 1, 20), dActionEntry (277, 1, 11, 1, 20), dActionEntry (278, 1, 11, 1, 20), dActionEntry (279, 1, 11, 1, 20), dActionEntry (280, 1, 11, 1, 20), dActionEntry (281, 1, 11, 1, 20), dActionEntry (282, 1, 11, 1, 20), dActionEntry (283, 1, 11, 1, 20), dActionEntry (284, 1, 11, 1, 20), dActionEntry (285, 1, 11, 1, 20), dActionEntry (286, 1, 11, 1, 20), dActionEntry (287, 1, 11, 1, 20), dActionEntry (288, 1, 11, 1, 20), dActionEntry (289, 1, 11, 1, 20), dActionEntry (290, 1, 11, 1, 20), dActionEntry (291, 1, 11, 1, 20), dActionEntry (292, 1, 11, 1, 20), dActionEntry (293, 1, 11, 1, 20), dActionEntry (294, 1, 11, 1, 20), dActionEntry (295, 1, 11, 1, 20), dActionEntry (296, 1, 11, 1, 20), dActionEntry (297, 1, 11, 1, 20), dActionEntry (298, 1, 11, 1, 20), dActionEntry (299, 1, 11, 1, 20), 
					dActionEntry (260, 0, 54, 0, 0), dActionEntry (290, 0, 23, 0, 0), dActionEntry (291, 0, 24, 0, 0), dActionEntry (292, 0, 31, 0, 0), dActionEntry (293, 0, 32, 0, 0), dActionEntry (294, 0, 27, 0, 0), dActionEntry (295, 0, 28, 0, 0), dActionEntry (296, 0, 25, 0, 0), dActionEntry (297, 0, 26, 0, 0), dActionEntry (298, 0, 30, 0, 0), dActionEntry (299, 0, 29, 0, 0), 
					dActionEntry (260, 1, 13, 1, 23), dActionEntry (264, 1, 13, 1, 23), dActionEntry (265, 1, 13, 1, 23), dActionEntry (266, 1, 13, 1, 23), dActionEntry (267, 1, 13, 1, 23), dActionEntry (268, 1, 13, 1, 23), dActionEntry (269, 1, 13, 1, 23), dActionEntry (270, 1, 13, 1, 23), dActionEntry (271, 1, 13, 1, 23), dActionEntry (272, 1, 13, 1, 23), dActionEntry (273, 1, 13, 1, 23), dActionEntry (274, 1, 13, 1, 23), dActionEntry (275, 1, 13, 1, 23), dActionEntry (276, 1, 13, 1, 23), dActionEntry (277, 1, 13, 1, 23), dActionEntry (278, 1, 13, 1, 23), dActionEntry (279, 1, 13, 1, 23), dActionEntry (280, 1, 13, 1, 23), dActionEntry (281, 1, 13, 1, 23), dActionEntry (282, 1, 13, 1, 23), dActionEntry (283, 1, 13, 1, 23), dActionEntry (284, 1, 13, 1, 23), dActionEntry (285, 1, 13, 1, 23), dActionEntry (286, 1, 13, 1, 23), dActionEntry (287, 1, 13, 1, 23), dActionEntry (288, 1, 13, 1, 23), dActionEntry (289, 1, 13, 1, 23), dActionEntry (290, 1, 13, 1, 23), dActionEntry (291, 1, 13, 1, 23), dActionEntry (292, 1, 13, 1, 23), dActionEntry (293, 1, 13, 1, 23), dActionEntry (294, 1, 13, 1, 23), dActionEntry (295, 1, 13, 1, 23), dActionEntry (296, 1, 13, 1, 23), dActionEntry (297, 1, 13, 1, 23), dActionEntry (298, 1, 13, 1, 23), dActionEntry (299, 1, 13, 1, 23), 
					dActionEntry (260, 1, 13, 1, 26), dActionEntry (264, 1, 13, 1, 26), dActionEntry (265, 1, 13, 1, 26), dActionEntry (266, 1, 13, 1, 26), dActionEntry (267, 1, 13, 1, 26), dActionEntry (268, 1, 13, 1, 26), dActionEntry (269, 1, 13, 1, 26), dActionEntry (270, 1, 13, 1, 26), dActionEntry (271, 1, 13, 1, 26), dActionEntry (272, 1, 13, 1, 26), dActionEntry (273, 1, 13, 1, 26), dActionEntry (274, 1, 13, 1, 26), dActionEntry (275, 1, 13, 1, 26), dActionEntry (276, 1, 13, 1, 26), dActionEntry (277, 1, 13, 1, 26), dActionEntry (278, 1, 13, 1, 26), dActionEntry (279, 1, 13, 1, 26), dActionEntry (280, 1, 13, 1, 26), dActionEntry (281, 1, 13, 1, 26), dActionEntry (282, 1, 13, 1, 26), dActionEntry (283, 1, 13, 1, 26), dActionEntry (284, 1, 13, 1, 26), dActionEntry (285, 1, 13, 1, 26), dActionEntry (286, 1, 13, 1, 26), dActionEntry (287, 1, 13, 1, 26), dActionEntry (288, 1, 13, 1, 26), dActionEntry (289, 1, 13, 1, 26), dActionEntry (290, 1, 13, 1, 26), dActionEntry (291, 1, 13, 1, 26), dActionEntry (292, 1, 13, 1, 26), dActionEntry (293, 1, 13, 1, 26), dActionEntry (294, 1, 13, 1, 26), dActionEntry (295, 1, 13, 1, 26), dActionEntry (296, 1, 13, 1, 26), dActionEntry (297, 1, 13, 1, 26), dActionEntry (298, 1, 13, 1, 26), dActionEntry (299, 1, 13, 1, 26), 
					dActionEntry (260, 1, 13, 1, 22), dActionEntry (264, 1, 13, 1, 22), dActionEntry (265, 1, 13, 1, 22), dActionEntry (266, 1, 13, 1, 22), dActionEntry (267, 1, 13, 1, 22), dActionEntry (268, 1, 13, 1, 22), dActionEntry (269, 1, 13, 1, 22), dActionEntry (270, 1, 13, 1, 22), dActionEntry (271, 1, 13, 1, 22), dActionEntry (272, 1, 13, 1, 22), dActionEntry (273, 1, 13, 1, 22), dActionEntry (274, 1, 13, 1, 22), dActionEntry (275, 1, 13, 1, 22), dActionEntry (276, 1, 13, 1, 22), dActionEntry (277, 1, 13, 1, 22), dActionEntry (278, 1, 13, 1, 22), dActionEntry (279, 1, 13, 1, 22), dActionEntry (280, 1, 13, 1, 22), dActionEntry (281, 1, 13, 1, 22), dActionEntry (282, 1, 13, 1, 22), dActionEntry (283, 1, 13, 1, 22), dActionEntry (284, 1, 13, 1, 22), dActionEntry (285, 1, 13, 1, 22), dActionEntry (286, 1, 13, 1, 22), dActionEntry (287, 1, 13, 1, 22), dActionEntry (288, 1, 13, 1, 22), dActionEntry (289, 1, 13, 1, 22), dActionEntry (290, 1, 13, 1, 22), dActionEntry (291, 1, 13, 1, 22), dActionEntry (292, 1, 13, 1, 22), dActionEntry (293, 1, 13, 1, 22), dActionEntry (294, 1, 13, 1, 22), dActionEntry (295, 1, 13, 1, 22), dActionEntry (296, 1, 13, 1, 22), dActionEntry (297, 1, 13, 1, 22), dActionEntry (298, 1, 13, 1, 22), dActionEntry (299, 1, 13, 1, 22), 
					dActionEntry (260, 1, 13, 1, 25), dActionEntry (264, 1, 13, 1, 25), dActionEntry (265, 1, 13, 1, 25), dActionEntry (266, 1, 13, 1, 25), dActionEntry (267, 1, 13, 1, 25), dActionEntry (268, 1, 13, 1, 25), dActionEntry (269, 1, 13, 1, 25), dActionEntry (270, 1, 13, 1, 25), dActionEntry (271, 1, 13, 1, 25), dActionEntry (272, 1, 13, 1, 25), dActionEntry (273, 1, 13, 1, 25), dActionEntry (274, 1, 13, 1, 25), dActionEntry (275, 1, 13, 1, 25), dActionEntry (276, 1, 13, 1, 25), dActionEntry (277, 1, 13, 1, 25), dActionEntry (278, 1, 13, 1, 25), dActionEntry (279, 1, 13, 1, 25), dActionEntry (280, 1, 13, 1, 25), dActionEntry (281, 1, 13, 1, 25), dActionEntry (282, 1, 13, 1, 25), dActionEntry (283, 1, 13, 1, 25), dActionEntry (284, 1, 13, 1, 25), dActionEntry (285, 1, 13, 1, 25), dActionEntry (286, 1, 13, 1, 25), dActionEntry (287, 1, 13, 1, 25), dActionEntry (288, 1, 13, 1, 25), dActionEntry (289, 1, 13, 1, 25), dActionEntry (290, 1, 13, 1, 25), dActionEntry (291, 1, 13, 1, 25), dActionEntry (292, 1, 13, 1, 25), dActionEntry (293, 1, 13, 1, 25), dActionEntry (294, 1, 13, 1, 25), dActionEntry (295, 1, 13, 1, 25), dActionEntry (296, 1, 13, 1, 25), dActionEntry (297, 1, 13, 1, 25), dActionEntry (298, 1, 13, 1, 25), dActionEntry (299, 1, 13, 1, 25), 
					dActionEntry (260, 1, 13, 1, 30), dActionEntry (264, 1, 13, 1, 30), dActionEntry (265, 1, 13, 1, 30), dActionEntry (266, 1, 13, 1, 30), dActionEntry (267, 1, 13, 1, 30), dActionEntry (268, 1, 13, 1, 30), dActionEntry (269, 1, 13, 1, 30), dActionEntry (270, 1, 13, 1, 30), dActionEntry (271, 1, 13, 1, 30), dActionEntry (272, 1, 13, 1, 30), dActionEntry (273, 1, 13, 1, 30), dActionEntry (274, 1, 13, 1, 30), dActionEntry (275, 1, 13, 1, 30), dActionEntry (276, 1, 13, 1, 30), dActionEntry (277, 1, 13, 1, 30), dActionEntry (278, 1, 13, 1, 30), dActionEntry (279, 1, 13, 1, 30), dActionEntry (280, 1, 13, 1, 30), dActionEntry (281, 1, 13, 1, 30), dActionEntry (282, 1, 13, 1, 30), dActionEntry (283, 1, 13, 1, 30), dActionEntry (284, 1, 13, 1, 30), dActionEntry (285, 1, 13, 1, 30), dActionEntry (286, 1, 13, 1, 30), dActionEntry (287, 1, 13, 1, 30), dActionEntry (288, 1, 13, 1, 30), dActionEntry (289, 1, 13, 1, 30), dActionEntry (290, 1, 13, 1, 30), dActionEntry (291, 1, 13, 1, 30), dActionEntry (292, 1, 13, 1, 30), dActionEntry (293, 1, 13, 1, 30), dActionEntry (294, 1, 13, 1, 30), dActionEntry (295, 1, 13, 1, 30), dActionEntry (296, 1, 13, 1, 30), dActionEntry (297, 1, 13, 1, 30), dActionEntry (298, 1, 13, 1, 30), dActionEntry (299, 1, 13, 1, 30), 
					dActionEntry (260, 1, 13, 1, 24), dActionEntry (264, 1, 13, 1, 24), dActionEntry (265, 1, 13, 1, 24), dActionEntry (266, 1, 13, 1, 24), dActionEntry (267, 1, 13, 1, 24), dActionEntry (268, 1, 13, 1, 24), dActionEntry (269, 1, 13, 1, 24), dActionEntry (270, 1, 13, 1, 24), dActionEntry (271, 1, 13, 1, 24), dActionEntry (272, 1, 13, 1, 24), dActionEntry (273, 1, 13, 1, 24), dActionEntry (274, 1, 13, 1, 24), dActionEntry (275, 1, 13, 1, 24), dActionEntry (276, 1, 13, 1, 24), dActionEntry (277, 1, 13, 1, 24), dActionEntry (278, 1, 13, 1, 24), dActionEntry (279, 1, 13, 1, 24), dActionEntry (280, 1, 13, 1, 24), dActionEntry (281, 1, 13, 1, 24), dActionEntry (282, 1, 13, 1, 24), dActionEntry (283, 1, 13, 1, 24), dActionEntry (284, 1, 13, 1, 24), dActionEntry (285, 1, 13, 1, 24), dActionEntry (286, 1, 13, 1, 24), dActionEntry (287, 1, 13, 1, 24), dActionEntry (288, 1, 13, 1, 24), dActionEntry (289, 1, 13, 1, 24), dActionEntry (290, 1, 13, 1, 24), dActionEntry (291, 1, 13, 1, 24), dActionEntry (292, 1, 13, 1, 24), dActionEntry (293, 1, 13, 1, 24), dActionEntry (294, 1, 13, 1, 24), dActionEntry (295, 1, 13, 1, 24), dActionEntry (296, 1, 13, 1, 24), dActionEntry (297, 1, 13, 1, 24), dActionEntry (298, 1, 13, 1, 24), dActionEntry (299, 1, 13, 1, 24), 
					dActionEntry (0, 1, 9, 1, 13), dActionEntry (256, 1, 9, 1, 13), dActionEntry (259, 1, 9, 1, 13), dActionEntry (261, 1, 9, 1, 13), 
					dActionEntry (0, 1, 4, 4, 12), dActionEntry (256, 1, 4, 4, 12), dActionEntry (259, 1, 4, 4, 12), dActionEntry (261, 1, 4, 4, 12), 
					dActionEntry (0, 1, 5, 5, 16), dActionEntry (256, 1, 5, 5, 16), dActionEntry (259, 1, 5, 5, 16), dActionEntry (261, 1, 5, 5, 16), 
					dActionEntry (260, 1, 11, 2, 21), dActionEntry (264, 1, 11, 2, 21), dActionEntry (265, 1, 11, 2, 21), dActionEntry (266, 1, 11, 2, 21), dActionEntry (267, 1, 11, 2, 21), dActionEntry (268, 1, 11, 2, 21), dActionEntry (269, 1, 11, 2, 21), dActionEntry (270, 1, 11, 2, 21), dActionEntry (271, 1, 11, 2, 21), dActionEntry (272, 1, 11, 2, 21), dActionEntry (273, 1, 11, 2, 21), dActionEntry (274, 1, 11, 2, 21), dActionEntry (275, 1, 11, 2, 21), dActionEntry (276, 1, 11, 2, 21), dActionEntry (277, 1, 11, 2, 21), dActionEntry (278, 1, 11, 2, 21), dActionEntry (279, 1, 11, 2, 21), dActionEntry (280, 1, 11, 2, 21), dActionEntry (281, 1, 11, 2, 21), dActionEntry (282, 1, 11, 2, 21), dActionEntry (283, 1, 11, 2, 21), dActionEntry (284, 1, 11, 2, 21), dActionEntry (285, 1, 11, 2, 21), dActionEntry (286, 1, 11, 2, 21), dActionEntry (287, 1, 11, 2, 21), dActionEntry (288, 1, 11, 2, 21), dActionEntry (289, 1, 11, 2, 21), dActionEntry (290, 1, 11, 2, 21), dActionEntry (291, 1, 11, 2, 21), dActionEntry (292, 1, 11, 2, 21), dActionEntry (293, 1, 11, 2, 21), dActionEntry (294, 1, 11, 2, 21), dActionEntry (295, 1, 11, 2, 21), dActionEntry (296, 1, 11, 2, 21), dActionEntry (297, 1, 11, 2, 21), dActionEntry (298, 1, 11, 2, 21), dActionEntry (299, 1, 11, 2, 21), 
					dActionEntry (46, 1, 6, 3, 10), dActionEntry (62, 1, 6, 3, 10), 
			};

	static short gotoCount[] = {7, 1, 0, 0, 0, 1, 0, 0, 0, 0, 5, 0, 1, 2, 0, 0, 0, 0, 18, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 17, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0};
	static short gotoStart[] = {0, 7, 8, 8, 8, 8, 9, 9, 9, 9, 9, 14, 14, 15, 17, 17, 17, 17, 17, 35, 35, 35, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 46, 47, 47, 47, 48, 48, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 69, 70, 71, 71, 71, 71, 71};
	static dGotoEntry gotoTable[] = {
					dGotoEntry (300, 8), dGotoEntry (301, 10), dGotoEntry (302, 9), dGotoEntry (303, 7), dGotoEntry (304, 4), dGotoEntry (305, 6), dGotoEntry (308, 5), 
					dGotoEntry (307, 12), dGotoEntry (307, 15), dGotoEntry (302, 16), dGotoEntry (303, 7), dGotoEntry (304, 4), dGotoEntry (305, 6), dGotoEntry (308, 5), 
					dGotoEntry (310, 18), dGotoEntry (306, 20), dGotoEntry (307, 21), dGotoEntry (311, 39), dGotoEntry (313, 38), dGotoEntry (314, 44), dGotoEntry (315, 40), dGotoEntry (316, 49), dGotoEntry (317, 46), dGotoEntry (318, 42), dGotoEntry (319, 33), dGotoEntry (320, 36), dGotoEntry (321, 35), dGotoEntry (322, 48), dGotoEntry (323, 45), dGotoEntry (326, 41), dGotoEntry (327, 50), dGotoEntry (328, 47), dGotoEntry (329, 43), dGotoEntry (330, 34), dGotoEntry (331, 37), 
					dGotoEntry (309, 53), dGotoEntry (324, 23), dGotoEntry (324, 24), dGotoEntry (324, 25), 
					dGotoEntry (324, 26), dGotoEntry (324, 27), dGotoEntry (325, 28), dGotoEntry (324, 29), 
					dGotoEntry (324, 30), dGotoEntry (324, 31), dGotoEntry (324, 32), dGotoEntry (324, 34), 
					dGotoEntry (324, 37), dGotoEntry (313, 55), dGotoEntry (314, 44), dGotoEntry (315, 40), dGotoEntry (316, 49), dGotoEntry (317, 46), dGotoEntry (318, 42), dGotoEntry (319, 33), dGotoEntry (320, 36), dGotoEntry (321, 35), dGotoEntry (322, 48), dGotoEntry (323, 45), dGotoEntry (326, 41), dGotoEntry (327, 50), dGotoEntry (328, 47), dGotoEntry (329, 43), dGotoEntry (330, 34), dGotoEntry (331, 37), 
					dGotoEntry (324, 41), dGotoEntry (324, 43), dGotoEntry (324, 45), dGotoEntry (324, 47), 
					dGotoEntry (324, 50), dGotoEntry (307, 56)};

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
					token = dToken (0);
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





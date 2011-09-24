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
//Auto generated Parser Generator class: dAssemblerParser.cpp
//


#include <dAssemblerCompiler.h>
#include "dAssemblerLexical.h"

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
			4, 1, 1, 1, 6, 4, 1, 4, 4, 1, 4, 4, 6, 7, 1, 1, 2, 8, 2, 2, 8, 7, 7, 7, 
			7, 7, 1, 8, 2, 2, 5, 5, 4, 6, 6, 2, 2, 2, 4, 7, 1, 1, 7, 7, 7, 7, 1, 9, 
			9, 1, 1, 1, 4, 3, 1, 1, 1, 2, 2, 2, 4, 4, 2, 7, 7, 7, 1, 7, 1, 7, 1, 7, 
			2, 2, 2, 7, 7, 7};
	static short actionsStart[] = {
			0, 4, 5, 6, 7, 13, 17, 18, 22, 26, 27, 31, 35, 41, 48, 49, 50, 52, 60, 62, 64, 72, 79, 86, 
			93, 100, 107, 108, 116, 118, 120, 125, 130, 134, 140, 146, 148, 150, 152, 156, 163, 164, 165, 172, 179, 186, 193, 194, 
			203, 212, 213, 48, 214, 218, 221, 222, 223, 224, 226, 228, 230, 234, 238, 240, 247, 254, 261, 262, 212, 269, 212, 276, 
			283, 285, 287, 289, 296, 303};
	static short gotoCount[] = {
			8, 1, 0, 0, 9, 0, 1, 0, 0, 0, 0, 6, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1, 1, 2, 1, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 1, 0, 3, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 0, 0, 0, 0};
	static short gotoStart[] = {
			0, 8, 9, 9, 9, 18, 18, 19, 19, 19, 19, 19, 25, 25, 26, 28, 28, 28, 28, 28, 28, 28, 28, 36, 36, 36, 36, 37, 38, 40, 41, 41, 
			41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 42, 44, 45, 46, 46, 49, 50, 50, 51, 51, 51, 51, 51, 51, 51, 
			51, 51, 51, 51, 51, 53, 53, 54, 54, 56, 56, 56, 56, 56};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (301, 9), dGotoEntry (302, 11), dGotoEntry (303, 10), dGotoEntry (304, 8), dGotoEntry (305, 5), 
			dGotoEntry (306, 7), dGotoEntry (309, 6), dGotoEntry (312, 4), dGotoEntry (308, 13), dGotoEntry (313, 22), 
			dGotoEntry (314, 21), dGotoEntry (315, 23), dGotoEntry (316, 24), dGotoEntry (317, 25), dGotoEntry (318, 27), 
			dGotoEntry (319, 28), dGotoEntry (320, 29), dGotoEntry (321, 26), dGotoEntry (308, 31), dGotoEntry (303, 32), 
			dGotoEntry (304, 8), dGotoEntry (305, 5), dGotoEntry (306, 7), dGotoEntry (309, 6), dGotoEntry (312, 4), 
			dGotoEntry (311, 34), dGotoEntry (307, 36), dGotoEntry (308, 37), dGotoEntry (314, 39), dGotoEntry (315, 23), 
			dGotoEntry (316, 24), dGotoEntry (317, 25), dGotoEntry (318, 27), dGotoEntry (319, 28), dGotoEntry (320, 29), 
			dGotoEntry (321, 26), dGotoEntry (323, 41), dGotoEntry (323, 43), dGotoEntry (308, 45), dGotoEntry (323, 46), 
			dGotoEntry (323, 48), dGotoEntry (323, 56), dGotoEntry (323, 58), dGotoEntry (324, 59), dGotoEntry (310, 61), 
			dGotoEntry (308, 62), dGotoEntry (308, 65), dGotoEntry (310, 64), dGotoEntry (323, 66), dGotoEntry (308, 67), 
			dGotoEntry (323, 69), dGotoEntry (323, 58), dGotoEntry (324, 73), dGotoEntry (323, 74), dGotoEntry (308, 76), 
			dGotoEntry (310, 75)};
	static dActionEntry actionTable[] = {
			dActionEntry (255, 1, 0, 0, 2), dActionEntry (256, 0, 2, 0, 0), dActionEntry (259, 0, 1, 0, 0), dActionEntry (261, 0, 3, 0, 0), 
			dActionEntry (257, 0, 12, 0, 0), dActionEntry (60, 0, 14, 0, 0), dActionEntry (257, 1, 8, 1, 13), dActionEntry (277, 0, 15, 0, 0), 
			dActionEntry (290, 0, 16, 0, 0), dActionEntry (292, 0, 20, 0, 0), dActionEntry (295, 0, 17, 0, 0), dActionEntry (298, 0, 19, 0, 0), 
			dActionEntry (299, 0, 18, 0, 0), dActionEntry (255, 1, 2, 1, 6), dActionEntry (256, 1, 2, 1, 6), dActionEntry (259, 1, 2, 1, 6), 
			dActionEntry (261, 1, 2, 1, 6), dActionEntry (257, 0, 30, 0, 0), dActionEntry (255, 1, 2, 1, 7), dActionEntry (256, 1, 2, 1, 7), 
			dActionEntry (259, 1, 2, 1, 7), dActionEntry (261, 1, 2, 1, 7), dActionEntry (255, 1, 2, 1, 5), dActionEntry (256, 1, 2, 1, 5), 
			dActionEntry (259, 1, 2, 1, 5), dActionEntry (261, 1, 2, 1, 5), dActionEntry (255, 2, 0, 0, 0), dActionEntry (255, 1, 1, 1, 3), 
			dActionEntry (256, 1, 1, 1, 3), dActionEntry (259, 1, 1, 1, 3), dActionEntry (261, 1, 1, 1, 3), dActionEntry (255, 1, 0, 1, 1), 
			dActionEntry (256, 0, 2, 0, 0), dActionEntry (259, 0, 1, 0, 0), dActionEntry (261, 0, 3, 0, 0), dActionEntry (277, 1, 7, 1, 14), 
			dActionEntry (290, 1, 7, 1, 14), dActionEntry (292, 1, 7, 1, 14), dActionEntry (295, 1, 7, 1, 14), dActionEntry (298, 1, 7, 1, 14), 
			dActionEntry (299, 1, 7, 1, 14), dActionEntry (258, 0, 33, 0, 0), dActionEntry (277, 1, 10, 0, 16), dActionEntry (290, 1, 10, 0, 16), 
			dActionEntry (292, 1, 10, 0, 16), dActionEntry (295, 1, 10, 0, 16), dActionEntry (298, 1, 10, 0, 16), dActionEntry (299, 1, 10, 0, 16), 
			dActionEntry (257, 0, 35, 0, 0), dActionEntry (263, 1, 20, 1, 29), dActionEntry (257, 1, 18, 1, 26), dActionEntry (263, 1, 18, 1, 26), 
			dActionEntry (260, 1, 17, 1, 24), dActionEntry (263, 1, 17, 1, 24), dActionEntry (277, 1, 17, 1, 24), dActionEntry (290, 1, 17, 1, 24), 
			dActionEntry (292, 1, 17, 1, 24), dActionEntry (295, 1, 17, 1, 24), dActionEntry (298, 1, 17, 1, 24), dActionEntry (299, 1, 17, 1, 24), 
			dActionEntry (123, 1, 19, 1, 28), dActionEntry (263, 1, 19, 1, 28), dActionEntry (123, 1, 19, 1, 27), dActionEntry (263, 1, 19, 1, 27), 
			dActionEntry (260, 1, 17, 1, 25), dActionEntry (263, 1, 17, 1, 25), dActionEntry (277, 1, 17, 1, 25), dActionEntry (290, 1, 17, 1, 25), 
			dActionEntry (292, 1, 17, 1, 25), dActionEntry (295, 1, 17, 1, 25), dActionEntry (298, 1, 17, 1, 25), dActionEntry (299, 1, 17, 1, 25), 
			dActionEntry (260, 1, 12, 1, 19), dActionEntry (277, 1, 12, 1, 19), dActionEntry (290, 1, 12, 1, 19), dActionEntry (292, 1, 12, 1, 19), 
			dActionEntry (295, 1, 12, 1, 19), dActionEntry (298, 1, 12, 1, 19), dActionEntry (299, 1, 12, 1, 19), dActionEntry (260, 0, 38, 0, 0), 
			dActionEntry (277, 0, 15, 0, 0), dActionEntry (290, 0, 16, 0, 0), dActionEntry (292, 0, 20, 0, 0), dActionEntry (295, 0, 17, 0, 0), 
			dActionEntry (298, 0, 19, 0, 0), dActionEntry (299, 0, 18, 0, 0), dActionEntry (260, 1, 13, 1, 21), dActionEntry (277, 1, 13, 1, 21), 
			dActionEntry (290, 1, 13, 1, 21), dActionEntry (292, 1, 13, 1, 21), dActionEntry (295, 1, 13, 1, 21), dActionEntry (298, 1, 13, 1, 21), 
			dActionEntry (299, 1, 13, 1, 21), dActionEntry (260, 1, 13, 1, 22), dActionEntry (277, 1, 13, 1, 22), dActionEntry (290, 1, 13, 1, 22), 
			dActionEntry (292, 1, 13, 1, 22), dActionEntry (295, 1, 13, 1, 22), dActionEntry (298, 1, 13, 1, 22), dActionEntry (299, 1, 13, 1, 22), 
			dActionEntry (260, 1, 13, 1, 23), dActionEntry (277, 1, 13, 1, 23), dActionEntry (290, 1, 13, 1, 23), dActionEntry (292, 1, 13, 1, 23), 
			dActionEntry (295, 1, 13, 1, 23), dActionEntry (298, 1, 13, 1, 23), dActionEntry (299, 1, 13, 1, 23), dActionEntry (263, 0, 40, 0, 0), 
			dActionEntry (260, 1, 14, 1, 40), dActionEntry (263, 0, 42, 0, 0), dActionEntry (277, 1, 14, 1, 40), dActionEntry (290, 1, 14, 1, 40), 
			dActionEntry (292, 1, 14, 1, 40), dActionEntry (295, 1, 14, 1, 40), dActionEntry (298, 1, 14, 1, 40), dActionEntry (299, 1, 14, 1, 40), 
			dActionEntry (257, 0, 44, 0, 0), dActionEntry (263, 0, 40, 0, 0), dActionEntry (123, 0, 49, 0, 0), dActionEntry (263, 0, 47, 0, 0), 
			dActionEntry (61, 1, 7, 1, 14), dActionEntry (255, 1, 7, 1, 14), dActionEntry (256, 1, 7, 1, 14), dActionEntry (259, 1, 7, 1, 14), 
			dActionEntry (261, 1, 7, 1, 14), dActionEntry (61, 0, 50, 0, 0), dActionEntry (255, 1, 4, 2, 11), dActionEntry (256, 1, 4, 2, 11), 
			dActionEntry (259, 1, 4, 2, 11), dActionEntry (261, 1, 4, 2, 11), dActionEntry (255, 1, 1, 2, 4), dActionEntry (256, 1, 1, 2, 4), 
			dActionEntry (259, 1, 1, 2, 4), dActionEntry (261, 1, 1, 2, 4), dActionEntry (277, 1, 10, 1, 15), dActionEntry (290, 1, 10, 1, 15), 
			dActionEntry (292, 1, 10, 1, 15), dActionEntry (295, 1, 10, 1, 15), dActionEntry (298, 1, 10, 1, 15), dActionEntry (299, 1, 10, 1, 15), 
			dActionEntry (277, 1, 11, 3, 18), dActionEntry (290, 1, 11, 3, 18), dActionEntry (292, 1, 11, 3, 18), dActionEntry (295, 1, 11, 3, 18), 
			dActionEntry (298, 1, 11, 3, 18), dActionEntry (299, 1, 11, 3, 18), dActionEntry (46, 1, 7, 1, 14), dActionEntry (62, 1, 7, 1, 14), 
			dActionEntry (46, 0, 51, 0, 0), dActionEntry (62, 0, 52, 0, 0), dActionEntry (46, 1, 6, 1, 9), dActionEntry (62, 1, 6, 1, 9), 
			dActionEntry (255, 1, 5, 3, 17), dActionEntry (256, 1, 5, 3, 17), dActionEntry (259, 1, 5, 3, 17), dActionEntry (261, 1, 5, 3, 17), 
			dActionEntry (260, 1, 12, 2, 20), dActionEntry (277, 1, 12, 2, 20), dActionEntry (290, 1, 12, 2, 20), dActionEntry (292, 1, 12, 2, 20), 
			dActionEntry (295, 1, 12, 2, 20), dActionEntry (298, 1, 12, 2, 20), dActionEntry (299, 1, 12, 2, 20), dActionEntry (44, 1, 22, 1, 37), 
			dActionEntry (44, 0, 53, 0, 0), dActionEntry (260, 1, 22, 1, 37), dActionEntry (277, 1, 22, 1, 37), dActionEntry (290, 1, 22, 1, 37), 
			dActionEntry (292, 1, 22, 1, 37), dActionEntry (295, 1, 22, 1, 37), dActionEntry (298, 1, 22, 1, 37), dActionEntry (299, 1, 22, 1, 37), 
			dActionEntry (260, 1, 14, 2, 41), dActionEntry (277, 1, 14, 2, 41), dActionEntry (290, 1, 14, 2, 41), dActionEntry (292, 1, 14, 2, 41), 
			dActionEntry (295, 1, 14, 2, 41), dActionEntry (298, 1, 14, 2, 41), dActionEntry (299, 1, 14, 2, 41), dActionEntry (260, 1, 7, 1, 14), 
			dActionEntry (277, 1, 7, 1, 14), dActionEntry (290, 1, 7, 1, 14), dActionEntry (292, 1, 7, 1, 14), dActionEntry (295, 1, 7, 1, 14), 
			dActionEntry (298, 1, 7, 1, 14), dActionEntry (299, 1, 7, 1, 14), dActionEntry (260, 1, 15, 2, 46), dActionEntry (277, 1, 15, 2, 46), 
			dActionEntry (290, 1, 15, 2, 46), dActionEntry (292, 1, 15, 2, 46), dActionEntry (295, 1, 15, 2, 46), dActionEntry (298, 1, 15, 2, 46), 
			dActionEntry (299, 1, 15, 2, 46), dActionEntry (44, 0, 54, 0, 0), dActionEntry (44, 1, 22, 1, 37), dActionEntry (260, 1, 22, 1, 37), 
			dActionEntry (263, 1, 22, 1, 37), dActionEntry (277, 1, 22, 1, 37), dActionEntry (290, 1, 22, 1, 37), dActionEntry (292, 1, 22, 1, 37), 
			dActionEntry (295, 1, 22, 1, 37), dActionEntry (298, 1, 22, 1, 37), dActionEntry (299, 1, 22, 1, 37), dActionEntry (44, 0, 55, 0, 0), 
			dActionEntry (260, 1, 15, 2, 44), dActionEntry (263, 0, 40, 0, 0), dActionEntry (277, 1, 15, 2, 44), dActionEntry (290, 1, 15, 2, 44), 
			dActionEntry (292, 1, 15, 2, 44), dActionEntry (295, 1, 15, 2, 44), dActionEntry (298, 1, 15, 2, 44), dActionEntry (299, 1, 15, 2, 44), 
			dActionEntry (263, 0, 57, 0, 0), dActionEntry (262, 0, 60, 0, 0), dActionEntry (255, 1, 3, 4, 8), dActionEntry (256, 1, 3, 4, 8), 
			dActionEntry (259, 1, 3, 4, 8), dActionEntry (261, 1, 3, 4, 8), dActionEntry (257, 0, 44, 0, 0), dActionEntry (262, 0, 63, 0, 0), 
			dActionEntry (263, 0, 40, 0, 0), dActionEntry (257, 0, 44, 0, 0), dActionEntry (123, 0, 68, 0, 0), dActionEntry (263, 0, 42, 0, 0), 
			dActionEntry (44, 1, 22, 1, 37), dActionEntry (125, 1, 22, 1, 37), dActionEntry (44, 1, 23, 1, 38), dActionEntry (125, 1, 23, 1, 38), 
			dActionEntry (44, 0, 70, 0, 0), dActionEntry (125, 0, 71, 0, 0), dActionEntry (255, 1, 9, 1, 36), dActionEntry (256, 1, 9, 1, 36), 
			dActionEntry (259, 1, 9, 1, 36), dActionEntry (261, 1, 9, 1, 36), dActionEntry (255, 1, 4, 4, 12), dActionEntry (256, 1, 4, 4, 12), 
			dActionEntry (259, 1, 4, 4, 12), dActionEntry (261, 1, 4, 4, 12), dActionEntry (46, 1, 6, 3, 10), dActionEntry (62, 1, 6, 3, 10), 
			dActionEntry (260, 1, 9, 1, 36), dActionEntry (277, 1, 9, 1, 36), dActionEntry (290, 1, 9, 1, 36), dActionEntry (292, 1, 9, 1, 36), 
			dActionEntry (295, 1, 9, 1, 36), dActionEntry (298, 1, 9, 1, 36), dActionEntry (299, 1, 9, 1, 36), dActionEntry (260, 1, 16, 4, 48), 
			dActionEntry (277, 1, 16, 4, 48), dActionEntry (290, 1, 16, 4, 48), dActionEntry (292, 1, 16, 4, 48), dActionEntry (295, 1, 16, 4, 48), 
			dActionEntry (298, 1, 16, 4, 48), dActionEntry (299, 1, 16, 4, 48), dActionEntry (260, 1, 16, 4, 50), dActionEntry (277, 1, 16, 4, 50), 
			dActionEntry (290, 1, 16, 4, 50), dActionEntry (292, 1, 16, 4, 50), dActionEntry (295, 1, 16, 4, 50), dActionEntry (298, 1, 16, 4, 50), 
			dActionEntry (299, 1, 16, 4, 50), dActionEntry (44, 0, 72, 0, 0), dActionEntry (260, 1, 15, 4, 47), dActionEntry (277, 1, 15, 4, 47), 
			dActionEntry (290, 1, 15, 4, 47), dActionEntry (292, 1, 15, 4, 47), dActionEntry (295, 1, 15, 4, 47), dActionEntry (298, 1, 15, 4, 47), 
			dActionEntry (299, 1, 15, 4, 47), dActionEntry (260, 1, 15, 4, 45), dActionEntry (277, 1, 15, 4, 45), dActionEntry (290, 1, 15, 4, 45), 
			dActionEntry (292, 1, 15, 4, 45), dActionEntry (295, 1, 15, 4, 45), dActionEntry (298, 1, 15, 4, 45), dActionEntry (299, 1, 15, 4, 45), 
			dActionEntry (260, 1, 15, 4, 42), dActionEntry (277, 1, 15, 4, 42), dActionEntry (290, 1, 15, 4, 42), dActionEntry (292, 1, 15, 4, 42), 
			dActionEntry (295, 1, 15, 4, 42), dActionEntry (298, 1, 15, 4, 42), dActionEntry (299, 1, 15, 4, 42), dActionEntry (257, 0, 44, 0, 0), 
			dActionEntry (262, 0, 63, 0, 0), dActionEntry (44, 0, 70, 0, 0), dActionEntry (125, 0, 77, 0, 0), dActionEntry (44, 1, 23, 3, 39), 
			dActionEntry (125, 1, 23, 3, 39), dActionEntry (260, 1, 16, 6, 49), dActionEntry (277, 1, 16, 6, 49), dActionEntry (290, 1, 16, 6, 49), 
			dActionEntry (292, 1, 16, 6, 49), dActionEntry (295, 1, 16, 6, 49), dActionEntry (298, 1, 16, 6, 49), dActionEntry (299, 1, 16, 6, 49), 
			dActionEntry (260, 1, 16, 6, 51), dActionEntry (277, 1, 16, 6, 51), dActionEntry (290, 1, 16, 6, 51), dActionEntry (292, 1, 16, 6, 51), 
			dActionEntry (295, 1, 16, 6, 51), dActionEntry (298, 1, 16, 6, 51), dActionEntry (299, 1, 16, 6, 51), dActionEntry (260, 1, 15, 6, 43), 
			dActionEntry (277, 1, 15, 6, 43), dActionEntry (290, 1, 15, 6, 43), dActionEntry (292, 1, 15, 6, 43), dActionEntry (295, 1, 15, 6, 43), 
			dActionEntry (298, 1, 15, 6, 43), dActionEntry (299, 1, 15, 6, 43)};

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
					case 29:// rule opcodeAddi : ADDI 
						{entry.m_value = parameter[0].m_value; entry.m_value.m_semanticValue = dVirtualMachine::addi;}
						break;
					case 26:// rule opcodeType1 : CALL 
						{entry.m_value = parameter[0].m_value; entry.m_value.m_semanticValue = dVirtualMachine::call;}
						break;
					case 24:// rule opcodeType0 : JUMPR 
						{entry.m_value = parameter[0].m_value; entry.m_value.m_semanticValue = dVirtualMachine::jumpr;}
						break;
					case 28:// rule pushPopOpcode : POP 
						{entry.m_value = parameter[0].m_value; entry.m_value.m_semanticValue = dVirtualMachine::pop;}
						break;
					case 27:// rule pushPopOpcode : PUSH 
						{entry.m_value = parameter[0].m_value; entry.m_value.m_semanticValue = dVirtualMachine::push;}
						break;
					case 25:// rule opcodeType0 : RET 
						{entry.m_value = parameter[0].m_value; entry.m_value.m_semanticValue = dVirtualMachine::ret;}
						break;
					case 40:// rule instructionType0 : opcodeType0 
						{
					dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
					char text[64];
					sprintf (text, "r%d", me->m_virtualMachine->GetRegisterCount() - 1);
					dUserVariable stackRegister (REGISTER, text);
					stackRegister.m_semanticValue = me->m_virtualMachine->GetRegisterCount() - 1;
					me->EmitInstrutionType0 (parameter[0].m_value, stackRegister);
				}
						break;
					case 11:// rule dataDeclaration : dataType literal 
						{((dAssemblerCompiler*)this)->EmitUnInitilizedDataDeclaration (parameter[0].m_value, parameter[1].m_value);}
						break;
					case 18:// rule beginFunction : BEGIN literal visibility 
						{ 
					dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
					me->EmitBeginFunction (parameter[1].m_value, parameter[2].m_value);
				}
						break;
					case 17:// rule functionDeclaration : beginFunction instructionList END 
						{
							dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
							me->EmitEndFunction ();
						}
						break;
					case 37:// rule register : REGISTER 
						{
				dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
				entry.m_value = me->TypeCheckRegister (parameter[0].m_value);
			}
						break;
					case 41:// rule instructionType0 : opcodeType0 register 
						{
					dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
					me->EmitInstrutionType0 (parameter[0].m_value, parameter[1].m_value);
				}
						break;
					case 46:// rule instructionType1 : opcodeType1 literal 
						{
						dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
						dUserVariable register0 (REGISTER, "r0");
						me->EmitInstrutionType1_label (parameter[0].m_value, register0, parameter[1].m_value);
					}
						break;
					case 38:// rule registerMask : register 
						{entry.m_value = parameter[0].m_value; entry.m_value.m_semanticValue = 1 << parameter[0].m_value.m_semanticValue;}
						break;
					case 36:// rule constantExpression : INTEGER 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitIntegerConst (parameter[0].m_value);}
						break;
					case 12:// rule dataDeclaration : dataType literal = constantExpression 
						{((dAssemblerCompiler*)this)->EmitInitilizedDataDeclaration (parameter[0].m_value, parameter[1].m_value, parameter[3].m_value);}
						break;
					case 48:// rule instructionType2 : opcodeAddi register , constantExpression 
						{ 
						dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
						dUserVariable register0 (REGISTER, "r0");
						me->EmitInstrutionType2 (parameter[0].m_value, parameter[1].m_value, register0, parameter[3].m_value);
					}
						break;
					case 47:// rule instructionType1 : opcodeType1 register , literal 
						{
						dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
						me->EmitInstrutionType1_label (parameter[0].m_value, parameter[1].m_value, parameter[3].m_value);
					}
						break;
					case 42:// rule instructionType1 : pushPopOpcode { registerMask } 
						{
						dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
						char text[64];
						sprintf (text, "r%d", me->m_virtualMachine->GetRegisterCount() - 1);
						dUserVariable stackRegister (REGISTER, text);
						stackRegister.m_semanticValue = me->m_virtualMachine->GetRegisterCount() - 1;
						me->EmitInstrutionType1 (parameter[0].m_value, stackRegister, parameter[2].m_value);
					}
						break;
					case 39:// rule registerMask : registerMask , register 
						{entry.m_value = parameter[0].m_value; entry.m_value.m_semanticValue = parameter[0].m_value.m_semanticValue | (1 << parameter[2].m_value.m_semanticValue);}
						break;
					case 49:// rule instructionType2 : opcodeAddi register , register , constantExpression 
						{ 
						dAssemblerCompiler* const me = (dAssemblerCompiler*) this;
						me->EmitInstrutionType2 (parameter[0].m_value, parameter[1].m_value, parameter[3].m_value, parameter[5].m_value);
					}
						break;
					case 43:// rule instructionType1 : pushPopOpcode register , { registerMask } 
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





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
			5, 1, 1, 5, 1, 4, 4, 4, 1, 4, 1, 4, 4, 2, 1, 2, 1, 1, 1, 15, 2, 2, 2, 1, 
			2, 15, 15, 12, 1, 2, 15, 4, 15, 15, 2, 1, 2, 15, 2, 2, 1, 1, 15, 1, 12, 3, 2, 2, 
			1, 12, 15, 1, 1, 11, 21, 15, 2, 2, 2, 2, 0, 1, 2, 4, 16, 11, 2, 2, 2, 2, 1, 2, 
			11, 15, 16, 1, 3, 1, 21, 1, 1, 1, 15, 21, 10, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 1, 3, 1, 15, 11, 2, 11, 2, 2, 3, 2, 2, 1, 1, 21, 1, 2, 2, 2, 1, 15, 21, 
			1, 21, 21, 1, 2, 2, 9, 21, 2, 3, 2, 2, 2, 3, 2, 1, 1, 2, 3, 3, 3, 2, 1, 2, 
			10, 2, 2, 21, 1, 1, 2, 3, 21, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 
			1, 2, 9, 2, 2, 2, 2, 1, 2, 0, 2, 2, 3, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 
			1, 3, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 2, 2, 1, 15, 
			1, 1, 3, 3, 2, 3, 3, 3, 1, 15, 1, 1, 2, 2, 2, 2, 2, 2, 2, 21, 2, 2, 2, 1, 
			1, 1, 1, 21, 1, 2, 3, 2, 3, 3, 1, 1, 1, 3, 3, 2, 2, 21, 2, 1, 2, 2, 21, 1, 
			3, 3, 2, 1, 2, 1, 1, 1, 3, 1, 2, 1, 2, 2, 1, 3, 1, 21, 3, 1, 1, 1, 2, 21, 
			3, 3, 1, 3, 3, 1, 9, 2, 1, 2, 2, 2, 2, 2, 2, 2};
	static short actionsStart[] = {
			0, 5, 6, 7, 12, 13, 17, 21, 25, 26, 30, 31, 35, 39, 41, 42, 44, 45, 46, 47, 62, 64, 66, 68, 
			69, 71, 86, 101, 113, 114, 116, 131, 135, 150, 165, 167, 168, 170, 185, 187, 189, 190, 191, 206, 207, 219, 222, 224, 
			226, 101, 227, 113, 242, 243, 254, 275, 290, 292, 294, 296, 298, 298, 299, 301, 305, 321, 332, 334, 336, 338, 340, 341, 
			343, 354, 369, 385, 386, 389, 390, 411, 412, 413, 414, 429, 450, 460, 461, 463, 464, 411, 465, 466, 467, 468, 469, 470, 
			471, 472, 473, 474, 477, 478, 343, 493, 343, 495, 497, 499, 502, 504, 506, 507, 508, 529, 530, 532, 534, 536, 537, 552, 
			573, 574, 595, 616, 617, 619, 621, 630, 619, 651, 619, 495, 654, 656, 659, 661, 662, 663, 665, 668, 671, 674, 676, 677, 
			450, 679, 681, 683, 704, 705, 617, 706, 709, 730, 732, 734, 736, 738, 740, 741, 742, 743, 744, 745, 746, 750, 751, 752, 
			753, 754, 756, 765, 767, 769, 495, 771, 772, 298, 774, 776, 778, 781, 783, 785, 113, 787, 788, 619, 789, 791, 793, 619, 
			795, 796, 799, 800, 802, 803, 804, 805, 806, 807, 808, 809, 810, 811, 812, 816, 817, 818, 819, 820, 821, 823, 825, 826, 
			841, 842, 843, 846, 849, 665, 668, 851, 854, 855, 870, 871, 872, 874, 876, 878, 880, 882, 617, 884, 905, 495, 907, 909, 
			910, 911, 912, 913, 934, 935, 937, 781, 940, 943, 946, 947, 948, 949, 952, 955, 957, 959, 980, 820, 982, 495, 984, 1005, 
			843, 846, 1006, 1008, 1009, 1011, 1012, 1013, 1014, 743, 1017, 1019, 1020, 1022, 1024, 937, 1025, 1026, 1047, 1011, 1050, 1051, 1052, 1054, 
			1075, 1078, 820, 1081, 1084, 1087, 756, 1088, 1090, 495, 1091, 1093, 495, 1095, 1097, 1099};
	static short gotoCount[] = {
			6, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 12, 0, 0, 2, 0, 0, 0, 10, 3, 1, 0, 0, 0, 
			0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 3, 0, 1, 0, 0, 16, 0, 0, 0, 0, 0, 1, 0, 2, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 1, 2, 0, 15, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 1, 0, 0, 5, 1, 5, 2, 0, 3, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 0, 
			2, 2, 2, 2, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 0, 15, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 1, 1, 2, 3, 2, 0, 0, 1, 1, 0, 2, 0, 0, 0, 2, 
			0, 3, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 16, 0, 2, 0, 0, 0, 0, 0, 16, 0, 0, 0, 2, 3, 3, 0, 0, 0, 0, 1, 0, 
			0, 15, 0, 1, 0, 2, 15, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 16, 0, 1, 0, 0, 0, 15, 
			0, 0, 1, 2, 0, 0, 1, 0, 1, 2, 0, 0, 2, 0, 0, 0};
	static short gotoStart[] = {
			0, 6, 6, 6, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 12, 12, 12, 12, 24, 24, 24, 26, 26, 26, 26, 36, 39, 40, 40, 40, 
			40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 43, 44, 44, 44, 47, 47, 48, 48, 48, 64, 64, 64, 64, 64, 64, 65, 65, 67, 
			67, 67, 67, 67, 67, 67, 67, 67, 67, 72, 72, 72, 73, 75, 75, 90, 92, 92, 92, 92, 92, 94, 94, 94, 94, 94, 96, 96, 96, 96, 96, 96, 
			96, 96, 96, 96, 97, 97, 97, 102, 103, 108, 110, 110, 113, 113, 113, 113, 113, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 131, 133, 134, 
			134, 136, 138, 140, 142, 142, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 148, 149, 149, 164, 164, 164, 166, 169, 169, 169, 169, 169, 169, 169, 169, 
			169, 169, 171, 171, 171, 171, 171, 171, 171, 171, 171, 172, 172, 172, 172, 174, 174, 174, 175, 176, 178, 181, 183, 183, 183, 184, 185, 185, 187, 187, 187, 187, 
			189, 189, 192, 192, 193, 193, 193, 193, 193, 194, 194, 194, 195, 195, 195, 195, 195, 195, 195, 195, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 
			196, 196, 196, 196, 196, 196, 196, 196, 198, 198, 198, 200, 216, 216, 218, 218, 218, 218, 218, 218, 234, 234, 234, 234, 236, 239, 242, 242, 242, 242, 242, 243, 
			243, 243, 258, 258, 259, 259, 261, 276, 276, 276, 276, 276, 276, 276, 277, 277, 277, 277, 279, 279, 279, 279, 279, 279, 279, 279, 295, 295, 296, 296, 296, 296, 
			311, 311, 311, 312, 314, 314, 314, 315, 315, 316, 318, 318, 318, 320, 320, 320};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (295, 8), dGotoEntry (296, 3), dGotoEntry (297, 9), dGotoEntry (298, 5), dGotoEntry (299, 6), 
			dGotoEntry (300, 1), dGotoEntry (297, 12), dGotoEntry (298, 5), dGotoEntry (299, 6), dGotoEntry (300, 1), 
			dGotoEntry (301, 15), dGotoEntry (302, 18), dGotoEntry (300, 23), dGotoEntry (303, 41), dGotoEntry (304, 26), 
			dGotoEntry (305, 37), dGotoEntry (306, 25), dGotoEntry (307, 32), dGotoEntry (308, 42), dGotoEntry (309, 35), 
			dGotoEntry (319, 28), dGotoEntry (320, 43), dGotoEntry (321, 30), dGotoEntry (324, 22), dGotoEntry (325, 46), 
			dGotoEntry (326, 47), dGotoEntry (300, 23), dGotoEntry (305, 50), dGotoEntry (306, 25), dGotoEntry (307, 32), 
			dGotoEntry (308, 42), dGotoEntry (309, 35), dGotoEntry (319, 28), dGotoEntry (320, 43), dGotoEntry (321, 30), 
			dGotoEntry (324, 22), dGotoEntry (319, 51), dGotoEntry (320, 52), dGotoEntry (324, 22), dGotoEntry (318, 55), 
			dGotoEntry (319, 60), dGotoEntry (315, 67), dGotoEntry (323, 68), dGotoEntry (326, 71), dGotoEntry (319, 51), 
			dGotoEntry (320, 52), dGotoEntry (324, 22), dGotoEntry (318, 73), dGotoEntry (315, 81), dGotoEntry (320, 100), 
			dGotoEntry (321, 86), dGotoEntry (322, 77), dGotoEntry (324, 22), dGotoEntry (327, 78), dGotoEntry (328, 83), 
			dGotoEntry (329, 79), dGotoEntry (330, 89), dGotoEntry (331, 95), dGotoEntry (332, 80), dGotoEntry (333, 93), 
			dGotoEntry (334, 94), dGotoEntry (335, 92), dGotoEntry (336, 85), dGotoEntry (337, 96), dGotoEntry (317, 101), 
			dGotoEntry (325, 103), dGotoEntry (326, 47), dGotoEntry (310, 108), dGotoEntry (311, 109), dGotoEntry (312, 110), 
			dGotoEntry (320, 111), dGotoEntry (324, 22), dGotoEntry (318, 113), dGotoEntry (315, 115), dGotoEntry (323, 116), 
			dGotoEntry (315, 81), dGotoEntry (320, 100), dGotoEntry (321, 86), dGotoEntry (322, 77), dGotoEntry (324, 22), 
			dGotoEntry (328, 119), dGotoEntry (329, 79), dGotoEntry (330, 89), dGotoEntry (331, 95), dGotoEntry (332, 80), 
			dGotoEntry (333, 93), dGotoEntry (334, 94), dGotoEntry (335, 92), dGotoEntry (336, 85), dGotoEntry (337, 96), 
			dGotoEntry (316, 122), dGotoEntry (317, 120), dGotoEntry (320, 123), dGotoEntry (324, 22), dGotoEntry (316, 127), 
			dGotoEntry (317, 120), dGotoEntry (338, 132), dGotoEntry (310, 108), dGotoEntry (311, 109), dGotoEntry (312, 135), 
			dGotoEntry (320, 111), dGotoEntry (324, 22), dGotoEntry (326, 71), dGotoEntry (310, 108), dGotoEntry (311, 109), 
			dGotoEntry (312, 136), dGotoEntry (320, 111), dGotoEntry (324, 22), dGotoEntry (315, 67), dGotoEntry (323, 137), 
			dGotoEntry (315, 139), dGotoEntry (323, 140), dGotoEntry (341, 143), dGotoEntry (315, 81), dGotoEntry (320, 100), 
			dGotoEntry (321, 86), dGotoEntry (322, 77), dGotoEntry (324, 22), dGotoEntry (327, 147), dGotoEntry (328, 83), 
			dGotoEntry (329, 79), dGotoEntry (330, 89), dGotoEntry (331, 95), dGotoEntry (332, 80), dGotoEntry (333, 93), 
			dGotoEntry (334, 94), dGotoEntry (335, 92), dGotoEntry (336, 85), dGotoEntry (337, 96), dGotoEntry (315, 115), 
			dGotoEntry (323, 154), dGotoEntry (315, 156), dGotoEntry (323, 157), dGotoEntry (324, 161), dGotoEntry (315, 156), 
			dGotoEntry (323, 169), dGotoEntry (315, 115), dGotoEntry (323, 171), dGotoEntry (315, 156), dGotoEntry (323, 172), 
			dGotoEntry (315, 67), dGotoEntry (323, 173), dGotoEntry (315, 139), dGotoEntry (323, 140), dGotoEntry (341, 176), 
			dGotoEntry (311, 183), dGotoEntry (320, 111), dGotoEntry (324, 22), dGotoEntry (314, 184), dGotoEntry (315, 81), 
			dGotoEntry (320, 100), dGotoEntry (321, 86), dGotoEntry (322, 77), dGotoEntry (324, 22), dGotoEntry (328, 119), 
			dGotoEntry (329, 79), dGotoEntry (330, 89), dGotoEntry (331, 95), dGotoEntry (332, 80), dGotoEntry (333, 93), 
			dGotoEntry (334, 94), dGotoEntry (335, 92), dGotoEntry (336, 85), dGotoEntry (337, 96), dGotoEntry (315, 115), 
			dGotoEntry (323, 188), dGotoEntry (315, 139), dGotoEntry (323, 140), dGotoEntry (341, 190), dGotoEntry (331, 194), 
			dGotoEntry (337, 96), dGotoEntry (324, 203), dGotoEntry (315, 67), dGotoEntry (323, 213), dGotoEntry (317, 215), 
			dGotoEntry (313, 216), dGotoEntry (315, 139), dGotoEntry (323, 218), dGotoEntry (315, 139), dGotoEntry (323, 140), 
			dGotoEntry (341, 220), dGotoEntry (315, 222), dGotoEntry (323, 223), dGotoEntry (318, 225), dGotoEntry (315, 226), 
			dGotoEntry (315, 156), dGotoEntry (323, 228), dGotoEntry (315, 156), dGotoEntry (323, 230), dGotoEntry (315, 139), 
			dGotoEntry (323, 140), dGotoEntry (341, 233), dGotoEntry (338, 132), dGotoEntry (318, 236), dGotoEntry (338, 238), 
			dGotoEntry (318, 244), dGotoEntry (339, 254), dGotoEntry (340, 253), dGotoEntry (315, 115), dGotoEntry (323, 256), 
			dGotoEntry (315, 81), dGotoEntry (320, 100), dGotoEntry (321, 86), dGotoEntry (322, 77), dGotoEntry (324, 22), 
			dGotoEntry (327, 257), dGotoEntry (328, 83), dGotoEntry (329, 79), dGotoEntry (330, 89), dGotoEntry (331, 95), 
			dGotoEntry (332, 80), dGotoEntry (333, 93), dGotoEntry (334, 94), dGotoEntry (335, 92), dGotoEntry (336, 85), 
			dGotoEntry (337, 96), dGotoEntry (315, 67), dGotoEntry (323, 260), dGotoEntry (315, 81), dGotoEntry (320, 100), 
			dGotoEntry (321, 86), dGotoEntry (322, 77), dGotoEntry (324, 22), dGotoEntry (327, 262), dGotoEntry (328, 83), 
			dGotoEntry (329, 79), dGotoEntry (330, 89), dGotoEntry (331, 95), dGotoEntry (332, 80), dGotoEntry (333, 93), 
			dGotoEntry (334, 94), dGotoEntry (335, 92), dGotoEntry (336, 85), dGotoEntry (337, 96), dGotoEntry (315, 222), 
			dGotoEntry (323, 264), dGotoEntry (315, 139), dGotoEntry (323, 140), dGotoEntry (341, 266), dGotoEntry (315, 139), 
			dGotoEntry (323, 140), dGotoEntry (341, 268), dGotoEntry (340, 272), dGotoEntry (315, 81), dGotoEntry (320, 100), 
			dGotoEntry (321, 86), dGotoEntry (322, 77), dGotoEntry (324, 22), dGotoEntry (328, 119), dGotoEntry (329, 79), 
			dGotoEntry (330, 89), dGotoEntry (331, 95), dGotoEntry (332, 80), dGotoEntry (333, 93), dGotoEntry (334, 94), 
			dGotoEntry (335, 92), dGotoEntry (336, 85), dGotoEntry (337, 96), dGotoEntry (318, 275), dGotoEntry (315, 67), 
			dGotoEntry (323, 277), dGotoEntry (315, 81), dGotoEntry (320, 100), dGotoEntry (321, 86), dGotoEntry (322, 77), 
			dGotoEntry (324, 22), dGotoEntry (328, 119), dGotoEntry (329, 79), dGotoEntry (330, 89), dGotoEntry (331, 95), 
			dGotoEntry (332, 80), dGotoEntry (333, 93), dGotoEntry (334, 94), dGotoEntry (335, 92), dGotoEntry (336, 85), 
			dGotoEntry (337, 96), dGotoEntry (318, 282), dGotoEntry (331, 284), dGotoEntry (337, 285), dGotoEntry (315, 81), 
			dGotoEntry (320, 100), dGotoEntry (321, 86), dGotoEntry (322, 77), dGotoEntry (324, 22), dGotoEntry (327, 287), 
			dGotoEntry (328, 83), dGotoEntry (329, 79), dGotoEntry (330, 89), dGotoEntry (331, 95), dGotoEntry (332, 80), 
			dGotoEntry (333, 93), dGotoEntry (334, 94), dGotoEntry (335, 92), dGotoEntry (336, 85), dGotoEntry (337, 96), 
			dGotoEntry (318, 289), dGotoEntry (315, 81), dGotoEntry (320, 100), dGotoEntry (321, 86), dGotoEntry (322, 77), 
			dGotoEntry (324, 22), dGotoEntry (328, 119), dGotoEntry (329, 79), dGotoEntry (330, 89), dGotoEntry (331, 95), 
			dGotoEntry (332, 80), dGotoEntry (333, 93), dGotoEntry (334, 94), dGotoEntry (335, 92), dGotoEntry (336, 85), 
			dGotoEntry (337, 96), dGotoEntry (318, 293), dGotoEntry (315, 156), dGotoEntry (323, 295), dGotoEntry (324, 296), 
			dGotoEntry (338, 298), dGotoEntry (315, 67), dGotoEntry (323, 299), dGotoEntry (315, 67), dGotoEntry (323, 302)};
	static dActionEntry actionTable[] = {
			dActionEntry (59, 0, 7, 0, 0), dActionEntry (255, 1, 0, 0, 1), dActionEntry (259, 0, 2, 0, 0), dActionEntry (261, 0, 4, 0, 0), 
			dActionEntry (262, 1, 5, 0, 10), dActionEntry (262, 0, 10, 0, 0), dActionEntry (260, 0, 11, 0, 0), dActionEntry (59, 0, 7, 0, 0), 
			dActionEntry (255, 1, 0, 1, 2), dActionEntry (259, 0, 2, 0, 0), dActionEntry (261, 0, 4, 0, 0), dActionEntry (262, 1, 5, 0, 10), 
			dActionEntry (262, 1, 5, 1, 11), dActionEntry (59, 1, 2, 1, 6), dActionEntry (255, 1, 2, 1, 6), dActionEntry (259, 1, 2, 1, 6), 
			dActionEntry (262, 1, 2, 1, 6), dActionEntry (59, 1, 2, 1, 7), dActionEntry (255, 1, 2, 1, 7), dActionEntry (259, 1, 2, 1, 7), 
			dActionEntry (262, 1, 2, 1, 7), dActionEntry (59, 1, 2, 1, 5), dActionEntry (255, 1, 2, 1, 5), dActionEntry (259, 1, 2, 1, 5), 
			dActionEntry (262, 1, 2, 1, 5), dActionEntry (255, 2, 0, 0, 0), dActionEntry (59, 1, 1, 1, 3), dActionEntry (255, 1, 1, 1, 3), 
			dActionEntry (259, 1, 1, 1, 3), dActionEntry (262, 1, 1, 1, 3), dActionEntry (263, 0, 13, 0, 0), dActionEntry (59, 1, 3, 2, 8), 
			dActionEntry (255, 1, 3, 2, 8), dActionEntry (259, 1, 3, 2, 8), dActionEntry (262, 1, 3, 2, 8), dActionEntry (59, 1, 1, 2, 4), 
			dActionEntry (255, 1, 1, 2, 4), dActionEntry (259, 1, 1, 2, 4), dActionEntry (262, 1, 1, 2, 4), dActionEntry (123, 1, 6, 0, 14), 
			dActionEntry (264, 0, 14, 0, 0), dActionEntry (263, 0, 16, 0, 0), dActionEntry (123, 1, 7, 0, 12), dActionEntry (265, 0, 17, 0, 0), 
			dActionEntry (123, 1, 6, 2, 15), dActionEntry (123, 1, 7, 1, 13), dActionEntry (123, 0, 19, 0, 0), dActionEntry (59, 0, 33, 0, 0), 
			dActionEntry (125, 1, 8, 0, 16), dActionEntry (256, 0, 44, 0, 0), dActionEntry (261, 0, 27, 0, 0), dActionEntry (262, 0, 40, 0, 0), 
			dActionEntry (263, 1, 5, 0, 10), dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), 
			dActionEntry (269, 0, 36, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), 
			dActionEntry (273, 0, 39, 0, 0), dActionEntry (274, 0, 38, 0, 0), dActionEntry (91, 1, 29, 1, 53), dActionEntry (263, 1, 29, 1, 53), 
			dActionEntry (91, 1, 29, 1, 56), dActionEntry (263, 1, 29, 1, 56), dActionEntry (91, 0, 45, 0, 0), dActionEntry (263, 1, 25, 1, 48), 
			dActionEntry (263, 0, 48, 0, 0), dActionEntry (91, 1, 29, 1, 61), dActionEntry (263, 1, 29, 1, 61), dActionEntry (59, 1, 10, 1, 21), 
			dActionEntry (125, 1, 10, 1, 21), dActionEntry (256, 1, 10, 1, 21), dActionEntry (261, 1, 10, 1, 21), dActionEntry (262, 1, 10, 1, 21), 
			dActionEntry (263, 1, 10, 1, 21), dActionEntry (266, 1, 10, 1, 21), dActionEntry (267, 1, 10, 1, 21), dActionEntry (268, 1, 10, 1, 21), 
			dActionEntry (269, 1, 10, 1, 21), dActionEntry (270, 1, 10, 1, 21), dActionEntry (271, 1, 10, 1, 21), dActionEntry (272, 1, 10, 1, 21), 
			dActionEntry (273, 1, 10, 1, 21), dActionEntry (274, 1, 10, 1, 21), dActionEntry (59, 0, 33, 0, 0), dActionEntry (125, 1, 8, 1, 17), 
			dActionEntry (256, 0, 44, 0, 0), dActionEntry (261, 0, 49, 0, 0), dActionEntry (262, 0, 40, 0, 0), dActionEntry (263, 1, 5, 0, 10), 
			dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), 
			dActionEntry (274, 0, 38, 0, 0), dActionEntry (256, 0, 53, 0, 0), dActionEntry (262, 0, 40, 0, 0), dActionEntry (263, 1, 5, 1, 11), 
			dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), 
			dActionEntry (274, 0, 38, 0, 0), dActionEntry (123, 0, 54, 0, 0), dActionEntry (91, 1, 29, 1, 54), dActionEntry (263, 1, 29, 1, 54), 
			dActionEntry (59, 1, 12, 1, 46), dActionEntry (125, 1, 12, 1, 46), dActionEntry (256, 1, 12, 1, 46), dActionEntry (261, 1, 12, 1, 46), 
			dActionEntry (262, 1, 12, 1, 46), dActionEntry (263, 1, 12, 1, 46), dActionEntry (266, 1, 12, 1, 46), dActionEntry (267, 1, 12, 1, 46), 
			dActionEntry (268, 1, 12, 1, 46), dActionEntry (269, 1, 12, 1, 46), dActionEntry (270, 1, 12, 1, 46), dActionEntry (271, 1, 12, 1, 46), 
			dActionEntry (272, 1, 12, 1, 46), dActionEntry (273, 1, 12, 1, 46), dActionEntry (274, 1, 12, 1, 46), dActionEntry (267, 0, 56, 0, 0), 
			dActionEntry (268, 0, 58, 0, 0), dActionEntry (269, 0, 59, 0, 0), dActionEntry (270, 0, 57, 0, 0), dActionEntry (59, 1, 10, 1, 22), 
			dActionEntry (125, 1, 10, 1, 22), dActionEntry (256, 1, 10, 1, 22), dActionEntry (261, 1, 10, 1, 22), dActionEntry (262, 1, 10, 1, 22), 
			dActionEntry (263, 1, 10, 1, 22), dActionEntry (266, 1, 10, 1, 22), dActionEntry (267, 1, 10, 1, 22), dActionEntry (268, 1, 10, 1, 22), 
			dActionEntry (269, 1, 10, 1, 22), dActionEntry (270, 1, 10, 1, 22), dActionEntry (271, 1, 10, 1, 22), dActionEntry (272, 1, 10, 1, 22), 
			dActionEntry (273, 1, 10, 1, 22), dActionEntry (274, 1, 10, 1, 22), dActionEntry (59, 1, 10, 1, 20), dActionEntry (125, 1, 10, 1, 20), 
			dActionEntry (256, 1, 10, 1, 20), dActionEntry (261, 1, 10, 1, 20), dActionEntry (262, 1, 10, 1, 20), dActionEntry (263, 1, 10, 1, 20), 
			dActionEntry (266, 1, 10, 1, 20), dActionEntry (267, 1, 10, 1, 20), dActionEntry (268, 1, 10, 1, 20), dActionEntry (269, 1, 10, 1, 20), 
			dActionEntry (270, 1, 10, 1, 20), dActionEntry (271, 1, 10, 1, 20), dActionEntry (272, 1, 10, 1, 20), dActionEntry (273, 1, 10, 1, 20), 
			dActionEntry (274, 1, 10, 1, 20), dActionEntry (91, 1, 29, 1, 52), dActionEntry (263, 1, 29, 1, 52), dActionEntry (263, 0, 61, 0, 0), 
			dActionEntry (91, 1, 29, 1, 55), dActionEntry (263, 1, 29, 1, 55), dActionEntry (59, 1, 9, 1, 18), dActionEntry (125, 1, 9, 1, 18), 
			dActionEntry (256, 1, 9, 1, 18), dActionEntry (261, 1, 9, 1, 18), dActionEntry (262, 1, 9, 1, 18), dActionEntry (263, 1, 9, 1, 18), 
			dActionEntry (266, 1, 9, 1, 18), dActionEntry (267, 1, 9, 1, 18), dActionEntry (268, 1, 9, 1, 18), dActionEntry (269, 1, 9, 1, 18), 
			dActionEntry (270, 1, 9, 1, 18), dActionEntry (271, 1, 9, 1, 18), dActionEntry (272, 1, 9, 1, 18), dActionEntry (273, 1, 9, 1, 18), 
			dActionEntry (274, 1, 9, 1, 18), dActionEntry (91, 1, 29, 1, 63), dActionEntry (263, 1, 29, 1, 63), dActionEntry (91, 1, 29, 1, 62), 
			dActionEntry (263, 1, 29, 1, 62), dActionEntry (263, 0, 62, 0, 0), dActionEntry (125, 0, 63, 0, 0), dActionEntry (59, 1, 10, 1, 23), 
			dActionEntry (125, 1, 10, 1, 23), dActionEntry (256, 1, 10, 1, 23), dActionEntry (261, 1, 10, 1, 23), dActionEntry (262, 1, 10, 1, 23), 
			dActionEntry (263, 1, 10, 1, 23), dActionEntry (266, 1, 10, 1, 23), dActionEntry (267, 1, 10, 1, 23), dActionEntry (268, 1, 10, 1, 23), 
			dActionEntry (269, 1, 10, 1, 23), dActionEntry (270, 1, 10, 1, 23), dActionEntry (271, 1, 10, 1, 23), dActionEntry (272, 1, 10, 1, 23), 
			dActionEntry (273, 1, 10, 1, 23), dActionEntry (274, 1, 10, 1, 23), dActionEntry (263, 0, 64, 0, 0), dActionEntry (261, 0, 65, 0, 0), 
			dActionEntry (262, 1, 14, 1, 24), dActionEntry (263, 1, 14, 1, 24), dActionEntry (266, 1, 14, 1, 24), dActionEntry (267, 1, 14, 1, 24), 
			dActionEntry (268, 1, 14, 1, 24), dActionEntry (269, 1, 14, 1, 24), dActionEntry (270, 1, 14, 1, 24), dActionEntry (271, 1, 14, 1, 24), 
			dActionEntry (272, 1, 14, 1, 24), dActionEntry (273, 1, 14, 1, 24), dActionEntry (274, 1, 14, 1, 24), dActionEntry (93, 0, 69, 0, 0), 
			dActionEntry (258, 0, 66, 0, 0), dActionEntry (263, 0, 70, 0, 0), dActionEntry (91, 0, 45, 0, 0), dActionEntry (263, 1, 25, 2, 50), 
			dActionEntry (91, 1, 30, 1, 64), dActionEntry (263, 1, 30, 1, 64), dActionEntry (40, 0, 72, 0, 0), dActionEntry (59, 1, 9, 2, 19), 
			dActionEntry (125, 1, 9, 2, 19), dActionEntry (256, 1, 9, 2, 19), dActionEntry (261, 1, 9, 2, 19), dActionEntry (262, 1, 9, 2, 19), 
			dActionEntry (263, 1, 9, 2, 19), dActionEntry (266, 1, 9, 2, 19), dActionEntry (267, 1, 9, 2, 19), dActionEntry (268, 1, 9, 2, 19), 
			dActionEntry (269, 1, 9, 2, 19), dActionEntry (270, 1, 9, 2, 19), dActionEntry (271, 1, 9, 2, 19), dActionEntry (272, 1, 9, 2, 19), 
			dActionEntry (273, 1, 9, 2, 19), dActionEntry (274, 1, 9, 2, 19), dActionEntry (263, 0, 74, 0, 0), dActionEntry (262, 1, 14, 2, 26), 
			dActionEntry (263, 1, 14, 2, 26), dActionEntry (266, 1, 14, 2, 26), dActionEntry (267, 1, 14, 2, 26), dActionEntry (268, 1, 14, 2, 26), 
			dActionEntry (269, 1, 14, 2, 26), dActionEntry (270, 1, 14, 2, 26), dActionEntry (271, 1, 14, 2, 26), dActionEntry (272, 1, 14, 2, 26), 
			dActionEntry (273, 1, 14, 2, 26), dActionEntry (274, 1, 14, 2, 26), dActionEntry (125, 0, 82, 0, 0), dActionEntry (261, 0, 84, 0, 0), 
			dActionEntry (262, 0, 40, 0, 0), dActionEntry (263, 0, 99, 0, 0), dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), dActionEntry (274, 0, 38, 0, 0), dActionEntry (276, 0, 76, 0, 0), 
			dActionEntry (277, 0, 88, 0, 0), dActionEntry (278, 0, 97, 0, 0), dActionEntry (279, 0, 75, 0, 0), dActionEntry (280, 0, 90, 0, 0), 
			dActionEntry (282, 0, 87, 0, 0), dActionEntry (285, 0, 98, 0, 0), dActionEntry (286, 0, 91, 0, 0), dActionEntry (59, 1, 13, 2, 40), 
			dActionEntry (125, 1, 13, 2, 40), dActionEntry (256, 1, 13, 2, 40), dActionEntry (261, 1, 13, 2, 40), dActionEntry (262, 1, 13, 2, 40), 
			dActionEntry (263, 1, 13, 2, 40), dActionEntry (266, 1, 13, 2, 40), dActionEntry (267, 1, 13, 2, 40), dActionEntry (268, 1, 13, 2, 40), 
			dActionEntry (269, 1, 13, 2, 40), dActionEntry (270, 1, 13, 2, 40), dActionEntry (271, 1, 13, 2, 40), dActionEntry (272, 1, 13, 2, 40), 
			dActionEntry (273, 1, 13, 2, 40), dActionEntry (274, 1, 13, 2, 40), dActionEntry (91, 1, 29, 2, 57), dActionEntry (263, 1, 29, 2, 57), 
			dActionEntry (91, 1, 29, 2, 60), dActionEntry (263, 1, 29, 2, 60), dActionEntry (91, 1, 29, 2, 58), dActionEntry (263, 1, 29, 2, 58), 
			dActionEntry (91, 1, 29, 2, 59), dActionEntry (263, 1, 29, 2, 59), dActionEntry (40, 0, 102, 0, 0), dActionEntry (91, 0, 45, 0, 0), 
			dActionEntry (263, 1, 25, 2, 49), dActionEntry (59, 1, 4, 8, 9), dActionEntry (255, 1, 4, 8, 9), dActionEntry (259, 1, 4, 8, 9), 
			dActionEntry (262, 1, 4, 8, 9), dActionEntry (40, 0, 104, 0, 0), dActionEntry (59, 1, 26, 2, 43), dActionEntry (125, 1, 26, 2, 43), 
			dActionEntry (256, 1, 26, 2, 43), dActionEntry (261, 1, 26, 2, 43), dActionEntry (262, 1, 26, 2, 43), dActionEntry (263, 1, 26, 2, 43), 
			dActionEntry (266, 1, 26, 2, 43), dActionEntry (267, 1, 26, 2, 43), dActionEntry (268, 1, 26, 2, 43), dActionEntry (269, 1, 26, 2, 43), 
			dActionEntry (270, 1, 26, 2, 43), dActionEntry (271, 1, 26, 2, 43), dActionEntry (272, 1, 26, 2, 43), dActionEntry (273, 1, 26, 2, 43), 
			dActionEntry (274, 1, 26, 2, 43), dActionEntry (262, 1, 14, 2, 25), dActionEntry (263, 1, 14, 2, 25), dActionEntry (266, 1, 14, 2, 25), 
			dActionEntry (267, 1, 14, 2, 25), dActionEntry (268, 1, 14, 2, 25), dActionEntry (269, 1, 14, 2, 25), dActionEntry (270, 1, 14, 2, 25), 
			dActionEntry (271, 1, 14, 2, 25), dActionEntry (272, 1, 14, 2, 25), dActionEntry (273, 1, 14, 2, 25), dActionEntry (274, 1, 14, 2, 25), 
			dActionEntry (93, 1, 28, 1, 109), dActionEntry (288, 1, 28, 1, 109), dActionEntry (93, 1, 28, 1, 108), dActionEntry (288, 1, 28, 1, 108), 
			dActionEntry (93, 0, 106, 0, 0), dActionEntry (288, 0, 105, 0, 0), dActionEntry (91, 1, 31, 2, 66), dActionEntry (263, 1, 31, 2, 66), 
			dActionEntry (40, 0, 107, 0, 0), dActionEntry (91, 1, 30, 2, 65), dActionEntry (263, 1, 30, 2, 65), dActionEntry (41, 1, 17, 0, 29), 
			dActionEntry (262, 0, 40, 0, 0), dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), 
			dActionEntry (269, 0, 36, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), 
			dActionEntry (273, 0, 39, 0, 0), dActionEntry (274, 0, 38, 0, 0), dActionEntry (59, 1, 13, 3, 41), dActionEntry (125, 1, 13, 3, 41), 
			dActionEntry (256, 1, 13, 3, 41), dActionEntry (261, 1, 13, 3, 41), dActionEntry (262, 1, 13, 3, 41), dActionEntry (263, 1, 13, 3, 41), 
			dActionEntry (266, 1, 13, 3, 41), dActionEntry (267, 1, 13, 3, 41), dActionEntry (268, 1, 13, 3, 41), dActionEntry (269, 1, 13, 3, 41), 
			dActionEntry (270, 1, 13, 3, 41), dActionEntry (271, 1, 13, 3, 41), dActionEntry (272, 1, 13, 3, 41), dActionEntry (273, 1, 13, 3, 41), 
			dActionEntry (274, 1, 13, 3, 41), dActionEntry (40, 0, 104, 0, 0), dActionEntry (59, 1, 26, 3, 44), dActionEntry (125, 1, 26, 3, 44), 
			dActionEntry (256, 1, 26, 3, 44), dActionEntry (261, 1, 26, 3, 44), dActionEntry (262, 1, 26, 3, 44), dActionEntry (263, 1, 26, 3, 44), 
			dActionEntry (266, 1, 26, 3, 44), dActionEntry (267, 1, 26, 3, 44), dActionEntry (268, 1, 26, 3, 44), dActionEntry (269, 1, 26, 3, 44), 
			dActionEntry (270, 1, 26, 3, 44), dActionEntry (271, 1, 26, 3, 44), dActionEntry (272, 1, 26, 3, 44), dActionEntry (273, 1, 26, 3, 44), 
			dActionEntry (274, 1, 26, 3, 44), dActionEntry (123, 0, 112, 0, 0), dActionEntry (59, 1, 41, 1, 105), dActionEntry (258, 0, 114, 0, 0), 
			dActionEntry (263, 0, 117, 0, 0), dActionEntry (59, 1, 35, 1, 75), dActionEntry (125, 0, 118, 0, 0), dActionEntry (261, 0, 84, 0, 0), 
			dActionEntry (262, 0, 40, 0, 0), dActionEntry (263, 0, 99, 0, 0), dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), dActionEntry (274, 0, 38, 0, 0), dActionEntry (276, 0, 76, 0, 0), 
			dActionEntry (277, 0, 88, 0, 0), dActionEntry (278, 0, 97, 0, 0), dActionEntry (279, 0, 75, 0, 0), dActionEntry (280, 0, 90, 0, 0), 
			dActionEntry (282, 0, 87, 0, 0), dActionEntry (285, 0, 98, 0, 0), dActionEntry (286, 0, 91, 0, 0), dActionEntry (59, 0, 121, 0, 0), 
			dActionEntry (59, 1, 34, 1, 77), dActionEntry (59, 1, 34, 1, 81), dActionEntry (59, 1, 23, 2, 68), dActionEntry (125, 1, 23, 2, 68), 
			dActionEntry (256, 1, 23, 2, 68), dActionEntry (261, 1, 23, 2, 68), dActionEntry (262, 1, 23, 2, 68), dActionEntry (263, 1, 23, 2, 68), 
			dActionEntry (266, 1, 23, 2, 68), dActionEntry (267, 1, 23, 2, 68), dActionEntry (268, 1, 23, 2, 68), dActionEntry (269, 1, 23, 2, 68), 
			dActionEntry (270, 1, 23, 2, 68), dActionEntry (271, 1, 23, 2, 68), dActionEntry (272, 1, 23, 2, 68), dActionEntry (273, 1, 23, 2, 68), 
			dActionEntry (274, 1, 23, 2, 68), dActionEntry (125, 1, 32, 1, 70), dActionEntry (261, 1, 32, 1, 70), dActionEntry (262, 1, 32, 1, 70), 
			dActionEntry (263, 1, 32, 1, 70), dActionEntry (266, 1, 32, 1, 70), dActionEntry (267, 1, 32, 1, 70), dActionEntry (268, 1, 32, 1, 70), 
			dActionEntry (269, 1, 32, 1, 70), dActionEntry (270, 1, 32, 1, 70), dActionEntry (271, 1, 32, 1, 70), dActionEntry (272, 1, 32, 1, 70), 
			dActionEntry (273, 1, 32, 1, 70), dActionEntry (274, 1, 32, 1, 70), dActionEntry (276, 1, 32, 1, 70), dActionEntry (277, 1, 32, 1, 70), 
			dActionEntry (278, 1, 32, 1, 70), dActionEntry (279, 1, 32, 1, 70), dActionEntry (280, 1, 32, 1, 70), dActionEntry (282, 1, 32, 1, 70), 
			dActionEntry (285, 1, 32, 1, 70), dActionEntry (286, 1, 32, 1, 70), dActionEntry (262, 0, 40, 0, 0), dActionEntry (266, 0, 34, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), dActionEntry (270, 0, 21, 0, 0), 
			dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), dActionEntry (274, 0, 38, 0, 0), 
			dActionEntry (59, 1, 34, 1, 82), dActionEntry (59, 1, 35, 1, 74), dActionEntry (61, 0, 124, 0, 0), dActionEntry (40, 0, 125, 0, 0), 
			dActionEntry (40, 0, 126, 0, 0), dActionEntry (40, 0, 128, 0, 0), dActionEntry (59, 1, 40, 1, 96), dActionEntry (59, 1, 34, 1, 80), 
			dActionEntry (59, 1, 34, 1, 78), dActionEntry (59, 1, 34, 1, 79), dActionEntry (59, 1, 34, 1, 76), dActionEntry (61, 0, 129, 0, 0), 
			dActionEntry (40, 0, 130, 0, 0), dActionEntry (59, 1, 39, 1, 95), dActionEntry (40, 0, 133, 0, 0), dActionEntry (61, 1, 42, 1, 83), 
			dActionEntry (91, 0, 131, 0, 0), dActionEntry (263, 0, 134, 0, 0), dActionEntry (59, 1, 13, 3, 39), dActionEntry (125, 1, 13, 3, 39), 
			dActionEntry (256, 1, 13, 3, 39), dActionEntry (261, 1, 13, 3, 39), dActionEntry (262, 1, 13, 3, 39), dActionEntry (263, 1, 13, 3, 39), 
			dActionEntry (266, 1, 13, 3, 39), dActionEntry (267, 1, 13, 3, 39), dActionEntry (268, 1, 13, 3, 39), dActionEntry (269, 1, 13, 3, 39), 
			dActionEntry (270, 1, 13, 3, 39), dActionEntry (271, 1, 13, 3, 39), dActionEntry (272, 1, 13, 3, 39), dActionEntry (273, 1, 13, 3, 39), 
			dActionEntry (274, 1, 13, 3, 39), dActionEntry (91, 0, 45, 0, 0), dActionEntry (263, 1, 25, 3, 51), dActionEntry (258, 0, 66, 0, 0), 
			dActionEntry (263, 0, 70, 0, 0), dActionEntry (91, 1, 31, 3, 67), dActionEntry (263, 1, 31, 3, 67), dActionEntry (41, 0, 141, 0, 0), 
			dActionEntry (258, 0, 138, 0, 0), dActionEntry (263, 0, 142, 0, 0), dActionEntry (41, 1, 17, 1, 30), dActionEntry (44, 0, 144, 0, 0), 
			dActionEntry (41, 1, 15, 1, 27), dActionEntry (44, 1, 15, 1, 27), dActionEntry (41, 0, 145, 0, 0), dActionEntry (263, 0, 146, 0, 0), 
			dActionEntry (125, 0, 148, 0, 0), dActionEntry (261, 0, 84, 0, 0), dActionEntry (262, 0, 40, 0, 0), dActionEntry (263, 0, 99, 0, 0), 
			dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), 
			dActionEntry (274, 0, 38, 0, 0), dActionEntry (276, 0, 76, 0, 0), dActionEntry (277, 0, 88, 0, 0), dActionEntry (278, 0, 97, 0, 0), 
			dActionEntry (279, 0, 75, 0, 0), dActionEntry (280, 0, 90, 0, 0), dActionEntry (282, 0, 87, 0, 0), dActionEntry (285, 0, 98, 0, 0), 
			dActionEntry (286, 0, 91, 0, 0), dActionEntry (278, 0, 149, 0, 0), dActionEntry (59, 1, 28, 1, 109), dActionEntry (288, 1, 28, 1, 109), 
			dActionEntry (59, 1, 28, 1, 108), dActionEntry (288, 1, 28, 1, 108), dActionEntry (59, 1, 41, 2, 106), dActionEntry (288, 0, 150, 0, 0), 
			dActionEntry (40, 0, 151, 0, 0), dActionEntry (59, 1, 23, 3, 69), dActionEntry (125, 1, 23, 3, 69), dActionEntry (256, 1, 23, 3, 69), 
			dActionEntry (261, 1, 23, 3, 69), dActionEntry (262, 1, 23, 3, 69), dActionEntry (263, 1, 23, 3, 69), dActionEntry (266, 1, 23, 3, 69), 
			dActionEntry (267, 1, 23, 3, 69), dActionEntry (268, 1, 23, 3, 69), dActionEntry (269, 1, 23, 3, 69), dActionEntry (270, 1, 23, 3, 69), 
			dActionEntry (271, 1, 23, 3, 69), dActionEntry (272, 1, 23, 3, 69), dActionEntry (273, 1, 23, 3, 69), dActionEntry (274, 1, 23, 3, 69), 
			dActionEntry (125, 1, 32, 2, 71), dActionEntry (261, 1, 32, 2, 71), dActionEntry (262, 1, 32, 2, 71), dActionEntry (263, 1, 32, 2, 71), 
			dActionEntry (266, 1, 32, 2, 71), dActionEntry (267, 1, 32, 2, 71), dActionEntry (268, 1, 32, 2, 71), dActionEntry (269, 1, 32, 2, 71), 
			dActionEntry (270, 1, 32, 2, 71), dActionEntry (271, 1, 32, 2, 71), dActionEntry (272, 1, 32, 2, 71), dActionEntry (273, 1, 32, 2, 71), 
			dActionEntry (274, 1, 32, 2, 71), dActionEntry (276, 1, 32, 2, 71), dActionEntry (277, 1, 32, 2, 71), dActionEntry (278, 1, 32, 2, 71), 
			dActionEntry (279, 1, 32, 2, 71), dActionEntry (280, 1, 32, 2, 71), dActionEntry (282, 1, 32, 2, 71), dActionEntry (285, 1, 32, 2, 71), 
			dActionEntry (286, 1, 32, 2, 71), dActionEntry (59, 0, 152, 0, 0), dActionEntry (125, 1, 21, 1, 35), dActionEntry (261, 1, 21, 1, 35), 
			dActionEntry (262, 1, 21, 1, 35), dActionEntry (263, 1, 21, 1, 35), dActionEntry (266, 1, 21, 1, 35), dActionEntry (267, 1, 21, 1, 35), 
			dActionEntry (268, 1, 21, 1, 35), dActionEntry (269, 1, 21, 1, 35), dActionEntry (270, 1, 21, 1, 35), dActionEntry (271, 1, 21, 1, 35), 
			dActionEntry (272, 1, 21, 1, 35), dActionEntry (273, 1, 21, 1, 35), dActionEntry (274, 1, 21, 1, 35), dActionEntry (276, 1, 21, 1, 35), 
			dActionEntry (277, 1, 21, 1, 35), dActionEntry (278, 1, 21, 1, 35), dActionEntry (279, 1, 21, 1, 35), dActionEntry (280, 1, 21, 1, 35), 
			dActionEntry (282, 1, 21, 1, 35), dActionEntry (285, 1, 21, 1, 35), dActionEntry (286, 1, 21, 1, 35), dActionEntry (125, 1, 33, 2, 72), 
			dActionEntry (261, 1, 33, 2, 72), dActionEntry (262, 1, 33, 2, 72), dActionEntry (263, 1, 33, 2, 72), dActionEntry (266, 1, 33, 2, 72), 
			dActionEntry (267, 1, 33, 2, 72), dActionEntry (268, 1, 33, 2, 72), dActionEntry (269, 1, 33, 2, 72), dActionEntry (270, 1, 33, 2, 72), 
			dActionEntry (271, 1, 33, 2, 72), dActionEntry (272, 1, 33, 2, 72), dActionEntry (273, 1, 33, 2, 72), dActionEntry (274, 1, 33, 2, 72), 
			dActionEntry (276, 1, 33, 2, 72), dActionEntry (277, 1, 33, 2, 72), dActionEntry (278, 1, 33, 2, 72), dActionEntry (279, 1, 33, 2, 72), 
			dActionEntry (280, 1, 33, 2, 72), dActionEntry (282, 1, 33, 2, 72), dActionEntry (285, 1, 33, 2, 72), dActionEntry (286, 1, 33, 2, 72), 
			dActionEntry (263, 0, 153, 0, 0), dActionEntry (258, 0, 114, 0, 0), dActionEntry (263, 0, 117, 0, 0), dActionEntry (258, 0, 155, 0, 0), 
			dActionEntry (263, 0, 158, 0, 0), dActionEntry (266, 0, 165, 0, 0), dActionEntry (267, 0, 159, 0, 0), dActionEntry (268, 0, 163, 0, 0), 
			dActionEntry (269, 0, 166, 0, 0), dActionEntry (270, 0, 160, 0, 0), dActionEntry (271, 0, 164, 0, 0), dActionEntry (272, 0, 162, 0, 0), 
			dActionEntry (273, 0, 168, 0, 0), dActionEntry (274, 0, 167, 0, 0), dActionEntry (125, 1, 33, 2, 73), dActionEntry (261, 1, 33, 2, 73), 
			dActionEntry (262, 1, 33, 2, 73), dActionEntry (263, 1, 33, 2, 73), dActionEntry (266, 1, 33, 2, 73), dActionEntry (267, 1, 33, 2, 73), 
			dActionEntry (268, 1, 33, 2, 73), dActionEntry (269, 1, 33, 2, 73), dActionEntry (270, 1, 33, 2, 73), dActionEntry (271, 1, 33, 2, 73), 
			dActionEntry (272, 1, 33, 2, 73), dActionEntry (273, 1, 33, 2, 73), dActionEntry (274, 1, 33, 2, 73), dActionEntry (276, 1, 33, 2, 73), 
			dActionEntry (277, 1, 33, 2, 73), dActionEntry (278, 1, 33, 2, 73), dActionEntry (279, 1, 33, 2, 73), dActionEntry (280, 1, 33, 2, 73), 
			dActionEntry (282, 1, 33, 2, 73), dActionEntry (285, 1, 33, 2, 73), dActionEntry (286, 1, 33, 2, 73), dActionEntry (258, 0, 114, 0, 0), 
			dActionEntry (263, 0, 117, 0, 0), dActionEntry (275, 0, 170, 0, 0), dActionEntry (61, 1, 42, 2, 84), dActionEntry (91, 0, 174, 0, 0), 
			dActionEntry (41, 0, 175, 0, 0), dActionEntry (258, 0, 138, 0, 0), dActionEntry (263, 0, 142, 0, 0), dActionEntry (59, 1, 26, 2, 43), 
			dActionEntry (61, 1, 26, 2, 43), dActionEntry (41, 0, 177, 0, 0), dActionEntry (41, 0, 178, 0, 0), dActionEntry (93, 1, 28, 3, 107), 
			dActionEntry (288, 1, 28, 3, 107), dActionEntry (41, 1, 28, 1, 109), dActionEntry (44, 1, 28, 1, 109), dActionEntry (288, 1, 28, 1, 109), 
			dActionEntry (41, 1, 28, 1, 108), dActionEntry (44, 1, 28, 1, 108), dActionEntry (288, 1, 28, 1, 108), dActionEntry (41, 1, 46, 1, 101), 
			dActionEntry (44, 1, 46, 1, 101), dActionEntry (288, 0, 179, 0, 0), dActionEntry (93, 1, 20, 3, 103), dActionEntry (288, 1, 20, 3, 103), 
			dActionEntry (40, 0, 180, 0, 0), dActionEntry (41, 0, 182, 0, 0), dActionEntry (44, 0, 181, 0, 0), dActionEntry (58, 0, 185, 0, 0), 
			dActionEntry (123, 1, 19, 0, 33), dActionEntry (41, 1, 16, 2, 47), dActionEntry (44, 1, 16, 2, 47), dActionEntry (125, 0, 186, 0, 0), 
			dActionEntry (261, 0, 84, 0, 0), dActionEntry (262, 0, 40, 0, 0), dActionEntry (263, 0, 99, 0, 0), dActionEntry (266, 0, 34, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), dActionEntry (270, 0, 21, 0, 0), 
			dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), dActionEntry (274, 0, 38, 0, 0), 
			dActionEntry (276, 0, 76, 0, 0), dActionEntry (277, 0, 88, 0, 0), dActionEntry (278, 0, 97, 0, 0), dActionEntry (279, 0, 75, 0, 0), 
			dActionEntry (280, 0, 90, 0, 0), dActionEntry (282, 0, 87, 0, 0), dActionEntry (285, 0, 98, 0, 0), dActionEntry (286, 0, 91, 0, 0), 
			dActionEntry (278, 1, 23, 2, 68), dActionEntry (40, 0, 187, 0, 0), dActionEntry (41, 0, 189, 0, 0), dActionEntry (258, 0, 138, 0, 0), 
			dActionEntry (263, 0, 142, 0, 0), dActionEntry (125, 1, 21, 2, 36), dActionEntry (261, 1, 21, 2, 36), dActionEntry (262, 1, 21, 2, 36), 
			dActionEntry (263, 1, 21, 2, 36), dActionEntry (266, 1, 21, 2, 36), dActionEntry (267, 1, 21, 2, 36), dActionEntry (268, 1, 21, 2, 36), 
			dActionEntry (269, 1, 21, 2, 36), dActionEntry (270, 1, 21, 2, 36), dActionEntry (271, 1, 21, 2, 36), dActionEntry (272, 1, 21, 2, 36), 
			dActionEntry (273, 1, 21, 2, 36), dActionEntry (274, 1, 21, 2, 36), dActionEntry (276, 1, 21, 2, 36), dActionEntry (277, 1, 21, 2, 36), 
			dActionEntry (278, 1, 21, 2, 36), dActionEntry (279, 1, 21, 2, 36), dActionEntry (280, 1, 21, 2, 36), dActionEntry (282, 1, 21, 2, 36), 
			dActionEntry (285, 1, 21, 2, 36), dActionEntry (286, 1, 21, 2, 36), dActionEntry (59, 1, 26, 3, 44), dActionEntry (61, 1, 26, 3, 44), 
			dActionEntry (59, 1, 27, 3, 45), dActionEntry (288, 0, 150, 0, 0), dActionEntry (41, 1, 28, 1, 109), dActionEntry (288, 1, 28, 1, 109), 
			dActionEntry (41, 1, 28, 1, 108), dActionEntry (288, 1, 28, 1, 108), dActionEntry (41, 0, 192, 0, 0), dActionEntry (288, 0, 191, 0, 0), 
			dActionEntry (40, 0, 193, 0, 0), dActionEntry (263, 1, 29, 1, 53), dActionEntry (263, 1, 29, 1, 56), dActionEntry (263, 0, 195, 0, 0), 
			dActionEntry (263, 1, 29, 1, 61), dActionEntry (263, 1, 29, 1, 54), dActionEntry (267, 0, 196, 0, 0), dActionEntry (268, 0, 198, 0, 0), 
			dActionEntry (269, 0, 199, 0, 0), dActionEntry (270, 0, 197, 0, 0), dActionEntry (263, 1, 29, 1, 52), dActionEntry (263, 1, 29, 1, 55), 
			dActionEntry (263, 1, 29, 1, 63), dActionEntry (263, 1, 29, 1, 62), dActionEntry (41, 0, 200, 0, 0), dActionEntry (288, 0, 191, 0, 0), 
			dActionEntry (266, 0, 207, 0, 0), dActionEntry (267, 0, 201, 0, 0), dActionEntry (268, 0, 205, 0, 0), dActionEntry (269, 0, 208, 0, 0), 
			dActionEntry (270, 0, 202, 0, 0), dActionEntry (271, 0, 206, 0, 0), dActionEntry (272, 0, 204, 0, 0), dActionEntry (273, 0, 210, 0, 0), 
			dActionEntry (274, 0, 209, 0, 0), dActionEntry (59, 1, 36, 3, 87), dActionEntry (288, 0, 150, 0, 0), dActionEntry (41, 0, 211, 0, 0), 
			dActionEntry (288, 0, 191, 0, 0), dActionEntry (93, 0, 212, 0, 0), dActionEntry (288, 0, 105, 0, 0), dActionEntry (59, 1, 20, 3, 103), 
			dActionEntry (41, 0, 214, 0, 0), dActionEntry (44, 0, 181, 0, 0), dActionEntry (123, 1, 18, 0, 31), dActionEntry (257, 0, 217, 0, 0), 
			dActionEntry (258, 0, 138, 0, 0), dActionEntry (263, 0, 142, 0, 0), dActionEntry (41, 0, 219, 0, 0), dActionEntry (258, 0, 138, 0, 0), 
			dActionEntry (263, 0, 142, 0, 0), dActionEntry (258, 0, 221, 0, 0), dActionEntry (263, 0, 224, 0, 0), dActionEntry (93, 1, 20, 4, 104), 
			dActionEntry (288, 1, 20, 4, 104), dActionEntry (41, 1, 15, 3, 28), dActionEntry (44, 1, 15, 3, 28), dActionEntry (263, 0, 227, 0, 0), 
			dActionEntry (278, 1, 23, 3, 69), dActionEntry (59, 1, 28, 3, 107), dActionEntry (288, 1, 28, 3, 107), dActionEntry (59, 1, 20, 3, 103), 
			dActionEntry (288, 1, 20, 3, 103), dActionEntry (41, 0, 229, 0, 0), dActionEntry (44, 0, 181, 0, 0), dActionEntry (123, 0, 231, 0, 0), 
			dActionEntry (41, 0, 232, 0, 0), dActionEntry (258, 0, 138, 0, 0), dActionEntry (263, 0, 142, 0, 0), dActionEntry (59, 0, 234, 0, 0), 
			dActionEntry (61, 1, 42, 1, 83), dActionEntry (91, 0, 131, 0, 0), dActionEntry (263, 1, 29, 2, 57), dActionEntry (263, 1, 29, 2, 60), 
			dActionEntry (263, 1, 29, 2, 58), dActionEntry (263, 1, 29, 2, 59), dActionEntry (123, 0, 235, 0, 0), dActionEntry (91, 1, 29, 1, 53), 
			dActionEntry (91, 1, 29, 1, 56), dActionEntry (91, 0, 237, 0, 0), dActionEntry (91, 1, 29, 1, 61), dActionEntry (91, 1, 29, 1, 54), 
			dActionEntry (267, 0, 239, 0, 0), dActionEntry (268, 0, 241, 0, 0), dActionEntry (269, 0, 242, 0, 0), dActionEntry (270, 0, 240, 0, 0), 
			dActionEntry (91, 1, 29, 1, 52), dActionEntry (91, 1, 29, 1, 55), dActionEntry (91, 1, 29, 1, 63), dActionEntry (91, 1, 29, 1, 62), 
			dActionEntry (123, 0, 243, 0, 0), dActionEntry (61, 1, 43, 3, 85), dActionEntry (91, 1, 43, 3, 85), dActionEntry (93, 0, 245, 0, 0), 
			dActionEntry (288, 0, 105, 0, 0), dActionEntry (59, 1, 20, 4, 104), dActionEntry (59, 1, 11, 6, 37), dActionEntry (125, 1, 11, 6, 37), 
			dActionEntry (256, 1, 11, 6, 37), dActionEntry (261, 1, 11, 6, 37), dActionEntry (262, 1, 11, 6, 37), dActionEntry (263, 1, 11, 6, 37), 
			dActionEntry (266, 1, 11, 6, 37), dActionEntry (267, 1, 11, 6, 37), dActionEntry (268, 1, 11, 6, 37), dActionEntry (269, 1, 11, 6, 37), 
			dActionEntry (270, 1, 11, 6, 37), dActionEntry (271, 1, 11, 6, 37), dActionEntry (272, 1, 11, 6, 37), dActionEntry (273, 1, 11, 6, 37), 
			dActionEntry (274, 1, 11, 6, 37), dActionEntry (123, 1, 24, 6, 42), dActionEntry (123, 1, 18, 1, 32), dActionEntry (41, 1, 28, 3, 107), 
			dActionEntry (44, 1, 28, 3, 107), dActionEntry (288, 1, 28, 3, 107), dActionEntry (41, 1, 20, 3, 103), dActionEntry (44, 1, 20, 3, 103), 
			dActionEntry (288, 1, 20, 3, 103), dActionEntry (41, 0, 246, 0, 0), dActionEntry (44, 0, 181, 0, 0), dActionEntry (41, 1, 46, 3, 102), 
			dActionEntry (44, 1, 46, 3, 102), dActionEntry (288, 0, 247, 0, 0), dActionEntry (40, 0, 248, 0, 0), dActionEntry (59, 1, 11, 7, 38), 
			dActionEntry (125, 1, 11, 7, 38), dActionEntry (256, 1, 11, 7, 38), dActionEntry (261, 1, 11, 7, 38), dActionEntry (262, 1, 11, 7, 38), 
			dActionEntry (263, 1, 11, 7, 38), dActionEntry (266, 1, 11, 7, 38), dActionEntry (267, 1, 11, 7, 38), dActionEntry (268, 1, 11, 7, 38), 
			dActionEntry (269, 1, 11, 7, 38), dActionEntry (270, 1, 11, 7, 38), dActionEntry (271, 1, 11, 7, 38), dActionEntry (272, 1, 11, 7, 38), 
			dActionEntry (273, 1, 11, 7, 38), dActionEntry (274, 1, 11, 7, 38), dActionEntry (123, 1, 19, 2, 34), dActionEntry (40, 0, 249, 0, 0), 
			dActionEntry (41, 0, 250, 0, 0), dActionEntry (288, 0, 191, 0, 0), dActionEntry (59, 1, 20, 4, 104), dActionEntry (288, 1, 20, 4, 104), 
			dActionEntry (41, 1, 28, 3, 107), dActionEntry (288, 1, 28, 3, 107), dActionEntry (283, 0, 252, 0, 0), dActionEntry (284, 0, 251, 0, 0), 
			dActionEntry (41, 1, 20, 3, 103), dActionEntry (288, 1, 20, 3, 103), dActionEntry (41, 0, 255, 0, 0), dActionEntry (44, 0, 181, 0, 0), 
			dActionEntry (125, 0, 258, 0, 0), dActionEntry (261, 0, 84, 0, 0), dActionEntry (262, 0, 40, 0, 0), dActionEntry (263, 0, 99, 0, 0), 
			dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), 
			dActionEntry (274, 0, 38, 0, 0), dActionEntry (276, 0, 76, 0, 0), dActionEntry (277, 0, 88, 0, 0), dActionEntry (278, 0, 97, 0, 0), 
			dActionEntry (279, 0, 75, 0, 0), dActionEntry (280, 0, 90, 0, 0), dActionEntry (282, 0, 87, 0, 0), dActionEntry (285, 0, 98, 0, 0), 
			dActionEntry (286, 0, 91, 0, 0), dActionEntry (59, 1, 37, 5, 89), dActionEntry (281, 0, 259, 0, 0), dActionEntry (59, 1, 36, 5, 88), 
			dActionEntry (91, 0, 261, 0, 0), dActionEntry (91, 1, 29, 2, 57), dActionEntry (91, 1, 29, 2, 60), dActionEntry (91, 1, 29, 2, 58), 
			dActionEntry (91, 1, 29, 2, 59), dActionEntry (125, 0, 263, 0, 0), dActionEntry (261, 0, 84, 0, 0), dActionEntry (262, 0, 40, 0, 0), 
			dActionEntry (263, 0, 99, 0, 0), dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), 
			dActionEntry (269, 0, 36, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), 
			dActionEntry (273, 0, 39, 0, 0), dActionEntry (274, 0, 38, 0, 0), dActionEntry (276, 0, 76, 0, 0), dActionEntry (277, 0, 88, 0, 0), 
			dActionEntry (278, 0, 97, 0, 0), dActionEntry (279, 0, 75, 0, 0), dActionEntry (280, 0, 90, 0, 0), dActionEntry (282, 0, 87, 0, 0), 
			dActionEntry (285, 0, 98, 0, 0), dActionEntry (286, 0, 91, 0, 0), dActionEntry (59, 1, 38, 5, 92), dActionEntry (61, 1, 43, 4, 86), 
			dActionEntry (91, 1, 43, 4, 86), dActionEntry (41, 1, 20, 4, 104), dActionEntry (44, 1, 20, 4, 104), dActionEntry (288, 1, 20, 4, 104), 
			dActionEntry (41, 0, 265, 0, 0), dActionEntry (258, 0, 138, 0, 0), dActionEntry (263, 0, 142, 0, 0), dActionEntry (41, 0, 267, 0, 0), 
			dActionEntry (258, 0, 138, 0, 0), dActionEntry (263, 0, 142, 0, 0), dActionEntry (59, 1, 38, 6, 93), dActionEntry (58, 0, 269, 0, 0), 
			dActionEntry (258, 0, 270, 0, 0), dActionEntry (125, 1, 44, 1, 97), dActionEntry (283, 1, 44, 1, 97), dActionEntry (284, 1, 44, 1, 97), 
			dActionEntry (125, 0, 271, 0, 0), dActionEntry (283, 0, 252, 0, 0), dActionEntry (284, 0, 251, 0, 0), dActionEntry (41, 1, 20, 4, 104), 
			dActionEntry (288, 1, 20, 4, 104), dActionEntry (59, 0, 273, 0, 0), dActionEntry (288, 0, 150, 0, 0), dActionEntry (125, 0, 274, 0, 0), 
			dActionEntry (261, 0, 84, 0, 0), dActionEntry (262, 0, 40, 0, 0), dActionEntry (263, 0, 99, 0, 0), dActionEntry (266, 0, 34, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), dActionEntry (270, 0, 21, 0, 0), 
			dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), dActionEntry (274, 0, 38, 0, 0), 
			dActionEntry (276, 0, 76, 0, 0), dActionEntry (277, 0, 88, 0, 0), dActionEntry (278, 0, 97, 0, 0), dActionEntry (279, 0, 75, 0, 0), 
			dActionEntry (280, 0, 90, 0, 0), dActionEntry (282, 0, 87, 0, 0), dActionEntry (285, 0, 98, 0, 0), dActionEntry (286, 0, 91, 0, 0), 
			dActionEntry (59, 1, 23, 2, 68), dActionEntry (281, 1, 23, 2, 68), dActionEntry (93, 0, 276, 0, 0), dActionEntry (288, 0, 105, 0, 0), 
			dActionEntry (125, 0, 278, 0, 0), dActionEntry (261, 0, 84, 0, 0), dActionEntry (262, 0, 40, 0, 0), dActionEntry (263, 0, 99, 0, 0), 
			dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), 
			dActionEntry (274, 0, 38, 0, 0), dActionEntry (276, 0, 76, 0, 0), dActionEntry (277, 0, 88, 0, 0), dActionEntry (278, 0, 97, 0, 0), 
			dActionEntry (279, 0, 75, 0, 0), dActionEntry (280, 0, 90, 0, 0), dActionEntry (282, 0, 87, 0, 0), dActionEntry (285, 0, 98, 0, 0), 
			dActionEntry (286, 0, 91, 0, 0), dActionEntry (59, 1, 23, 2, 68), dActionEntry (41, 0, 279, 0, 0), dActionEntry (44, 0, 181, 0, 0), 
			dActionEntry (123, 1, 20, 3, 103), dActionEntry (41, 0, 280, 0, 0), dActionEntry (44, 0, 181, 0, 0), dActionEntry (123, 0, 281, 0, 0), 
			dActionEntry (58, 0, 283, 0, 0), dActionEntry (59, 1, 37, 7, 91), dActionEntry (125, 1, 44, 2, 98), dActionEntry (283, 1, 44, 2, 98), 
			dActionEntry (284, 1, 44, 2, 98), dActionEntry (59, 1, 23, 3, 69), dActionEntry (281, 1, 23, 3, 69), dActionEntry (59, 1, 37, 7, 90), 
			dActionEntry (59, 1, 43, 3, 85), dActionEntry (91, 1, 43, 3, 85), dActionEntry (93, 0, 286, 0, 0), dActionEntry (288, 0, 105, 0, 0), 
			dActionEntry (59, 1, 23, 3, 69), dActionEntry (123, 1, 20, 4, 104), dActionEntry (125, 0, 288, 0, 0), dActionEntry (261, 0, 84, 0, 0), 
			dActionEntry (262, 0, 40, 0, 0), dActionEntry (263, 0, 99, 0, 0), dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), dActionEntry (274, 0, 38, 0, 0), dActionEntry (276, 0, 76, 0, 0), 
			dActionEntry (277, 0, 88, 0, 0), dActionEntry (278, 0, 97, 0, 0), dActionEntry (279, 0, 75, 0, 0), dActionEntry (280, 0, 90, 0, 0), 
			dActionEntry (282, 0, 87, 0, 0), dActionEntry (285, 0, 98, 0, 0), dActionEntry (286, 0, 91, 0, 0), dActionEntry (125, 1, 45, 3, 100), 
			dActionEntry (283, 1, 45, 3, 100), dActionEntry (284, 1, 45, 3, 100), dActionEntry (41, 0, 290, 0, 0), dActionEntry (61, 0, 291, 0, 0), 
			dActionEntry (59, 1, 43, 4, 86), dActionEntry (91, 1, 43, 4, 86), dActionEntry (125, 0, 292, 0, 0), dActionEntry (261, 0, 84, 0, 0), 
			dActionEntry (262, 0, 40, 0, 0), dActionEntry (263, 0, 99, 0, 0), dActionEntry (266, 0, 34, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 36, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 39, 0, 0), dActionEntry (274, 0, 38, 0, 0), dActionEntry (276, 0, 76, 0, 0), 
			dActionEntry (277, 0, 88, 0, 0), dActionEntry (278, 0, 97, 0, 0), dActionEntry (279, 0, 75, 0, 0), dActionEntry (280, 0, 90, 0, 0), 
			dActionEntry (282, 0, 87, 0, 0), dActionEntry (285, 0, 98, 0, 0), dActionEntry (286, 0, 91, 0, 0), dActionEntry (125, 1, 23, 2, 68), 
			dActionEntry (283, 1, 23, 2, 68), dActionEntry (284, 1, 23, 2, 68), dActionEntry (125, 1, 45, 4, 99), dActionEntry (283, 1, 45, 4, 99), 
			dActionEntry (284, 1, 45, 4, 99), dActionEntry (258, 0, 155, 0, 0), dActionEntry (263, 0, 158, 0, 0), dActionEntry (275, 0, 294, 0, 0), 
			dActionEntry (125, 1, 23, 3, 69), dActionEntry (283, 1, 23, 3, 69), dActionEntry (284, 1, 23, 3, 69), dActionEntry (59, 1, 38, 10, 94), 
			dActionEntry (41, 1, 36, 3, 87), dActionEntry (288, 0, 191, 0, 0), dActionEntry (91, 0, 297, 0, 0), dActionEntry (41, 1, 36, 5, 88), 
			dActionEntry (91, 0, 300, 0, 0), dActionEntry (93, 0, 301, 0, 0), dActionEntry (288, 0, 105, 0, 0), dActionEntry (41, 1, 43, 3, 85), 
			dActionEntry (91, 1, 43, 3, 85), dActionEntry (93, 0, 303, 0, 0), dActionEntry (288, 0, 105, 0, 0), dActionEntry (41, 1, 43, 4, 86), 
			dActionEntry (91, 1, 43, 4, 86)};

	const int lastToken = 295;

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



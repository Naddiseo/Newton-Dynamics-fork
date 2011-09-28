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
			5, 1, 1, 5, 1, 4, 4, 4, 1, 4, 1, 4, 4, 2, 1, 2, 1, 1, 1, 14, 2, 2, 2, 1, 
			2, 14, 14, 12, 1, 2, 14, 4, 14, 2, 11, 2, 14, 2, 2, 1, 1, 14, 1, 12, 4, 2, 2, 1, 
			12, 14, 1, 1, 11, 21, 14, 2, 2, 2, 2, 2, 1, 1, 2, 4, 15, 11, 2, 2, 2, 2, 3, 2, 
			4, 2, 11, 14, 15, 1, 5, 2, 21, 21, 21, 2, 14, 21, 10, 2, 3, 1, 1, 2, 1, 2, 2, 21, 
			2, 2, 1, 1, 2, 3, 1, 1, 15, 14, 15, 11, 1, 2, 11, 3, 2, 2, 2, 2, 3, 2, 4, 3, 
			3, 4, 2, 2, 1, 1, 21, 1, 3, 3, 3, 3, 3, 5, 14, 21, 1, 22, 21, 22, 1, 21, 3, 3, 
			9, 21, 3, 21, 21, 21, 4, 3, 3, 2, 4, 3, 14, 15, 1, 11, 1, 2, 3, 2, 2, 3, 3, 4, 
			2, 3, 3, 3, 3, 3, 2, 3, 5, 2, 10, 2, 2, 21, 1, 1, 3, 2, 3, 4, 4, 21, 22, 3, 
			3, 2, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 2, 9, 3, 2, 2, 3, 2, 2, 2, 1, 2, 2, 
			2, 2, 3, 2, 2, 3, 2, 3, 2, 3, 4, 4, 3, 2, 2, 1, 1, 1, 3, 3, 3, 2, 3, 3, 
			2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 2, 2, 2, 
			14, 3, 1, 1, 3, 2, 2, 3, 3, 3, 2, 3, 3, 2, 3, 3, 3, 3, 3, 5, 14, 1, 1, 2, 
			4, 2, 3, 2, 3, 4, 21, 22, 3, 3, 1, 1, 1, 1, 21, 21, 2, 2, 2, 3, 4, 2, 3, 3, 
			2, 3, 4, 4, 4, 21, 4, 1, 1, 3, 3, 2, 2, 2, 3, 2, 4, 9, 2, 21, 22, 1, 2, 3, 
			21, 21, 4, 3, 3, 2, 3, 3, 2, 1, 2, 1, 1, 21, 3, 3, 1, 2, 3, 3, 4, 1, 22, 21, 
			3, 2, 21, 4, 2, 3, 1, 21, 3, 1, 2, 1, 1, 2, 2, 3, 2, 2, 3, 2, 3, 4, 21, 3, 
			3, 1, 4, 3, 2, 2, 2, 3, 3, 21, 9, 2, 3, 2, 2, 1, 2, 3, 2, 2, 3, 2, 2, 2};
	static short actionsStart[] = {
			0, 5, 6, 7, 12, 13, 17, 21, 25, 26, 30, 31, 35, 39, 41, 42, 44, 45, 46, 47, 61, 63, 65, 67, 
			68, 70, 84, 98, 110, 111, 113, 127, 131, 145, 147, 158, 160, 174, 176, 178, 179, 180, 194, 195, 207, 211, 213, 215, 
			98, 216, 110, 230, 231, 242, 263, 277, 279, 281, 283, 285, 287, 288, 289, 291, 295, 310, 321, 323, 325, 327, 329, 332, 
			334, 338, 340, 351, 365, 380, 381, 386, 388, 409, 430, 451, 453, 467, 488, 451, 498, 501, 502, 451, 503, 504, 451, 506, 
			451, 451, 527, 528, 529, 531, 534, 535, 536, 551, 565, 340, 580, 581, 340, 583, 586, 588, 590, 592, 329, 594, 596, 583, 
			600, 603, 607, 609, 611, 612, 613, 634, 635, 638, 641, 329, 644, 647, 652, 666, 687, 688, 710, 731, 753, 754, 775, 329, 
			778, 787, 329, 808, 829, 850, 871, 329, 583, 875, 877, 881, 884, 898, 913, 340, 914, 915, 329, 917, 919, 583, 921, 924, 
			928, 583, 930, 933, 936, 329, 939, 941, 944, 949, 488, 951, 953, 955, 976, 977, 775, 978, 583, 980, 984, 988, 1009, 1031, 
			1034, 1037, 1039, 1040, 1041, 1042, 1043, 1044, 1048, 1049, 1050, 1051, 1052, 1054, 1063, 1066, 1068, 583, 1070, 1072, 285, 1074, 1075, 1077, 
			1079, 1081, 583, 1083, 1085, 1087, 1090, 1092, 1095, 583, 1097, 1101, 1105, 1108, 1110, 110, 1112, 1113, 329, 1114, 1117, 1120, 583, 1122, 
			1125, 1127, 1128, 1129, 1130, 1132, 1133, 1134, 1135, 1136, 1137, 1138, 1139, 1140, 1141, 1142, 1146, 1147, 1148, 1149, 1150, 1151, 1153, 1155, 
			1157, 1171, 1174, 1175, 1176, 1179, 1181, 1183, 1186, 1189, 1192, 583, 1194, 1197, 930, 933, 1199, 329, 941, 1202, 1207, 1221, 1222, 1223, 
			1225, 1229, 1231, 1234, 1236, 1239, 1243, 1264, 583, 1286, 1289, 1290, 1291, 1292, 1293, 1314, 1335, 1337, 1339, 1341, 1344, 1348, 1350, 1105, 
			1353, 583, 1355, 1359, 1363, 1367, 1388, 1392, 1393, 1394, 1397, 1400, 1402, 1404, 329, 1406, 1408, 1054, 1412, 1414, 1435, 1150, 1457, 583, 
			1459, 1480, 1501, 1186, 1189, 1505, 583, 1194, 1507, 1509, 1510, 1512, 1513, 1514, 1535, 1236, 1041, 1538, 583, 1540, 1543, 1547, 1548, 1570, 
			1591, 1594, 1596, 1344, 1617, 1350, 1619, 1620, 1641, 1512, 1644, 1646, 1647, 1648, 1650, 583, 1652, 1654, 583, 1656, 1658, 1501, 1661, 1682, 
			1685, 1150, 1688, 1692, 1695, 1697, 1699, 583, 1701, 1704, 1054, 1725, 1727, 1730, 1732, 1734, 1735, 583, 1737, 1739, 583, 1741, 1743, 1745};
	static short gotoCount[] = {
			6, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 12, 0, 0, 2, 0, 0, 0, 10, 3, 1, 0, 0, 0, 
			0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 0, 0, 3, 0, 1, 0, 0, 16, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 
			0, 0, 0, 0, 0, 0, 3, 0, 1, 0, 5, 0, 0, 1, 3, 0, 15, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 0, 
			2, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 5, 0, 1, 5, 3, 0, 0, 0, 0, 3, 0, 1, 3, 0, 4, 0, 0, 0, 0, 16, 0, 
			0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 1, 0, 3, 0, 0, 0, 3, 3, 3, 0, 4, 0, 0, 0, 0, 5, 
			0, 0, 3, 0, 0, 3, 0, 4, 0, 3, 0, 0, 0, 3, 0, 0, 1, 0, 3, 1, 0, 15, 0, 0, 3, 0, 3, 0, 4, 0, 0, 0, 
			0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 2, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 3, 
			0, 3, 0, 4, 3, 0, 0, 1, 1, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 
			0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 
			0, 0, 0, 2, 3, 3, 16, 0, 3, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 4, 4, 0, 0, 0, 
			0, 0, 1, 0, 0, 0, 3, 0, 1, 1, 0, 15, 0, 1, 0, 3, 15, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 3, 
			2, 0, 3, 0, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 1, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0, 15, 0, 
			0, 1, 3, 0, 0, 0, 0, 3, 0, 0, 1, 0, 0, 0, 0, 1, 0, 3, 0, 0, 3, 0, 0, 0};
	static short gotoStart[] = {
			0, 6, 6, 6, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 12, 12, 12, 12, 24, 24, 24, 26, 26, 26, 26, 36, 39, 40, 40, 40, 
			40, 40, 40, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 46, 47, 47, 47, 50, 50, 51, 51, 51, 67, 67, 67, 67, 67, 67, 69, 69, 69, 71, 
			71, 71, 71, 71, 71, 71, 71, 74, 74, 75, 75, 80, 80, 80, 81, 84, 84, 99, 99, 99, 101, 101, 101, 103, 105, 105, 105, 105, 107, 107, 107, 109, 
			109, 111, 113, 113, 113, 113, 114, 114, 114, 114, 114, 114, 119, 119, 120, 125, 128, 128, 128, 128, 128, 131, 131, 132, 135, 135, 139, 139, 139, 139, 139, 155, 
			155, 155, 155, 155, 158, 158, 159, 159, 159, 159, 159, 159, 159, 159, 159, 162, 165, 166, 166, 169, 169, 169, 169, 172, 175, 178, 178, 182, 182, 182, 182, 182, 
			187, 187, 187, 190, 190, 190, 193, 193, 197, 197, 200, 200, 200, 200, 203, 203, 203, 204, 204, 207, 208, 208, 223, 223, 223, 226, 226, 229, 229, 233, 233, 233, 
			233, 233, 233, 233, 233, 235, 235, 235, 235, 235, 235, 235, 235, 235, 236, 236, 236, 236, 239, 239, 239, 241, 241, 242, 242, 242, 242, 245, 245, 245, 245, 245, 
			248, 248, 251, 251, 255, 258, 258, 258, 259, 260, 260, 263, 263, 263, 263, 266, 266, 266, 266, 266, 266, 267, 267, 267, 267, 267, 268, 268, 268, 269, 269, 269, 
			269, 269, 269, 269, 269, 270, 270, 270, 270, 270, 271, 271, 271, 271, 271, 271, 271, 271, 271, 271, 274, 274, 274, 274, 274, 274, 277, 277, 278, 278, 278, 278, 
			278, 278, 278, 278, 280, 283, 286, 302, 302, 305, 305, 305, 305, 305, 305, 321, 321, 321, 321, 321, 321, 321, 321, 321, 324, 324, 327, 327, 331, 335, 335, 335, 
			335, 335, 335, 336, 336, 336, 336, 339, 339, 340, 341, 341, 356, 356, 357, 357, 360, 375, 375, 375, 375, 375, 375, 378, 378, 378, 378, 378, 379, 379, 379, 379, 
			382, 384, 384, 387, 387, 391, 392, 392, 392, 392, 392, 392, 392, 392, 392, 392, 408, 408, 409, 409, 409, 409, 409, 409, 412, 412, 412, 415, 415, 415, 415, 430, 
			430, 430, 431, 434, 434, 434, 434, 434, 437, 437, 437, 438, 438, 438, 438, 438, 439, 439, 442, 442, 442, 445, 445, 445};

	static dGotoEntry gotoTable[] = {
			dGotoEntry (295, 8), dGotoEntry (296, 3), dGotoEntry (297, 9), dGotoEntry (298, 5), dGotoEntry (299, 6), 
			dGotoEntry (300, 1), dGotoEntry (297, 12), dGotoEntry (298, 5), dGotoEntry (299, 6), dGotoEntry (300, 1), 
			dGotoEntry (301, 15), dGotoEntry (302, 18), dGotoEntry (300, 23), dGotoEntry (303, 40), dGotoEntry (304, 26), 
			dGotoEntry (305, 36), dGotoEntry (306, 25), dGotoEntry (307, 32), dGotoEntry (308, 41), dGotoEntry (309, 34), 
			dGotoEntry (319, 28), dGotoEntry (320, 42), dGotoEntry (321, 30), dGotoEntry (324, 22), dGotoEntry (325, 45), 
			dGotoEntry (326, 46), dGotoEntry (300, 23), dGotoEntry (305, 49), dGotoEntry (306, 25), dGotoEntry (307, 32), 
			dGotoEntry (308, 41), dGotoEntry (309, 34), dGotoEntry (319, 28), dGotoEntry (320, 42), dGotoEntry (321, 30), 
			dGotoEntry (324, 22), dGotoEntry (319, 50), dGotoEntry (320, 51), dGotoEntry (324, 22), dGotoEntry (318, 54), 
			dGotoEntry (319, 59), dGotoEntry (320, 61), dGotoEntry (324, 22), dGotoEntry (315, 67), dGotoEntry (323, 68), 
			dGotoEntry (337, 71), dGotoEntry (326, 73), dGotoEntry (319, 50), dGotoEntry (320, 51), dGotoEntry (324, 22), 
			dGotoEntry (318, 75), dGotoEntry (315, 83), dGotoEntry (320, 102), dGotoEntry (321, 88), dGotoEntry (322, 79), 
			dGotoEntry (324, 22), dGotoEntry (327, 80), dGotoEntry (328, 85), dGotoEntry (329, 81), dGotoEntry (330, 91), 
			dGotoEntry (331, 97), dGotoEntry (332, 82), dGotoEntry (333, 95), dGotoEntry (334, 96), dGotoEntry (335, 94), 
			dGotoEntry (336, 87), dGotoEntry (337, 98), dGotoEntry (316, 105), dGotoEntry (317, 104), dGotoEntry (325, 109), 
			dGotoEntry (326, 46), dGotoEntry (315, 114), dGotoEntry (323, 115), dGotoEntry (337, 117), dGotoEntry (338, 120), 
			dGotoEntry (310, 122), dGotoEntry (311, 123), dGotoEntry (312, 124), dGotoEntry (320, 125), dGotoEntry (324, 22), 
			dGotoEntry (318, 127), dGotoEntry (315, 129), dGotoEntry (323, 130), dGotoEntry (337, 132), dGotoEntry (315, 83), 
			dGotoEntry (320, 102), dGotoEntry (321, 88), dGotoEntry (322, 79), dGotoEntry (324, 22), dGotoEntry (328, 135), 
			dGotoEntry (329, 81), dGotoEntry (330, 91), dGotoEntry (331, 97), dGotoEntry (332, 82), dGotoEntry (333, 95), 
			dGotoEntry (334, 96), dGotoEntry (335, 94), dGotoEntry (336, 87), dGotoEntry (337, 98), dGotoEntry (316, 138), 
			dGotoEntry (317, 137), dGotoEntry (320, 140), dGotoEntry (324, 22), dGotoEntry (316, 141), dGotoEntry (317, 137), 
			dGotoEntry (316, 145), dGotoEntry (317, 137), dGotoEntry (316, 147), dGotoEntry (317, 137), dGotoEntry (316, 148), 
			dGotoEntry (317, 137), dGotoEntry (316, 149), dGotoEntry (317, 137), dGotoEntry (338, 153), dGotoEntry (310, 122), 
			dGotoEntry (311, 123), dGotoEntry (312, 158), dGotoEntry (320, 125), dGotoEntry (324, 22), dGotoEntry (326, 73), 
			dGotoEntry (310, 122), dGotoEntry (311, 123), dGotoEntry (312, 160), dGotoEntry (320, 125), dGotoEntry (324, 22), 
			dGotoEntry (315, 67), dGotoEntry (323, 161), dGotoEntry (337, 71), dGotoEntry (315, 114), dGotoEntry (323, 164), 
			dGotoEntry (337, 117), dGotoEntry (338, 166), dGotoEntry (315, 67), dGotoEntry (323, 168), dGotoEntry (337, 71), 
			dGotoEntry (315, 171), dGotoEntry (323, 172), dGotoEntry (337, 175), dGotoEntry (341, 177), dGotoEntry (315, 83), 
			dGotoEntry (320, 102), dGotoEntry (321, 88), dGotoEntry (322, 79), dGotoEntry (324, 22), dGotoEntry (327, 181), 
			dGotoEntry (328, 85), dGotoEntry (329, 81), dGotoEntry (330, 91), dGotoEntry (331, 97), dGotoEntry (332, 82), 
			dGotoEntry (333, 95), dGotoEntry (334, 96), dGotoEntry (335, 94), dGotoEntry (336, 87), dGotoEntry (337, 98), 
			dGotoEntry (315, 114), dGotoEntry (323, 185), dGotoEntry (337, 117), dGotoEntry (338, 187), dGotoEntry (315, 129), 
			dGotoEntry (323, 192), dGotoEntry (337, 132), dGotoEntry (315, 114), dGotoEntry (323, 193), dGotoEntry (337, 117), 
			dGotoEntry (324, 196), dGotoEntry (315, 114), dGotoEntry (323, 204), dGotoEntry (337, 117), dGotoEntry (315, 129), 
			dGotoEntry (323, 206), dGotoEntry (337, 132), dGotoEntry (315, 114), dGotoEntry (323, 207), dGotoEntry (337, 117), 
			dGotoEntry (315, 67), dGotoEntry (323, 208), dGotoEntry (337, 71), dGotoEntry (315, 171), dGotoEntry (323, 172), 
			dGotoEntry (337, 175), dGotoEntry (341, 211), dGotoEntry (310, 122), dGotoEntry (311, 123), dGotoEntry (312, 213), 
			dGotoEntry (320, 125), dGotoEntry (324, 22), dGotoEntry (315, 114), dGotoEntry (323, 215), dGotoEntry (337, 117), 
			dGotoEntry (315, 67), dGotoEntry (323, 217), dGotoEntry (337, 71), dGotoEntry (315, 171), dGotoEntry (323, 172), 
			dGotoEntry (337, 175), dGotoEntry (341, 220), dGotoEntry (315, 67), dGotoEntry (323, 222), dGotoEntry (337, 71), 
			dGotoEntry (315, 114), dGotoEntry (323, 224), dGotoEntry (337, 117), dGotoEntry (338, 226), dGotoEntry (311, 230), 
			dGotoEntry (320, 125), dGotoEntry (324, 22), dGotoEntry (314, 231), dGotoEntry (315, 83), dGotoEntry (320, 102), 
			dGotoEntry (321, 88), dGotoEntry (322, 79), dGotoEntry (324, 22), dGotoEntry (328, 135), dGotoEntry (329, 81), 
			dGotoEntry (330, 91), dGotoEntry (331, 97), dGotoEntry (332, 82), dGotoEntry (333, 95), dGotoEntry (334, 96), 
			dGotoEntry (335, 94), dGotoEntry (336, 87), dGotoEntry (337, 98), dGotoEntry (315, 129), dGotoEntry (323, 235), 
			dGotoEntry (337, 132), dGotoEntry (315, 67), dGotoEntry (323, 237), dGotoEntry (337, 71), dGotoEntry (315, 171), 
			dGotoEntry (323, 172), dGotoEntry (337, 175), dGotoEntry (341, 240), dGotoEntry (331, 242), dGotoEntry (337, 243), 
			dGotoEntry (324, 252), dGotoEntry (315, 67), dGotoEntry (323, 262), dGotoEntry (337, 71), dGotoEntry (316, 264), 
			dGotoEntry (317, 104), dGotoEntry (313, 266), dGotoEntry (315, 67), dGotoEntry (323, 269), dGotoEntry (337, 71), 
			dGotoEntry (315, 171), dGotoEntry (323, 272), dGotoEntry (337, 175), dGotoEntry (315, 67), dGotoEntry (323, 274), 
			dGotoEntry (337, 71), dGotoEntry (315, 171), dGotoEntry (323, 172), dGotoEntry (337, 175), dGotoEntry (341, 277), 
			dGotoEntry (315, 279), dGotoEntry (323, 280), dGotoEntry (337, 282), dGotoEntry (318, 284), dGotoEntry (315, 285), 
			dGotoEntry (315, 114), dGotoEntry (323, 287), dGotoEntry (337, 117), dGotoEntry (315, 67), dGotoEntry (323, 289), 
			dGotoEntry (337, 71), dGotoEntry (338, 153), dGotoEntry (318, 295), dGotoEntry (338, 297), dGotoEntry (318, 303), 
			dGotoEntry (313, 305), dGotoEntry (315, 67), dGotoEntry (323, 309), dGotoEntry (337, 71), dGotoEntry (315, 114), 
			dGotoEntry (323, 312), dGotoEntry (337, 117), dGotoEntry (338, 314), dGotoEntry (339, 322), dGotoEntry (340, 321), 
			dGotoEntry (315, 324), dGotoEntry (323, 325), dGotoEntry (337, 327), dGotoEntry (315, 324), dGotoEntry (323, 330), 
			dGotoEntry (337, 327), dGotoEntry (315, 83), dGotoEntry (320, 102), dGotoEntry (321, 88), dGotoEntry (322, 79), 
			dGotoEntry (324, 22), dGotoEntry (327, 331), dGotoEntry (328, 85), dGotoEntry (329, 81), dGotoEntry (330, 91), 
			dGotoEntry (331, 97), dGotoEntry (332, 82), dGotoEntry (333, 95), dGotoEntry (334, 96), dGotoEntry (335, 94), 
			dGotoEntry (336, 87), dGotoEntry (337, 98), dGotoEntry (315, 67), dGotoEntry (323, 334), dGotoEntry (337, 71), 
			dGotoEntry (315, 83), dGotoEntry (320, 102), dGotoEntry (321, 88), dGotoEntry (322, 79), dGotoEntry (324, 22), 
			dGotoEntry (327, 336), dGotoEntry (328, 85), dGotoEntry (329, 81), dGotoEntry (330, 91), dGotoEntry (331, 97), 
			dGotoEntry (332, 82), dGotoEntry (333, 95), dGotoEntry (334, 96), dGotoEntry (335, 94), dGotoEntry (336, 87), 
			dGotoEntry (337, 98), dGotoEntry (315, 279), dGotoEntry (323, 339), dGotoEntry (337, 282), dGotoEntry (315, 67), 
			dGotoEntry (323, 341), dGotoEntry (337, 71), dGotoEntry (315, 171), dGotoEntry (323, 172), dGotoEntry (337, 175), 
			dGotoEntry (341, 344), dGotoEntry (315, 171), dGotoEntry (323, 172), dGotoEntry (337, 175), dGotoEntry (341, 346), 
			dGotoEntry (340, 350), dGotoEntry (315, 114), dGotoEntry (323, 353), dGotoEntry (337, 117), dGotoEntry (338, 355), 
			dGotoEntry (324, 357), dGotoEntry (315, 83), dGotoEntry (320, 102), dGotoEntry (321, 88), dGotoEntry (322, 79), 
			dGotoEntry (324, 22), dGotoEntry (328, 135), dGotoEntry (329, 81), dGotoEntry (330, 91), dGotoEntry (331, 97), 
			dGotoEntry (332, 82), dGotoEntry (333, 95), dGotoEntry (334, 96), dGotoEntry (335, 94), dGotoEntry (336, 87), 
			dGotoEntry (337, 98), dGotoEntry (318, 359), dGotoEntry (315, 67), dGotoEntry (323, 361), dGotoEntry (337, 71), 
			dGotoEntry (315, 83), dGotoEntry (320, 102), dGotoEntry (321, 88), dGotoEntry (322, 79), dGotoEntry (324, 22), 
			dGotoEntry (328, 135), dGotoEntry (329, 81), dGotoEntry (330, 91), dGotoEntry (331, 97), dGotoEntry (332, 82), 
			dGotoEntry (333, 95), dGotoEntry (334, 96), dGotoEntry (335, 94), dGotoEntry (336, 87), dGotoEntry (337, 98), 
			dGotoEntry (315, 67), dGotoEntry (323, 364), dGotoEntry (337, 71), dGotoEntry (318, 368), dGotoEntry (315, 324), 
			dGotoEntry (323, 370), dGotoEntry (337, 327), dGotoEntry (331, 371), dGotoEntry (337, 372), dGotoEntry (315, 67), 
			dGotoEntry (323, 374), dGotoEntry (337, 71), dGotoEntry (315, 171), dGotoEntry (323, 172), dGotoEntry (337, 175), 
			dGotoEntry (341, 377), dGotoEntry (338, 379), dGotoEntry (315, 83), dGotoEntry (320, 102), dGotoEntry (321, 88), 
			dGotoEntry (322, 79), dGotoEntry (324, 22), dGotoEntry (327, 382), dGotoEntry (328, 85), dGotoEntry (329, 81), 
			dGotoEntry (330, 91), dGotoEntry (331, 97), dGotoEntry (332, 82), dGotoEntry (333, 95), dGotoEntry (334, 96), 
			dGotoEntry (335, 94), dGotoEntry (336, 87), dGotoEntry (337, 98), dGotoEntry (318, 384), dGotoEntry (315, 67), 
			dGotoEntry (323, 388), dGotoEntry (337, 71), dGotoEntry (315, 67), dGotoEntry (323, 390), dGotoEntry (337, 71), 
			dGotoEntry (315, 83), dGotoEntry (320, 102), dGotoEntry (321, 88), dGotoEntry (322, 79), dGotoEntry (324, 22), 
			dGotoEntry (328, 135), dGotoEntry (329, 81), dGotoEntry (330, 91), dGotoEntry (331, 97), dGotoEntry (332, 82), 
			dGotoEntry (333, 95), dGotoEntry (334, 96), dGotoEntry (335, 94), dGotoEntry (336, 87), dGotoEntry (337, 98), 
			dGotoEntry (318, 393), dGotoEntry (315, 114), dGotoEntry (323, 395), dGotoEntry (337, 117), dGotoEntry (315, 67), 
			dGotoEntry (323, 398), dGotoEntry (337, 71), dGotoEntry (324, 399), dGotoEntry (338, 402), dGotoEntry (315, 67), 
			dGotoEntry (323, 403), dGotoEntry (337, 71), dGotoEntry (315, 67), dGotoEntry (323, 406), dGotoEntry (337, 71)};
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
			dActionEntry (123, 1, 6, 2, 15), dActionEntry (123, 1, 7, 1, 13), dActionEntry (123, 0, 19, 0, 0), dActionEntry (125, 1, 8, 0, 16), 
			dActionEntry (256, 0, 43, 0, 0), dActionEntry (261, 0, 27, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 1, 5, 0, 10), 
			dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), 
			dActionEntry (274, 0, 37, 0, 0), dActionEntry (91, 1, 29, 1, 54), dActionEntry (263, 1, 29, 1, 54), dActionEntry (91, 1, 29, 1, 57), 
			dActionEntry (263, 1, 29, 1, 57), dActionEntry (91, 0, 44, 0, 0), dActionEntry (263, 1, 25, 1, 49), dActionEntry (263, 0, 47, 0, 0), 
			dActionEntry (91, 1, 29, 1, 62), dActionEntry (263, 1, 29, 1, 62), dActionEntry (125, 1, 10, 1, 20), dActionEntry (256, 1, 10, 1, 20), 
			dActionEntry (261, 1, 10, 1, 20), dActionEntry (262, 1, 10, 1, 20), dActionEntry (263, 1, 10, 1, 20), dActionEntry (266, 1, 10, 1, 20), 
			dActionEntry (267, 1, 10, 1, 20), dActionEntry (268, 1, 10, 1, 20), dActionEntry (269, 1, 10, 1, 20), dActionEntry (270, 1, 10, 1, 20), 
			dActionEntry (271, 1, 10, 1, 20), dActionEntry (272, 1, 10, 1, 20), dActionEntry (273, 1, 10, 1, 20), dActionEntry (274, 1, 10, 1, 20), 
			dActionEntry (125, 1, 8, 1, 17), dActionEntry (256, 0, 43, 0, 0), dActionEntry (261, 0, 48, 0, 0), dActionEntry (262, 0, 39, 0, 0), 
			dActionEntry (263, 1, 5, 0, 10), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), 
			dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), 
			dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (256, 0, 52, 0, 0), dActionEntry (262, 0, 39, 0, 0), 
			dActionEntry (263, 1, 5, 1, 11), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), 
			dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), 
			dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (123, 0, 53, 0, 0), dActionEntry (91, 1, 29, 1, 55), 
			dActionEntry (263, 1, 29, 1, 55), dActionEntry (125, 1, 12, 1, 47), dActionEntry (256, 1, 12, 1, 47), dActionEntry (261, 1, 12, 1, 47), 
			dActionEntry (262, 1, 12, 1, 47), dActionEntry (263, 1, 12, 1, 47), dActionEntry (266, 1, 12, 1, 47), dActionEntry (267, 1, 12, 1, 47), 
			dActionEntry (268, 1, 12, 1, 47), dActionEntry (269, 1, 12, 1, 47), dActionEntry (270, 1, 12, 1, 47), dActionEntry (271, 1, 12, 1, 47), 
			dActionEntry (272, 1, 12, 1, 47), dActionEntry (273, 1, 12, 1, 47), dActionEntry (274, 1, 12, 1, 47), dActionEntry (267, 0, 55, 0, 0), 
			dActionEntry (268, 0, 57, 0, 0), dActionEntry (269, 0, 58, 0, 0), dActionEntry (270, 0, 56, 0, 0), dActionEntry (125, 1, 10, 1, 21), 
			dActionEntry (256, 1, 10, 1, 21), dActionEntry (261, 1, 10, 1, 21), dActionEntry (262, 1, 10, 1, 21), dActionEntry (263, 1, 10, 1, 21), 
			dActionEntry (266, 1, 10, 1, 21), dActionEntry (267, 1, 10, 1, 21), dActionEntry (268, 1, 10, 1, 21), dActionEntry (269, 1, 10, 1, 21), 
			dActionEntry (270, 1, 10, 1, 21), dActionEntry (271, 1, 10, 1, 21), dActionEntry (272, 1, 10, 1, 21), dActionEntry (273, 1, 10, 1, 21), 
			dActionEntry (274, 1, 10, 1, 21), dActionEntry (91, 1, 29, 1, 53), dActionEntry (263, 1, 29, 1, 53), dActionEntry (262, 0, 39, 0, 0), 
			dActionEntry (263, 0, 60, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), 
			dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), 
			dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (91, 1, 29, 1, 56), dActionEntry (263, 1, 29, 1, 56), 
			dActionEntry (125, 1, 9, 1, 18), dActionEntry (256, 1, 9, 1, 18), dActionEntry (261, 1, 9, 1, 18), dActionEntry (262, 1, 9, 1, 18), 
			dActionEntry (263, 1, 9, 1, 18), dActionEntry (266, 1, 9, 1, 18), dActionEntry (267, 1, 9, 1, 18), dActionEntry (268, 1, 9, 1, 18), 
			dActionEntry (269, 1, 9, 1, 18), dActionEntry (270, 1, 9, 1, 18), dActionEntry (271, 1, 9, 1, 18), dActionEntry (272, 1, 9, 1, 18), 
			dActionEntry (273, 1, 9, 1, 18), dActionEntry (274, 1, 9, 1, 18), dActionEntry (91, 1, 29, 1, 64), dActionEntry (263, 1, 29, 1, 64), 
			dActionEntry (91, 1, 29, 1, 63), dActionEntry (263, 1, 29, 1, 63), dActionEntry (263, 0, 62, 0, 0), dActionEntry (125, 0, 63, 0, 0), 
			dActionEntry (125, 1, 10, 1, 22), dActionEntry (256, 1, 10, 1, 22), dActionEntry (261, 1, 10, 1, 22), dActionEntry (262, 1, 10, 1, 22), 
			dActionEntry (263, 1, 10, 1, 22), dActionEntry (266, 1, 10, 1, 22), dActionEntry (267, 1, 10, 1, 22), dActionEntry (268, 1, 10, 1, 22), 
			dActionEntry (269, 1, 10, 1, 22), dActionEntry (270, 1, 10, 1, 22), dActionEntry (271, 1, 10, 1, 22), dActionEntry (272, 1, 10, 1, 22), 
			dActionEntry (273, 1, 10, 1, 22), dActionEntry (274, 1, 10, 1, 22), dActionEntry (263, 0, 64, 0, 0), dActionEntry (261, 0, 65, 0, 0), 
			dActionEntry (262, 1, 14, 1, 23), dActionEntry (263, 1, 14, 1, 23), dActionEntry (266, 1, 14, 1, 23), dActionEntry (267, 1, 14, 1, 23), 
			dActionEntry (268, 1, 14, 1, 23), dActionEntry (269, 1, 14, 1, 23), dActionEntry (270, 1, 14, 1, 23), dActionEntry (271, 1, 14, 1, 23), 
			dActionEntry (272, 1, 14, 1, 23), dActionEntry (273, 1, 14, 1, 23), dActionEntry (274, 1, 14, 1, 23), dActionEntry (40, 0, 70, 0, 0), 
			dActionEntry (93, 0, 69, 0, 0), dActionEntry (258, 0, 66, 0, 0), dActionEntry (263, 0, 72, 0, 0), dActionEntry (91, 0, 44, 0, 0), 
			dActionEntry (263, 1, 25, 2, 51), dActionEntry (91, 1, 30, 1, 65), dActionEntry (263, 1, 30, 1, 65), dActionEntry (40, 0, 74, 0, 0), 
			dActionEntry (125, 1, 9, 2, 19), dActionEntry (256, 1, 9, 2, 19), dActionEntry (261, 1, 9, 2, 19), dActionEntry (262, 1, 9, 2, 19), 
			dActionEntry (263, 1, 9, 2, 19), dActionEntry (266, 1, 9, 2, 19), dActionEntry (267, 1, 9, 2, 19), dActionEntry (268, 1, 9, 2, 19), 
			dActionEntry (269, 1, 9, 2, 19), dActionEntry (270, 1, 9, 2, 19), dActionEntry (271, 1, 9, 2, 19), dActionEntry (272, 1, 9, 2, 19), 
			dActionEntry (273, 1, 9, 2, 19), dActionEntry (274, 1, 9, 2, 19), dActionEntry (263, 0, 76, 0, 0), dActionEntry (262, 1, 14, 2, 25), 
			dActionEntry (263, 1, 14, 2, 25), dActionEntry (266, 1, 14, 2, 25), dActionEntry (267, 1, 14, 2, 25), dActionEntry (268, 1, 14, 2, 25), 
			dActionEntry (269, 1, 14, 2, 25), dActionEntry (270, 1, 14, 2, 25), dActionEntry (271, 1, 14, 2, 25), dActionEntry (272, 1, 14, 2, 25), 
			dActionEntry (273, 1, 14, 2, 25), dActionEntry (274, 1, 14, 2, 25), dActionEntry (125, 0, 84, 0, 0), dActionEntry (261, 0, 86, 0, 0), 
			dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 101, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 78, 0, 0), 
			dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 99, 0, 0), dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), 
			dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 100, 0, 0), dActionEntry (286, 0, 93, 0, 0), dActionEntry (125, 1, 13, 2, 41), 
			dActionEntry (256, 1, 13, 2, 41), dActionEntry (261, 1, 13, 2, 41), dActionEntry (262, 1, 13, 2, 41), dActionEntry (263, 1, 13, 2, 41), 
			dActionEntry (266, 1, 13, 2, 41), dActionEntry (267, 1, 13, 2, 41), dActionEntry (268, 1, 13, 2, 41), dActionEntry (269, 1, 13, 2, 41), 
			dActionEntry (270, 1, 13, 2, 41), dActionEntry (271, 1, 13, 2, 41), dActionEntry (272, 1, 13, 2, 41), dActionEntry (273, 1, 13, 2, 41), 
			dActionEntry (274, 1, 13, 2, 41), dActionEntry (91, 1, 29, 2, 58), dActionEntry (263, 1, 29, 2, 58), dActionEntry (91, 1, 29, 2, 61), 
			dActionEntry (263, 1, 29, 2, 61), dActionEntry (91, 1, 29, 2, 59), dActionEntry (263, 1, 29, 2, 59), dActionEntry (91, 1, 29, 2, 60), 
			dActionEntry (263, 1, 29, 2, 60), dActionEntry (59, 0, 106, 0, 0), dActionEntry (254, 0, 103, 0, 0), dActionEntry (40, 0, 107, 0, 0), 
			dActionEntry (263, 0, 108, 0, 0), dActionEntry (91, 0, 44, 0, 0), dActionEntry (263, 1, 25, 2, 50), dActionEntry (59, 1, 4, 8, 9), 
			dActionEntry (255, 1, 4, 8, 9), dActionEntry (259, 1, 4, 8, 9), dActionEntry (262, 1, 4, 8, 9), dActionEntry (40, 0, 110, 0, 0), 
			dActionEntry (125, 1, 26, 2, 44), dActionEntry (256, 1, 26, 2, 44), dActionEntry (261, 1, 26, 2, 44), dActionEntry (262, 1, 26, 2, 44), 
			dActionEntry (263, 1, 26, 2, 44), dActionEntry (266, 1, 26, 2, 44), dActionEntry (267, 1, 26, 2, 44), dActionEntry (268, 1, 26, 2, 44), 
			dActionEntry (269, 1, 26, 2, 44), dActionEntry (270, 1, 26, 2, 44), dActionEntry (271, 1, 26, 2, 44), dActionEntry (272, 1, 26, 2, 44), 
			dActionEntry (273, 1, 26, 2, 44), dActionEntry (274, 1, 26, 2, 44), dActionEntry (262, 1, 14, 2, 24), dActionEntry (263, 1, 14, 2, 24), 
			dActionEntry (266, 1, 14, 2, 24), dActionEntry (267, 1, 14, 2, 24), dActionEntry (268, 1, 14, 2, 24), dActionEntry (269, 1, 14, 2, 24), 
			dActionEntry (270, 1, 14, 2, 24), dActionEntry (271, 1, 14, 2, 24), dActionEntry (272, 1, 14, 2, 24), dActionEntry (273, 1, 14, 2, 24), 
			dActionEntry (274, 1, 14, 2, 24), dActionEntry (93, 1, 28, 1, 112), dActionEntry (288, 1, 28, 1, 112), dActionEntry (93, 1, 28, 1, 111), 
			dActionEntry (288, 1, 28, 1, 111), dActionEntry (93, 0, 112, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (91, 1, 31, 2, 67), 
			dActionEntry (263, 1, 31, 2, 67), dActionEntry (40, 0, 116, 0, 0), dActionEntry (258, 0, 113, 0, 0), dActionEntry (263, 0, 118, 0, 0), 
			dActionEntry (93, 1, 28, 1, 110), dActionEntry (288, 1, 28, 1, 110), dActionEntry (40, 0, 121, 0, 0), dActionEntry (91, 0, 119, 0, 0), 
			dActionEntry (93, 1, 42, 1, 84), dActionEntry (288, 1, 42, 1, 84), dActionEntry (91, 1, 30, 2, 66), dActionEntry (263, 1, 30, 2, 66), 
			dActionEntry (41, 1, 17, 0, 28), dActionEntry (262, 0, 39, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (125, 1, 13, 3, 42), 
			dActionEntry (256, 1, 13, 3, 42), dActionEntry (261, 1, 13, 3, 42), dActionEntry (262, 1, 13, 3, 42), dActionEntry (263, 1, 13, 3, 42), 
			dActionEntry (266, 1, 13, 3, 42), dActionEntry (267, 1, 13, 3, 42), dActionEntry (268, 1, 13, 3, 42), dActionEntry (269, 1, 13, 3, 42), 
			dActionEntry (270, 1, 13, 3, 42), dActionEntry (271, 1, 13, 3, 42), dActionEntry (272, 1, 13, 3, 42), dActionEntry (273, 1, 13, 3, 42), 
			dActionEntry (274, 1, 13, 3, 42), dActionEntry (40, 0, 110, 0, 0), dActionEntry (125, 1, 26, 3, 45), dActionEntry (256, 1, 26, 3, 45), 
			dActionEntry (261, 1, 26, 3, 45), dActionEntry (262, 1, 26, 3, 45), dActionEntry (263, 1, 26, 3, 45), dActionEntry (266, 1, 26, 3, 45), 
			dActionEntry (267, 1, 26, 3, 45), dActionEntry (268, 1, 26, 3, 45), dActionEntry (269, 1, 26, 3, 45), dActionEntry (270, 1, 26, 3, 45), 
			dActionEntry (271, 1, 26, 3, 45), dActionEntry (272, 1, 26, 3, 45), dActionEntry (273, 1, 26, 3, 45), dActionEntry (274, 1, 26, 3, 45), 
			dActionEntry (123, 0, 126, 0, 0), dActionEntry (40, 0, 131, 0, 0), dActionEntry (59, 1, 41, 1, 106), dActionEntry (254, 1, 41, 1, 106), 
			dActionEntry (258, 0, 128, 0, 0), dActionEntry (263, 0, 133, 0, 0), dActionEntry (59, 1, 35, 1, 76), dActionEntry (254, 1, 35, 1, 76), 
			dActionEntry (125, 0, 134, 0, 0), dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 101, 0, 0), 
			dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), 
			dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 78, 0, 0), dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 99, 0, 0), 
			dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 100, 0, 0), 
			dActionEntry (286, 0, 93, 0, 0), dActionEntry (125, 1, 33, 1, 73), dActionEntry (261, 1, 33, 1, 73), dActionEntry (262, 1, 33, 1, 73), 
			dActionEntry (263, 1, 33, 1, 73), dActionEntry (266, 1, 33, 1, 73), dActionEntry (267, 1, 33, 1, 73), dActionEntry (268, 1, 33, 1, 73), 
			dActionEntry (269, 1, 33, 1, 73), dActionEntry (270, 1, 33, 1, 73), dActionEntry (271, 1, 33, 1, 73), dActionEntry (272, 1, 33, 1, 73), 
			dActionEntry (273, 1, 33, 1, 73), dActionEntry (274, 1, 33, 1, 73), dActionEntry (276, 1, 33, 1, 73), dActionEntry (277, 1, 33, 1, 73), 
			dActionEntry (278, 1, 33, 1, 73), dActionEntry (279, 1, 33, 1, 73), dActionEntry (280, 1, 33, 1, 73), dActionEntry (282, 1, 33, 1, 73), 
			dActionEntry (285, 1, 33, 1, 73), dActionEntry (286, 1, 33, 1, 73), dActionEntry (125, 1, 34, 1, 78), dActionEntry (261, 1, 34, 1, 78), 
			dActionEntry (262, 1, 34, 1, 78), dActionEntry (263, 1, 34, 1, 78), dActionEntry (266, 1, 34, 1, 78), dActionEntry (267, 1, 34, 1, 78), 
			dActionEntry (268, 1, 34, 1, 78), dActionEntry (269, 1, 34, 1, 78), dActionEntry (270, 1, 34, 1, 78), dActionEntry (271, 1, 34, 1, 78), 
			dActionEntry (272, 1, 34, 1, 78), dActionEntry (273, 1, 34, 1, 78), dActionEntry (274, 1, 34, 1, 78), dActionEntry (276, 1, 34, 1, 78), 
			dActionEntry (277, 1, 34, 1, 78), dActionEntry (278, 1, 34, 1, 78), dActionEntry (279, 1, 34, 1, 78), dActionEntry (280, 1, 34, 1, 78), 
			dActionEntry (282, 1, 34, 1, 78), dActionEntry (285, 1, 34, 1, 78), dActionEntry (286, 1, 34, 1, 78), dActionEntry (59, 0, 139, 0, 0), 
			dActionEntry (254, 0, 136, 0, 0), dActionEntry (125, 1, 23, 2, 69), dActionEntry (256, 1, 23, 2, 69), dActionEntry (261, 1, 23, 2, 69), 
			dActionEntry (262, 1, 23, 2, 69), dActionEntry (263, 1, 23, 2, 69), dActionEntry (266, 1, 23, 2, 69), dActionEntry (267, 1, 23, 2, 69), 
			dActionEntry (268, 1, 23, 2, 69), dActionEntry (269, 1, 23, 2, 69), dActionEntry (270, 1, 23, 2, 69), dActionEntry (271, 1, 23, 2, 69), 
			dActionEntry (272, 1, 23, 2, 69), dActionEntry (273, 1, 23, 2, 69), dActionEntry (274, 1, 23, 2, 69), dActionEntry (125, 1, 32, 1, 71), 
			dActionEntry (261, 1, 32, 1, 71), dActionEntry (262, 1, 32, 1, 71), dActionEntry (263, 1, 32, 1, 71), dActionEntry (266, 1, 32, 1, 71), 
			dActionEntry (267, 1, 32, 1, 71), dActionEntry (268, 1, 32, 1, 71), dActionEntry (269, 1, 32, 1, 71), dActionEntry (270, 1, 32, 1, 71), 
			dActionEntry (271, 1, 32, 1, 71), dActionEntry (272, 1, 32, 1, 71), dActionEntry (273, 1, 32, 1, 71), dActionEntry (274, 1, 32, 1, 71), 
			dActionEntry (276, 1, 32, 1, 71), dActionEntry (277, 1, 32, 1, 71), dActionEntry (278, 1, 32, 1, 71), dActionEntry (279, 1, 32, 1, 71), 
			dActionEntry (280, 1, 32, 1, 71), dActionEntry (282, 1, 32, 1, 71), dActionEntry (285, 1, 32, 1, 71), dActionEntry (286, 1, 32, 1, 71), 
			dActionEntry (262, 0, 39, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), 
			dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), 
			dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (59, 1, 35, 1, 75), dActionEntry (61, 0, 142, 0, 0), 
			dActionEntry (254, 1, 35, 1, 75), dActionEntry (40, 0, 143, 0, 0), dActionEntry (40, 0, 144, 0, 0), dActionEntry (40, 0, 146, 0, 0), 
			dActionEntry (59, 1, 40, 1, 97), dActionEntry (254, 1, 40, 1, 97), dActionEntry (125, 1, 34, 1, 79), dActionEntry (261, 1, 34, 1, 79), 
			dActionEntry (262, 1, 34, 1, 79), dActionEntry (263, 1, 34, 1, 79), dActionEntry (266, 1, 34, 1, 79), dActionEntry (267, 1, 34, 1, 79), 
			dActionEntry (268, 1, 34, 1, 79), dActionEntry (269, 1, 34, 1, 79), dActionEntry (270, 1, 34, 1, 79), dActionEntry (271, 1, 34, 1, 79), 
			dActionEntry (272, 1, 34, 1, 79), dActionEntry (273, 1, 34, 1, 79), dActionEntry (274, 1, 34, 1, 79), dActionEntry (276, 1, 34, 1, 79), 
			dActionEntry (277, 1, 34, 1, 79), dActionEntry (278, 1, 34, 1, 79), dActionEntry (279, 1, 34, 1, 79), dActionEntry (280, 1, 34, 1, 79), 
			dActionEntry (282, 1, 34, 1, 79), dActionEntry (285, 1, 34, 1, 79), dActionEntry (286, 1, 34, 1, 79), dActionEntry (61, 0, 150, 0, 0), 
			dActionEntry (40, 0, 151, 0, 0), dActionEntry (59, 1, 39, 1, 96), dActionEntry (254, 1, 39, 1, 96), dActionEntry (40, 0, 154, 0, 0), 
			dActionEntry (61, 1, 42, 1, 84), dActionEntry (91, 0, 152, 0, 0), dActionEntry (263, 0, 155, 0, 0), dActionEntry (59, 0, 156, 0, 0), 
			dActionEntry (59, 0, 157, 0, 0), dActionEntry (125, 1, 21, 1, 34), dActionEntry (256, 1, 21, 1, 34), dActionEntry (261, 1, 21, 1, 34), 
			dActionEntry (262, 1, 21, 1, 34), dActionEntry (263, 1, 21, 1, 34), dActionEntry (266, 1, 21, 1, 34), dActionEntry (267, 1, 21, 1, 34), 
			dActionEntry (268, 1, 21, 1, 34), dActionEntry (269, 1, 21, 1, 34), dActionEntry (270, 1, 21, 1, 34), dActionEntry (271, 1, 21, 1, 34), 
			dActionEntry (272, 1, 21, 1, 34), dActionEntry (273, 1, 21, 1, 34), dActionEntry (274, 1, 21, 1, 34), dActionEntry (125, 1, 13, 3, 40), 
			dActionEntry (256, 1, 13, 3, 40), dActionEntry (261, 1, 13, 3, 40), dActionEntry (262, 1, 13, 3, 40), dActionEntry (263, 1, 13, 3, 40), 
			dActionEntry (266, 1, 13, 3, 40), dActionEntry (267, 1, 13, 3, 40), dActionEntry (268, 1, 13, 3, 40), dActionEntry (269, 1, 13, 3, 40), 
			dActionEntry (270, 1, 13, 3, 40), dActionEntry (271, 1, 13, 3, 40), dActionEntry (272, 1, 13, 3, 40), dActionEntry (273, 1, 13, 3, 40), 
			dActionEntry (274, 1, 13, 3, 40), dActionEntry (59, 1, 22, 1, 36), dActionEntry (125, 1, 22, 1, 36), dActionEntry (256, 1, 22, 1, 36), 
			dActionEntry (261, 1, 22, 1, 36), dActionEntry (262, 1, 22, 1, 36), dActionEntry (263, 1, 22, 1, 36), dActionEntry (266, 1, 22, 1, 36), 
			dActionEntry (267, 1, 22, 1, 36), dActionEntry (268, 1, 22, 1, 36), dActionEntry (269, 1, 22, 1, 36), dActionEntry (270, 1, 22, 1, 36), 
			dActionEntry (271, 1, 22, 1, 36), dActionEntry (272, 1, 22, 1, 36), dActionEntry (273, 1, 22, 1, 36), dActionEntry (274, 1, 22, 1, 36), 
			dActionEntry (40, 0, 159, 0, 0), dActionEntry (91, 0, 44, 0, 0), dActionEntry (263, 1, 25, 3, 52), dActionEntry (40, 0, 70, 0, 0), 
			dActionEntry (258, 0, 66, 0, 0), dActionEntry (263, 0, 72, 0, 0), dActionEntry (91, 1, 31, 3, 68), dActionEntry (263, 1, 31, 3, 68), 
			dActionEntry (41, 1, 28, 1, 112), dActionEntry (288, 1, 28, 1, 112), dActionEntry (41, 1, 28, 1, 111), dActionEntry (288, 1, 28, 1, 111), 
			dActionEntry (41, 0, 163, 0, 0), dActionEntry (288, 0, 162, 0, 0), dActionEntry (41, 1, 28, 1, 110), dActionEntry (288, 1, 28, 1, 110), 
			dActionEntry (40, 0, 167, 0, 0), dActionEntry (41, 1, 42, 1, 84), dActionEntry (91, 0, 165, 0, 0), dActionEntry (288, 1, 42, 1, 84), 
			dActionEntry (91, 0, 169, 0, 0), dActionEntry (93, 1, 42, 2, 85), dActionEntry (288, 1, 42, 2, 85), dActionEntry (40, 0, 173, 0, 0), 
			dActionEntry (41, 0, 174, 0, 0), dActionEntry (258, 0, 170, 0, 0), dActionEntry (263, 0, 176, 0, 0), dActionEntry (41, 1, 17, 1, 29), 
			dActionEntry (44, 0, 178, 0, 0), dActionEntry (41, 1, 15, 1, 26), dActionEntry (44, 1, 15, 1, 26), dActionEntry (41, 0, 179, 0, 0), 
			dActionEntry (263, 0, 180, 0, 0), dActionEntry (125, 0, 182, 0, 0), dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), 
			dActionEntry (263, 0, 101, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), 
			dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), 
			dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 78, 0, 0), dActionEntry (277, 0, 90, 0, 0), 
			dActionEntry (278, 0, 99, 0, 0), dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), 
			dActionEntry (285, 0, 100, 0, 0), dActionEntry (286, 0, 93, 0, 0), dActionEntry (278, 0, 183, 0, 0), dActionEntry (59, 1, 28, 1, 112), 
			dActionEntry (254, 1, 28, 1, 112), dActionEntry (288, 1, 28, 1, 112), dActionEntry (59, 1, 28, 1, 111), dActionEntry (254, 1, 28, 1, 111), 
			dActionEntry (288, 1, 28, 1, 111), dActionEntry (59, 1, 41, 2, 107), dActionEntry (254, 1, 41, 2, 107), dActionEntry (288, 0, 184, 0, 0), 
			dActionEntry (59, 1, 28, 1, 110), dActionEntry (254, 1, 28, 1, 110), dActionEntry (288, 1, 28, 1, 110), dActionEntry (40, 0, 188, 0, 0), 
			dActionEntry (59, 1, 42, 1, 84), dActionEntry (91, 0, 186, 0, 0), dActionEntry (254, 1, 42, 1, 84), dActionEntry (288, 1, 42, 1, 84), 
			dActionEntry (125, 1, 23, 3, 70), dActionEntry (256, 1, 23, 3, 70), dActionEntry (261, 1, 23, 3, 70), dActionEntry (262, 1, 23, 3, 70), 
			dActionEntry (263, 1, 23, 3, 70), dActionEntry (266, 1, 23, 3, 70), dActionEntry (267, 1, 23, 3, 70), dActionEntry (268, 1, 23, 3, 70), 
			dActionEntry (269, 1, 23, 3, 70), dActionEntry (270, 1, 23, 3, 70), dActionEntry (271, 1, 23, 3, 70), dActionEntry (272, 1, 23, 3, 70), 
			dActionEntry (273, 1, 23, 3, 70), dActionEntry (274, 1, 23, 3, 70), dActionEntry (125, 1, 32, 2, 72), dActionEntry (261, 1, 32, 2, 72), 
			dActionEntry (262, 1, 32, 2, 72), dActionEntry (263, 1, 32, 2, 72), dActionEntry (266, 1, 32, 2, 72), dActionEntry (267, 1, 32, 2, 72), 
			dActionEntry (268, 1, 32, 2, 72), dActionEntry (269, 1, 32, 2, 72), dActionEntry (270, 1, 32, 2, 72), dActionEntry (271, 1, 32, 2, 72), 
			dActionEntry (272, 1, 32, 2, 72), dActionEntry (273, 1, 32, 2, 72), dActionEntry (274, 1, 32, 2, 72), dActionEntry (276, 1, 32, 2, 72), 
			dActionEntry (277, 1, 32, 2, 72), dActionEntry (278, 1, 32, 2, 72), dActionEntry (279, 1, 32, 2, 72), dActionEntry (280, 1, 32, 2, 72), 
			dActionEntry (282, 1, 32, 2, 72), dActionEntry (285, 1, 32, 2, 72), dActionEntry (286, 1, 32, 2, 72), dActionEntry (59, 0, 189, 0, 0), 
			dActionEntry (59, 0, 190, 0, 0), dActionEntry (125, 1, 21, 1, 34), dActionEntry (261, 1, 21, 1, 34), dActionEntry (262, 1, 21, 1, 34), 
			dActionEntry (263, 1, 21, 1, 34), dActionEntry (266, 1, 21, 1, 34), dActionEntry (267, 1, 21, 1, 34), dActionEntry (268, 1, 21, 1, 34), 
			dActionEntry (269, 1, 21, 1, 34), dActionEntry (270, 1, 21, 1, 34), dActionEntry (271, 1, 21, 1, 34), dActionEntry (272, 1, 21, 1, 34), 
			dActionEntry (273, 1, 21, 1, 34), dActionEntry (274, 1, 21, 1, 34), dActionEntry (276, 1, 21, 1, 34), dActionEntry (277, 1, 21, 1, 34), 
			dActionEntry (278, 1, 21, 1, 34), dActionEntry (279, 1, 21, 1, 34), dActionEntry (280, 1, 21, 1, 34), dActionEntry (282, 1, 21, 1, 34), 
			dActionEntry (285, 1, 21, 1, 34), dActionEntry (286, 1, 21, 1, 34), dActionEntry (125, 1, 34, 2, 82), dActionEntry (261, 1, 34, 2, 82), 
			dActionEntry (262, 1, 34, 2, 82), dActionEntry (263, 1, 34, 2, 82), dActionEntry (266, 1, 34, 2, 82), dActionEntry (267, 1, 34, 2, 82), 
			dActionEntry (268, 1, 34, 2, 82), dActionEntry (269, 1, 34, 2, 82), dActionEntry (270, 1, 34, 2, 82), dActionEntry (271, 1, 34, 2, 82), 
			dActionEntry (272, 1, 34, 2, 82), dActionEntry (273, 1, 34, 2, 82), dActionEntry (274, 1, 34, 2, 82), dActionEntry (276, 1, 34, 2, 82), 
			dActionEntry (277, 1, 34, 2, 82), dActionEntry (278, 1, 34, 2, 82), dActionEntry (279, 1, 34, 2, 82), dActionEntry (280, 1, 34, 2, 82), 
			dActionEntry (282, 1, 34, 2, 82), dActionEntry (285, 1, 34, 2, 82), dActionEntry (286, 1, 34, 2, 82), dActionEntry (59, 1, 22, 1, 36), 
			dActionEntry (125, 1, 22, 1, 36), dActionEntry (261, 1, 22, 1, 36), dActionEntry (262, 1, 22, 1, 36), dActionEntry (263, 1, 22, 1, 36), 
			dActionEntry (266, 1, 22, 1, 36), dActionEntry (267, 1, 22, 1, 36), dActionEntry (268, 1, 22, 1, 36), dActionEntry (269, 1, 22, 1, 36), 
			dActionEntry (270, 1, 22, 1, 36), dActionEntry (271, 1, 22, 1, 36), dActionEntry (272, 1, 22, 1, 36), dActionEntry (273, 1, 22, 1, 36), 
			dActionEntry (274, 1, 22, 1, 36), dActionEntry (276, 1, 22, 1, 36), dActionEntry (277, 1, 22, 1, 36), dActionEntry (278, 1, 22, 1, 36), 
			dActionEntry (279, 1, 22, 1, 36), dActionEntry (280, 1, 22, 1, 36), dActionEntry (282, 1, 22, 1, 36), dActionEntry (285, 1, 22, 1, 36), 
			dActionEntry (286, 1, 22, 1, 36), dActionEntry (263, 0, 191, 0, 0), dActionEntry (125, 1, 34, 2, 83), dActionEntry (261, 1, 34, 2, 83), 
			dActionEntry (262, 1, 34, 2, 83), dActionEntry (263, 1, 34, 2, 83), dActionEntry (266, 1, 34, 2, 83), dActionEntry (267, 1, 34, 2, 83), 
			dActionEntry (268, 1, 34, 2, 83), dActionEntry (269, 1, 34, 2, 83), dActionEntry (270, 1, 34, 2, 83), dActionEntry (271, 1, 34, 2, 83), 
			dActionEntry (272, 1, 34, 2, 83), dActionEntry (273, 1, 34, 2, 83), dActionEntry (274, 1, 34, 2, 83), dActionEntry (276, 1, 34, 2, 83), 
			dActionEntry (277, 1, 34, 2, 83), dActionEntry (278, 1, 34, 2, 83), dActionEntry (279, 1, 34, 2, 83), dActionEntry (280, 1, 34, 2, 83), 
			dActionEntry (282, 1, 34, 2, 83), dActionEntry (285, 1, 34, 2, 83), dActionEntry (286, 1, 34, 2, 83), dActionEntry (40, 0, 131, 0, 0), 
			dActionEntry (258, 0, 128, 0, 0), dActionEntry (263, 0, 133, 0, 0), dActionEntry (266, 0, 200, 0, 0), dActionEntry (267, 0, 194, 0, 0), 
			dActionEntry (268, 0, 198, 0, 0), dActionEntry (269, 0, 201, 0, 0), dActionEntry (270, 0, 195, 0, 0), dActionEntry (271, 0, 199, 0, 0), 
			dActionEntry (272, 0, 197, 0, 0), dActionEntry (273, 0, 203, 0, 0), dActionEntry (274, 0, 202, 0, 0), dActionEntry (125, 1, 33, 2, 74), 
			dActionEntry (261, 1, 33, 2, 74), dActionEntry (262, 1, 33, 2, 74), dActionEntry (263, 1, 33, 2, 74), dActionEntry (266, 1, 33, 2, 74), 
			dActionEntry (267, 1, 33, 2, 74), dActionEntry (268, 1, 33, 2, 74), dActionEntry (269, 1, 33, 2, 74), dActionEntry (270, 1, 33, 2, 74), 
			dActionEntry (271, 1, 33, 2, 74), dActionEntry (272, 1, 33, 2, 74), dActionEntry (273, 1, 33, 2, 74), dActionEntry (274, 1, 33, 2, 74), 
			dActionEntry (276, 1, 33, 2, 74), dActionEntry (277, 1, 33, 2, 74), dActionEntry (278, 1, 33, 2, 74), dActionEntry (279, 1, 33, 2, 74), 
			dActionEntry (280, 1, 33, 2, 74), dActionEntry (282, 1, 33, 2, 74), dActionEntry (285, 1, 33, 2, 74), dActionEntry (286, 1, 33, 2, 74), 
			dActionEntry (125, 1, 34, 2, 81), dActionEntry (261, 1, 34, 2, 81), dActionEntry (262, 1, 34, 2, 81), dActionEntry (263, 1, 34, 2, 81), 
			dActionEntry (266, 1, 34, 2, 81), dActionEntry (267, 1, 34, 2, 81), dActionEntry (268, 1, 34, 2, 81), dActionEntry (269, 1, 34, 2, 81), 
			dActionEntry (270, 1, 34, 2, 81), dActionEntry (271, 1, 34, 2, 81), dActionEntry (272, 1, 34, 2, 81), dActionEntry (273, 1, 34, 2, 81), 
			dActionEntry (274, 1, 34, 2, 81), dActionEntry (276, 1, 34, 2, 81), dActionEntry (277, 1, 34, 2, 81), dActionEntry (278, 1, 34, 2, 81), 
			dActionEntry (279, 1, 34, 2, 81), dActionEntry (280, 1, 34, 2, 81), dActionEntry (282, 1, 34, 2, 81), dActionEntry (285, 1, 34, 2, 81), 
			dActionEntry (286, 1, 34, 2, 81), dActionEntry (125, 1, 34, 2, 80), dActionEntry (261, 1, 34, 2, 80), dActionEntry (262, 1, 34, 2, 80), 
			dActionEntry (263, 1, 34, 2, 80), dActionEntry (266, 1, 34, 2, 80), dActionEntry (267, 1, 34, 2, 80), dActionEntry (268, 1, 34, 2, 80), 
			dActionEntry (269, 1, 34, 2, 80), dActionEntry (270, 1, 34, 2, 80), dActionEntry (271, 1, 34, 2, 80), dActionEntry (272, 1, 34, 2, 80), 
			dActionEntry (273, 1, 34, 2, 80), dActionEntry (274, 1, 34, 2, 80), dActionEntry (276, 1, 34, 2, 80), dActionEntry (277, 1, 34, 2, 80), 
			dActionEntry (278, 1, 34, 2, 80), dActionEntry (279, 1, 34, 2, 80), dActionEntry (280, 1, 34, 2, 80), dActionEntry (282, 1, 34, 2, 80), 
			dActionEntry (285, 1, 34, 2, 80), dActionEntry (286, 1, 34, 2, 80), dActionEntry (125, 1, 34, 2, 77), dActionEntry (261, 1, 34, 2, 77), 
			dActionEntry (262, 1, 34, 2, 77), dActionEntry (263, 1, 34, 2, 77), dActionEntry (266, 1, 34, 2, 77), dActionEntry (267, 1, 34, 2, 77), 
			dActionEntry (268, 1, 34, 2, 77), dActionEntry (269, 1, 34, 2, 77), dActionEntry (270, 1, 34, 2, 77), dActionEntry (271, 1, 34, 2, 77), 
			dActionEntry (272, 1, 34, 2, 77), dActionEntry (273, 1, 34, 2, 77), dActionEntry (274, 1, 34, 2, 77), dActionEntry (276, 1, 34, 2, 77), 
			dActionEntry (277, 1, 34, 2, 77), dActionEntry (278, 1, 34, 2, 77), dActionEntry (279, 1, 34, 2, 77), dActionEntry (280, 1, 34, 2, 77), 
			dActionEntry (282, 1, 34, 2, 77), dActionEntry (285, 1, 34, 2, 77), dActionEntry (286, 1, 34, 2, 77), dActionEntry (40, 0, 131, 0, 0), 
			dActionEntry (258, 0, 128, 0, 0), dActionEntry (263, 0, 133, 0, 0), dActionEntry (275, 0, 205, 0, 0), dActionEntry (61, 1, 42, 2, 85), 
			dActionEntry (91, 0, 209, 0, 0), dActionEntry (40, 0, 173, 0, 0), dActionEntry (41, 0, 210, 0, 0), dActionEntry (258, 0, 170, 0, 0), 
			dActionEntry (263, 0, 176, 0, 0), dActionEntry (59, 1, 26, 2, 44), dActionEntry (61, 1, 26, 2, 44), dActionEntry (254, 1, 26, 2, 44), 
			dActionEntry (125, 1, 21, 2, 35), dActionEntry (256, 1, 21, 2, 35), dActionEntry (261, 1, 21, 2, 35), dActionEntry (262, 1, 21, 2, 35), 
			dActionEntry (263, 1, 21, 2, 35), dActionEntry (266, 1, 21, 2, 35), dActionEntry (267, 1, 21, 2, 35), dActionEntry (268, 1, 21, 2, 35), 
			dActionEntry (269, 1, 21, 2, 35), dActionEntry (270, 1, 21, 2, 35), dActionEntry (271, 1, 21, 2, 35), dActionEntry (272, 1, 21, 2, 35), 
			dActionEntry (273, 1, 21, 2, 35), dActionEntry (274, 1, 21, 2, 35), dActionEntry (59, 1, 22, 2, 37), dActionEntry (125, 1, 22, 2, 37), 
			dActionEntry (256, 1, 22, 2, 37), dActionEntry (261, 1, 22, 2, 37), dActionEntry (262, 1, 22, 2, 37), dActionEntry (263, 1, 22, 2, 37), 
			dActionEntry (266, 1, 22, 2, 37), dActionEntry (267, 1, 22, 2, 37), dActionEntry (268, 1, 22, 2, 37), dActionEntry (269, 1, 22, 2, 37), 
			dActionEntry (270, 1, 22, 2, 37), dActionEntry (271, 1, 22, 2, 37), dActionEntry (272, 1, 22, 2, 37), dActionEntry (273, 1, 22, 2, 37), 
			dActionEntry (274, 1, 22, 2, 37), dActionEntry (41, 0, 212, 0, 0), dActionEntry (41, 0, 214, 0, 0), dActionEntry (93, 1, 28, 3, 108), 
			dActionEntry (288, 1, 28, 3, 108), dActionEntry (93, 1, 28, 3, 109), dActionEntry (288, 1, 28, 3, 109), dActionEntry (41, 0, 216, 0, 0), 
			dActionEntry (288, 0, 162, 0, 0), dActionEntry (41, 1, 42, 2, 85), dActionEntry (91, 0, 218, 0, 0), dActionEntry (288, 1, 42, 2, 85), 
			dActionEntry (40, 0, 173, 0, 0), dActionEntry (41, 0, 219, 0, 0), dActionEntry (258, 0, 170, 0, 0), dActionEntry (263, 0, 176, 0, 0), 
			dActionEntry (93, 0, 221, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (41, 1, 28, 1, 112), dActionEntry (44, 1, 28, 1, 112), 
			dActionEntry (288, 1, 28, 1, 112), dActionEntry (41, 1, 28, 1, 111), dActionEntry (44, 1, 28, 1, 111), dActionEntry (288, 1, 28, 1, 111), 
			dActionEntry (41, 1, 46, 1, 102), dActionEntry (44, 1, 46, 1, 102), dActionEntry (288, 0, 223, 0, 0), dActionEntry (93, 1, 20, 3, 104), 
			dActionEntry (288, 1, 20, 3, 104), dActionEntry (41, 1, 28, 1, 110), dActionEntry (44, 1, 28, 1, 110), dActionEntry (288, 1, 28, 1, 110), 
			dActionEntry (40, 0, 227, 0, 0), dActionEntry (41, 1, 42, 1, 84), dActionEntry (44, 1, 42, 1, 84), dActionEntry (91, 0, 225, 0, 0), 
			dActionEntry (288, 1, 42, 1, 84), dActionEntry (41, 0, 229, 0, 0), dActionEntry (44, 0, 228, 0, 0), dActionEntry (58, 0, 232, 0, 0), 
			dActionEntry (123, 1, 19, 0, 32), dActionEntry (41, 1, 16, 2, 48), dActionEntry (44, 1, 16, 2, 48), dActionEntry (125, 0, 233, 0, 0), 
			dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 101, 0, 0), dActionEntry (266, 0, 33, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), 
			dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), 
			dActionEntry (276, 0, 78, 0, 0), dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 99, 0, 0), dActionEntry (279, 0, 77, 0, 0), 
			dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 100, 0, 0), dActionEntry (286, 0, 93, 0, 0), 
			dActionEntry (278, 1, 23, 2, 69), dActionEntry (40, 0, 234, 0, 0), dActionEntry (41, 0, 236, 0, 0), dActionEntry (288, 0, 162, 0, 0), 
			dActionEntry (59, 1, 42, 2, 85), dActionEntry (91, 0, 238, 0, 0), dActionEntry (254, 1, 42, 2, 85), dActionEntry (288, 1, 42, 2, 85), 
			dActionEntry (40, 0, 173, 0, 0), dActionEntry (41, 0, 239, 0, 0), dActionEntry (258, 0, 170, 0, 0), dActionEntry (263, 0, 176, 0, 0), 
			dActionEntry (125, 1, 21, 2, 35), dActionEntry (261, 1, 21, 2, 35), dActionEntry (262, 1, 21, 2, 35), dActionEntry (263, 1, 21, 2, 35), 
			dActionEntry (266, 1, 21, 2, 35), dActionEntry (267, 1, 21, 2, 35), dActionEntry (268, 1, 21, 2, 35), dActionEntry (269, 1, 21, 2, 35), 
			dActionEntry (270, 1, 21, 2, 35), dActionEntry (271, 1, 21, 2, 35), dActionEntry (272, 1, 21, 2, 35), dActionEntry (273, 1, 21, 2, 35), 
			dActionEntry (274, 1, 21, 2, 35), dActionEntry (276, 1, 21, 2, 35), dActionEntry (277, 1, 21, 2, 35), dActionEntry (278, 1, 21, 2, 35), 
			dActionEntry (279, 1, 21, 2, 35), dActionEntry (280, 1, 21, 2, 35), dActionEntry (282, 1, 21, 2, 35), dActionEntry (285, 1, 21, 2, 35), 
			dActionEntry (286, 1, 21, 2, 35), dActionEntry (59, 1, 22, 2, 37), dActionEntry (125, 1, 22, 2, 37), dActionEntry (261, 1, 22, 2, 37), 
			dActionEntry (262, 1, 22, 2, 37), dActionEntry (263, 1, 22, 2, 37), dActionEntry (266, 1, 22, 2, 37), dActionEntry (267, 1, 22, 2, 37), 
			dActionEntry (268, 1, 22, 2, 37), dActionEntry (269, 1, 22, 2, 37), dActionEntry (270, 1, 22, 2, 37), dActionEntry (271, 1, 22, 2, 37), 
			dActionEntry (272, 1, 22, 2, 37), dActionEntry (273, 1, 22, 2, 37), dActionEntry (274, 1, 22, 2, 37), dActionEntry (276, 1, 22, 2, 37), 
			dActionEntry (277, 1, 22, 2, 37), dActionEntry (278, 1, 22, 2, 37), dActionEntry (279, 1, 22, 2, 37), dActionEntry (280, 1, 22, 2, 37), 
			dActionEntry (282, 1, 22, 2, 37), dActionEntry (285, 1, 22, 2, 37), dActionEntry (286, 1, 22, 2, 37), dActionEntry (59, 1, 26, 3, 45), 
			dActionEntry (61, 1, 26, 3, 45), dActionEntry (254, 1, 26, 3, 45), dActionEntry (59, 1, 27, 3, 46), dActionEntry (254, 1, 27, 3, 46), 
			dActionEntry (288, 0, 184, 0, 0), dActionEntry (41, 0, 241, 0, 0), dActionEntry (288, 0, 162, 0, 0), dActionEntry (263, 1, 29, 1, 54), 
			dActionEntry (263, 1, 29, 1, 57), dActionEntry (263, 0, 244, 0, 0), dActionEntry (263, 1, 29, 1, 62), dActionEntry (263, 1, 29, 1, 55), 
			dActionEntry (267, 0, 245, 0, 0), dActionEntry (268, 0, 247, 0, 0), dActionEntry (269, 0, 248, 0, 0), dActionEntry (270, 0, 246, 0, 0), 
			dActionEntry (263, 1, 29, 1, 53), dActionEntry (263, 1, 29, 1, 56), dActionEntry (263, 1, 29, 1, 64), dActionEntry (263, 1, 29, 1, 63), 
			dActionEntry (41, 0, 249, 0, 0), dActionEntry (288, 0, 162, 0, 0), dActionEntry (266, 0, 256, 0, 0), dActionEntry (267, 0, 250, 0, 0), 
			dActionEntry (268, 0, 254, 0, 0), dActionEntry (269, 0, 257, 0, 0), dActionEntry (270, 0, 251, 0, 0), dActionEntry (271, 0, 255, 0, 0), 
			dActionEntry (272, 0, 253, 0, 0), dActionEntry (273, 0, 259, 0, 0), dActionEntry (274, 0, 258, 0, 0), dActionEntry (59, 1, 36, 3, 88), 
			dActionEntry (254, 1, 36, 3, 88), dActionEntry (288, 0, 184, 0, 0), dActionEntry (41, 0, 260, 0, 0), dActionEntry (288, 0, 162, 0, 0), 
			dActionEntry (93, 0, 261, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (59, 1, 20, 3, 104), dActionEntry (254, 1, 20, 3, 104), 
			dActionEntry (41, 0, 263, 0, 0), dActionEntry (44, 0, 228, 0, 0), dActionEntry (41, 0, 265, 0, 0), dActionEntry (123, 1, 18, 0, 30), 
			dActionEntry (257, 0, 267, 0, 0), dActionEntry (41, 1, 28, 3, 108), dActionEntry (288, 1, 28, 3, 108), dActionEntry (41, 1, 28, 3, 109), 
			dActionEntry (288, 1, 28, 3, 109), dActionEntry (93, 0, 268, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (41, 1, 20, 3, 104), 
			dActionEntry (288, 1, 20, 3, 104), dActionEntry (41, 0, 270, 0, 0), dActionEntry (44, 0, 228, 0, 0), dActionEntry (91, 1, 43, 3, 86), 
			dActionEntry (93, 1, 43, 3, 86), dActionEntry (288, 1, 43, 3, 86), dActionEntry (93, 0, 271, 0, 0), dActionEntry (288, 0, 111, 0, 0), 
			dActionEntry (40, 0, 173, 0, 0), dActionEntry (258, 0, 170, 0, 0), dActionEntry (263, 0, 176, 0, 0), dActionEntry (41, 0, 273, 0, 0), 
			dActionEntry (288, 0, 162, 0, 0), dActionEntry (41, 1, 42, 2, 85), dActionEntry (44, 1, 42, 2, 85), dActionEntry (91, 0, 275, 0, 0), 
			dActionEntry (288, 1, 42, 2, 85), dActionEntry (40, 0, 173, 0, 0), dActionEntry (41, 0, 276, 0, 0), dActionEntry (258, 0, 170, 0, 0), 
			dActionEntry (263, 0, 176, 0, 0), dActionEntry (40, 0, 281, 0, 0), dActionEntry (258, 0, 278, 0, 0), dActionEntry (263, 0, 283, 0, 0), 
			dActionEntry (93, 1, 20, 4, 105), dActionEntry (288, 1, 20, 4, 105), dActionEntry (41, 1, 15, 3, 27), dActionEntry (44, 1, 15, 3, 27), 
			dActionEntry (263, 0, 286, 0, 0), dActionEntry (278, 1, 23, 3, 70), dActionEntry (59, 1, 28, 3, 108), dActionEntry (254, 1, 28, 3, 108), 
			dActionEntry (288, 1, 28, 3, 108), dActionEntry (59, 1, 28, 3, 109), dActionEntry (254, 1, 28, 3, 109), dActionEntry (288, 1, 28, 3, 109), 
			dActionEntry (93, 0, 288, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (59, 1, 20, 3, 104), dActionEntry (254, 1, 20, 3, 104), 
			dActionEntry (288, 1, 20, 3, 104), dActionEntry (41, 0, 290, 0, 0), dActionEntry (44, 0, 228, 0, 0), dActionEntry (123, 0, 291, 0, 0), 
			dActionEntry (59, 0, 292, 0, 0), dActionEntry (61, 0, 293, 0, 0), dActionEntry (61, 1, 42, 1, 84), dActionEntry (91, 0, 152, 0, 0), 
			dActionEntry (263, 1, 29, 2, 58), dActionEntry (263, 1, 29, 2, 61), dActionEntry (263, 1, 29, 2, 59), dActionEntry (263, 1, 29, 2, 60), 
			dActionEntry (123, 0, 294, 0, 0), dActionEntry (91, 1, 29, 1, 54), dActionEntry (91, 1, 29, 1, 57), dActionEntry (91, 0, 296, 0, 0), 
			dActionEntry (91, 1, 29, 1, 62), dActionEntry (91, 1, 29, 1, 55), dActionEntry (267, 0, 298, 0, 0), dActionEntry (268, 0, 300, 0, 0), 
			dActionEntry (269, 0, 301, 0, 0), dActionEntry (270, 0, 299, 0, 0), dActionEntry (91, 1, 29, 1, 53), dActionEntry (91, 1, 29, 1, 56), 
			dActionEntry (91, 1, 29, 1, 64), dActionEntry (91, 1, 29, 1, 63), dActionEntry (123, 0, 302, 0, 0), dActionEntry (61, 1, 43, 3, 86), 
			dActionEntry (91, 1, 43, 3, 86), dActionEntry (93, 0, 304, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (59, 1, 20, 4, 105), 
			dActionEntry (254, 1, 20, 4, 105), dActionEntry (125, 1, 11, 6, 38), dActionEntry (256, 1, 11, 6, 38), dActionEntry (261, 1, 11, 6, 38), 
			dActionEntry (262, 1, 11, 6, 38), dActionEntry (263, 1, 11, 6, 38), dActionEntry (266, 1, 11, 6, 38), dActionEntry (267, 1, 11, 6, 38), 
			dActionEntry (268, 1, 11, 6, 38), dActionEntry (269, 1, 11, 6, 38), dActionEntry (270, 1, 11, 6, 38), dActionEntry (271, 1, 11, 6, 38), 
			dActionEntry (272, 1, 11, 6, 38), dActionEntry (273, 1, 11, 6, 38), dActionEntry (274, 1, 11, 6, 38), dActionEntry (59, 1, 18, 0, 30), 
			dActionEntry (254, 1, 18, 0, 30), dActionEntry (257, 0, 306, 0, 0), dActionEntry (123, 1, 24, 6, 43), dActionEntry (123, 1, 18, 1, 31), 
			dActionEntry (41, 1, 43, 3, 86), dActionEntry (91, 1, 43, 3, 86), dActionEntry (288, 1, 43, 3, 86), dActionEntry (93, 0, 307, 0, 0), 
			dActionEntry (288, 0, 111, 0, 0), dActionEntry (41, 1, 20, 4, 105), dActionEntry (288, 1, 20, 4, 105), dActionEntry (91, 1, 43, 4, 87), 
			dActionEntry (93, 1, 43, 4, 87), dActionEntry (288, 1, 43, 4, 87), dActionEntry (41, 1, 28, 3, 108), dActionEntry (44, 1, 28, 3, 108), 
			dActionEntry (288, 1, 28, 3, 108), dActionEntry (41, 1, 28, 3, 109), dActionEntry (44, 1, 28, 3, 109), dActionEntry (288, 1, 28, 3, 109), 
			dActionEntry (93, 0, 308, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (41, 1, 20, 3, 104), dActionEntry (44, 1, 20, 3, 104), 
			dActionEntry (288, 1, 20, 3, 104), dActionEntry (41, 0, 310, 0, 0), dActionEntry (44, 0, 228, 0, 0), dActionEntry (41, 1, 46, 3, 103), 
			dActionEntry (44, 1, 46, 3, 103), dActionEntry (288, 0, 311, 0, 0), dActionEntry (40, 0, 315, 0, 0), dActionEntry (41, 1, 42, 1, 84), 
			dActionEntry (44, 1, 42, 1, 84), dActionEntry (91, 0, 313, 0, 0), dActionEntry (288, 1, 42, 1, 84), dActionEntry (125, 1, 11, 7, 39), 
			dActionEntry (256, 1, 11, 7, 39), dActionEntry (261, 1, 11, 7, 39), dActionEntry (262, 1, 11, 7, 39), dActionEntry (263, 1, 11, 7, 39), 
			dActionEntry (266, 1, 11, 7, 39), dActionEntry (267, 1, 11, 7, 39), dActionEntry (268, 1, 11, 7, 39), dActionEntry (269, 1, 11, 7, 39), 
			dActionEntry (270, 1, 11, 7, 39), dActionEntry (271, 1, 11, 7, 39), dActionEntry (272, 1, 11, 7, 39), dActionEntry (273, 1, 11, 7, 39), 
			dActionEntry (274, 1, 11, 7, 39), dActionEntry (123, 1, 19, 2, 33), dActionEntry (40, 0, 316, 0, 0), dActionEntry (41, 0, 317, 0, 0), 
			dActionEntry (288, 0, 162, 0, 0), dActionEntry (59, 1, 43, 3, 86), dActionEntry (91, 1, 43, 3, 86), dActionEntry (254, 1, 43, 3, 86), 
			dActionEntry (288, 1, 43, 3, 86), dActionEntry (93, 0, 318, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (59, 1, 20, 4, 105), 
			dActionEntry (254, 1, 20, 4, 105), dActionEntry (288, 1, 20, 4, 105), dActionEntry (283, 0, 320, 0, 0), dActionEntry (284, 0, 319, 0, 0), 
			dActionEntry (40, 0, 326, 0, 0), dActionEntry (258, 0, 323, 0, 0), dActionEntry (263, 0, 328, 0, 0), dActionEntry (40, 0, 326, 0, 0), 
			dActionEntry (258, 0, 323, 0, 0), dActionEntry (263, 0, 328, 0, 0), dActionEntry (275, 0, 329, 0, 0), dActionEntry (125, 0, 332, 0, 0), 
			dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 101, 0, 0), dActionEntry (266, 0, 33, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), 
			dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), 
			dActionEntry (276, 0, 78, 0, 0), dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 99, 0, 0), dActionEntry (279, 0, 77, 0, 0), 
			dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 100, 0, 0), dActionEntry (286, 0, 93, 0, 0), 
			dActionEntry (125, 1, 37, 5, 90), dActionEntry (261, 1, 37, 5, 90), dActionEntry (262, 1, 37, 5, 90), dActionEntry (263, 1, 37, 5, 90), 
			dActionEntry (266, 1, 37, 5, 90), dActionEntry (267, 1, 37, 5, 90), dActionEntry (268, 1, 37, 5, 90), dActionEntry (269, 1, 37, 5, 90), 
			dActionEntry (270, 1, 37, 5, 90), dActionEntry (271, 1, 37, 5, 90), dActionEntry (272, 1, 37, 5, 90), dActionEntry (273, 1, 37, 5, 90), 
			dActionEntry (274, 1, 37, 5, 90), dActionEntry (276, 1, 37, 5, 90), dActionEntry (277, 1, 37, 5, 90), dActionEntry (278, 1, 37, 5, 90), 
			dActionEntry (279, 1, 37, 5, 90), dActionEntry (280, 1, 37, 5, 90), dActionEntry (281, 0, 333, 0, 0), dActionEntry (282, 1, 37, 5, 90), 
			dActionEntry (285, 1, 37, 5, 90), dActionEntry (286, 1, 37, 5, 90), dActionEntry (59, 1, 36, 5, 89), dActionEntry (91, 0, 335, 0, 0), 
			dActionEntry (254, 1, 36, 5, 89), dActionEntry (91, 1, 29, 2, 58), dActionEntry (91, 1, 29, 2, 61), dActionEntry (91, 1, 29, 2, 59), 
			dActionEntry (91, 1, 29, 2, 60), dActionEntry (125, 0, 337, 0, 0), dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), 
			dActionEntry (263, 0, 101, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), 
			dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), 
			dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 78, 0, 0), dActionEntry (277, 0, 90, 0, 0), 
			dActionEntry (278, 0, 99, 0, 0), dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), 
			dActionEntry (285, 0, 100, 0, 0), dActionEntry (286, 0, 93, 0, 0), dActionEntry (125, 1, 38, 5, 93), dActionEntry (261, 1, 38, 5, 93), 
			dActionEntry (262, 1, 38, 5, 93), dActionEntry (263, 1, 38, 5, 93), dActionEntry (266, 1, 38, 5, 93), dActionEntry (267, 1, 38, 5, 93), 
			dActionEntry (268, 1, 38, 5, 93), dActionEntry (269, 1, 38, 5, 93), dActionEntry (270, 1, 38, 5, 93), dActionEntry (271, 1, 38, 5, 93), 
			dActionEntry (272, 1, 38, 5, 93), dActionEntry (273, 1, 38, 5, 93), dActionEntry (274, 1, 38, 5, 93), dActionEntry (276, 1, 38, 5, 93), 
			dActionEntry (277, 1, 38, 5, 93), dActionEntry (278, 1, 38, 5, 93), dActionEntry (279, 1, 38, 5, 93), dActionEntry (280, 1, 38, 5, 93), 
			dActionEntry (282, 1, 38, 5, 93), dActionEntry (285, 1, 38, 5, 93), dActionEntry (286, 1, 38, 5, 93), dActionEntry (61, 1, 43, 4, 87), 
			dActionEntry (91, 1, 43, 4, 87), dActionEntry (59, 1, 24, 6, 43), dActionEntry (254, 1, 24, 6, 43), dActionEntry (59, 1, 18, 1, 31), 
			dActionEntry (254, 1, 18, 1, 31), dActionEntry (41, 1, 43, 4, 87), dActionEntry (91, 1, 43, 4, 87), dActionEntry (288, 1, 43, 4, 87), 
			dActionEntry (41, 1, 43, 3, 86), dActionEntry (44, 1, 43, 3, 86), dActionEntry (91, 1, 43, 3, 86), dActionEntry (288, 1, 43, 3, 86), 
			dActionEntry (93, 0, 338, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (41, 1, 20, 4, 105), dActionEntry (44, 1, 20, 4, 105), 
			dActionEntry (288, 1, 20, 4, 105), dActionEntry (41, 0, 340, 0, 0), dActionEntry (288, 0, 162, 0, 0), dActionEntry (41, 1, 42, 2, 85), 
			dActionEntry (44, 1, 42, 2, 85), dActionEntry (91, 0, 342, 0, 0), dActionEntry (288, 1, 42, 2, 85), dActionEntry (40, 0, 173, 0, 0), 
			dActionEntry (41, 0, 343, 0, 0), dActionEntry (258, 0, 170, 0, 0), dActionEntry (263, 0, 176, 0, 0), dActionEntry (40, 0, 173, 0, 0), 
			dActionEntry (41, 0, 345, 0, 0), dActionEntry (258, 0, 170, 0, 0), dActionEntry (263, 0, 176, 0, 0), dActionEntry (125, 1, 38, 6, 94), 
			dActionEntry (261, 1, 38, 6, 94), dActionEntry (262, 1, 38, 6, 94), dActionEntry (263, 1, 38, 6, 94), dActionEntry (266, 1, 38, 6, 94), 
			dActionEntry (267, 1, 38, 6, 94), dActionEntry (268, 1, 38, 6, 94), dActionEntry (269, 1, 38, 6, 94), dActionEntry (270, 1, 38, 6, 94), 
			dActionEntry (271, 1, 38, 6, 94), dActionEntry (272, 1, 38, 6, 94), dActionEntry (273, 1, 38, 6, 94), dActionEntry (274, 1, 38, 6, 94), 
			dActionEntry (276, 1, 38, 6, 94), dActionEntry (277, 1, 38, 6, 94), dActionEntry (278, 1, 38, 6, 94), dActionEntry (279, 1, 38, 6, 94), 
			dActionEntry (280, 1, 38, 6, 94), dActionEntry (282, 1, 38, 6, 94), dActionEntry (285, 1, 38, 6, 94), dActionEntry (286, 1, 38, 6, 94), 
			dActionEntry (59, 1, 43, 4, 87), dActionEntry (91, 1, 43, 4, 87), dActionEntry (254, 1, 43, 4, 87), dActionEntry (288, 1, 43, 4, 87), 
			dActionEntry (58, 0, 347, 0, 0), dActionEntry (258, 0, 348, 0, 0), dActionEntry (125, 1, 44, 1, 98), dActionEntry (283, 1, 44, 1, 98), 
			dActionEntry (284, 1, 44, 1, 98), dActionEntry (125, 0, 349, 0, 0), dActionEntry (283, 0, 320, 0, 0), dActionEntry (284, 0, 319, 0, 0), 
			dActionEntry (59, 1, 28, 1, 112), dActionEntry (288, 1, 28, 1, 112), dActionEntry (59, 1, 28, 1, 111), dActionEntry (288, 1, 28, 1, 111), 
			dActionEntry (59, 0, 352, 0, 0), dActionEntry (288, 0, 351, 0, 0), dActionEntry (59, 1, 28, 1, 110), dActionEntry (288, 1, 28, 1, 110), 
			dActionEntry (40, 0, 356, 0, 0), dActionEntry (59, 1, 42, 1, 84), dActionEntry (91, 0, 354, 0, 0), dActionEntry (288, 1, 42, 1, 84), 
			dActionEntry (59, 1, 36, 3, 88), dActionEntry (288, 0, 351, 0, 0), dActionEntry (125, 0, 358, 0, 0), dActionEntry (261, 0, 86, 0, 0), 
			dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 101, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 78, 0, 0), 
			dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 99, 0, 0), dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), 
			dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 100, 0, 0), dActionEntry (286, 0, 93, 0, 0), dActionEntry (125, 1, 23, 2, 69), 
			dActionEntry (261, 1, 23, 2, 69), dActionEntry (262, 1, 23, 2, 69), dActionEntry (263, 1, 23, 2, 69), dActionEntry (266, 1, 23, 2, 69), 
			dActionEntry (267, 1, 23, 2, 69), dActionEntry (268, 1, 23, 2, 69), dActionEntry (269, 1, 23, 2, 69), dActionEntry (270, 1, 23, 2, 69), 
			dActionEntry (271, 1, 23, 2, 69), dActionEntry (272, 1, 23, 2, 69), dActionEntry (273, 1, 23, 2, 69), dActionEntry (274, 1, 23, 2, 69), 
			dActionEntry (276, 1, 23, 2, 69), dActionEntry (277, 1, 23, 2, 69), dActionEntry (278, 1, 23, 2, 69), dActionEntry (279, 1, 23, 2, 69), 
			dActionEntry (280, 1, 23, 2, 69), dActionEntry (281, 1, 23, 2, 69), dActionEntry (282, 1, 23, 2, 69), dActionEntry (285, 1, 23, 2, 69), 
			dActionEntry (286, 1, 23, 2, 69), dActionEntry (93, 0, 360, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (125, 0, 362, 0, 0), 
			dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 101, 0, 0), dActionEntry (266, 0, 33, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), 
			dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), 
			dActionEntry (276, 0, 78, 0, 0), dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 99, 0, 0), dActionEntry (279, 0, 77, 0, 0), 
			dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 100, 0, 0), dActionEntry (286, 0, 93, 0, 0), 
			dActionEntry (125, 1, 23, 2, 69), dActionEntry (261, 1, 23, 2, 69), dActionEntry (262, 1, 23, 2, 69), dActionEntry (263, 1, 23, 2, 69), 
			dActionEntry (266, 1, 23, 2, 69), dActionEntry (267, 1, 23, 2, 69), dActionEntry (268, 1, 23, 2, 69), dActionEntry (269, 1, 23, 2, 69), 
			dActionEntry (270, 1, 23, 2, 69), dActionEntry (271, 1, 23, 2, 69), dActionEntry (272, 1, 23, 2, 69), dActionEntry (273, 1, 23, 2, 69), 
			dActionEntry (274, 1, 23, 2, 69), dActionEntry (276, 1, 23, 2, 69), dActionEntry (277, 1, 23, 2, 69), dActionEntry (278, 1, 23, 2, 69), 
			dActionEntry (279, 1, 23, 2, 69), dActionEntry (280, 1, 23, 2, 69), dActionEntry (282, 1, 23, 2, 69), dActionEntry (285, 1, 23, 2, 69), 
			dActionEntry (286, 1, 23, 2, 69), dActionEntry (41, 1, 43, 4, 87), dActionEntry (44, 1, 43, 4, 87), dActionEntry (91, 1, 43, 4, 87), 
			dActionEntry (288, 1, 43, 4, 87), dActionEntry (93, 0, 363, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (41, 0, 365, 0, 0), 
			dActionEntry (44, 0, 228, 0, 0), dActionEntry (123, 1, 20, 3, 104), dActionEntry (41, 0, 366, 0, 0), dActionEntry (44, 0, 228, 0, 0), 
			dActionEntry (123, 0, 367, 0, 0), dActionEntry (58, 0, 369, 0, 0), dActionEntry (125, 1, 37, 7, 92), dActionEntry (261, 1, 37, 7, 92), 
			dActionEntry (262, 1, 37, 7, 92), dActionEntry (263, 1, 37, 7, 92), dActionEntry (266, 1, 37, 7, 92), dActionEntry (267, 1, 37, 7, 92), 
			dActionEntry (268, 1, 37, 7, 92), dActionEntry (269, 1, 37, 7, 92), dActionEntry (270, 1, 37, 7, 92), dActionEntry (271, 1, 37, 7, 92), 
			dActionEntry (272, 1, 37, 7, 92), dActionEntry (273, 1, 37, 7, 92), dActionEntry (274, 1, 37, 7, 92), dActionEntry (276, 1, 37, 7, 92), 
			dActionEntry (277, 1, 37, 7, 92), dActionEntry (278, 1, 37, 7, 92), dActionEntry (279, 1, 37, 7, 92), dActionEntry (280, 1, 37, 7, 92), 
			dActionEntry (282, 1, 37, 7, 92), dActionEntry (285, 1, 37, 7, 92), dActionEntry (286, 1, 37, 7, 92), dActionEntry (125, 1, 44, 2, 99), 
			dActionEntry (283, 1, 44, 2, 99), dActionEntry (284, 1, 44, 2, 99), dActionEntry (41, 0, 373, 0, 0), dActionEntry (288, 0, 162, 0, 0), 
			dActionEntry (59, 1, 42, 2, 85), dActionEntry (91, 0, 375, 0, 0), dActionEntry (288, 1, 42, 2, 85), dActionEntry (40, 0, 173, 0, 0), 
			dActionEntry (41, 0, 376, 0, 0), dActionEntry (258, 0, 170, 0, 0), dActionEntry (263, 0, 176, 0, 0), dActionEntry (91, 0, 378, 0, 0), 
			dActionEntry (125, 1, 23, 3, 70), dActionEntry (261, 1, 23, 3, 70), dActionEntry (262, 1, 23, 3, 70), dActionEntry (263, 1, 23, 3, 70), 
			dActionEntry (266, 1, 23, 3, 70), dActionEntry (267, 1, 23, 3, 70), dActionEntry (268, 1, 23, 3, 70), dActionEntry (269, 1, 23, 3, 70), 
			dActionEntry (270, 1, 23, 3, 70), dActionEntry (271, 1, 23, 3, 70), dActionEntry (272, 1, 23, 3, 70), dActionEntry (273, 1, 23, 3, 70), 
			dActionEntry (274, 1, 23, 3, 70), dActionEntry (276, 1, 23, 3, 70), dActionEntry (277, 1, 23, 3, 70), dActionEntry (278, 1, 23, 3, 70), 
			dActionEntry (279, 1, 23, 3, 70), dActionEntry (280, 1, 23, 3, 70), dActionEntry (281, 1, 23, 3, 70), dActionEntry (282, 1, 23, 3, 70), 
			dActionEntry (285, 1, 23, 3, 70), dActionEntry (286, 1, 23, 3, 70), dActionEntry (125, 1, 37, 7, 91), dActionEntry (261, 1, 37, 7, 91), 
			dActionEntry (262, 1, 37, 7, 91), dActionEntry (263, 1, 37, 7, 91), dActionEntry (266, 1, 37, 7, 91), dActionEntry (267, 1, 37, 7, 91), 
			dActionEntry (268, 1, 37, 7, 91), dActionEntry (269, 1, 37, 7, 91), dActionEntry (270, 1, 37, 7, 91), dActionEntry (271, 1, 37, 7, 91), 
			dActionEntry (272, 1, 37, 7, 91), dActionEntry (273, 1, 37, 7, 91), dActionEntry (274, 1, 37, 7, 91), dActionEntry (276, 1, 37, 7, 91), 
			dActionEntry (277, 1, 37, 7, 91), dActionEntry (278, 1, 37, 7, 91), dActionEntry (279, 1, 37, 7, 91), dActionEntry (280, 1, 37, 7, 91), 
			dActionEntry (282, 1, 37, 7, 91), dActionEntry (285, 1, 37, 7, 91), dActionEntry (286, 1, 37, 7, 91), dActionEntry (59, 1, 43, 3, 86), 
			dActionEntry (91, 1, 43, 3, 86), dActionEntry (254, 1, 43, 3, 86), dActionEntry (93, 0, 380, 0, 0), dActionEntry (288, 0, 111, 0, 0), 
			dActionEntry (125, 1, 23, 3, 70), dActionEntry (261, 1, 23, 3, 70), dActionEntry (262, 1, 23, 3, 70), dActionEntry (263, 1, 23, 3, 70), 
			dActionEntry (266, 1, 23, 3, 70), dActionEntry (267, 1, 23, 3, 70), dActionEntry (268, 1, 23, 3, 70), dActionEntry (269, 1, 23, 3, 70), 
			dActionEntry (270, 1, 23, 3, 70), dActionEntry (271, 1, 23, 3, 70), dActionEntry (272, 1, 23, 3, 70), dActionEntry (273, 1, 23, 3, 70), 
			dActionEntry (274, 1, 23, 3, 70), dActionEntry (276, 1, 23, 3, 70), dActionEntry (277, 1, 23, 3, 70), dActionEntry (278, 1, 23, 3, 70), 
			dActionEntry (279, 1, 23, 3, 70), dActionEntry (280, 1, 23, 3, 70), dActionEntry (282, 1, 23, 3, 70), dActionEntry (285, 1, 23, 3, 70), 
			dActionEntry (286, 1, 23, 3, 70), dActionEntry (93, 0, 381, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (123, 1, 20, 4, 105), 
			dActionEntry (125, 0, 383, 0, 0), dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 101, 0, 0), 
			dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), 
			dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 78, 0, 0), dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 99, 0, 0), 
			dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 100, 0, 0), 
			dActionEntry (286, 0, 93, 0, 0), dActionEntry (125, 1, 45, 3, 101), dActionEntry (283, 1, 45, 3, 101), dActionEntry (284, 1, 45, 3, 101), 
			dActionEntry (59, 1, 28, 3, 108), dActionEntry (288, 1, 28, 3, 108), dActionEntry (41, 0, 385, 0, 0), dActionEntry (61, 0, 386, 0, 0), 
			dActionEntry (59, 1, 28, 3, 109), dActionEntry (288, 1, 28, 3, 109), dActionEntry (93, 0, 387, 0, 0), dActionEntry (288, 0, 111, 0, 0), 
			dActionEntry (59, 1, 20, 3, 104), dActionEntry (288, 1, 20, 3, 104), dActionEntry (41, 0, 389, 0, 0), dActionEntry (44, 0, 228, 0, 0), 
			dActionEntry (59, 1, 36, 5, 89), dActionEntry (91, 0, 391, 0, 0), dActionEntry (59, 1, 43, 4, 87), dActionEntry (91, 1, 43, 4, 87), 
			dActionEntry (254, 1, 43, 4, 87), dActionEntry (125, 0, 392, 0, 0), dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), 
			dActionEntry (263, 0, 101, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), 
			dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), 
			dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 78, 0, 0), dActionEntry (277, 0, 90, 0, 0), 
			dActionEntry (278, 0, 99, 0, 0), dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), 
			dActionEntry (285, 0, 100, 0, 0), dActionEntry (286, 0, 93, 0, 0), dActionEntry (125, 1, 23, 2, 69), dActionEntry (283, 1, 23, 2, 69), 
			dActionEntry (284, 1, 23, 2, 69), dActionEntry (125, 1, 45, 4, 100), dActionEntry (283, 1, 45, 4, 100), dActionEntry (284, 1, 45, 4, 100), 
			dActionEntry (40, 0, 116, 0, 0), dActionEntry (258, 0, 113, 0, 0), dActionEntry (263, 0, 118, 0, 0), dActionEntry (275, 0, 394, 0, 0), 
			dActionEntry (59, 1, 43, 3, 86), dActionEntry (91, 1, 43, 3, 86), dActionEntry (288, 1, 43, 3, 86), dActionEntry (93, 0, 396, 0, 0), 
			dActionEntry (288, 0, 111, 0, 0), dActionEntry (59, 1, 20, 4, 105), dActionEntry (288, 1, 20, 4, 105), dActionEntry (93, 0, 397, 0, 0), 
			dActionEntry (288, 0, 111, 0, 0), dActionEntry (125, 1, 23, 3, 70), dActionEntry (283, 1, 23, 3, 70), dActionEntry (284, 1, 23, 3, 70), 
			dActionEntry (125, 1, 38, 10, 95), dActionEntry (261, 1, 38, 10, 95), dActionEntry (262, 1, 38, 10, 95), dActionEntry (263, 1, 38, 10, 95), 
			dActionEntry (266, 1, 38, 10, 95), dActionEntry (267, 1, 38, 10, 95), dActionEntry (268, 1, 38, 10, 95), dActionEntry (269, 1, 38, 10, 95), 
			dActionEntry (270, 1, 38, 10, 95), dActionEntry (271, 1, 38, 10, 95), dActionEntry (272, 1, 38, 10, 95), dActionEntry (273, 1, 38, 10, 95), 
			dActionEntry (274, 1, 38, 10, 95), dActionEntry (276, 1, 38, 10, 95), dActionEntry (277, 1, 38, 10, 95), dActionEntry (278, 1, 38, 10, 95), 
			dActionEntry (279, 1, 38, 10, 95), dActionEntry (280, 1, 38, 10, 95), dActionEntry (282, 1, 38, 10, 95), dActionEntry (285, 1, 38, 10, 95), 
			dActionEntry (286, 1, 38, 10, 95), dActionEntry (41, 1, 36, 3, 88), dActionEntry (288, 0, 162, 0, 0), dActionEntry (59, 1, 43, 4, 87), 
			dActionEntry (91, 1, 43, 4, 87), dActionEntry (288, 1, 43, 4, 87), dActionEntry (59, 1, 43, 3, 86), dActionEntry (91, 1, 43, 3, 86), 
			dActionEntry (93, 0, 400, 0, 0), dActionEntry (288, 0, 111, 0, 0), dActionEntry (91, 0, 401, 0, 0), dActionEntry (59, 1, 43, 4, 87), 
			dActionEntry (91, 1, 43, 4, 87), dActionEntry (41, 1, 36, 5, 89), dActionEntry (91, 0, 404, 0, 0), dActionEntry (93, 0, 405, 0, 0), 
			dActionEntry (288, 0, 111, 0, 0), dActionEntry (41, 1, 43, 3, 86), dActionEntry (91, 1, 43, 3, 86), dActionEntry (93, 0, 407, 0, 0), 
			dActionEntry (288, 0, 111, 0, 0), dActionEntry (41, 1, 43, 4, 87), dActionEntry (91, 1, 43, 4, 87)};

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



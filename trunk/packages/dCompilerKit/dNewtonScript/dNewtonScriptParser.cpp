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

const dNewtonScriptParser::dActionEntry* dNewtonScriptParser::GetNextAction (dList<dStackPair>& stack, dToken token, dNewtonScriptLexical& scanner) const
{
	static short actionsCount[] = {
			5, 1, 1, 5, 1, 4, 4, 4, 1, 4, 1, 4, 4, 2, 1, 2, 1, 1, 1, 14, 2, 2, 2, 1, 
			2, 14, 14, 12, 1, 2, 14, 4, 14, 2, 11, 2, 14, 2, 2, 1, 1, 14, 1, 12, 4, 2, 2, 1, 
			12, 14, 1, 1, 11, 23, 14, 2, 2, 2, 2, 2, 1, 1, 2, 4, 15, 11, 2, 2, 2, 2, 3, 2, 
			4, 2, 11, 14, 15, 1, 2, 5, 2, 23, 23, 2, 14, 23, 10, 2, 3, 1, 1, 2, 1, 2, 23, 2, 
			2, 2, 23, 23, 1, 1, 2, 3, 1, 1, 15, 14, 15, 11, 1, 2, 11, 3, 2, 2, 2, 2, 3, 2, 
			4, 3, 3, 4, 2, 2, 1, 1, 23, 1, 23, 23, 3, 3, 3, 3, 3, 5, 14, 23, 1, 24, 23, 24, 
			1, 23, 3, 3, 9, 23, 3, 23, 23, 23, 23, 23, 4, 3, 3, 2, 4, 3, 14, 15, 1, 11, 1, 2, 
			3, 2, 2, 3, 3, 4, 2, 3, 3, 3, 3, 3, 2, 3, 5, 2, 10, 2, 2, 23, 1, 1, 3, 2, 
			3, 4, 4, 24, 3, 3, 2, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 2, 23, 9, 3, 2, 2, 3, 
			2, 2, 2, 1, 2, 2, 2, 2, 3, 2, 2, 3, 2, 3, 2, 3, 4, 4, 3, 2, 2, 1, 1, 1, 
			3, 3, 3, 2, 3, 3, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 
			1, 1, 1, 2, 2, 2, 14, 3, 1, 1, 3, 2, 2, 3, 3, 3, 2, 3, 3, 2, 3, 3, 3, 3, 
			3, 5, 14, 1, 1, 2, 4, 2, 3, 2, 3, 4, 23, 24, 3, 3, 1, 1, 1, 1, 23, 2, 2, 2, 
			3, 4, 2, 3, 3, 2, 3, 4, 4, 4, 2, 4, 1, 1, 3, 3, 2, 2, 2, 3, 2, 4, 9, 2, 
			23, 24, 1, 2, 3, 4, 3, 3, 2, 3, 3, 2, 1, 2, 23, 1, 1, 23, 3, 3, 1, 2, 3, 3, 
			4, 1, 24, 23, 3, 2, 4, 2, 3, 1, 23, 3, 1, 2, 1, 1, 2, 2, 3, 2, 2, 3, 2, 3, 
			4, 23, 3, 3, 1, 4, 3, 2, 2, 2, 3, 3, 23, 9, 2, 3, 2, 2, 1, 2, 3, 2, 2, 3, 
			2, 2, 2};
	static short actionsStart[] = {
			0, 5, 6, 7, 12, 13, 17, 21, 25, 26, 30, 31, 35, 39, 41, 42, 44, 45, 46, 47, 61, 63, 65, 67, 
			68, 70, 84, 98, 110, 111, 113, 127, 131, 145, 147, 158, 160, 174, 176, 178, 179, 180, 194, 195, 207, 211, 213, 215, 
			98, 216, 110, 230, 231, 242, 265, 279, 281, 283, 285, 287, 289, 290, 291, 293, 297, 312, 323, 325, 327, 329, 331, 334, 
			336, 340, 342, 353, 367, 382, 383, 385, 390, 392, 415, 438, 440, 454, 477, 438, 487, 490, 491, 438, 492, 493, 495, 438, 
			438, 438, 518, 541, 564, 565, 566, 568, 571, 572, 573, 588, 602, 342, 617, 618, 342, 620, 623, 625, 627, 629, 331, 631, 
			633, 620, 637, 640, 644, 646, 648, 649, 650, 673, 674, 697, 720, 723, 726, 331, 729, 732, 737, 751, 774, 775, 799, 822, 
			846, 847, 870, 331, 873, 882, 331, 905, 928, 951, 974, 997, 1020, 331, 620, 1024, 1026, 1030, 1033, 1047, 1062, 342, 1063, 1064, 
			331, 1066, 1068, 620, 1070, 1073, 1077, 620, 1079, 1082, 1085, 331, 1088, 1090, 1093, 1098, 477, 1100, 1102, 1104, 1127, 1128, 870, 1129, 
			620, 1131, 1135, 1139, 1163, 1166, 1169, 1171, 1172, 1173, 1174, 1175, 1176, 1180, 1181, 1182, 1183, 1184, 1186, 1209, 1218, 1221, 1223, 620, 
			1225, 1227, 287, 1229, 1230, 1232, 1234, 1236, 620, 1238, 1240, 1242, 1245, 1247, 1250, 620, 1252, 1256, 1260, 1263, 1265, 110, 1267, 1268, 
			331, 1269, 1272, 1275, 620, 1277, 1280, 1282, 1283, 1284, 1285, 1287, 1288, 1289, 1290, 1291, 1292, 1293, 1294, 1295, 1296, 1297, 1301, 1302, 
			1303, 1304, 1305, 1306, 1308, 1310, 1312, 1326, 1329, 1330, 1331, 1334, 1336, 1338, 1341, 1344, 1347, 620, 1349, 1352, 1079, 1082, 1354, 331, 
			1090, 1357, 1362, 1376, 1377, 1378, 1380, 1384, 1386, 1389, 1391, 1394, 1398, 1421, 620, 1445, 1448, 1449, 1450, 1451, 1452, 1475, 1477, 1479, 
			1481, 1484, 1488, 1490, 1260, 1493, 620, 1495, 1499, 1503, 438, 1507, 1511, 1512, 1513, 1516, 1519, 1521, 1523, 331, 1525, 1527, 1209, 1531, 
			1533, 1556, 1305, 1580, 620, 1582, 1341, 1344, 1586, 620, 1349, 1588, 1590, 1591, 1593, 1616, 1617, 1618, 1641, 1391, 1173, 1644, 620, 1646, 
			1649, 1653, 1654, 1678, 1701, 1704, 1484, 1706, 1490, 1708, 1709, 1732, 1616, 1735, 1737, 1738, 1739, 1741, 620, 1743, 1745, 620, 1747, 1749, 
			1582, 1752, 1775, 1778, 1305, 1781, 1785, 1788, 1790, 1792, 620, 1794, 1797, 1209, 1820, 1822, 1825, 1827, 1829, 1830, 620, 1832, 1834, 620, 
			1836, 1838, 1840};
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
			dActionEntry (273, 1, 14, 2, 25), dActionEntry (274, 1, 14, 2, 25), dActionEntry (123, 0, 94, 0, 0), dActionEntry (125, 0, 84, 0, 0), 
			dActionEntry (254, 0, 78, 0, 0), dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 103, 0, 0), 
			dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), 
			dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 79, 0, 0), dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 101, 0, 0), 
			dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 102, 0, 0), 
			dActionEntry (286, 0, 93, 0, 0), dActionEntry (125, 1, 13, 2, 41), dActionEntry (256, 1, 13, 2, 41), dActionEntry (261, 1, 13, 2, 41), 
			dActionEntry (262, 1, 13, 2, 41), dActionEntry (263, 1, 13, 2, 41), dActionEntry (266, 1, 13, 2, 41), dActionEntry (267, 1, 13, 2, 41), 
			dActionEntry (268, 1, 13, 2, 41), dActionEntry (269, 1, 13, 2, 41), dActionEntry (270, 1, 13, 2, 41), dActionEntry (271, 1, 13, 2, 41), 
			dActionEntry (272, 1, 13, 2, 41), dActionEntry (273, 1, 13, 2, 41), dActionEntry (274, 1, 13, 2, 41), dActionEntry (91, 1, 29, 2, 58), 
			dActionEntry (263, 1, 29, 2, 58), dActionEntry (91, 1, 29, 2, 61), dActionEntry (263, 1, 29, 2, 61), dActionEntry (91, 1, 29, 2, 59), 
			dActionEntry (263, 1, 29, 2, 59), dActionEntry (91, 1, 29, 2, 60), dActionEntry (263, 1, 29, 2, 60), dActionEntry (59, 0, 108, 0, 0), 
			dActionEntry (254, 0, 105, 0, 0), dActionEntry (40, 0, 109, 0, 0), dActionEntry (263, 0, 110, 0, 0), dActionEntry (91, 0, 44, 0, 0), 
			dActionEntry (263, 1, 25, 2, 50), dActionEntry (59, 1, 4, 8, 9), dActionEntry (255, 1, 4, 8, 9), dActionEntry (259, 1, 4, 8, 9), 
			dActionEntry (262, 1, 4, 8, 9), dActionEntry (40, 0, 112, 0, 0), dActionEntry (125, 1, 26, 2, 44), dActionEntry (256, 1, 26, 2, 44), 
			dActionEntry (261, 1, 26, 2, 44), dActionEntry (262, 1, 26, 2, 44), dActionEntry (263, 1, 26, 2, 44), dActionEntry (266, 1, 26, 2, 44), 
			dActionEntry (267, 1, 26, 2, 44), dActionEntry (268, 1, 26, 2, 44), dActionEntry (269, 1, 26, 2, 44), dActionEntry (270, 1, 26, 2, 44), 
			dActionEntry (271, 1, 26, 2, 44), dActionEntry (272, 1, 26, 2, 44), dActionEntry (273, 1, 26, 2, 44), dActionEntry (274, 1, 26, 2, 44), 
			dActionEntry (262, 1, 14, 2, 24), dActionEntry (263, 1, 14, 2, 24), dActionEntry (266, 1, 14, 2, 24), dActionEntry (267, 1, 14, 2, 24), 
			dActionEntry (268, 1, 14, 2, 24), dActionEntry (269, 1, 14, 2, 24), dActionEntry (270, 1, 14, 2, 24), dActionEntry (271, 1, 14, 2, 24), 
			dActionEntry (272, 1, 14, 2, 24), dActionEntry (273, 1, 14, 2, 24), dActionEntry (274, 1, 14, 2, 24), dActionEntry (93, 1, 28, 1, 114), 
			dActionEntry (288, 1, 28, 1, 114), dActionEntry (93, 1, 28, 1, 113), dActionEntry (288, 1, 28, 1, 113), dActionEntry (93, 0, 114, 0, 0), 
			dActionEntry (288, 0, 113, 0, 0), dActionEntry (91, 1, 31, 2, 67), dActionEntry (263, 1, 31, 2, 67), dActionEntry (40, 0, 118, 0, 0), 
			dActionEntry (258, 0, 115, 0, 0), dActionEntry (263, 0, 120, 0, 0), dActionEntry (93, 1, 28, 1, 112), dActionEntry (288, 1, 28, 1, 112), 
			dActionEntry (40, 0, 123, 0, 0), dActionEntry (91, 0, 121, 0, 0), dActionEntry (93, 1, 42, 1, 92), dActionEntry (288, 1, 42, 1, 92), 
			dActionEntry (91, 1, 30, 2, 66), dActionEntry (263, 1, 30, 2, 66), dActionEntry (41, 1, 17, 0, 28), dActionEntry (262, 0, 39, 0, 0), 
			dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), 
			dActionEntry (274, 0, 37, 0, 0), dActionEntry (125, 1, 13, 3, 42), dActionEntry (256, 1, 13, 3, 42), dActionEntry (261, 1, 13, 3, 42), 
			dActionEntry (262, 1, 13, 3, 42), dActionEntry (263, 1, 13, 3, 42), dActionEntry (266, 1, 13, 3, 42), dActionEntry (267, 1, 13, 3, 42), 
			dActionEntry (268, 1, 13, 3, 42), dActionEntry (269, 1, 13, 3, 42), dActionEntry (270, 1, 13, 3, 42), dActionEntry (271, 1, 13, 3, 42), 
			dActionEntry (272, 1, 13, 3, 42), dActionEntry (273, 1, 13, 3, 42), dActionEntry (274, 1, 13, 3, 42), dActionEntry (40, 0, 112, 0, 0), 
			dActionEntry (125, 1, 26, 3, 45), dActionEntry (256, 1, 26, 3, 45), dActionEntry (261, 1, 26, 3, 45), dActionEntry (262, 1, 26, 3, 45), 
			dActionEntry (263, 1, 26, 3, 45), dActionEntry (266, 1, 26, 3, 45), dActionEntry (267, 1, 26, 3, 45), dActionEntry (268, 1, 26, 3, 45), 
			dActionEntry (269, 1, 26, 3, 45), dActionEntry (270, 1, 26, 3, 45), dActionEntry (271, 1, 26, 3, 45), dActionEntry (272, 1, 26, 3, 45), 
			dActionEntry (273, 1, 26, 3, 45), dActionEntry (274, 1, 26, 3, 45), dActionEntry (123, 0, 128, 0, 0), dActionEntry (59, 0, 130, 0, 0), 
			dActionEntry (123, 0, 131, 0, 0), dActionEntry (40, 0, 135, 0, 0), dActionEntry (59, 1, 40, 1, 108), dActionEntry (254, 1, 40, 1, 108), 
			dActionEntry (258, 0, 132, 0, 0), dActionEntry (263, 0, 137, 0, 0), dActionEntry (59, 1, 35, 1, 76), dActionEntry (254, 1, 35, 1, 76), 
			dActionEntry (123, 0, 94, 0, 0), dActionEntry (125, 0, 138, 0, 0), dActionEntry (254, 0, 78, 0, 0), dActionEntry (261, 0, 86, 0, 0), 
			dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 103, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 79, 0, 0), 
			dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 101, 0, 0), dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), 
			dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 102, 0, 0), dActionEntry (286, 0, 93, 0, 0), dActionEntry (123, 1, 33, 1, 73), 
			dActionEntry (125, 1, 33, 1, 73), dActionEntry (254, 1, 33, 1, 73), dActionEntry (261, 1, 33, 1, 73), dActionEntry (262, 1, 33, 1, 73), 
			dActionEntry (263, 1, 33, 1, 73), dActionEntry (266, 1, 33, 1, 73), dActionEntry (267, 1, 33, 1, 73), dActionEntry (268, 1, 33, 1, 73), 
			dActionEntry (269, 1, 33, 1, 73), dActionEntry (270, 1, 33, 1, 73), dActionEntry (271, 1, 33, 1, 73), dActionEntry (272, 1, 33, 1, 73), 
			dActionEntry (273, 1, 33, 1, 73), dActionEntry (274, 1, 33, 1, 73), dActionEntry (276, 1, 33, 1, 73), dActionEntry (277, 1, 33, 1, 73), 
			dActionEntry (278, 1, 33, 1, 73), dActionEntry (279, 1, 33, 1, 73), dActionEntry (280, 1, 33, 1, 73), dActionEntry (282, 1, 33, 1, 73), 
			dActionEntry (285, 1, 33, 1, 73), dActionEntry (286, 1, 33, 1, 73), dActionEntry (59, 0, 143, 0, 0), dActionEntry (254, 0, 140, 0, 0), 
			dActionEntry (125, 1, 23, 2, 69), dActionEntry (256, 1, 23, 2, 69), dActionEntry (261, 1, 23, 2, 69), dActionEntry (262, 1, 23, 2, 69), 
			dActionEntry (263, 1, 23, 2, 69), dActionEntry (266, 1, 23, 2, 69), dActionEntry (267, 1, 23, 2, 69), dActionEntry (268, 1, 23, 2, 69), 
			dActionEntry (269, 1, 23, 2, 69), dActionEntry (270, 1, 23, 2, 69), dActionEntry (271, 1, 23, 2, 69), dActionEntry (272, 1, 23, 2, 69), 
			dActionEntry (273, 1, 23, 2, 69), dActionEntry (274, 1, 23, 2, 69), dActionEntry (123, 1, 32, 1, 71), dActionEntry (125, 1, 32, 1, 71), 
			dActionEntry (254, 1, 32, 1, 71), dActionEntry (261, 1, 32, 1, 71), dActionEntry (262, 1, 32, 1, 71), dActionEntry (263, 1, 32, 1, 71), 
			dActionEntry (266, 1, 32, 1, 71), dActionEntry (267, 1, 32, 1, 71), dActionEntry (268, 1, 32, 1, 71), dActionEntry (269, 1, 32, 1, 71), 
			dActionEntry (270, 1, 32, 1, 71), dActionEntry (271, 1, 32, 1, 71), dActionEntry (272, 1, 32, 1, 71), dActionEntry (273, 1, 32, 1, 71), 
			dActionEntry (274, 1, 32, 1, 71), dActionEntry (276, 1, 32, 1, 71), dActionEntry (277, 1, 32, 1, 71), dActionEntry (278, 1, 32, 1, 71), 
			dActionEntry (279, 1, 32, 1, 71), dActionEntry (280, 1, 32, 1, 71), dActionEntry (282, 1, 32, 1, 71), dActionEntry (285, 1, 32, 1, 71), 
			dActionEntry (286, 1, 32, 1, 71), dActionEntry (262, 0, 39, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (59, 1, 35, 1, 75), 
			dActionEntry (61, 0, 146, 0, 0), dActionEntry (254, 1, 35, 1, 75), dActionEntry (40, 0, 147, 0, 0), dActionEntry (40, 0, 148, 0, 0), 
			dActionEntry (40, 0, 150, 0, 0), dActionEntry (59, 1, 39, 1, 99), dActionEntry (254, 1, 39, 1, 99), dActionEntry (123, 0, 94, 0, 0), 
			dActionEntry (125, 0, 152, 0, 0), dActionEntry (254, 0, 78, 0, 0), dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), 
			dActionEntry (263, 0, 103, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), 
			dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), 
			dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 79, 0, 0), dActionEntry (277, 0, 90, 0, 0), 
			dActionEntry (278, 0, 101, 0, 0), dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), 
			dActionEntry (285, 0, 102, 0, 0), dActionEntry (286, 0, 93, 0, 0), dActionEntry (123, 1, 34, 1, 84), dActionEntry (125, 1, 34, 1, 84), 
			dActionEntry (254, 1, 34, 1, 84), dActionEntry (261, 1, 34, 1, 84), dActionEntry (262, 1, 34, 1, 84), dActionEntry (263, 1, 34, 1, 84), 
			dActionEntry (266, 1, 34, 1, 84), dActionEntry (267, 1, 34, 1, 84), dActionEntry (268, 1, 34, 1, 84), dActionEntry (269, 1, 34, 1, 84), 
			dActionEntry (270, 1, 34, 1, 84), dActionEntry (271, 1, 34, 1, 84), dActionEntry (272, 1, 34, 1, 84), dActionEntry (273, 1, 34, 1, 84), 
			dActionEntry (274, 1, 34, 1, 84), dActionEntry (276, 1, 34, 1, 84), dActionEntry (277, 1, 34, 1, 84), dActionEntry (278, 1, 34, 1, 84), 
			dActionEntry (279, 1, 34, 1, 84), dActionEntry (280, 1, 34, 1, 84), dActionEntry (282, 1, 34, 1, 84), dActionEntry (285, 1, 34, 1, 84), 
			dActionEntry (286, 1, 34, 1, 84), dActionEntry (123, 1, 34, 1, 91), dActionEntry (125, 1, 34, 1, 91), dActionEntry (254, 1, 34, 1, 91), 
			dActionEntry (261, 1, 34, 1, 91), dActionEntry (262, 1, 34, 1, 91), dActionEntry (263, 1, 34, 1, 91), dActionEntry (266, 1, 34, 1, 91), 
			dActionEntry (267, 1, 34, 1, 91), dActionEntry (268, 1, 34, 1, 91), dActionEntry (269, 1, 34, 1, 91), dActionEntry (270, 1, 34, 1, 91), 
			dActionEntry (271, 1, 34, 1, 91), dActionEntry (272, 1, 34, 1, 91), dActionEntry (273, 1, 34, 1, 91), dActionEntry (274, 1, 34, 1, 91), 
			dActionEntry (276, 1, 34, 1, 91), dActionEntry (277, 1, 34, 1, 91), dActionEntry (278, 1, 34, 1, 91), dActionEntry (279, 1, 34, 1, 91), 
			dActionEntry (280, 1, 34, 1, 91), dActionEntry (282, 1, 34, 1, 91), dActionEntry (285, 1, 34, 1, 91), dActionEntry (286, 1, 34, 1, 91), 
			dActionEntry (61, 0, 156, 0, 0), dActionEntry (40, 0, 157, 0, 0), dActionEntry (59, 1, 38, 1, 98), dActionEntry (254, 1, 38, 1, 98), 
			dActionEntry (40, 0, 160, 0, 0), dActionEntry (61, 1, 42, 1, 92), dActionEntry (91, 0, 158, 0, 0), dActionEntry (263, 0, 161, 0, 0), 
			dActionEntry (59, 0, 162, 0, 0), dActionEntry (59, 0, 163, 0, 0), dActionEntry (125, 1, 21, 1, 34), dActionEntry (256, 1, 21, 1, 34), 
			dActionEntry (261, 1, 21, 1, 34), dActionEntry (262, 1, 21, 1, 34), dActionEntry (263, 1, 21, 1, 34), dActionEntry (266, 1, 21, 1, 34), 
			dActionEntry (267, 1, 21, 1, 34), dActionEntry (268, 1, 21, 1, 34), dActionEntry (269, 1, 21, 1, 34), dActionEntry (270, 1, 21, 1, 34), 
			dActionEntry (271, 1, 21, 1, 34), dActionEntry (272, 1, 21, 1, 34), dActionEntry (273, 1, 21, 1, 34), dActionEntry (274, 1, 21, 1, 34), 
			dActionEntry (125, 1, 13, 3, 40), dActionEntry (256, 1, 13, 3, 40), dActionEntry (261, 1, 13, 3, 40), dActionEntry (262, 1, 13, 3, 40), 
			dActionEntry (263, 1, 13, 3, 40), dActionEntry (266, 1, 13, 3, 40), dActionEntry (267, 1, 13, 3, 40), dActionEntry (268, 1, 13, 3, 40), 
			dActionEntry (269, 1, 13, 3, 40), dActionEntry (270, 1, 13, 3, 40), dActionEntry (271, 1, 13, 3, 40), dActionEntry (272, 1, 13, 3, 40), 
			dActionEntry (273, 1, 13, 3, 40), dActionEntry (274, 1, 13, 3, 40), dActionEntry (59, 1, 22, 1, 36), dActionEntry (125, 1, 22, 1, 36), 
			dActionEntry (256, 1, 22, 1, 36), dActionEntry (261, 1, 22, 1, 36), dActionEntry (262, 1, 22, 1, 36), dActionEntry (263, 1, 22, 1, 36), 
			dActionEntry (266, 1, 22, 1, 36), dActionEntry (267, 1, 22, 1, 36), dActionEntry (268, 1, 22, 1, 36), dActionEntry (269, 1, 22, 1, 36), 
			dActionEntry (270, 1, 22, 1, 36), dActionEntry (271, 1, 22, 1, 36), dActionEntry (272, 1, 22, 1, 36), dActionEntry (273, 1, 22, 1, 36), 
			dActionEntry (274, 1, 22, 1, 36), dActionEntry (40, 0, 165, 0, 0), dActionEntry (91, 0, 44, 0, 0), dActionEntry (263, 1, 25, 3, 52), 
			dActionEntry (40, 0, 70, 0, 0), dActionEntry (258, 0, 66, 0, 0), dActionEntry (263, 0, 72, 0, 0), dActionEntry (91, 1, 31, 3, 68), 
			dActionEntry (263, 1, 31, 3, 68), dActionEntry (41, 1, 28, 1, 114), dActionEntry (288, 1, 28, 1, 114), dActionEntry (41, 1, 28, 1, 113), 
			dActionEntry (288, 1, 28, 1, 113), dActionEntry (41, 0, 169, 0, 0), dActionEntry (288, 0, 168, 0, 0), dActionEntry (41, 1, 28, 1, 112), 
			dActionEntry (288, 1, 28, 1, 112), dActionEntry (40, 0, 173, 0, 0), dActionEntry (41, 1, 42, 1, 92), dActionEntry (91, 0, 171, 0, 0), 
			dActionEntry (288, 1, 42, 1, 92), dActionEntry (91, 0, 175, 0, 0), dActionEntry (93, 1, 42, 2, 93), dActionEntry (288, 1, 42, 2, 93), 
			dActionEntry (40, 0, 179, 0, 0), dActionEntry (41, 0, 180, 0, 0), dActionEntry (258, 0, 176, 0, 0), dActionEntry (263, 0, 182, 0, 0), 
			dActionEntry (41, 1, 17, 1, 29), dActionEntry (44, 0, 184, 0, 0), dActionEntry (41, 1, 15, 1, 26), dActionEntry (44, 1, 15, 1, 26), 
			dActionEntry (41, 0, 185, 0, 0), dActionEntry (263, 0, 186, 0, 0), dActionEntry (123, 0, 94, 0, 0), dActionEntry (125, 0, 188, 0, 0), 
			dActionEntry (254, 0, 78, 0, 0), dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 103, 0, 0), 
			dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), 
			dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 79, 0, 0), dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 101, 0, 0), 
			dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 102, 0, 0), 
			dActionEntry (286, 0, 93, 0, 0), dActionEntry (278, 0, 189, 0, 0), dActionEntry (123, 1, 36, 2, 78), dActionEntry (125, 1, 36, 2, 78), 
			dActionEntry (254, 1, 36, 2, 78), dActionEntry (261, 1, 36, 2, 78), dActionEntry (262, 1, 36, 2, 78), dActionEntry (263, 1, 36, 2, 78), 
			dActionEntry (266, 1, 36, 2, 78), dActionEntry (267, 1, 36, 2, 78), dActionEntry (268, 1, 36, 2, 78), dActionEntry (269, 1, 36, 2, 78), 
			dActionEntry (270, 1, 36, 2, 78), dActionEntry (271, 1, 36, 2, 78), dActionEntry (272, 1, 36, 2, 78), dActionEntry (273, 1, 36, 2, 78), 
			dActionEntry (274, 1, 36, 2, 78), dActionEntry (276, 1, 36, 2, 78), dActionEntry (277, 1, 36, 2, 78), dActionEntry (278, 1, 36, 2, 78), 
			dActionEntry (279, 1, 36, 2, 78), dActionEntry (280, 1, 36, 2, 78), dActionEntry (282, 1, 36, 2, 78), dActionEntry (285, 1, 36, 2, 78), 
			dActionEntry (286, 1, 36, 2, 78), dActionEntry (123, 1, 36, 2, 77), dActionEntry (125, 1, 36, 2, 77), dActionEntry (254, 1, 36, 2, 77), 
			dActionEntry (261, 1, 36, 2, 77), dActionEntry (262, 1, 36, 2, 77), dActionEntry (263, 1, 36, 2, 77), dActionEntry (266, 1, 36, 2, 77), 
			dActionEntry (267, 1, 36, 2, 77), dActionEntry (268, 1, 36, 2, 77), dActionEntry (269, 1, 36, 2, 77), dActionEntry (270, 1, 36, 2, 77), 
			dActionEntry (271, 1, 36, 2, 77), dActionEntry (272, 1, 36, 2, 77), dActionEntry (273, 1, 36, 2, 77), dActionEntry (274, 1, 36, 2, 77), 
			dActionEntry (276, 1, 36, 2, 77), dActionEntry (277, 1, 36, 2, 77), dActionEntry (278, 1, 36, 2, 77), dActionEntry (279, 1, 36, 2, 77), 
			dActionEntry (280, 1, 36, 2, 77), dActionEntry (282, 1, 36, 2, 77), dActionEntry (285, 1, 36, 2, 77), dActionEntry (286, 1, 36, 2, 77), 
			dActionEntry (59, 1, 28, 1, 114), dActionEntry (254, 1, 28, 1, 114), dActionEntry (288, 1, 28, 1, 114), dActionEntry (59, 1, 28, 1, 113), 
			dActionEntry (254, 1, 28, 1, 113), dActionEntry (288, 1, 28, 1, 113), dActionEntry (59, 1, 40, 2, 109), dActionEntry (254, 1, 40, 2, 109), 
			dActionEntry (288, 0, 190, 0, 0), dActionEntry (59, 1, 28, 1, 112), dActionEntry (254, 1, 28, 1, 112), dActionEntry (288, 1, 28, 1, 112), 
			dActionEntry (40, 0, 194, 0, 0), dActionEntry (59, 1, 42, 1, 92), dActionEntry (91, 0, 192, 0, 0), dActionEntry (254, 1, 42, 1, 92), 
			dActionEntry (288, 1, 42, 1, 92), dActionEntry (125, 1, 23, 3, 70), dActionEntry (256, 1, 23, 3, 70), dActionEntry (261, 1, 23, 3, 70), 
			dActionEntry (262, 1, 23, 3, 70), dActionEntry (263, 1, 23, 3, 70), dActionEntry (266, 1, 23, 3, 70), dActionEntry (267, 1, 23, 3, 70), 
			dActionEntry (268, 1, 23, 3, 70), dActionEntry (269, 1, 23, 3, 70), dActionEntry (270, 1, 23, 3, 70), dActionEntry (271, 1, 23, 3, 70), 
			dActionEntry (272, 1, 23, 3, 70), dActionEntry (273, 1, 23, 3, 70), dActionEntry (274, 1, 23, 3, 70), dActionEntry (123, 1, 32, 2, 72), 
			dActionEntry (125, 1, 32, 2, 72), dActionEntry (254, 1, 32, 2, 72), dActionEntry (261, 1, 32, 2, 72), dActionEntry (262, 1, 32, 2, 72), 
			dActionEntry (263, 1, 32, 2, 72), dActionEntry (266, 1, 32, 2, 72), dActionEntry (267, 1, 32, 2, 72), dActionEntry (268, 1, 32, 2, 72), 
			dActionEntry (269, 1, 32, 2, 72), dActionEntry (270, 1, 32, 2, 72), dActionEntry (271, 1, 32, 2, 72), dActionEntry (272, 1, 32, 2, 72), 
			dActionEntry (273, 1, 32, 2, 72), dActionEntry (274, 1, 32, 2, 72), dActionEntry (276, 1, 32, 2, 72), dActionEntry (277, 1, 32, 2, 72), 
			dActionEntry (278, 1, 32, 2, 72), dActionEntry (279, 1, 32, 2, 72), dActionEntry (280, 1, 32, 2, 72), dActionEntry (282, 1, 32, 2, 72), 
			dActionEntry (285, 1, 32, 2, 72), dActionEntry (286, 1, 32, 2, 72), dActionEntry (59, 0, 130, 0, 0), dActionEntry (59, 0, 195, 0, 0), 
			dActionEntry (123, 1, 21, 1, 34), dActionEntry (125, 1, 21, 1, 34), dActionEntry (254, 1, 21, 1, 34), dActionEntry (261, 1, 21, 1, 34), 
			dActionEntry (262, 1, 21, 1, 34), dActionEntry (263, 1, 21, 1, 34), dActionEntry (266, 1, 21, 1, 34), dActionEntry (267, 1, 21, 1, 34), 
			dActionEntry (268, 1, 21, 1, 34), dActionEntry (269, 1, 21, 1, 34), dActionEntry (270, 1, 21, 1, 34), dActionEntry (271, 1, 21, 1, 34), 
			dActionEntry (272, 1, 21, 1, 34), dActionEntry (273, 1, 21, 1, 34), dActionEntry (274, 1, 21, 1, 34), dActionEntry (276, 1, 21, 1, 34), 
			dActionEntry (277, 1, 21, 1, 34), dActionEntry (278, 1, 21, 1, 34), dActionEntry (279, 1, 21, 1, 34), dActionEntry (280, 1, 21, 1, 34), 
			dActionEntry (282, 1, 21, 1, 34), dActionEntry (285, 1, 21, 1, 34), dActionEntry (286, 1, 21, 1, 34), dActionEntry (123, 1, 34, 2, 82), 
			dActionEntry (125, 1, 34, 2, 82), dActionEntry (254, 1, 34, 2, 82), dActionEntry (261, 1, 34, 2, 82), dActionEntry (262, 1, 34, 2, 82), 
			dActionEntry (263, 1, 34, 2, 82), dActionEntry (266, 1, 34, 2, 82), dActionEntry (267, 1, 34, 2, 82), dActionEntry (268, 1, 34, 2, 82), 
			dActionEntry (269, 1, 34, 2, 82), dActionEntry (270, 1, 34, 2, 82), dActionEntry (271, 1, 34, 2, 82), dActionEntry (272, 1, 34, 2, 82), 
			dActionEntry (273, 1, 34, 2, 82), dActionEntry (274, 1, 34, 2, 82), dActionEntry (276, 1, 34, 2, 82), dActionEntry (277, 1, 34, 2, 82), 
			dActionEntry (278, 1, 34, 2, 82), dActionEntry (279, 1, 34, 2, 82), dActionEntry (280, 1, 34, 2, 82), dActionEntry (282, 1, 34, 2, 82), 
			dActionEntry (285, 1, 34, 2, 82), dActionEntry (286, 1, 34, 2, 82), dActionEntry (59, 1, 22, 1, 36), dActionEntry (123, 1, 22, 1, 36), 
			dActionEntry (125, 1, 22, 1, 36), dActionEntry (254, 1, 22, 1, 36), dActionEntry (261, 1, 22, 1, 36), dActionEntry (262, 1, 22, 1, 36), 
			dActionEntry (263, 1, 22, 1, 36), dActionEntry (266, 1, 22, 1, 36), dActionEntry (267, 1, 22, 1, 36), dActionEntry (268, 1, 22, 1, 36), 
			dActionEntry (269, 1, 22, 1, 36), dActionEntry (270, 1, 22, 1, 36), dActionEntry (271, 1, 22, 1, 36), dActionEntry (272, 1, 22, 1, 36), 
			dActionEntry (273, 1, 22, 1, 36), dActionEntry (274, 1, 22, 1, 36), dActionEntry (276, 1, 22, 1, 36), dActionEntry (277, 1, 22, 1, 36), 
			dActionEntry (278, 1, 22, 1, 36), dActionEntry (279, 1, 22, 1, 36), dActionEntry (280, 1, 22, 1, 36), dActionEntry (282, 1, 22, 1, 36), 
			dActionEntry (285, 1, 22, 1, 36), dActionEntry (286, 1, 22, 1, 36), dActionEntry (263, 0, 196, 0, 0), dActionEntry (123, 1, 34, 2, 83), 
			dActionEntry (125, 1, 34, 2, 83), dActionEntry (254, 1, 34, 2, 83), dActionEntry (261, 1, 34, 2, 83), dActionEntry (262, 1, 34, 2, 83), 
			dActionEntry (263, 1, 34, 2, 83), dActionEntry (266, 1, 34, 2, 83), dActionEntry (267, 1, 34, 2, 83), dActionEntry (268, 1, 34, 2, 83), 
			dActionEntry (269, 1, 34, 2, 83), dActionEntry (270, 1, 34, 2, 83), dActionEntry (271, 1, 34, 2, 83), dActionEntry (272, 1, 34, 2, 83), 
			dActionEntry (273, 1, 34, 2, 83), dActionEntry (274, 1, 34, 2, 83), dActionEntry (276, 1, 34, 2, 83), dActionEntry (277, 1, 34, 2, 83), 
			dActionEntry (278, 1, 34, 2, 83), dActionEntry (279, 1, 34, 2, 83), dActionEntry (280, 1, 34, 2, 83), dActionEntry (282, 1, 34, 2, 83), 
			dActionEntry (285, 1, 34, 2, 83), dActionEntry (286, 1, 34, 2, 83), dActionEntry (40, 0, 135, 0, 0), dActionEntry (258, 0, 132, 0, 0), 
			dActionEntry (263, 0, 137, 0, 0), dActionEntry (266, 0, 205, 0, 0), dActionEntry (267, 0, 199, 0, 0), dActionEntry (268, 0, 203, 0, 0), 
			dActionEntry (269, 0, 206, 0, 0), dActionEntry (270, 0, 200, 0, 0), dActionEntry (271, 0, 204, 0, 0), dActionEntry (272, 0, 202, 0, 0), 
			dActionEntry (273, 0, 208, 0, 0), dActionEntry (274, 0, 207, 0, 0), dActionEntry (123, 1, 33, 2, 74), dActionEntry (125, 1, 33, 2, 74), 
			dActionEntry (254, 1, 33, 2, 74), dActionEntry (261, 1, 33, 2, 74), dActionEntry (262, 1, 33, 2, 74), dActionEntry (263, 1, 33, 2, 74), 
			dActionEntry (266, 1, 33, 2, 74), dActionEntry (267, 1, 33, 2, 74), dActionEntry (268, 1, 33, 2, 74), dActionEntry (269, 1, 33, 2, 74), 
			dActionEntry (270, 1, 33, 2, 74), dActionEntry (271, 1, 33, 2, 74), dActionEntry (272, 1, 33, 2, 74), dActionEntry (273, 1, 33, 2, 74), 
			dActionEntry (274, 1, 33, 2, 74), dActionEntry (276, 1, 33, 2, 74), dActionEntry (277, 1, 33, 2, 74), dActionEntry (278, 1, 33, 2, 74), 
			dActionEntry (279, 1, 33, 2, 74), dActionEntry (280, 1, 33, 2, 74), dActionEntry (282, 1, 33, 2, 74), dActionEntry (285, 1, 33, 2, 74), 
			dActionEntry (286, 1, 33, 2, 74), dActionEntry (123, 0, 94, 0, 0), dActionEntry (125, 0, 210, 0, 0), dActionEntry (254, 0, 78, 0, 0), 
			dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 103, 0, 0), dActionEntry (266, 0, 33, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), 
			dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), 
			dActionEntry (276, 0, 79, 0, 0), dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 101, 0, 0), dActionEntry (279, 0, 77, 0, 0), 
			dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 102, 0, 0), dActionEntry (286, 0, 93, 0, 0), 
			dActionEntry (123, 1, 23, 2, 69), dActionEntry (125, 1, 23, 2, 69), dActionEntry (254, 1, 23, 2, 69), dActionEntry (261, 1, 23, 2, 69), 
			dActionEntry (262, 1, 23, 2, 69), dActionEntry (263, 1, 23, 2, 69), dActionEntry (266, 1, 23, 2, 69), dActionEntry (267, 1, 23, 2, 69), 
			dActionEntry (268, 1, 23, 2, 69), dActionEntry (269, 1, 23, 2, 69), dActionEntry (270, 1, 23, 2, 69), dActionEntry (271, 1, 23, 2, 69), 
			dActionEntry (272, 1, 23, 2, 69), dActionEntry (273, 1, 23, 2, 69), dActionEntry (274, 1, 23, 2, 69), dActionEntry (276, 1, 23, 2, 69), 
			dActionEntry (277, 1, 23, 2, 69), dActionEntry (278, 1, 23, 2, 69), dActionEntry (279, 1, 23, 2, 69), dActionEntry (280, 1, 23, 2, 69), 
			dActionEntry (282, 1, 23, 2, 69), dActionEntry (285, 1, 23, 2, 69), dActionEntry (286, 1, 23, 2, 69), dActionEntry (123, 1, 34, 2, 81), 
			dActionEntry (125, 1, 34, 2, 81), dActionEntry (254, 1, 34, 2, 81), dActionEntry (261, 1, 34, 2, 81), dActionEntry (262, 1, 34, 2, 81), 
			dActionEntry (263, 1, 34, 2, 81), dActionEntry (266, 1, 34, 2, 81), dActionEntry (267, 1, 34, 2, 81), dActionEntry (268, 1, 34, 2, 81), 
			dActionEntry (269, 1, 34, 2, 81), dActionEntry (270, 1, 34, 2, 81), dActionEntry (271, 1, 34, 2, 81), dActionEntry (272, 1, 34, 2, 81), 
			dActionEntry (273, 1, 34, 2, 81), dActionEntry (274, 1, 34, 2, 81), dActionEntry (276, 1, 34, 2, 81), dActionEntry (277, 1, 34, 2, 81), 
			dActionEntry (278, 1, 34, 2, 81), dActionEntry (279, 1, 34, 2, 81), dActionEntry (280, 1, 34, 2, 81), dActionEntry (282, 1, 34, 2, 81), 
			dActionEntry (285, 1, 34, 2, 81), dActionEntry (286, 1, 34, 2, 81), dActionEntry (123, 1, 34, 2, 80), dActionEntry (125, 1, 34, 2, 80), 
			dActionEntry (254, 1, 34, 2, 80), dActionEntry (261, 1, 34, 2, 80), dActionEntry (262, 1, 34, 2, 80), dActionEntry (263, 1, 34, 2, 80), 
			dActionEntry (266, 1, 34, 2, 80), dActionEntry (267, 1, 34, 2, 80), dActionEntry (268, 1, 34, 2, 80), dActionEntry (269, 1, 34, 2, 80), 
			dActionEntry (270, 1, 34, 2, 80), dActionEntry (271, 1, 34, 2, 80), dActionEntry (272, 1, 34, 2, 80), dActionEntry (273, 1, 34, 2, 80), 
			dActionEntry (274, 1, 34, 2, 80), dActionEntry (276, 1, 34, 2, 80), dActionEntry (277, 1, 34, 2, 80), dActionEntry (278, 1, 34, 2, 80), 
			dActionEntry (279, 1, 34, 2, 80), dActionEntry (280, 1, 34, 2, 80), dActionEntry (282, 1, 34, 2, 80), dActionEntry (285, 1, 34, 2, 80), 
			dActionEntry (286, 1, 34, 2, 80), dActionEntry (123, 1, 34, 2, 79), dActionEntry (125, 1, 34, 2, 79), dActionEntry (254, 1, 34, 2, 79), 
			dActionEntry (261, 1, 34, 2, 79), dActionEntry (262, 1, 34, 2, 79), dActionEntry (263, 1, 34, 2, 79), dActionEntry (266, 1, 34, 2, 79), 
			dActionEntry (267, 1, 34, 2, 79), dActionEntry (268, 1, 34, 2, 79), dActionEntry (269, 1, 34, 2, 79), dActionEntry (270, 1, 34, 2, 79), 
			dActionEntry (271, 1, 34, 2, 79), dActionEntry (272, 1, 34, 2, 79), dActionEntry (273, 1, 34, 2, 79), dActionEntry (274, 1, 34, 2, 79), 
			dActionEntry (276, 1, 34, 2, 79), dActionEntry (277, 1, 34, 2, 79), dActionEntry (278, 1, 34, 2, 79), dActionEntry (279, 1, 34, 2, 79), 
			dActionEntry (280, 1, 34, 2, 79), dActionEntry (282, 1, 34, 2, 79), dActionEntry (285, 1, 34, 2, 79), dActionEntry (286, 1, 34, 2, 79), 
			dActionEntry (40, 0, 135, 0, 0), dActionEntry (258, 0, 132, 0, 0), dActionEntry (263, 0, 137, 0, 0), dActionEntry (275, 0, 211, 0, 0), 
			dActionEntry (61, 1, 42, 2, 93), dActionEntry (91, 0, 215, 0, 0), dActionEntry (40, 0, 179, 0, 0), dActionEntry (41, 0, 216, 0, 0), 
			dActionEntry (258, 0, 176, 0, 0), dActionEntry (263, 0, 182, 0, 0), dActionEntry (59, 1, 26, 2, 44), dActionEntry (61, 1, 26, 2, 44), 
			dActionEntry (254, 1, 26, 2, 44), dActionEntry (125, 1, 21, 2, 35), dActionEntry (256, 1, 21, 2, 35), dActionEntry (261, 1, 21, 2, 35), 
			dActionEntry (262, 1, 21, 2, 35), dActionEntry (263, 1, 21, 2, 35), dActionEntry (266, 1, 21, 2, 35), dActionEntry (267, 1, 21, 2, 35), 
			dActionEntry (268, 1, 21, 2, 35), dActionEntry (269, 1, 21, 2, 35), dActionEntry (270, 1, 21, 2, 35), dActionEntry (271, 1, 21, 2, 35), 
			dActionEntry (272, 1, 21, 2, 35), dActionEntry (273, 1, 21, 2, 35), dActionEntry (274, 1, 21, 2, 35), dActionEntry (59, 1, 22, 2, 37), 
			dActionEntry (125, 1, 22, 2, 37), dActionEntry (256, 1, 22, 2, 37), dActionEntry (261, 1, 22, 2, 37), dActionEntry (262, 1, 22, 2, 37), 
			dActionEntry (263, 1, 22, 2, 37), dActionEntry (266, 1, 22, 2, 37), dActionEntry (267, 1, 22, 2, 37), dActionEntry (268, 1, 22, 2, 37), 
			dActionEntry (269, 1, 22, 2, 37), dActionEntry (270, 1, 22, 2, 37), dActionEntry (271, 1, 22, 2, 37), dActionEntry (272, 1, 22, 2, 37), 
			dActionEntry (273, 1, 22, 2, 37), dActionEntry (274, 1, 22, 2, 37), dActionEntry (41, 0, 218, 0, 0), dActionEntry (41, 0, 220, 0, 0), 
			dActionEntry (93, 1, 28, 3, 110), dActionEntry (288, 1, 28, 3, 110), dActionEntry (93, 1, 28, 3, 111), dActionEntry (288, 1, 28, 3, 111), 
			dActionEntry (41, 0, 222, 0, 0), dActionEntry (288, 0, 168, 0, 0), dActionEntry (41, 1, 42, 2, 93), dActionEntry (91, 0, 224, 0, 0), 
			dActionEntry (288, 1, 42, 2, 93), dActionEntry (40, 0, 179, 0, 0), dActionEntry (41, 0, 225, 0, 0), dActionEntry (258, 0, 176, 0, 0), 
			dActionEntry (263, 0, 182, 0, 0), dActionEntry (93, 0, 227, 0, 0), dActionEntry (288, 0, 113, 0, 0), dActionEntry (41, 1, 28, 1, 114), 
			dActionEntry (44, 1, 28, 1, 114), dActionEntry (288, 1, 28, 1, 114), dActionEntry (41, 1, 28, 1, 113), dActionEntry (44, 1, 28, 1, 113), 
			dActionEntry (288, 1, 28, 1, 113), dActionEntry (41, 1, 45, 1, 104), dActionEntry (44, 1, 45, 1, 104), dActionEntry (288, 0, 229, 0, 0), 
			dActionEntry (93, 1, 20, 3, 106), dActionEntry (288, 1, 20, 3, 106), dActionEntry (41, 1, 28, 1, 112), dActionEntry (44, 1, 28, 1, 112), 
			dActionEntry (288, 1, 28, 1, 112), dActionEntry (40, 0, 233, 0, 0), dActionEntry (41, 1, 42, 1, 92), dActionEntry (44, 1, 42, 1, 92), 
			dActionEntry (91, 0, 231, 0, 0), dActionEntry (288, 1, 42, 1, 92), dActionEntry (41, 0, 235, 0, 0), dActionEntry (44, 0, 234, 0, 0), 
			dActionEntry (58, 0, 238, 0, 0), dActionEntry (123, 1, 19, 0, 32), dActionEntry (41, 1, 16, 2, 48), dActionEntry (44, 1, 16, 2, 48), 
			dActionEntry (123, 0, 94, 0, 0), dActionEntry (125, 0, 239, 0, 0), dActionEntry (254, 0, 78, 0, 0), dActionEntry (261, 0, 86, 0, 0), 
			dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 103, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 79, 0, 0), 
			dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 101, 0, 0), dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), 
			dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 102, 0, 0), dActionEntry (286, 0, 93, 0, 0), dActionEntry (278, 1, 23, 2, 69), 
			dActionEntry (40, 0, 240, 0, 0), dActionEntry (41, 0, 242, 0, 0), dActionEntry (288, 0, 168, 0, 0), dActionEntry (59, 1, 42, 2, 93), 
			dActionEntry (91, 0, 244, 0, 0), dActionEntry (254, 1, 42, 2, 93), dActionEntry (288, 1, 42, 2, 93), dActionEntry (40, 0, 179, 0, 0), 
			dActionEntry (41, 0, 245, 0, 0), dActionEntry (258, 0, 176, 0, 0), dActionEntry (263, 0, 182, 0, 0), dActionEntry (59, 1, 22, 2, 37), 
			dActionEntry (123, 1, 22, 2, 37), dActionEntry (125, 1, 22, 2, 37), dActionEntry (254, 1, 22, 2, 37), dActionEntry (261, 1, 22, 2, 37), 
			dActionEntry (262, 1, 22, 2, 37), dActionEntry (263, 1, 22, 2, 37), dActionEntry (266, 1, 22, 2, 37), dActionEntry (267, 1, 22, 2, 37), 
			dActionEntry (268, 1, 22, 2, 37), dActionEntry (269, 1, 22, 2, 37), dActionEntry (270, 1, 22, 2, 37), dActionEntry (271, 1, 22, 2, 37), 
			dActionEntry (272, 1, 22, 2, 37), dActionEntry (273, 1, 22, 2, 37), dActionEntry (274, 1, 22, 2, 37), dActionEntry (276, 1, 22, 2, 37), 
			dActionEntry (277, 1, 22, 2, 37), dActionEntry (278, 1, 22, 2, 37), dActionEntry (279, 1, 22, 2, 37), dActionEntry (280, 1, 22, 2, 37), 
			dActionEntry (282, 1, 22, 2, 37), dActionEntry (285, 1, 22, 2, 37), dActionEntry (286, 1, 22, 2, 37), dActionEntry (59, 1, 26, 3, 45), 
			dActionEntry (61, 1, 26, 3, 45), dActionEntry (254, 1, 26, 3, 45), dActionEntry (59, 1, 27, 3, 46), dActionEntry (254, 1, 27, 3, 46), 
			dActionEntry (288, 0, 190, 0, 0), dActionEntry (41, 0, 247, 0, 0), dActionEntry (288, 0, 168, 0, 0), dActionEntry (263, 1, 29, 1, 54), 
			dActionEntry (263, 1, 29, 1, 57), dActionEntry (263, 0, 250, 0, 0), dActionEntry (263, 1, 29, 1, 62), dActionEntry (263, 1, 29, 1, 55), 
			dActionEntry (267, 0, 251, 0, 0), dActionEntry (268, 0, 253, 0, 0), dActionEntry (269, 0, 254, 0, 0), dActionEntry (270, 0, 252, 0, 0), 
			dActionEntry (263, 1, 29, 1, 53), dActionEntry (263, 1, 29, 1, 56), dActionEntry (263, 1, 29, 1, 64), dActionEntry (263, 1, 29, 1, 63), 
			dActionEntry (41, 0, 255, 0, 0), dActionEntry (288, 0, 168, 0, 0), dActionEntry (123, 1, 23, 3, 70), dActionEntry (125, 1, 23, 3, 70), 
			dActionEntry (254, 1, 23, 3, 70), dActionEntry (261, 1, 23, 3, 70), dActionEntry (262, 1, 23, 3, 70), dActionEntry (263, 1, 23, 3, 70), 
			dActionEntry (266, 1, 23, 3, 70), dActionEntry (267, 1, 23, 3, 70), dActionEntry (268, 1, 23, 3, 70), dActionEntry (269, 1, 23, 3, 70), 
			dActionEntry (270, 1, 23, 3, 70), dActionEntry (271, 1, 23, 3, 70), dActionEntry (272, 1, 23, 3, 70), dActionEntry (273, 1, 23, 3, 70), 
			dActionEntry (274, 1, 23, 3, 70), dActionEntry (276, 1, 23, 3, 70), dActionEntry (277, 1, 23, 3, 70), dActionEntry (278, 1, 23, 3, 70), 
			dActionEntry (279, 1, 23, 3, 70), dActionEntry (280, 1, 23, 3, 70), dActionEntry (282, 1, 23, 3, 70), dActionEntry (285, 1, 23, 3, 70), 
			dActionEntry (286, 1, 23, 3, 70), dActionEntry (266, 0, 262, 0, 0), dActionEntry (267, 0, 256, 0, 0), dActionEntry (268, 0, 260, 0, 0), 
			dActionEntry (269, 0, 263, 0, 0), dActionEntry (270, 0, 257, 0, 0), dActionEntry (271, 0, 261, 0, 0), dActionEntry (272, 0, 259, 0, 0), 
			dActionEntry (273, 0, 265, 0, 0), dActionEntry (274, 0, 264, 0, 0), dActionEntry (59, 1, 37, 3, 96), dActionEntry (254, 1, 37, 3, 96), 
			dActionEntry (288, 0, 190, 0, 0), dActionEntry (41, 0, 266, 0, 0), dActionEntry (288, 0, 168, 0, 0), dActionEntry (93, 0, 267, 0, 0), 
			dActionEntry (288, 0, 113, 0, 0), dActionEntry (59, 1, 20, 3, 106), dActionEntry (254, 1, 20, 3, 106), dActionEntry (41, 0, 269, 0, 0), 
			dActionEntry (44, 0, 234, 0, 0), dActionEntry (41, 0, 271, 0, 0), dActionEntry (123, 1, 18, 0, 30), dActionEntry (257, 0, 273, 0, 0), 
			dActionEntry (41, 1, 28, 3, 110), dActionEntry (288, 1, 28, 3, 110), dActionEntry (41, 1, 28, 3, 111), dActionEntry (288, 1, 28, 3, 111), 
			dActionEntry (93, 0, 274, 0, 0), dActionEntry (288, 0, 113, 0, 0), dActionEntry (41, 1, 20, 3, 106), dActionEntry (288, 1, 20, 3, 106), 
			dActionEntry (41, 0, 276, 0, 0), dActionEntry (44, 0, 234, 0, 0), dActionEntry (91, 1, 43, 3, 94), dActionEntry (93, 1, 43, 3, 94), 
			dActionEntry (288, 1, 43, 3, 94), dActionEntry (93, 0, 277, 0, 0), dActionEntry (288, 0, 113, 0, 0), dActionEntry (40, 0, 179, 0, 0), 
			dActionEntry (258, 0, 176, 0, 0), dActionEntry (263, 0, 182, 0, 0), dActionEntry (41, 0, 279, 0, 0), dActionEntry (288, 0, 168, 0, 0), 
			dActionEntry (41, 1, 42, 2, 93), dActionEntry (44, 1, 42, 2, 93), dActionEntry (91, 0, 281, 0, 0), dActionEntry (288, 1, 42, 2, 93), 
			dActionEntry (40, 0, 179, 0, 0), dActionEntry (41, 0, 282, 0, 0), dActionEntry (258, 0, 176, 0, 0), dActionEntry (263, 0, 182, 0, 0), 
			dActionEntry (40, 0, 287, 0, 0), dActionEntry (258, 0, 284, 0, 0), dActionEntry (263, 0, 289, 0, 0), dActionEntry (93, 1, 20, 4, 107), 
			dActionEntry (288, 1, 20, 4, 107), dActionEntry (41, 1, 15, 3, 27), dActionEntry (44, 1, 15, 3, 27), dActionEntry (263, 0, 292, 0, 0), 
			dActionEntry (278, 1, 23, 3, 70), dActionEntry (59, 1, 28, 3, 110), dActionEntry (254, 1, 28, 3, 110), dActionEntry (288, 1, 28, 3, 110), 
			dActionEntry (59, 1, 28, 3, 111), dActionEntry (254, 1, 28, 3, 111), dActionEntry (288, 1, 28, 3, 111), dActionEntry (93, 0, 294, 0, 0), 
			dActionEntry (288, 0, 113, 0, 0), dActionEntry (59, 1, 20, 3, 106), dActionEntry (254, 1, 20, 3, 106), dActionEntry (288, 1, 20, 3, 106), 
			dActionEntry (41, 0, 296, 0, 0), dActionEntry (44, 0, 234, 0, 0), dActionEntry (123, 0, 297, 0, 0), dActionEntry (59, 0, 298, 0, 0), 
			dActionEntry (61, 0, 299, 0, 0), dActionEntry (61, 1, 42, 1, 92), dActionEntry (91, 0, 158, 0, 0), dActionEntry (263, 1, 29, 2, 58), 
			dActionEntry (263, 1, 29, 2, 61), dActionEntry (263, 1, 29, 2, 59), dActionEntry (263, 1, 29, 2, 60), dActionEntry (123, 0, 300, 0, 0), 
			dActionEntry (91, 1, 29, 1, 54), dActionEntry (91, 1, 29, 1, 57), dActionEntry (91, 0, 302, 0, 0), dActionEntry (91, 1, 29, 1, 62), 
			dActionEntry (91, 1, 29, 1, 55), dActionEntry (267, 0, 304, 0, 0), dActionEntry (268, 0, 306, 0, 0), dActionEntry (269, 0, 307, 0, 0), 
			dActionEntry (270, 0, 305, 0, 0), dActionEntry (91, 1, 29, 1, 53), dActionEntry (91, 1, 29, 1, 56), dActionEntry (91, 1, 29, 1, 64), 
			dActionEntry (91, 1, 29, 1, 63), dActionEntry (123, 0, 94, 0, 0), dActionEntry (61, 1, 43, 3, 94), dActionEntry (91, 1, 43, 3, 94), 
			dActionEntry (93, 0, 309, 0, 0), dActionEntry (288, 0, 113, 0, 0), dActionEntry (59, 1, 20, 4, 107), dActionEntry (254, 1, 20, 4, 107), 
			dActionEntry (125, 1, 11, 6, 38), dActionEntry (256, 1, 11, 6, 38), dActionEntry (261, 1, 11, 6, 38), dActionEntry (262, 1, 11, 6, 38), 
			dActionEntry (263, 1, 11, 6, 38), dActionEntry (266, 1, 11, 6, 38), dActionEntry (267, 1, 11, 6, 38), dActionEntry (268, 1, 11, 6, 38), 
			dActionEntry (269, 1, 11, 6, 38), dActionEntry (270, 1, 11, 6, 38), dActionEntry (271, 1, 11, 6, 38), dActionEntry (272, 1, 11, 6, 38), 
			dActionEntry (273, 1, 11, 6, 38), dActionEntry (274, 1, 11, 6, 38), dActionEntry (59, 1, 18, 0, 30), dActionEntry (254, 1, 18, 0, 30), 
			dActionEntry (257, 0, 311, 0, 0), dActionEntry (123, 1, 24, 6, 43), dActionEntry (123, 1, 18, 1, 31), dActionEntry (41, 1, 43, 3, 94), 
			dActionEntry (91, 1, 43, 3, 94), dActionEntry (288, 1, 43, 3, 94), dActionEntry (93, 0, 312, 0, 0), dActionEntry (288, 0, 113, 0, 0), 
			dActionEntry (41, 1, 20, 4, 107), dActionEntry (288, 1, 20, 4, 107), dActionEntry (91, 1, 43, 4, 95), dActionEntry (93, 1, 43, 4, 95), 
			dActionEntry (288, 1, 43, 4, 95), dActionEntry (41, 1, 28, 3, 110), dActionEntry (44, 1, 28, 3, 110), dActionEntry (288, 1, 28, 3, 110), 
			dActionEntry (41, 1, 28, 3, 111), dActionEntry (44, 1, 28, 3, 111), dActionEntry (288, 1, 28, 3, 111), dActionEntry (93, 0, 313, 0, 0), 
			dActionEntry (288, 0, 113, 0, 0), dActionEntry (41, 1, 20, 3, 106), dActionEntry (44, 1, 20, 3, 106), dActionEntry (288, 1, 20, 3, 106), 
			dActionEntry (41, 0, 315, 0, 0), dActionEntry (44, 0, 234, 0, 0), dActionEntry (41, 1, 45, 3, 105), dActionEntry (44, 1, 45, 3, 105), 
			dActionEntry (288, 0, 316, 0, 0), dActionEntry (40, 0, 320, 0, 0), dActionEntry (41, 1, 42, 1, 92), dActionEntry (44, 1, 42, 1, 92), 
			dActionEntry (91, 0, 318, 0, 0), dActionEntry (288, 1, 42, 1, 92), dActionEntry (125, 1, 11, 7, 39), dActionEntry (256, 1, 11, 7, 39), 
			dActionEntry (261, 1, 11, 7, 39), dActionEntry (262, 1, 11, 7, 39), dActionEntry (263, 1, 11, 7, 39), dActionEntry (266, 1, 11, 7, 39), 
			dActionEntry (267, 1, 11, 7, 39), dActionEntry (268, 1, 11, 7, 39), dActionEntry (269, 1, 11, 7, 39), dActionEntry (270, 1, 11, 7, 39), 
			dActionEntry (271, 1, 11, 7, 39), dActionEntry (272, 1, 11, 7, 39), dActionEntry (273, 1, 11, 7, 39), dActionEntry (274, 1, 11, 7, 39), 
			dActionEntry (123, 1, 19, 2, 33), dActionEntry (40, 0, 321, 0, 0), dActionEntry (41, 0, 322, 0, 0), dActionEntry (288, 0, 168, 0, 0), 
			dActionEntry (59, 1, 43, 3, 94), dActionEntry (91, 1, 43, 3, 94), dActionEntry (254, 1, 43, 3, 94), dActionEntry (288, 1, 43, 3, 94), 
			dActionEntry (93, 0, 323, 0, 0), dActionEntry (288, 0, 113, 0, 0), dActionEntry (59, 1, 20, 4, 107), dActionEntry (254, 1, 20, 4, 107), 
			dActionEntry (288, 1, 20, 4, 107), dActionEntry (283, 0, 325, 0, 0), dActionEntry (284, 0, 324, 0, 0), dActionEntry (40, 0, 331, 0, 0), 
			dActionEntry (258, 0, 328, 0, 0), dActionEntry (263, 0, 333, 0, 0), dActionEntry (40, 0, 331, 0, 0), dActionEntry (258, 0, 328, 0, 0), 
			dActionEntry (263, 0, 333, 0, 0), dActionEntry (275, 0, 334, 0, 0), dActionEntry (123, 0, 94, 0, 0), dActionEntry (125, 0, 337, 0, 0), 
			dActionEntry (254, 0, 78, 0, 0), dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 103, 0, 0), 
			dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), 
			dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), 
			dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 79, 0, 0), dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 101, 0, 0), 
			dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 102, 0, 0), 
			dActionEntry (286, 0, 93, 0, 0), dActionEntry (123, 1, 34, 5, 85), dActionEntry (125, 1, 34, 5, 85), dActionEntry (254, 1, 34, 5, 85), 
			dActionEntry (261, 1, 34, 5, 85), dActionEntry (262, 1, 34, 5, 85), dActionEntry (263, 1, 34, 5, 85), dActionEntry (266, 1, 34, 5, 85), 
			dActionEntry (267, 1, 34, 5, 85), dActionEntry (268, 1, 34, 5, 85), dActionEntry (269, 1, 34, 5, 85), dActionEntry (270, 1, 34, 5, 85), 
			dActionEntry (271, 1, 34, 5, 85), dActionEntry (272, 1, 34, 5, 85), dActionEntry (273, 1, 34, 5, 85), dActionEntry (274, 1, 34, 5, 85), 
			dActionEntry (276, 1, 34, 5, 85), dActionEntry (277, 1, 34, 5, 85), dActionEntry (278, 1, 34, 5, 85), dActionEntry (279, 1, 34, 5, 85), 
			dActionEntry (280, 1, 34, 5, 85), dActionEntry (281, 0, 338, 0, 0), dActionEntry (282, 1, 34, 5, 85), dActionEntry (285, 1, 34, 5, 85), 
			dActionEntry (286, 1, 34, 5, 85), dActionEntry (59, 1, 37, 5, 97), dActionEntry (91, 0, 340, 0, 0), dActionEntry (254, 1, 37, 5, 97), 
			dActionEntry (91, 1, 29, 2, 58), dActionEntry (91, 1, 29, 2, 61), dActionEntry (91, 1, 29, 2, 59), dActionEntry (91, 1, 29, 2, 60), 
			dActionEntry (123, 1, 34, 5, 89), dActionEntry (125, 1, 34, 5, 89), dActionEntry (254, 1, 34, 5, 89), dActionEntry (261, 1, 34, 5, 89), 
			dActionEntry (262, 1, 34, 5, 89), dActionEntry (263, 1, 34, 5, 89), dActionEntry (266, 1, 34, 5, 89), dActionEntry (267, 1, 34, 5, 89), 
			dActionEntry (268, 1, 34, 5, 89), dActionEntry (269, 1, 34, 5, 89), dActionEntry (270, 1, 34, 5, 89), dActionEntry (271, 1, 34, 5, 89), 
			dActionEntry (272, 1, 34, 5, 89), dActionEntry (273, 1, 34, 5, 89), dActionEntry (274, 1, 34, 5, 89), dActionEntry (276, 1, 34, 5, 89), 
			dActionEntry (277, 1, 34, 5, 89), dActionEntry (278, 1, 34, 5, 89), dActionEntry (279, 1, 34, 5, 89), dActionEntry (280, 1, 34, 5, 89), 
			dActionEntry (282, 1, 34, 5, 89), dActionEntry (285, 1, 34, 5, 89), dActionEntry (286, 1, 34, 5, 89), dActionEntry (61, 1, 43, 4, 95), 
			dActionEntry (91, 1, 43, 4, 95), dActionEntry (59, 1, 24, 6, 43), dActionEntry (254, 1, 24, 6, 43), dActionEntry (59, 1, 18, 1, 31), 
			dActionEntry (254, 1, 18, 1, 31), dActionEntry (41, 1, 43, 4, 95), dActionEntry (91, 1, 43, 4, 95), dActionEntry (288, 1, 43, 4, 95), 
			dActionEntry (41, 1, 43, 3, 94), dActionEntry (44, 1, 43, 3, 94), dActionEntry (91, 1, 43, 3, 94), dActionEntry (288, 1, 43, 3, 94), 
			dActionEntry (93, 0, 341, 0, 0), dActionEntry (288, 0, 113, 0, 0), dActionEntry (41, 1, 20, 4, 107), dActionEntry (44, 1, 20, 4, 107), 
			dActionEntry (288, 1, 20, 4, 107), dActionEntry (41, 0, 343, 0, 0), dActionEntry (288, 0, 168, 0, 0), dActionEntry (41, 1, 42, 2, 93), 
			dActionEntry (44, 1, 42, 2, 93), dActionEntry (91, 0, 345, 0, 0), dActionEntry (288, 1, 42, 2, 93), dActionEntry (40, 0, 179, 0, 0), 
			dActionEntry (41, 0, 346, 0, 0), dActionEntry (258, 0, 176, 0, 0), dActionEntry (263, 0, 182, 0, 0), dActionEntry (40, 0, 179, 0, 0), 
			dActionEntry (41, 0, 348, 0, 0), dActionEntry (258, 0, 176, 0, 0), dActionEntry (263, 0, 182, 0, 0), dActionEntry (59, 1, 43, 4, 95), 
			dActionEntry (91, 1, 43, 4, 95), dActionEntry (254, 1, 43, 4, 95), dActionEntry (288, 1, 43, 4, 95), dActionEntry (58, 0, 351, 0, 0), 
			dActionEntry (258, 0, 352, 0, 0), dActionEntry (125, 1, 41, 1, 100), dActionEntry (283, 1, 41, 1, 100), dActionEntry (284, 1, 41, 1, 100), 
			dActionEntry (125, 0, 353, 0, 0), dActionEntry (283, 0, 325, 0, 0), dActionEntry (284, 0, 324, 0, 0), dActionEntry (59, 1, 28, 1, 114), 
			dActionEntry (288, 1, 28, 1, 114), dActionEntry (59, 1, 28, 1, 113), dActionEntry (288, 1, 28, 1, 113), dActionEntry (59, 0, 356, 0, 0), 
			dActionEntry (288, 0, 355, 0, 0), dActionEntry (59, 1, 28, 1, 112), dActionEntry (288, 1, 28, 1, 112), dActionEntry (40, 0, 360, 0, 0), 
			dActionEntry (59, 1, 42, 1, 92), dActionEntry (91, 0, 358, 0, 0), dActionEntry (288, 1, 42, 1, 92), dActionEntry (59, 1, 37, 3, 96), 
			dActionEntry (288, 0, 355, 0, 0), dActionEntry (123, 0, 94, 0, 0), dActionEntry (125, 0, 362, 0, 0), dActionEntry (254, 0, 78, 0, 0), 
			dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 103, 0, 0), dActionEntry (266, 0, 33, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), 
			dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), 
			dActionEntry (276, 0, 79, 0, 0), dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 101, 0, 0), dActionEntry (279, 0, 77, 0, 0), 
			dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 102, 0, 0), dActionEntry (286, 0, 93, 0, 0), 
			dActionEntry (123, 1, 23, 2, 69), dActionEntry (125, 1, 23, 2, 69), dActionEntry (254, 1, 23, 2, 69), dActionEntry (261, 1, 23, 2, 69), 
			dActionEntry (262, 1, 23, 2, 69), dActionEntry (263, 1, 23, 2, 69), dActionEntry (266, 1, 23, 2, 69), dActionEntry (267, 1, 23, 2, 69), 
			dActionEntry (268, 1, 23, 2, 69), dActionEntry (269, 1, 23, 2, 69), dActionEntry (270, 1, 23, 2, 69), dActionEntry (271, 1, 23, 2, 69), 
			dActionEntry (272, 1, 23, 2, 69), dActionEntry (273, 1, 23, 2, 69), dActionEntry (274, 1, 23, 2, 69), dActionEntry (276, 1, 23, 2, 69), 
			dActionEntry (277, 1, 23, 2, 69), dActionEntry (278, 1, 23, 2, 69), dActionEntry (279, 1, 23, 2, 69), dActionEntry (280, 1, 23, 2, 69), 
			dActionEntry (281, 1, 23, 2, 69), dActionEntry (282, 1, 23, 2, 69), dActionEntry (285, 1, 23, 2, 69), dActionEntry (286, 1, 23, 2, 69), 
			dActionEntry (93, 0, 364, 0, 0), dActionEntry (288, 0, 113, 0, 0), dActionEntry (41, 1, 43, 4, 95), dActionEntry (44, 1, 43, 4, 95), 
			dActionEntry (91, 1, 43, 4, 95), dActionEntry (288, 1, 43, 4, 95), dActionEntry (93, 0, 366, 0, 0), dActionEntry (288, 0, 113, 0, 0), 
			dActionEntry (41, 0, 368, 0, 0), dActionEntry (44, 0, 234, 0, 0), dActionEntry (123, 1, 20, 3, 106), dActionEntry (41, 0, 369, 0, 0), 
			dActionEntry (44, 0, 234, 0, 0), dActionEntry (123, 1, 34, 7, 90), dActionEntry (125, 1, 34, 7, 90), dActionEntry (254, 1, 34, 7, 90), 
			dActionEntry (261, 1, 34, 7, 90), dActionEntry (262, 1, 34, 7, 90), dActionEntry (263, 1, 34, 7, 90), dActionEntry (266, 1, 34, 7, 90), 
			dActionEntry (267, 1, 34, 7, 90), dActionEntry (268, 1, 34, 7, 90), dActionEntry (269, 1, 34, 7, 90), dActionEntry (270, 1, 34, 7, 90), 
			dActionEntry (271, 1, 34, 7, 90), dActionEntry (272, 1, 34, 7, 90), dActionEntry (273, 1, 34, 7, 90), dActionEntry (274, 1, 34, 7, 90), 
			dActionEntry (276, 1, 34, 7, 90), dActionEntry (277, 1, 34, 7, 90), dActionEntry (278, 1, 34, 7, 90), dActionEntry (279, 1, 34, 7, 90), 
			dActionEntry (280, 1, 34, 7, 90), dActionEntry (282, 1, 34, 7, 90), dActionEntry (285, 1, 34, 7, 90), dActionEntry (286, 1, 34, 7, 90), 
			dActionEntry (123, 0, 370, 0, 0), dActionEntry (58, 0, 372, 0, 0), dActionEntry (123, 1, 34, 7, 87), dActionEntry (125, 1, 34, 7, 87), 
			dActionEntry (254, 1, 34, 7, 87), dActionEntry (261, 1, 34, 7, 87), dActionEntry (262, 1, 34, 7, 87), dActionEntry (263, 1, 34, 7, 87), 
			dActionEntry (266, 1, 34, 7, 87), dActionEntry (267, 1, 34, 7, 87), dActionEntry (268, 1, 34, 7, 87), dActionEntry (269, 1, 34, 7, 87), 
			dActionEntry (270, 1, 34, 7, 87), dActionEntry (271, 1, 34, 7, 87), dActionEntry (272, 1, 34, 7, 87), dActionEntry (273, 1, 34, 7, 87), 
			dActionEntry (274, 1, 34, 7, 87), dActionEntry (276, 1, 34, 7, 87), dActionEntry (277, 1, 34, 7, 87), dActionEntry (278, 1, 34, 7, 87), 
			dActionEntry (279, 1, 34, 7, 87), dActionEntry (280, 1, 34, 7, 87), dActionEntry (282, 1, 34, 7, 87), dActionEntry (285, 1, 34, 7, 87), 
			dActionEntry (286, 1, 34, 7, 87), dActionEntry (125, 1, 41, 2, 101), dActionEntry (283, 1, 41, 2, 101), dActionEntry (284, 1, 41, 2, 101), 
			dActionEntry (41, 0, 376, 0, 0), dActionEntry (288, 0, 168, 0, 0), dActionEntry (59, 1, 42, 2, 93), dActionEntry (91, 0, 378, 0, 0), 
			dActionEntry (288, 1, 42, 2, 93), dActionEntry (40, 0, 179, 0, 0), dActionEntry (41, 0, 379, 0, 0), dActionEntry (258, 0, 176, 0, 0), 
			dActionEntry (263, 0, 182, 0, 0), dActionEntry (91, 0, 381, 0, 0), dActionEntry (123, 1, 23, 3, 70), dActionEntry (125, 1, 23, 3, 70), 
			dActionEntry (254, 1, 23, 3, 70), dActionEntry (261, 1, 23, 3, 70), dActionEntry (262, 1, 23, 3, 70), dActionEntry (263, 1, 23, 3, 70), 
			dActionEntry (266, 1, 23, 3, 70), dActionEntry (267, 1, 23, 3, 70), dActionEntry (268, 1, 23, 3, 70), dActionEntry (269, 1, 23, 3, 70), 
			dActionEntry (270, 1, 23, 3, 70), dActionEntry (271, 1, 23, 3, 70), dActionEntry (272, 1, 23, 3, 70), dActionEntry (273, 1, 23, 3, 70), 
			dActionEntry (274, 1, 23, 3, 70), dActionEntry (276, 1, 23, 3, 70), dActionEntry (277, 1, 23, 3, 70), dActionEntry (278, 1, 23, 3, 70), 
			dActionEntry (279, 1, 23, 3, 70), dActionEntry (280, 1, 23, 3, 70), dActionEntry (281, 1, 23, 3, 70), dActionEntry (282, 1, 23, 3, 70), 
			dActionEntry (285, 1, 23, 3, 70), dActionEntry (286, 1, 23, 3, 70), dActionEntry (123, 1, 34, 7, 86), dActionEntry (125, 1, 34, 7, 86), 
			dActionEntry (254, 1, 34, 7, 86), dActionEntry (261, 1, 34, 7, 86), dActionEntry (262, 1, 34, 7, 86), dActionEntry (263, 1, 34, 7, 86), 
			dActionEntry (266, 1, 34, 7, 86), dActionEntry (267, 1, 34, 7, 86), dActionEntry (268, 1, 34, 7, 86), dActionEntry (269, 1, 34, 7, 86), 
			dActionEntry (270, 1, 34, 7, 86), dActionEntry (271, 1, 34, 7, 86), dActionEntry (272, 1, 34, 7, 86), dActionEntry (273, 1, 34, 7, 86), 
			dActionEntry (274, 1, 34, 7, 86), dActionEntry (276, 1, 34, 7, 86), dActionEntry (277, 1, 34, 7, 86), dActionEntry (278, 1, 34, 7, 86), 
			dActionEntry (279, 1, 34, 7, 86), dActionEntry (280, 1, 34, 7, 86), dActionEntry (282, 1, 34, 7, 86), dActionEntry (285, 1, 34, 7, 86), 
			dActionEntry (286, 1, 34, 7, 86), dActionEntry (59, 1, 43, 3, 94), dActionEntry (91, 1, 43, 3, 94), dActionEntry (254, 1, 43, 3, 94), 
			dActionEntry (93, 0, 383, 0, 0), dActionEntry (288, 0, 113, 0, 0), dActionEntry (93, 0, 384, 0, 0), dActionEntry (288, 0, 113, 0, 0), 
			dActionEntry (123, 1, 20, 4, 107), dActionEntry (123, 0, 94, 0, 0), dActionEntry (125, 0, 386, 0, 0), dActionEntry (254, 0, 78, 0, 0), 
			dActionEntry (261, 0, 86, 0, 0), dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 103, 0, 0), dActionEntry (266, 0, 33, 0, 0), 
			dActionEntry (267, 0, 20, 0, 0), dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), 
			dActionEntry (271, 0, 31, 0, 0), dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), 
			dActionEntry (276, 0, 79, 0, 0), dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 101, 0, 0), dActionEntry (279, 0, 77, 0, 0), 
			dActionEntry (280, 0, 92, 0, 0), dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 102, 0, 0), dActionEntry (286, 0, 93, 0, 0), 
			dActionEntry (125, 1, 44, 3, 103), dActionEntry (283, 1, 44, 3, 103), dActionEntry (284, 1, 44, 3, 103), dActionEntry (59, 1, 28, 3, 110), 
			dActionEntry (288, 1, 28, 3, 110), dActionEntry (41, 0, 388, 0, 0), dActionEntry (61, 0, 389, 0, 0), dActionEntry (59, 1, 28, 3, 111), 
			dActionEntry (288, 1, 28, 3, 111), dActionEntry (93, 0, 390, 0, 0), dActionEntry (288, 0, 113, 0, 0), dActionEntry (59, 1, 20, 3, 106), 
			dActionEntry (288, 1, 20, 3, 106), dActionEntry (41, 0, 392, 0, 0), dActionEntry (44, 0, 234, 0, 0), dActionEntry (59, 1, 37, 5, 97), 
			dActionEntry (91, 0, 394, 0, 0), dActionEntry (59, 1, 43, 4, 95), dActionEntry (91, 1, 43, 4, 95), dActionEntry (254, 1, 43, 4, 95), 
			dActionEntry (123, 0, 94, 0, 0), dActionEntry (125, 0, 395, 0, 0), dActionEntry (254, 0, 78, 0, 0), dActionEntry (261, 0, 86, 0, 0), 
			dActionEntry (262, 0, 39, 0, 0), dActionEntry (263, 0, 103, 0, 0), dActionEntry (266, 0, 33, 0, 0), dActionEntry (267, 0, 20, 0, 0), 
			dActionEntry (268, 0, 29, 0, 0), dActionEntry (269, 0, 35, 0, 0), dActionEntry (270, 0, 21, 0, 0), dActionEntry (271, 0, 31, 0, 0), 
			dActionEntry (272, 0, 24, 0, 0), dActionEntry (273, 0, 38, 0, 0), dActionEntry (274, 0, 37, 0, 0), dActionEntry (276, 0, 79, 0, 0), 
			dActionEntry (277, 0, 90, 0, 0), dActionEntry (278, 0, 101, 0, 0), dActionEntry (279, 0, 77, 0, 0), dActionEntry (280, 0, 92, 0, 0), 
			dActionEntry (282, 0, 89, 0, 0), dActionEntry (285, 0, 102, 0, 0), dActionEntry (286, 0, 93, 0, 0), dActionEntry (125, 1, 23, 2, 69), 
			dActionEntry (283, 1, 23, 2, 69), dActionEntry (284, 1, 23, 2, 69), dActionEntry (125, 1, 44, 4, 102), dActionEntry (283, 1, 44, 4, 102), 
			dActionEntry (284, 1, 44, 4, 102), dActionEntry (40, 0, 118, 0, 0), dActionEntry (258, 0, 115, 0, 0), dActionEntry (263, 0, 120, 0, 0), 
			dActionEntry (275, 0, 397, 0, 0), dActionEntry (59, 1, 43, 3, 94), dActionEntry (91, 1, 43, 3, 94), dActionEntry (288, 1, 43, 3, 94), 
			dActionEntry (93, 0, 399, 0, 0), dActionEntry (288, 0, 113, 0, 0), dActionEntry (59, 1, 20, 4, 107), dActionEntry (288, 1, 20, 4, 107), 
			dActionEntry (93, 0, 400, 0, 0), dActionEntry (288, 0, 113, 0, 0), dActionEntry (125, 1, 23, 3, 70), dActionEntry (283, 1, 23, 3, 70), 
			dActionEntry (284, 1, 23, 3, 70), dActionEntry (123, 1, 34, 10, 88), dActionEntry (125, 1, 34, 10, 88), dActionEntry (254, 1, 34, 10, 88), 
			dActionEntry (261, 1, 34, 10, 88), dActionEntry (262, 1, 34, 10, 88), dActionEntry (263, 1, 34, 10, 88), dActionEntry (266, 1, 34, 10, 88), 
			dActionEntry (267, 1, 34, 10, 88), dActionEntry (268, 1, 34, 10, 88), dActionEntry (269, 1, 34, 10, 88), dActionEntry (270, 1, 34, 10, 88), 
			dActionEntry (271, 1, 34, 10, 88), dActionEntry (272, 1, 34, 10, 88), dActionEntry (273, 1, 34, 10, 88), dActionEntry (274, 1, 34, 10, 88), 
			dActionEntry (276, 1, 34, 10, 88), dActionEntry (277, 1, 34, 10, 88), dActionEntry (278, 1, 34, 10, 88), dActionEntry (279, 1, 34, 10, 88), 
			dActionEntry (280, 1, 34, 10, 88), dActionEntry (282, 1, 34, 10, 88), dActionEntry (285, 1, 34, 10, 88), dActionEntry (286, 1, 34, 10, 88), 
			dActionEntry (41, 1, 37, 3, 96), dActionEntry (288, 0, 168, 0, 0), dActionEntry (59, 1, 43, 4, 95), dActionEntry (91, 1, 43, 4, 95), 
			dActionEntry (288, 1, 43, 4, 95), dActionEntry (59, 1, 43, 3, 94), dActionEntry (91, 1, 43, 3, 94), dActionEntry (93, 0, 403, 0, 0), 
			dActionEntry (288, 0, 113, 0, 0), dActionEntry (91, 0, 404, 0, 0), dActionEntry (59, 1, 43, 4, 95), dActionEntry (91, 1, 43, 4, 95), 
			dActionEntry (41, 1, 37, 5, 97), dActionEntry (91, 0, 407, 0, 0), dActionEntry (93, 0, 408, 0, 0), dActionEntry (288, 0, 113, 0, 0), 
			dActionEntry (41, 1, 43, 3, 94), dActionEntry (91, 1, 43, 3, 94), dActionEntry (93, 0, 410, 0, 0), dActionEntry (288, 0, 113, 0, 0), 
			dActionEntry (41, 1, 43, 4, 95), dActionEntry (91, 1, 43, 4, 95)};

	bool errorMode = false;
	const dStackPair& stackTop = stack.GetLast()->GetInfo();
	int state = stackTop.m_state;
	int start = actionsStart[state];
	int count = actionsCount[state];

	const dActionEntry* const table = &actionTable[start];
	const dActionEntry* action = FindAction (table, count, token);
	while (!action && stack.GetCount()) {
		errorMode = true; 
		// we found a syntax error in go into error recovering mode
		stack.Remove (stack.GetLast());

		const dStackPair& stackTop = stack.GetLast()->GetInfo();
		int state = stackTop.m_state;
		int start = actionsStart[state];
		int count = actionsCount[state];
		const dActionEntry* const table = &actionTable[start];
		action = FindAction (table, count, ERROR_TOKEN);
	}

	if (errorMode) {
		if (action) {
			dStackPair& stackTop = stack.GetLast()->GetInfo();
			stackTop.m_token = ERROR_TOKEN;

			int state = action->m_nextState;
			int start = actionsStart[state];
			int count = actionsCount[state];
			const dActionEntry* const table = &actionTable[start];
			//int scannerIndex = scanner.GetIndex();
			while (!FindAction (table, count, token)) {
				//scannerIndex = scanner.GetIndex();
				token = dToken (scanner.NextToken());
			}
			action = FindAction (table, count, token);
			//scanner.SetIndex (scannerIndex);
			dStackPair& entry = stack.Append()->GetInfo();
			entry.m_state = state;
			entry.m_value = dUserVariable (ERROR_TOKEN, "error");
			entry.m_token = token;

		} else {
			_ASSERTE (0);
		}

	}

	return action;
}


bool dNewtonScriptParser::Parse(dNewtonScriptLexical& scanner)
{
	static short gotoCount[] = {
			6, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 12, 0, 0, 2, 0, 0, 0, 10, 3, 1, 0, 0, 0, 
			0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 0, 0, 3, 0, 1, 0, 0, 16, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 
			0, 0, 0, 0, 0, 0, 3, 0, 1, 0, 5, 0, 0, 1, 0, 3, 0, 15, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 16, 2, 
			2, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 5, 0, 1, 5, 3, 0, 0, 0, 0, 3, 0, 1, 3, 0, 4, 0, 0, 0, 0, 
			16, 0, 0, 0, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 1, 0, 3, 15, 0, 0, 0, 0, 3, 3, 3, 0, 
			4, 0, 0, 0, 0, 5, 0, 0, 3, 0, 0, 3, 0, 4, 0, 3, 0, 0, 0, 3, 0, 0, 1, 0, 3, 1, 0, 15, 0, 0, 3, 0, 
			3, 0, 4, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 2, 0, 1, 0, 0, 0, 
			3, 0, 0, 0, 0, 3, 0, 3, 0, 4, 3, 0, 0, 1, 1, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 
			0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 
			0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 3, 3, 16, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 
			4, 4, 2, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 1, 1, 0, 15, 0, 1, 0, 3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1, 
			0, 0, 0, 3, 2, 0, 3, 0, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 1, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 
			0, 15, 0, 0, 1, 3, 0, 0, 0, 0, 3, 0, 0, 1, 0, 0, 0, 0, 1, 0, 3, 0, 0, 3, 0, 0, 0};
	static short gotoStart[] = {
			0, 6, 6, 6, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 12, 12, 12, 12, 24, 24, 24, 26, 26, 26, 26, 36, 39, 40, 40, 40, 
			40, 40, 40, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 46, 47, 47, 47, 50, 50, 51, 51, 51, 67, 67, 67, 67, 67, 67, 69, 69, 69, 71, 
			71, 71, 71, 71, 71, 71, 71, 74, 74, 75, 75, 80, 80, 80, 81, 81, 84, 84, 99, 99, 101, 101, 101, 103, 105, 105, 105, 105, 107, 107, 107, 123, 
			125, 127, 129, 129, 129, 129, 129, 129, 130, 130, 130, 130, 130, 130, 135, 135, 136, 141, 144, 144, 144, 144, 144, 147, 147, 148, 151, 151, 155, 155, 155, 155, 
			155, 171, 171, 171, 171, 171, 171, 171, 174, 174, 175, 175, 175, 175, 175, 175, 175, 175, 175, 178, 181, 182, 182, 185, 200, 200, 200, 200, 200, 203, 206, 209, 
			209, 213, 213, 213, 213, 213, 218, 218, 218, 221, 221, 221, 224, 224, 228, 228, 231, 231, 231, 231, 234, 234, 234, 235, 235, 238, 239, 239, 254, 254, 254, 257, 
			257, 260, 260, 264, 264, 264, 264, 264, 264, 264, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 267, 267, 267, 267, 270, 270, 270, 272, 272, 273, 273, 273, 
			273, 276, 276, 276, 276, 276, 279, 279, 282, 282, 286, 289, 289, 289, 290, 291, 291, 294, 294, 294, 294, 297, 297, 297, 297, 297, 297, 298, 298, 298, 298, 298, 
			299, 299, 299, 300, 300, 300, 300, 300, 300, 300, 300, 301, 301, 301, 301, 301, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 305, 305, 305, 305, 305, 305, 
			308, 308, 309, 309, 309, 309, 309, 309, 309, 309, 311, 314, 317, 333, 333, 336, 336, 336, 336, 336, 336, 336, 336, 336, 336, 336, 336, 336, 336, 339, 339, 342, 
			342, 346, 350, 352, 352, 352, 352, 352, 353, 353, 353, 353, 356, 356, 357, 358, 358, 373, 373, 374, 374, 377, 377, 377, 377, 377, 380, 380, 380, 380, 380, 380, 
			381, 381, 381, 381, 384, 386, 386, 389, 389, 393, 394, 394, 394, 394, 394, 394, 394, 394, 394, 410, 410, 411, 411, 411, 411, 411, 411, 414, 414, 414, 417, 417, 
			417, 417, 432, 432, 432, 433, 436, 436, 436, 436, 436, 439, 439, 439, 440, 440, 440, 440, 440, 441, 441, 444, 444, 444, 447, 447, 447};
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
			dGotoEntry (318, 75), dGotoEntry (315, 83), dGotoEntry (318, 98), dGotoEntry (320, 104), dGotoEntry (321, 88), 
			dGotoEntry (322, 80), dGotoEntry (324, 22), dGotoEntry (327, 81), dGotoEntry (328, 85), dGotoEntry (329, 82), 
			dGotoEntry (330, 91), dGotoEntry (331, 99), dGotoEntry (332, 97), dGotoEntry (333, 96), dGotoEntry (334, 95), 
			dGotoEntry (335, 87), dGotoEntry (337, 100), dGotoEntry (316, 107), dGotoEntry (317, 106), dGotoEntry (325, 111), 
			dGotoEntry (326, 46), dGotoEntry (315, 116), dGotoEntry (323, 117), dGotoEntry (337, 119), dGotoEntry (338, 122), 
			dGotoEntry (310, 124), dGotoEntry (311, 125), dGotoEntry (312, 126), dGotoEntry (320, 127), dGotoEntry (324, 22), 
			dGotoEntry (318, 129), dGotoEntry (315, 133), dGotoEntry (323, 134), dGotoEntry (337, 136), dGotoEntry (315, 83), 
			dGotoEntry (318, 98), dGotoEntry (320, 104), dGotoEntry (321, 88), dGotoEntry (322, 80), dGotoEntry (324, 22), 
			dGotoEntry (328, 139), dGotoEntry (329, 82), dGotoEntry (330, 91), dGotoEntry (331, 99), dGotoEntry (332, 97), 
			dGotoEntry (333, 96), dGotoEntry (334, 95), dGotoEntry (335, 87), dGotoEntry (337, 100), dGotoEntry (316, 142), 
			dGotoEntry (317, 141), dGotoEntry (320, 144), dGotoEntry (324, 22), dGotoEntry (316, 145), dGotoEntry (317, 141), 
			dGotoEntry (316, 149), dGotoEntry (317, 141), dGotoEntry (315, 83), dGotoEntry (318, 98), dGotoEntry (320, 104), 
			dGotoEntry (321, 88), dGotoEntry (322, 80), dGotoEntry (324, 22), dGotoEntry (327, 151), dGotoEntry (328, 85), 
			dGotoEntry (329, 82), dGotoEntry (330, 91), dGotoEntry (331, 99), dGotoEntry (332, 97), dGotoEntry (333, 96), 
			dGotoEntry (334, 95), dGotoEntry (335, 87), dGotoEntry (337, 100), dGotoEntry (316, 153), dGotoEntry (317, 141), 
			dGotoEntry (316, 154), dGotoEntry (317, 141), dGotoEntry (316, 155), dGotoEntry (317, 141), dGotoEntry (338, 159), 
			dGotoEntry (310, 124), dGotoEntry (311, 125), dGotoEntry (312, 164), dGotoEntry (320, 127), dGotoEntry (324, 22), 
			dGotoEntry (326, 73), dGotoEntry (310, 124), dGotoEntry (311, 125), dGotoEntry (312, 166), dGotoEntry (320, 127), 
			dGotoEntry (324, 22), dGotoEntry (315, 67), dGotoEntry (323, 167), dGotoEntry (337, 71), dGotoEntry (315, 116), 
			dGotoEntry (323, 170), dGotoEntry (337, 119), dGotoEntry (338, 172), dGotoEntry (315, 67), dGotoEntry (323, 174), 
			dGotoEntry (337, 71), dGotoEntry (315, 177), dGotoEntry (323, 178), dGotoEntry (337, 181), dGotoEntry (340, 183), 
			dGotoEntry (315, 83), dGotoEntry (318, 98), dGotoEntry (320, 104), dGotoEntry (321, 88), dGotoEntry (322, 80), 
			dGotoEntry (324, 22), dGotoEntry (327, 187), dGotoEntry (328, 85), dGotoEntry (329, 82), dGotoEntry (330, 91), 
			dGotoEntry (331, 99), dGotoEntry (332, 97), dGotoEntry (333, 96), dGotoEntry (334, 95), dGotoEntry (335, 87), 
			dGotoEntry (337, 100), dGotoEntry (315, 116), dGotoEntry (323, 191), dGotoEntry (337, 119), dGotoEntry (338, 193), 
			dGotoEntry (315, 133), dGotoEntry (323, 197), dGotoEntry (337, 136), dGotoEntry (315, 116), dGotoEntry (323, 198), 
			dGotoEntry (337, 119), dGotoEntry (324, 201), dGotoEntry (315, 116), dGotoEntry (323, 209), dGotoEntry (337, 119), 
			dGotoEntry (315, 83), dGotoEntry (318, 98), dGotoEntry (320, 104), dGotoEntry (321, 88), dGotoEntry (322, 80), 
			dGotoEntry (324, 22), dGotoEntry (328, 139), dGotoEntry (329, 82), dGotoEntry (330, 91), dGotoEntry (331, 99), 
			dGotoEntry (332, 97), dGotoEntry (333, 96), dGotoEntry (334, 95), dGotoEntry (335, 87), dGotoEntry (337, 100), 
			dGotoEntry (315, 133), dGotoEntry (323, 212), dGotoEntry (337, 136), dGotoEntry (315, 116), dGotoEntry (323, 213), 
			dGotoEntry (337, 119), dGotoEntry (315, 67), dGotoEntry (323, 214), dGotoEntry (337, 71), dGotoEntry (315, 177), 
			dGotoEntry (323, 178), dGotoEntry (337, 181), dGotoEntry (340, 217), dGotoEntry (310, 124), dGotoEntry (311, 125), 
			dGotoEntry (312, 219), dGotoEntry (320, 127), dGotoEntry (324, 22), dGotoEntry (315, 116), dGotoEntry (323, 221), 
			dGotoEntry (337, 119), dGotoEntry (315, 67), dGotoEntry (323, 223), dGotoEntry (337, 71), dGotoEntry (315, 177), 
			dGotoEntry (323, 178), dGotoEntry (337, 181), dGotoEntry (340, 226), dGotoEntry (315, 67), dGotoEntry (323, 228), 
			dGotoEntry (337, 71), dGotoEntry (315, 116), dGotoEntry (323, 230), dGotoEntry (337, 119), dGotoEntry (338, 232), 
			dGotoEntry (311, 236), dGotoEntry (320, 127), dGotoEntry (324, 22), dGotoEntry (314, 237), dGotoEntry (315, 83), 
			dGotoEntry (318, 98), dGotoEntry (320, 104), dGotoEntry (321, 88), dGotoEntry (322, 80), dGotoEntry (324, 22), 
			dGotoEntry (328, 139), dGotoEntry (329, 82), dGotoEntry (330, 91), dGotoEntry (331, 99), dGotoEntry (332, 97), 
			dGotoEntry (333, 96), dGotoEntry (334, 95), dGotoEntry (335, 87), dGotoEntry (337, 100), dGotoEntry (315, 133), 
			dGotoEntry (323, 241), dGotoEntry (337, 136), dGotoEntry (315, 67), dGotoEntry (323, 243), dGotoEntry (337, 71), 
			dGotoEntry (315, 177), dGotoEntry (323, 178), dGotoEntry (337, 181), dGotoEntry (340, 246), dGotoEntry (332, 248), 
			dGotoEntry (337, 249), dGotoEntry (324, 258), dGotoEntry (315, 67), dGotoEntry (323, 268), dGotoEntry (337, 71), 
			dGotoEntry (316, 270), dGotoEntry (317, 106), dGotoEntry (313, 272), dGotoEntry (315, 67), dGotoEntry (323, 275), 
			dGotoEntry (337, 71), dGotoEntry (315, 177), dGotoEntry (323, 278), dGotoEntry (337, 181), dGotoEntry (315, 67), 
			dGotoEntry (323, 280), dGotoEntry (337, 71), dGotoEntry (315, 177), dGotoEntry (323, 178), dGotoEntry (337, 181), 
			dGotoEntry (340, 283), dGotoEntry (315, 285), dGotoEntry (323, 286), dGotoEntry (337, 288), dGotoEntry (318, 290), 
			dGotoEntry (315, 291), dGotoEntry (315, 116), dGotoEntry (323, 293), dGotoEntry (337, 119), dGotoEntry (315, 67), 
			dGotoEntry (323, 295), dGotoEntry (337, 71), dGotoEntry (338, 159), dGotoEntry (318, 301), dGotoEntry (338, 303), 
			dGotoEntry (318, 308), dGotoEntry (313, 310), dGotoEntry (315, 67), dGotoEntry (323, 314), dGotoEntry (337, 71), 
			dGotoEntry (315, 116), dGotoEntry (323, 317), dGotoEntry (337, 119), dGotoEntry (338, 319), dGotoEntry (336, 327), 
			dGotoEntry (339, 326), dGotoEntry (315, 329), dGotoEntry (323, 330), dGotoEntry (337, 332), dGotoEntry (315, 329), 
			dGotoEntry (323, 335), dGotoEntry (337, 332), dGotoEntry (315, 83), dGotoEntry (318, 98), dGotoEntry (320, 104), 
			dGotoEntry (321, 88), dGotoEntry (322, 80), dGotoEntry (324, 22), dGotoEntry (327, 336), dGotoEntry (328, 85), 
			dGotoEntry (329, 82), dGotoEntry (330, 91), dGotoEntry (331, 99), dGotoEntry (332, 97), dGotoEntry (333, 96), 
			dGotoEntry (334, 95), dGotoEntry (335, 87), dGotoEntry (337, 100), dGotoEntry (315, 67), dGotoEntry (323, 339), 
			dGotoEntry (337, 71), dGotoEntry (315, 285), dGotoEntry (323, 342), dGotoEntry (337, 288), dGotoEntry (315, 67), 
			dGotoEntry (323, 344), dGotoEntry (337, 71), dGotoEntry (315, 177), dGotoEntry (323, 178), dGotoEntry (337, 181), 
			dGotoEntry (340, 347), dGotoEntry (315, 177), dGotoEntry (323, 178), dGotoEntry (337, 181), dGotoEntry (340, 349), 
			dGotoEntry (316, 350), dGotoEntry (317, 141), dGotoEntry (339, 354), dGotoEntry (315, 116), dGotoEntry (323, 357), 
			dGotoEntry (337, 119), dGotoEntry (338, 359), dGotoEntry (324, 361), dGotoEntry (315, 83), dGotoEntry (318, 98), 
			dGotoEntry (320, 104), dGotoEntry (321, 88), dGotoEntry (322, 80), dGotoEntry (324, 22), dGotoEntry (328, 139), 
			dGotoEntry (329, 82), dGotoEntry (330, 91), dGotoEntry (331, 99), dGotoEntry (332, 97), dGotoEntry (333, 96), 
			dGotoEntry (334, 95), dGotoEntry (335, 87), dGotoEntry (337, 100), dGotoEntry (318, 363), dGotoEntry (315, 67), 
			dGotoEntry (323, 365), dGotoEntry (337, 71), dGotoEntry (315, 67), dGotoEntry (323, 367), dGotoEntry (337, 71), 
			dGotoEntry (318, 371), dGotoEntry (315, 329), dGotoEntry (323, 373), dGotoEntry (337, 332), dGotoEntry (332, 374), 
			dGotoEntry (337, 375), dGotoEntry (315, 67), dGotoEntry (323, 377), dGotoEntry (337, 71), dGotoEntry (315, 177), 
			dGotoEntry (323, 178), dGotoEntry (337, 181), dGotoEntry (340, 380), dGotoEntry (338, 382), dGotoEntry (315, 83), 
			dGotoEntry (318, 98), dGotoEntry (320, 104), dGotoEntry (321, 88), dGotoEntry (322, 80), dGotoEntry (324, 22), 
			dGotoEntry (327, 385), dGotoEntry (328, 85), dGotoEntry (329, 82), dGotoEntry (330, 91), dGotoEntry (331, 99), 
			dGotoEntry (332, 97), dGotoEntry (333, 96), dGotoEntry (334, 95), dGotoEntry (335, 87), dGotoEntry (337, 100), 
			dGotoEntry (318, 387), dGotoEntry (315, 67), dGotoEntry (323, 391), dGotoEntry (337, 71), dGotoEntry (315, 67), 
			dGotoEntry (323, 393), dGotoEntry (337, 71), dGotoEntry (315, 83), dGotoEntry (318, 98), dGotoEntry (320, 104), 
			dGotoEntry (321, 88), dGotoEntry (322, 80), dGotoEntry (324, 22), dGotoEntry (328, 139), dGotoEntry (329, 82), 
			dGotoEntry (330, 91), dGotoEntry (331, 99), dGotoEntry (332, 97), dGotoEntry (333, 96), dGotoEntry (334, 95), 
			dGotoEntry (335, 87), dGotoEntry (337, 100), dGotoEntry (318, 396), dGotoEntry (315, 116), dGotoEntry (323, 398), 
			dGotoEntry (337, 119), dGotoEntry (315, 67), dGotoEntry (323, 401), dGotoEntry (337, 71), dGotoEntry (324, 402), 
			dGotoEntry (338, 405), dGotoEntry (315, 67), dGotoEntry (323, 406), dGotoEntry (337, 71), dGotoEntry (315, 67), 
			dGotoEntry (323, 409), dGotoEntry (337, 71)};

	dList<dStackPair> stack;
	const int lastToken = 295;
	
	stack.Append ();
	dToken token = dToken (scanner.NextToken());
	for (;;) {

		const dActionEntry* const action = GetNextAction (stack, token, scanner);
		switch (action->m_stateType) 
		{
			case dSHIFT: 
			{
				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_state = action->m_nextState;
				entry.m_value = dUserVariable (token, scanner.GetTokenString());
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
				const dGotoEntry* const table = &gotoTable[start];
				const dGotoEntry* const gotoEntry = FindGoto (table, count, dToken (action->m_nextState + lastToken));

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



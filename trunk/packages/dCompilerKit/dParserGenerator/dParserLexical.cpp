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
//Auto generated Lexical Analyzer class: dParserLexical.cpp
//

#include <dParserCompiler.h>

#include "dParserLexical.h"


dParserLexical::dParserLexical(const char* const data)
	:m_tokenString ("")
	,m_data(data)
	,m_index(0)
	,m_startIndex(0)
	,m_lineNumber(0)
{
}

dParserLexical::~dParserLexical()
{
}


void dParserLexical::ReadBalancedExpresion (char open, char close)
{
	int count = 1;
	while (count) {
		int ch = NextChar();
		if (ch == '\n') {
			m_lineNumber ++;
		}

		if(ch == open) {
			count ++;
		} else if (ch == close) {
			count --;
		} else {
			if (ch == '\'')	{
				ch = NextChar();
				if (ch == '\\') {
					ch = NextChar();
				}
				ch = NextChar();
			} else if (ch == '\"') {
				for (ch = NextChar(); ch != '\"'; ch = NextChar()) {
					if (ch == '\\') {
						ch = NextChar();
					}
				}
			}
		}
	}

	string tmp (m_tokenString);
	GetLexString();
	m_tokenString = tmp + m_tokenString;
}


void dParserLexical::GetLexString ()
{
	int length = m_index - m_startIndex;
	m_tokenString = string (&m_data[m_startIndex], length);
	m_startIndex = m_index;
}


int dParserLexical::GetNextStateIndex (char symbol, int count, const char* const characterSet) const
{
	int i0 = 0;
	int i1 = count - 1;
	while ((i1 - i0) >= 4) {
		int i = (i1 + i0 + 1)>>1;
		if (symbol <= characterSet[i]) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		if (symbol == characterSet[i]) {
			return i;
		}
	}
	return -1;
}

int dParserLexical::NextToken ()
{
	static short transitionsCount[] = {
			64, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 0, 0, 63, 63, 0, 0, 2, 127, 127, 
			127, 127, 77, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 1, 1, 1, 8, 127, 127, 127, 127, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 1, 1, 1, 1, 1, 1};
	static short transitionsStart[] = {
			0, 64, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 
			68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 194, 194, 68, 68, 257, 259, 386, 
			259, 259, 513, 590, 591, 592, 593, 594, 595, 596, 597, 598, 599, 600, 601, 602, 603, 604, 605, 606, 607, 608, 609, 610, 
			611, 612, 613, 614, 615, 616, 624, 751, 624, 624, 878, 879, 880, 881, 882, 883, 884, 885, 886, 887, 888, 889, 890, 891, 
			892, 893, 894, 895, 896, 897, 898, 899};
	static short nextStateSet[] = {
			1, 1, 1, 1, 77, 50, 45, 39, 40, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 
			42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 
			42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 43, 44, 1, 1, 1, 1, 37, 37, 37, 37, 
			37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
			37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
			37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
			37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
			37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
			37, 37, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 
			41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 
			41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 46, 38, 49, 49, 49, 49, 49, 
			49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 
			49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 47, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 
			49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 
			49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 
			49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 
			49, 49, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 
			48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 
			36, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 
			48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 
			48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 
			48, 48, 48, 48, 48, 48, 48, 48, 48, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 
			61, 60, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 
			59, 59, 59, 59, 58, 57, 56, 55, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 
			59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 54, 53, 52, 51, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 
			25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 2, 101, 98, 94, 90, 86, 82, 78, 
			81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 
			81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 79, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 
			81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 
			81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 
			81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 
			81, 81, 81, 81, 81, 81, 81, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 
			80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 
			80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 
			80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 
			80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 
			80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 9, 80, 80, 83, 84, 85, 8, 87, 88, 89, 7, 91, 92, 
			93, 6, 95, 96, 97, 5, 99, 100, 4, 102, 103, 3};
	static char  nextCharacterSet[] = {
			9, 10, 13, 32, 37, 39, 47, 58, 59, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 
			80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 
			109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 9, 10, 13, 32, 1, 2, 3, 4, 
			5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
			30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 
			54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 
			78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 
			102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 
			126, 127, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 
			77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 
			106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 42, 47, 1, 2, 3, 4, 5, 
			6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
			30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 
			54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 
			78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 
			102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 
			126, 127, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 
			23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 
			47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 
			71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 
			95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 
			119, 120, 121, 122, 123, 124, 125, 126, 127, 33, 37, 38, 40, 41, 42, 43, 44, 45, 46, 47, 58, 59, 60, 61, 
			62, 63, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 
			87, 88, 89, 90, 91, 92, 93, 94, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 
			113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 
			39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 37, 108, 112, 114, 115, 116, 117, 123, 
			1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 
			25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 
			49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 
			73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 
			97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 
			121, 122, 123, 124, 125, 126, 127, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 
			18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 
			42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 
			66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 
			90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 
			114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 110, 105, 111, 110, 111, 107, 101, 110, 116, 97, 
			114, 116, 105, 103, 104, 116, 114, 101, 99, 101, 102, 116};
	
	m_startIndex = m_index;

	int state = 0;
	int zeroCount = 2;
	char ch = NextChar();
	do {
		int transCount = transitionsCount[state];
		int tranStart = transitionsStart[state];
		int nextStateIndex = GetNextStateIndex (ch, transCount, &nextCharacterSet[tranStart]);
		if (nextStateIndex >= 0) {
			ch = NextChar();
			short* const stateArray = &nextStateSet[tranStart];
			state = stateArray[nextStateIndex];
		} else {
			UnGetChar ();
			switch (state) 
			{
				case 1:
				{
					GetLexString ();
					{}
					state = 0;
					ch = NextChar();
					break;
				}
				case 2:
				{
					GetLexString ();
					{ return dParserCompiler::GRAMMAR_SEGMENT;}
					state = 0;
					ch = NextChar();
					break;
				}
				case 3:
				{
					GetLexString ();
					{ return dParserCompiler::LEFT;}
					state = 0;
					ch = NextChar();
					break;
				}
				case 4:
				{
					GetLexString ();
					{ return dParserCompiler::PREC;}
					state = 0;
					ch = NextChar();
					break;
				}
				case 5:
				{
					GetLexString ();
					{ return dParserCompiler::RIGHT;}
					state = 0;
					ch = NextChar();
					break;
				}
				case 6:
				{
					GetLexString ();
					{ return dParserCompiler::START;}
					state = 0;
					ch = NextChar();
					break;
				}
				case 7:
				{
					GetLexString ();
					{ return dParserCompiler::TOKEN;}
					state = 0;
					ch = NextChar();
					break;
				}
				case 8:
				{
					GetLexString ();
					{ return dParserCompiler::UNION;}
					state = 0;
					ch = NextChar();
					break;
				}
				case 9:
				{
					GetLexString ();
					{ m_tokenString.replace(0, 2, ""); m_tokenString.replace(m_tokenString.size() - 2, 2, ""); return dParserCompiler::CODE_BLOCK;}
					state = 0;
					ch = NextChar();
					break;
				}
				case 10:
				{
					GetLexString ();
					{ m_tokenString = "!"; return('!'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 11:
				{
					GetLexString ();
					{ m_tokenString = "%"; return('%'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 12:
				{
					GetLexString ();
					{ m_tokenString = "&"; return('&'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 13:
				{
					GetLexString ();
					{ m_tokenString = "("; return('('); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 14:
				{
					GetLexString ();
					{ m_tokenString = ")"; return(')'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 15:
				{
					GetLexString ();
					{ m_tokenString = "*"; return('*'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 16:
				{
					GetLexString ();
					{ m_tokenString = "+"; return('+'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 17:
				{
					GetLexString ();
					{ m_tokenString = ","; return(','); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 18:
				{
					GetLexString ();
					{ m_tokenString = "-"; return('-'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 19:
				{
					GetLexString ();
					{ m_tokenString = "."; return('.'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 20:
				{
					GetLexString ();
					{ m_tokenString = "/"; return('/'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 21:
				{
					GetLexString ();
					{ m_tokenString = ":"; return(':'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 22:
				{
					GetLexString ();
					{ m_tokenString = ";"; return(';'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 23:
				{
					GetLexString ();
					{ m_tokenString = "<"; return('<'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 24:
				{
					GetLexString ();
					{ m_tokenString = "="; return('='); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 25:
				{
					GetLexString ();
					{ m_tokenString = ">"; return('>'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 26:
				{
					GetLexString ();
					{ m_tokenString = "?"; return('?'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 27:
				{
					GetLexString ();
					{ m_tokenString = m_tokenString.substr (1, 1); return(m_tokenString[0]); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 28:
				{
					GetLexString ();
					{ m_tokenString = "["; return('['); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 29:
				{
					GetLexString ();
					{ m_tokenString = "\\"; return('\\'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 30:
				{
					GetLexString ();
					{ m_tokenString = "]"; return(']'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 31:
				{
					GetLexString ();
					{ m_tokenString = "^"; return('^'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 32:
				{
					GetLexString ();
					{ m_tokenString = "{"; return('{'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 33:
				{
					GetLexString ();
					{ m_tokenString = "|"; return('|'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 34:
				{
					GetLexString ();
					{ m_tokenString = "}"; return('}'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 35:
				{
					GetLexString ();
					{ m_tokenString = "~"; return('~'); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 36:
				{
					GetLexString ();
					{}
					state = 0;
					ch = NextChar();
					break;
				}
				case 37:
				{
					GetLexString ();
					{}
					state = 0;
					ch = NextChar();
					break;
				}
				case 38:
				{
					GetLexString ();
					{}
					state = 0;
					ch = NextChar();
					break;
				}
				case 39:
				{
					GetLexString ();
					{ return(dParserCompiler::COLOM); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 40:
				{
					GetLexString ();
					{ return(dParserCompiler::SIMICOLOM); }
					state = 0;
					ch = NextChar();
					break;
				}
				case 41:
				{
					GetLexString ();
					{ return dParserCompiler::LITERAL;}
					state = 0;
					ch = NextChar();
					break;
				}
				case 42:
				{
					GetLexString ();
					{ return dParserCompiler::LITERAL;}
					state = 0;
					ch = NextChar();
					break;
				}
				case 43:
				{
					GetLexString ();
					{ ReadBalancedExpresion ('{', '}'); return dParserCompiler::SEMANTIC_ACTION;}
					state = 0;
					ch = NextChar();
					break;
				}
				case 44:
				{
					GetLexString ();
					{ return(dParserCompiler::OR); }
					state = 0;
					ch = NextChar();
					break;
				}


				default:
				{
					// Lexical error
					return -1;
				}
			}
		}
		if (!ch) {
			zeroCount--;
		}
	} while (zeroCount);
	// Unknown pattern
	return -1;
}



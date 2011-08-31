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
//Auto generated Lexical Analyzer class: dAssemblerLexical.cpp
//

//
// Newton Scrip Lex parcel
// based on a subset of Java language specification 1.0 
//

#include <dVirtualMachine.h>
#include "dAssemblerParcer.h"

#include "dAssemblerLexical.h"

dAssemblerLexical::dAssemblerLexical(const char* const data)
	:m_token(0)
	,m_state(0)
	,m_lastState(116)
	,m_startState(0)
	,m_index(0)
	,m_startIndex(0)
	,m_data(data)
	,m_tokenString("")
{
}

dAssemblerLexical::~dAssemblerLexical()
{
}

char dAssemblerLexical::NextChar()
{
	return m_data[m_index++];
}

const char* dAssemblerLexical::GetTokenString() const
{
	return m_tokenString.c_str();
}

bool dAssemblerLexical::IsCharInSet (int ch, const char* const set) const
{
	for (int i = 0; i < set[i]; i ++) {
		if (ch == set[i]) {
			 return true;
		}
	}
	return false;
}

void dAssemblerLexical::GetLexString ()
{
	int length = m_index - m_startIndex;
	m_tokenString = string (&m_data[m_startIndex], length);
	m_startIndex = m_index;
	m_state = NextPattern();
}

int dAssemblerLexical::NextPattern ()
{
	static int nextState[] = {2, 0, 8, 0, 0, 0, 0, 0, 12, 0, 0, 0, 16, 0, 0, 0, 18, 0, 20, 0, 26, 0, 0, 0, 0, 0, 30, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 51, 0, 0, 0, 0, 0, 57, 0, 0, 0, 0, 0, 62, 0, 0, 0, 0, 67, 0, 0, 0, 0, 73, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 87, 0, 0, 0, 0, 0, 0, 95, 0, 0, 0, 0, 0, 0, 0, 98, 0, 0, 101, 0, 0, 104, 0, 0, 112, 0, 0, 0, 0, 0, 0, 0, 114, 0, };

	m_index = m_startIndex;
	_ASSERTE (nextState[m_startState] <= (sizeof (nextState) / sizeof (nextState[0])));
	m_startState = nextState[m_startState];
	return m_startState;
}



int dAssemblerLexical::NextToken ()
{
	//static strings patterns
	static char text_0[] = {9, 10, 13, 32, 0};
	static char text_1[] = {76, 108, 0};
	static char text_2[] = {79, 111, 0};
	static char text_3[] = {65, 97, 0};
	static char text_4[] = {68, 100, 0};
	static char text_5[] = {73, 105, 0};
	static char text_6[] = {82, 114, 0};
	static char text_7[] = {69, 101, 0};
	static char text_8[] = {84, 116, 0};
	static char text_9[] = {66, 98, 0};
	static char text_10[] = {89, 121, 0};
	static char text_11[] = {87, 119, 0};
	static char text_12[] = {85, 117, 0};
	static char text_13[] = {70, 102, 0};
	static char text_14[] = {83, 115, 0};
	static char text_15[] = {43, 45, 0};
	static char text_16[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 0};
	static char text_17[] = {46, 0};
	static char text_18[] = {65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};
	static char text_19[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};
	static char text_20[] = {47, 0};
	static char text_21[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};

	static char* testSetArray[] = {text_0, text_1, text_2, text_3, text_4, text_5, text_6, text_7, text_8, text_9, text_10, text_11, text_12, text_13, text_14, text_15, text_16, text_17, text_18, text_19, text_20, text_21};
	static int transitionsCount[] = {1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 2, 1, 2, 1, 0, 2, 1, 0, 2, 1, 0, 1, 1, 0, 1, 0, 0, 1, 2, 0, 2, 2, 1, 0, 0, 1, 0, 0, 0};
	static int nextState[][116] = {
		{1, 0, 3, 4, 5, 6, 7, 0, 9, 10, 11, 0, 13, 14, 15, 0, 17, 0, 19, 0, 21, 22, 23, 24, 25, 0, 27, 28, 29, 0, 31, 32, 33, 34, 35, 36, 37, 0, 39, 40, 41, 42, 43, 44, 0, 46, 47, 48, 49, 50, 0, 52, 53, 54, 55, 56, 0, 58, 59, 60, 61, 0, 63, 64, 65, 66, 0, 68, 69, 70, 71, 72, 0, 74, 75, 76, 77, 78, 79, 0, 81, 82, 83, 84, 85, 86, 0, 88, 89, 89, 91, 0, 93, 94, 0, 96, 97, 0, 99, 100, 0, 102, 0, 0, 105, 106, 0, 108, 108, 110, 0, 0, 113, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 89, 0, 90, 0, 0, 94, 0, 0, 97, 0, 0, 0, 0, 0, 0, 0, 0, 0, 107, 0, 109, 109, 0, 0, 0, 0, 0, 0, 0}
	};
	static int charaterTests[][116] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 58, 0, 44, 0, 98, 101, 103, 105, 110, 0, 101, 110, 100, 0, 105, 110, 99, 108, 117, 100, 101, 0, 112, 117, 98, 108, 105, 99, 0, 100, 97, 116, 97, 58, 0, 99, 111, 100, 101, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	static int testSetArrayIndex[][116] = {
		{0, 0, 1, 2, 3, 4, 5, 0, 3, 4, 4, 0, 6, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 10, 8, 7, 0, 11, 2, 6, 4, 0, 4, 4, 11, 2, 6, 0, 4, 2, 12, 9, 1, 7, 0, 2, 13, 13, 14, 7, 8, 0, 15, 16, 16, 16, 0, 15, 16, 0, 15, 16, 0, 6, 16, 0, 18, 0, 0, 20, 20, 0, 20, 20, 20, 0, 0, 21, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 17, 0, 0, 16, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 21, 21, 0, 0, 0, 0, 0, 0, 0}
	};
	//static int testSetArrayIndexOffset[] = {$(testSetArrayOffsets)};
	//static int testSetArrayIndex[] = {$(testSetArrayIndex)};

	m_state = 0;
	m_startState = 0;
	m_startIndex = m_index;

	while ((m_state != m_lastState) && (m_data[m_index] != 0))
	{
		switch (m_state)
		{
						// {WhiteSpace}
			case 1:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_0)) m_state = 1;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{}
				}
				break;
			}
			// {loadI}
			case 7:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::LOADI;}
				}
				break;
			}
			// {add}
			case 11:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::ADD;}
				}
				break;
			}
			// {ret}
			case 15:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::RET;}
				}
				break;
			}
			// ":"
			case 17:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return(':'); }
				}
				break;
			}
			// ","
			case 19:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return(','); }
				}
				break;
			}
			// "begin"
			case 25:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::BEGIN;}
				}
				break;
			}
			// "end"
			case 29:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::END;}
				}
				break;
			}
			// "include"
			case 37:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::INCLUDE;}
				}
				break;
			}
			// "public"
			case 44:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::PUBLIC;}
				}
				break;
			}
			// "data:"
			case 50:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::DATASEGMENT;}
				}
				break;
			}
			// "code:"
			case 56:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::CODESEGMENT;}
				}
				break;
			}
			// {Byte}
			case 61:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::BYTE;}
				}
				break;
			}
			// {Word}
			case 66:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::WORD;}
				}
				break;
			}
			// {DWord}
			case 72:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::DWORD;}
				}
				break;
			}
			// {Double}
			case 79:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::DOUBLE;}
				}
				break;
			}
			// {Offset}
			case 86:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::INTERGER;}	
				}
				break;
			}
			// {Float}
			case 91:
			{
				char ch = NextChar();
				if (ch == 101) m_state = 92;
				else if (IsCharInSet (ch, text_16)) m_state = 91;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::FLOAT;}
				}
				break;
			}
			case 94:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_16)) m_state = 94;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::FLOAT;}
				}
				break;
			}
			// {Integer}
			case 97:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_16)) m_state = 97;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::INTERGER;}
				}
				break;
			}
			// {Register}
			case 100:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_16)) m_state = 100;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::REGISTER;}
				}
				break;
			}
			// {Literal}
			case 102:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_19)) m_state = 103;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::LITERAL;}
				}
				break;
			}
			case 103:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_19)) m_state = 103;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::LITERAL;}
				}
				break;
			}
			// {Comment}
			case 110:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_20)) m_state = 108;
				else if (IsCharInSet (ch, text_21)) m_state = 109;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{}
				}
				break;
			}
			case 106:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_21)) m_state = 111;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{}
				}
				break;
			}
			case 111:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_21)) m_state = 111;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{}
				}
				break;
			}
			// .+
			case 113:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_21)) m_state = 113;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{}
				}
				break;
			}

			default:;
			{
				char ch = NextChar();
				int count = transitionsCount[m_state];
				bool stateChanged = false;
				for (int i = 0; i < count; i ++) {
					int test = charaterTests[i][m_state];
					if (test) {
						if (ch == test) {
							m_state = nextState[i][m_state];
							stateChanged = true;
							break;
						}
					} else {
						int index = testSetArrayIndex[i][m_state];
						const char* text = testSetArray[index];
						if (IsCharInSet (ch, text)) {
							m_state = nextState[i][m_state];
							stateChanged = true;
							break;
						}
					}
				}

				if (!stateChanged) {
					m_state = NextPattern();
				}
				break;
			}
		}
	}
	m_tokenString = "";
	return -1;
}


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
	,m_lastState(72)
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
	static int nextState[] = {2, 0, 8, 0, 0, 0, 0, 0, 12, 0, 0, 0, 16, 0, 0, 0, 22, 0, 0, 0, 0, 0, 26, 0, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 47, 0, 0, 0, 0, 0, 53, 0, 0, 0, 0, 0, 56, 0, 0, 59, 0, 0, 62, 0, 0, 70, 0, 0, 0, 0, 0, 0, 0, };

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
	static char text_9[] = {43, 45, 0};
	static char text_10[] = {65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};
	static char text_11[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};
	static char text_12[] = {47, 0};
	static char text_13[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};

	static char* testSetArray[] = {text_0, text_1, text_2, text_3, text_4, text_5, text_6, text_7, text_8, text_9, text_10, text_11, text_12, text_13};
	static int transitionsCount[] = {1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 2, 0, 2, 2, 1, 0, 0, 0, 0};
	static int nextState[][72] = {
		{1, 0, 3, 4, 5, 6, 7, 0, 9, 10, 11, 0, 13, 14, 15, 0, 17, 18, 19, 20, 21, 0, 23, 24, 25, 0, 27, 28, 29, 30, 31, 32, 33, 0, 35, 36, 37, 38, 39, 40, 0, 42, 43, 44, 45, 46, 0, 48, 49, 50, 51, 52, 0, 54, 0, 0, 57, 58, 0, 60, 0, 0, 63, 64, 0, 66, 66, 68, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65, 0, 67, 67, 0, 0, 0, 0, 0}
	};
	static int charaterTests[][72] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 98, 101, 103, 105, 110, 0, 101, 110, 100, 0, 105, 110, 99, 108, 117, 100, 101, 0, 112, 117, 98, 108, 105, 99, 0, 100, 97, 116, 97, 58, 0, 99, 111, 100, 101, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	static int testSetArrayIndex[][72] = {
		{0, 0, 1, 2, 3, 4, 5, 0, 3, 4, 4, 0, 6, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 6, 6, 0, 10, 0, 0, 12, 12, 0, 12, 12, 12, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 13, 13, 0, 0, 0, 0, 0}
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
			// "begin"
			case 21:
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
			case 25:
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
			case 33:
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
			case 40:
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
			case 46:
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
			case 52:
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
			// {SignedInt}
			case 54:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_9)) m_state = 55;
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
			case 55:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_9)) m_state = 55;
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
			case 58:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_6)) m_state = 58;
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
			case 60:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_11)) m_state = 61;
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
			case 61:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_11)) m_state = 61;
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
			case 68:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_12)) m_state = 66;
				else if (IsCharInSet (ch, text_13)) m_state = 67;
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
			case 64:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_13)) m_state = 69;
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
			case 69:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_13)) m_state = 69;
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


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
	,m_lastState(22)
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
	static int nextState[] = {11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, };

	m_index = m_startIndex;
	_ASSERTE (nextState[m_startState] <= (sizeof (nextState) / sizeof (nextState[0])));
	m_startState = nextState[m_startState];
	return m_startState;
}



int dAssemblerLexical::NextToken ()
{
	//static strings patterns
	static char text_0[] = {47, 0};
	static char text_1[] = {42, 0};
	static char text_2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_3[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_4[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};

	static char* testSetArray[] = {text_0, text_1, text_2, text_3, text_4};
	static int transitionsCount[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 0, 2, 2, 2, 0, 2, 0, 0, 0};
	static int nextState[][22] = {
		{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, 12, 13, 0, 15, 17, 15, 0, 15, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 0, 16, 18, 16, 0, 16, 0, 0, 0}
	};
	static int charaterTests[][22] = {
		{87, 104, 105, 116, 101, 83, 112, 97, 99, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	static int testSetArrayIndex[][22] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 3, 2, 0, 2, 0, 0, 0}
	};
	//static int testSetArrayIndexOffset[] = {$(testSetArrayOffsets)};
	//static int testSetArrayIndex[] = {$(testSetArrayIndex)};

	m_state = 0;
	m_startState = 0;
	m_startIndex = m_index;

	while (m_state != m_lastState)
	{
		switch (m_state)
		{
						// WhiteSpace
			case 10:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{}
				}
				break;
			}
			// {Comment}
			case 17:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{}
				}
				break;
			}
			case 13:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_4)) m_state = 19;
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
			case 19:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_4)) m_state = 19;
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


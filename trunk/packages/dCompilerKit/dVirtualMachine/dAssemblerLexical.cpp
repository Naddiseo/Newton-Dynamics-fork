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
	,m_lastState(161)
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
	static int nextState[] = {6, 0, 0, 0, 0, 0, 10, 0, 0, 0, 14, 0, 0, 0, 20, 0, 0, 0, 0, 0, 24, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 39, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 0, 0, 51, 0, 0, 0, 0, 0, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 70, 0, 0, 0, 0, 0, 0, 0, 0, 78, 0, 0, 0, 0, 0, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, };

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
	static int transitionsCount[] = {1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 0, 2, 2, 2, 0, 2, 0, 0, 0};
	static int nextState[][89] = {
		{1, 2, 3, 4, 5, 0, 7, 8, 9, 0, 11, 12, 13, 0, 15, 16, 17, 18, 19, 0, 21, 22, 23, 0, 25, 26, 27, 28, 29, 30, 31, 0, 33, 34, 35, 36, 37, 38, 0, 40, 41, 42, 43, 44, 0, 46, 47, 48, 49, 50, 0, 52, 53, 54, 55, 56, 57, 58, 59, 60, 0, 62, 63, 64, 65, 66, 67, 68, 69, 0, 71, 72, 73, 74, 75, 76, 77, 0, 79, 80, 0, 82, 84, 82, 0, 82, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 83, 85, 83, 0, 83, 0, 0, 0}
	};
	static int charaterTests[][89] = {
		{108, 111, 97, 100, 73, 0, 97, 100, 100, 0, 114, 101, 116, 0, 98, 101, 103, 105, 110, 0, 101, 110, 100, 0, 105, 110, 99, 108, 117, 100, 101, 0, 112, 117, 98, 108, 105, 99, 0, 100, 97, 116, 97, 58, 0, 99, 111, 100, 101, 58, 0, 83, 105, 103, 110, 101, 100, 73, 110, 116, 0, 82, 101, 103, 105, 115, 116, 101, 114, 0, 76, 105, 116, 101, 114, 97, 108, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	static int testSetArrayIndex[][89] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 3, 2, 0, 2, 0, 0, 0}
	};
	//static int testSetArrayIndexOffset[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 3, 5, 7, 9, 9, 11, 11, 11};
	//static int testSetArrayIndex[] = {0, 0, 1, 1, 2, 0, 3, 1, 2, 1, 2};

	m_state = 0;
	m_startState = 0;
	m_startIndex = m_index;

	while (m_state != m_lastState)
	{
		switch (m_state)
		{
						// loadI
			case 5:
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
			// add
			case 9:
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
			// ret
			case 13:
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
			case 19:
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
			case 23:
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
			case 31:
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
			case 38:
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
			case 44:
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
			case 50:
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
			// SignedInt
			case 60:
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
			// Register
			case 69:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::REGISTER;}
				}
				break;
			}
			// Literal
			case 77:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{return dAssemblerParcer::LITERAL;}
				}
				break;
			}
			// {Comment}
			case 84:
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
			case 80:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_4)) m_state = 86;
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
			case 86:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_4)) m_state = 86;
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


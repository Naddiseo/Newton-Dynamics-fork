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
//Auto generated Lexical Analyzer class: dAssemberLexical.cpp
//

//
// Newton Scrip Lex parcel
// based on a subset of Java language specification 1.0 
//

#include <dCompilerKit.h>
#include "dAssemberLexical.h"

dAssemberLexical::dAssemberLexical(const char* const data)
	:m_token(0)
	,m_state(0)
	,m_lastState(9)
	,m_startState(0)
	,m_index(0)
	,m_length(0)
	,m_data(data)
	,m_tokenString("")
{
}

dAssemberLexical::~dAssemberLexical()
{
}

bool dAssemberLexical::IsCharInSet (int ch, const char* const set)
{
	for (int i = 0; i < set[i]; i ++) {
		if (ch == set[i]) {
			 return true;
		}
	}
	return false;
}

int dAssemberLexical::NextPattern ()
{
	m_state = 0;
	switch (m_startState)
	{
			case 0: m_startState = 9; break;
	}
	return m_startState;
}

int dAssemberLexical::NextToken ()
{
	//static strings patterns
	static char text_0[] = {47, 0};
	static char text_1[] = {42, 0};
	static char text_2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_3[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_4[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	
	m_state = 0;
	while (m_state != m_lastState)
	{
		switch (m_state)
		{
			// {Comment}
			case 0:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_0)) m_state = 1;
				else m_state = NextPattern();
				break;
			}
			case 1:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_0)) m_state = 2;
				else if (IsCharInSet (ch, text_1)) m_state = 3;
				else m_state = NextPattern();
				break;
			}
			case 3:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_1)) m_state = 4;
				else if (IsCharInSet (ch, text_2)) m_state = 5;
				else m_state = NextPattern();
				break;
			}
			case 5:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_1)) m_state = 4;
				else if (IsCharInSet (ch, text_2)) m_state = 5;
				else m_state = NextPattern();
				break;
			}
			case 4:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_0)) m_state = 6;
				else if (IsCharInSet (ch, text_3)) m_state = 7;
				else m_state = NextPattern();
				break;
			}
			case 7:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_1)) m_state = 4;
				else if (IsCharInSet (ch, text_2)) m_state = 5;
				else m_state = NextPattern();
				break;
			}
			case 6:
			{
				{
					m_state = NextPattern();
					m_tokenString = string (&m_data[m_index], m_length);
					m_index += m_length;
					m_length = 0;
					{
						//user specified action
						{}
					}
				}
				break;
			}
			case 2:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_4)) m_state = 8;
				else {
					m_state = NextPattern();
					m_tokenString = string (&m_data[m_index], m_length);
					m_index += m_length;
					m_length = 0;
					{
						//user specified action
						{}
					}
				}
				break;
			}
			case 8:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_4)) m_state = 8;
				else {
					m_state = NextPattern();
					m_tokenString = string (&m_data[m_index], m_length);
					m_index += m_length;
					m_length = 0;
					{
						//user specified action
						{}
					}
				}
				break;
			}
		}
	}
	return -1;
}


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

#include <dVirtualMachine.h>
#include "dAssemblerParcer.h"

//
// Newton virtual machine assembler grammar
// based loosely on the MIPS R3000 and the Intel 386 instructions sets 
//

#include "dAssemblerLexical.h"


dAssemblerLexical::dAssemblerLexical(const char* const data)
	:m_tokenString ("")
	,m_data(data)
	,m_index(0)
	,m_startIndex(0)
	,m_lineNumber(0)
{
}

dAssemblerLexical::~dAssemblerLexical()
{
}

bool dAssemblerLexical::IsCharInSet (char ch, const char* const set, int setSize) const
{
	int i0 = 0;
	int i1 = setSize - 1;
	while ((i1 - i0) >= 4) {
		int i = (i1 + i0 + 1)>>1;
		if (ch <= set[i]) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		if (ch == set[i]) {
			return true;
		}
	}
	return false;
}

void dAssemblerLexical::ReadBalancedExpresion (char open, char close)
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


void dAssemblerLexical::GetLexString ()
{
	int length = m_index - m_startIndex;
	m_tokenString = string (&m_data[m_startIndex], length);
	m_startIndex = m_index;
}


int dAssemblerLexical::NextToken ()
{
	//static strings patterns
	static char text_0[] = {9, 10, 13, 32, 0};
	static char text_1[] = {47, 0};
	static char text_2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_3[] = {42, 0};
	static char text_4[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_5[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_6[] = {46, 0};
	static char text_7[] = {60, 0};
	static char text_8[] = {62, 0};
	static char text_9[] = {61, 0};
	static char text_10[] = {43, 45, 0};
	static char text_11[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 0};
	static char text_12[] = {65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};
	static char text_13[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};

	static int characterSetSize[] = {4, 1, 126, 1, 126, 126, 1, 1, 1, 1, 2, 10, 53, 63};
	static char* characterSetArray[] = {text_0, text_1, text_2, text_3, text_4, text_5, text_6, text_7, text_8, text_9, text_10, text_11, text_12, text_13};

	static int transitionsCount[] = {10, 2, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 2, 2, 0, 2, 0, 1, 1, 0, 1, 1, 1, 0, 0};
	static int transitionsStart[] = {0, 21, 0, 11, 0, 0, 0, 0, 10, 0, 0, 0, 0, 13, 17, 15, 0, 19, 0, 24, 23, 0, 25, 26, 27, 0, 0};
	static unsigned nextTranstionList[] = {0x0690001, 0x04002, 0x014003, 0x064004, 0x074005, 0x084006, 0x094007, 0x0a4008, 0x0b4009, 0x0c400a, 0x0b4009, 0x01400c, 0x03400d, 0x03400e, 0x04400f, 0x03400e, 0x04400f, 0x014010, 0x054011, 0x03400e, 0x04400f, 0x06d0013, 0x06e0014, 0x0740015, 0x0700016, 0x06f0017, 0x0720018, 0x0740019, 0};
	
	m_startIndex = m_index;

	int state = 0;
	for (bool matchFound = false; !matchFound; )
	{
		switch (state) 
		{
			case 10:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_13, characterSetSize[13])) state = 11;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{return dAssemblerParcer::LITERAL;}
					state = 0;
				}
				break;
			}
			case 11:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_13, characterSetSize[13])) state = 11;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{return dAssemblerParcer::LITERAL;}
					state = 0;
				}
				break;
			}
			case 9:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_11, characterSetSize[11])) state = 9;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{return dAssemblerParcer::INTEGER;}
					state = 0;
				}
				break;
			}
			case 7:
			{
				{
					GetLexString ();
					//user specified action
					{return '=';}
					state = 0;
				}
				break;
			}
			case 6:
			{
				{
					GetLexString ();
					//user specified action
					{return '>';}
					state = 0;
				}
				break;
			}
			case 5:
			{
				{
					GetLexString ();
					//user specified action
					{return '<';}
					state = 0;
				}
				break;
			}
			case 4:
			{
				{
					GetLexString ();
					//user specified action
					{return '.';}
					state = 0;
				}
				break;
			}
			case 16:
			{
				{
					GetLexString ();
					//user specified action
					{}
					state = 0;
				}
				break;
			}
			case 12:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_2, characterSetSize[2])) state = 18;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{}
					state = 0;
				}
				break;
			}
			case 18:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_2, characterSetSize[2])) state = 18;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{}
					state = 0;
				}
				break;
			}
			case 2:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_0, characterSetSize[0])) state = 2;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{}
					state = 0;
				}
				break;
			}
			case 21:
			{
				{
					GetLexString ();
					//user specified action
					{return dAssemblerParcer::INT;}
					state = 0;
				}
				break;
			}
			case 25:
			{
				{
					GetLexString ();
					//user specified action
					{return dAssemblerParcer::IMPORT;}
					state = 0;
				}
				break;
			}

		
			default:
			{
				char ch = NextChar();
				int count = transitionsCount[state];
				int start = transitionsStart[state];
				unsigned* const transitionsList = &nextTranstionList[start];

				bool stateChanged = false;
				for (int i = 0; i < count; i ++) {
					dTransitionInfo transition (transitionsList[i]);
					if (transition.m_infoType == m_infoIsCharacter) {
						if (ch == char (transition.m_info)) {
							state = transition.m_nextState;
							stateChanged = true;
							break;
						}
					} else {
						_ASSERTE (transition.m_infoType == m_infoIsCharacterSet);
						int index = transition.m_info;
						int length = characterSetSize[index];
						const char* text = characterSetArray[index];
						if (IsCharInSet (ch, text, length)) {
							state = transition.m_nextState;
							stateChanged = true;
							break;
						}

					}
				}

				if (!stateChanged) {
					// Unknown pattern
					return -1;
				}
			}
		}
	}
	// Unknown pattern
	return -1;
}



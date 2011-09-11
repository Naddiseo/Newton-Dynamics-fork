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
	static char text_1[] = {43, 0};
	static char text_2[] = {42, 0};
	static char text_3[] = {40, 0};
	static char text_4[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 0};

	static int characterSetSize[] = {4, 1, 1, 1, 10};
	static char* characterSetArray[] = {text_0, text_1, text_2, text_3, text_4};

	static int transitionsCount[] = {5, 0, 0, 0, 0, 0, 0};
	static int transitionsStart[] = {0, 0, 0, 0, 0, 0, 0};
	static unsigned nextTranstionList[] = {0x04001, 0x014002, 0x024003, 0x034004, 0x044005, 0};
	
	m_startIndex = m_index;

	int state = 0;
	for (bool matchFound = false; !matchFound; )
	{
		switch (state) 
		{
			case 5:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_4, characterSetSize[4])) state = 5;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{return dAssemblerParcer::id;}
					state = 0;
				}
				break;
			}
			case 4:
			{
				{
					GetLexString ();
					//user specified action
					{return ')';}
					state = 0;
				}
				break;
			}
			case 3:
			{
				{
					GetLexString ();
					//user specified action
					{return '*';}
					state = 0;
				}
				break;
			}
			case 2:
			{
				{
					GetLexString ();
					//user specified action
					{return '+';}
					state = 0;
				}
				break;
			}
			case 1:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_0, characterSetSize[0])) state = 1;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{}
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



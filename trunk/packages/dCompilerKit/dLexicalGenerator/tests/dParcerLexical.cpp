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
//Auto generated Lexical Analyzer class: dParcerLexical.cpp
//

#include <dParcerCompiler.h>
#include "dGrammarLexical.h"

#include "dParcerLexical.h"

#ifdef _MSC_VER
	#pragma warning (disable: 4702) // warning C4702: unreachable code
#endif


dParcerLexical::dParcerLexical(const char* const data)
	:m_tokenString ("")
	,m_data(data)
	,m_index(0)
	,m_startIndex(0)
//	,m_token(0)
//	,state(0)
//	,m_lastState($(statesCount))
//	,m_startState(0)
//	
{
}

dParcerLexical::~dParcerLexical()
{
}

char dParcerLexical::NextChar()
{
	return m_data[m_index++];
}

const char* dParcerLexical::GetTokenString() const
{
	return m_tokenString.c_str();
}

bool dParcerLexical::IsCharInSet (char ch, const char* const set, int setSize) const
{
//	for (int i = 0; i < set[i]; i ++) {
//		if (ch == set[i]) {
//			 return true;
//		}
//	}
//	return false;

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


void dParcerLexical::GetLexString ()
{
	int length = m_index - m_startIndex;
	m_tokenString = string (&m_data[m_startIndex], length);
	m_startIndex = m_index;
//	state = NextPattern();
}

/*
int dParcerLexical::NextPattern ()
{
	static int nextState[] = {$(nextTokenStart)};

	m_index = m_startIndex;
	_ASSERTE (nextState[m_startState] <= (sizeof (nextState) / sizeof (nextState[0])));
	m_startState = nextState[m_startState];
	return m_startState;
}
*/


int dParcerLexical::NextToken ()
{
	//static strings patterns

	static int characterSetSize[] = {0};
	static char* characterSetArray[] = {0};

	static int transitionsCount[] = {1, 1, 1, 1, 1, 1, 1, 1, 0, 0};
	static int transitionsStart[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 0};
	static dTransitionInfo nextTranstionList[] = {34, 0, 1, 37, 0, 2, 116, 0, 3, 111, 0, 4, 107, 0, 5, 101, 0, 6, 110, 0, 7, 34, 0, 8, 0};
	
//	state = 0;
//	m_startState = 0;
	m_startIndex = m_index;
//	while ((state != m_lastState) && (m_data[m_index] != 0))

	int state = 0;
	for (bool matchFound = false; !matchFound; )
	{
		switch (state) 
		{
			case 8:
			{
				{
					GetLexString ();
					state = 0;
					//user specified action
					matchFound = true;
					{ return dParcerCompiler::TOKEN;}
					matchFound = false;
				}
				break;
			}

		
			default:
			{
				char ch = NextChar();
				int count = transitionsCount[state];
				int start = transitionsStart[state];
				dTransitionInfo* const transitionsList = &nextTranstionList[start];

				bool stateChanged = false;
				for (int i = 0; (i < count) && !stateChanged; i ++) {
					dTransitionInfo& transition = transitionsList[i];
					switch (transition.m_type)
					{
						case m_infoIsCharacter:
						{
							if (ch == transition.m_info) {
								state = transition.m_nextState;
								stateChanged = true;
							}
							break;
						}

						case m_infoIsCharacterSet:
						{
							int index = transition.m_info;
							int length = characterSetSize[index];
							const char* text = characterSetArray[index];
							if (IsCharInSet (ch, text, length)) {
								state = transition.m_nextState;
								stateChanged = true;
							}
							break;
						}

						case m_infoIsInitBalanceCounter:
						{
							_ASSERTE (0);
							break;
						}

						case m_infoIsIncrementBalanceCounter:
						{
							_ASSERTE (0);
							break;
						}

						case m_infoIsDecrementBalanceCounter:
						{
							_ASSERTE (0);
							break;
						}
					}
				}
			}
		}
	}
//	m_tokenString = "";

	return -1;
}


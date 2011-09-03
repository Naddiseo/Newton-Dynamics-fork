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

dParcerLexical::dParcerLexical(const char* const data)
	:m_token(0)
	,m_state(0)
	,m_lastState($(statesCount))
	,m_startState(0)
	,m_index(0)
	,m_startIndex(0)
	,m_data(data)
	,m_tokenString("")
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
	for (int i = 0; i < set[i]; i ++) {
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
	m_state = NextPattern();
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


	struct dTransitionInfo
	{
		short m_info;
		short m_type;
		short m_nextState;
	};
	static int transitionsCount[] = {1, 0, 0};
	static int transitionsStart[] = {0, 0, 0};
	static dTransitionInfo nextTranstionList[] = {97, 0, 1, 0};
	
//	static char* testSetArray[] = {};
//	static int nextState[][$(statesCount)] = {$(nextState)};
//	static int charaterTests[][$(statesCount)] = {$(charactesTest)};
//	static int testSetArrayIndex[][$(statesCount)] = {$(testSetArrayIndex__)};
//	//static int testSetArrayIndexOffset[] = {$(testSetArrayOffsets)};
//	//static int testSetArrayIndex[] = {$(testSetArrayIndex)};

	m_state = 0;
//	m_startState = 0;
//	m_startIndex = m_index;
//	while ((m_state != m_lastState) && (m_data[m_index] != 0))

	for (bool matchFound = false; !matchFound; )
	{
		switch (m_state)
		{
//			$(finiteAutomataCode)
			case 1:
				matchFound = true;
				break;

			default:;
			{
				char ch = NextChar();
				int count = transitionsCount[m_state];
				dTransitionInfo* const transitionsList = &nextTranstionList[transitionsStart[m_state]];

				bool stateChanged = false;
				for (int i = 0; (i < count) && !stateChanged; i ++) {
					//int test = charaterTests[i][m_state];
					dTransitionInfo& transition = transitionsList[i];
					switch (transition.m_info)
					{
						case m_infoIsCharacter:
						{
							if (ch == transition.m_info) {
								m_state = transition.m_nextState;
								stateChanged = true;
							}
							break;
						}

						case m_infoIsCharacter:
						{
							_ASSERTE (0);
							break;
						}

						case m_infoIsCharacterSet:
						{
							_ASSERTE (0);
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
/*
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
*/
				}

//				if (!stateChanged) {
//					m_state = NextPattern();
//				}
				break;
			}
		}
	}
//	m_tokenString = "";

	return -1;
}



xxxxxxxx

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
//Auto generated Lexical Analyzer class: $(className).cpp
//
$(userIncludeCode)
#include "$(className).h"

$(className)::$(className)(const char* const data)
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

$(className)::~$(className)()
{
}

char $(className)::NextChar()
{
	return m_data[m_index++];
}

const char* $(className)::GetTokenString() const
{
	return m_tokenString.c_str();
}

bool $(className)::IsCharInSet (int ch, const char* const set) const
{
	for (int i = 0; i < set[i]; i ++) {
		if (ch == set[i]) {
			 return true;
		}
	}
	return false;
}

void $(className)::GetLexString ()
{
	int length = m_index - m_startIndex;
	m_tokenString = string (&m_data[m_startIndex], length);
	m_startIndex = m_index;
	m_state = NextPattern();
}

int $(className)::NextPattern ()
{
	static int nextState[] = {$(nextTokenStart)};

	m_index = m_startIndex;
	_ASSERTE (nextState[m_startState] <= (sizeof (nextState) / sizeof (nextState[0])));
	m_startState = nextState[m_startState];
	return m_startState;
}



int $(className)::NextToken ()
{
	//static strings patterns
$(characterSets)
	static char* testSetArray[] = {$(characterSetArray)};
	static int transitionsCount[] = {$(transitionsCount)};
	static int nextState[][$(statesCount)] = {$(nextState)};
	static int charaterTests[][$(statesCount)] = {$(charactesTest)};
	static int testSetArrayIndex[][$(statesCount)] = {$(testSetArrayIndex)};

	m_state = 0;
	m_startState = 0;
	m_startIndex = m_index;

	while (m_state != m_lastState)
	{
		switch (m_state)
		{
			$(finiteAutomataCode)
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


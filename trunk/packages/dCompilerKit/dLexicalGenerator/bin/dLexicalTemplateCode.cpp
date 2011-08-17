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
	m_index = m_startIndex;
	switch (m_startState)
	{
$(nextTokenStart)
	}
	return m_startState;
}

int $(className)::NextToken ()
{
	//static strings patterns
$(characterSets)
	
	m_state = 0;
	m_startState = 0;
	m_startIndex = m_index;
	while (m_state != m_lastState)
	{
		switch (m_state)
		{
$(finiteAutomataCode)
		}
	}
	m_tokenString = "";
	return -1;
}


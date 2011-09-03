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

#ifdef _MSC_VER
	#pragma warning (disable: 4702) // warning C4702: unreachable code
#endif


$(className)::$(className)(const char* const data)
	:m_tokenString ("")
	,m_data(data)
	,m_index(0)
	,m_startIndex(0)
	,m_lineNumber(0)
{
}

$(className)::~$(className)()
{
}

bool $(className)::IsCharInSet (char ch, const char* const set, int setSize) const
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


void $(className)::GetLexString ()
{
	int length = m_index - m_startIndex;
	m_tokenString = string (&m_data[m_startIndex], length);
	m_startIndex = m_index;
}


int $(className)::NextToken ()
{
	//static strings patterns
$(characterSets)
	static int characterSetSize[] = {$(characterSetSize)};
	static char* characterSetArray[] = {$(characterSetArray)};

	static int transitionsCount[] = {$(transitionsCount)};
	static int transitionsStart[] = {$(transitionsStart)};
	static dTransitionInfo nextTranstionList[] = {$(nextTranstionList)};
	
	m_startIndex = m_index;

	int state = 0;
	for (bool matchFound = false; !matchFound; )
	{
		switch (state) 
		{
$(userActions)
		
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


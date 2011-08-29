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
//Auto generated Parcer Generator class: test0.cpp
//


#include "test0.h"



enum test0::ActionType
{
	ACCEPT,
	SHIFT,
	REDUCE
};

class test0::dActionEntry
{
public:
	Token m_token;
	int m_state;
	int m_reduceCount;
	ActionType m_action;
};



class test0::dStackPair
{
	public:
	class dUserVariable: public string
	{
		dUserVariable ()
			:string()
		{
		}
	
		dUserVariable (const char* const text)
			:string(text)
		{
		}
	};



	dStackPair()
		:m_state(0), m_token(Token (0)), m_value()
	{
	}

	int m_state;
	Token m_token;
	dUserVariable m_value;
};






test0::test0()
{
}

test0::~test0()
{
}


bool test0::ErrorHandler (string* const line) const
{
	return false;
}

const test0::dActionEntry* test0::FindAction (const dActionEntry* const actionList, int count, Token token) const
{
	for (int i = 0; i < count; i ++) {
		if (actionList[i].m_token == token) {
			return &actionList[i]; 
		}
	}
	return NULL;
}


int test0::Parce(xxxxx& scanner)
{

	dList<dStackPair> stack;

	static int actionOffsets[][2] = {$(actionEntry}};
	static dActionEntry actionTable[] = {$(actionTable}};

	dStackPair.Append ();
	for (Token token = Token (scanner.NextToken()); token != -1; ) {
		dStackPair& stackTop = pool[stack];
		int actionStart = actionOffsets[stackTop.m_state][0];
		int actionCount = actionOffsets[stackTop.m_state][1];

		dActionEntry* const action = FindAction (&actionTable[actionStart], actionCount, token);
		if (action) {
			if (action->m_action == ACCEPT) {
				break;
			} else if (action->m_action == SHIFT) {
				_ASSERTE (0);
				dStackPair& entry = dStackPair.Append().GetInfo();
				entry.m_state = action->m_state;
				entry.m_token = action->m_token;
				entry.m_value = dStackPair::dUserVariable (scanner.GetTokenString());

				token = Token (scanner.NextToken());

			} else {
				_ASSERTE (action->m_action == REDUCE);
				_ASSERTE (0);
			}

		} else {
			// error 
			if (!ErrorHandler ("error")) {
				return 0;
			}
		}

	}

	return 1;
}


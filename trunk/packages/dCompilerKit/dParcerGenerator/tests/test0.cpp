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
#include <dList.h>


enum test0::ActionType
{
	ACCEPT,
	SHIFT,
	REDUCE
};

class test0::dActionEntry
{
	public:
	int m_nextState;
	int statesToPop;
	int m_actionCount;
	Token m_token;
	ActionType m_action;
};



class test0::dStackPair
{
	public:
	class dUserVariable: public string
	{
		public:
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


bool test0::ErrorHandler (const string& line) const
{
	line;
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

//	static int actionOffsets[][2] = {$(actionEntry}};
//	static dActionEntry actionTable[] = {$(actionTable}};
static int actionOffsets[][2] = {1};
static dActionEntry actionTable[] = {{Token(1)}};


	stack.Append ();
	for (Token token = Token (scanner.NextToken()); token != -1; ) {
		const dStackPair& stackTop = stack.GetLast()->GetInfo();
		int actionStart = actionOffsets[stackTop.m_state][0];
		int actionCount = actionOffsets[stackTop.m_state][1];
		const dActionEntry* const action = FindAction (&actionTable[actionStart], actionCount, token);

		if (action) {
			if (action->m_action == ACCEPT) {
				break;
			} else if (action->m_action == SHIFT) {
				_ASSERTE (0);
				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_token = action->m_token;
				entry.m_state = action->m_nextState;
				entry.m_value = dStackPair::dUserVariable (scanner.GetTokenString());

				token = Token (scanner.NextToken());

			} else {

				_ASSERTE (action->m_action == REDUCE);
				_ASSERTE (0);

				_ASSERTE (action->m_action == REDUCE);
				dStackPair parameter[256];

				int reduceCount = action->m_actionCount;
				_ASSERTE (reduceCount < sizeof (parameter) / sizeof (parameter[0]));

				for (int i = 0; i < reduceCount; i ++) {
					parameter[i] = stack.GetLast()->GetInfo();
					stack.Remove (stack.GetLast());
				}
				

				const dStackPair& newStackTop = stack.GetLast()->GetInfo();
				int actionStart = actionOffsets[newStackTop.m_state][0];
				int actionCount = actionOffsets[newStackTop.m_state][1];
				const dActionEntry* const Goto = FindAction (&actionTable[actionStart], actionCount, action->m_token);

				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_token = Goto->m_token;
				entry.m_state = Goto->m_nextState;

				switch (entry.m_token) 
				{
					//do userAction
					//$(userActionsCode);
					case 256:
					{
						break;
					}
					default:;
				}
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


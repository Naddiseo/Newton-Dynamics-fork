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
//Auto generated Parcer Generator class: test1.cpp
//

#include "test1.h"
#include <dList.h>

#define MAX_USER_PARAM	64

enum test1::ActionType
{
	ACCEPT,
	SHIFT,
	REDUCE
};
/*
class test1::dActionEntry
{
	public:
	int m_nextState;
	int m_statesToPop;
	int m_actionCount;
	Token m_token;
	ActionType m_action;
};
*/

class test1::dActionEntry
{
	public:
	dActionEntry (unsigned val)
		:m_value(val)
	{
	}
	union {
		unsigned m_value;
		struct {
			unsigned  m_stateType	: 2;// 0 = shift, 1 = reduce, 2 = accept
			unsigned  m_token		:12;
			unsigned  m_nextState	:12;
			unsigned  m_reduceCount	: 6;

		};
	};
};


class test1::dStackPair
{
	public:

	class myVariable 
	{
		myVariable ()
		myVariable (Token token, const char* const text)
			:m_token(token), m_data (text) 
		{
		}
		Token m_token;
		string m_data;
	};

	dStackPair()
		:m_state(0), m_token(Token (0)), m_value()
	{
	}

	int m_state;
	Token m_token;
	dUserVariable m_value;
};


test1::test1()
{
}

test1::~test1()
{
}


bool test1::ErrorHandler (const string& line) const
{
	line;
	return false;
}

test1::dActionEntry test1::FindAction (const int* const actionList, int count, Token token) const
{
	int i0 = 0;
	int i1 = count - 1;
	while ((i1 - i0) >= 4) {
		int i = (i1 + i0 + 1)>>1;

		dActionEntry action (actionList[i]);
		if (token <=  Token(action.m_token)) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		dActionEntry action (actionList[i]);
		if (token == Token(action.m_token)) {
			return action;;
		}
	}

	return dActionEntry(unsigned (-1));
}


int test1::Parce(lextest1& scanner)
{
	dList<dStackPair> stack;
	static int actionsCount[] = {2, 2, 1, 6, 2, 6, 2, 2, 2, 1, 6, 6};
	static int actionsStart[] = {0, 2, 4, 5, 11, 13, 19, 21, 23, 25, 26, 32};
	static int actionTable[] = {0x100a0, 0x14400, 0x2, 0x180ac, 0x1c0a8, 0x4004001, 0x40040a1, 0x40040a5, 0x40040a9, 0x40040ad, 0x4004401, 0x100a0, 0x14400, 0x4008001, 0x40080a1, 0x40080a5, 0x40080a9, 0x40080ad, 0x4008401, 0x100a0, 0x14400, 0x100a0, 0x14400, 0x2c0a4, 0x180ac, 0x1c0a8, 0xc004001, 0xc0040a1, 0xc0040a5, 0xc0040a9, 0xc0040ad, 0xc004401, 0xc008001, 0xc0080a1, 0xc0080a5, 0xc0080a9, 0xc0080ad, 0xc008401};

	stack.Append ();
	for (Token token = Token (scanner.NextToken()); token != -1; ) {
		const dStackPair& stackTop = stack.GetLast()->GetInfo();
		int start = actionsStart[stackTop.m_state];
		int count = actionsCount[stackTop.m_state];
		dActionEntry action (FindAction (&actionTable[start], count, token));

		switch (action.m_stateType) 
		{
			case 0: // 0 = shift
			{
				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_token = Token (action.m_token);
				entry.m_state = action.m_nextState;
				entry.m_value = dStackPair::dUserVariable (entry.m_token, scanner.GetTokenString());
				token = Token (scanner.NextToken());
				break;
			}

			case 1: // 1 = reduce
			{
				dStackPair parameter[MAX_USER_PARAM];

				int reduceCount = action.m_reduceCount;
				_ASSERTE (reduceCount < sizeof (parameter) / sizeof (parameter[0]));

				for (int i = 0; i < reduceCount; i ++) {
					parameter[i] = stack.GetLast()->GetInfo();
					stack.Remove (stack.GetLast());
				}

				const dStackPair& stackTop = stack.GetLast()->GetInfo();
				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_token = Token (action.m_token);
				entry.m_state = stackTop.m_state;

				switch (action.m_nextState) 
				{
					//do user semantic Action
					//$(semanticActionsCode);
					case 0:
					{
						break;
					}
					default:;
				}

				break;

			}
	
			case 2: // 2 = accept
			{
				_ASSERTE (0);
			}
			
			default:  // syntax grammar error
			{
				_ASSERTE (0);
				// error
				//if (!ErrorHandler ("error")) {
				//}
			}
		}
	}

	return 1;
}







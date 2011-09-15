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
	dSHIFT = 0,
	dREDUCE,
	dACCEPT,
	dERROR
};


class test1::dActionEntry
{
	public:
	dActionEntry (unsigned val)
		:m_value(val)
	{
	}

	short m_token;
	short m_stateType;// 0 = shift, 1 = reduce, 2 = accept
	short m_nextState;
	short m_ruleSymbols;
	short m_ruleIndex;
};

class test1::dGotoEntry
{
	public:
	dGotoEntry ()
	{
	}

	short  m_token;
	short  m_nextState;
};



class test1::dStackPair
{
	public:

	class dUserVariable: public string
	{
		dUserVariable () 
			:string("")
		{
		}
		
		dUserVariable (Token token, const char* const text)
			:m_token(token), m_data (text) 
		{
		}
		Token m_token;
	};

	dStackPair()
		:m_state(0), m_token(dToken (0)), m_value()
	{
	}

	int m_state;
	dToken m_token;
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

test1::dActionEntry test1::FindAction (const int* const actionList, int count, dToken token) const
{
	int i0 = 0;
	int i1 = count - 1;
	while ((i1 - i0) >= 4) {
		int i = (i1 + i0 + 1)>>1;

		dActionEntry action (actionList[i]);
		if (token <= dToken(action.m_token)) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		dActionEntry action (actionList[i]);
		if (token == dToken(action.m_token)) {
			return action;;
		}
	}

	return dActionEntry(unsigned (-1));
}

test1::dGotoEntry dAssemblerParcer::FindGoto (const int* const gotoList, int count, dToken token) const
{
	int i0 = 0;
	int i1 = count - 1;
	while ((i1 - i0) >= 4) {
		int i = (i1 + i0 + 1)>>1;

		dGotoEntry action (gotoList[i]);
		if (token <= dToken(action.m_token)) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		dGotoEntry action (gotoList[i]);
		if (token == dToken(action.m_token)) {
			return action;
		}
	}

	return dGotoEntry(unsigned (-1));
}


bool test1::Parce(lextest1& scanner)
{
	dList<dStackPair> stack;
	static int actionsCount[] = {2, 2, 2, 3, 3, 3, 2, 2, 2, 3, 3, 3};
	static int actionsStart[] = {0, 2, 4, 6, 9, 12, 15, 17, 19, 21, 24, 27};
	static dActionEntry actionTable[] = {{0, 0, 0, 1, 40}, {0, 0, 0, 5, 256}, {0, 0, 0, 1, 40}, {0, 0, 0, 5, 256}, {2, 0, 0, 0, 0}, {0, 0, 0, 7, 43}, {1, 4, 1, 1, 0}, {1, 4, 1, 1, 42}, {1, 4, 1, 1, 43}, {1, 2, 1, 0, 0}, {0, 0, 0, 8, 42}, {1, 2, 1, 0, 43}, {1, 6, 1, 2, 0}, {1, 6, 1, 2, 42}, {1, 6, 1, 2, 43}, {0, 0, 0, 9, 41}, {0, 0, 0, 7, 43}, {0, 0, 0, 1, 40}, {0, 0, 0, 5, 256}, {0, 0, 0, 1, 40}, {0, 0, 0, 5, 256}, {1, 5, 3, 2, 0}, {1, 5, 3, 2, 42}, {1, 5, 3, 2, 43}, {1, 1, 3, 0, 0}, {0, 0, 0, 8, 42}, {1, 1, 3, 0, 43}, {1, 3, 3, 1, 0}, {1, 3, 3, 1, 42}, {1, 3, 3, 1, 43}};

	static int gotoCount[] = {3, 3, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0};
	static int gotoStart[] = {0, 3, 6, 6, 6, 6, 6, 6, 8, 9, 9, 9};
	static int gotoTable[] = {257, 2, 259, 3, 258, 4, 257, 6, 259, 3, 258, 4, 259, 3, 258, 10, 259, 11};

	const int lastToken = 257;

	stack.Append ();
	dToken token = dToken (scanner.NextToken());
	for (;;) {
		const dStackPair& stackTop = stack.GetLast()->GetInfo();
		int start = actionsStart[stackTop.m_state];
		int count = actionsCount[stackTop.m_state];
		dActionEntry action (FindAction (&actionTable[start], count, token));

		switch (action.m_stateType) 
		{
			case dSHIFT: 
			{
				_ASSERTE (0);
/*
				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_token = dToken (action.m_token);
				entry.m_state = action.m_nextState;
				entry.m_value = dStackPair::dUserVariable (entry.m_token, scanner.GetTokenString());
				token = dToken (scanner.NextToken());
				if (token == -1) {
					token = dToken (0);
				}
*/
				break;
			}

			case dREDUCE: 
			{
				_ASSERTE (0);
/*
				dStackPair parameter[MAX_USER_PARAM];

				int reduceCount = action.m_reduceCount;
				_ASSERTE (reduceCount < sizeof (parameter) / sizeof (parameter[0]));

				for (int i = 0; i < reduceCount; i ++) {
					parameter[reduceCount - i - 1] = stack.GetLast()->GetInfo();
					stack.Remove (stack.GetLast());
				}

				const dStackPair& stackTop = stack.GetLast()->GetInfo();
				int start = gotoStart[stackTop.m_state];
				int count = gotoCount[stackTop.m_state];
				dGotoEntry gotoEntry (FindGoto (&gotoTable[start], count, dToken (action.m_nextState + lastToken)));

				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_state = gotoEntry.m_nextState;
				entry.m_token = dToken (gotoEntry.m_token);
				
				switch (action.m_ruleIndex) 
				{
					//do user semantic Actions
					case 4:// rule T : F 
						{entry.m_value = parameter[0].m_value;}
						break;
					case 2:// rule E : T 
						{entry.m_value = parameter[0].m_value;}
						break;
					case 6:// rule F : id 
						{entry.m_value = parameter[0].m_value;}
						break;
					case 5:// rule F : ( E ) 
						{entry.m_value = parameter[1].m_value;}
						break;
					case 1:// rule E : E + T 
						{entry.m_value = parameter[0].m_value + " + " + parameter[1].m_value; printf ("%s\n", entry.m_value.c_str());}
						break;
					case 3:// rule T : T * F 
						{entry.m_value = parameter[0].m_value + " * " + parameter[1].m_value;}
						break;

					default:;
				}
*/
				break;

			}
	
			case dACCEPT: // 2 = accept
			{
				// program parced successfully, exit with successful code
				_ASSERTE (0);
				return true;
			}
			
			default:  
			{
				_ASSERTE (0);
				// syntact error parciing program
				//if (!ErrorHandler ("error")) {
				//}
				break;
			}
		}
	}
	return false;
}







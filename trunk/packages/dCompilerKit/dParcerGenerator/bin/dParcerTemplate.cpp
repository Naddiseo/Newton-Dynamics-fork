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
//Auto generated Parcer Generator class: $(className).cpp
//
$(userCode)
#include "$(className).h"
#include <dList.h>

#define MAX_USER_PARAM	64
/*
enum $(className)::ActionType
{
	ACCEPT,
	SHIFT,
	REDUCE
};
*/

class $(className)::dActionEntry
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

class $(className)::dGotoEntry
{
	public:
	dGotoEntry ()
		:m_value(0)
	{
	}
	dGotoEntry (unsigned val)
		:m_value(val)
	{
	}
	union {
		unsigned m_value;
		struct {
			short  m_token;
			short  m_nextState;
		};
	};
};



class $(className)::dStackPair
{
	public:
$(userVariableClass)
	dStackPair()
		:m_state(0), m_token(dToken (0)), m_value()
	{
	}

	int m_state;
	dToken m_token;
	dUserVariable m_value;
};


$(className)::$(className)()
{
}

$(className)::~$(className)()
{
}


bool $(className)::ErrorHandler (const string& line) const
{
	line;
	return false;
}

$(className)::dActionEntry $(className)::FindAction (const int* const actionList, int count, dToken token) const
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

$(className)::dGotoEntry dAssemblerParcer::FindGoto (const int* const gotoList, int count, dToken token) const
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


bool $(className)::Parce($(scannerClass)& scanner)
{
	dList<dStackPair> stack;
	static int actionsCount[] = {$(actionsCount)};
	static int actionsStart[] = {$(actionsStart)};
	static int actionTable[] = {$(actionTable)};

	static int gotoCount[] = {$(gotoCount)};
	static int gotoStart[] = {$(gotoStart)};
	static int gotoTable[] = {$(gotoTable)};

	const int lastToken = &(lastTerminalToken);

	stack.Append ();
	dToken token = dToken (scanner.NextToken());
	for (;;) {
		const dStackPair& stackTop = stack.GetLast()->GetInfo();
		int start = actionsStart[stackTop.m_state];
		int count = actionsCount[stackTop.m_state];
		dActionEntry action (FindAction (&actionTable[start], count, token));

		switch (action.m_stateType) 
		{
			case 0: // 0 = shift
			{
				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_token = dToken (action.m_token);
				entry.m_state = action.m_nextState;
				entry.m_value = dStackPair::dUserVariable (entry.m_token, scanner.GetTokenString());
				token = dToken (scanner.NextToken());
				if (token == -1) {
					token = dToken (0);
				}
				break;
			}

			case 1: // 1 = reduce
			{
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
				
				switch (entry.m_state) 
				{
					//do user semantic Actions
$(semanticActionsCode)
					default:;
				}

				break;

			}
	
			case 2: // 2 = accept
			{
				// program parce successfully, exit with successful code
				return true;
			}
			
			default:  // syntax grammar error
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


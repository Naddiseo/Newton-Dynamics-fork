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

enum $(className)::ActionType
{
	ACCEPT,
	SHIFT,
	REDUCE
};
/*
class $(className)::dActionEntry
{
	public:
	int m_nextState;
	int m_statesToPop;
	int m_actionCount;
	Token m_token;
	ActionType m_action;
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
			unsigned  m_token		:14;
			unsigned  m_stateType	: 2;  // 0 = shift, 1 = reduce, 2 = accept
			unsigned  m_nextState	:16;
		};
	};
};


class $(className)::dStackPair
{
	public:
$(userVariableClass)
	dStackPair()
		:m_state(0), m_token(Token (0)), m_value()
	{
	}

	int m_state;
	Token m_token;
	$(userVariable) m_value;
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

$(className)::dActionEntry $(className)::FindAction (const int* const actionList, int count, Token token) const
{
	for (int i = 0; i < count; i ++) {
		dActionEntry action (actionList[i]);
		if (Token(action.m_token) == token) {
			return action; 
		}
	}
	return dActionEntry(unsigned (-1));
}


int $(className)::Parce($(scannerClass)& scanner)
{

	dList<dStackPair> stack;

	static int actionsCount[] = {$(actionsCount)};
	static int actionsStart[] = {$(actionsStart)};
	static int actionTable[] = {$(actionTable)};


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
				_ASSERTE (0);
//				dStackPair& entry = stack.Append()->GetInfo();
//				entry.m_token = action->m_token;
//				entry.m_state = action->m_nextState;
//				entry.m_value = dStackPair::dUserVariable (entry.m_token, scanner.GetTokenString());
//				token = Token (scanner.NextToken());
				break;
			}

			case 1: // 1 = reduce
			{
/*
				_ASSERTE (0);
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
				const dActionEntry* const GotoAction = FindAction (&actionTable[actionStart], actionCount, action->m_token);

				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_token = GotoAction->m_token;
				entry.m_state = GotoAction->m_nextState;


				dStackPair::dUserVariable* params[MAX_USER_PARAM];
				_ASSERTE (GotoAction->m_statesToPop < sizeof (params)/ sizeof (params[0]));
				_ASSERTE (GotoAction->m_statesToPop < stack.GetCount());
				int index = GotoAction->m_statesToPop - 1;
				for (dList<dStackPair>::dListNode* node = stack.GetLast(); node; node = node->GetPrev()) {
					params[index] = &node->GetInfo().m_value;
					index --;
				}

				switch (entry.m_token) 
				{
					//do user semantic Action
					//$(semanticActionsCode);
				case 256:
					{
						break;
					}
				default:;
				}
*/
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


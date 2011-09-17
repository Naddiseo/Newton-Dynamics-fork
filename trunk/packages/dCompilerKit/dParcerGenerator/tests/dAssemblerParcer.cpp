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
//Auto generated Parcer Generator class: dAssemblerParcer.cpp
//

#include <dVirtualMachine.h>
#include "dAssemblerLexical.h"
#include "dAssemblerCompiler.h"
//
// Newton virtual machine assembler grammar
// based loosely on a subset and a hybrid between the MIPS R3000 and the Intel 386 instructions set 
//

#include "dAssemblerParcer.h"
#include <dList.h>

#define MAX_USER_PARAM	64

enum dAssemblerParcer::ActionType
{
	dSHIFT = 0,
	dREDUCE,
	dACCEPT,
	dERROR
};


class dAssemblerParcer::dActionEntry
{
	public:
	dActionEntry (short token, short stateType,	short nextState, short ruleSymbols, short ruleIndex)
		:m_token(token), m_stateType(stateType), m_nextState(nextState), m_ruleSymbols(ruleSymbols), m_ruleIndex(ruleIndex)
	{
	}

	short m_token;
	short m_stateType;// 0 = shift, 1 = reduce, 2 = accept
	short m_nextState;
	short m_ruleSymbols;
	short m_ruleIndex;
};

class dAssemblerParcer::dGotoEntry
{
	public:
	dGotoEntry (short token, short nextState)
		:m_token(token), m_nextState(nextState)
	{
	}

	short  m_token;
	short  m_nextState;
};



class dAssemblerParcer::dStackPair
{
	public:
	dStackPair()
		:m_state(0), m_token(dToken (0)), m_value()
	{
	}

	int m_state;
	dToken m_token;
	dUserVariable m_value;
};


dAssemblerParcer::dAssemblerParcer()
{
}

dAssemblerParcer::~dAssemblerParcer()
{
}


bool dAssemblerParcer::ErrorHandler (const string& line) const
{
	line;
	return false;
}

const dAssemblerParcer::dActionEntry* dAssemblerParcer::FindAction (const dActionEntry* const actionList, int count, dToken token) const
{

	int i0 = 0;
	int i1 = count - 1;
	while ((i1 - i0) >= 4) {
		int i = (i1 + i0 + 1)>>1;

		const dActionEntry& action = actionList[i];
		if (token <= dToken(action.m_token)) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		const dActionEntry& action = actionList[i];
		if (token == dToken(action.m_token)) {
			return& action;
		}
	}

	_ASSERT (0);
	return NULL;
}

const dAssemblerParcer::dGotoEntry* dAssemblerParcer::FindGoto (const dGotoEntry* const gotoList, int count, dToken token) const
{
	int i0 = 0;
	int i1 = count - 1;
	while ((i1 - i0) >= 4) {
		int i = (i1 + i0 + 1)>>1;

		const dGotoEntry& action = gotoList[i];
		if (token <= dToken(action.m_token)) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		const dGotoEntry& action = gotoList[i];
		if (token == dToken(action.m_token)) {
			return &action;
		}
	}

	_ASSERT (0);
	return NULL;
}


bool dAssemblerParcer::Parce(lextest1& scanner)
{
	dList<dStackPair> stack;
	static short actionsCount[] = {5, 1, 1, 3, 1, 3, 1, 3, 3, 1, 4, 4, 3, 2, 2, 2, 1, 1, 3, 3, 2};
	static short actionsStart[] = {0, 5, 6, 7, 10, 11, 14, 15, 18, 21, 22, 26, 30, 33, 35, 37, 39, 21, 40, 43, 46};
	static dActionEntry actionTable[] = {
					dActionEntry (0, 1, 1, 0, 4), dActionEntry (256, 0, 1, 0, 0), dActionEntry (258, 0, 2, 0, 0), dActionEntry (264, 1, 1, 0, 4), dActionEntry (263, 1, 1, 0, 4), 
					dActionEntry (60, 0, 9, 0, 0), 
					dActionEntry (257, 1, 7, 1, 13), 
					dActionEntry (0, 1, 2, 1, 6), dActionEntry (256, 1, 2, 1, 6), dActionEntry (267, 1, 2, 1, 6), 
					dActionEntry (257, 0, 10, 0, 0), 
					dActionEntry (0, 1, 2, 1, 5), dActionEntry (256, 1, 2, 1, 5), dActionEntry (267, 1, 2, 1, 5), 
					dActionEntry (0, 2, 0, 0, 0), 
					dActionEntry (0, 1, 1, 1, 2), dActionEntry (264, 1, 1, 1, 2), dActionEntry (263, 1, 1, 1, 2), 
					dActionEntry (0, 1, 0, 1, 1), dActionEntry (256, 0, 1, 0, 0), dActionEntry (258, 0, 2, 0, 0), 
					dActionEntry (257, 0, 13, 0, 0), 
					dActionEntry (0, 1, 6, 1, 14), dActionEntry (61, 1, 6, 1, 14), dActionEntry (256, 1, 6, 1, 14), dActionEntry (258, 1, 6, 1, 14), 
					dActionEntry (0, 1, 4, 2, 10), dActionEntry (61, 0, 16, 0, 0), dActionEntry (256, 1, 4, 2, 10), dActionEntry (258, 1, 4, 2, 10), 
					dActionEntry (0, 1, 1, 2, 3), dActionEntry (264, 1, 1, 2, 3), dActionEntry (263, 1, 1, 2, 3), 
					dActionEntry (46, 1, 6, 1, 14), dActionEntry (62, 1, 6, 1, 14), 
					dActionEntry (46, 0, 17, 0, 0), dActionEntry (62, 0, 1, 0, 0), 
					dActionEntry (46, 1, 5, 1, 8), dActionEntry (62, 1, 5, 1, 8), 
					dActionEntry (259, 0, 18, 0, 0), 
					dActionEntry (0, 1, 8, 1, 12), dActionEntry (256, 1, 8, 1, 12), dActionEntry (258, 1, 8, 1, 12), 
					dActionEntry (0, 1, 4, 4, 11), dActionEntry (256, 1, 4, 4, 11), dActionEntry (258, 1, 4, 4, 11), 
					dActionEntry (46, 1, 5, 3, 9), dActionEntry (62, 1, 5, 3, 9), 
			};

	static short gotoCount[] = {6, 0, 0, 0, 1, 0, 0, 0, 4, 2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0};
	static short gotoStart[] = {0, 6, 6, 6, 6, 7, 7, 7, 7, 11, 13, 13, 13, 13, 13, 13, 13, 14, 15, 15, 15};
	static dGotoEntry gotoTable[] = {
					dGotoEntry (264, 3), dGotoEntry (267, 4), dGotoEntry (263, 5), dGotoEntry (260, 6), dGotoEntry (262, 7), dGotoEntry (261, 8), 
					dGotoEntry (266, 11), dGotoEntry (264, 3), dGotoEntry (267, 4), dGotoEntry (263, 5), dGotoEntry (262, 12), 
					dGotoEntry (265, 14), dGotoEntry (266, 15), dGotoEntry (268, 19), dGotoEntry (266, 20), 
			};

	const int lastToken = 260;

	stack.Append ();
	dToken token = dToken (scanner.NextToken());
	for (;;) {
		const dStackPair& stackTop = stack.GetLast()->GetInfo();
		int start = actionsStart[stackTop.m_state];
		int count = actionsCount[stackTop.m_state];
		const dActionEntry* const action (FindAction (&actionTable[start], count, token));
		_ASSERTE (action);

		switch (action->m_stateType) 
		{
			case dSHIFT: 
			{
				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_token = dToken (action->m_token);
				entry.m_state = action->m_nextState;
				entry.m_value = dUserVariable (entry.m_token, scanner.GetTokenString());
				token = dToken (scanner.NextToken());
				if (token == -1) {
					token = dToken (0);
				}

				break;
			}

			case dREDUCE: 
			{
				dStackPair parameter[MAX_USER_PARAM];

				int reduceCount = action->m_ruleSymbols;
				_ASSERTE (reduceCount < sizeof (parameter) / sizeof (parameter[0]));

				for (int i = 0; i < reduceCount; i ++) {
					parameter[reduceCount - i - 1] = stack.GetLast()->GetInfo();
					stack.Remove (stack.GetLast());
				}

				const dStackPair& stackTop = stack.GetLast()->GetInfo();
				int start = gotoStart[stackTop.m_state];
				int count = gotoCount[stackTop.m_state];
				const dGotoEntry* const gotoEntry = FindGoto (&gotoTable[start], count, dToken (action->m_nextState + lastToken));

				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_state = gotoEntry->m_nextState;
				entry.m_token = dToken (gotoEntry->m_token);
				
				switch (action->m_ruleIndex) 
				{
					//do user semantic Actions
					case 13:// rule dataType : INT 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitDataType (parameter[0].m_value);}
						break;
					case 14:// rule literal : LITERAL 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitSymbol (parameter[0].m_value);}
						break;
					case 12:// rule constantExpression : INTEGER 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitIntegerConst (parameter[0].m_value);}
						break;

					default:;
				}

				break;

			}
	
			case dACCEPT: // 2 = accept
			{
				// program parced successfully, exit with successful code
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





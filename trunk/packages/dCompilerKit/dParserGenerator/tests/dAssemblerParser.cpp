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
//Auto generated Parser Generator class: dAssemblerParcer.cpp
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
		dToken actionToken (dToken(action.m_token));
		if (token <= actionToken) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		const dActionEntry& action = actionList[i];
		dToken actionToken (dToken(action.m_token));
		if (token == actionToken) {
			return& action;
		}
	}

	_ASSERT (0);
	return NULL;
}

const dAssemblerParcer::dGotoEntry* dAssemblerParser::FindGoto (const dGotoEntry* const gotoList, int count, dToken token) const
{
	int i0 = 0;
	int i1 = count - 1;
	while ((i1 - i0) >= 4) {
		int i = (i1 + i0 + 1)>>1;

		const dGotoEntry& action = gotoList[i];
		dToken actionToken (dToken(action.m_token));
		if (token <= actionToken) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		const dGotoEntry& action = gotoList[i];
		dToken actionToken (dToken(action.m_token));
		if (token == actionToken) {
			return &action;
		}
	}

	_ASSERT (0);
	return NULL;
}


bool dAssemblerParcer::Parce(lextest1& scanner)
{
	dList<dStackPair> stack;
	static short actionsCount[] = {4, 1, 1, 1, 4, 1, 4, 4, 1, 4, 4, 3, 4, 1, 5, 5, 4, 3, 0, 2, 2, 2, 1, 4, 0, 4, 1, 4, 0, 1, 4, 4, 4, 4, 2};
	static short actionsStart[] = {0, 4, 5, 6, 7, 11, 12, 16, 20, 21, 25, 29, 32, 36, 37, 42, 47, 51, 54, 54, 56, 58, 60, 61, 54, 65, 69, 70, 54, 36, 74, 78, 82, 86, 90};
	static dActionEntry actionTable[] = {
					dActionEntry (0, 1, 0, 0, 2), dActionEntry (259, 0, 1, 0, 0), dActionEntry (256, 0, 2, 0, 0), dActionEntry (261, 0, 3, 0, 0), 
					dActionEntry (257, 0, 11, 0, 0), 
					dActionEntry (60, 0, 13, 0, 0), 
					dActionEntry (257, 1, 8, 1, 14), 
					dActionEntry (0, 1, 2, 1, 6), dActionEntry (259, 1, 2, 1, 6), dActionEntry (256, 1, 2, 1, 6), dActionEntry (261, 1, 2, 1, 6), 
					dActionEntry (257, 0, 14, 0, 0), 
					dActionEntry (0, 1, 2, 1, 7), dActionEntry (259, 1, 2, 1, 7), dActionEntry (256, 1, 2, 1, 7), dActionEntry (261, 1, 2, 1, 7), 
					dActionEntry (0, 1, 2, 1, 5), dActionEntry (259, 1, 2, 1, 5), dActionEntry (256, 1, 2, 1, 5), dActionEntry (261, 1, 2, 1, 5), 
					dActionEntry (0, 2, 0, 0, 0), 
					dActionEntry (0, 1, 1, 1, 3), dActionEntry (259, 1, 1, 1, 3), dActionEntry (256, 1, 1, 1, 3), dActionEntry (261, 1, 1, 1, 3), 
					dActionEntry (0, 1, 0, 1, 1), dActionEntry (259, 0, 1, 0, 0), dActionEntry (256, 0, 2, 0, 0), dActionEntry (261, 0, 3, 0, 0), 
					dActionEntry (266, 1, 7, 1, 15), dActionEntry (265, 1, 7, 1, 15), dActionEntry (264, 1, 7, 1, 15), 
					dActionEntry (266, 1, 10, 0, 18), dActionEntry (265, 1, 10, 0, 18), dActionEntry (264, 1, 10, 0, 18), dActionEntry (258, 0, 17, 0, 0), 
					dActionEntry (257, 0, 19, 0, 0), 
					dActionEntry (0, 1, 7, 1, 15), dActionEntry (61, 1, 7, 1, 15), dActionEntry (259, 1, 7, 1, 15), dActionEntry (256, 1, 7, 1, 15), dActionEntry (261, 1, 7, 1, 15), 
					dActionEntry (0, 1, 4, 2, 11), dActionEntry (61, 0, 22, 0, 0), dActionEntry (259, 1, 4, 2, 11), dActionEntry (256, 1, 4, 2, 11), dActionEntry (261, 1, 4, 2, 11), 
					dActionEntry (0, 1, 1, 2, 4), dActionEntry (259, 1, 1, 2, 4), dActionEntry (256, 1, 1, 2, 4), dActionEntry (261, 1, 1, 2, 4), 
					dActionEntry (266, 1, 10, 1, 17), dActionEntry (265, 1, 10, 1, 17), dActionEntry (264, 1, 10, 1, 17), 
					
					dActionEntry (46, 1, 7, 1, 15), dActionEntry (62, 1, 7, 1, 15), 
					dActionEntry (46, 0, 29, 0, 0), dActionEntry (62, 0, 2, 0, 0), 
					dActionEntry (46, 1, 6, 1, 9), dActionEntry (62, 1, 6, 1, 9), 
					dActionEntry (262, 0, 30, 0, 0), 
					dActionEntry (266, 1, 13, 1, 22), dActionEntry (265, 1, 13, 1, 22), dActionEntry (260, 1, 13, 1, 22), dActionEntry (264, 1, 13, 1, 22), 
					dActionEntry (266, 1, 11, 1, 20), dActionEntry (265, 1, 11, 1, 20), dActionEntry (260, 1, 11, 1, 20), dActionEntry (264, 1, 11, 1, 20), 
					dActionEntry (260, 0, 32, 0, 0), 
					dActionEntry (266, 1, 13, 1, 23), dActionEntry (265, 1, 13, 1, 23), dActionEntry (260, 1, 13, 1, 23), dActionEntry (264, 1, 13, 1, 23), 
					dActionEntry (0, 1, 9, 1, 13), dActionEntry (259, 1, 9, 1, 13), dActionEntry (256, 1, 9, 1, 13), dActionEntry (261, 1, 9, 1, 13), 
					dActionEntry (0, 1, 4, 4, 12), dActionEntry (259, 1, 4, 4, 12), dActionEntry (256, 1, 4, 4, 12), dActionEntry (261, 1, 4, 4, 12), 
					dActionEntry (0, 1, 5, 5, 16), dActionEntry (259, 1, 5, 5, 16), dActionEntry (256, 1, 5, 5, 16), dActionEntry (261, 1, 5, 5, 16), 
					dActionEntry (266, 1, 11, 2, 21), dActionEntry (265, 1, 11, 2, 21), dActionEntry (260, 1, 11, 2, 21), dActionEntry (264, 1, 11, 2, 21), 
					dActionEntry (46, 1, 6, 3, 10), dActionEntry (62, 1, 6, 3, 10), 
			};

	static short gotoCount[] = {7, 1, 0, 0, 0, 1, 0, 0, 0, 0, 5, 0, 1, 2, 0, 0, 0, 0, 6, 0, 0, 0, 1, 0, 1, 0, 5, 0, 1, 1, 0, 0, 0, 0, 0};
	static short gotoStart[] = {0, 7, 8, 8, 8, 8, 9, 9, 9, 9, 9, 14, 14, 15, 17, 17, 17, 17, 17, 23, 23, 23, 23, 24, 24, 25, 25, 30, 30, 31, 32, 32, 32, 32, 32};
	static dGotoEntry gotoTable[] = {
					dGotoEntry (267, 8), dGotoEntry (268, 10), dGotoEntry (269, 9), dGotoEntry (270, 7), dGotoEntry (271, 4), dGotoEntry (272, 6), dGotoEntry (275, 5), 
					dGotoEntry (274, 12), dGotoEntry (274, 15), dGotoEntry (269, 16), dGotoEntry (270, 7), dGotoEntry (271, 4), dGotoEntry (272, 6), dGotoEntry (275, 5), 
					dGotoEntry (277, 18), dGotoEntry (273, 20), dGotoEntry (274, 21), dGotoEntry (278, 26), dGotoEntry (280, 25), dGotoEntry (281, 23), dGotoEntry (282, 27), dGotoEntry (283, 24), dGotoEntry (285, 28), 
					dGotoEntry (276, 31), dGotoEntry (284, 24), dGotoEntry (280, 33), dGotoEntry (281, 23), dGotoEntry (282, 27), dGotoEntry (283, 24), dGotoEntry (285, 28), 
					dGotoEntry (284, 28), dGotoEntry (274, 34)};

	const int lastToken = 267;

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
				entry.m_state = action->m_nextState;
				entry.m_value = dUserVariable (entry.m_token, scanner.GetTokenString());
				token = dToken (scanner.NextToken());
				entry.m_token = token;
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
					case 14:// rule dataType : INT 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitDataType (parameter[0].m_value);}
						break;
					case 15:// rule literal : LITERAL 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitSymbol (parameter[0].m_value);}
						break;
					case 11:// rule dataDeclaration : dataType literal 
						{((dAssemblerCompiler*)this)->EmitUnInitilizedDataDeclaration (parameter[0].m_value, parameter[1].m_value);}
						break;
					case 13:// rule constantExpression : INTEGER 
						{entry.m_value = ((dAssemblerCompiler*)this)->EmitIntegerConst (parameter[0].m_value);}
						break;
					case 12:// rule dataDeclaration : dataType literal = constantExpression 
						{((dAssemblerCompiler*)this)->EmitInitilizedDataDeclaration (parameter[0].m_value, parameter[1].m_value, parameter[3].m_value);}
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





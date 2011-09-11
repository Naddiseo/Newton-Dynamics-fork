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
// based loosely on the MIPS R3000 and the Intel 386 instructions sets 
//

#include "dAssemblerParcer.h"
#include <dList.h>

#define MAX_USER_PARAM	64
/*
enum dAssemblerParcer::ActionType
{
	ACCEPT,
	SHIFT,
	REDUCE
};
*/

class dAssemblerParcer::dActionEntry
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

class dAssemblerParcer::dGotoEntry
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



class dAssemblerParcer::dStackPair
{
	public:
	class dUserVariable: public string 
	{	
		public:
		dUserVariable ()
			:string(), m_token (dToken(0))
		{
		}

		dUserVariable (dToken token, const char* const text)
			:string(text), m_token (token)
		{
		}

		dToken m_token;
	};



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

dAssemblerParcer::dActionEntry dAssemblerParcer::FindAction (const int* const actionList, int count, dToken token) const
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

dAssemblerParcer::dGotoEntry dAssemblerParcer::FindGoto (const int* const gotoList, int count, dToken token) const
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


bool dAssemblerParcer::Parce(dAssemblerLexical& scanner)
{
	dList<dStackPair> stack;
	static int actionsCount[] = {2, 2, 1, 6, 2, 6, 2, 2, 2, 1, 6, 6};
	static int actionsStart[] = {0, 2, 4, 5, 11, 13, 19, 21, 23, 25, 26, 32};
	static int actionTable[] = {0x100a0, 0x14400, 0x2, 0x180ac, 0x1c0a8, 0x4004001, 0x40040a1, 0x40040a5, 0x40040a9, 0x40040ad, 0x4004401, 0x100a0, 0x14400, 0x4008001, 0x40080a1, 0x40080a5, 0x40080a9, 0x40080ad, 0x4008401, 0x100a0, 0x14400, 0x100a0, 0x14400, 0x2c0a4, 0x180ac, 0x1c0a8, 0xc004001, 0xc0040a1, 0xc0040a5, 0xc0040a9, 0xc0040ad, 0xc004401, 0xc008001, 0xc0080a1, 0xc0080a5, 0xc0080a9, 0xc0080ad, 0xc008401};

	static int gotoCount[] = {3, 0, 0, 0, 3, 0, 2, 1, 0, 0, 0, 0};
	static int gotoStart[] = {0, 3, 3, 3, 3, 6, 6, 8, 9, 9, 9, 9};
	static int gotoTable[] = {0x10101, 0x30103, 0x20102, 0x80101, 0x30103, 0x20102, 0x30103, 0x90102, 0xa0103};

	const int lastToken = 257;

int xxx = -1;
	stack.Append ();
	dToken token = dToken (scanner.NextToken());
	for (;;) {

xxx ++;
if (xxx == 4)
xxx *=1;

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
					parameter[i] = stack.GetLast()->GetInfo();
					stack.Remove (stack.GetLast());
				}

				const dStackPair& stackTop = stack.GetLast()->GetInfo();
				int start = gotoStart[stackTop.m_state];
				int count = gotoCount[stackTop.m_state];
				dGotoEntry gotoEntry (FindGoto (&gotoTable[start], count, dToken (action.m_nextState + lastToken)));

				dStackPair& entry = stack.Append()->GetInfo();
				entry.m_state = gotoEntry.m_nextState;
				entry.m_token = dToken (gotoEntry.m_token);
				
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
				// successfully parced grammar, exit with successful code
				return true;
			}
			
			default:  // syntax grammar error
			{
				_ASSERTE (0);
				// failed parcing gramamr, break with error code
				// error
				//if (!ErrorHandler ("error")) {
				//}
				break;
			}
		}
	}

	return false;
}







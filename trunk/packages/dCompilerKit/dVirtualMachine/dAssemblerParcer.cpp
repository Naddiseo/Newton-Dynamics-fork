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
// based loosely on a subset of the MIPS R3000 and the Intel 386 instructions set 
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

	class dUserVariable
	{
		public:
		dUserVariable () 
			:m_token (dToken (0)), m_data("")
		{
		}
		
		
		dUserVariable (dToken token, const char* const text)
			:m_token(token), m_data (text) 
		{
		}
		dToken m_token;
		string m_data;
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
	static int actionsCount[] = {2, 2, 2, 1, 3, 3, 3, 2, 2, 2, 3, 3, 3};
	static int actionsStart[] = {0, 2, 4, 6, 7, 10, 13, 16, 18, 20, 22, 25, 28};
	static int actionTable[] = {0x40a0, 0x18400, 0x40a0, 0x18400, 0x4000001, 0x200ac, 0x2, 0x4008001, 0x40080a9, 0x40080ad, 0x4004001, 0x240a8, 0x40040ad, 0x400c001, 0x400c0a9, 0x400c0ad, 0x280a4, 0x200ac, 0x40a0, 0x18400, 0x40a0, 0x18400, 0xc00c001, 0xc00c0a9, 0xc00c0ad, 0xc004001, 0x240a8, 0xc0040ad, 0xc008001, 0xc0080a9, 0xc0080ad};

	static int gotoCount[] = {4, 3, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0};
	static int gotoStart[] = {0, 4, 7, 7, 7, 7, 7, 7, 7, 9, 10, 10, 10};
	static int gotoTable[] = {0x20102, 0x30101, 0x40104, 0x50103, 0x70102, 0x40104, 0x50103, 0x40104, 0xb0103, 0xc0104};

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
					case 2:
						{printf ("%s\n", parameter[0].m_value.m_data.c_str());}
						break;
					case 4:
						{entry.m_value = parameter[0].m_value;}
						break;
					case 5:
						{entry.m_value = parameter[0].m_value;}
						break;
					case 6:
						{entry.m_value = parameter[0].m_value;}
						break;
					case 10:
						{entry.m_value = parameter[1].m_value;}
						break;
					case 11:
						{entry.m_value.m_data = parameter[0].m_value.m_data + " + " + parameter[2].m_value.m_data;}
						break;
					case 12:
						{entry.m_value.m_data = parameter[0].m_value.m_data + " * " + parameter[2].m_value.m_data;}
						break;

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





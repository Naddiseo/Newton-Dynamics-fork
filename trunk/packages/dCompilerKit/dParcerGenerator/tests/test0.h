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
//Auto generated Parcer Generator class: test0.h
//

#ifndef __test0_h__
#define __test0_h__

#ifdef _MSC_VER
#pragma warning (disable: 4702) // warning C4702: unreachable code
#pragma warning (disable: 4100) // warning C4100: unreferenced formal parameter
#pragma warning (disable: 4201) // warning C4201: nonstandard extension used : nameless struct/union
#endif


#include <string>
using namespace std;

class lextest1;

class test0
{
	public:
	enum Token
	{
		id = 256
	};

	class dActionEntry
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

	 

	enum ActionType;
	class dStackPair;
	class dActionEntry;
	class dUserVariable;

	test0();
	virtual ~test0();
	virtual int Parce(lextest1& scanner);

	virtual bool ErrorHandler (const string& line) const;

	private:
	const dActionEntry* FindAction (const dActionEntry* const list, int count, Token token) const;
};

#endif

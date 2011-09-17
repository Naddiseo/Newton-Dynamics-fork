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
//Auto generated Parcer Generator class: dAssemblerParcer.h
//

#ifndef __dAssemblerParcer_h__
#define __dAssemblerParcer_h__

#ifdef _MSC_VER
#pragma warning (disable: 4702) // warning C4702: unreachable code
#pragma warning (disable: 4100) // warning C4100: unreferenced formal parameter
#pragma warning (disable: 4201) // warning C4201: nonstandard extension used : nameless struct/union
#endif


#include <string>
using namespace std;

class lextest1;

class dAssemblerParcer
{
	public:
	enum dToken
	{
		IMPORT = 256,
		LITERAL
	};

	enum ActionType;
	class dStackPair;
	class dGotoEntry;
	class dActionEntry;
	
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


	dAssemblerParcer();
	virtual ~dAssemblerParcer();
	virtual bool Parce(lextest1& scanner);

	virtual bool ErrorHandler (const string& line) const;

	private:
	const dGotoEntry* FindGoto (const dGotoEntry* const gotoList, int count, dToken token) const;
	const dActionEntry* FindAction (const dActionEntry* const list, int count, dToken token) const;
};

#endif

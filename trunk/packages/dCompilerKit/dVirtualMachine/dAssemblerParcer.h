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

#include <string>
using namespace std;

class dAssemblerLexical;

class dAssemblerParcer
{
	public:
	enum Token
	{
		id = 256
	};

	enum ActionType;
	class dStackPair;
	class dActionEntry;
	class dUserVariable;

	dAssemblerParcer();
	virtual ~dAssemblerParcer();
	virtual int Parce(dAssemblerLexical& scanner);

	virtual bool ErrorHandler (const string& line) const;

	private:
	const dActionEntry* FindAction (const dActionEntry* const list, int count, Token token) const;
};

#endif

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
		END = 256,
		INCLUDE,
		LITERAL,
		DATASEGMENT,
		BYTE,
		WORD,
		DWORD,
		DOUBLE,
		OFFSET,
		INTERGER,
		FLOAT,
		CODESEGMENT,
		BEGIN,
		PUBLIC,
		LOADI,
		REGISTER,
		ADD,
		RET
	};

	enum ActionType;
	class dStackPair;
	class dActionEntry;

	dAssemblerParcer();
	virtual ~dAssemblerParcer();
	virtual int Parce(dAssemblerLexical& scanner);

	virtual bool ErrorHandler (const string& line) const;

	private:
	const dActionEntry* FindAction (const dActionEntry* const list, int count, Token token) const;
};

#endif

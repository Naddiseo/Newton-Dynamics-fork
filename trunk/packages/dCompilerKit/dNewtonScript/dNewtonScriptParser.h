/* Copyright (c) <2009> <Newton Game Dynamics>
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
//Auto generated Parser Generator class: dNewtonScriptParser.h
//

#ifndef __dNewtonScriptParser_h__
#define __dNewtonScriptParser_h__

#ifdef _MSC_VER
#pragma warning (disable: 4702) // warning C4702: unreachable code
#pragma warning (disable: 4100) // warning C4100: unreferenced formal parameter
#pragma warning (disable: 4201) // warning C4201: nonstandard extension used : nameless struct/union
#endif


#include <string>
using namespace std;

class dNewtonScriptLexical;

class dNewtonScriptParser
{
	public:
	enum dToken
	{
		ACCEPTING_TOKEN = 255, 
		NATIVE = 256, 
		CONSTANT,
		INTEGER_VALUE,
		IMPORT,
		IMPORT_FILE_NAME,
		PRIVATE,
		CLASS,
		IDENTIFIER,
		EXTENDS,
		GUI,
		BOOLEAN,
		BYTE,
		SHORT,
		INT,
		LONG,
		UNSIGNED,
		FLOAT,
		DOUBLE,
		FOR,
		WHILE,
		DO,
		IF,
		ELSE,
		SWITCH,
		CASE,
		DEFAULT,
		CONTINUE,
		BREAK
	};

	enum ActionType;
	class dStackPair;
	class dGotoEntry;
	class dActionEntry;
	
	class dUserVariable
	{
		public:
		dUserVariable () 
			:m_token (dToken (0)), m_semanticValue(0), m_data("")
		{
		}
		
		
		dUserVariable (dToken token, const char* const text)
			:m_token(token), m_semanticValue(0), m_data (text) 
		{
		}
		dToken m_token;
		int m_semanticValue;
		string m_data;
	};


	dNewtonScriptParser();
	virtual ~dNewtonScriptParser();
	virtual bool Parse(dNewtonScriptLexical& scanner);

	virtual bool ErrorHandler (const string& line) const;

	private:
	const dGotoEntry* FindGoto (const dGotoEntry* const gotoList, int count, dToken token) const;
	const dActionEntry* FindAction (const dActionEntry* const list, int count, dToken token) const;
};

#endif

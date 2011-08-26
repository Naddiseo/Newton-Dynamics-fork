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
//Auto generated Lexical Analyzer class: $(className).h
//

#ifndef __$(className)_h__
#define __$(className)_h__

#include <string>
using namespace std;

class $(className)
{
	public:
	$(className)(const char* const data);
	virtual ~$(className)();

	virtual int NextToken ();
	virtual const char* GetTokenString () const;
	

	protected:
	virtual char NextChar ();
	virtual void GetLexString ();
	virtual int NextPattern ();
	virtual bool IsCharInSet (int ch, const char* const set) const;

	// local lexical variables
	int m_token;
	int m_state;
	int m_lastState;
	int m_startState;
	int m_index;
	int m_startIndex;
	const char* m_data;
	string m_tokenString;
};
#endif

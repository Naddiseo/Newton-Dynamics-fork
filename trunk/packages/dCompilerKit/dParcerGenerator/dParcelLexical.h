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
//Auto generated Lexical Analyzer class: dParcelLexical.h
//

#ifndef __dParcelLexical_h__
#define __dParcelLexical_h__

#include <string>
using namespace std;

class dParcelLexical
{
	public:
	dParcelLexical(const char* const data);
	virtual ~dParcelLexical();

	int NextToken ();
	const char* GetTokenString () const;
	

	protected:
	char NextChar ();
	void GetLexString ();

	int NextPattern ();
	bool IsCharInSet (int ch, const char* const set) const;

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

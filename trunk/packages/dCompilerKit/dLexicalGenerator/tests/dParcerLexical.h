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
//Auto generated Lexical Analyzer class: dParcerLexical.h
//

#ifndef __dParcerLexical_h__
#define __dParcerLexical_h__

#include <string>
using namespace std;

class dParcerLexical
{
	enum dTranstionType
	{
		m_infoIsCharacter = 0,
		m_infoIsCharacterSet,
		m_infoIsInitBalanceCounter,
		m_infoIsIncrementBalanceCounter,
		m_infoIsDecrementBalanceCounter,
	};

	struct dTransitionInfo
	{
		short m_info;
		short m_type;
		short m_nextState;
	};


	public:
	dParcerLexical(const char* const data);
	virtual ~dParcerLexical();

	virtual int NextToken ();
	virtual const char* GetTokenString () const;
	

	protected:
	virtual char NextChar ();
	virtual void GetLexString ();
//	virtual int NextPattern ();

	bool IsCharInSet (char ch, const char* const set, int setSize) const;

	// local lexical variables
	string m_tokenString;
	const char* m_data;
	int m_index;
	int m_startIndex;
//	int m_token;
//	int m_state;
//	int m_lastState;
//	int m_startState;
	
//	int m_startIndex;
	
};
#endif

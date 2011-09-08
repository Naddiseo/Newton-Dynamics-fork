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
//Auto generated Lexical Analyzer class: dAssemblerLexical.h
//

#ifndef __dAssemblerLexical_h__
#define __dAssemblerLexical_h__

#include <string>
using namespace std;

class dAssemblerLexical
{
	enum dStateType
	{
		m_extiState = 'a',
		m_itermediateState,
	};
	enum dTranstionType
	{
		m_infoIsCharacter = 0,
		m_infoIsCharacterSet,
	};

	struct dTransitionInfo
	{
		short m_info;
		short m_type;
		short m_nextState;
	};


	public:
	dAssemblerLexical(const char* const data);
	virtual ~dAssemblerLexical();

	virtual int NextToken ();

	const char* GetTokenString() const
	{
		return m_tokenString.c_str();
	}

	const char* GetNextBuffer() const
	{
		return &m_data[m_index];
	}

	protected:
	int GetLineNumber () const
	{
		return m_lineNumber;
	}

	char NextChar ()
	{
		char ch = m_data[m_index++];
		if (ch == '\n') {
			m_lineNumber ++;
		}
		return ch;
	}

	void UnGetChar ()
	{
		char ch = m_data[--m_index];
		if (ch == '\n') {
			m_lineNumber --;
		}
	}

	void ReadBalancedExpresion (char open, char close);

	void GetLexString ();
	bool IsCharInSet (char ch, const char* const set, int setSize) const;

	// local lexical variables
	string m_tokenString;
	const char* m_data;
	int m_index;
	int m_startIndex;
	int m_lineNumber;
	
};
#endif

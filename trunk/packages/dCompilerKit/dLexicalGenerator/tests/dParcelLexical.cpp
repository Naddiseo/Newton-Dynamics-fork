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
//Auto generated Lexical Analyzer class: dParcelLexical.cpp
//


#include <dParcelCompiler.h>

#include "dParcelLexical.h"

dParcelLexical::dParcelLexical(const char* const data)
	:m_token(0)
	,m_state(0)
	,m_lastState(161)
	,m_startState(0)
	,m_index(0)
	,m_startIndex(0)
	,m_data(data)
	,m_tokenString("")
{
}

dParcelLexical::~dParcelLexical()
{
}

char dParcelLexical::NextChar()
{
	return m_data[m_index++];
}

const char* dParcelLexical::GetTokenString() const
{
	return m_tokenString.c_str();
}

bool dParcelLexical::IsCharInSet (int ch, const char* const set) const
{
	for (int i = 0; i < set[i]; i ++) {
		if (ch == set[i]) {
			 return true;
		}
	}
	return false;
}

void dParcelLexical::GetLexString ()
{
	int length = m_index - m_startIndex;
	m_tokenString = string (&m_data[m_startIndex], length);
	m_startIndex = m_index;
	m_state = NextPattern();
}

int dParcelLexical::NextPattern ()
{
//	m_index = m_startIndex;
//	switch (m_startState)
//	{
//$(n___extTokenStart)
//	}
//	return m_startState;

	static int nextState[] = {2, 0, 4, 0, 6, 0, 8, 0, 12, 0, 0, 0, 16, 0, 0, 0, 20, 0, 0, 0, 24, 0, 0, 0, 28, 0, 0, 0, 32, 0, 0, 0, 36, 0, 0, 0, 40, 0, 0, 0, 44, 0, 0, 0, 48, 0, 0, 0, 52, 0, 0, 0, 56, 0, 0, 0, 60, 0, 0, 0, 64, 0, 0, 0, 68, 0, 0, 0, 72, 0, 0, 0, 76, 0, 0, 0, 80, 0, 0, 0, 84, 0, 0, 0, 88, 0, 0, 0, 92, 0, 0, 0, 96, 0, 0, 0, 100, 0, 0, 0, 104, 0, 0, 0, 108, 0, 0, 0, 111, 0, 0, 118, 0, 0, 0, 0, 0, 0, 124, 0, 0, 0, 0, 0, 131, 0, 0, 0, 0, 0, 0, 138, 0, 0, 0, 0, 0, 0, 141, 0, 0, 150, 0, 0, 0, 0, 0, 0, 0, 0, 152, 0, 161, 0, 0, 0, 0, 0, 0, 0, 0, };

	m_index = m_startIndex;
	_ASSERTE (sizeof (nextState) / sizeof (nextState[0]) < nextState[m_startState]);
	return  nextState[m_startState];
}

int dParcelLexical::NextToken ()
{
	//static strings patterns
	static char text_0[] = {9, 10, 13, 32, 0};
	static char text_1[] = {124, 0};
	static char text_2[] = {58, 0};
	static char text_3[] = {59, 0};
	static char text_4[] = {65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};
	static char text_5[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};
	static char text_6[] = {37, 0};
	static char text_7[] = {123, 0};
	static char text_8[] = {10, 13, 0};
	static char text_9[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_10[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 126, 127, 0};
	static char text_11[] = {125, 0};
	static char text_12[] = {47, 0};
	static char text_13[] = {42, 0};
	static char text_14[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_15[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_16[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};

	
	m_state = 0;
	m_startState = 0;
	m_startIndex = m_index;
	while (m_state != m_lastState)
	{
		switch (m_state)
		{
			// [ \t\n\r]+
			case 0:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_0)) m_state = 1;
				else m_state = NextPattern();
				break;
			}
			case 1:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_0)) m_state = 1;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{}
				}
				break;
			}
			// [|]
			case 2:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_1)) m_state = 3;
				else m_state = NextPattern();
				break;
			}
			case 3:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return(dParcelCompiler::OR); }
				}
				break;
			}
			// [:]
			case 4:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_2)) m_state = 5;
				else m_state = NextPattern();
				break;
			}
			case 5:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return(dParcelCompiler::COLOM); }
				}
				break;
			}
			// [;]
			case 6:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_3)) m_state = 7;
				else m_state = NextPattern();
				break;
			}
			case 7:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return(dParcelCompiler::SIMICOLOM); }
				}
				break;
			}
			// "';'"
			case 8:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 9;
				else m_state = NextPattern();
				break;
			}
			case 9:
			{
				char ch = NextChar();
				if (ch == 59) m_state = 10;
				else m_state = NextPattern();
				break;
			}
			case 10:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 11;
				else m_state = NextPattern();
				break;
			}
			case 11:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return(';'); }
				}
				break;
			}
			// "'{'"
			case 12:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 13;
				else m_state = NextPattern();
				break;
			}
			case 13:
			{
				char ch = NextChar();
				if (ch == 123) m_state = 14;
				else m_state = NextPattern();
				break;
			}
			case 14:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 15;
				else m_state = NextPattern();
				break;
			}
			case 15:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('{'); }
				}
				break;
			}
			// "'}'"
			case 16:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 17;
				else m_state = NextPattern();
				break;
			}
			case 17:
			{
				char ch = NextChar();
				if (ch == 125) m_state = 18;
				else m_state = NextPattern();
				break;
			}
			case 18:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 19;
				else m_state = NextPattern();
				break;
			}
			case 19:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('}'); }
				}
				break;
			}
			// "','"
			case 20:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 21;
				else m_state = NextPattern();
				break;
			}
			case 21:
			{
				char ch = NextChar();
				if (ch == 44) m_state = 22;
				else m_state = NextPattern();
				break;
			}
			case 22:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 23;
				else m_state = NextPattern();
				break;
			}
			case 23:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return(','); }
				}
				break;
			}
			// "':'"
			case 24:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 25;
				else m_state = NextPattern();
				break;
			}
			case 25:
			{
				char ch = NextChar();
				if (ch == 58) m_state = 26;
				else m_state = NextPattern();
				break;
			}
			case 26:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 27;
				else m_state = NextPattern();
				break;
			}
			case 27:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return(':'); }
				}
				break;
			}
			// "'='"
			case 28:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 29;
				else m_state = NextPattern();
				break;
			}
			case 29:
			{
				char ch = NextChar();
				if (ch == 61) m_state = 30;
				else m_state = NextPattern();
				break;
			}
			case 30:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 31;
				else m_state = NextPattern();
				break;
			}
			case 31:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('='); }
				}
				break;
			}
			// "'&'"
			case 32:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 33;
				else m_state = NextPattern();
				break;
			}
			case 33:
			{
				char ch = NextChar();
				if (ch == 38) m_state = 34;
				else m_state = NextPattern();
				break;
			}
			case 34:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 35;
				else m_state = NextPattern();
				break;
			}
			case 35:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('&'); }
				}
				break;
			}
			// "'!'"
			case 36:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 37;
				else m_state = NextPattern();
				break;
			}
			case 37:
			{
				char ch = NextChar();
				if (ch == 33) m_state = 38;
				else m_state = NextPattern();
				break;
			}
			case 38:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 39;
				else m_state = NextPattern();
				break;
			}
			case 39:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('!'); }
				}
				break;
			}
			// "'~'"
			case 40:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 41;
				else m_state = NextPattern();
				break;
			}
			case 41:
			{
				char ch = NextChar();
				if (ch == 126) m_state = 42;
				else m_state = NextPattern();
				break;
			}
			case 42:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 43;
				else m_state = NextPattern();
				break;
			}
			case 43:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('~'); }
				}
				break;
			}
			// "'-'"
			case 44:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 45;
				else m_state = NextPattern();
				break;
			}
			case 45:
			{
				char ch = NextChar();
				if (ch == 45) m_state = 46;
				else m_state = NextPattern();
				break;
			}
			case 46:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 47;
				else m_state = NextPattern();
				break;
			}
			case 47:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('-'); }
				}
				break;
			}
			// "'%'"
			case 48:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 49;
				else m_state = NextPattern();
				break;
			}
			case 49:
			{
				char ch = NextChar();
				if (ch == 37) m_state = 50;
				else m_state = NextPattern();
				break;
			}
			case 50:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 51;
				else m_state = NextPattern();
				break;
			}
			case 51:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('%'); }
				}
				break;
			}
			// "'<'"
			case 52:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 53;
				else m_state = NextPattern();
				break;
			}
			case 53:
			{
				char ch = NextChar();
				if (ch == 60) m_state = 54;
				else m_state = NextPattern();
				break;
			}
			case 54:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 55;
				else m_state = NextPattern();
				break;
			}
			case 55:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('<'); }
				}
				break;
			}
			// "'>'"
			case 56:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 57;
				else m_state = NextPattern();
				break;
			}
			case 57:
			{
				char ch = NextChar();
				if (ch == 62) m_state = 58;
				else m_state = NextPattern();
				break;
			}
			case 58:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 59;
				else m_state = NextPattern();
				break;
			}
			case 59:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('>'); }
				}
				break;
			}
			// "'/'"
			case 60:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 61;
				else m_state = NextPattern();
				break;
			}
			case 61:
			{
				char ch = NextChar();
				if (ch == 47) m_state = 62;
				else m_state = NextPattern();
				break;
			}
			case 62:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 63;
				else m_state = NextPattern();
				break;
			}
			case 63:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('/'); }
				}
				break;
			}
			// "'^'"
			case 64:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 65;
				else m_state = NextPattern();
				break;
			}
			case 65:
			{
				char ch = NextChar();
				if (ch == 94) m_state = 66;
				else m_state = NextPattern();
				break;
			}
			case 66:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 67;
				else m_state = NextPattern();
				break;
			}
			case 67:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('^'); }
				}
				break;
			}
			// "'\.'"
			case 68:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 69;
				else m_state = NextPattern();
				break;
			}
			case 69:
			{
				char ch = NextChar();
				if (ch == 46) m_state = 70;
				else m_state = NextPattern();
				break;
			}
			case 70:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 71;
				else m_state = NextPattern();
				break;
			}
			case 71:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('.'); }
				}
				break;
			}
			// "'\|'"
			case 72:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 73;
				else m_state = NextPattern();
				break;
			}
			case 73:
			{
				char ch = NextChar();
				if (ch == 124) m_state = 74;
				else m_state = NextPattern();
				break;
			}
			case 74:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 75;
				else m_state = NextPattern();
				break;
			}
			case 75:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('|'); }
				}
				break;
			}
			// "'\?'"
			case 76:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 77;
				else m_state = NextPattern();
				break;
			}
			case 77:
			{
				char ch = NextChar();
				if (ch == 63) m_state = 78;
				else m_state = NextPattern();
				break;
			}
			case 78:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 79;
				else m_state = NextPattern();
				break;
			}
			case 79:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('?'); }
				}
				break;
			}
			// "'\\'"
			case 80:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 81;
				else m_state = NextPattern();
				break;
			}
			case 81:
			{
				char ch = NextChar();
				if (ch == 92) m_state = 82;
				else m_state = NextPattern();
				break;
			}
			case 82:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 83;
				else m_state = NextPattern();
				break;
			}
			case 83:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('\\'); }
				}
				break;
			}
			// "'\('"
			case 84:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 85;
				else m_state = NextPattern();
				break;
			}
			case 85:
			{
				char ch = NextChar();
				if (ch == 40) m_state = 86;
				else m_state = NextPattern();
				break;
			}
			case 86:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 87;
				else m_state = NextPattern();
				break;
			}
			case 87:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('('); }
				}
				break;
			}
			// "'\)'"
			case 88:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 89;
				else m_state = NextPattern();
				break;
			}
			case 89:
			{
				char ch = NextChar();
				if (ch == 41) m_state = 90;
				else m_state = NextPattern();
				break;
			}
			case 90:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 91;
				else m_state = NextPattern();
				break;
			}
			case 91:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return(')'); }
				}
				break;
			}
			// "'\+'"
			case 92:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 93;
				else m_state = NextPattern();
				break;
			}
			case 93:
			{
				char ch = NextChar();
				if (ch == 43) m_state = 94;
				else m_state = NextPattern();
				break;
			}
			case 94:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 95;
				else m_state = NextPattern();
				break;
			}
			case 95:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('+'); }
				}
				break;
			}
			// "'\*'"
			case 96:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 97;
				else m_state = NextPattern();
				break;
			}
			case 97:
			{
				char ch = NextChar();
				if (ch == 42) m_state = 98;
				else m_state = NextPattern();
				break;
			}
			case 98:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 99;
				else m_state = NextPattern();
				break;
			}
			case 99:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('*'); }
				}
				break;
			}
			// "'\['"
			case 100:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 101;
				else m_state = NextPattern();
				break;
			}
			case 101:
			{
				char ch = NextChar();
				if (ch == 91) m_state = 102;
				else m_state = NextPattern();
				break;
			}
			case 102:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 103;
				else m_state = NextPattern();
				break;
			}
			case 103:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return('['); }
				}
				break;
			}
			// "'\]'"
			case 104:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 105;
				else m_state = NextPattern();
				break;
			}
			case 105:
			{
				char ch = NextChar();
				if (ch == 93) m_state = 106;
				else m_state = NextPattern();
				break;
			}
			case 106:
			{
				char ch = NextChar();
				if (ch == 39) m_state = 107;
				else m_state = NextPattern();
				break;
			}
			case 107:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return(']'); }
				}
				break;
			}
			// "%%"
			case 108:
			{
				char ch = NextChar();
				if (ch == 37) m_state = 109;
				else m_state = NextPattern();
				break;
			}
			case 109:
			{
				char ch = NextChar();
				if (ch == 37) m_state = 110;
				else m_state = NextPattern();
				break;
			}
			case 110:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return dParcelCompiler::GRAMMAR_SEGEMENT;}
				}
				break;
			}
			// "%token"
			case 111:
			{
				char ch = NextChar();
				if (ch == 37) m_state = 112;
				else m_state = NextPattern();
				break;
			}
			case 112:
			{
				char ch = NextChar();
				if (ch == 116) m_state = 113;
				else m_state = NextPattern();
				break;
			}
			case 113:
			{
				char ch = NextChar();
				if (ch == 111) m_state = 114;
				else m_state = NextPattern();
				break;
			}
			case 114:
			{
				char ch = NextChar();
				if (ch == 107) m_state = 115;
				else m_state = NextPattern();
				break;
			}
			case 115:
			{
				char ch = NextChar();
				if (ch == 101) m_state = 116;
				else m_state = NextPattern();
				break;
			}
			case 116:
			{
				char ch = NextChar();
				if (ch == 110) m_state = 117;
				else m_state = NextPattern();
				break;
			}
			case 117:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return dParcelCompiler::TOKEN;}
				}
				break;
			}
			// "%left"
			case 118:
			{
				char ch = NextChar();
				if (ch == 37) m_state = 119;
				else m_state = NextPattern();
				break;
			}
			case 119:
			{
				char ch = NextChar();
				if (ch == 108) m_state = 120;
				else m_state = NextPattern();
				break;
			}
			case 120:
			{
				char ch = NextChar();
				if (ch == 101) m_state = 121;
				else m_state = NextPattern();
				break;
			}
			case 121:
			{
				char ch = NextChar();
				if (ch == 102) m_state = 122;
				else m_state = NextPattern();
				break;
			}
			case 122:
			{
				char ch = NextChar();
				if (ch == 116) m_state = 123;
				else m_state = NextPattern();
				break;
			}
			case 123:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return dParcelCompiler::LEFT;}
				}
				break;
			}
			// "%right"
			case 124:
			{
				char ch = NextChar();
				if (ch == 37) m_state = 125;
				else m_state = NextPattern();
				break;
			}
			case 125:
			{
				char ch = NextChar();
				if (ch == 114) m_state = 126;
				else m_state = NextPattern();
				break;
			}
			case 126:
			{
				char ch = NextChar();
				if (ch == 105) m_state = 127;
				else m_state = NextPattern();
				break;
			}
			case 127:
			{
				char ch = NextChar();
				if (ch == 103) m_state = 128;
				else m_state = NextPattern();
				break;
			}
			case 128:
			{
				char ch = NextChar();
				if (ch == 104) m_state = 129;
				else m_state = NextPattern();
				break;
			}
			case 129:
			{
				char ch = NextChar();
				if (ch == 116) m_state = 130;
				else m_state = NextPattern();
				break;
			}
			case 130:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return dParcelCompiler::RIGHT;}
				}
				break;
			}
			// "%start"
			case 131:
			{
				char ch = NextChar();
				if (ch == 37) m_state = 132;
				else m_state = NextPattern();
				break;
			}
			case 132:
			{
				char ch = NextChar();
				if (ch == 115) m_state = 133;
				else m_state = NextPattern();
				break;
			}
			case 133:
			{
				char ch = NextChar();
				if (ch == 116) m_state = 134;
				else m_state = NextPattern();
				break;
			}
			case 134:
			{
				char ch = NextChar();
				if (ch == 97) m_state = 135;
				else m_state = NextPattern();
				break;
			}
			case 135:
			{
				char ch = NextChar();
				if (ch == 114) m_state = 136;
				else m_state = NextPattern();
				break;
			}
			case 136:
			{
				char ch = NextChar();
				if (ch == 116) m_state = 137;
				else m_state = NextPattern();
				break;
			}
			case 137:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return dParcelCompiler::START;}
				}
				break;
			}
			// {Literal}
			case 138:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_4)) m_state = 139;
				else m_state = NextPattern();
				break;
			}
			case 139:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_5)) m_state = 140;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return dParcelCompiler::LITERAL;}
				}
				break;
			}
			case 140:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_5)) m_state = 140;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return dParcelCompiler::LITERAL;}
				}
				break;
			}
			// {CodeBlock}
			case 141:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_6)) m_state = 142;
				else m_state = NextPattern();
				break;
			}
			case 142:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_7)) m_state = 143;
				else m_state = NextPattern();
				break;
			}
			case 143:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_6)) m_state = 144;
				else if (IsCharInSet (ch, text_8)) m_state = 145;
				else if (IsCharInSet (ch, text_9)) m_state = 146;
				else m_state = NextPattern();
				break;
			}
			case 146:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_6)) m_state = 144;
				else if (IsCharInSet (ch, text_8)) m_state = 145;
				else if (IsCharInSet (ch, text_9)) m_state = 146;
				else m_state = NextPattern();
				break;
			}
			case 145:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_6)) m_state = 144;
				else if (IsCharInSet (ch, text_8)) m_state = 145;
				else if (IsCharInSet (ch, text_9)) m_state = 146;
				else m_state = NextPattern();
				break;
			}
			case 144:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_6)) m_state = 144;
				else if (IsCharInSet (ch, text_8)) m_state = 147;
				else if (IsCharInSet (ch, text_10)) m_state = 148;
				else if (IsCharInSet (ch, text_11)) m_state = 149;
				else m_state = NextPattern();
				break;
			}
			case 149:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ return dParcelCompiler::CODE_BLOCK;}
				}
				break;
			}
			case 148:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_6)) m_state = 144;
				else if (IsCharInSet (ch, text_8)) m_state = 145;
				else if (IsCharInSet (ch, text_9)) m_state = 146;
				else m_state = NextPattern();
				break;
			}
			case 147:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_6)) m_state = 144;
				else if (IsCharInSet (ch, text_8)) m_state = 145;
				else if (IsCharInSet (ch, text_9)) m_state = 146;
				else m_state = NextPattern();
				break;
			}
			// [{]
			case 150:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_7)) m_state = 151;
				else m_state = NextPattern();
				break;
			}
			case 151:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{ ReadUserAction(*this); return dParcelCompiler::USER_ACTION;}
				}
				break;
			}
			// {Comment}
			case 152:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_12)) m_state = 153;
				else m_state = NextPattern();
				break;
			}
			case 153:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_12)) m_state = 154;
				else if (IsCharInSet (ch, text_13)) m_state = 155;
				else m_state = NextPattern();
				break;
			}
			case 155:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_13)) m_state = 156;
				else if (IsCharInSet (ch, text_14)) m_state = 157;
				else m_state = NextPattern();
				break;
			}
			case 157:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_13)) m_state = 156;
				else if (IsCharInSet (ch, text_14)) m_state = 157;
				else m_state = NextPattern();
				break;
			}
			case 156:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_12)) m_state = 158;
				else if (IsCharInSet (ch, text_15)) m_state = 159;
				else m_state = NextPattern();
				break;
			}
			case 159:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_13)) m_state = 156;
				else if (IsCharInSet (ch, text_14)) m_state = 157;
				else m_state = NextPattern();
				break;
			}
			case 158:
			{
				{
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{}
				}
				break;
			}
			case 154:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_16)) m_state = 160;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{}
				}
				break;
			}
			case 160:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_16)) m_state = 160;
				else {
					m_index --;
					GetLexString ();
					//user specified action
					m_state = 0;
					m_startState = 0;
					{}
				}
				break;
			}

		}
	}
	m_tokenString = "";
	return -1;
}


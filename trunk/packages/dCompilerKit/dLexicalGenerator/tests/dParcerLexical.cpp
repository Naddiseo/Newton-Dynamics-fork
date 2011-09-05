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
//Auto generated Lexical Analyzer class: dParcerLexical.cpp
//

#include <dParcerCompiler.h>
#include "dGrammarLexical.h"

#include "dParcerLexical.h"

#ifdef _MSC_VER
	#pragma warning (disable: 4702) // warning C4702: unreachable code
#endif


dParcerLexical::dParcerLexical(const char* const data)
	:m_tokenString ("")
	,m_data(data)
	,m_index(0)
	,m_startIndex(0)
	,m_lineNumber(0)
{
}

dParcerLexical::~dParcerLexical()
{
}

bool dParcerLexical::IsCharInSet (char ch, const char* const set, int setSize) const
{
	int i0 = 0;
	int i1 = setSize - 1;
	while ((i1 - i0) >= 4) {
		int i = (i1 + i0 + 1)>>1;
		if (ch <= set[i]) {
			i1 = i;
		} else {
			i0 = i;
		}
	}

	for (int i = i0; i <= i1; i ++) {
		if (ch == set[i]) {
			return true;
		}
	}
	return false;

}


void dParcerLexical::GetLexString ()
{
	int length = m_index - m_startIndex;
	m_tokenString = string (&m_data[m_startIndex], length);
	m_startIndex = m_index;
}


int dParcerLexical::NextToken ()
{
	//static strings patterns
	static char text_0[] = {124, 0};
	static char text_1[] = {58, 0};
	static char text_2[] = {59, 0};
	static char text_3[] = {65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};
	static char text_4[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};
	static char text_5[] = {37, 0};
	static char text_6[] = {123, 0};
	static char text_7[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_8[] = {10, 13, 0};
	static char text_9[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 126, 127, 0};
	static char text_10[] = {125, 0};
	static char text_11[] = {9, 10, 13, 32, 0};
	static char text_12[] = {47, 0};
	static char text_13[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_14[] = {42, 0};
	static char text_15[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_16[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};

	static int characterSetSize[] = {1, 1, 1, 53, 63, 1, 1, 126, 2, 126, 1, 4, 1, 126, 1, 126, 126};
	static char* characterSetArray[] = {text_0, text_1, text_2, text_3, text_4, text_5, text_6, text_7, text_8, text_9, text_10, text_11, text_12, text_13, text_14, text_15, text_16};

	static int transitionsCount[] = {10, 6, 25, 0, 0, 0, 0, 1, 0, 0, 2, 0, 2, 2, 2, 0, 2, 0, 3, 4, 3, 3, 3, 3, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0};
	static int transitionsStart[] = {0, 90, 40, 0, 0, 0, 0, 20, 0, 0, 10, 0, 12, 16, 14, 0, 18, 0, 21, 30, 27, 24, 37, 34, 0, 0, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 112, 108, 104, 100, 96, 97, 98, 99, 0, 101, 102, 103, 0, 105, 106, 107, 0, 109, 110, 111, 0, 113, 114, 0, 0};
	static dTransitionInfo nextTranstionList[] = {37, 0, 1, 39, 0, 2, 0, 1, 3, 1, 1, 4, 2, 1, 5, 3, 1, 6, 5, 1, 7, 6, 1, 8, 11, 1, 9, 12, 1, 10, 12, 1, 11, 14, 1, 12, 14, 1, 13, 15, 1, 14, 14, 1, 13, 15, 1, 14, 12, 1, 15, 16, 1, 16, 14, 1, 13, 15, 1, 14, 6, 1, 18, 5, 1, 19, 7, 1, 20, 8, 1, 21, 5, 1, 19, 7, 1, 20, 8, 1, 21, 5, 1, 19, 7, 1, 20, 8, 1, 21, 5, 1, 19, 8, 1, 22, 9, 1, 23, 10, 1, 24, 5, 1, 19, 7, 1, 20, 8, 1, 21, 5, 1, 19, 7, 1, 20, 8, 1, 21, 33, 0, 26, 37, 0, 27, 38, 0, 28, 40, 0, 29, 41, 0, 30, 42, 0, 31, 43, 0, 32, 44, 0, 33, 45, 0, 34, 46, 0, 35, 47, 0, 36, 58, 0, 37, 59, 0, 38, 60, 0, 39, 61, 0, 40, 62, 0, 41, 63, 0, 42, 91, 0, 43, 92, 0, 44, 93, 0, 45, 94, 0, 46, 123, 0, 47, 124, 0, 48, 125, 0, 49, 126, 0, 50, 39, 0, 51, 39, 0, 52, 39, 0, 53, 39, 0, 54, 39, 0, 55, 39, 0, 56, 39, 0, 57, 39, 0, 58, 39, 0, 59, 39, 0, 60, 39, 0, 61, 39, 0, 62, 39, 0, 63, 39, 0, 64, 39, 0, 65, 39, 0, 66, 39, 0, 67, 39, 0, 68, 39, 0, 69, 39, 0, 70, 39, 0, 71, 39, 0, 72, 39, 0, 73, 39, 0, 74, 39, 0, 75, 37, 0, 76, 108, 0, 77, 114, 0, 78, 115, 0, 79, 116, 0, 80, 117, 0, 81, 110, 0, 82, 105, 0, 83, 111, 0, 84, 110, 0, 85, 111, 0, 86, 107, 0, 87, 101, 0, 88, 110, 0, 89, 116, 0, 90, 97, 0, 91, 114, 0, 92, 116, 0, 93, 105, 0, 94, 103, 0, 95, 104, 0, 96, 116, 0, 97, 101, 0, 98, 102, 0, 99, 116, 0, 100, 0};
	
	m_startIndex = m_index;

	int state = 0;
	for (bool matchFound = false; !matchFound; )
	{
		switch (state) 
		{
			case 15:
			#line 85 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{}
					state = 0;
				}
				break;
			}
			case 11:
			#line 85 "tests/dParcerLexical.lex"
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_13, characterSetSize[13])) state = 17;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{}
					state = 0;
				}
				break;
			}
			case 17:
			#line 85 "tests/dParcerLexical.lex"
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_13, characterSetSize[13])) state = 17;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{}
					state = 0;
				}
				break;
			}
			case 9:
			#line 84 "tests/dParcerLexical.lex"
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_11, characterSetSize[11])) state = 9;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{}
					state = 0;
				}
				break;
			}
			case 8:
			#line 82 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ ((dGrammarLexical*)this)->ReadUserAction(); return dParcerCompiler::USER_ACTION;}
					state = 0;
				}
				break;
			}
			case 24:
			#line 81 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString.replace(0, 2, ""); m_tokenString.replace(m_tokenString.size() - 2, 2, ""); return dParcerCompiler::CODE_BLOCK;}
					state = 0;
				}
				break;
			}
			case 6:
			#line 80 "tests/dParcerLexical.lex"
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_4, characterSetSize[4])) state = 25;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::LITERAL;}
					state = 0;
				}
				break;
			}
			case 25:
			#line 80 "tests/dParcerLexical.lex"
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_4, characterSetSize[4])) state = 25;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::LITERAL;}
					state = 0;
				}
				break;
			}
			case 5:
			#line 46 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return(dParcerCompiler::SIMICOLOM); }
					state = 0;
				}
				break;
			}
			case 4:
			#line 45 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return(dParcerCompiler::COLOM); }
					state = 0;
				}
				break;
			}
			case 3:
			#line 44 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return(dParcerCompiler::OR); }
					state = 0;
				}
				break;
			}
			case 51:
			#line 54 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('~'); }
					state = 0;
				}
				break;
			}
			case 52:
			#line 49 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('}'); }
					state = 0;
				}
				break;
			}
			case 53:
			#line 63 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('|'); }
					state = 0;
				}
				break;
			}
			case 54:
			#line 48 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('{'); }
					state = 0;
				}
				break;
			}
			case 55:
			#line 60 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('^'); }
					state = 0;
				}
				break;
			}
			case 56:
			#line 71 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return(']'); }
					state = 0;
				}
				break;
			}
			case 57:
			#line 65 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('\\'); }
					state = 0;
				}
				break;
			}
			case 58:
			#line 70 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('['); }
					state = 0;
				}
				break;
			}
			case 59:
			#line 64 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('?'); }
					state = 0;
				}
				break;
			}
			case 60:
			#line 58 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('>'); }
					state = 0;
				}
				break;
			}
			case 61:
			#line 51 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('='); }
					state = 0;
				}
				break;
			}
			case 62:
			#line 57 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('<'); }
					state = 0;
				}
				break;
			}
			case 63:
			#line 47 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return(';'); }
					state = 0;
				}
				break;
			}
			case 64:
			#line 61 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return(':'); }
					state = 0;
				}
				break;
			}
			case 65:
			#line 59 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('/'); }
					state = 0;
				}
				break;
			}
			case 66:
			#line 62 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('.'); }
					state = 0;
				}
				break;
			}
			case 67:
			#line 55 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('-'); }
					state = 0;
				}
				break;
			}
			case 68:
			#line 50 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return(','); }
					state = 0;
				}
				break;
			}
			case 69:
			#line 68 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('+'); }
					state = 0;
				}
				break;
			}
			case 70:
			#line 69 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('*'); }
					state = 0;
				}
				break;
			}
			case 71:
			#line 67 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return(')'); }
					state = 0;
				}
				break;
			}
			case 72:
			#line 66 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('('); }
					state = 0;
				}
				break;
			}
			case 73:
			#line 52 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('&'); }
					state = 0;
				}
				break;
			}
			case 74:
			#line 56 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('%'); }
					state = 0;
				}
				break;
			}
			case 75:
			#line 53 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return('!'); }
					state = 0;
				}
				break;
			}
			case 85:
			#line 75 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::UNION;}
					state = 0;
				}
				break;
			}
			case 89:
			#line 76 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::TOKEN;}
					state = 0;
				}
				break;
			}
			case 93:
			#line 79 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::START;}
					state = 0;
				}
				break;
			}
			case 97:
			#line 78 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::RIGHT;}
					state = 0;
				}
				break;
			}
			case 100:
			#line 77 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::LEFT;}
					state = 0;
				}
				break;
			}
			case 76:
			#line 74 "tests/dParcerLexical.lex"
			{
				{
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::GRAMMAR_SEGMENT;}
					state = 0;
				}
				break;
			}

		
			default:
			{
				char ch = NextChar();
				int count = transitionsCount[state];
				int start = transitionsStart[state];
				dTransitionInfo* const transitionsList = &nextTranstionList[start];

				bool stateChanged = false;
				for (int i = 0; i < count; i ++) {
					dTransitionInfo& transition = transitionsList[i];
					if (transition.m_type == m_infoIsCharacter) {
						if (ch == transition.m_info) {
							state = transition.m_nextState;
							stateChanged = true;
							break;
						}
					} else {
						_ASSERTE (transition.m_type == m_infoIsCharacterSet);
						int index = transition.m_info;
						int length = characterSetSize[index];
						const char* text = characterSetArray[index];
						if (IsCharInSet (ch, text, length)) {
							state = transition.m_nextState;
							stateChanged = true;
							break;
						}

					}
				}

				if (!stateChanged) {
					// Unknown pattern
					return -1;
				}
			}
		}
	}
	// Unknown pattern
	return -1;
}


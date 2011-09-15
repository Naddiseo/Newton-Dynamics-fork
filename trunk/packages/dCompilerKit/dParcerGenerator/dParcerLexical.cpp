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

#include "dParcerLexical.h"


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

void dParcerLexical::ReadBalancedExpresion (char open, char close)
{
	int count = 1;
	while (count) {
		int ch = NextChar();
		if (ch == '\n') {
			m_lineNumber ++;
		}

		if(ch == open) {
			count ++;
		} else if (ch == close) {
			count --;
		} else {
			if (ch == '\'')	{
				ch = NextChar();
				if (ch == '\\') {
					ch = NextChar();
				}
				ch = NextChar();
			} else if (ch == '\"') {
				for (ch = NextChar(); ch != '\"'; ch = NextChar()) {
					if (ch == '\\') {
						ch = NextChar();
					}
				}
			}
		}
	}

	string tmp (m_tokenString);
	GetLexString();
	m_tokenString = tmp + m_tokenString;
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
	static char text_0[] = {9, 10, 13, 32, 0};
	static char text_1[] = {47, 0};
	static char text_2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_3[] = {42, 0};
	static char text_4[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_5[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_6[] = {65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};
	static char text_7[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 95, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0};
	static char text_8[] = {123, 0};
	static char text_9[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 0};
	static char text_10[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 126, 127, 0};
	static char text_11[] = {125, 0};
	static char text_12[] = {124, 0};
	static char text_13[] = {58, 0};
	static char text_14[] = {59, 0};

	static int characterSetSize[] = {4, 1, 126, 1, 126, 126, 53, 63, 1, 126, 126, 1, 1, 1, 1};
	static char* characterSetArray[] = {text_0, text_1, text_2, text_3, text_4, text_5, text_6, text_7, text_8, text_9, text_10, text_11, text_12, text_13, text_14};

	static int transitionsCount[] = {9, 7, 25, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0};
	static int transitionsStart[] = {0, 69, 19, 0, 9, 0, 0, 0, 0, 0, 0, 0, 11, 15, 13, 0, 17, 0, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 96, 92, 88, 84, 76, 80, 78, 82, 0, 85, 86, 87, 0, 89, 90, 91, 0, 93, 94, 95, 0, 97, 98, 99, 0, 101, 102, 0, 0};
	static unsigned nextTranstionList[] = {0x0250001, 0x0270002, 0x04003, 0x014004, 0x064005, 0x084006, 0x0c4007, 0x0d4008, 0x0e4009, 0x01400b, 0x03400c, 0x03400d, 0x04400e, 0x03400d, 0x04400e, 0x01400f, 0x054010, 0x03400d, 0x04400e, 0x0210012, 0x0250013, 0x0260014, 0x0280015, 0x0290016, 0x02a0017, 0x02b0018, 0x02c0019, 0x02d001a, 0x02e001b, 0x02f001c, 0x03a001d, 0x03b001e, 0x03c001f, 0x03d0020, 0x03e0021, 0x03f0022, 0x05b0023, 0x05c0024, 0x05d0025, 0x05e0026, 0x07b0027, 0x07c0028, 0x07d0029, 0x07e002a, 0x027002b, 0x027002c, 0x027002d, 0x027002e, 0x027002f, 0x0270030, 0x0270031, 0x0270032, 0x0270033, 0x0270034, 0x0270035, 0x0270036, 0x0270037, 0x0270038, 0x0270039, 0x027003a, 0x027003b, 0x027003c, 0x027003d, 0x027003e, 0x027003f, 0x0270040, 0x0270041, 0x0270042, 0x0270043, 0x0250044, 0x06c0045, 0x0720046, 0x0730047, 0x0740048, 0x0750049, 0x08404a, 0x025004b, 0x09404c, 0x025004b, 0x09404c, 0x0a404d, 0x0b404e, 0x025004b, 0x09404c, 0x06e004f, 0x0690050, 0x06f0051, 0x06e0052, 0x06f0053, 0x06b0054, 0x0650055, 0x06e0056, 0x0740057, 0x0610058, 0x0720059, 0x074005a, 0x069005b, 0x067005c, 0x068005d, 0x074005e, 0x065005f, 0x0660060, 0x0740061, 0};
	
	m_startIndex = m_index;

	int state = 0;
	for (bool matchFound = false; !matchFound; )
	{
		switch (state) 
		{
			case 9:
			{
				{
					GetLexString ();
					//user specified action
					{ return(dParcerCompiler::SIMICOLOM); }
					state = 0;
				}
				break;
			}
			case 8:
			{
				{
					GetLexString ();
					//user specified action
					{ return(dParcerCompiler::COLOM); }
					state = 0;
				}
				break;
			}
			case 7:
			{
				{
					GetLexString ();
					//user specified action
					{ return(dParcerCompiler::OR); }
					state = 0;
				}
				break;
			}
			case 6:
			{
				{
					GetLexString ();
					//user specified action
					{ ReadBalancedExpresion ('{', '}'); return dParcerCompiler::SEMANTIC_ACTION;}
					state = 0;
				}
				break;
			}
			case 5:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_7, characterSetSize[7])) state = 10;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::LITERAL;}
					state = 0;
				}
				break;
			}
			case 10:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_7, characterSetSize[7])) state = 10;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::LITERAL;}
					state = 0;
				}
				break;
			}
			case 15:
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
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_2, characterSetSize[2])) state = 17;
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
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_2, characterSetSize[2])) state = 17;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{}
					state = 0;
				}
				break;
			}
			case 3:
			{
				char ch = NextChar();
				if (IsCharInSet (ch, text_0, characterSetSize[0])) state = 3;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{}
					state = 0;
				}
				break;
			}
			case 43:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "~"; return('~'); }
					state = 0;
				}
				break;
			}
			case 44:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "}"; return('}'); }
					state = 0;
				}
				break;
			}
			case 45:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "|"; return('|'); }
					state = 0;
				}
				break;
			}
			case 46:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "{"; return('{'); }
					state = 0;
				}
				break;
			}
			case 47:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "^"; return('^'); }
					state = 0;
				}
				break;
			}
			case 48:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "]"; return(']'); }
					state = 0;
				}
				break;
			}
			case 49:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "\\"; return('\\'); }
					state = 0;
				}
				break;
			}
			case 50:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "["; return('['); }
					state = 0;
				}
				break;
			}
			case 51:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "?"; return('?'); }
					state = 0;
				}
				break;
			}
			case 52:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = ">"; return('>'); }
					state = 0;
				}
				break;
			}
			case 53:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "="; return('='); }
					state = 0;
				}
				break;
			}
			case 54:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "<"; return('<'); }
					state = 0;
				}
				break;
			}
			case 55:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = ";"; return(';'); }
					state = 0;
				}
				break;
			}
			case 56:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = ":"; return(':'); }
					state = 0;
				}
				break;
			}
			case 57:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "/"; return('/'); }
					state = 0;
				}
				break;
			}
			case 58:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "."; return('.'); }
					state = 0;
				}
				break;
			}
			case 59:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "-"; return('-'); }
					state = 0;
				}
				break;
			}
			case 60:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = ","; return(','); }
					state = 0;
				}
				break;
			}
			case 61:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "+"; return('+'); }
					state = 0;
				}
				break;
			}
			case 62:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "*"; return('*'); }
					state = 0;
				}
				break;
			}
			case 63:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = ")"; return(')'); }
					state = 0;
				}
				break;
			}
			case 64:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "("; return('('); }
					state = 0;
				}
				break;
			}
			case 65:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "&"; return('&'); }
					state = 0;
				}
				break;
			}
			case 66:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "%"; return('%'); }
					state = 0;
				}
				break;
			}
			case 67:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString = "!"; return('!'); }
					state = 0;
				}
				break;
			}
			case 78:
			{
				{
					GetLexString ();
					//user specified action
					{ m_tokenString.replace(0, 2, ""); m_tokenString.replace(m_tokenString.size() - 2, 2, ""); return dParcerCompiler::CODE_BLOCK;}
					state = 0;
				}
				break;
			}
			case 82:
			{
				{
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::UNION;}
					state = 0;
				}
				break;
			}
			case 86:
			{
				{
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::TOKEN;}
					state = 0;
				}
				break;
			}
			case 90:
			{
				{
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::START;}
					state = 0;
				}
				break;
			}
			case 94:
			{
				{
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::RIGHT;}
					state = 0;
				}
				break;
			}
			case 97:
			{
				{
					GetLexString ();
					//user specified action
					{ return dParcerCompiler::LEFT;}
					state = 0;
				}
				break;
			}
			case 68:
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
				unsigned* const transitionsList = &nextTranstionList[start];

				bool stateChanged = false;
				for (int i = 0; i < count; i ++) {
					dTransitionInfo transition (transitionsList[i]);
					if (transition.m_infoType == m_infoIsCharacter) {
						if (ch == char (transition.m_info)) {
							state = transition.m_nextState;
							stateChanged = true;
							break;
						}
					} else {
						_ASSERTE (transition.m_infoType == m_infoIsCharacterSet);
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



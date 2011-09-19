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
//Auto generated Lexical Analyzer class: dAssemblerLexical.cpp
//

#include <dVirtualMachine.h>
#include "dAssemblerParser.h"

//
// Newton virtual machine assembler grammar
// based loosely on the MIPS R3000 and the Intel 386 instructions sets 
//

#include "dAssemblerLexical.h"


dAssemblerLexical::dAssemblerLexical(const char* const data)
	:m_tokenString ("")
	,m_data(data)
	,m_index(0)
	,m_startIndex(0)
	,m_lineNumber(0)
{
}

dAssemblerLexical::~dAssemblerLexical()
{
}

bool dAssemblerLexical::IsCharInSet (char ch, const char* const set, int setSize) const
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

void dAssemblerLexical::ReadBalancedExpresion (char open, char close)
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


void dAssemblerLexical::GetLexString ()
{
	int length = m_index - m_startIndex;
	m_tokenString = string (&m_data[m_startIndex], length);
	m_startIndex = m_index;
}


int dAssemblerLexical::NextToken ()
{
	//static strings patterns

	static int characterSetSize[] = {0};
	static char* characterSetArray[] = {0};

	static int transitionsCount[] = {53, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	static int transitionsStart[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	static unsigned nextTranstionList[] = {0x0410001, 0x0420001, 0x0430001, 0x0440001, 0x0450001, 0x0460001, 0x0470001, 0x0480001, 0x0490001, 0x04a0001, 0x04b0001, 0x04c0001, 0x04d0001, 0x04e0001, 0x04f0001, 0x0500001, 0x0510001, 0x0520001, 0x0530001, 0x0540001, 0x0550001, 0x0560001, 0x0570001, 0x0580001, 0x0590001, 0x05a0001, 0x05f0001, 0x0610001, 0x0620002, 0x0630001, 0x0640001, 0x0650001, 0x0660001, 0x0670001, 0x0680001, 0x0690001, 0x06a0001, 0x06b0001, 0x06c0001, 0x06d0001, 0x06e0001, 0x06f0001, 0x0700001, 0x0710001, 0x0720001, 0x0730001, 0x0740001, 0x0750001, 0x0760001, 0x0770001, 0x0780001, 0x0790001, 0x07a0001, 0};
	
	m_startIndex = m_index;

	int state = 0;
	for (bool matchFound = false; !matchFound; )
	{
		switch (state) 
		{
			case 2:
			{
				char ch = NextChar();
				if (ch == 48) state = 3;
				else if (ch == 49) state = 3;
				else if (ch == 50) state = 3;
				else if (ch == 51) state = 3;
				else if (ch == 52) state = 3;
				else if (ch == 53) state = 3;
				else if (ch == 54) state = 3;
				else if (ch == 55) state = 3;
				else if (ch == 56) state = 3;
				else if (ch == 57) state = 3;
				else if (ch == 65) state = 3;
				else if (ch == 66) state = 3;
				else if (ch == 67) state = 3;
				else if (ch == 68) state = 3;
				else if (ch == 69) state = 3;
				else if (ch == 70) state = 3;
				else if (ch == 71) state = 3;
				else if (ch == 72) state = 3;
				else if (ch == 73) state = 3;
				else if (ch == 74) state = 3;
				else if (ch == 75) state = 3;
				else if (ch == 76) state = 3;
				else if (ch == 77) state = 3;
				else if (ch == 78) state = 3;
				else if (ch == 79) state = 3;
				else if (ch == 80) state = 3;
				else if (ch == 81) state = 3;
				else if (ch == 82) state = 3;
				else if (ch == 83) state = 3;
				else if (ch == 84) state = 3;
				else if (ch == 85) state = 3;
				else if (ch == 86) state = 3;
				else if (ch == 87) state = 3;
				else if (ch == 88) state = 3;
				else if (ch == 89) state = 3;
				else if (ch == 90) state = 3;
				else if (ch == 95) state = 3;
				else if (ch == 97) state = 3;
				else if (ch == 98) state = 3;
				else if (ch == 99) state = 3;
				else if (ch == 100) state = 3;
				else if (ch == 101) state = 4;
				else if (ch == 102) state = 3;
				else if (ch == 103) state = 3;
				else if (ch == 104) state = 3;
				else if (ch == 105) state = 3;
				else if (ch == 106) state = 3;
				else if (ch == 107) state = 3;
				else if (ch == 108) state = 3;
				else if (ch == 109) state = 3;
				else if (ch == 110) state = 3;
				else if (ch == 111) state = 3;
				else if (ch == 112) state = 3;
				else if (ch == 113) state = 3;
				else if (ch == 114) state = 3;
				else if (ch == 115) state = 3;
				else if (ch == 116) state = 3;
				else if (ch == 117) state = 3;
				else if (ch == 118) state = 3;
				else if (ch == 119) state = 3;
				else if (ch == 120) state = 3;
				else if (ch == 121) state = 3;
				else if (ch == 122) state = 3;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{return dAssemblerParser::LITERAL;}
					state = 0;
				}
				break;
			}
			case 4:
			{
				char ch = NextChar();
				if (ch == 48) state = 3;
				else if (ch == 49) state = 3;
				else if (ch == 50) state = 3;
				else if (ch == 51) state = 3;
				else if (ch == 52) state = 3;
				else if (ch == 53) state = 3;
				else if (ch == 54) state = 3;
				else if (ch == 55) state = 3;
				else if (ch == 56) state = 3;
				else if (ch == 57) state = 3;
				else if (ch == 65) state = 3;
				else if (ch == 66) state = 3;
				else if (ch == 67) state = 3;
				else if (ch == 68) state = 3;
				else if (ch == 69) state = 3;
				else if (ch == 70) state = 3;
				else if (ch == 71) state = 3;
				else if (ch == 72) state = 3;
				else if (ch == 73) state = 3;
				else if (ch == 74) state = 3;
				else if (ch == 75) state = 3;
				else if (ch == 76) state = 3;
				else if (ch == 77) state = 3;
				else if (ch == 78) state = 3;
				else if (ch == 79) state = 3;
				else if (ch == 80) state = 3;
				else if (ch == 81) state = 3;
				else if (ch == 82) state = 3;
				else if (ch == 83) state = 3;
				else if (ch == 84) state = 3;
				else if (ch == 85) state = 3;
				else if (ch == 86) state = 3;
				else if (ch == 87) state = 3;
				else if (ch == 88) state = 3;
				else if (ch == 89) state = 3;
				else if (ch == 90) state = 3;
				else if (ch == 95) state = 3;
				else if (ch == 97) state = 3;
				else if (ch == 98) state = 3;
				else if (ch == 99) state = 3;
				else if (ch == 100) state = 3;
				else if (ch == 101) state = 3;
				else if (ch == 102) state = 3;
				else if (ch == 103) state = 5;
				else if (ch == 104) state = 3;
				else if (ch == 105) state = 3;
				else if (ch == 106) state = 3;
				else if (ch == 107) state = 3;
				else if (ch == 108) state = 3;
				else if (ch == 109) state = 3;
				else if (ch == 110) state = 3;
				else if (ch == 111) state = 3;
				else if (ch == 112) state = 3;
				else if (ch == 113) state = 3;
				else if (ch == 114) state = 3;
				else if (ch == 115) state = 3;
				else if (ch == 116) state = 3;
				else if (ch == 117) state = 3;
				else if (ch == 118) state = 3;
				else if (ch == 119) state = 3;
				else if (ch == 120) state = 3;
				else if (ch == 121) state = 3;
				else if (ch == 122) state = 3;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{return dAssemblerParser::LITERAL;}
					state = 0;
				}
				break;
			}
			case 5:
			{
				char ch = NextChar();
				if (ch == 48) state = 3;
				else if (ch == 49) state = 3;
				else if (ch == 50) state = 3;
				else if (ch == 51) state = 3;
				else if (ch == 52) state = 3;
				else if (ch == 53) state = 3;
				else if (ch == 54) state = 3;
				else if (ch == 55) state = 3;
				else if (ch == 56) state = 3;
				else if (ch == 57) state = 3;
				else if (ch == 65) state = 3;
				else if (ch == 66) state = 3;
				else if (ch == 67) state = 3;
				else if (ch == 68) state = 3;
				else if (ch == 69) state = 3;
				else if (ch == 70) state = 3;
				else if (ch == 71) state = 3;
				else if (ch == 72) state = 3;
				else if (ch == 73) state = 3;
				else if (ch == 74) state = 3;
				else if (ch == 75) state = 3;
				else if (ch == 76) state = 3;
				else if (ch == 77) state = 3;
				else if (ch == 78) state = 3;
				else if (ch == 79) state = 3;
				else if (ch == 80) state = 3;
				else if (ch == 81) state = 3;
				else if (ch == 82) state = 3;
				else if (ch == 83) state = 3;
				else if (ch == 84) state = 3;
				else if (ch == 85) state = 3;
				else if (ch == 86) state = 3;
				else if (ch == 87) state = 3;
				else if (ch == 88) state = 3;
				else if (ch == 89) state = 3;
				else if (ch == 90) state = 3;
				else if (ch == 95) state = 3;
				else if (ch == 97) state = 3;
				else if (ch == 98) state = 3;
				else if (ch == 99) state = 3;
				else if (ch == 100) state = 3;
				else if (ch == 101) state = 3;
				else if (ch == 102) state = 3;
				else if (ch == 103) state = 3;
				else if (ch == 104) state = 3;
				else if (ch == 105) state = 6;
				else if (ch == 106) state = 3;
				else if (ch == 107) state = 3;
				else if (ch == 108) state = 3;
				else if (ch == 109) state = 3;
				else if (ch == 110) state = 3;
				else if (ch == 111) state = 3;
				else if (ch == 112) state = 3;
				else if (ch == 113) state = 3;
				else if (ch == 114) state = 3;
				else if (ch == 115) state = 3;
				else if (ch == 116) state = 3;
				else if (ch == 117) state = 3;
				else if (ch == 118) state = 3;
				else if (ch == 119) state = 3;
				else if (ch == 120) state = 3;
				else if (ch == 121) state = 3;
				else if (ch == 122) state = 3;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{return dAssemblerParser::LITERAL;}
					state = 0;
				}
				break;
			}
			case 6:
			{
				char ch = NextChar();
				if (ch == 48) state = 3;
				else if (ch == 49) state = 3;
				else if (ch == 50) state = 3;
				else if (ch == 51) state = 3;
				else if (ch == 52) state = 3;
				else if (ch == 53) state = 3;
				else if (ch == 54) state = 3;
				else if (ch == 55) state = 3;
				else if (ch == 56) state = 3;
				else if (ch == 57) state = 3;
				else if (ch == 65) state = 3;
				else if (ch == 66) state = 3;
				else if (ch == 67) state = 3;
				else if (ch == 68) state = 3;
				else if (ch == 69) state = 3;
				else if (ch == 70) state = 3;
				else if (ch == 71) state = 3;
				else if (ch == 72) state = 3;
				else if (ch == 73) state = 3;
				else if (ch == 74) state = 3;
				else if (ch == 75) state = 3;
				else if (ch == 76) state = 3;
				else if (ch == 77) state = 3;
				else if (ch == 78) state = 3;
				else if (ch == 79) state = 3;
				else if (ch == 80) state = 3;
				else if (ch == 81) state = 3;
				else if (ch == 82) state = 3;
				else if (ch == 83) state = 3;
				else if (ch == 84) state = 3;
				else if (ch == 85) state = 3;
				else if (ch == 86) state = 3;
				else if (ch == 87) state = 3;
				else if (ch == 88) state = 3;
				else if (ch == 89) state = 3;
				else if (ch == 90) state = 3;
				else if (ch == 95) state = 3;
				else if (ch == 97) state = 3;
				else if (ch == 98) state = 3;
				else if (ch == 99) state = 3;
				else if (ch == 100) state = 3;
				else if (ch == 101) state = 3;
				else if (ch == 102) state = 3;
				else if (ch == 103) state = 3;
				else if (ch == 104) state = 3;
				else if (ch == 105) state = 3;
				else if (ch == 106) state = 3;
				else if (ch == 107) state = 3;
				else if (ch == 108) state = 3;
				else if (ch == 109) state = 3;
				else if (ch == 110) state = 7;
				else if (ch == 111) state = 3;
				else if (ch == 112) state = 3;
				else if (ch == 113) state = 3;
				else if (ch == 114) state = 3;
				else if (ch == 115) state = 3;
				else if (ch == 116) state = 3;
				else if (ch == 117) state = 3;
				else if (ch == 118) state = 3;
				else if (ch == 119) state = 3;
				else if (ch == 120) state = 3;
				else if (ch == 121) state = 3;
				else if (ch == 122) state = 3;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{return dAssemblerParser::LITERAL;}
					state = 0;
				}
				break;
			}
			case 7:
			{
				char ch = NextChar();
				if (ch == 48) state = 3;
				else if (ch == 49) state = 3;
				else if (ch == 50) state = 3;
				else if (ch == 51) state = 3;
				else if (ch == 52) state = 3;
				else if (ch == 53) state = 3;
				else if (ch == 54) state = 3;
				else if (ch == 55) state = 3;
				else if (ch == 56) state = 3;
				else if (ch == 57) state = 3;
				else if (ch == 58) state = 8;
				else if (ch == 65) state = 3;
				else if (ch == 66) state = 3;
				else if (ch == 67) state = 3;
				else if (ch == 68) state = 3;
				else if (ch == 69) state = 3;
				else if (ch == 70) state = 3;
				else if (ch == 71) state = 3;
				else if (ch == 72) state = 3;
				else if (ch == 73) state = 3;
				else if (ch == 74) state = 3;
				else if (ch == 75) state = 3;
				else if (ch == 76) state = 3;
				else if (ch == 77) state = 3;
				else if (ch == 78) state = 3;
				else if (ch == 79) state = 3;
				else if (ch == 80) state = 3;
				else if (ch == 81) state = 3;
				else if (ch == 82) state = 3;
				else if (ch == 83) state = 3;
				else if (ch == 84) state = 3;
				else if (ch == 85) state = 3;
				else if (ch == 86) state = 3;
				else if (ch == 87) state = 3;
				else if (ch == 88) state = 3;
				else if (ch == 89) state = 3;
				else if (ch == 90) state = 3;
				else if (ch == 95) state = 3;
				else if (ch == 97) state = 3;
				else if (ch == 98) state = 3;
				else if (ch == 99) state = 3;
				else if (ch == 100) state = 3;
				else if (ch == 101) state = 3;
				else if (ch == 102) state = 3;
				else if (ch == 103) state = 3;
				else if (ch == 104) state = 3;
				else if (ch == 105) state = 3;
				else if (ch == 106) state = 3;
				else if (ch == 107) state = 3;
				else if (ch == 108) state = 3;
				else if (ch == 109) state = 3;
				else if (ch == 110) state = 3;
				else if (ch == 111) state = 3;
				else if (ch == 112) state = 3;
				else if (ch == 113) state = 3;
				else if (ch == 114) state = 3;
				else if (ch == 115) state = 3;
				else if (ch == 116) state = 3;
				else if (ch == 117) state = 3;
				else if (ch == 118) state = 3;
				else if (ch == 119) state = 3;
				else if (ch == 120) state = 3;
				else if (ch == 121) state = 3;
				else if (ch == 122) state = 3;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{return dAssemblerParser::LITERAL;}
					state = 0;
				}
				break;
			}
			case 8:
			{
				{
					GetLexString ();
					//user specified action
					{return dAssemblerParser::BEGIN;}
					state = 0;
				}
				break;
			}
			case 3:
			{
				char ch = NextChar();
				if (ch == 48) state = 3;
				else if (ch == 49) state = 3;
				else if (ch == 50) state = 3;
				else if (ch == 51) state = 3;
				else if (ch == 52) state = 3;
				else if (ch == 53) state = 3;
				else if (ch == 54) state = 3;
				else if (ch == 55) state = 3;
				else if (ch == 56) state = 3;
				else if (ch == 57) state = 3;
				else if (ch == 65) state = 3;
				else if (ch == 66) state = 3;
				else if (ch == 67) state = 3;
				else if (ch == 68) state = 3;
				else if (ch == 69) state = 3;
				else if (ch == 70) state = 3;
				else if (ch == 71) state = 3;
				else if (ch == 72) state = 3;
				else if (ch == 73) state = 3;
				else if (ch == 74) state = 3;
				else if (ch == 75) state = 3;
				else if (ch == 76) state = 3;
				else if (ch == 77) state = 3;
				else if (ch == 78) state = 3;
				else if (ch == 79) state = 3;
				else if (ch == 80) state = 3;
				else if (ch == 81) state = 3;
				else if (ch == 82) state = 3;
				else if (ch == 83) state = 3;
				else if (ch == 84) state = 3;
				else if (ch == 85) state = 3;
				else if (ch == 86) state = 3;
				else if (ch == 87) state = 3;
				else if (ch == 88) state = 3;
				else if (ch == 89) state = 3;
				else if (ch == 90) state = 3;
				else if (ch == 95) state = 3;
				else if (ch == 97) state = 3;
				else if (ch == 98) state = 3;
				else if (ch == 99) state = 3;
				else if (ch == 100) state = 3;
				else if (ch == 101) state = 3;
				else if (ch == 102) state = 3;
				else if (ch == 103) state = 3;
				else if (ch == 104) state = 3;
				else if (ch == 105) state = 3;
				else if (ch == 106) state = 3;
				else if (ch == 107) state = 3;
				else if (ch == 108) state = 3;
				else if (ch == 109) state = 3;
				else if (ch == 110) state = 3;
				else if (ch == 111) state = 3;
				else if (ch == 112) state = 3;
				else if (ch == 113) state = 3;
				else if (ch == 114) state = 3;
				else if (ch == 115) state = 3;
				else if (ch == 116) state = 3;
				else if (ch == 117) state = 3;
				else if (ch == 118) state = 3;
				else if (ch == 119) state = 3;
				else if (ch == 120) state = 3;
				else if (ch == 121) state = 3;
				else if (ch == 122) state = 3;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{return dAssemblerParser::LITERAL;}
					state = 0;
				}
				break;
			}
			case 1:
			{
				char ch = NextChar();
				if (ch == 48) state = 3;
				else if (ch == 49) state = 3;
				else if (ch == 50) state = 3;
				else if (ch == 51) state = 3;
				else if (ch == 52) state = 3;
				else if (ch == 53) state = 3;
				else if (ch == 54) state = 3;
				else if (ch == 55) state = 3;
				else if (ch == 56) state = 3;
				else if (ch == 57) state = 3;
				else if (ch == 65) state = 3;
				else if (ch == 66) state = 3;
				else if (ch == 67) state = 3;
				else if (ch == 68) state = 3;
				else if (ch == 69) state = 3;
				else if (ch == 70) state = 3;
				else if (ch == 71) state = 3;
				else if (ch == 72) state = 3;
				else if (ch == 73) state = 3;
				else if (ch == 74) state = 3;
				else if (ch == 75) state = 3;
				else if (ch == 76) state = 3;
				else if (ch == 77) state = 3;
				else if (ch == 78) state = 3;
				else if (ch == 79) state = 3;
				else if (ch == 80) state = 3;
				else if (ch == 81) state = 3;
				else if (ch == 82) state = 3;
				else if (ch == 83) state = 3;
				else if (ch == 84) state = 3;
				else if (ch == 85) state = 3;
				else if (ch == 86) state = 3;
				else if (ch == 87) state = 3;
				else if (ch == 88) state = 3;
				else if (ch == 89) state = 3;
				else if (ch == 90) state = 3;
				else if (ch == 95) state = 3;
				else if (ch == 97) state = 3;
				else if (ch == 98) state = 3;
				else if (ch == 99) state = 3;
				else if (ch == 100) state = 3;
				else if (ch == 101) state = 3;
				else if (ch == 102) state = 3;
				else if (ch == 103) state = 3;
				else if (ch == 104) state = 3;
				else if (ch == 105) state = 3;
				else if (ch == 106) state = 3;
				else if (ch == 107) state = 3;
				else if (ch == 108) state = 3;
				else if (ch == 109) state = 3;
				else if (ch == 110) state = 3;
				else if (ch == 111) state = 3;
				else if (ch == 112) state = 3;
				else if (ch == 113) state = 3;
				else if (ch == 114) state = 3;
				else if (ch == 115) state = 3;
				else if (ch == 116) state = 3;
				else if (ch == 117) state = 3;
				else if (ch == 118) state = 3;
				else if (ch == 119) state = 3;
				else if (ch == 120) state = 3;
				else if (ch == 121) state = 3;
				else if (ch == 122) state = 3;
				else {
					UnGetChar();
					GetLexString ();
					//user specified action
					{return dAssemblerParser::LITERAL;}
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



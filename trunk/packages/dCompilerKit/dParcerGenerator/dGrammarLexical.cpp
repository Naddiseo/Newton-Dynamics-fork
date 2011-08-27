#include "dParcerCompiler.h"
#include "dGrammarLexical.h"

dGrammarLexical::dGrammarLexical(const char* const data)
	:dParcerLexical(data)
{
}

dGrammarLexical::~dGrammarLexical(void)
{
}


void dGrammarLexical::ReadUserAction()
{
	int state = 0;
	while (state != 14) 
	{
		switch (state) 
		{
			// ([\}]+[ \n]*[;\|])|([\"][^"]*[\"])
			case 0:
			{
				char ch = NextChar();
				if (ch == '\"') state = 1;
				else if (ch == '}') state = 10;
				else state = 0;
				break;
			}

			case 1:
			{
				char ch = NextChar();
				if (ch == '\"') state = 0;
				else if (ch != '\n') state = 2;
				else state = 0;
				break;
			}

			case 2:
			{
				char ch = NextChar();
				if (ch == '\"') {
					if (m_data[m_index-2] == '\\') state = 2;
					else state = 0;
				}

				else if (ch != '\n') state = 2;
				else state = 0;
				break;
			}


			case 10:
			{
				char ch = NextChar();
				if (ch == '}') state = 10;
				else if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r') state = 12;
				else if (ch == '|' || ch == ';') state = 13;
				else state = 0;
				break;
			}

			case 12:
			{
				char ch = NextChar();
				if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r') state = 12;
				else if (ch == '|' || ch == ';') state = 13;
				else state = 0;
				break;
			}

			case 13:
			{
				m_index --;
				state = 14;
				break;
			}
		}
	}

	GetLexString();
	m_tokenString = "{" + m_tokenString;
}

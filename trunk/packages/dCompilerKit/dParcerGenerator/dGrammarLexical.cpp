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
	int count = 1;
	while (count) {
		int ch = NextChar();
		if (ch == '\n') {
			m_lineNumber ++;
		}

		if(ch == '{') {
			count ++;
		} else if (ch == '}') {
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

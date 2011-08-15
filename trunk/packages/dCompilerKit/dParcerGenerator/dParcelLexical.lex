
/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/


%{
//
// Parcel Generator Lexical
//
%}

%{
#include <dParcelCompiler.h>
#include "dParcelLexical.h"
%}


%{
// read the user action 
void ReadUserAction(dParcelLexical& lexical)
{

	int state = 0;
	while (state != 14) 
	{
		switch (state) {

			// ([\}]+[ \n]*[;\|])|([\"][^"]*[\"])
			case 0:
			{
				char ch = lexical.NextChar();
				if (ch == '\"') state = 1;
				else if (ch == '}') state = 10;
				else state = 0;
				break;
			}

			case 1:
			{
				char ch = lexical.NextChar();
				if (ch == '\"') state = 0;
				else if (ch != '\n') state = 2;
				else state = 0;
				break;
			}

			case 2:
			{
				char ch = lexical.NextChar();
				if (ch == '\"') {
					if (lexical.m_data[lexical.m_index-2] == '\\') state = 2;
					else state = 0;
				}
				
				else if (ch != '\n') state = 2;
				else state = 0;
				break;
			}


			case 10:
			{
				char ch = lexical.NextChar();
				if (ch == '}') state = 10;
				else if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r') state = 12;
				else if (ch == '|' || ch == ';') state = 13;
				else state = 0;
				break;
			}

			case 12:
			{
				char ch = lexical.NextChar();
				if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r') state = 12;
				else if (ch == '|' || ch == ';') state = 13;
				else state = 0;
				break;
			}

			case 13:
			{
				lexical.m_index --;
				state = 14;
				break;
			}
		}
	}

	lexical.GetLexString();
	lexical.m_tokenString = "{" + lexical.m_tokenString;
}


%}

WhiteSpace			[ \t\n\r]*

AnyButAstr			[^\*]
AnyButSlash			[^\/]
Comment1			[\/][\/].*
Comment2			[\/][\*]({AnyButAstr}|[\*]{AnyButSlash})*[\*][\/]
Comment				({Comment1}|{Comment2})


AnyButPercent		[^\%]
AnyButCloseCurly	[^\}]
CarryReturn			[\r\n]
CodeBlock			[\%][\{]({AnyButPercent}|{CarryReturn}|([\%]+({AnyButCloseCurly}|{CarryReturn})))*[\%]+[\}]


Literal				[a-zA-Z_][0-9a-zA-Z_]*



%%
[ \t\n\r]+			{}
[|]					{ return(dParcelCompiler::OR); }
[:]					{ return(dParcelCompiler::COLOM); }
[;]					{ return(dParcelCompiler::SIMICOLOM); }
"';'"				{ return(';'); }
"'{'"				{ return('{'); }
"'}'"				{ return('}'); }
"','"				{ return(','); }
"':'"				{ return(':'); }
"'='"				{ return('='); }
"'&'"				{ return('&'); }
"'!'"				{ return('!'); }
"'~'"				{ return('~'); }
"'-'"				{ return('-'); }
"'%'"				{ return('%'); }
"'<'"				{ return('<'); }
"'>'"				{ return('>'); }
"'/'"				{ return('/'); }
"'^'"				{ return('^'); }
"'\.'"				{ return('.'); }
"'\|'"				{ return('|'); }
"'\?'"				{ return('?'); }
"'\\'"				{ return('\\'); }
"'\('"				{ return('('); }
"'\)'"				{ return(')'); }
"'\+'"				{ return('+'); }
"'\*'"				{ return('*'); }
"'\['"				{ return('['); }
"'\]'"				{ return(']'); }


"%%"				{ return dParcelCompiler::GRAMMAR_SEGEMENT;}
"%token"			{ return dParcelCompiler::TOKEN;}
"%left"				{ return dParcelCompiler::LEFT;}
"%right"			{ return dParcelCompiler::RIGHT;}
"%start"			{ return dParcelCompiler::START;}
{Literal}			{ return dParcelCompiler::LITERAL;}
{CodeBlock}			{ return dParcelCompiler::CODE_BLOCK;}
[{]					{ ReadUserAction(*this); return dParcelCompiler::USER_ACTION;}
{Comment}			{}

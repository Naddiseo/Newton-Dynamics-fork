
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
#include <dParcerCompiler.h>
#include "dGrammarLexical.h"
%}

				
WhiteSpace			[ \t\n\r]+

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
[a][bc]	{}

/*
{WhiteSpace}		{}

[|]					{ return(dParcerCompiler::OR); }
[:]					{ return(dParcerCompiler::COLOM); }
[;]					{ return(dParcerCompiler::SIMICOLOM); }
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


"%%"				{ return dParcerCompiler::GRAMMAR_SEGEMENT;}
"%union"			{ return dParcerCompiler::UNION;}
"%token"			{ return dParcerCompiler::TOKEN;}
"%left"				{ return dParcerCompiler::LEFT;}
"%right"			{ return dParcerCompiler::RIGHT;}
"%start"			{ return dParcerCompiler::START;}
{Literal}			{ return dParcerCompiler::LITERAL;}
{CodeBlock}			{ m_tokenString.replace(0, 2, ""); m_tokenString.replace(m_tokenString.size() - 2, 2, ""); return dParcerCompiler::CODE_BLOCK;}
[{]					{ ((dGrammarLexical*)this)->ReadUserAction(); return dParcerCompiler::USER_ACTION;}

{Comment}			{}
*/
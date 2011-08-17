
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
#include <dParcelCompiler.h>
#include "dGrammarLexical.h"
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
[{]					{ ((dGrammarLexical*)this)->ReadUserAction(); return dParcelCompiler::USER_ACTION;}
{Comment}			{}

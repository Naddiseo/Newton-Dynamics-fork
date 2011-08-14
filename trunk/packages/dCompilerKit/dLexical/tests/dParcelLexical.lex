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
//#include <dVirtualMachine.h>
%}

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
[|]					{ return(OR); }
[:]					{ return(COLOM); }
[;]					{ return(SIMICOLOM); }
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


"%%"				{ return GRAMMAR_SEGEMENT;}
"%token"			{ return TOKEN;}
"%left"				{ return LEFT;}
"%right"			{ return RIGHT;}
"%start"			{ return START;}


{Comment}			{}
{Literal}			{ return LITERAL;}
{CodeBlock}			{ return CODE_BLOCK;}







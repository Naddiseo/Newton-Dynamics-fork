
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
// Newton Scrip Lex parcel
// based on a subset of Java language specification 1.0 
//
%}

%{
#include <dVirtualMachine.h>
#include "dAssemblerParcer.h"
%}

WhiteSpace		[ \t\n\r]+

AnyButAstr		[^\*]
AnyButSlash		[^\/]
Comment1        [\/][\/].*
Comment2        [\/][\*]({AnyButAstr}|[\*]{AnyButSlash})*[\*][\/]
Comment			({Comment1}|{Comment2})

Integer			[\-\+]?[0-9]+
Float			{Integer}[\.][0-9]+(e{Integer})?



Register		[rR][0-9]+
loadI			[lL][oO][aA][dD][iI]
add				[aA][dD][dD]
ret				[rR][eE][tT]



Byte			[bB][yY][tT][eE]	
Word			[wW][oO][rR][dD]	
DWord			[dD]{Word}	
Double			[dD][oO][uU][bB][lL][eE]	
Offset			[oO][fF][fF][sS][eE][tT]

Literal			[a-zA-Z_][0-9a-zA-Z_]*

%%
{WhiteSpace}	{}
{loadI}			{return dAssemblerParcer::LOADI;}
{add}			{return dAssemblerParcer::ADD;}
{ret}			{return dAssemblerParcer::RET;}



":"				{return(':'); }
","				{return(','); }

"begin"			{return dAssemblerParcer::BEGIN;}
"end"			{return dAssemblerParcer::END;}
"include"		{return dAssemblerParcer::INCLUDE;}
"public"		{return dAssemblerParcer::PUBLIC;}
"data:"			{return dAssemblerParcer::DATASEGMENT;}
"code:"			{return dAssemblerParcer::CODESEGMENT;}


{Byte}			{return dAssemblerParcer::BYTE;}
{Word}			{return dAssemblerParcer::WORD;}
{DWord}			{return dAssemblerParcer::DWORD;}
{Double}		{return dAssemblerParcer::DOUBLE;}
{Offset}		{return dAssemblerParcer::INTERGER;}	

{Float}			{return dAssemblerParcer::FLOAT;}
{Integer}		{return dAssemblerParcer::INTERGER;}


{Register}		{return dAssemblerParcer::REGISTER;}

{Literal}		{return dAssemblerParcer::LITERAL;}

{Comment}		{}

.+				{}

%%
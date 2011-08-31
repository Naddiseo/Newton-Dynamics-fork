
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
// Newton Script Lex parcel
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

UnsignedInt		[0-9]+
SignedInt		[\-\+]?{UnsignedInt}

Register		[rR]{UnsignedInt}
loadI			[lL][oO][aA][dD][iI]
add				[aA][dD][dD]
ret				[rR][eE][tT]

Literal			[a-zA-Z_][0-9a-zA-Z_]*

%%
{WhiteSpace}	{}

{loadI}			{return dAssemblerParcer::LOADI;}
{add}			{return dAssemblerParcer::ADD;}
{ret}			{return dAssemblerParcer::RET;}

"begin"			{return dAssemblerParcer::BEGIN;}
"end"			{return dAssemblerParcer::END;}
"include"		{return dAssemblerParcer::INCLUDE;}
"public"		{return dAssemblerParcer::PUBLIC;}
"data:"			{return dAssemblerParcer::DATASEGMENT;}
"code:"			{return dAssemblerParcer::CODESEGMENT;}

{SignedInt}		{return dAssemblerParcer::INTERGER;}
{Register}		{return dAssemblerParcer::REGISTER;}

{Literal}		{return dAssemblerParcer::LITERAL;}

{Comment}		{}

%%





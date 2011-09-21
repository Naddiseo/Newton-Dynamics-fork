
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
#include <dVirtualMachine.h>
#include "dAssemblerParser.h"

//
// Newton virtual machine assembler grammar
// based loosely on the MIPS R3000 and the Intel 386 instructions sets 
//
%}


WhiteSpace		[ \t\n\r]+

AnyButAstr		[^\*]
AnyButSlash		[^\/]
Comment1        [\/][\/].*
Comment2        [\/][\*]({AnyButAstr}|[\*]{AnyButSlash})*[\*][\/]
Comment			({Comment1}|{Comment2})


Integer			[\-\+]?[0-9]+
//Float			{Integer}[\.][0-9]+(e{Integer})?

Literal			[a-zA-Z_][0-9a-zA-Z_]*

Register		[rR][0-9]+

add				[aA][dD][dD]
lea				[lL][eE][aA]

loadd			[lL][oO][aA][dD][dD]


%%
{WhiteSpace}	{/* skip is a white space*/}
{Comment}		{/* skip commnets */}


[,]				{return ',';}
[;]				{return ';';}
[.]				{return '.';}
[<]				{return '<';}
[>]				{return '>';}
[=]				{return '=';}
//[+]			{return '+';}	
//[*]			{return '*';}	
//[(]			{return '(';}	
//[)]			{return ')';}	

{Integer}		{return dAssemblerParser::INTEGER;}

{Literal}		{return dAssemblerParser::LITERAL;}


{Register}		{return dAssemblerParser::REGISTER;}

"int"			{return dAssemblerParser::INT;}
"import"		{return dAssemblerParser::IMPORT;}
"private"		{return dAssemblerParser::PRIVATE;}

"begin:"		{return dAssemblerParser::BEGIN;}

"end:"			{return dAssemblerParser::END;}
{loadd}			{return dAssemblerParser::LOADD;}

{lea}			{return dAssemblerParser::LEA;}
{add}			{return dAssemblerParser::ADD;}








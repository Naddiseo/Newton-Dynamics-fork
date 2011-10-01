
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
#include "dScriptCompiler.h"
#include "dNewtonScriptParser.h"

//
// Newton Script lexer 
// loosely based on a subset of Java and C sharp
//
%}


WhiteSpace		[ \t\n\r]+

AnyButAstr		[^\*]
AnyButSlash		[^\/]
Comment1        [\/][\/].*
Comment2        [\/][\*]({AnyButAstr}|[\*]{AnyButSlash})*[\*][\/]
Comment			({Comment1}|{Comment2})


Indetifier		[a-zA-Z_][0-9a-zA-Z_]*



Integer			[\-\+]?[0-9]+
//Float			{Integer}[\.][0-9]+(e{Integer})?



%%


{WhiteSpace}	{/* skip is a white space*/}
{Comment}		{/* skip commnets */}


{Indetifier}	{return dNewtonScriptParser::IDENTIFIER;}


";"				{return ';';}
":"				{return ':';}
","				{return ',';}
"{"				{return '{';}
"}"				{return '}';}

"!"				{return '!';}
"~"				{return '~';}
"&"				{return '&';}
"="				{return '=';}
"\+"			{return '+';}
"\-"			{return '-';}
"\*"			{return '*';}
"/"				{return '/';}
"%"				{return '%';}
"\."			{return '.';}
"\["			{return '[';}
"\]"			{return ']';}
"\("			{return '(';}
"\)"			{return ')';}
"\^"			{return '^';}
"\|"			{return '|';}
"<"				{return '<';}
">"				{return '>';}

"&&"			{return dNewtonScriptParser::LOGIC_AND;}
"\|\|"			{return dNewtonScriptParser::LOGIC_OR;}
"<<"			{return dNewtonScriptParser::SHIFT_LEFT;}
">>"			{return dNewtonScriptParser::SHIFT_RIGHT;}
"<="			{return dNewtonScriptParser::LESS_EQUAL;}
">="			{return dNewtonScriptParser::GREATHER_EQUAL;}

"=="			{return dNewtonScriptParser::IDENTICAL;}
"!="			{return dNewtonScriptParser::DIFFERENT;}
"::"			{return dNewtonScriptParser::DOUBLE_COLOM;}
 

"void"			{return dNewtonScriptParser::VOID_TYPE;}
"bool"			{return dNewtonScriptParser::BOOLEAN;}
"byte"			{return dNewtonScriptParser::BYTE;}
"short"			{return dNewtonScriptParser::SHORT;}
"int"			{return dNewtonScriptParser::INT;}
"long"			{return dNewtonScriptParser::LONG;}
"unsigned"		{return dNewtonScriptParser::UNSIGNED;}
"float"			{return dNewtonScriptParser::FLOAT;}
"double"		{return dNewtonScriptParser::DOUBLE;}

"sizeof"		{return dNewtonScriptParser::SIZEOF;}
"cast"			{return dNewtonScriptParser::CAST;}
"new"			{return dNewtonScriptParser::NEW;}
"if"			{return dNewtonScriptParser::IF;}
"else"			{return dNewtonScriptParser::ELSE;}
"for"			{return dNewtonScriptParser::FOR;}
"while"			{return dNewtonScriptParser::WHILE;}
"do"			{return dNewtonScriptParser::DO;}
"switch"		{return dNewtonScriptParser::SWITCH;}
"case"			{return dNewtonScriptParser::CASE;}
"default"		{return dNewtonScriptParser::DEFAULT;}
"break"			{return dNewtonScriptParser::BREAK;}
"continue"		{return dNewtonScriptParser::CONTINUE;}
"return"		{return dNewtonScriptParser::RETURN;}


"gui"			{return dNewtonScriptParser::GUI;}
"const"			{return dNewtonScriptParser::CONSTANT;}
"native"		{return dNewtonScriptParser::NATIVE;}
"extends"		{return dNewtonScriptParser::EXTENDS;}
"private"		{return dNewtonScriptParser::PRIVATE;}
"base"			{return dNewtonScriptParser::BASE;}
"class"			{return dNewtonScriptParser::CLASS;}
"import"		{return dNewtonScriptParser::IMPORT;}


{Integer}		{return dNewtonScriptParser::INTEGER_VALUE;}












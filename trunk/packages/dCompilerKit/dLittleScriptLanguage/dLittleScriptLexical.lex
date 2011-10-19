
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
#include "dLSCstdafx.h"
#include "dLittleScriptParser.h"

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


Indentifier		[a-zA-Z_][0-9a-zA-Z_]*



Integer			[0-9]+
Float			{Integer}[\.][0-9]+(e{Integer})?
string			["][^"]*["]




%%


{WhiteSpace}	{/* skip is a white space*/}
{Comment}		{/* skip commnets */}


{Indentifier}	{m_tokenString = '_' + m_tokenString ; return dLittleScriptParser::IDENTIFIER;}


";"				{return ';';}
":"				{return ':';}
","				{return ',';}
"{"				{return '{';}
"}"				{return '}';}
"!"				{return '!';}
"~"				{return '~';}
"&"				{return '&';}
"="				{return '=';}
"/"				{return '/';}
"%"				{return '%';}
"\+"			{return '+';}
"\-"			{return '-';}
"\*"			{return '*';}
"\."			{return '.';}
"\?"			{return '?';}
"\["			{return '[';}
"\]"			{return ']';}
"\("			{return '(';}
"\)"			{return ')';}
"\^"			{return '^';}
"\|"			{return '|';}
"<"				{return '<';}
">"				{return '>';}

"&&"			{return dLittleScriptParser::LOGIC_AND;}
"\|\|"			{return dLittleScriptParser::LOGIC_OR;}
"<<"			{return dLittleScriptParser::SHIFT_LEFT;}
">>"			{return dLittleScriptParser::SHIFT_RIGHT;}
"<="			{return dLittleScriptParser::LESS_EQUAL;}
">="			{return dLittleScriptParser::GREATHER_EQUAL;}

"=="			{return dLittleScriptParser::IDENTICAL;}
"!="			{return dLittleScriptParser::DIFFERENT;}
"::"			{return dLittleScriptParser::DOUBLE_COLOM;}
 

"void"			{return dLittleScriptParser::VOID_TYPE;}
"enum"			{return dLittleScriptParser::ENUM;}
"bool"			{return dLittleScriptParser::BOOLEAN;}
"byte"			{return dLittleScriptParser::BYTE;}
"short"			{return dLittleScriptParser::SHORT;}
"int"			{return dLittleScriptParser::INT;}
"long"			{return dLittleScriptParser::LONG;}
"unsigned"		{return dLittleScriptParser::UNSIGNED;}
"float"			{return dLittleScriptParser::FLOAT;}
"double"		{return dLittleScriptParser::DOUBLE;}

"operator"		{return dLittleScriptParser::OPERATOR;}
"sizeof"		{return dLittleScriptParser::SIZEOF;}
"cast"			{return dLittleScriptParser::CAST;}
"new"			{return dLittleScriptParser::NEW;}
"if"			{return dLittleScriptParser::IF;}
"else"			{return dLittleScriptParser::ELSE;}
"for"			{return dLittleScriptParser::FOR;}
"while"			{return dLittleScriptParser::WHILE;}
"do"			{return dLittleScriptParser::DO;}
"switch"		{return dLittleScriptParser::SWITCH;}
"case"			{return dLittleScriptParser::CASE;}
"default"		{return dLittleScriptParser::DEFAULT;}
"break"			{return dLittleScriptParser::BREAK;}
"continue"		{return dLittleScriptParser::CONTINUE;}
"return"		{return dLittleScriptParser::RETURN;}


"gui"			{return dLittleScriptParser::GUI;}
"const"			{return dLittleScriptParser::CONSTANT;}
"native"		{return dLittleScriptParser::NATIVE;}
"extends"		{return dLittleScriptParser::EXTENDS;}
"private"		{return dLittleScriptParser::PRIVATE;}
"base"			{return dLittleScriptParser::BASE;}
"class"			{return dLittleScriptParser::CLASS;}
"import"		{return dLittleScriptParser::IMPORT;}

{Float}			{return dLittleScriptParser::FLOAT_VALUE;}
{Integer}		{return dLittleScriptParser::INTEGER_VALUE;}
{string}		{return dLittleScriptParser::STRING_VALUE;}	











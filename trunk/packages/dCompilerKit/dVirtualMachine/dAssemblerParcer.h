/* Copych1 (c) <2009> <Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

//
//Auto generated Parcer Generator class: dAssemblerParcer.h
//

#ifndef __dAssemblerParcer_h__
#define __dAssemblerParcer_h__

class dParcelLexical;

class dAssemblerParcer
{
	public:
	enum Token
	{
		END = 256,
		INCLUDE,
		LITERAL,
		LITERAL,
		DATASEGMENT,
		WORD,
		CODESEGMENT,
		PUBLIC,
		LITERAL,
		BEGINFUNCTION,
		LITERAL,
		ENDFUNCTION,
		LITERAL,
		MOV,
		REGISTER,
		REGISTER,
		ADD,
		REGISTER,
		REGISTER,
		REGISTER
	};

	dAssemblerParcer();
	virtual ~dAssemblerParcer();
	virtual int Parce(dParcelLexical& lexical);

};

#endif

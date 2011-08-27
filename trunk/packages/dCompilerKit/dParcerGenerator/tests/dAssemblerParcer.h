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


class dAssemblerLexical;

class dAssemblerParcer
{
	public:
	enum Token
	{
		END = 256,
		INCLUDE,
		LITERAL,
		DATASEGMENT,
		WORD,
		CODESEGMENT,
		PUBLIC,
		BEGINFUNCTION,
		ENDFUNCTION,
		MOV,
		REGISTER,
		ADD
	};

	dAssemblerParcer();
	virtual ~dAssemblerParcer();
	virtual int Parce(dAssemblerLexical& scanner);

};

#endif

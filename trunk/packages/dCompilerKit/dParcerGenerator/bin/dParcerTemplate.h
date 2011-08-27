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
//Auto generated Parcer Generator class: $(className).h
//

#ifndef __$(className)_h__
#define __$(className)_h__


class $(scannerClass);

class $(className)
{
	public:
	enum Token
	{
$(Tokens)
	};

	$(className)();
	virtual ~$(className)();
	virtual int Parce($(scannerClass)& scanner);

};

#endif

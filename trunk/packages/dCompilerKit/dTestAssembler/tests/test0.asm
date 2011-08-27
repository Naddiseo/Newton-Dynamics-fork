
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

// Assembler program test0.asm


data:
	offset mem0		10, 10	
	dword  value0	1000	
	double value2	1000.25e-2	

code:

// add two integer values
begin testFunction: public
	loadi	r3, 0			// clear registe
	loadd	r2, [r3].value0	// load 10 to register1
	loadi	r1, mem1		// load 2 to register2
	add		r1, r2, r0		// add r0 = r1 + r2
	ret						// return the value in register0	
end testFunction	

end


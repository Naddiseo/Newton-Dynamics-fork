
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


code:

// add two integer values
begin testFuntion: public
	loadi	r2, 10			// load 10 to register1
	loadi	r1, 20			// load 2 to register2
	add		r1, r2, r0		// add r0 = r1 + r2
	ret						// return the value in register0	
end testFuntion	

end


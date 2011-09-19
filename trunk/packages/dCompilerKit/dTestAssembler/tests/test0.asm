b

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



int b = 20
int a = 10


begin: function_One
	
	addi	r0, 0
	loadd	r1, a	
	loadd	r2, b	
	add		r1, r2
	ret		r31
	
end:
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


// the code assume the risk convention for that r0 is always 0
// reg 31 is assume to be teh stack 

// calculate fibonacci value of register r1
begin: fibonacci
	addi	r29, 1					// preload const one into register r29
	call	fibonacci_kerner		// calculate fibonacci of r1
	ret		
end:


begin: fibonacci_kerner
	ble		r1, r29, exit_test	// see if the value on r1 is less or equal to 1
	
	sub		r1, r29				// r1 = r1 - 1 			
	push	r1					// save r1 
	call	fibonacci_kerner	// calculate fibonacci(r1 - 1)
	pop		r2					// restore r1
	
	push	r1					// save fibonacci(r1 - 1) 
	mov		r1, r2				
	sub		r1, r29				// r1 = r1 - 2
	call	fibonacci_kerner	// calculate fibonacci (r1 - 2)
	pop		r2					// restore fibonacci of r1 - 1
	add		r1, r2				// calculate r1 = fibonacci (r1 - 1) + fibonacci (r1 - 2)
	
exit_test:	
	ret		
end:
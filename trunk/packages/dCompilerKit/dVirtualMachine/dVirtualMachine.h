#ifndef __dVirtualMachine_h__
#define __dVirtualMachine_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#include <dContainersStdAfx.h>

#ifdef _MSC_VER
#pragma warning (disable: 4201) // warning C4201: nonstandard extension used : nameless struct/union
#endif




class dVirtualMachine
{
	public:
	enum dMachineInstrution
	{
		// type0 opcode
		ret,			// ret						pc = [rn]; rn += 4

		// type1 opcode
		jump,		// jump		imm32				pc += imm32
		call,		// call		imm32				rn -= 4; [rn] = pc + 4; pc == imm32 

		// type2 opcode
		callr,		// callr	Ri					rn -= 4; [rn] = pc + 4; pc == ri 
		jumpr,		// jump		Ri					pc = ri
		push,		// push		Ri					rn -= 4; [rn] = r1;	
		pop,		// pop		Ri					r1 = [rn]; rn += 4;

		// type3 opcode
		mov,		// move		Ri, Rj				R(i) = R(j)
		add,		// add  	Ri, Rj				R(i) = R(i) + R(j)
		sub,		// sub  	Ri, Rj				R(i) = R(i) - R(j)
		mul,		// mul  	Ri, Rj				R(i) = lowpart (R(i) * R(j)) ; R(j) = upperpart (R(i) * R(j))
		div,		// div  	Ri, Rj				R(i) = num (R(i) / R(j)) ; R(j) = recidual (R(i) / R(j))
		abs,		// abs		Ri, Rj				R(i) = abs (R(j)) 
		neg,		// neg		Ri, Rj				R(i) = - R(j) 
		and,		// and  	Ri, Rj				R(i) = R(i) & R(j)
		or,			// or		Ri, Rj				R(i) = R(j) | R(j)
		xor,		// xor		Ri, Rj				R(i) = R(j) ^ R(j)	
		not,		// xor		Ri, Rj				R(i) = ~R(j)
		sll,		// sll		Ri, Rj				R(i) = R(i) << R(j)	
		srl,		// srl		Ri, Rj				R(i) = R(i) >> R(j)	

		// type4 opcode
		addi,		// addi 	Ri, Rj, imm32		R(i) = R(j) + imm32

		beq,		// beq		Ri, Rj, imm32  		if (R(i) == R(j)) PC = PC + imm32  
		bne,		// bneq		Ri, Rj, imm32  		if (R(i) != R(j)) PC = PC + imm32  
		blt,		// blt		Ri, Rj, imm32  		if (R(i) < R(j)) PC = PC + imm32  (signed comparison)
		ble,		// ble		Ri, Rj, imm32  		if (R(i) <= R(j)) PC = PC + imm32  (signed comparison)
		bgt,		// bgt		Ri, Rj, imm32  		if (R(i) > R(j)) PC = PC + imm32  (signed comparison)
		bget,		// bget		Ri, Rj, imm32  		if (R(i) >= R(j)) PC = PC + imm32  (signed comparison)

		loadb,		// loadb	Ri, imm32[Rj]		R(i) = byte at memory [R(j) + imm32]	
		loadw,		// loadq	Ri, imm32[Rj]		R(i) = word at memory [R(j) + imm32]	
		loadd,		// loadd	Ri, imm32[Rj]		R(i) = dword at memory [R(j) + imm32]
		storeb,		// storeb	Ri, imm32[Rj]		byte at memory [R(j) + imm32] = R(i)
		storew,		// storeq	Ri, imm32[Rj]		word at memory [R(j) + imm32] = R(i)
		stored,		// stored	Ri, imm32[Rj]		dword at memory [R(j) + imm32] = R(i)

		syscall,	// syscall	Ri, Rj, imm32		execute a system call

		nop,		// do nothing 
	};

	struct dNemonic
	{
		dMachineInstrution m_instruction;
		char* m_name;
	};

	union dOpCode
	{
		short m_bytecode;
		struct 
		{
			unsigned	m_opcode	:  6;
			unsigned	m_reg0		:  5;
			unsigned	m_reg1		:  5;
		};
	};


	dVirtualMachine(void);
	virtual ~dVirtualMachine(void);

	int GetRegisterCount() const; 

	protected:
	int m_registerCount;			
	unsigned m_registes[16];
	int m_codeSegementSize;
	dOpCode* m_codeSegment;

	static dNemonic m_nemonics[]; 
};


#endif
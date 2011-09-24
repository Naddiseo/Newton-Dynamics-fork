#ifndef __dVirtualMachine_h__
#define __dVirtualMachine_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#ifdef _MSC_VER
#include <windows.h>
#include <crtdbg.h>
#pragma warning (disable: 4201) // warning C4201: nonstandard extension used : nameless struct/union
#endif


#ifndef DTRACE
	#ifdef _DEBUG
		#include <stdarg.h>

		inline void dExpandTraceMessage (const char *fmt, ...)
		{
			va_list v_args;
			char* const text = (char*) malloc (strlen (fmt) + 2048);

			text[0] = 0;
			va_start (v_args, fmt);     
			vsprintf(text, fmt, v_args);
			va_end (v_args);            

			OutputDebugStringA (text);

			free (text);
		}

		#define DTRACE(x)										\
		{														\
			dExpandTraceMessage x;								\
		}
	#else
		#define DTRACE(x)
	#endif
#endif


class dVirtualMachine
{
	public:
	enum dMachineInstrution
	{
		// type0 opcode
		jumpr,		// jump		Ri					pc = R(i)
		ret,		// ret		Ri					pc = [R(i)], R(i) = R(i) + 4  

		// type1 opcode
		push,		// push		Ri, imm32mask		tmp = R(i); k = 0;  mask = registenMask; while (mask) { while (!(mask & 1)) mask >>= 1; k ++;} {tmp -= 4; [tmp] = Rn[k]}; R(i) = tmp; 
		pop,		// pop		Ri, imm32mask		tmp = R(i); k = 31; mask = registenMask; while (mask) { while (!(mask & (1<<31))) mask <<= 1; k --;} {[tmp] = Rn[k]; tmp -= 4;}
		call,		// call		Ri, imm32			R(i) = R(i) - 4, [R(i)] = pc + 6, pc = imm32

		// type2 opcode
		addi,		// addi 	Ri, Rj, imm32		R(i) = R(j) + imm32
		beq,		// beq		Ri, Rj, imm32  		if (R(i) == R(j)) PC = PC + imm32  
		bne,		// bneq		Ri, Rj, imm32  		if (R(i) != R(j)) PC = PC + imm32  
		blt,		// blt		Ri, Rj, imm32  		if (R(i) < R(j)) PC = PC + imm32  (signed comparison)
		ble,		// ble		Ri, Rj, imm32  		if (R(i) <= R(j)) PC = PC + imm32  (signed comparison)
		bgt,		// bgt		Ri, Rj, imm32  		if (R(i) > R(j)) PC = PC + imm32  (signed comparison)
		bget,		// bget		Ri, Rj, imm32  		if (R(i) >= R(j)) PC = PC + imm32  (signed comparison)


		// move
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

		// load 
		loadb,		// loadb	Ri, imm32[Rj]		R(i) = byte at memory [R(j) + imm32]	
		loadw,		// loadq	Ri, imm32[Rj]		R(i) = word at memory [R(j) + imm32]	
		loadd,		// loadd	Ri, imm32[Rj]		R(i) = dword at memory [R(j) + imm32]

		// store 
		storeb,		// storeb	Ri, imm32[Rj]		byte at memory [R(j) + imm32] = R(i)
		storew,		// storeq	Ri, imm32[Rj]		word at memory [R(j) + imm32] = R(i)
		stored,		// stored	Ri, imm32[Rj]		dword at memory [R(j) + imm32] = R(i)


		// call / return 

		callr,		// call		Ri, Rj				R(i) = R(i) - 4, [R(i)] = pc + 6, pc = R(j)
		
		syscall,	// syscall	Ri, Rj, imm32		execute a system call
		
		jump,		// jump		imm32					pc += imm32

		// frame pointer 
		enter,		// enter	Ri, Rj, imm32		[R(j)] = R(i), R(i) = R(j), R(j) = R(j) + imm32	
		exit,		// exit		Ri, Rj				R(j) = R(i), R(i) = [R(j)]

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
#ifndef __dVirtualMachine_h__
#define __dVirtualMachine_h__



class dVirtualMachine
{
	public:
	enum dMachineInstrution
	{
		// move
		mov,		// move		Ri, Rj				R(i) = R(j)

		// arithmetic
		addi,		// addi 	Ri, Rj, imm32		R(i) = R(j) + signed (imm32)
		add,		// add  	Ri, Rj				R(i) = R(i) + R(j)
		sub,		// sub  	Ri, Rj				R(i) = R(i) - R(j)
		mul,		// mul  	Ri, Rj				R(i) = lowpart (R(i) * R(j)) ; R(j) = upperpart (R(i) * R(j))
		div,		// div  	Ri, Rj				R(i) = nume (R(i) / R(j)) ; R(j) = recidual (R(i) / R(j))
		abs,		// abs		Ri, Rj,				R(i) = abs (R(j)) 
		neg,		// neg		Ri, Rj,				R(i) = - R(j) 
		
		// logical
		and,		// and  	Ri, Rj				R(i) = R(i) & R(j)
		or,			// or		Ri, Rj				R(i) = R(j) | R(j)
		xor,		// xor		Ri, Rj				R(i) = R(j) ^ R(j)	
		not,		// xor		Ri, Rj				R(i) = ~R(j)

		// shift 
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

		// conditional jumps
		beq,		// beq		Ri, Rj, imm32  		if (R(i) == R(j)) PC = PC + imm32  
		bne,		// bneq		Ri, Rj, imm32  		if (R(i) != R(j)) PC = PC + imm32  
		blt,		// blt		Ri, Rj, imm32  		if (R(i) < R(j)) PC = PC + imm32  (signed comparison)
		ble,		// ble		Ri, Rj, imm32  		if (R(i) <= R(j)) PC = PC + imm32  (signed comparison)
		bgt,		// bgt		Ri, Rj, imm32  		if (R(i) > R(j)) PC = PC + imm32  (signed comparison)
		bget,		// bget		Ri, Rj, imm32  		if (R(i) >= R(j)) PC = PC + imm32  (signed comparison)

		// call / return 
		call,		// call		Ri, imm32			R(i) = R(i) - 4, [R(i)] = pc + 6, pc = imm32
		callr,		// call		Ri, Rj				R(i) = R(i) - 4, [R(i)] = pc + 6, pc = R(j)
		ret,		// ret		Ri					pc = [R(i)], R(i) = R(i) + 4  
		syscall,	// syscall	Ri, Rj, imm32		execute a system call
		jump,		// jump		Ri					pc = R(i)

		// frame pointer 
		enter,		// enter	Ri, Rj, imm32		[R(j)] = R(i), R(i) = R(j), R(j) = R(j) + imm32	
		exit,		// exit		Ri, Rj				R(j) = R(i), R(i) = [R(j)]
		push,		// push		Ri, imm32mask		tmp = R(i); k = 0;  mask = registenMask; while (mask) { while (!(mask & 1)) mask >>= 1; k ++;} {tmp -= 4; [tmp] = Rn[k]}; R(i) = tmp; 
		pop,		// pop		Ri, imm32mask		tmp = R(i); k = 31; mask = registenMask; while (mask) { while (!(mask & (1<<31))) mask <<= 1; k --;} {[tmp] = Rn[k]; tmp -= 4;}


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
		struct dType0
		{
			unsigned	m_opcode	:  6;
			unsigned	m_reg0		:  5;
			unsigned	m_reg1		:  5;
		};
	};


	dVirtualMachine(void);
	virtual ~dVirtualMachine(void);

	bool CompileAssembly (const char* const code);

	protected:
	unsigned m_registes[16];
	int m_codeSegementSize;
	dOpCode* m_codeSegment;

	static dNemonic m_nemonics[]; 
};


#endif
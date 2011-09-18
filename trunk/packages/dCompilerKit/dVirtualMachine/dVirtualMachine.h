#ifndef __dVirtualMachine_h__
#define __dVirtualMachine_h__



class dVirtualMachine
{
	public:
	enum dMachineInstrution
	{
		// move
		mov,		// move		Rn0, Rn1			Rn0 = Rn1

		// arithmetic
		addi,		// addi 	Rn0, Rn1, imm32		Rn0 = Rn1 + signed (imm32)
		add,		// add  	Rn0, Rn1			Rn0 = Rn0 + Rn1
		sub,		// sub  	Rn0, Rn1			Rn0 = Rn0 - Rn1
		mul,		// mul  	Rn0, Rn1			Rn0 = lowpart (Rn0 * Rn1) ; Rn1 = upperpart (Rn0 * Rn1)
		div,		// div  	Rn0, Rn1			Rn0 = nume (Rn0 / Rn1) ; Rn1 = recidual (Rn0 / Rn1)
		abs,		// abs		Rn0, Rn1,			Rn0 = abs (Rn1) 
		neg,		// neg		Rn0, Rn1,			Rn0 = - Rn1 
		
		// logical
		and,		// and  	Rn0, Rn1			Rn0 = Rn0 & Rn1
		or,			// or		Rn0, Rn1			Rn0 = Rn1 | Rn1
		xor,		// xor		Rn0, Rn1			Rn0 = Rn1 ^ Rn1	
		not,		// xor		Rn0, Rn1			Rn0 = ~Rn1

		// shift 
		sll,		// sll		Rn0, Rn1			Rn0 = Rn0 << Rn1	
		srl,		// srl		Rn0, Rn1			Rn0 = Rn0 >> Rn1	

		// load 
		loadb,		// loadb	Rn0, imm32[Rn1]		Rn0 = byte at memory [Rn1 + imm32]	
		loadw,		// loadq	Rn0, imm32[Rn1]		Rn0 = word at memory [Rn1 + imm32]	
		loadd,		// loadd	Rn0, imm32[Rn1]		Rn0 = dword at memory [Rn1 + imm32]

		// store 
		storeb,		// storeb	Rn0, Rn1, imm32		byte at memory [Rn1 + imm32] = Rn0
		storew,		// storeq	Rn0, Rn1, imm32		word at memory [Rn1 + imm32] = Rn0
		stored,		// stored	Rn0, Rn1, imm32		dword at memory [Rn1 + imm32] = Rn0

		// conditional jumps
		beq,		// beq		Rn0, Rn1, imm32  	if (Rn0 == Rn1) PC = PC + imm32  
		bne,		// bneq		Rn0, Rn1, imm32  	if (Rn0 != Rn1) PC = PC + imm32  
		blt,		// blt		Rn0, Rn1, imm32  	if (Rn0 < Rn1) PC = PC + imm32  (signed comparison)
		ble,		// ble		Rn0, Rn1, imm32  	if (Rn0 <= Rn1) PC = PC + imm32  (signed comparison)
		bgt,		// bgt		Rn0, Rn1, imm32  	if (Rn0 > Rn1) PC = PC + imm32  (signed comparison)
		bget,		// bget		Rn0, Rn1, imm32  	if (Rn0 >= Rn1) PC = PC + imm32  (signed comparison)

		// call / return 
		call,		// call		Rn0, imm32			Rn0 = Rn0 - 4, [Rn0] = pc + 6, pc = imm32
		callr,		// call		Rn0, Rn1			Rn0 = Rn0 - 4, [Rn0] = pc + 6, pc = Rn1
		ret,		// ret		rn0					pc = [Rn0], Rn0 = Rn0 + 4  
		syscall,	// syscall	Rn0, Rn1, imm32		execute a system call
		jump,		// jump		Rn0					pc = Rn0

		// frame pointer 
		enter,		// enter	Rn0, Rn1, imm32		[Rn1] = Rn0, Rn0 = Rn1, Rn1 = Rn1 + imm32	
		exit,		// exit		Rn0, Rn1			Rn1 = Rn0, Rn0 = [Rn1]
		push,		// push		Rn0, imm32mask		tmp = rn0; i = 0;  mask = registenMask; while (mask) { while (!(mask & 1)) mask >>= 1; i ++;} {tmp -= 4; [tmp] = Rn[i]}; rn0 = tmp; 
		pop,		// pop		Rn0, imm32mask		tmp = rn0; i = 31; mask = registenMask; while (mask) { while (!(mask & (1<<31))) mask <<= 1; i --;} {[tmp] = Rn[i]; tmp -= 4;}


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
		struct dType1
		{
			unsigned	m_opcode	:  6;
			unsigned	m_reg0		:  5;
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
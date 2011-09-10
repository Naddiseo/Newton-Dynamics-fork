#ifndef __dVirtualMachine_h__
#define __dVirtualMachine_h__



class dVirtualMachine
{
	public:
	enum dMachineInstrution
	{
		add,		// add  	Rn0, Rn1, Rn2		Rn0 = Rn1 + Rn2
		sub,		// add  	Rn0, Rn1, Rn2		Rn0 = Rn1 - Rn2
		addi,		// addi 	Rn0, Rn1, offset	Rn0 = Rn1 + offset
		addu,		// add  	Rn0, Rn1, Rn2		Rn0 = Rn1 + Rn2  
		subu,		// add  	Rn0, Rn1, Rn2		Rn0 = Rn1 - Rn2
		and,		// and  	Rn0, Rn1, Rn2		Rn0 = Rn1 & Rn2
		or,			// or		Rn0, Rn1, Rn2		Rn0 = Rn1 | Rn2
		xor,		// xor		Rn0, Rn1, Rn2		Rn0 = Rn1 ^ Rn2	
		sll,		// sll		Rn0, Rn1, Rn2		Rn0 = Rn1 << Rn2	
		srl,		// srl		Rn0, Rn1, Rn2		Rn0 = Rn1 >> Rn2	
		loadb,		// loadb	Rn0, Rn1, offset	Rn0 = byte at memory [Rn1 + offset]	
		loadw,		// loadq	Rn0, Rn1, offset	Rn0 = word at memory [Rn1 + offset]	
		loadd,		// loadd	Rn0, Rn1, offset	Rn0 = dword at memory [Rn1 + offset]	
		storeb,		// storeb	Rn0, Rn1, offset	byte at memory [Rn1 + offset] = Rn0
		storew,		// storeq	Rn0, Rn1, offset	word at memory [Rn1 + offset] = Rn0
		stored,		// stored	Rn0, Rn1, offset	dword at memory [Rn1 + offset] = Rn0
		beq,		// beq		Rn0, Rn1, offset  	if (Rn0 == Rn1) PC = PC + offset  
		bne,		// bneq		Rn0, Rn1, offset  	if (Rn0 != Rn1) PC = PC + offset  
		blt,		// blt		Rn0, Rn1, offset  	if (Rn0 < Rn1) PC = PC + offset  (signed comparison)
		ble,		// ble		Rn0, Rn1, offset  	if (Rn0 <= Rn1) PC = PC + offset  (signed comparison)
		bgt,		// bgt		Rn0, Rn1, offset  	if (Rn0 > Rn1) PC = PC + offset  (signed comparison)
		bget,		// bget		Rn0, Rn1, offset  	if (Rn0 >= Rn1) PC = PC + offset  (signed comparison)
		call,		// call		Rn0, offset			Rn0 = [pc], pc = offset
		callr,		// call		Rn0, Rn1			Rn0 = [pc], pc = Rn1
		syscall,	// syscall	Rn0, Rn1, offset	execute a system call
		jump,		// jump		Rn0					pc = Rn0
		enter,		// enter	Rn0, Rn1, bytes		[Rn1] = Rn0, Rn0 = Rn1, Rn1 = Rn1 + bytes	
		exit,		// exit		Rn0, Rn1, bytes		Rn1 = Rn0, Rn0 = [Rn1]
		push,		// push		Rn0, registerMask	tmp = rn0; i = 0; mask = registenMask; while (mask & 1) {[tmp] = Rn(i); tmp += 4}; rn0 = tmp; 
		pop,		// pop		Rn0, registerMask   tmp = rn0; i = 0; mask = registenMask; while (mask & (1<<15)) {tmp -= 4; Rn(i) = [tmp];}
	};

	struct dNemonic
	{
		dMachineInstrution m_instruction;
		char* m_name;
	};

	union dOpCode
	{
		struct dType0
		{
			unsigned	m_opcode	: 6;
			unsigned	m_reg0		: 4;
			unsigned	m_reg1		: 4;
			unsigned	m_reg2		: 4;
		};
		struct dType1
		{
			unsigned	m_opcode	: 6;
			unsigned	m_reg0		: 4;
			unsigned	m_reg1		: 4;
			unsigned	m_immediate :18;
		};
		struct dType2
		{
			unsigned	m_opcode	: 6;
			unsigned	m_reg0		: 4;
			unsigned	immediate	:22;
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
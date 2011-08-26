#ifndef __dVirtualMachine_h__
#define __dVirtualMachine_h__



class dVirtualMachine
{
	public:
	enum opcode{
		move, 
		loadb, 
		loadw, 
		loadd, 
		loadf, 
		add,
		sub,
	};

	union OpCodeFormat
	{
		struct Aritmetic
		{
			unsigned	m_opcode : 6;
			unsigned	m_reg0	 : 5;
			unsigned	m_reg1	 : 5;
			unsigned	m_reg2	 : 5;
		};
		struct RegiterMove
		{
			unsigned	m_opcode : 6;
			unsigned	m_reg0	 : 5;
			unsigned	m_reg1	 : 5;
		};

		struct LoadStore
		{
			unsigned	m_opcode : 6;
			unsigned	m_reg0	 : 5;
			unsigned	m_reg1	 : 5;
			unsigned	m_reg2	 : 5;
			unsigned	m_offset : 11;
		};
	};


	dVirtualMachine(void);
	virtual ~dVirtualMachine(void);

	protected:
	unsigned m_registes[32];
	int m_codeSegementSize;
	OpCodeFormat* m_codeSegment;

	
};


#endif
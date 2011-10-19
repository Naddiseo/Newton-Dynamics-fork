#ifndef  __dTreeAdressStmt_H_
#define  __dTreeAdressStmt_H_

class dTreeAdressStmt
{
	public:
	enum dOperator
	{
		m_nothing,
		m_sub,
		m_lessEqual,
		m_greatherEqual,
		
	};

	enum dInstruction
	{
		m_nop,
		m_assigment,
		m_if, 
		m_ifnot, 
		m_goto,
	};
	dTreeAdressStmt(void);
	~dTreeAdressStmt(void);

	dInstruction m_instrution;
	dOperator m_operator;
	string m_arg0;
	string m_arg1;
	string m_arg2;
	dList<dTreeAdressStmt>::dListNode* m_jmpTarget;
};


#endif
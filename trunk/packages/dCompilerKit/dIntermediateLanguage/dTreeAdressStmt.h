#ifndef  __dTreeAdressStmt_H_
#define  __dTreeAdressStmt_H_


#define dTRACE_INTRUCTION(x) (x)->Trace();

class dTreeAdressStmt
{
	public:
	enum dOperator
	{
		m_nothing,
		m_add,
		m_sub,
		m_mul,
		m_less,
		m_lessEqual,
		m_greather,
		m_greatherEqual,
		
	};

	enum dInstruction
	{
		m_nop,
		m_assigment,
		m_if, 
		m_ifnot, 
		m_goto,
		m_target,
	};
	dTreeAdressStmt(void);
	~dTreeAdressStmt(void);

	dInstruction m_instrution;
	dOperator m_operator;
	string m_arg0;
	string m_arg1;
	string m_arg2;
	dList<dTreeAdressStmt>::dListNode* m_jmpTarget;

	void Trace () const;
	void TraceAssigment () const;
};


#endif
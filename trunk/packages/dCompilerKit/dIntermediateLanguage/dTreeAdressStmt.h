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

#ifndef  __dTreeAdressStmt_H_
#define  __dTreeAdressStmt_H_


#define dTRACE_INTRUCTION(x) (x)->Trace();

class dTreeAdressStmt
{
	public:
	enum dArgType
	{
		m_intVar,
		m_floatVar,
		m_intConst,
		m_floatConst,
	};

	struct dArg
	{
		dArg ()
			:m_type (m_intVar)
			,m_label()
		{
		}
		dArgType m_type;
		string m_label;
	};


	enum dOperator
	{
		m_nothing,
		m_add,
		m_sub,
		m_mul,
		m_equal,
		m_different,
		m_less,
		m_lessEqual,
		m_greather,
		m_greatherEqual,
		m_operatorsCount,
	};

	enum dInstruction
	{
		m_nop,
		m_call,
		m_ret,
		m_if, 
		m_function,
		m_argument,
		m_local,
		m_load,
		m_store,
		m_assigment,
		m_goto,
		m_label,
		m_param,
		m_restoreParam,
	};
	dTreeAdressStmt(void);
	~dTreeAdressStmt(void);

	dInstruction m_instruction;
	dOperator m_operator;
	dArg m_arg0;
	dArg m_arg1;
	dArg m_arg2;
	dList<dTreeAdressStmt>::dListNode* m_jmpTarget;

	void Trace () const;
	void TraceAssigment () const;
	void TraceConditional () const;
};


#endif
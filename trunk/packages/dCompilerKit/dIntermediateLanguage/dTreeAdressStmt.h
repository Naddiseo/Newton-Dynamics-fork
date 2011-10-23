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
		m_function,
		m_argument,
		m_local,
		m_assigment,
		m_if, 
		m_ifnot, 
		m_goto,
		m_target,
		m_param,
		m_call,
		m_ret,
		m_restoreParam,
	};
	dTreeAdressStmt(void);
	~dTreeAdressStmt(void);

	dInstruction m_instruction;
	dOperator m_operator;
	string m_arg0;
	string m_arg1;
	string m_arg2;
	dList<dTreeAdressStmt>::dListNode* m_jmpTarget;

	void Trace () const;
	void TraceAssigment () const;
};


#endif
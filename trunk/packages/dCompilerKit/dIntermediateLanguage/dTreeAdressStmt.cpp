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

#include "dCILstdafx.h"
#include "dTreeAdressStmt.h"

dTreeAdressStmt::dTreeAdressStmt(void)
	:m_instruction(m_nop)
	,m_operator(m_nothing)
	,m_arg0()
	,m_arg1()
	,m_arg2()
	,m_jmpTarget (NULL)
{
}

dTreeAdressStmt::~dTreeAdressStmt(void)
{
}


void dTreeAdressStmt::TraceAssigment () const
{

	DTRACE (("%s = %s", m_arg0.c_str(), m_arg1.c_str()));
	switch (m_operator)
	{
		case m_nothing:
		{
			break;
		}
		case m_add:
		{
			DTRACE ((" + "));
			break;
		}

		case m_sub:
		{
			DTRACE ((" - "));
			break;
		}

		case m_mul:
		{
			DTRACE ((" * "));
			break;
		}

		case m_less:
		{
			DTRACE ((" < "));
			break;
		}

		case m_greather:
		{
			DTRACE ((" > "));
			break;
		}

		case m_lessEqual:
		{
			DTRACE ((" <= "));
			break;
		}

		case m_greatherEqual:
		{
			DTRACE ((" >= "));
			break;
		}


		default:;
			_ASSERTE (0);

	}

	DTRACE (("%s\n", m_arg2.c_str()));
}

void dTreeAdressStmt::Trace () const
{
	switch (m_instruction)
	{
		case m_function:
		{
			DTRACE (("function %s\n", m_arg0.c_str()));
			break;
		}

		case m_assigment:
		{
			TraceAssigment ();
			break;
		}

		case m_if:
		{
			DTRACE (("if (%s) goto %s\n", m_arg0.c_str(), m_arg1.c_str()));
			break;
		}

		case m_ifnot:
		{
			DTRACE (("if (!%s) goto %s\n", m_arg0.c_str(), m_arg1.c_str()));
			break;
		}

		case m_goto:
		{
			DTRACE (("goto %s\n", m_arg0.c_str()));
			break;
		}

		case m_target:
		{
			DTRACE (("%s:\n", m_arg0.c_str()));
			break;
		}

		case m_param:
		{
			DTRACE (("param %s\n", m_arg0.c_str()));
			break;
		}

		case m_argument:
		{
			DTRACE (("argument %s\n", m_arg0.c_str()));
			break;
		}

		case m_local:
		{
			DTRACE (("local %s\n", m_arg0.c_str()));
			break;
		}


		case m_restoreParam:
		{
			DTRACE (("restoreParam %s\n", m_arg0.c_str()));
			break;
		}

		case m_call:
		{
			DTRACE (("call %s\n", m_arg0.c_str()));
			break;
		}

		case m_ret:
		{
			DTRACE (("ret\n"));
			break;
		}


		default:;
		_ASSERTE (0);

	}
}
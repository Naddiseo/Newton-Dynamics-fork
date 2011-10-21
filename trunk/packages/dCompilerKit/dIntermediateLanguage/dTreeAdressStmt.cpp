#include "dCILstdafx.h"
#include "dTreeAdressStmt.h"

dTreeAdressStmt::dTreeAdressStmt(void)
	:m_instrution(m_nop)
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
	switch (m_instrution)
	{
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

		default:;
		_ASSERTE (0);

	}
}
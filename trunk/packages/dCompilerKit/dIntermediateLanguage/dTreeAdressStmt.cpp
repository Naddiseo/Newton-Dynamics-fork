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

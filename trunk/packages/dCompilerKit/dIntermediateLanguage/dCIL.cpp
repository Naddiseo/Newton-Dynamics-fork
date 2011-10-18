#include "dCILstdafx.h"
#include "dCIL.h"

dCIL::dCIL(void)
	:m_program()
	,m_tempIndex (0)
{
}

dCIL::~dCIL(void)
{
}

string dCIL::NewTemp ()
{
	char tmp[256];
	sprintf (tmp, "__tmp%d", m_tempIndex);
	m_tempIndex ++;
	return string (tmp);
}


dCIL::dProgram::dListNode* dCIL::NewStatement()
{
	return m_program.Append();
}
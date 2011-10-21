#include "dCILstdafx.h"
#include "dCIL.h"

dCIL::dCIL(void)
	:m_program()
	,m_tempIndex (0)
	,m_labelIndex (0)
{
}

dCIL::~dCIL(void)
{
}

string dCIL::NewTemp ()
{
	char tmp[256];
	sprintf (tmp, "t%0d", m_tempIndex);
	m_tempIndex ++;
	return string (tmp);
}

string dCIL::NewLabel ()
{
	char tmp[256];
	sprintf (tmp, "label%d", m_labelIndex);
	m_labelIndex ++;
	return string (tmp);
}

dCIL::dProgram::dListNode* dCIL::NewStatement()
{
	return m_program.Append();
}
#ifndef __dCIL_H_
#define __dCIL_H_

#include "dCILstdafx.h"
#include "dTreeAdressStmt.h"

class dCIL
{
	class dProgram: public dList<dTreeAdressStmt>
	{
		
	};
	public:
	dCIL(void);
	virtual ~dCIL(void);

	dProgram m_program;
};


#endif
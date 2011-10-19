#ifndef __dCIL_H_
#define __dCIL_H_

#include "dCILstdafx.h"
#include "dTreeAdressStmt.h"



class dCIL
{
	public:
	class dProgram: public dList<dTreeAdressStmt>
	{
		
	};
	
	dCIL(void);
	virtual ~dCIL(void);

	string NewTemp (); 
	string NewLabel (); 
	dProgram::dListNode* NewStatement();

	int m_tempIndex;
	int m_labelIndex;
	dProgram m_program;
};


#endif
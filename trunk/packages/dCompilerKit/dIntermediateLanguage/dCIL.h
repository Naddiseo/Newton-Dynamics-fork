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
	void ResetTemporaries();
	dProgram::dListNode* NewStatement();

	void Optimize(dProgram::dListNode* const scopeSegment);

	int m_tempIndex;
	int m_labelIndex;
	dProgram m_program;
};


#endif
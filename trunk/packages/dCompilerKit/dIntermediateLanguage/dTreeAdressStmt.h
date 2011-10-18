#ifndef  __dTreeAdressStmt_H_
#define  __dTreeAdressStmt_H_

class dTreeAdressStmt
{
	public:
	enum operand
	{
		m_assigment,

	};
	dTreeAdressStmt(void);
	~dTreeAdressStmt(void);

	operand m_code;
	string m_arg0;
	string m_arg1;
	string m_arg2;
};


#endif
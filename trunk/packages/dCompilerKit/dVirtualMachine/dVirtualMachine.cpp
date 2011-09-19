#include "dVirtualMachine.h"
#include "dAssemblerParser.h"
#include "dAssemblerLexical.h"
#include "dAssemblerCompiler.h"




dVirtualMachine::dNemonic dVirtualMachine::m_nemonics[] = 
{
	{mov,		"mov"},
	{lea,		"addi"},		

	{add,		"add"},
	{sub,		"sub"},		
	{mul,		"mul"},
	{div,		"div"}, 
	{abs,		"abs"},
	{neg,		"neg"},

	{and,		"and"},		
	{or,		"or"},		
	{xor,		"xor"},
	{not,		"xor"},

	{sll,		"sll"},		
	{srl,		"srl"},		

	{loadb,		"loadb"},		
	{loadw,		"loadw"},		
	{loadd,		"loadd"},		

	{storeb,	"storeb"},	
	{storew,	"storew"},	
	{stored,	"stored"},	

	{beq,		"beq"},		
	{bne,		"bne"},		
	{blt,		"blt"},		
	{ble,		"ble"},		
	{bgt,		"bgt"},		
	{bget,		"bget"},		

	{call,		"call"},		
	{callr,		"callr"},		
	{syscall,	"syscall"},		
	{jump,		"jump"},		

	{enter,		"enter"},		
	{exit,		"exit"},		
	{push,		"push"},		 
	{pop,		"pop"},		

	{nop,		"nop"},
};

dVirtualMachine::dVirtualMachine(void)
{	
	_ASSERTE (nop <= 1 << 6);
}

dVirtualMachine::~dVirtualMachine(void)
{
}


bool dVirtualMachine::CompileAssembly (const char* const code)
{
	dAssemblerCompiler compiler(this);
	dAssemblerLexical scanner (code);

	compiler.Parce(scanner);

	return true;
}
#include "dVirtualMachine.h"
#include "dAssemblerParcer.h"
#include "dAssemblerLexical.h"
#include "dAssemblerCompiler.h"

dVirtualMachine::dNemonic dVirtualMachine::m_nemonics[] = 
{
	{add,		"add"},
	{sub,		"sub"},		
	{addi,		"addi"},		
	{addu,		"addu"},		
	{subu,		"subu"},		
	{and,		"and"},		
	{or,		"or"},		
	{xor,		"xor"},		
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
};

dVirtualMachine::dVirtualMachine(void)
{
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
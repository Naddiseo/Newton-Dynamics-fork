#ifndef __dGrammarLexical_d__
#define __dGrammarLexical_d__

#include "dParcelLexical.h"

class dGrammarLexical: public dParcelLexical
{
public:
	dGrammarLexical(const char* const data);
	~dGrammarLexical(void);

	// read the user action 
	void ReadUserAction();
};


#endif
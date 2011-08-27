#ifndef __dGrammarLexical_d__
#define __dGrammarLexical_d__

#include "dParcerLexical.h"

class dGrammarLexical: public dParcerLexical
{
public:
	dGrammarLexical(const char* const data);
	~dGrammarLexical(void);

	// read the user action 
	void ReadUserAction();
};


#endif
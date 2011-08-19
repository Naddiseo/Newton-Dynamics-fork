/* Copych1 (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

// dParcelCompiler.cpp : Defines the entry point for the console application.
//

#include "dParcelCompiler.h"
#include "dGrammarLexical.h"

//The Parcel input file consists of three sections, separated by a line containing only `%%'. 
//
//	definitions
//	%%
//	production rules
//	%%
//	user code

class dParcelCompiler::dSymbol
{
	public:
	Token m_type;
	string m_name;
};
class dParcelCompiler::dRuleInfo: public dParcelCompiler::dSymbol
{
	public:
	class dRuleSentence: public dSymbol
	{
	};
	string m_userActionCode;
	dList<dRuleSentence> m_sentence;
};


class dParcelCompiler::dProductionRules: public dList<dParcelCompiler::dRuleInfo>
{
	public:

	dListNode* Find (string& name) const
	{
		for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
			if (node->GetInfo().m_name == name) {
				return node;
			}
		}
		_ASSERTE (0);
		return NULL;
	}
};


class dParcelCompiler::dItem
{
	public:
	int m_indexMarker;
	dProductionRules::dListNode* m_rule;
};



class dParcelCompiler::dState: public dList<dParcelCompiler::dItem>
{
	public:
	dState (dList<dItem>& itemSet)
	{
		for (dListNode* node = itemSet.GetFirst(); node; node = node->GetNext()) {
			Append(node->GetInfo());
		}
	}
};





extern "C" int  xmain(int argc, char **argv);

dParcelCompiler::dParcelCompiler(const char* const inputRules, const char* const outputFileName)
{

//char* xxx[] = {"", "TextFile.y", 0};
//xmain(2, xxx);

	// scan the grammar into a list of rules.
	dProductionRules ruleList;
	ScanGrammarFile(inputRules, ruleList);

	// convert the rules into a DFA.
	dState* state = GenerateDFA (ruleList);



	delete state;
}





dParcelCompiler::~dParcelCompiler()
{
}

void dParcelCompiler::ScanGrammarFile(const char* const inputRules, dProductionRules& ruleList)
{
	dGrammarLexical lexical (inputRules);

	string startSymbol ("");

	// scan the definition segment
	for (Token token = Token(lexical.NextToken()); token != GRAMMAR_SEGEMENT; token = Token(lexical.NextToken())) {
		DTRACE (("%s\n", lexical.GetTokenString()));
		switch (int (token)) {

			case TOKEN:
			{
				token = Token(lexical.NextToken());
				break;
			}

			case START:
			{
				token = Token(lexical.NextToken());
				startSymbol = lexical.GetTokenString();
				break;
			}
			default:;

		}
	}

	// scan the production rules segment
	for (Token token = Token(lexical.NextToken()); token != GRAMMAR_SEGEMENT; token = Token(lexical.NextToken())) {
		DTRACE (("%s\n", lexical.GetTokenString()));
		switch (int (token)) 
		{
			case LITERAL:
			{
				// add the first Rule;
				dRuleInfo& rule = ruleList.Append()->GetInfo();
				rule.m_type = token;
				rule.m_name += lexical.GetTokenString();
				token = Token(lexical.NextToken());
				_ASSERTE (token == COLOM);
				for (Token token = ScanGrammarRule(lexical, ruleList); token != SIMICOLOM; token = ScanGrammarRule(lexical, ruleList)); 

//				dRuleInfo& start = ruleList.Addtop()->GetInfo();


				break;
			}
			default:
				_ASSERTE (0);
		}
	}

	dProductionRules::dListNode* firtRuleNode = ruleList.GetFirst();	
	if (startSymbol != "") {
		firtRuleNode = ruleList.Find (startSymbol);	
	}
	//Expand the Grammar Rule by addin a empty start Rule;
	const dRuleInfo& firstRule = firtRuleNode->GetInfo();

	dRuleInfo& rule = ruleList.Addtop()->GetInfo();
	rule.m_type = firstRule.m_type;
	rule.m_name = string("__") + firstRule.m_name;

	dRuleInfo::dRuleSentence& symbol = rule.m_sentence.Append()->GetInfo();
	symbol.m_type = firstRule.m_type;
	symbol.m_name = firstRule.m_name;

	// scan literal use code

}

dParcelCompiler::Token dParcelCompiler::ScanGrammarRule(dGrammarLexical& lexical, dProductionRules& rules)
{

	dRuleInfo* currentRule = &rules.GetLast()->GetInfo();

	Token token = Token(lexical.NextToken());
	for ( ;token != SIMICOLOM; token = Token(lexical.NextToken())) {
		DTRACE (("%s\n", lexical.GetTokenString()));
		switch (int (token)) 
		{
			case LITERAL:
			{
				dRuleInfo::dRuleSentence& symbol = currentRule->m_sentence.Append()->GetInfo();
				symbol.m_type = token;
				symbol.m_name = lexical.GetTokenString();
				break;
			}

			case USER_ACTION:
			{
				currentRule->m_userActionCode += lexical.GetTokenString();
				break;
			}

			case OR:
			{
				// this is a rule with multiples sentences, add new rule withe the same name
				dRuleInfo& rule = rules.Append()->GetInfo();
				rule.m_type = currentRule->m_type;
				rule.m_name += currentRule->m_name;
				currentRule = &rule;
				break;
			}


			default:
				if (isascii (token)) {
					dRuleInfo::dRuleSentence& symbol = currentRule->m_sentence.Append()->GetInfo();
					symbol.m_type = LITERAL;
					symbol.m_name = lexical.GetTokenString();
				} else {
					_ASSERTE (0);
				}
		}
	}

	return token;
}

dParcelCompiler::dState* dParcelCompiler::GenerateDFA (dProductionRules& ruleList)
{
	dList<dItem> firstItemSet;
	dItem& item =  firstItemSet.Append()->GetInfo();
	item.m_indexMarker = 0;
	item.m_rule = ruleList.GetFirst();

	dState* const rootState = Closure (ruleList, firstItemSet);



	return rootState;
}

dParcelCompiler::dState* dParcelCompiler::Closure (dProductionRules& rulesList, dList<dItem>& itemSet)
{
//	int itemCount = 0;
//	dState::dItem items[2048];
//	items[]

	dState* const state = new dState (itemSet);

	return state;
}
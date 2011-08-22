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
	TokenType m_type;
	Token m_id;
	string m_name;
};

class dParcelCompiler::dSentenceSymbol: public dParcelCompiler::dSymbol
{
};

class dParcelCompiler::dRuleInfo: public dParcelCompiler::dSymbol, public dList<dParcelCompiler::dSentenceSymbol>
{
	public:
	string m_userActionCode;

	dListNode* GetSymbolNodeByIndex (int index) const
	{
		int i = 0;
		for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
			if (index == i) {
				return node;
			}
			i ++;
		}
		return NULL;
	}
};


class dParcelCompiler::dProductionRule: public dList<dParcelCompiler::dRuleInfo>
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
	dProductionRule::dListNode* m_ruleNode;
};

class dParcelCompiler::dState: public dList<dParcelCompiler::dItem>
{
	public:
	dState (dList<dItem>& itemSet)
		:m_key(0)
	{
		for (dListNode* node = itemSet.GetFirst(); node; node = node->GetNext()) {
			Append(node->GetInfo());
		}
	}

	int GetKey() const
	{
		return m_key;
	}

	void CalculateKey ()
	{
		int key = 0;
		for (dState::dListNode* itemNode = GetFirst(); itemNode; itemNode = itemNode->GetNext()) {
			dItem& item = itemNode->GetInfo();
			for (dRuleInfo::dListNode* node = item.m_ruleNode->GetInfo().GetFirst(); node; node = node->GetNext()) {
				const dSentenceSymbol& info = node->GetInfo();
				key = dCRC (info.m_name.c_str(), key);
			}
		}
		m_key = key;
	}

	dListNode* FindItem (dProductionRule::dListNode* const rule) const
	{
		for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
			dItem& item = node->GetInfo();
			if ((item.m_ruleNode == rule) && (item.m_indexMarker == 0)) {
				return node;
			}
		}
		return NULL;
	}

	void Trace() const
	{
		for (dState::dListNode* itemNode = GetFirst(); itemNode; itemNode = itemNode->GetNext()) {
			dItem& item = itemNode->GetInfo();
			DTRACE(("%s -> ", item.m_ruleNode->GetInfo().m_name.c_str()));

			int index = 0;
			bool hasIndex = false;
			dRuleInfo::dListNode* node = item.m_ruleNode->GetInfo().GetFirst();
			for (; node; node = node->GetNext()) {

				if (index == item.m_indexMarker) {
					DTRACE((". "));
					hasIndex = true;
					break;
				}
				const dSentenceSymbol& info = node->GetInfo();
				DTRACE(("%s ", info.m_name.c_str()));
				index ++;
			}

			for (; node; node = node->GetNext()) {
				const dSentenceSymbol& info = node->GetInfo();
				DTRACE(("%s ", info.m_name.c_str()));
			}
			if (!hasIndex) {
				DTRACE(("."));
			}
			DTRACE(("\n"));
		}
		DTRACE(("\n"));
	}

	private:
	int m_key;
};

dParcelCompiler::dParcelCompiler(const char* const inputRules, const char* const outputFileName)
{
	// scan the grammar into a list of rules.
	dProductionRule ruleList;
	dTree<int, string> terminalTokens;
	dTree<TokenType, string> symbolList;
	
	ScanGrammarFile(inputRules, ruleList, symbolList, terminalTokens);

	// convert the rules into a DFA.
	dTree<dState*,int> stateList;
	CanonicalItemSets (stateList, ruleList, symbolList);

	dTree<dState*,int>::Iterator iter(stateList);
	for (iter.Begin(); iter; iter ++) {
		dState* const state = iter.GetNode()->GetInfo();
		delete state;
	}
}


dParcelCompiler::~dParcelCompiler()
{
}

void dParcelCompiler::ScanGrammarFile(const char* const inputRules, dProductionRule& ruleList, dTree<TokenType, string>& symbolList, dTree<int, string>& terminalTokens)
{
	dGrammarLexical lexical (inputRules);

	string startSymbol ("");

	int teminalTokenEnumeration = 256;

	// scan the definition segment
	for (Token token = Token(lexical.NextToken()); token != GRAMMAR_SEGEMENT; token = Token(lexical.NextToken())) {
//		DTRACE (("%s\n", lexical.GetTokenString()));
		switch (int (token)) {

			case TOKEN:
			{
				token = Token(lexical.NextToken());
				terminalTokens.Insert(teminalTokenEnumeration, lexical.GetTokenString());
				teminalTokenEnumeration ++;
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
//		DTRACE (("%s\n", lexical.GetTokenString()));
		switch (int (token)) 
		{
			case LITERAL:
			{
				// add the first Rule;
				dRuleInfo& rule = ruleList.Append()->GetInfo();
				rule.m_id = token;
				rule.m_type = NONTERMINAL;
				rule.m_name = lexical.GetTokenString();
				symbolList.Insert(rule.m_type, rule.m_name);
				token = Token(lexical.NextToken());
				_ASSERTE (token == COLOM);
				for (Token token = ScanGrammarRule(lexical, ruleList, symbolList, terminalTokens); token != SIMICOLOM; token = ScanGrammarRule(lexical, ruleList, symbolList, terminalTokens)); 
				break;
			}
			default:
				_ASSERTE (0);
		}
	}

	dProductionRule::dListNode* firtRuleNode = ruleList.GetFirst();	
	if (startSymbol != "") {
		firtRuleNode = ruleList.Find (startSymbol);	
	}
	//Expand the Grammar Rule by addin a empty start Rule;
	const dRuleInfo& firstRule = firtRuleNode->GetInfo();

	dRuleInfo& rule = ruleList.Addtop()->GetInfo();
	rule.m_id = firstRule.m_id;
	rule.m_type = NONTERMINAL;
	rule.m_name = firstRule.m_name + string("\'");
	symbolList.Insert(rule.m_type, rule.m_name);
	
	dSentenceSymbol& symbol = rule.Append()->GetInfo();
	symbol.m_id = firstRule.m_id;
	symbol.m_type = firstRule.m_type;
	symbol.m_name = firstRule.m_name;
	symbolList.Insert(symbol.m_type, symbol.m_name);

	// scan literal use code
}

dParcelCompiler::Token dParcelCompiler::ScanGrammarRule(
	dGrammarLexical& lexical, 
	dProductionRule& rules, 
	dTree<TokenType, string>& symbolList, 
	dTree<int, string>& terminalTokens)
{
	dRuleInfo* currentRule = &rules.GetLast()->GetInfo();

	Token token = Token(lexical.NextToken());
	for ( ;token != SIMICOLOM; token = Token(lexical.NextToken())) {
//		DTRACE (("%s\n", lexical.GetTokenString()));
		switch (int (token)) 
		{
			case LITERAL:
			{
				dSentenceSymbol& symbol = currentRule->Append()->GetInfo();
				symbol.m_id = token;
				symbol.m_name = lexical.GetTokenString();
				symbol.m_type = terminalTokens.Find(symbol.m_name) ? TERMINAL : NONTERMINAL;
				symbolList.Insert(symbol.m_type, symbol.m_name);
				break;
			}

			case USER_ACTION:
			{
				currentRule->m_userActionCode += lexical.GetTokenString();
				break;
			}

			case OR:
			{
				// this is a rule with multiples sentences, add new rule with the same name
				dRuleInfo& rule = rules.Append()->GetInfo();
				rule.m_id = currentRule->m_id;
				rule.m_type = NONTERMINAL;
				rule.m_name += currentRule->m_name;
				currentRule = &rule;
				break;
			}


			default:
			{
				if (isascii (token)) {
					dSentenceSymbol& symbol = currentRule->Append()->GetInfo();
					symbol.m_name = lexical.GetTokenString();
					symbol.m_type = TERMINAL;
					symbol.m_id = LITERAL;
					symbolList.Insert(symbol.m_type, symbol.m_name);
				} else {
					_ASSERTE (0);
				}
			}
		}
	}

	return token;
}

// Generate the closure for a Set of Item  
dParcelCompiler::dState* dParcelCompiler::Closure (dProductionRule& rulesList, dList<dItem>& itemSet)
{
	dState* const state = new dState (itemSet);
	for (dState::dListNode* itemNode = state->GetFirst(); itemNode; itemNode = itemNode->GetNext()) {
		dItem& item = itemNode->GetInfo();

		dRuleInfo::dListNode* const symbolNode = item.m_ruleNode->GetInfo().GetSymbolNodeByIndex (item.m_indexMarker);
		if (symbolNode) {
			dSentenceSymbol& sentenceSymbol = symbolNode->GetInfo();

			for (dProductionRule::dListNode* node = rulesList.GetFirst(); node; node = node->GetNext()) {
				const dRuleInfo& info = node->GetInfo();
				if (info.m_name == sentenceSymbol.m_name) {
					dState::dListNode* const itemNode = state->FindItem(node);
					if (!itemNode) {
						dItem& newItem = state->Append()->GetInfo();
						newItem.m_indexMarker = 0;
						newItem.m_ruleNode = node;
					}
				}
			}
		}
	}
	
	state->CalculateKey ();
	return state;
}

// generates the canonical Items set for a LR(1) grammar
void dParcelCompiler::CanonicalItemSets (dTree<dState*,int>& stateMap, dProductionRule& ruleList, dTree<TokenType, string>& symbolList)
{
	dList<dItem> itemSet;
	dItem& item = itemSet.Append()->GetInfo();
	item.m_indexMarker = 0;
	item.m_ruleNode = ruleList.GetFirst();

	dList<dState*> stateList;
	dState* const state = Closure (ruleList, itemSet);
	stateMap.Insert(state, state->GetKey());
	stateList.Append(state);

	state->Trace();

	for (dList<dState*>::dListNode* node = stateList.GetFirst(); node; node = node->GetNext()) {
		dState* const state = node->GetInfo();

		dTree<TokenType, string>::Iterator iter (symbolList);
		for (iter.Begin(); iter; iter ++) {
			
			string symbol (iter.GetKey());
			itemSet.RemoveAll();

			dState* const newState = Goto (ruleList, state, symbol);

			if (newState->GetCount() && !stateMap.Find(newState->GetKey())) {
				stateMap.Insert(newState, newState->GetKey());
				newState->Trace();

				stateList.Append(newState);
			} else {
				delete newState;
			}
		}
	}
}

dParcelCompiler::dState* dParcelCompiler::Goto (dProductionRule& rulesList, dState* const state, const string& symbol)
{
	dList<dItem> itemSet;

	for (dState::dListNode* node = state->GetFirst(); node; node = node->GetNext()) {
		dItem& item = node->GetInfo();

		int index = 0;
		const dRuleInfo& ruleInfo = item.m_ruleNode->GetInfo();;
		for (dRuleInfo::dListNode* infoSymbolNode = ruleInfo.GetFirst(); infoSymbolNode; infoSymbolNode = infoSymbolNode->GetNext()) {
			const dSentenceSymbol& infoSymbol = infoSymbolNode->GetInfo();
			if (infoSymbol.m_name == symbol) {
				if (index == item.m_indexMarker) {
					dItem& newItem = itemSet.Append()->GetInfo();
					newItem.m_indexMarker = index + 1;
					newItem.m_ruleNode = item.m_ruleNode;
					break;
				}
			}
			index ++;
		}
	}

	return Closure (rulesList, itemSet);
}


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

// dParcerCompiler.cpp : Defines the entry point for the console application.
//

#include "dParcerCompiler.h"
#include "dGrammarLexical.h"

#define DACCEPT_SYMBOL "$$$"



//The Parcel input file consists of three sections, separated by a line containing only `%%'. 
//
//	definitions
//	%%
//	production rules
//	%%
//	user code

enum dParcerCompiler::TokenType
{
	TERMINAL,
	NONTERMINAL
};


enum dParcerCompiler::ActionType
{
	ACCEPT,
	SHIFT,
	REDUCE
};



/*
class dParcerCompiler::dActionEntry
{
	public: 
	Token m_token;
	int m_state;
	int m_reduceCount;
	ActionType m_action;
};
*/




class dParcerCompiler::dSymbol
{
	public:
	TokenType m_type;
	Token m_token;
	string m_name;
};


class dParcerCompiler::dRuleInfo: public dParcerCompiler::dSymbol, public dList<dParcerCompiler::dSymbol>
{
	public:
	int m_ruleId;
	int m_ruleNumber;
	string m_userActionCode;

	dRuleInfo()
		:dSymbol(), dList<dSymbol>()
		,m_ruleId(0), m_ruleNumber(0), m_userActionCode("")
	{
	}

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


class dParcerCompiler::dProductionRule: public dList<dParcerCompiler::dRuleInfo>
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

class dParcerCompiler::dTransition
{
	public:
	string m_name;
	TokenType m_type;
	dState* m_targetState;
};

class dParcerCompiler::dItem
{
	public:
	dItem ()
		:m_error(true), m_indexMarker(0), m_ruleNode(NULL)
	{
	}

	bool m_error;
	int m_indexMarker;
	dProductionRule::dListNode* m_ruleNode;
};

class dParcerCompiler::dAction 
{
	public:
	ActionType m_type;
	dProductionRule::dListNode* m_ruleNode;
};

class dParcerCompiler::dState: public dList<dParcerCompiler::dItem>
{
	public:
	dState (dList<dItem>& itemSet)
		:m_key(0), m_number(0), m_goto(), m_actions(), m_transitions()
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
				const dSymbol& info = node->GetInfo();
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
		DTRACE(("state %d:\n", m_number));
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
				const dSymbol& info = node->GetInfo();
				DTRACE(("%s ", info.m_name.c_str()));
				index ++;
			}

			for (; node; node = node->GetNext()) {
				const dSymbol& info = node->GetInfo();
				DTRACE(("%s ", info.m_name.c_str()));
			}
			if (!hasIndex) {
				DTRACE(("."));
			}
			DTRACE(("\n"));
		}
		DTRACE(("\n"));
	}

	int m_key;
	int m_number;
	dTree<dState*, string> m_goto; 
	dTree<dAction, string> m_actions; 
	dList<dTransition> m_transitions;
	
};

class dUserVariable: public string
{
	dUserVariable ()
		:string()
	{
	}

	dUserVariable (const char* const text)
		:string(text)
	{
	}
};

dParcerCompiler::dParcerCompiler(const char* const inputRules, const char* const outputFileName, const char* const scannerClassName)
{
	// scan the grammar into a list of rules.
	dProductionRule ruleList;
	dTree<int, string> tokenEnumeration;
	dTree<TokenType, string> symbolList;
	string userCodeBlock;
	string userVariable ("dUserVariable");
	string userVariableClass ("");

	
	symbolList.Insert(TERMINAL, DACCEPT_SYMBOL);
	ScanGrammarFile(inputRules, ruleList, symbolList, tokenEnumeration, userCodeBlock, userVariableClass);

	// convert the rules into a NFA.
	dTree<dState*,int> stateList;
	CanonicalItemSets (stateList, ruleList, symbolList);

	// create a LR(1) parsing table from the NFA graphs
	BuildParcingTable (stateList, symbolList);

	//Write Parcer Code
	char className[256];
	const char* ptr = strrchr (outputFileName, '/');
	if (ptr) {
		ptr ++;
	} else {
		ptr = strrchr (outputFileName, '\\');
		if (ptr) {
			ptr ++;
		} else {
			ptr = outputFileName;
		}
	}
	strcpy (className, ptr);
	strtok (className, ".");
	GenerateHeaderFile (className, scannerClassName, outputFileName, ruleList, tokenEnumeration);
	GenerateParcerCode (className, scannerClassName, outputFileName, userCodeBlock, userVariable, userVariableClass, stateList, symbolList);

	dTree<dState*,int>::Iterator iter(stateList);
	for (iter.Begin(); iter; iter ++) {
		dState* const state = iter.GetNode()->GetInfo();
		delete state;
	}
}


dParcerCompiler::~dParcerCompiler()
{
}

void dParcerCompiler::ScanGrammarFile(
	const char* const inputRules, 
	dProductionRule& ruleList, 
	dTree<TokenType, string>& symbolList, 
	dTree<int, string>& tokenEnumerationMap,
	string& userCodeBlock,
	string& userVariableClass)
{
	dGrammarLexical lexical (inputRules);


	char path[2048];
	GetModuleFileName(NULL, path, sizeof(path)); 
	//	for Linux:
	//	char szTmp[32]; 
	//	sprintf(szTmp, "/proc/%d/exe", getpid()); 
	//	int bytes = MIN(readlink(szTmp, pBuf, len), len - 1); 
	//	if(bytes >= 0)
	//		pBuf[bytes] = '\0'; 

	// read the default template user variable
	char* const ptr1 = strrchr (path, '\\');
	sprintf (ptr1, "/dParcerUserVariableTemplate.cpp");
	FILE* const templateFile = fopen (path, "r");
	_ASSERTE (templateFile);

	fseek (templateFile, 0, SEEK_END);
	int size = ftell (templateFile) + 1;
	fseek (templateFile, 0, SEEK_SET);

	
	userVariableClass.resize(size);
	fread ((void*)userVariableClass.c_str(), 1, size, templateFile);
	fclose (templateFile);	


	string startSymbol ("");

	int tokenEnumeration = 256;

	// scan the definition segment
	for (Token token = Token(lexical.NextToken()); token != GRAMMAR_SEGMENT; ) {
//		DTRACE (("%s\n", lexical.GetTokenString()));
		switch (int (token)) 
		{
			case START:
			{
				token = Token(lexical.NextToken());
				startSymbol = lexical.GetTokenString();
				token = Token(lexical.NextToken());
				break;
			}

			case TOKEN:
			{
				for (token = Token(lexical.NextToken()); token == LITERAL; token = Token(lexical.NextToken())) {
					const char* const name = lexical.GetTokenString();
					tokenEnumerationMap.Insert(tokenEnumeration, name);
					tokenEnumeration ++;
					symbolList.Insert(TERMINAL, name);
				}
				break;
			}

			case UNION:
			{
				token = Token(lexical.NextToken());
				_ASSERTE (token == USER_ACTION);
				userVariableClass = lexical.GetTokenString() + 1;
				userVariableClass.replace(userVariableClass.size() - 1, 1, "");
				token = Token(lexical.NextToken());
				break;
			}


			case CODE_BLOCK:
			{
				userCodeBlock += lexical.GetTokenString();
				token = Token(lexical.NextToken());
				break;
			}

			default:;
			{
				token = Token(lexical.NextToken());
			}

		}
	}

	int ruleNumber = 1;

	// scan the production rules segment
	for (Token token = Token(lexical.NextToken()); token != GRAMMAR_SEGMENT; token = Token(lexical.NextToken())) {
//		DTRACE (("%s\n", lexical.GetTokenString()));
		switch (int (token)) 
		{
			case LITERAL:
			{
				// add the first Rule;
				dRuleInfo& rule = ruleList.Append()->GetInfo();
				rule.m_token = token;
				rule.m_type = NONTERMINAL;
				rule.m_name = lexical.GetTokenString();
				symbolList.Insert(rule.m_type, rule.m_name);

				dTree<int, string>::dTreeNode* nonTerminalIdNode = tokenEnumerationMap.Find(rule.m_name);
				if (!nonTerminalIdNode) {
					nonTerminalIdNode = tokenEnumerationMap.Insert(tokenEnumeration, rule.m_name);
					tokenEnumeration ++;
				}
				rule.m_ruleId = nonTerminalIdNode->GetInfo();
				rule.m_ruleNumber = ruleNumber;
				ruleNumber ++;

				token = Token(lexical.NextToken());
				_ASSERTE (token == COLOM);
				for (Token token = ScanGrammarRule(lexical, ruleList, symbolList, ruleNumber, tokenEnumerationMap, tokenEnumeration); token != SIMICOLOM; token = ScanGrammarRule(lexical, ruleList, symbolList, ruleNumber, tokenEnumerationMap, tokenEnumeration)); 
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
	rule.m_ruleNumber = 0;
	rule.m_ruleId = tokenEnumeration;
	rule.m_token = firstRule.m_token;
	rule.m_type = NONTERMINAL;
	rule.m_name = firstRule.m_name + string("\'");
	symbolList.Insert(rule.m_type, rule.m_name);
	
	dSymbol& symbol = rule.Append()->GetInfo();
	symbol.m_token = firstRule.m_token;
	symbol.m_type = firstRule.m_type;
	symbol.m_name = firstRule.m_name;
	symbolList.Insert(symbol.m_type, symbol.m_name);

	// scan literal use code
}

dParcerCompiler::Token dParcerCompiler::ScanGrammarRule(
	dGrammarLexical& lexical, 
	dProductionRule& rules, 
	dTree<TokenType, string>& symbolList, 
	int& ruleNumber,
	dTree<int, string>& tokenEnumerationMap,
	int& tokenEnumeration)
{
	dRuleInfo* currentRule = &rules.GetLast()->GetInfo();

	Token token = Token(lexical.NextToken());
	for ( ;token != SIMICOLOM; token = Token(lexical.NextToken())) {
//		DTRACE (("%s\n", lexical.GetTokenString()));
		switch (int (token)) 
		{
			case LITERAL:
			{
				dSymbol& symbol = currentRule->Append()->GetInfo();
				symbol.m_token = token;
				symbol.m_name = lexical.GetTokenString();
				
				dTree<TokenType, string>::dTreeNode* symbolNode = symbolList.Find(symbol.m_name);
				if (!symbolList.Find(symbol.m_name)) {
					symbolNode = symbolList.Insert(NONTERMINAL, symbol.m_name);
				}
//				symbol.m_type = terminalTokens.Find(symbol.m_name) ? TERMINAL : NONTERMINAL;
//				symbolList.Insert(symbol.m_type, symbol.m_name);
				symbol.m_type = symbolNode->GetInfo();


				dTree<int, string>::dTreeNode* nonTerminalIdNode = tokenEnumerationMap.Find(symbol.m_name);
				if (!nonTerminalIdNode) {
					nonTerminalIdNode = tokenEnumerationMap.Insert(tokenEnumeration, symbol.m_name);
					tokenEnumeration ++;
				}
				break;
			}

			case USER_ACTION:
			{
				currentRule->m_userActionCode = lexical.GetTokenString();
				break;
			}

			case OR:
			{
				// this is a rule with multiples sentences, add new rule with the same name
				dRuleInfo& rule = rules.Append()->GetInfo();
				rule.m_ruleNumber = ruleNumber;
				ruleNumber ++;
				rule.m_ruleId = currentRule->m_ruleId;
				rule.m_token = currentRule->m_token;
				rule.m_type = NONTERMINAL;
				rule.m_name += currentRule->m_name;
				currentRule = &rule;
				break;
			}


			default:
			{
				if (isascii (token)) {
					dSymbol& symbol = currentRule->Append()->GetInfo();
					symbol.m_name = lexical.GetTokenString();
					symbol.m_type = TERMINAL;
					symbol.m_token = LITERAL;
					symbolList.Insert(TERMINAL, symbol.m_name);

					tokenEnumerationMap.Insert(token, symbol.m_name);
				} else {
					_ASSERTE (0);
				}
			}
		}


	}

	return token;
}

// Generate the closure for a Set of Item  
dParcerCompiler::dState* dParcerCompiler::Closure (dProductionRule& rulesList, dList<dItem>& itemSet)
{
	dState* const state = new dState (itemSet);
	for (dState::dListNode* itemNode = state->GetFirst(); itemNode; itemNode = itemNode->GetNext()) {
		dItem& item = itemNode->GetInfo();

		dRuleInfo::dListNode* const symbolNode = item.m_ruleNode->GetInfo().GetSymbolNodeByIndex (item.m_indexMarker);
		if (symbolNode) {
			dSymbol& sentenceSymbol = symbolNode->GetInfo();

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

// generates the got state for this symbol
dParcerCompiler::dState* dParcerCompiler::Goto (dProductionRule& rulesList, dState* const state, const string& symbol)
{
	dList<dItem> itemSet;

	for (dState::dListNode* node = state->GetFirst(); node; node = node->GetNext()) {
		dItem& item = node->GetInfo();

		int index = 0;
		const dRuleInfo& ruleInfo = item.m_ruleNode->GetInfo();;
		for (dRuleInfo::dListNode* infoSymbolNode = ruleInfo.GetFirst(); infoSymbolNode; infoSymbolNode = infoSymbolNode->GetNext()) {
			const dSymbol& infoSymbol = infoSymbolNode->GetInfo();
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

// generates the canonical Items set for a LR(1) grammar
void dParcerCompiler::CanonicalItemSets (dTree<dState*,int>& stateMap, dProductionRule& ruleList, dTree<TokenType, string>& symbolList)
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

	int stateNumber = 1;
	for (dList<dState*>::dListNode* node = stateList.GetFirst(); node; node = node->GetNext()) {
		dState* const state = node->GetInfo();

		dTree<TokenType, string>::Iterator iter (symbolList);
		for (iter.Begin(); iter; iter ++) {

			string symbol (iter.GetKey());
			dState* const newState = Goto (ruleList, state, symbol);

			if (newState->GetCount()) {
				dTransition& transition = state->m_transitions.Append()->GetInfo();
				transition.m_name = symbol;
				transition.m_type = iter.GetNode()->GetInfo();
				transition.m_targetState = newState;

				dTree<dState*,int>::dTreeNode* const targetStateNode = stateMap.Find(newState->GetKey());
				if (!targetStateNode) {
					newState->m_number = stateNumber;
					stateNumber ++;
					stateMap.Insert(newState, newState->GetKey());
					newState->Trace();

					stateList.Append(newState);
				} else {
					transition.m_targetState = targetStateNode->GetInfo();
					delete newState;
				}
			} else {
				delete newState;
			}
		}
	}
}



void dParcerCompiler::BuildParcingTable (dTree<dState*,int>& stateList, dTree<TokenType, string>& symbolList)
{
	dTree<dState*,int>::Iterator stateIter (stateList);
	dTree<TokenType, string>::Iterator symbolIter (symbolList);

	// create Shift Reduce action table
	for (stateIter.Begin(); stateIter; stateIter ++) {
		dState* const state = stateIter.GetNode()->GetInfo();

		if ((state->GetCount() == 1) && (state->m_number != 0)) {
			// emit simple reduce rule on all inputs
			dState::dListNode* const itemNode = state->GetFirst();
			dItem& item = itemNode->GetInfo();
			
			if (item.m_error == false) {
				// rule already used for in another action
				_ASSERTE (0);
			}

			item.m_error = false;
			const dRuleInfo& ruleInfo = item.m_ruleNode->GetInfo();
			if (ruleInfo.GetCount() == item.m_indexMarker) {
				for (symbolIter.Begin(); symbolIter; symbolIter ++) {
					TokenType type = symbolIter.GetNode()->GetInfo();
					if (type == TERMINAL) {
						string symbol = symbolIter.GetKey();

						dTree<dAction, string>::dTreeNode* const actionNode = state->m_actions.Insert (symbol); 
						// this could be a reduce-reduce conflict;
						_ASSERTE (actionNode);
						if (actionNode) {
							item.m_error = false;
							dAction& action = actionNode->GetInfo();
							action.m_type = REDUCE;
							action.m_ruleNode = item.m_ruleNode;
						}
					}
				}
			}
		} else {
			// check if accepting rule in in the estate
			for (dState::dListNode* itemNode = state->GetFirst(); itemNode; itemNode = itemNode->GetNext()) {
				dItem& item = itemNode->GetInfo();
				const dRuleInfo& ruleInfo = item.m_ruleNode->GetInfo();
				// check if this item is the accepting Rule
				if ((ruleInfo.m_ruleNumber == 0) && (item.m_indexMarker == 1)) {
					if (item.m_error == false) {
						// rule already used in another action
						_ASSERTE (0);
					}
					item.m_error = false;
					dTree<dAction, string>::dTreeNode* const actionNode = state->m_actions.Insert (DACCEPT_SYMBOL); 
					_ASSERTE (actionNode);
					dAction& action = actionNode->GetInfo();
					action.m_type = ACCEPT;
					action.m_ruleNode = item.m_ruleNode;
				} else {
					// if it does not contain the accepting rule them is see if is is a shift action
					for (symbolIter.Begin(); symbolIter; symbolIter ++) {
						TokenType type = symbolIter.GetNode()->GetInfo();
						if (type == TERMINAL) {
							string symbol = symbolIter.GetKey();
							for (dList<dTransition>::dListNode* node = state->m_transitions.GetFirst(); node; node = node->GetNext()) {
								dTransition& transition = node->GetInfo();
								if (transition.m_name == symbol) {
									int i = 0; 
									for (dRuleInfo::dListNode* node = ruleInfo.GetFirst(); node; node = node->GetNext()) {
										dSymbol& ruleSymbol = node->GetInfo();
										if ((symbol == ruleSymbol.m_name) && (i == item.m_indexMarker)) {
											if (item.m_error == false) {
												// rule already used in another action
												_ASSERTE (0);
											}
											item.m_error = false;

											// this is a shift action
											dTree<dAction, string>::dTreeNode* const actionNode = state->m_actions.Insert (symbol); 
											dAction& action = actionNode->GetInfo();
											action.m_type = SHIFT;
											action.m_ruleNode = item.m_ruleNode;
										}
										i ++;
									}
								}
							}
						}
					}
				}
			}
		}
	}


	// create Goto Table
	for (stateIter.Begin(); stateIter; stateIter ++) {
		dState* const state = stateIter.GetNode()->GetInfo();
		for (dList<dTransition>::dListNode* node = state->m_transitions.GetFirst(); node; node = node->GetNext()) {
			dTransition& transition = node->GetInfo();
			if (transition.m_type == NONTERMINAL) {
				state->m_goto.Insert (transition.m_targetState, transition.m_name); 
			}
		}
	}
}




void dParcerCompiler::GenerateHeaderFile (
	const char* const className, 
	const char* const scannerClassName,
	const char* const outputFileName,
	dProductionRule& ruleList, 
	dTree<int, string>& tokenEnumerationMap)
{
	char path[2048];

	// in windows
	GetModuleFileName(NULL, path, sizeof(path)); 

	//	for Linux:
	//	char szTmp[32]; 
	//	sprintf(szTmp, "/proc/%d/exe", getpid()); 
	//	int bytes = MIN(readlink(szTmp, pBuf, len), len - 1); 
	//	if(bytes >= 0)
	//		pBuf[bytes] = '\0'; 


	char* const ptr = strrchr (path, '\\');
	sprintf (ptr, "/dParcerTemplate.h");

	FILE* const templateFile = fopen (path, "r");
	_ASSERTE (templateFile);

	fseek (templateFile, 0, SEEK_END);
	int size = ftell (templateFile) + 1;
	fseek (templateFile, 0, SEEK_SET);

	string templateHeader ("") ;
	templateHeader.resize(size);
	fread ((void*)templateHeader.c_str(), 1, size, templateFile);
	fclose (templateFile);	

	string name (className);
	for (size_t position = templateHeader.find ("$(className)"); position != -1; position = templateHeader.find ("$(className)")) {
		templateHeader.replace(position, 12, name);
	}

	string scanner(scannerClassName);
	for (size_t position = templateHeader.find ("$(scannerClass)"); position != -1; position = templateHeader.find ("$(scannerClass)")) {
		templateHeader.replace(position, 15, scanner);
	}


	string enumTokens ("");
	bool firstToken = true;
	dTree<int, string> symbolFilter;
	for (dProductionRule::dListNode* ruleNode = ruleList.GetFirst(); ruleNode; ruleNode = ruleNode->GetNext()) {
		dRuleInfo& ruleInfo = ruleNode->GetInfo();
		for (dRuleInfo::dListNode* symbolNode = ruleInfo.GetFirst(); symbolNode; symbolNode = symbolNode->GetNext()) {
			dSymbol& symbol = symbolNode->GetInfo();
			if (symbol.m_type == TERMINAL) {
				if (!symbolFilter.Find(symbol.m_name)) {
					symbolFilter.Insert(0, symbol.m_name);
					dTree<int, string>::dTreeNode* const node = tokenEnumerationMap.Find(symbol.m_name);
					_ASSERTE (node);
					int value = node->GetInfo();
					if (value >= 256) {
						enumTokens += "\t\t";
						enumTokens += symbol.m_name;
						if (firstToken) {
							firstToken = false;
							enumTokens += " = 256,\n";
						} else {
							enumTokens += ",\n";
						}
					}
				}
			}
		}
	}
	int position ;
	enumTokens.replace(enumTokens.size()-2, 2, "");
	position = templateHeader.find ("$(Tokens)");
	templateHeader.replace(position, 9, enumTokens);


	strcpy (path, outputFileName);
	char* const ptr1 = strrchr (path, '.');
	if (ptr1) {
		*ptr1 = 0;
	}
	strcat (path, ".h");
	FILE* const headerFile = fopen (path, "w");
	_ASSERTE (headerFile);
	fprintf (headerFile, "%s", templateHeader.c_str());
	fclose (headerFile);
}


void dParcerCompiler::GenerateParcerCode (
	const char* const className, 
	const char* const scannerClassName,
	const char* const outputFileName,
	const string& userCode,
	const string& userVariable,
	const string& userVariableClass, 
	dTree<dState*,int>& stateList, 
	dTree<TokenType, string>& symbolList)
{
	char path[2048];

	// in windows
	GetModuleFileName(NULL, path, sizeof(path)); 

	//	for Linux:
	//	char szTmp[32]; 
	//	sprintf(szTmp, "/proc/%d/exe", getpid()); 
	//	int bytes = MIN(readlink(szTmp, pBuf, len), len - 1); 
	//	if(bytes >= 0)
	//		pBuf[bytes] = '\0'; 


	char* const ptr = strrchr (path, '\\');
	sprintf (ptr, "/dParcerTemplate.cpp");

	FILE* const templateFile = fopen (path, "r");
	_ASSERTE (templateFile);

	fseek (templateFile, 0, SEEK_END);
	int size = ftell (templateFile) + 1;
	fseek (templateFile, 0, SEEK_SET);

	string templateHeader ("") ;
	templateHeader.resize(size);
	fread ((void*)templateHeader.c_str(), 1, size, templateFile);
	fclose (templateFile);	

	size_t position = templateHeader.find ("$(userCode)");
	templateHeader.replace(position, 11, userCode);

	
	string name (className);
	for (size_t i = templateHeader.find ("$(className)"); i != -1; i = templateHeader.find ("$(className)")) {
		templateHeader.replace(i, 12, name);
	}

	string scanner(scannerClassName);
	for (size_t i = templateHeader.find ("$(scannerClass)"); i != -1; i = templateHeader.find ("$(scannerClass)")) {
		templateHeader.replace(i, 15, scanner);
	}


	position = templateHeader.find ("$(userVariableClass)");
	templateHeader.replace(position, 20, userVariableClass);

	for (size_t i = templateHeader.find ("$(userVariable)"); i != -1; i = templateHeader.find ("$(userVariable)")) {
		templateHeader.replace(i, 15, userVariable);
	}

	dTree<dState*,int>::Iterator stateIter (stateList);
	for (stateIter.Begin(); stateIter; stateIter ++) {
		dState* const state = stateIter.GetNode()->GetInfo();

		dTree<dAction, string>::Iterator actionIter (state->m_actions);
		for (actionIter.Begin(); actionIter; actionIter++) {
			dAction& action = actionIter.GetNode()->GetInfo();
			if (action.m_type == SHIFT) {
//				_ASSERTE (0);
			} else if (action.m_type == REDUCE) {
//				_ASSERTE (0);
			} else {
//				_ASSERTE (action.m_type == ACCEPT);
			}

		}
	}
		


	strcpy (path, outputFileName);
	char* const ptr1 = strrchr (path, '.');
	if (ptr1) {
		*ptr1 = 0;
	}
	strcat (path, ".cpp");
	FILE* const ouputFile = fopen (path, "w");
	_ASSERTE (ouputFile);
	fprintf (ouputFile, "%s", templateHeader.c_str());
	fclose (ouputFile);
}

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
#include "dParcerLexical.h"

#define DACCEPT_SYMBOL "$$$"



//The Parcel input file consists of three sections, separated by a line containing only `%%'. 
//
//	definitions
//	%%
//	production rules
//	%%
//	user code

enum dParcerCompiler::dTokenType
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
	dToken m_token;
	int m_state;
	int m_reduceCount;
	ActionType m_action;
};
*/




class dParcerCompiler::dSymbol
{
	public:
	dTokenType m_type;
	dToken m_token;
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
	dTokenType m_type;
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


dParcerCompiler::dParcerCompiler(const string& inputRules, const char* const outputFileName, const char* const scannerClassName)
{
	// scan the grammar into a list of rules.
	dProductionRule ruleList;
	dTree<int, string> tokenEnumeration;
	dTree<dTokenType, string> symbolList;
	string userCodeBlock;
	string userVariable ("dUserVariable");
	string userVariableClass ("");
	string endUserCode ("");

	// scan grammar to a set of LR(1) rules
	symbolList.Insert(TERMINAL, DACCEPT_SYMBOL);
	ScanGrammarFile(inputRules, ruleList, symbolList, tokenEnumeration, userCodeBlock, userVariableClass, endUserCode);

	// convert the rules into a NFA.
	dTree<dState*,int> stateList;
	CanonicalItemSets (stateList, ruleList, symbolList);

	// create a LR(1) parsing table from the NFA graphs
	BuildParcingTable (stateList, symbolList);

	//Write Parcer class and header file
	string className (GetClassName(outputFileName));
	GenerateHeaderFile (className, scannerClassName, outputFileName, ruleList, tokenEnumeration);
	GenerateParcerCode (className, scannerClassName, outputFileName, userCodeBlock, userVariable, userVariableClass, stateList, symbolList, endUserCode);

	dTree<dState*,int>::Iterator iter(stateList);
	for (iter.Begin(); iter; iter ++) {
		dState* const state = iter.GetNode()->GetInfo();
		delete state;
	}
}


dParcerCompiler::~dParcerCompiler()
{
}

string dParcerCompiler::GetClassName(const char* const fileName) const
{
	char className[256];
	const char* ptr = strrchr (fileName, '/');
	if (ptr) {
		ptr ++;
	} else {
		ptr = strrchr (fileName, '\\');
		if (ptr) {
			ptr ++;
		} else {
			ptr = fileName;
		}
	}
	strcpy (className, ptr);
	strtok (className, ".");
	return string (className);
}

void dParcerCompiler::ScanGrammarFile(
	const string& inputRules, 
	dProductionRule& ruleList, 
	dTree<dTokenType, string>& symbolList, 
	dTree<int, string>& tokenEnumerationMap,
	string& userCodeBlock,
	string& userVariableClass,
	string& endUserCode)
{
	string startSymbol ("");
	int tokenEnumeration = 256;

	dParcerLexical lexical (inputRules.c_str());
	LoadTemplateFile("/dParcerUserVariableTemplate.cpp", userVariableClass);
	// scan the definition segment
	for (dToken token = dToken(lexical.NextToken()); token != GRAMMAR_SEGMENT; ) {
//		DTRACE (("%s\n", lexical.GetTokenString()));
		switch (int (token)) 
		{
			case START:
			{
				token = dToken(lexical.NextToken());
				startSymbol = lexical.GetTokenString();
				token = dToken(lexical.NextToken());
				break;
			}

			case TOKEN:
			{
				for (token = dToken(lexical.NextToken()); token == LITERAL; token = dToken(lexical.NextToken())) {
					const char* const name = lexical.GetTokenString();
					tokenEnumerationMap.Insert(tokenEnumeration, name);
					tokenEnumeration ++;
					symbolList.Insert(TERMINAL, name);
				}
				break;
			}

			case UNION:
			{
				token = dToken(lexical.NextToken());
				_ASSERTE (token == USER_ACTION);
				userVariableClass = lexical.GetTokenString() + 1;
				userVariableClass.replace(userVariableClass.size() - 1, 1, "");
				token = dToken(lexical.NextToken());
				break;
			}


			case CODE_BLOCK:
			{
				userCodeBlock += lexical.GetTokenString();
				token = dToken(lexical.NextToken());
				break;
			}

			default:;
			{
				token = dToken(lexical.NextToken());
			}
		}
	}

	int ruleNumber = 1;

	// scan the production rules segment
	dToken token1 = dToken(lexical.NextToken());
	for (; (token1 != GRAMMAR_SEGMENT) && (token1 != -1); token1 = dToken(lexical.NextToken())) {
//		DTRACE (("%s\n", lexical.GetTokenString()));
		switch (int (token1)) 
		{
			case LITERAL:
			{
				// add the first Rule;
				dRuleInfo& rule = ruleList.Append()->GetInfo();
				rule.m_token = token1;
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

				token1 = ScanGrammarRule(lexical, ruleList, symbolList, ruleNumber, tokenEnumerationMap, tokenEnumeration); 
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

	//Expand the Grammar Rule by adding an empty start Rule;
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
	if (token1 == GRAMMAR_SEGMENT) {
		endUserCode = lexical.GetNextBuffer();
	}
}

dParcerCompiler::dToken dParcerCompiler::ScanGrammarRule(
	dParcerLexical& lexical, 
	dProductionRule& rules, 
	dTree<dTokenType, string>& symbolList, 
	int& ruleNumber,
	dTree<int, string>& tokenEnumerationMap,
	int& tokenEnumeration)
{
	struct dTokenStringPair 
	{
		dToken m_token;
		string m_info;
	};

	dRuleInfo* currentRule = &rules.GetLast()->GetInfo();
	
	dToken token = dToken(lexical.NextToken());
	do {
		
		dList<dTokenStringPair> ruleTokens;
		for (token = dToken(lexical.NextToken()); (token != SIMICOLOM) && (token != OR) ; token = dToken(lexical.NextToken())) {
			dTokenStringPair& pair = ruleTokens.Append()->GetInfo();
			pair.m_token = token;
			pair.m_info = lexical.GetTokenString();
		}
		
		dList<dTokenStringPair>::dListNode* lastNode = ruleTokens.GetLast();
		if (lastNode->GetInfo().m_token != USER_ACTION) {
			lastNode = NULL;
		} else {
			currentRule->m_userActionCode = lastNode->GetInfo().m_info;
		}
		for (dList<dTokenStringPair>::dListNode* node = ruleTokens.GetFirst(); node != lastNode; node = node->GetNext()) {
			dTokenStringPair& pair = node->GetInfo();
			if (pair.m_token == LITERAL) {
				dSymbol& symbol = currentRule->Append()->GetInfo();
				symbol.m_token = pair.m_token;
				symbol.m_name = pair.m_info;

				dTree<dTokenType, string>::dTreeNode* symbolNode = symbolList.Find(symbol.m_name);
				if (!symbolList.Find(symbol.m_name)) {
					symbolNode = symbolList.Insert(NONTERMINAL, symbol.m_name);
				}
				symbol.m_type = symbolNode->GetInfo();

				dTree<int, string>::dTreeNode* nonTerminalIdNode = tokenEnumerationMap.Find(symbol.m_name);
				if (!nonTerminalIdNode) {
					nonTerminalIdNode = tokenEnumerationMap.Insert(tokenEnumeration, symbol.m_name);
					tokenEnumeration ++;
				}
			} else if (isascii (pair.m_token)) {
				dSymbol& symbol = currentRule->Append()->GetInfo();
				symbol.m_name = pair.m_info;
				symbol.m_type = TERMINAL;
				symbol.m_token = LITERAL;
				symbolList.Insert(TERMINAL, symbol.m_name);
				tokenEnumerationMap.Insert(token, symbol.m_name);

			} else if (pair.m_token != USER_ACTION) {
				// no user action allowed in the middle of a sentence
				_ASSERTE (pair.m_token == USER_ACTION);
			} else {
				_ASSERTE (0);
			}
		}

		if (token == OR) {
			// this is a rule with multiples sentences alternates, add new rule with the same name Non terminal
			dRuleInfo& rule = rules.Append()->GetInfo();
			rule.m_ruleNumber = ruleNumber;
			ruleNumber ++;
			rule.m_ruleId = currentRule->m_ruleId;
			rule.m_token = currentRule->m_token;
			rule.m_type = NONTERMINAL;
			rule.m_name += currentRule->m_name;
			currentRule = &rule;
		}

	} while (token != SIMICOLOM);

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
void dParcerCompiler::CanonicalItemSets (dTree<dState*,int>& stateMap, dProductionRule& ruleList, dTree<dTokenType, string>& symbolList)
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

		dTree<dTokenType, string>::Iterator iter (symbolList);
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



void dParcerCompiler::BuildParcingTable (dTree<dState*,int>& stateList, dTree<dTokenType, string>& symbolList)
{
	dTree<dState*,int>::Iterator stateIter (stateList);
	dTree<dTokenType, string>::Iterator symbolIter (symbolList);

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
					dTokenType type = symbolIter.GetNode()->GetInfo();
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
						dTokenType type = symbolIter.GetNode()->GetInfo();
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

void dParcerCompiler::LoadTemplateFile(const char* const templateName, string& templateOuput) const
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
	sprintf (ptr, templateName);

	FILE* const templateFile = fopen (path, "r");
	_ASSERTE (templateFile);

	fseek (templateFile, 0, SEEK_END);
	int size = ftell (templateFile) + 1;
	fseek (templateFile, 0, SEEK_SET);

	templateOuput = ("") ;
	templateOuput.resize(size);
	fread ((void*)templateOuput.c_str(), 1, size, templateFile);
	fclose (templateFile);	

	templateOuput.erase(strlen (templateOuput.c_str()));
}

void dParcerCompiler::SaveFile(const char* const fileName, const char* const extention, const string& input) const
{
	char path[2048];

	strcpy (path, fileName);
	char* const ptr1 = strrchr (path, '.');
	if (ptr1) {
		*ptr1 = 0;
	}
	strcat (path, extention);
	FILE* const headerFile = fopen (path, "w");
	_ASSERTE (headerFile);
	fprintf (headerFile, "%s", input.c_str());
	fclose (headerFile);
}

void dParcerCompiler::ReplaceMacro (string& data, const string& newName, const string& macro) const
{
	int size = macro.size();
	int position = data.find (macro);
	data.replace(position, size, newName);
}

void dParcerCompiler::ReplaceAllMacros (string& data, const string& newName, const string& macro) const
{
	int size = macro.size();
	for (size_t i = data.find (macro); i != -1; i = data.find (macro)) {
		data.replace(i, size, newName);
	}
}


void dParcerCompiler::GenerateHeaderFile (
	const string& className, 
	const string& scannerClassName,
	const char* const outputFileName,
	dProductionRule& ruleList, 
	dTree<int, string>& tokenEnumerationMap)
{
	string templateHeader ("");
	LoadTemplateFile("/dParcerTemplate.h", templateHeader);

	ReplaceAllMacros (templateHeader, className, "$(className)");
	ReplaceAllMacros (templateHeader, scannerClassName, "$(scannerClass)");

	string enumdTokens ("");
	bool firstdToken = true;
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
						enumdTokens += "\t\t";
						enumdTokens += symbol.m_name;
						if (firstdToken) {
							firstdToken = false;
							enumdTokens += " = 256,\n";
						} else {
							enumdTokens += ",\n";
						}
					}
				}
			}
		}
	}

	enumdTokens.replace(enumdTokens.size()-2, 2, "");

	ReplaceMacro (templateHeader, enumdTokens, "$(dTokens)");

	SaveFile(outputFileName, ".h", templateHeader);
}


void dParcerCompiler::GenerateParcerCode (
	const string& className, 
	const string& scannerClassName,
	const char* const outputFileName,
	const string& userCode,
	const string& userVariable,
	const string& userVariableClass, 
	dTree<dState*,int>& stateList, 
	dTree<dTokenType, string>& symbolList,
	string& endUserCode)
{
	string templateHeader ("");
	LoadTemplateFile("/dParcerTemplate.cpp", templateHeader);

	size_t position = templateHeader.find ("$(userCode)");
	templateHeader.replace(position, 11, userCode);

	ReplaceAllMacros (templateHeader, className, "$(className)");
	ReplaceAllMacros (templateHeader, scannerClassName, "$(scannerClass)");

	ReplaceMacro (templateHeader, userVariableClass, "$(userVariableClass)");
	ReplaceAllMacros (templateHeader, userVariable, "$(userVariable)");

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

	templateHeader += endUserCode;

	SaveFile(outputFileName, ".cpp", templateHeader);
}

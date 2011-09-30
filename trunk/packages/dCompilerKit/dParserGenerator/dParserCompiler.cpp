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

// dParserCompiler.cpp : Defines the entry point for the console application.
//

#include "dParserCompiler.h"
#include "dParserLexical.h"

#define DACCEPT_SYMBOL "$$$"
#define DACCEPTING_TOKEN 255	

#define DDEBUG_STATES

#define DERROR_SYMBOL	"error"
#define DERROR_TOKEN	254



//The Parcel input file consists of three sections, separated by a line containing only `%%'. 
//
//	definitions
//	%%
//	production rules
//	%%
//	user code

enum dParserCompiler::dTokenType
{
	TERMINAL,
	NONTERMINAL
};


enum dParserCompiler::ActionType
{
	dSHIFT = 0,
	dREDUCE,
	dACCEPT,
	dERROR
};

class dParserCompiler::dTokenInfo
{
	public:
	dTokenInfo (int tokenId, dTokenType type, const string& name)
		:m_tokenId (tokenId), m_type(type), m_name(name)
	{
	}

	int m_tokenId;
	dTokenType m_type;
	string m_name;
};

class dParserCompiler::dGotoEntry
{
	public:
	dGotoEntry ()
	{
	}
	short  m_token;
	short  m_nextState;
};


class dParserCompiler::dActionEntry
{
	public:
	dActionEntry ()
	{
	}

	short m_token;
	short m_stateType;// 0 = shift, 1 = reduce, 2 = accept
	short m_nextState;
	short m_ruleSymbols;
	short m_ruleIndex;
};



class dParserCompiler::dSymbol
{
	public:
	dTokenType m_type;
	dToken m_token;
	string m_name;
	unsigned m_nameCRC;
};


class dParserCompiler::dRuleInfo: public dParserCompiler::dSymbol, public dList<dParserCompiler::dSymbol>
{
	public:
	dRuleInfo()
		:dSymbol(), dList<dSymbol>()
		,m_ruleId(0), m_ruleNumber(0), m_ruleReduced(false), m_semanticActionCode("")
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

	int m_ruleId;
	int m_ruleNumber;
	bool m_ruleReduced;
	string m_semanticActionCode;

};


class dParserCompiler::dProductionRule: public dList<dParserCompiler::dRuleInfo>
{
	public:

	dListNode* Find (unsigned nameCRC) const
	{
		for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
			if (node->GetInfo().m_nameCRC == nameCRC) {
				return node;
			}
		}

		_ASSERTE (0);
		return NULL;
	}
};

class dParserCompiler::dTransition
{
	public:
	string m_name;
	unsigned m_symbol;
	dTokenType m_type;
	dState* m_targetState;
};

class dParserCompiler::dItem
{
	public:
	dItem ()
		:m_indexMarker(0), m_lookAheadSymbol(0), m_lastOperatorSymbol(0), m_lookAheadSymbolName(""), m_lastOperatorSymbolName (""), m_ruleNode(NULL)
	{
	}

	int m_indexMarker;
	
	unsigned m_lookAheadSymbol;
	unsigned m_lastOperatorSymbol;
	string m_lookAheadSymbolName;
	string m_lastOperatorSymbolName;
	dProductionRule::dListNode* m_ruleNode;
};

class dParserCompiler::dAction 
{
	public:
	ActionType m_type;
	int m_nextState;
	dItem* m_myItem;
	dProductionRule::dListNode* m_reduceRuleNode;
};

class dParserCompiler::dTokenStringPair 
{
	public:
	dToken m_token;
	string m_info;
};


class dParserCompiler::dState: public dList<dParserCompiler::dItem>
{
	public:
	class dItemKey
	{
		public:
		dItemKey (const dItem& item)
			:m_lookAheadSymbol (item.m_lookAheadSymbol), m_rule (item.m_ruleNode)
		{
		}

		dItemKey (unsigned symbol, dProductionRule::dListNode* rule)
			:m_lookAheadSymbol (symbol), m_rule (rule)
		{
		}

		bool operator< (const dItemKey& key) const
		{	
			if (m_lookAheadSymbol < key.m_lookAheadSymbol) {
				return true;	
			} else if (m_lookAheadSymbol == key.m_lookAheadSymbol){
				if (m_rule < key.m_rule) {
					return true;	
				} 
			}
			return false;
		}

		bool operator> (const dItemKey& key) const
		{
			if (m_lookAheadSymbol > key.m_lookAheadSymbol) {
				return true;	
			} else if (m_lookAheadSymbol == key.m_lookAheadSymbol){
				if (m_rule > key.m_rule) {
					return true;	
				} 
			}
			return false;
		}

		unsigned m_lookAheadSymbol;
		dProductionRule::dListNode* m_rule;
		
		
	};

	dState (const dList<dItem>& itemSet)
		:m_key(0), m_number(0), m_goto(), m_actions(), m_transitions()
	{
		for (dListNode* node = itemSet.GetFirst(); node; node = node->GetNext()) {
			AddItem (node->GetInfo());
		}
	}

	void AddItem (const dItem& item)
	{
		dState::dListNode* const node = Append(item);
		
		dItemKey key (item);
		dTree<dList<dState::dListNode*>, dItemKey>::dTreeNode* mapNode = m_itemMap.Find (key);
		if (!mapNode) {
			mapNode = m_itemMap.Insert (key);
		}

		dList<dState::dListNode*>& bucket = mapNode->GetInfo();
		// check if bucket is not too big
		_ASSERTE (bucket.GetCount() < 16);
		bucket.Append(node);
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
			int index = 0;
			key = dCRC (item.m_lookAheadSymbolName.c_str(), key);
			for (dRuleInfo::dListNode* node = item.m_ruleNode->GetInfo().GetFirst(); node; node = node->GetNext()) {
				if (index == item.m_indexMarker) {
					key = dCRC (".", key);
				}
				const dSymbol& info = node->GetInfo();
				key = dCRC (info.m_name.c_str(), key);
				index ++;
			}
			if (item.m_indexMarker == item.m_ruleNode->GetInfo().GetCount()) {
				key = dCRC (".", key);
			}
		}
		m_key = key;
	}

	dListNode* FindItem (dProductionRule::dListNode* const rule, int marker, unsigned lookAheadSymbol) const
	{
		dItemKey key (lookAheadSymbol, rule);
		dTree<dList<dState::dListNode*>, dItemKey>::dTreeNode* const mapNode = m_itemMap.Find (key);
		if (mapNode) {
			dList<dState::dListNode*>& bucket = mapNode->GetInfo();
			for (dList<dState::dListNode*>::dListNode* node = bucket.GetFirst(); node; node = node->GetNext()) {
				dState::dListNode* const nodeItem = node->GetInfo();
				const dItem& item = nodeItem->GetInfo();
				if (item.m_indexMarker == marker) {
					_ASSERTE (item.m_ruleNode == rule);
					_ASSERTE (item.m_lookAheadSymbol == lookAheadSymbol);
					return nodeItem;
				}
			}
		}
		return NULL;
	}


	void Trace(FILE* const debugFile) const
	{
		fprintf (debugFile, "state %d:\n", m_number);
		for (dState::dListNode* itemNode = GetFirst(); itemNode; itemNode = itemNode->GetNext()) {
			dItem& item = itemNode->GetInfo();
			fprintf (debugFile, "%s -> ", item.m_ruleNode->GetInfo().m_name.c_str());

			int index = 0;
			bool hasIndex = false;
			dRuleInfo::dListNode* node = item.m_ruleNode->GetInfo().GetFirst();
			for (; node; node = node->GetNext()) {

				if (index == item.m_indexMarker) {
					fprintf (debugFile, ". ");
					hasIndex = true;
					break;
				}
				const dSymbol& info = node->GetInfo();
				fprintf (debugFile, "%s ", info.m_name.c_str());
				index ++;
			}

			for (; node; node = node->GetNext()) {
				const dSymbol& info = node->GetInfo();
				fprintf (debugFile, "%s ", info.m_name.c_str());
			}
			if (!hasIndex) {
				fprintf (debugFile, ". ");
			}
			fprintf (debugFile, ":: %s\n", item.m_lookAheadSymbolName.c_str());
		}
		fprintf (debugFile, "\n");
	}

	int m_key;
	int m_number;
	dTree<dState*, unsigned> m_goto; 
	dTree<dAction, unsigned> m_actions; 
	dList<dTransition> m_transitions;
	dTree<dList<dState::dListNode*>, dItemKey> m_itemMap;
};

class dParserCompiler::dOperatorsAssociation: public dList <unsigned>
{
	public:
	enum dAssoctivity
	{
		m_left,
		m_right
	};

	bool FindOperator(unsigned symbol) const
	{
		for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
			if (symbol == node->GetInfo()) {
				return true;
			}
		}
		return false;
	}

	int m_prioprity;
	dAssoctivity m_associativity;
};

class dParserCompiler::dOperatorsPrecedence: public dList <dOperatorsAssociation>
{
	public:
	const dOperatorsAssociation* FindAssociation (unsigned symbol)	const
	{
		for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
			const dOperatorsAssociation& association = node->GetInfo();
			if (association.FindOperator(symbol)) {
				return &node->GetInfo();
			}
		}

		return NULL;
	}

	void SaveLastOperationSymbol (const dState* const state) const
	{
		for (dState::dListNode* node = state->GetFirst(); node; node = node->GetNext()) {
			dItem& item = node->GetInfo();
			const dRuleInfo& ruleInfo = item.m_ruleNode->GetInfo();
			for (dRuleInfo::dListNode* infoSymbolNode = ruleInfo.GetFirst(); infoSymbolNode; infoSymbolNode = infoSymbolNode->GetNext()) {
				const dSymbol& infoSymbol = infoSymbolNode->GetInfo();
				if (FindAssociation(infoSymbol.m_nameCRC)) {
					item.m_lastOperatorSymbol = infoSymbol.m_nameCRC;
					item.m_lastOperatorSymbolName = infoSymbol.m_name;
				}
			}
		}
	}

};

dParserCompiler::dParserCompiler(const string& inputRules, const char* const outputFileName, const char* const scannerClassName)
{
	// scan the grammar into a list of rules.
	int lastTerminalToken;
	dProductionRule ruleList;
	dOperatorsPrecedence operatorPrecedence;
	dTree<dTokenInfo, unsigned> symbolList;
	string userCodeBlock;
	string userVariableClass ("");
	string endUserCode ("\n");

	// scan grammar to a set of LR(1) rules
	symbolList.Insert(dTokenInfo (DACCEPTING_TOKEN, TERMINAL, DACCEPT_SYMBOL), dCRC (DACCEPT_SYMBOL));
	symbolList.Insert(dTokenInfo (DERROR_TOKEN, TERMINAL, DERROR_SYMBOL), dCRC (DERROR_SYMBOL));

	ScanGrammarFile(inputRules, ruleList, symbolList, operatorPrecedence, userCodeBlock, userVariableClass, endUserCode, lastTerminalToken);

	// convert the rules into a NFA.
	string debugFileName (outputFileName);
	debugFileName += ".txt";
	FILE* const debugFile = fopen (debugFileName.c_str(), "w");

	dTree<dState*,int> stateList;
	CanonicalItemSets (stateList, ruleList, symbolList, operatorPrecedence, debugFile);
	fclose (debugFile);


	// create a LR(1) parsing table from the NFA graphs
	const string& startSymbol = ruleList.GetFirst()->GetInfo().m_name;
	BuildParcingTable (stateList, dCRC (startSymbol.c_str()), operatorPrecedence);


	//Write Parser class and header file
	string className (GetClassName(outputFileName));
	GenerateHeaderFile (className, scannerClassName, outputFileName, symbolList, userVariableClass);
	GenerateParserCode (className, scannerClassName, outputFileName, symbolList, stateList, userCodeBlock, endUserCode, lastTerminalToken);

	dTree<dState*,int>::Iterator iter(stateList);
	for (iter.Begin(); iter; iter ++) {
		dState* const state = iter.GetNode()->GetInfo();
		delete state;
	}
}


dParserCompiler::~dParserCompiler()
{
}

string dParserCompiler::GetClassName(const char* const fileName) const
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




void dParserCompiler::LoadTemplateFile(const char* const templateName, string& templateOuput) const
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

	FILE* const templateFile = fopen (path, "rb");
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

void dParserCompiler::SaveFile(const char* const fileName, const char* const extention, const string& input) const
{
	char path[2048];

	strcpy (path, fileName);
	char* const ptr1 = strrchr (path, '.');
	if (ptr1) {
		*ptr1 = 0;
	}
	strcat (path, extention);
	FILE* const headerFile = fopen (path, "wb");
	_ASSERTE (headerFile);
	fprintf (headerFile, "%s", input.c_str());
	fclose (headerFile);
}



void dParserCompiler::ScanGrammarFile(
	const string& inputRules, 
	dProductionRule& ruleList, 
	dTree<dTokenInfo, unsigned>& symbolList, 
	dOperatorsPrecedence& operatorPrecedence,
	string& userCodeBlock,
	string& userVariableClass,
	string& endUserCode,
	int& lastTokenEnum)
{
	string startSymbol ("");
	int tokenEnumeration = 256;
	int operatorPrecedencePriority = 0;

	dParserLexical lexical (inputRules.c_str());
	LoadTemplateFile("/dParserUserVariableTemplate.cpp", userVariableClass);

	// scan the definition segment
	for (dToken token = dToken(lexical.NextToken()); token != GRAMMAR_SEGMENT; ) {
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
					symbolList.Insert(dTokenInfo (tokenEnumeration, TERMINAL, name), dCRC (name));
					tokenEnumeration ++;
				}
				break;
			}

			case LEFT:
			case RIGHT:
			{
				dOperatorsAssociation& association = operatorPrecedence.Append()->GetInfo();
				association.m_prioprity = operatorPrecedencePriority;
				operatorPrecedencePriority ++;
				switch (int (token))
				{
					case LEFT:
						association.m_associativity = dOperatorsAssociation::m_left;
						break;
					case RIGHT:
						association.m_associativity = dOperatorsAssociation::m_right;
						break;
				}

				for (token = dToken(lexical.NextToken()); (token == LITERAL) || ((token < 256) && !isalnum (token)); token = dToken(lexical.NextToken())) {
					association.Append(dCRC (lexical.GetTokenString()));
				}
				break;
			}


			case UNION:
			{
				token = dToken(lexical.NextToken());
				_ASSERTE (token == SEMANTIC_ACTION);
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
				_ASSERTE (0);
				token = dToken(lexical.NextToken());
			}
		}
	}


	int ruleNumber = 1;
	lastTokenEnum = tokenEnumeration;

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
				rule.m_nameCRC = dCRC (lexical.GetTokenString());

//				dTree<dTokenInfo, string>::dTreeNode* nonTerminalIdNode = symbolList.Find(rule.m_name);
				dTree<dTokenInfo, unsigned>::dTreeNode* nonTerminalIdNode = symbolList.Find(rule.m_nameCRC);
				if (!nonTerminalIdNode) {
					nonTerminalIdNode = symbolList.Insert(dTokenInfo (tokenEnumeration, NONTERMINAL, rule.m_name), rule.m_nameCRC);
					tokenEnumeration ++;
				}
				rule.m_ruleId = nonTerminalIdNode->GetInfo().m_tokenId;
				rule.m_ruleNumber = ruleNumber;
				ruleNumber ++;

				token1 = ScanGrammarRule(lexical, ruleList, symbolList, ruleNumber, tokenEnumeration); 
				break;
			}
			default:
				_ASSERTE (0);
		}
	}

	dProductionRule::dListNode* firtRuleNode = ruleList.GetFirst();	
	if (startSymbol != "") {
		firtRuleNode = ruleList.Find (dCRC (startSymbol.c_str()));	
	}

	//Expand the Grammar Rule by adding an empty start Rule;
	const dRuleInfo& firstRule = firtRuleNode->GetInfo();

	dRuleInfo& rule = ruleList.Addtop()->GetInfo();
	rule.m_ruleNumber = 0;
	rule.m_ruleId = tokenEnumeration;
	rule.m_token = firstRule.m_token;
	rule.m_type = NONTERMINAL;
	rule.m_name = firstRule.m_name + string("__");
	rule.m_nameCRC = dCRC (rule.m_name.c_str());
	symbolList.Insert(dTokenInfo (tokenEnumeration, rule.m_type, rule.m_name), rule.m_nameCRC);
	tokenEnumeration ++;
	
	dSymbol& symbol = rule.Append()->GetInfo();
	symbol.m_token = firstRule.m_token;
	symbol.m_type = firstRule.m_type;
	symbol.m_name = firstRule.m_name;
	symbol.m_nameCRC = firstRule.m_nameCRC;

	// scan literal use code
	if (token1 == GRAMMAR_SEGMENT) {
		endUserCode = lexical.GetNextBuffer();
		endUserCode += "\n";
	}
}


dParserCompiler::dToken dParserCompiler::ScanGrammarRule(
	dParserLexical& lexical, 
	dProductionRule& rules, 
	dTree<dTokenInfo, unsigned>& symbolList, 
	int& ruleNumber,
	int& tokenEnumeration)
{

	dRuleInfo* currentRule = &rules.GetLast()->GetInfo();
	dToken token = dToken(lexical.NextToken());
	do {
		
		dList<dTokenStringPair> ruleTokens;
		for (token = dToken(lexical.NextToken()); !((token == SIMICOLOM) || (token == OR)); token = dToken(lexical.NextToken())) {
			_ASSERTE (token != -1);
			dTokenStringPair& pair = ruleTokens.Append()->GetInfo();
			pair.m_token = token;
			pair.m_info = lexical.GetTokenString();
//			if (pair.m_info == DERROR_SYMBOL) {
//				_ASSERTE (0);
//				pair.m_token = DERROR_TOKEN;
//			}
		}
		
		dList<dTokenStringPair>::dListNode* lastNode = ruleTokens.GetLast();
		if (lastNode) {
			if (lastNode->GetInfo().m_token != SEMANTIC_ACTION) {
				lastNode = NULL;
			} else {
				currentRule->m_semanticActionCode = lastNode->GetInfo().m_info;
			}
		}
		for (dList<dTokenStringPair>::dListNode* node = ruleTokens.GetFirst(); node != lastNode; node = node->GetNext()) {
			dTokenStringPair& pair = node->GetInfo();
			if (pair.m_token == LITERAL) {
				dSymbol& symbol = currentRule->Append()->GetInfo();
				symbol.m_token = pair.m_token;
				symbol.m_name = pair.m_info;
				symbol.m_nameCRC = dCRC (symbol.m_name.c_str());

				dTree<dTokenInfo, unsigned>::dTreeNode* symbolNode = symbolList.Find(symbol.m_nameCRC);
				if (!symbolNode) {
					symbolNode = symbolList.Insert(dTokenInfo (tokenEnumeration, NONTERMINAL, symbol.m_name), symbol.m_nameCRC);
					tokenEnumeration ++;
				}
				symbol.m_type = symbolNode->GetInfo().m_type;

			} else if (pair.m_token < 256) {
				_ASSERTE (pair.m_info.length() == 1);
				dSymbol& symbol = currentRule->Append()->GetInfo();
				symbol.m_name = pair.m_info;
				symbol.m_nameCRC = dCRC (symbol.m_name.c_str());

				symbol.m_type = TERMINAL;
				symbol.m_token = LITERAL;
				symbolList.Insert(dTokenInfo (pair.m_token, TERMINAL, symbol.m_name), symbol.m_nameCRC);

			} else if (pair.m_token != SEMANTIC_ACTION) {
				// no user action allowed in the middle of a sentence
				_ASSERTE (pair.m_token == SEMANTIC_ACTION);
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
			//rule.m_name += currentRule->m_name;
			rule.m_name = currentRule->m_name;
			rule.m_nameCRC = currentRule->m_nameCRC;
			currentRule = &rule;
		}

	} while (token != SIMICOLOM);

	return token;

}


// generates the canonical Items set for a LR(1) grammar
void dParserCompiler::CanonicalItemSets (
	dTree<dState*,int>& stateMap, 
	const dProductionRule& ruleList, 
	const dTree<dTokenInfo, unsigned>& symbolList, 
	const dOperatorsPrecedence& operatorPrecence,
	FILE* const debugFile)
{
	dList<dItem> itemSet;
	dList<dState*> stateList;

	// start by building an item set with only the first rule
	dItem& item = itemSet.Append()->GetInfo();
	item.m_indexMarker = 0;
	item.m_lookAheadSymbol = dCRC (DACCEPT_SYMBOL);
	item.m_lookAheadSymbolName = DACCEPT_SYMBOL;
	item.m_ruleNode = ruleList.GetFirst();

	// build a rule info map
	dTree<dList<void*>, unsigned> ruleMap;
	for (dProductionRule::dListNode* ruleNode = ruleList.GetFirst(); ruleNode; ruleNode = ruleNode->GetNext()) {
		dRuleInfo& info = ruleNode->GetInfo();

		dTree<dList<void*>, unsigned>::dTreeNode* node = ruleMap.Find(info.m_nameCRC);
		if (!node) {
			node = ruleMap.Insert(info.m_nameCRC);
		}
		dList<void*>& entry = node->GetInfo();
		entry.Append(ruleNode);
	}


	// find the closure for the first this item set with only the first rule
	dState* const state = Closure (itemSet, symbolList, ruleMap);
	operatorPrecence.SaveLastOperationSymbol (state);

	stateMap.Insert(state, state->GetKey());
	stateList.Append(state);

	state->Trace(debugFile);

	// now for each state found 
	int stateNumber = 1;
	for (dList<dState*>::dListNode* node = stateList.GetFirst(); node; node = node->GetNext()) {
		dState* const state = node->GetInfo();

		dTree<dTokenInfo, unsigned>::Iterator iter (symbolList);
		for (iter.Begin(); iter; iter ++) {

			unsigned symbol = iter.GetKey();
			//const string symbolName = iter.GetNode()->GetInfo().m_name;

			dState* const newState = Goto (state, symbol, symbolList, ruleMap);

			if (newState->GetCount()) {
				const dTokenInfo& tokenInfo = iter.GetNode()->GetInfo();
				dTransition& transition = state->m_transitions.Append()->GetInfo();
				transition.m_symbol = symbol;
				transition.m_name = tokenInfo.m_name; 
				transition.m_type = tokenInfo.m_type;
				_ASSERTE (transition.m_symbol == dCRC(transition.m_name.c_str()));
				transition.m_targetState = newState;

				dTree<dState*,int>::dTreeNode* const targetStateNode = stateMap.Find(newState->GetKey());
				if (!targetStateNode) {
					newState->m_number = stateNumber;

					stateNumber ++;
					stateMap.Insert(newState, newState->GetKey());
					newState->Trace(debugFile);
					stateList.Append(newState);

					operatorPrecence.SaveLastOperationSymbol (newState);

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


// Generate the closure for a Set of Item  
dParserCompiler::dState* dParserCompiler::Closure (
	const dList<dItem>& itemSet, 
	const dTree<dTokenInfo, unsigned>& symbolList,
	const dTree<dList<void*>, unsigned>& ruleMap) const
{
	dState* const state = new dState (itemSet);
	for (dState::dListNode* itemNode = state->GetFirst(); itemNode; itemNode = itemNode->GetNext()) {
		dItem& item = itemNode->GetInfo();

		dRuleInfo::dListNode* const symbolNode = item.m_ruleNode->GetInfo().GetSymbolNodeByIndex (item.m_indexMarker);
		if (symbolNode) {

			// get Beta token string
			const dRuleInfo& rule = item.m_ruleNode->GetInfo();
			dRuleInfo::dListNode* ruleNode = rule.GetFirst();
			for (int i = 0; i < item.m_indexMarker; i ++) {
				ruleNode = ruleNode->GetNext();
			}

			dList<unsigned> firstSymbolList;
			for (ruleNode = ruleNode->GetNext(); ruleNode; ruleNode = ruleNode->GetNext()) {
				const dSymbol& symbol = ruleNode->GetInfo();
				firstSymbolList.Append(symbol.m_nameCRC);
			}

			firstSymbolList.Append(item.m_lookAheadSymbol);
			const dSymbol& sentenceSymbol = symbolNode->GetInfo();

			dTree<dList<void*>, unsigned>::dTreeNode* const ruleNodes = ruleMap.Find(sentenceSymbol.m_nameCRC);
			if (ruleNodes) {
				const dList<void*>& matchingRulesList = ruleNodes->GetInfo();
				_ASSERTE (matchingRulesList);
				for (dList<void*>::dListNode* node = matchingRulesList.GetFirst(); node; node = node->GetNext()) {
					dProductionRule::dListNode* const ruleNode = (dProductionRule::dListNode*) node->GetInfo();
					const dRuleInfo& info = ruleNode->GetInfo();
					_ASSERTE (info.m_name == sentenceSymbol.m_name);
					dTree<int, unsigned> firstList;
					First (firstSymbolList, symbolList, ruleMap, firstList);
					dTree<int, unsigned>::Iterator firstIter (firstList);
					for (firstIter.Begin(); firstIter; firstIter ++) {
						unsigned symbol = firstIter.GetKey();
						dState::dListNode* const itemNode = state->FindItem(ruleNode, 0, symbol);
						if (!itemNode) {
							dItem newItem;
							newItem.m_indexMarker = 0;
							newItem.m_ruleNode = ruleNode;
							newItem.m_lookAheadSymbol = symbol;
							newItem.m_lookAheadSymbolName = symbolList.Find (symbol)->GetInfo().m_name;
							state->AddItem(newItem);
						}
					}
				}
			}
		}
	}

	state->CalculateKey ();

	return state;
}


void dParserCompiler::First (
	const dList<unsigned>& symbolSet, 
	const dTree<dTokenInfo, unsigned>& symbolList, 
	const dTree<dList<void*>, unsigned>& ruleMap,
	dTree<int, unsigned>& firstSetOut) const
{
	if (symbolSet.GetCount() > 1) {
		//string empty ("");
		//unsigned empty = 0;

		dList<unsigned>::dListNode* node = symbolSet.GetFirst();
		bool deriveEmpty = true;
		while ((deriveEmpty) && node) {
			unsigned symbol = node->GetInfo();
			node = node->GetNext();

			dTree<int, unsigned> tmpFirst;
			dTree<int, unsigned> symbolListMark; 
			First (symbol, symbolListMark, symbolList, ruleMap, tmpFirst);
			dTree<int, unsigned>::Iterator iter (tmpFirst);
			deriveEmpty = false;  
			for (iter.Begin(); iter; iter ++) {
				unsigned symbol = iter.GetKey();
				if (symbol == 0) {
					deriveEmpty = true;  
				} else {
					firstSetOut.Insert(0, symbol);
				}
			}
		}
		if (deriveEmpty) {
			firstSetOut.Insert(0, 0);
		}

	} else  {
		unsigned symbol = symbolSet.GetFirst()->GetInfo();
		dTree<int, unsigned> symbolListMark; 
		First (symbol, symbolListMark, symbolList, ruleMap, firstSetOut);
	}
}


void dParserCompiler::First (
	unsigned symbol, 
	dTree<int, unsigned>& symbolListMark, 
	const dTree<dTokenInfo, unsigned>& symbolList, 
	const dTree<dList<void*>, unsigned>& ruleMap,
	dTree<int, unsigned>& firstSetOut) const
{
	if (symbolListMark.Find(symbol)) {
		return;
	}
	symbolListMark.Insert(0, symbol);

	dTree<dTokenInfo, unsigned>::dTreeNode* const node = symbolList.Find(symbol);
	_ASSERTE (node);
	if (node->GetInfo().m_type == TERMINAL) {
		firstSetOut.Insert(0, symbol);
	} else if (DoesSymbolDeriveEmpty (symbol, ruleMap)) {
		firstSetOut.Insert(0, 0);
	} else {

		dTree<dList<void*>, unsigned>::dTreeNode* const ruleNodes = ruleMap.Find(symbol);
		if (ruleNodes) {
			const dList<void*>& matchingRulesList = ruleNodes->GetInfo();
			for (dList<void*>::dListNode* node = matchingRulesList.GetFirst(); node; node = node->GetNext()) {
				dProductionRule::dListNode* const ruleInfoNode = (dProductionRule::dListNode*) node->GetInfo();
				const dRuleInfo& info = ruleInfoNode->GetInfo();
				//				if (symbol == info.m_name) {
				bool allDeriveEmpty = true;
				for (dRuleInfo::dListNode* sentenceSymbolNode = info.GetFirst(); sentenceSymbolNode; sentenceSymbolNode = sentenceSymbolNode->GetNext()) {
					const dSymbol& sentenceSymnol = sentenceSymbolNode->GetInfo();
					if (!DoesSymbolDeriveEmpty (sentenceSymnol.m_nameCRC, ruleMap)) {
						allDeriveEmpty = false;
						dTree<int, unsigned> newFirstSetOut;
						First (sentenceSymnol.m_nameCRC, symbolListMark, symbolList, ruleMap, newFirstSetOut);
						dTree<int, unsigned>::Iterator iter (newFirstSetOut);
						for (iter.Begin(); iter; iter ++) {
							unsigned symbol = iter.GetKey();
							_ASSERTE (symbol != 0);
							_ASSERTE (symbolList.Find(symbol)->GetInfo().m_type == TERMINAL);
							firstSetOut.Insert(0, symbol);
						}
						break;
					}
				}
				if (allDeriveEmpty) {
					firstSetOut.Insert(0, 0);
				}
			}
		}
	}
}


bool dParserCompiler::DoesSymbolDeriveEmpty (unsigned symbol, const dTree<dList<void*>, unsigned>& ruleMap) const 
{
	dTree<dList<void*>, unsigned>::dTreeNode* const ruleNodes = ruleMap.Find(symbol);
	if (ruleNodes) {
		//const dList<void*>& matchingRulesList = ruleMap.Find(symbol)->GetInfo();
		const dList<void*>& matchingRulesList = ruleNodes->GetInfo();
		for (dList<void*>::dListNode* node = matchingRulesList.GetFirst(); node; node = node->GetNext()) {
			dProductionRule::dListNode* const ruleInfoNode = (dProductionRule::dListNode*) node->GetInfo();

			const dRuleInfo& info = ruleInfoNode->GetInfo();
			if (symbol == info.m_nameCRC) {
				if (!info.GetCount()) {
					return true;
				}
			}
		}
	}
	return false;
}


// generates the got state for this symbol
dParserCompiler::dState* dParserCompiler::Goto (
	const dState* const state, 
	unsigned symbol, 
	const dTree<dTokenInfo, unsigned>& symbolList,
	const dTree<dList<void*>, unsigned>& ruleMap) const
{
	dList<dItem> itemSet;

	// iterate over each item contained on state
	for (dState::dListNode* node = state->GetFirst(); node; node = node->GetNext()) {
		const dItem& item = node->GetInfo();

		int index = 0;
		const dRuleInfo& ruleInfo = item.m_ruleNode->GetInfo();
		// for this item get it rule, and iterate over ever rule symbol.  		
		for (dRuleInfo::dListNode* infoSymbolNode = ruleInfo.GetFirst(); infoSymbolNode; infoSymbolNode = infoSymbolNode->GetNext()) {
			// see if this rule symbol match symbol
			const dSymbol& infoSymbol = infoSymbolNode->GetInfo();
			if (infoSymbol.m_nameCRC == symbol) {
				// a symbol was found, now see if it is at the right position in the rule
				if (index == item.m_indexMarker) {
					// the rule match symbol at the right position, add this rule to the new item set.
					dItem& newItem = itemSet.Append()->GetInfo();
					newItem.m_indexMarker = index + 1;
					newItem.m_ruleNode = item.m_ruleNode;
					newItem.m_lookAheadSymbol = item.m_lookAheadSymbol;
					newItem.m_lookAheadSymbolName = item.m_lookAheadSymbolName;
					break;
				}
			}
			index ++;
		}
	}

	//find the closure for this new item set.
	return Closure (itemSet, symbolList, ruleMap);
}



void dParserCompiler::BuildParcingTable (
	const dTree<dState*,int>& stateList, 
	unsigned startSymbol, 
	const dOperatorsPrecedence& operatorPrecedence) const
{
	dTree<dState*,int>::Iterator stateIter (stateList);

//	string emptySymbol ("");
	unsigned emptySymbol = 0;
//	string acceptingSymbol (DACCEPT_SYMBOL);
	const unsigned acceptingSymbol = dCRC (DACCEPT_SYMBOL);
	// create Shift Reduce action table
	for (stateIter.Begin(); stateIter; stateIter ++) {
		dState* const state = stateIter.GetNode()->GetInfo();


		// add all shift actions first
		for (dList<dTransition>::dListNode* node = state->m_transitions.GetFirst(); node; node = node->GetNext()) {
			dTransition& transition = node->GetInfo();
			if (transition.m_type == TERMINAL) {

				// find item generating this shift action and mark it as used.
				const dState* const targetState = transition.m_targetState;
				_ASSERTE (!state->m_actions.Find (transition.m_symbol));
				dTree<dAction, unsigned>::dTreeNode* const actionNode = state->m_actions.Insert (transition.m_symbol); 
				dAction& action = actionNode->GetInfo();
				action.m_type = dSHIFT;
				action.m_myItem = NULL;
				action.m_nextState = targetState->m_number;
				action.m_reduceRuleNode = NULL;
			}
		}

		// add all reduce actions
		dList<dAction*> potencialConflictinActions;
		for (dState::dListNode* itemNode = state->GetFirst(); itemNode; itemNode = itemNode->GetNext()) {
			dItem& item = itemNode->GetInfo();
			const dRuleInfo& ruleInfo = item.m_ruleNode->GetInfo();
			if ((ruleInfo.m_ruleNumber == 0) && (item.m_indexMarker == 1)) {
				dTree<dAction, unsigned>::dTreeNode* const actionNode = state->m_actions.Insert (acceptingSymbol); 
				_ASSERTE (actionNode);
				dAction& action = actionNode->GetInfo();
				action.m_type = dACCEPT;
				action.m_myItem = &item;
				action.m_reduceRuleNode = NULL;
			} else if ((item.m_indexMarker == ruleInfo.GetCount()) && (ruleInfo.m_nameCRC != startSymbol)) {
				dTree<dAction, unsigned>::dTreeNode* actionNode = state->m_actions.Find (item.m_lookAheadSymbol); 
				if (!actionNode) {
					actionNode = state->m_actions.Insert (item.m_lookAheadSymbol); 
					dAction& action = actionNode->GetInfo();
					action.m_type = dREDUCE;
					action.m_myItem = &item;
					action.m_nextState = 0;
					action.m_reduceRuleNode = item.m_ruleNode;
				} else {
					dAction& action = actionNode->GetInfo();
					action.m_myItem = &item;
					action.m_reduceRuleNode = item.m_ruleNode;
					potencialConflictinActions.Append (&actionNode->GetInfo());
				}
			}
		}

		// now resolve all conflicting actions
		if (potencialConflictinActions.GetCount()) {

			// resolve conflicting actions
			dList<dAction*>::dListNode* nextActionNode = NULL;
			for (dList<dAction*>::dListNode* actionNode = potencialConflictinActions.GetFirst(); actionNode; actionNode = nextActionNode) {
				dAction* const action = actionNode->GetInfo();

				if (action->m_type == dREDUCE) {
					// this is a reduce reduce conflict
					_ASSERTE (0);
					DTRACE (("This is a reduce Reduce conflict, resolve in favor of of first production rule\n")); 
				}
				nextActionNode = actionNode->GetNext();

				const dItem& item = *action->m_myItem;
				if (item.m_lastOperatorSymbol != emptySymbol) {
					const dOperatorsAssociation* const operatorAssosiation = operatorPrecedence.FindAssociation (item.m_lastOperatorSymbol);
					_ASSERTE (operatorAssosiation);
					if (operatorAssosiation->m_associativity == dOperatorsAssociation::m_left) {

						const dOperatorsAssociation* const lookAheadOperatorAssosiation = operatorPrecedence.FindAssociation (item.m_lookAheadSymbol);
						if (!(lookAheadOperatorAssosiation && (lookAheadOperatorAssosiation->m_prioprity > operatorAssosiation->m_prioprity))) {
							action->m_type = dREDUCE;
						}
					}
					potencialConflictinActions.Remove(actionNode);
				}
			}

			// for any conflicting actions left, display warning
			for (dList<dAction*>::dListNode* actionNode = potencialConflictinActions.GetFirst(); actionNode; actionNode = actionNode->GetNext()) {
				_ASSERTE (0);
				dAction* const action = actionNode->GetInfo();
				if (action->m_type == dSHIFT) {
					DTRACE (("This is a shift Reduce conflict, resolve in favor of shift\n")); 
				} else {
					DTRACE (("This is a reduce Reduce conflict, resolving by the first reduce rule\n"));
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
				state->m_goto.Insert (transition.m_targetState, transition.m_symbol); 
			}
		}
	}
}

void dParserCompiler::ReplaceMacro (string& data, const string& newName, const string& macro) const
{
	int size = macro.size();
	int position = data.find (macro);
	_ASSERTE (position != -1);
	data.replace(position, size, newName);
}

void dParserCompiler::ReplaceAllMacros (string& data, const string& newName, const string& macro) const
{
	int size = macro.size();
	for (size_t i = data.find (macro); i != -1; i = data.find (macro)) {
		data.replace(i, size, newName);
	}
}


void dParserCompiler::GenerateHeaderFile (
	const string& className, 
	const string& scannerClassName, 
	const char* const outputFileName, 
	const dTree<dTokenInfo, unsigned>& symbolList, 
	const string& userVariableClass) 
{
	string templateHeader ("");
	LoadTemplateFile("/dParserTemplate.h", templateHeader);

	ReplaceAllMacros (templateHeader, className, "$(className)");
	ReplaceAllMacros (templateHeader, scannerClassName, "$(scannerClass)");
	ReplaceMacro (templateHeader, userVariableClass, "$(userVariableClass)");


	dTree<dTree<dTokenInfo, unsigned>::dTreeNode*, int> sortToken;
	dTree<dTokenInfo, unsigned>::Iterator iter (symbolList);
	for (iter.Begin(); iter; iter ++) {
		const dTokenInfo& info = iter.GetNode()->GetInfo();
		if ((info.m_type == TERMINAL) && (info.m_tokenId >= 256)) {
			sortToken.Insert(iter.GetNode(), info.m_tokenId);
		}
	} 

	dTree<dTree<dTokenInfo, unsigned>::dTreeNode*, int>::Iterator iter1 (sortToken);
	bool first = true;
	char text[256];
	sprintf (text, " = %d, \n", DACCEPTING_TOKEN);
	string enumdTokens ("\t\tACCEPTING_TOKEN");
	enumdTokens += text;

	sprintf (text, " = %d, \n", DERROR_TOKEN);
	enumdTokens += "\t\tERROR_TOKEN";
	enumdTokens += text;

	for (iter1.Begin(); iter1; iter1 ++) {
		dTree<dTokenInfo, unsigned>::dTreeNode* const node = iter1.GetNode()->GetInfo();
		//const string& name = node->GetKey();
		const string& name = node->GetInfo().m_name;
		enumdTokens += "\t\t";
		enumdTokens += name;
		if (first) {
			first = false;
			sprintf (text, " = %d, \n", iter1.GetKey());
			enumdTokens += text;
		} else {
			enumdTokens += ",\n";
		}
	}

	enumdTokens.replace(enumdTokens.size()-2, 2, "");
	ReplaceMacro (templateHeader, enumdTokens, "$(Tokens)");

	SaveFile(outputFileName, ".h", templateHeader);
}


void dParserCompiler::GenerateParserCode (
	const string& className, 
	const string& scannerClassName, 
	const char* const outputFileName, 
	const dTree<dTokenInfo, unsigned>& symbolList, 
	dTree<dState*,int>& stateList, 
	const string& userCode, 
	string& endUserCode,
	int lastTerminalTokenEnum)
{
	string templateHeader ("");
	LoadTemplateFile("/dParserTemplate.cpp", templateHeader);

	size_t position = templateHeader.find ("$(userCode)");
	templateHeader.replace(position, 11, userCode);

	ReplaceAllMacros (templateHeader, className, "$(className)");
	ReplaceAllMacros (templateHeader, scannerClassName, "$(scannerClass)");


	char text[256];
	sprintf (text, "%d", lastTerminalTokenEnum);
	ReplaceMacro (templateHeader, text, "&(lastTerminalToken)");

	dTree<dState*,int> sortedStates;
	dTree<dState*,int>::Iterator stateIter (stateList);
	for (stateIter.Begin(); stateIter; stateIter ++) {
		dState* const state = stateIter.GetNode()->GetInfo();
		sortedStates.Insert(state, state->m_number);
	}

	dTree<int, string> actionFilter;

	string emptySematicAction ("");
	string stateActionsStart ("");
	string stateActionsCount ("");
	string nextActionsStateList ("");
	string sematicActions ("");
	int entriesCount = 0;

	int newLineCount = 0;
	int starAndCountIndex = 0;
	dTree<dState*,int>::Iterator sortStateIter (sortedStates);
	for (sortStateIter.Begin(); sortStateIter; sortStateIter ++) {
		dState* const state = sortStateIter.GetNode()->GetInfo();

		int count = 0;
		dTree<dActionEntry, int> actionSort;
		dTree<dAction, unsigned>::Iterator actionIter (state->m_actions);
		for (actionIter.Begin(); actionIter; actionIter++) {
			count ++;

			dAction& action = actionIter.GetNode()->GetInfo();
			if (action.m_type == dSHIFT) {
				unsigned actionSymbol = actionIter.GetKey();
				_ASSERTE (symbolList.Find(actionSymbol));

				dActionEntry entry;
				entry.m_stateType = short (action.m_type);
				entry.m_ruleIndex = 0;
				entry.m_ruleSymbols = 0;
				entry.m_nextState = short (action.m_nextState);
				entry.m_token = short (symbolList.Find(actionSymbol)->GetInfo().m_tokenId);

				actionSort.Insert (entry, entry.m_token);

			} else if (action.m_type == dREDUCE) {

				unsigned actionSymbol = actionIter.GetKey();
				_ASSERTE (symbolList.Find(actionSymbol));

				dRuleInfo& reduceRule = action.m_reduceRuleNode->GetInfo();
				_ASSERTE (symbolList.Find(reduceRule.m_nameCRC));
				_ASSERTE (symbolList.Find(reduceRule.m_nameCRC)->GetInfo().m_tokenId >= 256);

				dActionEntry entry;
				entry.m_stateType = short (action.m_type);
				entry.m_ruleIndex = short (reduceRule.m_ruleNumber);
				entry.m_ruleSymbols = short (reduceRule.GetCount());
				entry.m_nextState = short (symbolList.Find(reduceRule.m_nameCRC)->GetInfo().m_tokenId - lastTerminalTokenEnum);
				entry.m_token = short (symbolList.Find(actionSymbol)->GetInfo().m_tokenId);
				actionSort.Insert (entry, entry.m_token);

				if (!reduceRule.m_ruleReduced && (reduceRule.m_semanticActionCode != emptySematicAction)) {
					// issue a sematic action code;

					reduceRule.m_ruleReduced = true;
					char text[128];
					string userSematicAction (reduceRule.m_semanticActionCode);
					for (int i = 0; i < int (entry.m_ruleSymbols); i ++) {

						sprintf (text, "%d", i + 1);
						string macro ("$");
						macro += text;

						sprintf (text, "%d", i);
						string macroVariable ("parameter[");
						macroVariable += text;
						macroVariable += "].m_value";
						ReplaceAllMacros (userSematicAction, macroVariable, macro);
					}
					ReplaceAllMacros (userSematicAction, "entry.m_value", "$$");

					sprintf (text, "%d:", reduceRule.m_ruleNumber);
					sematicActions += "\t\t\t\t\tcase "; 
					sematicActions += text; 
					sematicActions += "// rule ";
					sematicActions += reduceRule.m_name;
					sematicActions += " : ";
					for (dRuleInfo::dListNode* node = reduceRule.GetFirst(); node; node = node->GetNext()) {
						sematicActions+= node->GetInfo().m_name;
						sematicActions += " ";
					}
					sematicActions += "\n";

					sematicActions += "\t\t\t\t\t\t";
					sematicActions += userSematicAction;
					sematicActions += "\n";
					sematicActions += "\t\t\t\t\t\tbreak;\n";
				}

			} else {
				_ASSERTE (action.m_type == dACCEPT);

				dActionEntry entry;
				entry.m_stateType = short (action.m_type);
				entry.m_ruleIndex = 0;
				entry.m_ruleSymbols = 0;
				entry.m_nextState = 0;
				entry.m_token = DACCEPTING_TOKEN;
				actionSort.Insert (entry, entry.m_token);
			}
		}

		int actionIndex = entriesCount;
		string stateActions ("");
		dTree<dActionEntry, int>::Iterator iter (actionSort);
		for (iter.Begin(); iter; iter ++) {
			const dActionEntry& entry = iter.GetNode()->GetInfo();
			sprintf (text, "%d, %d, %d, %d, %d, ", entry.m_token, entry.m_stateType, entry.m_nextState, entry.m_ruleSymbols, entry.m_ruleIndex);
			stateActions += text; 
			entriesCount ++;
		}

		dTree<int, string>::dTreeNode* const stateActionNode = actionFilter.Find(stateActions);
		if (stateActionNode) {
			entriesCount = actionIndex;
			actionIndex =  stateActionNode->GetInfo();
		} else {
			actionFilter.Insert(actionIndex, stateActions);

			for (iter.Begin(); iter; iter ++) {
				if (newLineCount % 4 == 0) {
					nextActionsStateList += "\n\t\t\t";
				}
				newLineCount ++;
				const dActionEntry& entry = iter.GetNode()->GetInfo();
				sprintf (text, "dActionEntry (%d, %d, %d, %d, %d), ", entry.m_token, entry.m_stateType, entry.m_nextState, entry.m_ruleSymbols, entry.m_ruleIndex);
				nextActionsStateList += text; 
			}
		}

		if ((starAndCountIndex % 24) == 0) {
			stateActionsStart += "\n\t\t\t";
			stateActionsCount += "\n\t\t\t";
		}
		starAndCountIndex ++;

		sprintf (text, "%d, ", actionIndex);
		stateActionsStart += text;

		sprintf (text, "%d, ", count);
		stateActionsCount += text;
	}
	nextActionsStateList.replace(nextActionsStateList.size()-2, 2, "");
	stateActionsCount.replace(stateActionsCount.size()-2, 2, "");
	stateActionsStart.replace(stateActionsStart.size()-2, 2, "");

	ReplaceMacro (templateHeader, stateActionsCount, "$(actionsCount)");
	ReplaceMacro (templateHeader, stateActionsStart, "$(actionsStart)");
	ReplaceMacro (templateHeader, nextActionsStateList, "$(actionTable)");

	ReplaceMacro (templateHeader, sematicActions, "$(semanticActionsCode)");


	string stateGotoStart ("");
	string stateGotoCount ("");
	string nextGotoStateList ("");
	entriesCount = 0;
	int newLine = 0;
	int gotoStateCount = 0;
	for (sortStateIter.Begin(); sortStateIter; sortStateIter ++) {

		char text[256];
		dState* const state = sortStateIter.GetNode()->GetInfo();

		int currentEntryuCount = entriesCount;

		int count = 0;
		dTree<dState*, unsigned>::Iterator gotoIter (state->m_goto); 
		dTree<dTree<dState*, unsigned>::dTreeNode*, int> sortGotoActions;
		for (gotoIter.Begin(); gotoIter; gotoIter++) {
			int id = symbolList.Find(gotoIter.GetKey())->GetInfo().m_tokenId;
			sortGotoActions.Insert(gotoIter.GetNode(), id);
		}

		dTree<dTree<dState*, unsigned>::dTreeNode*, int>::Iterator iter1 (sortGotoActions);
		for (iter1.Begin(); iter1; iter1++) {
			count ++;
			if ((newLine % 5) == 0) {
				nextGotoStateList += "\n\t\t\t";
			}
			newLine ++;

			dTree<dState*, unsigned>::dTreeNode* const node = iter1.GetNode()->GetInfo();
			dState* const targetState = node->GetInfo();

			dGotoEntry entry;
			entry.m_nextState = short (targetState->m_number);
			entry.m_token = short(iter1.GetKey());

			sprintf (text, "dGotoEntry (%d, %d), ", entry.m_token, entry.m_nextState);
			nextGotoStateList += text;
			entriesCount ++;
		}

		if ((gotoStateCount % 32) == 0) {
			stateGotoStart += "\n\t\t\t";
			stateGotoCount += "\n\t\t\t";
		}
		gotoStateCount ++;

		sprintf (text, "%d, ", currentEntryuCount);
		stateGotoStart += text;

		sprintf (text, "%d, ", count);
		stateGotoCount += text;
	}

	nextGotoStateList.replace(nextGotoStateList.size()-2, 2, "");
	stateGotoCount.replace(stateGotoCount.size()-2, 2, "");
	stateGotoStart.replace(stateGotoStart.size()-2, 2, "");

	ReplaceMacro (templateHeader, stateGotoCount, "$(gotoCount)");
	ReplaceMacro (templateHeader, stateGotoStart, "$(gotoStart)");
	ReplaceMacro (templateHeader, nextGotoStateList, "$(gotoTable)");


	templateHeader += endUserCode;
	SaveFile(outputFileName, ".cpp", templateHeader);
}

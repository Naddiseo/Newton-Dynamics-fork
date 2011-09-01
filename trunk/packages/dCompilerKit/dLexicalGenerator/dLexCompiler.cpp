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

// dLexCompiler.cpp : Defines the entry point for the console application.
//

#include "dLexCompiler.h"
#include "dCompilerCore.h"


//The flex input file consists of three sections, separated by a line containing only `%%'. 
//
//	definitions
//	%%
//	rules
//	%%
//	user code

dLexCompiler::dLexCompiler(const char* const inputRules, const char* const outputFileName)
	:m_token (m_end)
	,m_grammarTokenStart(0)
	,m_grammarTokenLength(0)
	,m_grammar (inputRules)
	,m_tokenList()
{
	string automataCode ("");
	string nextCodeCases ("");
	string userPreheaderCode (""); 
	dChatertSetMap characterSet;
	dTree<int, int> transitionsCountMap;
	dTree<dTree<int, int>, int> nextState;
	dTree<dTree<char, int>, int> characterTestMap;
	dTree<dTree<int, int>, int> testSetArrayIndexMap;


	int stateCount = ParseDefinitions (userPreheaderCode, nextCodeCases, automataCode, characterSet, transitionsCountMap, nextState, characterTestMap, testSetArrayIndexMap);

	char cppFileName[256];
	strcpy (cppFileName, outputFileName);
	char* const dot = strrchr (cppFileName, '.');
	if (dot) {
		*dot = 0;
	}

	char className[256];
	const char* ptr = strrchr (cppFileName, '/');
	if (ptr) {
		ptr ++;
	} else {
		ptr = strrchr (cppFileName, '\\');
		if (ptr) {
			ptr ++;
		} else {
			ptr = cppFileName;
		}
	}

	strcpy (className, ptr);

	strcat (cppFileName, ".cpp");
	
	CreateCodeFile (cppFileName, className, stateCount, userPreheaderCode, nextCodeCases, automataCode, characterSet, 
					transitionsCountMap, nextState, characterTestMap, testSetArrayIndexMap);

	*dot = 0;
	strcat (cppFileName, ".h");
	CreateHeaderFile (cppFileName, className);
}

dLexCompiler::~dLexCompiler()
{
}

void dLexCompiler::CreateCodeFile (
	const char* const fileName, 
	const char* const className, 
	int stateCount,
	string& userPreheaderCode, 
	string& nextCodeCases, 
	string& automataCode, 
	dChatertSetMap& characterSet,
	dTree<int, int>& transitionsCount,
	dTree<dTree<int, int>, int>& nextStateMap,
	dTree<dTree<char, int>, int>& characterTestMap,
	dTree<dTree<int, int>, int>& testSetArrayIndexMap) const
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
	sprintf (ptr, "/dLexicalTemplate.cpp");

	FILE* const templateFile = fopen (path, "r");
	_ASSERTE (templateFile);

	fseek (templateFile, 0, SEEK_END);
	int size = ftell (templateFile) + 1;
	fseek (templateFile, 0, SEEK_SET);

	string templateHeader ("") ;
	templateHeader.resize(size);
	fread ((void*)templateHeader.c_str(), 1, size, templateFile);
	fclose (templateFile);	

	size_t position;
	position = templateHeader.find ("$(userIncludeCode)");
	templateHeader.replace(position, 18, userPreheaderCode);

	string name (className);
	for (size_t i = templateHeader.find ("$(className)"); i != -1; i = templateHeader.find ("$(className)")) {
		templateHeader.replace(i, 12, name);
	}

	char stateCountString[256];
	sprintf (stateCountString, "%d", stateCount + 1);
	string statesCountText (stateCountString);
	for (size_t i = templateHeader.find ("$(statesCount)"); i != -1; i = templateHeader.find ("$(statesCount)")) {
		templateHeader.replace(i, 14, statesCountText);
	}
	
	position = templateHeader.find ("$(nextTokenStart)");
	templateHeader.replace(position, 17, nextCodeCases);


	if (characterSet.GetSets().GetCount()) {
		string characterSets ("");
		string characterSetList ("");
		dTree<dList <dChatertSetMap::ChatertSet>::dListNode*, int>::Iterator iter (characterSet.GetSets());
		for (iter.Begin(); iter; iter ++) {
			dChatertSetMap::ChatertSet& set = iter.GetNode()->GetInfo()->GetInfo();
			int count = set.GetLength();

			char name[256];

			sprintf (name, "text_%d, ", set.GetId() & 0x7fff);		
			characterSetList += name;

			sprintf (name, "\tstatic char text_%d[] = {", set.GetId() & 0x7fff);
			characterSets += name;
			const char* const str = set.GetSet();
			for (int i = 0; i < count; i ++) {
				char tmp[128];
				sprintf (tmp, "%d, ", str[i]);
				characterSets += tmp;
			}
			characterSets += "0};\n";
		}
		position = templateHeader.find ("$(characterSets)");
		templateHeader.replace(position, 16, characterSets);

		characterSetList.replace(characterSetList.size()-2, 2, "");
		position = templateHeader.find ("$(characterSetArray)");
		templateHeader.replace(position, 20, characterSetList);
	} else {
		position = templateHeader.find ("$(characterSets)");
		templateHeader.replace(position, 16, "");

		position = templateHeader.find ("$(characterSetArray)");
		templateHeader.replace(position, 20, "");
	}

	for (int i = 0; i < stateCount; i ++) {
		if (!transitionsCount.Find(i)) {
			transitionsCount.Insert(0, i);
		}
	}

	string transitionsCountString ("");
	dTree<int, int>::Iterator iter1 (transitionsCount); 
	for (iter1.Begin(); iter1; iter1 ++) {
		int count = iter1.GetNode()->GetInfo();
		char text[256];
		sprintf (text, "%d, ", count);
		transitionsCountString += text;
	}
	transitionsCountString += "0";
	position = templateHeader.find ("$(transitionsCount)");
	templateHeader.replace(position, 19, transitionsCountString);


	string nextStateTable ("\n");
	dTree<dTree<int, int>, int>::Iterator iter2 (nextStateMap); 
	for (iter2.Begin(); iter2; iter2 ++) {
		dTree<int, int>& nextState = iter2.GetNode()->GetInfo();
		dTree<int, int>::Iterator iter (nextState); 
		for (int i = 0; i < stateCount; i ++) {
			if (!nextState.Find(i)) {
				nextState.Insert(0, i);
			}
		}

		nextStateTable += "\t\t{";
		for (iter.Begin(); iter; iter ++) {
			int ch = iter.GetNode()->GetInfo();
			char text[256];
			sprintf (text, "%d, ", ch);
			nextStateTable += text;
		}
		nextStateTable += "0},\n";
	}
	nextStateTable.replace(nextStateTable.size()-2, 2, "\n\t");
	position = templateHeader.find ("$(nextState)");
	templateHeader.replace(position, 12, nextStateTable);

	
	string characterTestTable ("\n");
	dTree<dTree<char, int>, int>::Iterator iter3 (characterTestMap); 
	for (iter3.Begin(); iter3; iter3 ++) {
		dTree<char, int>& characters = iter3.GetNode()->GetInfo();
		dTree<char, int>::Iterator iter (characters); 
		for (int i = 0; i < stateCount; i ++) {
			if (!characters.Find(i)) {
				characters.Insert(char(0), i);
			}
		}

		characterTestTable += "\t\t{";
		for (iter.Begin(); iter; iter ++) {
			char ch = iter.GetNode()->GetInfo();
			char text[256];
			sprintf (text, "%d, ", ch);
			characterTestTable += text;
		}
		characterTestTable += "0},\n";
	}
	characterTestTable.replace(characterTestTable.size()-2, 2, "\n\t");
	position = templateHeader.find ("$(charactesTest)");
	templateHeader.replace(position, 16, characterTestTable);

/*
	int nextStateOffset = 0;
	string characterSetIndexTableOffsets ("0, ");
	string characterSetIndexTableStates ("");
	for (int i = 0; i < stateCount; i ++) {
		char text[256];
		dTree<dTree<int, int>, int>::Iterator iter (testSetArrayIndexMap); 
		for (iter.Begin(); iter; iter ++) {
			dTree<int, int>& nextState = iter.GetNode()->GetInfo();
			dTree<int, int>::dTreeNode* const node = nextState.Find(i);
			if (node) {
				int state = node->GetInfo();
				sprintf (text, "%d, ", state);
				characterSetIndexTableStates += text;
				nextStateOffset ++;
			}
		}
		sprintf (text, "%d, ", nextStateOffset);
		characterSetIndexTableOffsets += text;
	}
	characterSetIndexTableOffsets.replace(characterSetIndexTableOffsets.size()-2, 2, "");
	position = templateHeader.find ("$(testSetArrayOffsets)");
	templateHeader.replace(position, 22, characterSetIndexTableOffsets);

	characterSetIndexTableStates.replace(characterSetIndexTableStates.size()-2, 2, "");
	position = templateHeader.find ("$(testSetArrayIndex)");
	templateHeader.replace(position, 20, characterSetIndexTableStates);
*/
{
	string characterSetIndexTable ("\n");
	dTree<dTree<int, int>, int>::Iterator iter4 (testSetArrayIndexMap); 
	for (iter4.Begin(); iter4; iter4 ++) {
		dTree<int, int>& characters = iter4.GetNode()->GetInfo();
		dTree<int, int>::Iterator iter (characters); 
		for (int i = 0; i < stateCount; i ++) {
			if (!characters.Find(i)) {
				characters.Insert(char(0), i);
			}
		}

		characterSetIndexTable += "\t\t{";
		for (iter.Begin(); iter; iter ++) {
			int ch = iter.GetNode()->GetInfo();
			char text[256];
			sprintf (text, "%d, ", ch);
			characterSetIndexTable += text;
		}
		characterSetIndexTable += "0},\n";
	}
	characterSetIndexTable.replace(characterSetIndexTable.size()-2, 2, "\n\t");
	position = templateHeader.find ("$(testSetArrayIndex__)");
	templateHeader.replace(position, 22, characterSetIndexTable);
}



	position = templateHeader.find ("$(finiteAutomataCode)");
	templateHeader.replace(position, 21, automataCode);

	FILE* const headerFile = fopen (fileName, "w");
	_ASSERTE (headerFile);
	fprintf (headerFile, "%s", templateHeader.c_str());

	fclose (headerFile);
}

void dLexCompiler::CreateHeaderFile (const char* const fileName, const char* const className) const
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
	sprintf (ptr, "/dLexicalTemplate.h");

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


	FILE* const headerFile = fopen (fileName, "w");
	_ASSERTE (headerFile);
	fprintf (headerFile, "%s", templateHeader.c_str());

	fclose (headerFile);
}

void dLexCompiler::CopyTokenStream (char* const buffer) const
{
	strncpy (buffer, &m_grammar[m_grammarTokenStart], m_grammarTokenLength);	
	buffer[m_grammarTokenLength] = 0;
}

void dLexCompiler::NextToken ()
{
	m_grammarTokenStart += m_grammarTokenLength;
	if (m_grammar[m_grammarTokenStart]) {
		
		for (bool reStart = true; reStart;) {
			reStart = false;
			for (TokenDataList::dListNode* node = m_tokenList.GetFirst(); node; node = node->GetNext()) {
				TokenData* const tokenData = node->GetInfo();

				const char* const text = &m_grammar[m_grammarTokenStart];
				int count = tokenData->FindMatch (text);
				if (count >= 0) {
					m_grammarTokenLength = count;
					if ((tokenData->m_token == m_whiteSpace) || (tokenData->m_token == m_comment)) {
						m_grammarTokenStart += m_grammarTokenLength;
						reStart = true;
						break;
					} else {
						m_token = tokenData->m_token;
						return;
					}
				} 
			}
		}
	}
	m_token = m_end;
}

void dLexCompiler::MatchToken (Token token)
{
	if (m_token == token) {
		NextToken ();
	} else {
		_ASSERTE (0);
		DTRACE(("parse error\n"));
//		m_error = true;
	}
}



// DefinitionExpression	: DefinitionBlock 
// DefinitionExpression	: DefinitionBlock DefinitionExpression | nothing

// DefinitionBlock		: m_comment

//
// id							: m_openSquareBrakect BracketedExpression m_closeSquareBrakect
// id							: m_openParentesis DefinitionExpression m_closeParentesis
// id							: .
// id							: CHARACTER 
int dLexCompiler::ParseDefinitions (
	string& preheaderCode, 
	string& nextTokeCode, 
	string& automataCode, 
	dChatertSetMap& characterSet,
	dTree<int, int>& transitionsCountMap,
	dTree<dTree<int, int>, int>& nextState,
	dTree<dTree<char, int>, int>& characterTestMap,
	dTree<dTree<int, int>, int>& testSetArrayIndexMap)
{
	// parse definitions
	{
//m_tokenList.AddTokenData (m_codeBlock, "%(:{([^{}]*):})%");
//m_tokenList.AddTokenData (m_codeBlock, "(:{(([^{}\"\']*)|(\'(.|[\n])\')|(\"(\\.|[^\"])*\"))*:})");
m_tokenList.AddTokenData (m_codeBlock, "%(:{(([^{}\"])|(\"[^\"]*\"))*:})%");


//		m_tokenList.AddTokenData (m_whiteSpace, "[ \t\v\n\f]+");
//		m_tokenList.AddTokenData (m_comment, "(/\\*([^*]|[\r\n]|(\\*+([^*/]|[\r\n])))*\\*+/)|(//.*)");
		//m_tokenList.AddTokenData (m_codeBlock, "%\\{([^%]|[\r\n]|(%+([^%}]|[\r\n])))*%+\\}");
//		m_tokenList.AddTokenData (m_codeBlock, "%(:{(([^{}\"\']*)|(\'(.|[\n])\')|(\"(\\.|[^\"])*\"))*:})%");
//		m_tokenList.AddTokenData (m_literal, "[a-zA-Z_][0-9a-zA-Z]*");
//		m_tokenList.AddTokenData (m_number, "[0-9]+");
//		m_tokenList.AddTokenData (m_intenalSize, "%[pneako]");
//		m_tokenList.AddTokenData (m_delimiter, "%%");
//		m_tokenList.AddTokenData (m_extendedRegularExpresion, "((\\[[^\\]]+\\])|[^ \t\v\n\f[]+)+");

		for (NextToken(); (m_token != m_end) && (m_token != m_delimiter);) {
			ParseDefinitionExpression (preheaderCode);
		}
		m_tokenList.DeleteAll();
	}

	// parse rules
	int initialState = 0;
	{
		//	0	m_whiteSpace,				
		//	1   m_action
		//	2	m_comment,						
		//	3	m_delimiter,
		//	4	m_verbatingText,
		//	5	m_intenalSize,
		//	6	m_number,
		//	7	m_quatedString,
		//	8	m_literal,
		//	9	m_extendedRegularExpresion,

		m_tokenList.AddTokenData (m_whiteSpace, "[ \t\v\n\f]+");
		m_tokenList.AddTokenData (m_quatedString, "\"[^\" \t\v\n\f]*\"");
		m_tokenList.AddTokenData (m_delimiter, "%%");
		m_tokenList.AddTokenData (m_comment, "(/\\*([^*]|[\r\n]|(\\*+([^*/]|[\r\n])))*\\*+/)|(//.*)");
		m_tokenList.AddTokenData (m_extendedRegularExpresion, "((\\[[^\\]]+\\])|[^ \t\v\n\f[]+)+");
		
		for (NextToken(); (m_token != m_end) && (m_token != m_delimiter); ) {

_ASSERTE (0);
			string expression (&m_grammar[m_grammarTokenStart], m_grammarTokenLength);
			Token expresionToken = m_token;

			m_grammarTokenStart += m_grammarTokenLength;
			const char* const str = &m_grammar[m_grammarTokenStart];
			int length = 0;
			for (int ch = str[length]; ch && (ch != '{') ; ch = str[length]) {
				length ++;
			}
			if (str[length] == '{') {
				m_grammarTokenStart += length;
				const char* const str = &m_grammar[m_grammarTokenStart];
				int length = 1;
				for (int ch = str[length]; ch != '\n'; ch = str[length]) {
					length ++;
					if (!ch) {
						_ASSERTE (0);
						return 0;
					}
				}
				string userAction (str, length);

				
				automataCode += "\t\t\t// ";
				automataCode += expression;
				automataCode += "\n";
				switch (expresionToken) 
				{
					case m_quatedString:
					{
						string keyword (expression.substr(1, expression.length() - 2));
						dDeterministicFiniteAutonataCompiler keywordAutomata (keyword.c_str());
						initialState = keywordAutomata.ConvertSwitchCaseStatements (automataCode, nextTokeCode, initialState, characterSet, userAction, 
																					transitionsCountMap, nextState, characterTestMap, testSetArrayIndexMap);
//						DTRACE ((automataCode.c_str()));
						break;
					}

					case m_extendedRegularExpresion:
					{
						//m_defintions
						ExpandedNFA expresionNFA (expression.c_str(), &m_defintions);
						dDeterministicFiniteAutonataCompiler expresionAutomata (expresionNFA);
						initialState = expresionAutomata.ConvertSwitchCaseStatements (automataCode, nextTokeCode, initialState, characterSet, userAction, 
																					  transitionsCountMap, nextState, characterTestMap, testSetArrayIndexMap);
//						DTRACE ((automataCode.c_str()));
						break;
					}
					default:;
						_ASSERTE (0);
				}

				m_grammarTokenStart += length;
				m_grammarTokenLength = 0;
			}
			NextToken();
		}
	}

	if (m_token != m_delimiter) {
		NextToken();
	}
	return initialState + 1;
}


// DefinitionExpression	: DefinitionBlock 
// DefinitionExpression	: DefinitionBlock DefinitionExpression | nothing
void dLexCompiler::ParseDefinitionExpression (string& preheaderCode)
{
	for (ParseDefinitionBlock (preheaderCode); (m_token != m_end) && (m_token != m_delimiter); ParseDefinitionBlock (preheaderCode));
}

// DefinitionBlock		: m_comment
void dLexCompiler::ParseDefinitionBlock (string& preheaderCode)
{
	if (m_token == m_comment) {
		MatchToken (m_token);

	} else if (m_token == m_codeBlock) {

		char* const code = new char[m_grammarTokenLength+1];
		CopyTokenStream (code);
		code[m_grammarTokenLength - 2] = 0;

		preheaderCode += code + 2;

		delete[] code;
		MatchToken (m_token);

	} else if (m_token == m_intenalSize) {
		MatchToken (m_token);
		MatchToken (m_token);

	} else if (m_token == m_literal) {
		char literal [256]; 
		char extendedRegularExpresion[256];

		CopyTokenStream (literal);
		MatchToken (m_token);
		
		CopyTokenStream (extendedRegularExpresion);
		MatchToken (m_token);

		m_defintions.AddDefinition (literal, extendedRegularExpresion);

	} else {
		_ASSERTE (0);
	}
}


dLexCompiler::DefinitionsMap::DefinitionsMap ()
	:dTree<ExpandedNFA*,int>()
{
}

dLexCompiler::DefinitionsMap::~DefinitionsMap ()
{
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		ExpandedNFA* const expression = iter.GetNode()->GetInfo();
		delete expression;
	}
}


void dLexCompiler::DefinitionsMap::AddDefinition (const char* const label, const char* const regularExpression)
{
	ExpandedNFA* const expression = new ExpandedNFA (regularExpression, this);
	Insert(expression, dCRC (label));
}

dLexCompiler::ExpandedNFA* dLexCompiler::DefinitionsMap::FindNDAExpresion (const char* const label) const
{
	dTreeNode* const node = Find (dCRC (label));
	_ASSERTE (node);
	return node->GetInfo();
}

dLexCompiler::ExpandedNFA::ExpandedNFA (const char* const regularExpression, DefinitionsMap* const map)
	:dNonDeterministicFiniteAutonataCompiler()
	,m_map (map)
{
	CompileExpression(regularExpression);
}


void dLexCompiler::ExpandedNFA::PreProcessExpression (const char* const regularExpression)
{
	char buffer[1024];

	_ASSERTE (sizeof (m_regularExpression) > strlen (regularExpression));
	sprintf (m_regularExpression, "%s", regularExpression);

	int i = 0;
	for (int ch = GetChar(); ch; ch = GetChar()) {
		if ((ch>>8) == '\\') {
			switch (char (ch))
			{
				case '~':
				case '!':
				case '@':
				case '#':
				case '$':
				case '%':
				case '^':
				case '&':
				case '*':
				case '(':
				case ')':
				case '-':
				case '+':


				case '{':
				case '}':
				case '|':
				case ':':
				case '"':
				case '<':
				case '>':
				case '?':

				case '`':
				case '_':
				case '=':
				case '[':
				case ']':
				case '\\':
				case ';':
				case '\'':
				case ',':
				case '.':
				case '/':

				case 'n':
				case 'r':
				case 't':
				case 'v':
				case 'f':
					buffer[i] = '\\';
					i ++;
					break;

				case '1':
					if (m_regularExpression[m_regularExpressionIndex] == 'b') {
						GetChar();
						ch = 0x1b;
					}
					break;
				default:;
					ch = char (ch);
			}
		}
		buffer[i] = char (ch);
		i ++;
	}
	buffer[i] = 0;
	m_regularExpressionIndex = 0;
	dNonDeterministicFiniteAutonataCompiler::PreProcessExpression (buffer);
}

bool dLexCompiler::ExpandedNFA::IsOperator (int charater) const
{
	return dNonDeterministicFiniteAutonataCompiler::IsOperator(charater);
/*
	if (dNonDeterministicFiniteAutonataCompiler::IsOperator(charater)) {
		return true;
	}
	if ((charater == '{') || (charater == '}')) {
		return true;
	}
	return false;
*/
}

bool dLexCompiler::ExpandedNFA::CheckInsertConcatenation (int left, int right) const
{
	bool test = (((!IsOperator(left))  || (left == '}') || (left == m_closeParentesis) || (left == m_closeSquareBrakect) || (left == m_zeroOrMore) || (left == m_oneOrMore) || (left == m_zeroOrOne)) && 
				 ((!IsOperator(right)) || (right == '{') || (right == m_openParentesis)  || (right == m_openSquareBrakect))); 
	return test;
}


void dLexCompiler::ExpandedNFA::PushNFA (ExpandedNFA* const nfa, const char* const label)
{
	dTree<dAutomataState*,dAutomataState*> filter;

	label;
	int stack = 1;
	StateConstructPair pool[128];

	pool[0] = StateConstructPair (nfa->m_startState, new dAutomataState(m_stateID));
	m_stateID ++;
	// accepting dAutomataState is the data
	filter.Insert(pool[0].GetAccepting(), pool[0].GetStart());

	while (stack) {
		stack --;
		StateConstructPair statePair = pool[stack];
		_ASSERTE (filter.Find(statePair.GetStart()));

		for (dList<dAutomataState::dTransition>::dListNode* node = statePair.GetStart()->m_transtions.GetFirst(); node; node = node->GetNext()) {
			dAutomataState::dTransition& sourceTransition = node->GetInfo();
			dAutomataState* const targetState = sourceTransition.GetState();

			dTree<dAutomataState*,dAutomataState*>::dTreeNode* const mapMode = filter.Find(targetState);

			dAutomataState::dCharacter ch (sourceTransition.GetCharater());
//			if (ch & (1<<15)) {
			if (ch.m_type == dAutomataState::CHARACTER_SET) {
				const dChatertSetMap::ChatertSet* const charSet = nfa->m_charaterSetMap.FindSet (ch.m_info);
				int newch = m_charaterSetMap.AddSet(charSet->GetSet(), charSet->GetLength());
				ch = dAutomataState::dCharacter (newch, dAutomataState::CHARACTER_SET);
			}

			if (mapMode) {
				dAutomataState* const accepting = mapMode->GetInfo();
				statePair.GetAccepting()->m_transtions.Append(dAutomataState::dTransition(ch, accepting));
			} else {
				pool[stack] = StateConstructPair (targetState, new dAutomataState(m_stateID));
				filter.Insert(pool[stack].GetAccepting(), targetState);
				statePair.GetAccepting()->m_transtions.Append(dAutomataState::dTransition(ch, pool[stack].GetAccepting()));
				stack ++;
				m_stateID ++;
				_ASSERTE (stack < sizeof (pool)/sizeof (pool[0]));
			}
		}
	}

	dAutomataState* const startState = filter.Find(nfa->m_startState)->GetInfo();
	dAutomataState* const acceptingState = filter.Find(nfa->m_acceptingState)->GetInfo();

	m_stack.Push(startState, acceptingState);

//	DTRACE(("Push %s\n", label));
}


void dLexCompiler::ExpandedNFA::ShiftID()
{
	if (m_token == '{') {
		char label[256];
		int i = 0; 
		Match(m_token);
		for (Match(m_token); m_token != '}'; Match(m_token)) {
			label[i] = char (m_token);
			i ++;
			Match(m_token);
		}
		label[i] = 0;
		Match ('}');

		dLexCompiler::ExpandedNFA* const expresion = m_map->FindNDAExpresion (label);
		PushNFA (expresion, label);


	} else {
		dNonDeterministicFiniteAutonataCompiler::ShiftID();
	}
}

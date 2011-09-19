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
#include "dFiniteAutomata.h"

enum dLexCompiler::dToken
{
	m_whiteSpace,
	m_comment,
	m_delimiter,
	m_codeBlock,
	m_intenalSize,
	m_number,
	m_quatedString,
	m_literal,
	m_extendedRegularExpresion,
	m_curlyBrace,
	m_end,
};



class dLexCompiler::dTokenData: public dDeterministicFiniteAutonata
{
	public:
	dTokenData (dToken token, const char* const regulatExpresion)
		:dDeterministicFiniteAutonata (regulatExpresion)
		,m_token(token)
	{
	}

	~dTokenData()
	{

	}
	dToken m_token;
};


dLexCompiler::dTokenDataList::dTokenDataList ()
	:dList<dTokenData*>()
{
}

dLexCompiler::dTokenDataList::~dTokenDataList ()
{
	DeleteAll();
}

void dLexCompiler::dTokenDataList::AddTokenData (dToken token, const char* const regulatExpresion)
{
	dTokenData* const data = new dTokenData (token, regulatExpresion);
	Append (data);
}

void dLexCompiler::dTokenDataList::DeleteAll()
{
	for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
		delete node->GetInfo();
	}
	dList<dTokenData*>::RemoveAll();
}


dLexCompiler::dDefinitionsMap::dDefinitionsMap ()
{

}
dLexCompiler::dDefinitionsMap::~dDefinitionsMap ()
{
}



void dLexCompiler::dDefinitionsMap::PreProcessDefinitions (string& regularExpresionWithMacros)
{
	int i0 = 0;
	for (int i1 = regularExpresionWithMacros.find('{', i0); i1 != -1; i1 = regularExpresionWithMacros.find('{', i0)) {
		i0 = i1;
		int size = regularExpresionWithMacros.size();
		for (i1 ++; (i1 <= size) && isalnum (regularExpresionWithMacros[i1]); i1 ++) {
		} 
		if (i1 < size) {
			string expressionName (&regularExpresionWithMacros[i0 + 1], i1 - i0 - 1);
			dTreeNode* const node = Find(dCRC (expressionName.c_str()));
			if (node) {
				string expression (node->GetInfo());
				regularExpresionWithMacros.replace(i0, i1 - i0 + 1, expression);
			} else {
				i0 ++;
			}
		} else {
			i0 ++;
		}
	}
}

void dLexCompiler::dDefinitionsMap::AddDefinition (string& regularExpresion, string& key)
{
	PreProcessDefinitions (regularExpresion);
	Insert (regularExpresion, dCRC (key.c_str()));
}


class dLexCompiler::dExpandedState: public dAutomataState
{
	public:
	dExpandedState (int id)
		:dAutomataState (id)
		,m_lineNumber(0)
		,m_hasUserAction (false)
		,m_userAction("")
	{
	}	

	~dExpandedState()
	{
	}

	int m_lineNumber;
	bool m_hasUserAction;	
	string m_userAction;
};


class dLexCompiler::dExpandedNFA: public dNonDeterministicFiniteAutonata
{
public:
	dExpandedNFA ()
		:dNonDeterministicFiniteAutonata()
		,m_empty (true), m_lineNumber(0)
	{
	}

	dAutomataState* CreateState (int id)
	{
		dExpandedState* const state =  new dExpandedState (id); 
		state->m_lineNumber = m_lineNumber;
		return state;
	}

	void PushSet (const char* const set, int size)
	{
		dAutomataState* const startState = CreateState (m_stateID ++);
		dAutomataState* const acceptingState = CreateState (m_stateID ++);

//		int setId = m_charaterSetMap.AddSet(set, size);
//		dAutomataState::dCharacter charInfo (setId, dAutomataState::CHARACTER_SET);
//		startState->m_transtions.Append(dAutomataState::dTransition(charInfo, acceptingState));

		for (int i = 0; i < size; i ++) {
			dAutomataState::dCharacter charInfo (GetScapeChar (set[i]), dAutomataState::CHARACTER);
			startState->m_transtions.Append(dAutomataState::dTransition(charInfo, acceptingState));
		}

		m_stack.Push(startState, acceptingState);


//		dNonDeterministicFiniteAutonata::PushSet (set, size);
	}

	void PreProcessExpression (const char* const regularExpression)
	{
		char buffer[D_ASCII_SET_MAX_SIZE];

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
		dNonDeterministicFiniteAutonata::PreProcessExpression (buffer);
	}


	void AddExpression (string& expression, const string& userAction, int lineNumber)
	{
		m_lineNumber = lineNumber;
		if (m_empty) {
			m_empty = false;
			CompileExpression(expression.c_str());

			dExpandedState* const acceptingState = (dExpandedState*) m_acceptingState;
			_ASSERTE (acceptingState->m_exitState);
			acceptingState->m_hasUserAction = true;
			acceptingState->m_userAction = userAction;

		} else {
			dAutomataState* const startState0 = m_startState;
			dAutomataState* const acceptingState0 = m_acceptingState;
			
			CompileExpression(expression.c_str());
			dExpandedState* const acceptingState = (dExpandedState*) m_acceptingState;
			_ASSERTE (acceptingState->m_exitState);
			acceptingState->m_hasUserAction = true;
			acceptingState->m_userAction = userAction;

			dAutomataState* const startState1 = m_startState;
			dAutomataState* const acceptingState1 = m_acceptingState;
			
			m_startState = CreateState (m_stateID ++);
			m_acceptingState = CreateState (m_stateID ++);

			m_startState->m_transtions.Append(dAutomataState::dTransition(dAutomataState::dCharacter(), startState0));
			m_startState->m_transtions.Append(dAutomataState::dTransition(dAutomataState::dCharacter(), startState1));
			acceptingState0->m_transtions.Append(dAutomataState::dTransition(dAutomataState::dCharacter(), m_acceptingState));
			acceptingState1->m_transtions.Append(dAutomataState::dTransition(dAutomataState::dCharacter(), m_acceptingState));

			//DTRACE_NFA(("operator union\n"));
		}
	}

	bool m_empty;
	int m_lineNumber;
};

class dLexCompiler::dExpandedDFA: public dDeterministicFiniteAutonata
{
	public: 
	dExpandedDFA (
		const dNonDeterministicFiniteAutonata& nfa, 
		string& semanticActionCodeOutput, 
		dTree<dTransitionCountStart, int>& transitionsCountMap,
		dList<dTransitionType>& nextStateRun,
		const char* const inputFileName)
		:dDeterministicFiniteAutonata (), m_stateCount(0), m_nfa(&nfa)
	{
		CreateDeterministicFiniteAutomaton (nfa);
		ConvertSwitchCaseStatements (semanticActionCodeOutput, transitionsCountMap, nextStateRun, inputFileName);
	}

	int GetStateCount() const
	{
		return m_stateCount;
	}

	const dChatertSetMap& GetCharacterSetMap() const
	{
		return m_charaterSetMap;
	}

	private:
	dAutomataState* CreateState (int id)
	{
		m_stateCount ++;
		return new dExpandedState (id); 
	}

	dAutomataState* CreateTargetState (dTree<dAutomataState*,dAutomataState*>& subSet, int id)
	{
		dExpandedState* const state = (dExpandedState*) dDeterministicFiniteAutonata::CreateTargetState (subSet, id);

		dExpandedState* userActionState = NULL;
		dTree<dAutomataState*,dAutomataState*>::Iterator iter (subSet);
		for (iter.Begin(); iter; iter ++) {
			dExpandedState* const subSetState = (dExpandedState*) iter.GetNode()->GetInfo();
			if (subSetState->m_hasUserAction) {
				if (!userActionState) {
					userActionState = subSetState;
				} else if (userActionState->m_id < subSetState->m_id) {
					userActionState = subSetState;
				}
			}
		}

		if (userActionState) {
			state->m_hasUserAction = true;
			state->m_lineNumber = userActionState->m_lineNumber;
			state->m_userAction = userActionState->m_userAction;
		}

		return state;
	}

	void AddText (string& output, const char* const fmt, ...) const
	{
		va_list v_args;
		char text[2048];

		text[0] = '\t';
		text[1] = '\t';
		text[2] = '\t';
		text[3] = 0;
		va_start (v_args, fmt);     
		vsprintf(&text[3], fmt, v_args);
		va_end (v_args);            

		output += text;
	}


	void ConvertSwitchCaseStatements (
		string& semanticActionCodeOutput, 
		dTree<dTransitionCountStart, int>& transitionsCountMap,
		dList<dTransitionType>& nextStateRun,
		const char* const inputFileName) const 
	{
		dTree<dAutomataState*,dAutomataState*> filter;

		int stack = 1;
		dAutomataState* pool[128];

		string fileName (string ("\"") + string(inputFileName) + string ("\""));

		pool[0] = m_startState;
		filter.Insert(pool[0], pool[0]);

		int startIndex = 0;
		while (stack) {
			stack --;

			dExpandedState* const state = (dExpandedState*) pool[stack];
			_ASSERTE (filter.Find(state));

			char condition[128];
			condition[0] = 0;

			if (!transitionsCountMap.Find(state->m_id)) {
				dTransitionCountStart countStartEntry;
				countStartEntry.m_start = 0;
				countStartEntry.m_count = 0;
				transitionsCountMap.Insert(countStartEntry, state->m_id);
			}

			if (state->m_exitState) {
				AddText (semanticActionCodeOutput, "case %d:\n", state->m_id);
				AddText (semanticActionCodeOutput, "{\n");
				if (!state->m_exitState || state->m_transtions.GetCount()) {
					AddText (semanticActionCodeOutput, "\tchar ch = NextChar();\n");
				}

				for (dList<dAutomataState::dTransition>::dListNode* node = state->m_transtions.GetFirst(); node; node = node->GetNext()) {
					dAutomataState::dTransition& sourceTransition = node->GetInfo();
					dAutomataState* const targetState = sourceTransition.GetState();

					if (!filter.Find(targetState)) {
						pool[stack] = targetState;
						filter.Insert(targetState, targetState);
						stack ++;
						_ASSERTE (stack < sizeof (pool)/sizeof (pool[0]));
					}

					dAutomataState::dCharacter ch (sourceTransition.GetCharater());
					if (ch.m_type == dAutomataState::CHARACTER_SET) {
						AddText (semanticActionCodeOutput, "\t%sif (IsCharInSet (ch, text_%d, characterSetSize[%d])) state = %d;\n", condition, ch.m_info, ch.m_info, targetState->m_id);
					} else if (ch.m_type == dAutomataState::CHARACTER) {
						AddText (semanticActionCodeOutput, "\t%sif (ch == %d) state = %d;\n", condition, GetScapeChar(ch.m_info), targetState->m_id);
					} else {
						_ASSERTE (0);
					}

					sprintf (condition, "else ");
				}

				AddText (semanticActionCodeOutput, "\t%s{\n", condition);

				if (state->m_transtions.GetCount()) {
					AddText (semanticActionCodeOutput, "\t\tUnGetChar();\n");
				}
				AddText (semanticActionCodeOutput, "\t\tGetLexString ();\n");

				AddText (semanticActionCodeOutput, "\t\t//user specified action\n");
				//AddText (semanticActionCodeOutput, "\t\tmatchFound = true;\n");
				AddText (semanticActionCodeOutput, "\t\t%s\n", state->m_userAction.c_str());
				AddText (semanticActionCodeOutput, "\t\tstate = 0;\n");
				//AddText (semanticActionCodeOutput, "\t\tmatchFound = false;\n");

				AddText (semanticActionCodeOutput, "\t}\n");

				AddText (semanticActionCodeOutput, "\tbreak;\n");

				AddText (semanticActionCodeOutput, "}\n");

			} else {

				dTransitionCountStart& countStartEntry = transitionsCountMap.Find(state->m_id)->GetInfo();
				countStartEntry.m_start = startIndex;
				countStartEntry.m_count = state->m_transtions.GetCount();
				startIndex += state->m_transtions.GetCount();
				transitionsCountMap.Insert(countStartEntry, state->m_id);

				for (dList<dAutomataState::dTransition>::dListNode* node = state->m_transtions.GetFirst(); node; node = node->GetNext()) {
					dAutomataState::dTransition& sourceTransition = node->GetInfo();
					dAutomataState* const targetState = sourceTransition.GetState();

					dLexCompiler::dTransitionType& transitionType = nextStateRun.Append()->GetInfo();
					transitionType.m_nextState = targetState->m_id;

					if (!filter.Find(targetState)) {
						pool[stack] = targetState;
						filter.Insert(targetState, targetState);
						stack ++;
						_ASSERTE (stack < sizeof (pool)/sizeof (pool[0]));
					}

					dAutomataState::dCharacter ch (sourceTransition.GetCharater());
					if (ch.m_type == dAutomataState::CHARACTER_SET) {
						const dChatertSetMap::ChatertSet* const charSet = m_charaterSetMap.FindSet (ch.m_info);
						_ASSERTE (charSet);
						transitionType.m_info = char(ch.m_info);
						transitionType.m_infoType = 1;
					} else if (ch.m_type == dAutomataState::CHARACTER) {
						transitionType.m_info = char(GetScapeChar(ch.m_info));
						transitionType.m_infoType = 0;
					} else {
						_ASSERTE (0);
					}
				}
			}
		}
	}

	int m_stateCount;
	const dNonDeterministicFiniteAutonata* m_nfa;
};


dLexCompiler::dLexCompiler(const char* const inputRules, const char* const outputFileName, const char* const inputFileName)
	:m_token (m_end)
	,m_lineNumber(1)
	,m_grammarTokenStart(0)
	,m_grammarTokenLength(0)
	,m_grammar (inputRules)
	,m_tokenList()
{
	string semanticActionCode ("");
	string userPreHeaderCode (""); 
	string userPostHeaderCode ("\n"); 
	dList<dTransitionType> nextStateRun;
	dTree<dTransitionCountStart, int> transitionsCountStartMap;

	// convert specification file into one single giant non deterministic finite automaton
	dExpandedNFA nfa;
	ParseDefinitions (nfa, userPreHeaderCode, userPostHeaderCode);

	// convert nfa to Deterministic Finite Automaton
	dExpandedDFA dfa (nfa, semanticActionCode, transitionsCountStartMap, nextStateRun, inputFileName);
	_ASSERTE (dfa.GetCharacterSetMap().GetSets().GetCount() == 0);

	// save header and source files
	string className (GetClassName(outputFileName));
	CreateHeaderFile (outputFileName, className);
	CreateCodeFile (outputFileName, className, dfa.GetStateCount(), userPreHeaderCode, userPostHeaderCode, semanticActionCode, transitionsCountStartMap, nextStateRun); 
	
}

dLexCompiler::~dLexCompiler()
{
}

string dLexCompiler::GetClassName(const char* const fileName) const
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




void dLexCompiler::NextToken ()
{
	m_grammarTokenStart += m_grammarTokenLength;
	
	if (m_grammar[m_grammarTokenStart]) {
		int lineNumber = m_lineNumber;
		for (bool reStart = true; reStart;) {
			reStart = false;
			for (dTokenDataList::dListNode* node = m_tokenList.GetFirst(); node; node = node->GetNext()) {
				dTokenData* const dTokenData = node->GetInfo();
				const char* const text = &m_grammar[m_grammarTokenStart];
				int count = dTokenData->FindMatch (text);
				if (count >= 0) {
					const char* const textstring = &m_grammar[m_grammarTokenStart];
					for (int i = 0; i < count ; i ++) {
						if (textstring[i] == '\n') {
							lineNumber ++;
						}
					}

					m_grammarTokenLength = count;
					if ((dTokenData->m_token == m_whiteSpace) || (dTokenData->m_token == m_comment)) {
						m_grammarTokenStart += m_grammarTokenLength;
						reStart = true;
						break;
					} else {
						m_lineNumber = lineNumber;
						m_token = dTokenData->m_token;
						return;
					}
				} 
			}
		}
	}
	m_token = m_end;
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
		_ASSERTE (0);
//		MatchToken (m_token);

	} else if (m_token == m_codeBlock) {
		string code (&m_grammar[m_grammarTokenStart] + 2, m_grammarTokenLength - 4) ;
		preheaderCode.append(code);
		MatchToken (m_token);

	} else if (m_token == m_intenalSize) {
		_ASSERTE (0);
		MatchToken (m_token);
		MatchToken (m_token);

	} else if (m_token == m_literal) {
		string literal (&m_grammar[m_grammarTokenStart], m_grammarTokenLength);
		MatchToken (m_token);
		
		string extendedRegularExpresion (&m_grammar[m_grammarTokenStart], m_grammarTokenLength);
		MatchToken (m_token);

		m_defintions.AddDefinition(extendedRegularExpresion, literal);

	} else {
		_ASSERTE (0);
	}
}


void dLexCompiler::MatchToken (dToken token)
{
	if (m_token == token) {
		NextToken ();
	} else {
		_ASSERTE (0);
		DTRACE(("parse error\n"));
		//		m_error = true;
	}
}



void dLexCompiler::LoadTemplateFile(const char* const templateName, string& templateOuput) const
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

void dLexCompiler::SaveFile(const char* const fileName, const char* const extention, const string& input) const
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

void dLexCompiler::ReplaceMacro (string& data, const string& newName, const string& macro) const
{
	int size = macro.size();
	int position = data.find (macro);
	data.replace(position, size, newName);
}


void dLexCompiler::ReplaceAllMacros (string& data, const string& newName, const string& macro) const
{
	int size = macro.size();
	for (size_t i = data.find (macro); i != -1; i = data.find (macro)) {
		data.replace(i, size, newName);
	}
}




// DefinitionExpression	: DefinitionBlock 
// DefinitionExpression	: DefinitionBlock DefinitionExpression | nothing
// DefinitionBlock		: m_comment
// id					: m_openSquareBrakect BracketedExpression m_closeSquareBrakect
// id					: m_openParentesis DefinitionExpression m_closeParentesis
// id					: .
// id					: CHARACTER 
void dLexCompiler::ParseDefinitions (dExpandedNFA& nfa, string& preHeaderCode, string& postHeaderCode) 
{
	// parse definitions
	{
		m_tokenList.AddTokenData (m_whiteSpace, "[ \n\r\t\v\f]+");
		m_tokenList.AddTokenData (m_comment, "(/\\*([^*]|[\r\n]|(\\*+([^*/]|[\r\n])))*\\*+/)|(//.*)");
		m_tokenList.AddTokenData (m_codeBlock, "%\\{([^%]|[\r\n]|(%+([^%}]|[\r\n])))*%+\\}");
		m_tokenList.AddTokenData (m_literal, "[a-zA-Z_][0-9a-zA-Z]*");
		m_tokenList.AddTokenData (m_number, "[0-9]+");
		m_tokenList.AddTokenData (m_intenalSize, "%[pneako]");
		m_tokenList.AddTokenData (m_delimiter, "%%");
		m_tokenList.AddTokenData (m_extendedRegularExpresion, "((\\[[^\\]]+\\])|[^ \n\r\t\v\f[]+)+");

		for (NextToken(); (m_token != m_end) && (m_token != m_delimiter);) {
			ParseDefinitionExpression (preHeaderCode);
		}
		m_tokenList.DeleteAll();
	}

	// parse rules

	//	int initialState = 0;
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

		m_tokenList.AddTokenData (m_whiteSpace, "[ \n\r\t\v\f]+");
		m_tokenList.AddTokenData (m_quatedString, "\"[^\" \t\v\n\f]*\"");
		m_tokenList.AddTokenData (m_delimiter, "%%");
		m_tokenList.AddTokenData (m_comment, "(/\\*([^*]|[\r\n]|(\\*+([^*/]|[\r\n])))*\\*+/)|(//.*)");
		m_tokenList.AddTokenData (m_extendedRegularExpresion, "((\\[[^\\]]+\\])|[^ \n\r\t\v\f[]+)+");

		
		for (NextToken(); (m_token != m_end) && (m_token != m_delimiter); ) {

			int lineNumber = m_lineNumber;
			string expression (&m_grammar[m_grammarTokenStart], m_grammarTokenLength);
			m_defintions.PreProcessDefinitions(expression);
			dToken expresionToken (m_token);

			// until I get the balance expression feature working
			m_grammarTokenStart += m_grammarTokenLength;
			const char* const str = &m_grammar[m_grammarTokenStart];
			int length = 0;
			for (int ch = str[length]; ch && (ch != '{') ; ch = str[length]) {
				if (ch == '\n') {
					m_lineNumber ++;
				}
				length ++;
			}

			if (str[length] == '{') {

				m_grammarTokenStart += length;
				const char* const str = &m_grammar[m_grammarTokenStart];
				int length = 1;

				int count = 1;
				while (count) {
					char ch = str[length++];
					if (ch == '\n') {
						m_lineNumber ++;
					}

					if(ch == '{') {
						count ++;
					} else if (ch == '}') {
						count --;
					} else {
						if (ch == '\'')	{
							ch = str[length++];;
							if (ch == '\\') {
								ch = str[length++];
							}
							ch = str[length++];;
						} else if (ch == '\"') {
							for (ch = str[length++]; ch != '\"'; ch = str[length++]) {
								if (ch == '\\') {
									ch = str[length++];;
								}
							}
						}
					}
				}

				string userAction (str, length);
				switch (expresionToken) 
				{
					case m_quatedString:
					{
						string keyword (expression.substr(1, expression.length() - 2));
						nfa.AddExpression(keyword, userAction, lineNumber);
						break;
					}

					case m_extendedRegularExpresion:
					{
						nfa.AddExpression(expression, userAction, lineNumber);
						//DTRACE ((semanticActionCode.c_str()));

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

	if (m_token == m_delimiter) {
		postHeaderCode = &m_grammar[m_grammarTokenStart + m_grammarTokenLength];
		postHeaderCode += "\n";
	}
}



void dLexCompiler::CreateHeaderFile (const char* const fileName, const string& className) const
{
	string templateHeader ("");
	LoadTemplateFile("/dLexicalTemplate.h", templateHeader);

	ReplaceAllMacros (templateHeader, className, "$(className)");

	SaveFile(fileName, ".h", templateHeader);
}


void dLexCompiler::CreateCodeFile (
	const char* const fileName, 
	const string& className,
	int stateCount,
	const string& userPreHeaderCode, 
	const string& userPostHeaderCode, 
	const string& semanticActionCode,
//	const dChatertSetMap& characterSet,
	dTree<dTransitionCountStart, int>& transitionsCountStartMap,
	dList<dTransitionType>& nextStateRun) const
{
	string templateHeader ("");
	LoadTemplateFile("/dLexicalTemplate.cpp", templateHeader);

	ReplaceMacro (templateHeader, userPreHeaderCode, "$(userIncludeCode)");
	ReplaceAllMacros (templateHeader, className, "$(className)");

/*
	if (characterSet.GetSets().GetCount()) {
		string characterSets ("");
		string characterSetList ("");
		string characterSetSize ("");
		dTree<dList <dChatertSetMap::ChatertSet>::dListNode*, int>::Iterator iter (characterSet.GetSets());
		for (iter.Begin(); iter; iter ++) {
			dChatertSetMap::ChatertSet& set = iter.GetNode()->GetInfo()->GetInfo();
			int count = set.GetLength();

			char name[256];

			sprintf (name, "text_%d, ", set.GetId());		
			characterSetList += name;

			sprintf (name, "%d, ", count);		
			characterSetSize += name;

			sprintf (name, "\tstatic char text_%d[] = {", set.GetId());
			characterSets += name;
			const char* const str = set.GetSet();
			for (int i = 0; i < count; i ++) {
				char tmp[128];
				sprintf (tmp, "%d, ", str[i]);
				characterSets += tmp;
			}
			characterSets += "0};\n";
		}
		ReplaceMacro (templateHeader, characterSets, "$(characterSets)");

		characterSetList.replace(characterSetList.size()-2, 2, "");
		ReplaceMacro (templateHeader, characterSetList, "$(characterSetArray)");

		characterSetSize.replace(characterSetSize.size()-2, 2, "");
		ReplaceMacro (templateHeader, characterSetSize, "$(characterSetSize)");
	} else {
		ReplaceMacro (templateHeader, "", "$(characterSets)");
		ReplaceMacro (templateHeader, "0", "$(characterSetArray)");
		ReplaceMacro (templateHeader, "0", "$(characterSetSize)");
	}
*/

	for (int i = 0; i < stateCount; i ++) {
		if (!transitionsCountStartMap.Find(i)) {
			dTransitionCountStart entry;
			entry.m_count = 0;
			entry.m_start = 0;
			transitionsCountStartMap.Insert(entry, i);
		}
	}

	string transitionsCountString ("");
	string transitionsStartString ("");
	dTree<dTransitionCountStart, int>::Iterator iter1 (transitionsCountStartMap); 
	for (iter1.Begin(); iter1; iter1 ++) {
		char text[256];
		dTransitionCountStart entry = iter1.GetNode()->GetInfo();
		sprintf (text, "%d, ", entry.m_count);
		transitionsCountString += text;

		sprintf (text, "%d, ", entry.m_start);
		transitionsStartString += text;
	}

/*
	transitionsCountString += "0";
	ReplaceMacro (templateHeader, transitionsCountString, "$(transitionsCount)");

	transitionsStartString += "0";
	ReplaceMacro (templateHeader, transitionsStartString, "$(transitionsStart)");

	string nextStateRunString ("");
	for (dList<dTransitionType>::dListNode* node = nextStateRun.GetFirst(); node; node = node->GetNext()) {
		char text[256];
		dTransitionType value (node->GetInfo());
		sprintf (text, "0x0%x, ", value.m_value);
		nextStateRunString += text;
	}
	nextStateRunString += "0";
	ReplaceMacro (templateHeader, nextStateRunString, "$(nextTranstionList)");

	ReplaceMacro (templateHeader, semanticActionCode, "$(semanticActionCode)");

	//	templateHeader = templateHeader + userPostHeaderCode;
	templateHeader = templateHeader + userPostHeaderCode;
*/
	SaveFile(fileName, ".cpp", templateHeader);
}


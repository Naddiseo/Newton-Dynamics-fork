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
	m_action,
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
		:dAutomataState (id), m_hasUserAction (false), m_userAction("")
	{
	}	
	bool m_hasUserAction;	
	string m_userAction;
};


class dLexCompiler::dExpandedNFA: public dNonDeterministicFiniteAutonata
{
public:
	dExpandedNFA ()
		:dNonDeterministicFiniteAutonata()
		,m_empty (true)
	{
	}

	dAutomataState* CreateState (int id)
	{
		return new dExpandedState (id); 
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


	void AddExpression (string& expression, const string& userAction)
	{
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
};

class dLexCompiler::dExpandedDFA: public dDeterministicFiniteAutonata
{
	public: 
		dExpandedDFA (
			const dNonDeterministicFiniteAutonata& nfa, 
			string& automataCodeOutput, 
			dTree<dTransitionCountStart, int>& transitionsCountMap,
			dList<dTransitionType>& nextStateRun)
		:dDeterministicFiniteAutonata (), m_stateCount(0), m_nfa(&nfa)
	{
		CreateDeterministicFiniteAutomaton (nfa);
		ConvertSwitchCaseStatements (automataCodeOutput, transitionsCountMap, nextStateRun);
	}

	int GetStateCount() const
	{
		return m_stateCount;
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
			state->m_userAction = state->m_userAction;
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
		string& automataCodeOutput, 
		dTree<dTransitionCountStart, int>& transitionsCountMap,
		dList<dTransitionType>& nextStateRun) const 
//		int stateIdBase, 
//		dChatertSetMap& characterSet, 
//		const string& userAction,
		
//		dTree<dTree<int, int>, int>& nextStateMap,
//		dTree<dTree<char, int>, int>& characterTestMap,
//		dTree<dTree<int, int>, int>& testSetArrayIndexMap)
	{
#if 0
		dTree<dAutomataState*,dAutomataState*> filter;

		int stack = 1;
		dAutomataState* pool[128];

		pool[0] = m_deterministicFiniteAutomata;
		filter.Insert(pool[0], pool[0]);

		int lastState = m_deterministicFiniteAutomata->m_id;

		while (stack) {
			stack --;

			dAutomataState* const state = pool[stack];
			_ASSERTE (filter.Find(state));

			AddText (parseTokenOutput, "case %d:\n", state->m_id + stateIdBase);
			AddText (parseTokenOutput, "{\n");

			if (!state->m_exitState || state->m_transtions.GetCount()) {
				AddText (parseTokenOutput, "\tchar ch = NextChar();\n");
			}

			if ((state->m_id) > lastState) {
				lastState = state->m_id;
			}

			char condition[126];
			condition[0] = 0;
			for (dList<dAutomataState::dTransition>::dListNode* node = state->m_transtions.GetFirst(); node; node = node->GetNext()) {
				dAutomataState::dTransition& sourceTransition = node->GetInfo();

				dAutomataState* const targetState = sourceTransition.GetState();
				dTree<dAutomataState*,dAutomataState*>::dTreeNode* const mapMode = filter.Find(targetState);

				int ch = sourceTransition.GetCharater();

				if (mapMode) {
					if (ch & (1<<15)) {
						const dChatertSetMap::ChatertSet* const charSet = m_charaterSetMap.FindSet (ch);
						_ASSERTE (charSet);
						ch = characterSet.AddSet (charSet->GetSet(), charSet->GetLength());
						AddText (parseTokenOutput, "\t%sif (IsCharInSet (ch, text_%d)) m_state = %d;\n", condition, ch & 0x7fff, targetState->m_id + stateIdBase);
					} else {
						AddText (parseTokenOutput, "\t%sif (ch == %d) m_state = %d;\n", condition, GetScapeChar(ch), targetState->m_id + stateIdBase);
					}

				} else {
					pool[stack] = targetState;
					filter.Insert(targetState, targetState);
					stack ++;
					_ASSERTE (stack < sizeof (pool)/sizeof (pool[0]));

					if (ch & (1<<15)) {
						const dChatertSetMap::ChatertSet* const charSet = m_charaterSetMap.FindSet (ch);
						_ASSERTE (charSet);
						ch = characterSet.AddSet (charSet->GetSet(), charSet->GetLength());

						AddText (parseTokenOutput, "\t%sif (IsCharInSet (ch, text_%d)) m_state = %d;\n", condition, ch & 0x7fff, targetState->m_id + stateIdBase);

					} else {
						AddText (parseTokenOutput, "\t%sif (ch == %d) m_state = %d;\n", condition, GetScapeChar(ch), targetState->m_id + stateIdBase);
					}
				}
				sprintf (condition, "else ");
			}

			if (state->m_exitState) {
				// this is the exit;

				AddText (parseTokenOutput, "\t%s{\n", condition);

				//AddText (parseTokenOutput, "\t\tm_state = %d;\n", condition, state->m_id + stateIdBase);
				if (state->m_transtions.GetCount()) {
					AddText (parseTokenOutput, "\t\tm_index --;\n");
				}
				AddText (parseTokenOutput, "\t\tGetLexString ();\n");

				//AddText (parseTokenOutput, "\t\t{\n");
				AddText (parseTokenOutput, "\t\t//user specified action\n");
				AddText (parseTokenOutput, "\t\tm_state = 0;\n");
				AddText (parseTokenOutput, "\t\tm_startState = 0;\n");
				AddText (parseTokenOutput, "\t\t%s\n", action.c_str());

				//AddText (parseTokenOutput, "\t\t}\n");



				AddText (parseTokenOutput, "\t}\n");

			} else {
				AddText (parseTokenOutput, "\t%sm_state = NextPattern();\n", condition);
			}

			AddText (parseTokenOutput, "\tbreak;\n");

			AddText (parseTokenOutput, "}\n");
		}

		//	AddText (nextTokenOutput, "case %d: m_startState = %d; break;\n", m_deterministicFiniteAutomata->m_id + stateIdBase, lastState + stateIdBase + 1);
		char nextJump[256];
		static string filler ("0, ");
		sprintf (nextJump, "%d, ", lastState + stateIdBase + 1);
		nextTokenOutput += nextJump;
		for (int i = 0; i < lastState; i ++) {
			nextTokenOutput += filler;
		}


		//	DTRACE ((parseTokenOutput.c_str()));
		return stateIdBase + lastState + 1;

#endif
automataCodeOutput = "int x = 0;";

		dTree<dAutomataState*,dAutomataState*> filter;

		int stack = 1;
		dAutomataState* pool[128];

		pool[0] = m_startState;
		filter.Insert(pool[0], pool[0]);

//		int lastState = m_deterministicFiniteAutomata->m_id;

		int startIndex = 0;
		while (stack) {
			stack --;

			dAutomataState* const state = pool[stack];
			_ASSERTE (filter.Find(state));

//			if ((state->m_id) > lastState) {
//				lastState = state->m_id;
//			}

			char condition[126];
			condition[0] = 0;

			if (state->m_exitState) {
				_ASSERTE (0);
/*
				AddText (parseTokenOutput, "case %d:\n", state->m_id + stateIdBase);
				AddText (parseTokenOutput, "{\n");
				if (!state->m_exitState || state->m_transtions.GetCount()) {
					AddText (parseTokenOutput, "\tchar ch = NextChar();\n");
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
					//				if (ch & (1<<15)) {
					if (ch.m_type == dAutomataState::CHARACTER_SET) {
						const dChatertSetMap::ChatertSet* const charSet = m_charaterSetMap.FindSet (ch.m_info);
						_ASSERTE (charSet);
						int ch = characterSet.AddSet (charSet->GetSet(), charSet->GetLength());
						AddText (parseTokenOutput, "\t%sif (IsCharInSet (ch, text_%d)) m_state = %d;\n", condition, ch, targetState->m_id + stateIdBase);

					} else if (ch.m_type == dAutomataState::CHARACTER) {
						AddText (parseTokenOutput, "\t%sif (ch == %d) m_state = %d;\n", condition, GetScapeChar(ch.m_info), targetState->m_id + stateIdBase);
					} else {
						_ASSERTE (0);
					}


					sprintf (condition, "else ");
				}

				AddText (parseTokenOutput, "\t%s{\n", condition);

				//AddText (parseTokenOutput, "\t\tm_state = %d;\n", condition, state->m_id + stateIdBase);
				if (state->m_transtions.GetCount()) {
					AddText (parseTokenOutput, "\t\tm_index --;\n");
				}
				AddText (parseTokenOutput, "\t\tGetLexString ();\n");

				//AddText (parseTokenOutput, "\t\t{\n");
				AddText (parseTokenOutput, "\t\t//user specified action\n");
				AddText (parseTokenOutput, "\t\tm_state = 0;\n");
				AddText (parseTokenOutput, "\t\tm_startState = 0;\n");
				AddText (parseTokenOutput, "\t\t%s\n", userAction.c_str());

				//AddText (parseTokenOutput, "\t\t}\n");

				AddText (parseTokenOutput, "\t}\n");

				AddText (parseTokenOutput, "\tbreak;\n");

				AddText (parseTokenOutput, "}\n");
*/
			} else {

				dTransitionCountStart countStartEntry;
				countStartEntry.m_start = startIndex;
				countStartEntry.m_count = state->m_transtions.GetCount();
				startIndex += state->m_transtions.GetCount();
				transitionsCountMap.Insert(countStartEntry, state->m_id);

//				for (int i = 0; i < state->m_transtions.GetCount(); i ++) {
//					if (!nextStateMap.Find(i)) {
//						nextStateMap.Insert(i);
//						characterTestMap.Insert(i);
//						testSetArrayIndexMap.Insert(i);
//					}
//				}

//				int transitionIndex = 0;
				for (dList<dAutomataState::dTransition>::dListNode* node = state->m_transtions.GetFirst(); node; node = node->GetNext()) {

//					dTree<int, int>& nextState = nextStateMap.Find(transitionIndex)->GetInfo();
//					dTree<char, int>& charatestTest = characterTestMap.Find(transitionIndex)->GetInfo();
//					dTree<int, int>& testSetArrayIndex = testSetArrayIndexMap.Find(transitionIndex)->GetInfo();

					dAutomataState::dTransition& sourceTransition = node->GetInfo();
					dAutomataState* const targetState = sourceTransition.GetState();

					dLexCompiler::dTransitionType& transitionType = nextStateRun.Append()->GetInfo();
					transitionType.m_targetState = targetState->m_id;

//					nextState.Insert(targetState->m_id + stateIdBase, state->m_id + stateIdBase);

					if (!filter.Find(targetState)) {
						pool[stack] = targetState;
						filter.Insert(targetState, targetState);
						stack ++;
						_ASSERTE (stack < sizeof (pool)/sizeof (pool[0]));
					}

					//int ch = sourceTransition.GetCharater();
					dAutomataState::dCharacter ch (sourceTransition.GetCharater());
					//if (ch & (1<<15)) {
					if (ch.m_type == dAutomataState::CHARACTER_SET) {
						_ASSERTE (0);
						const dChatertSetMap::ChatertSet* const charSet = m_charaterSetMap.FindSet (ch.m_info);
						_ASSERTE (charSet);
//						int ch = characterSet.AddSet (charSet->GetSet(), charSet->GetLength());
//						testSetArrayIndex.Insert(ch, state->m_id + stateIdBase);
					} else if (ch.m_type == dAutomataState::CHARACTER) {
//						charatestTest.Insert(char(GetScapeChar(ch.m_info)), state->m_id + stateIdBase);
						transitionType.m_value = char(GetScapeChar(ch.m_info));
						transitionType.m_transitionType = 0;
					} else {
						_ASSERTE (0);
					}
//					transitionIndex ++;
				}
			}
		}

//		char nextJump[256];
//		static string filler ("0, ");
//		sprintf (nextJump, "%d, ", lastState + stateIdBase + 1);
//		nextTokenOutput += nextJump;
//		for (int i = 0; i < lastState; i ++) {
//			nextTokenOutput += filler;
//		}
//		return stateIdBase + lastState + 1;
	}

	int m_stateCount;
	const dNonDeterministicFiniteAutonata* m_nfa;
};





dLexCompiler::dLexCompiler(const char* const inputRules, const char* const outputFileName)
	:m_token (m_end)
	,m_grammarTokenStart(0)
	,m_grammarTokenLength(0)
	,m_grammar (inputRules)
	,m_tokenList()
{
	string automataCode ("");
//	string nextCodeCases ("");
	string userPreheaderCode (""); 
	dChatertSetMap characterSet;
	dList<dTransitionType> nextStateRun;
	dTree<dTransitionCountStart, int> transitionsCountStartMap;
//	dTree<dTree<int, int>, int> nextState;
//	dTree<dTree<char, int>, int> characterTestMap;
//	dTree<dTree<int, int>, int> testSetArrayIndexMap;

	int stateCount = ParseDefinitions (userPreheaderCode, automataCode, transitionsCountStartMap, nextStateRun);

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
//	CreateCodeFile (cppFileName, className, stateCount, userPreheaderCode, nextCodeCases, automataCode, characterSet, 
//					transitionsCountMap, nextState, characterTestMap, testSetArrayIndexMap);
	CreateCodeFile (cppFileName, className, stateCount, userPreheaderCode, characterSet, transitionsCountStartMap, nextStateRun); 

	*dot = 0;
	strcat (cppFileName, ".h");
	CreateHeaderFile (cppFileName, className);

}

dLexCompiler::~dLexCompiler()
{
}



void dLexCompiler::NextToken ()
{
	m_grammarTokenStart += m_grammarTokenLength;
	if (m_grammar[m_grammarTokenStart]) {

		for (bool reStart = true; reStart;) {
			reStart = false;
			for (dTokenDataList::dListNode* node = m_tokenList.GetFirst(); node; node = node->GetNext()) {
				dTokenData* const dTokenData = node->GetInfo();

				const char* const text = &m_grammar[m_grammarTokenStart];
				int count = dTokenData->FindMatch (text);
				if (count >= 0) {
					m_grammarTokenLength = count;
					if ((dTokenData->m_token == m_whiteSpace) || (dTokenData->m_token == m_comment)) {
						m_grammarTokenStart += m_grammarTokenLength;
						reStart = true;
						break;
					} else {
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

// DefinitionBlock		: m_comment

//
// id							: m_openSquareBrakect BracketedExpression m_closeSquareBrakect
// id							: m_openParentesis DefinitionExpression m_closeParentesis
// id							: .
// id							: CHARACTER 
int dLexCompiler::ParseDefinitions (
	string& preheaderCode, 
	string& automataCode, 
	dTree<dTransitionCountStart, int>& transitionsCountMap,
	dList<dTransitionType>& nextStateRun)
//	string& nextTokeCode, 
//	string& automataCode, 
//	dChatertSetMap& characterSet,
//	dTree<dTree<int, int>, int>& nextState,
//	dTree<dTree<char, int>, int>& characterTestMap,
//	dTree<dTree<int, int>, int>& testSetArrayIndexMap)
{
	// parse definitions
	{
//m_tokenList.AddTokenData (m_codeBlock, "%(:{([^{}]*):})%");
//m_tokenList.AddTokenData (m_codeBlock, "(:{(([^{}\"\']*)|(\'(.|[\n])\')|(\"(\\.|[^\"])*\"))*:})");
//m_tokenList.AddTokenData (m_codeBlock, "%(:{(([^{}\"])|(\"[^\"]*\"))*:})%");


		m_tokenList.AddTokenData (m_whiteSpace, "[ \t\v\n\f]+");
		m_tokenList.AddTokenData (m_comment, "(/\\*([^*]|[\r\n]|(\\*+([^*/]|[\r\n])))*\\*+/)|(//.*)");
		m_tokenList.AddTokenData (m_codeBlock, "%\\{([^%]|[\r\n]|(%+([^%}]|[\r\n])))*%+\\}");
		//m_tokenList.AddTokenData (m_codeBlock, "%(:{(([^{}\"\']*)|(\'(.|[\n])\')|(\"(\\.|[^\"])*\"))*:})%");
		m_tokenList.AddTokenData (m_literal, "[a-zA-Z_][0-9a-zA-Z]*");
		m_tokenList.AddTokenData (m_number, "[0-9]+");
		m_tokenList.AddTokenData (m_intenalSize, "%[pneako]");
		m_tokenList.AddTokenData (m_delimiter, "%%");
		m_tokenList.AddTokenData (m_extendedRegularExpresion, "((\\[[^\\]]+\\])|[^ \t\v\n\f[]+)+");

		for (NextToken(); (m_token != m_end) && (m_token != m_delimiter);) {
			ParseDefinitionExpression (preheaderCode);
		}
		m_tokenList.DeleteAll();
	}

	// parse rules

	dExpandedNFA nfa;
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

		m_tokenList.AddTokenData (m_whiteSpace, "[ \t\v\n\f]+");
		m_tokenList.AddTokenData (m_quatedString, "\"[^\" \t\v\n\f]*\"");
		m_tokenList.AddTokenData (m_delimiter, "%%");
		m_tokenList.AddTokenData (m_comment, "(/\\*([^*]|[\r\n]|(\\*+([^*/]|[\r\n])))*\\*+/)|(//.*)");
		m_tokenList.AddTokenData (m_extendedRegularExpresion, "((\\[[^\\]]+\\])|[^ \t\v\n\f[]+)+");

		for (NextToken(); (m_token != m_end) && (m_token != m_delimiter); ) {

			string expression (&m_grammar[m_grammarTokenStart], m_grammarTokenLength);
			m_defintions.PreProcessDefinitions(expression);
			dToken expresionToken (m_token);

			// until I get the balance expression feature working
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
					}
				}
				string userAction (str, length);

//				automataCode += "\t\t\t// ";
//				automataCode += expression;
//				automataCode += "\n";

				switch (expresionToken) 
				{
					case m_quatedString:
					{
						string keyword (expression.substr(1, expression.length() - 2));

						nfa.AddExpression(expression, userAction);
/*
						dDeterministicFiniteAutonata keywordAutomata (keyword.c_str());
						initialState = keywordAutomata.ConvertSwitchCaseStatements (automataCode, nextTokeCode, initialState, characterSet, userAction, 
							transitionsCountMap, nextState, characterTestMap, testSetArrayIndexMap);
							//DTRACE ((automataCode.c_str()));
*/
						break;
					}

					case m_extendedRegularExpresion:
					{
						//m_defintions
						//ExpandedNFA expresionNFA (expression.c_str(), &m_defintions);
						//dDeterministicFiniteAutonata expresionAutomata (expresionNFA);
						//initialState = expresionAutomata.ConvertSwitchCaseStatements (automataCode, nextTokeCode, initialState, characterSet, userAction, 
						//															  transitionsCountMap, nextState, characterTestMap, testSetArrayIndexMap);
						nfa.AddExpression(expression, userAction);
							//DTRACE ((automataCode.c_str()));

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

	dExpandedDFA expresionAutomata (nfa, automataCode, transitionsCountMap, nextStateRun);

	return expresionAutomata.GetStateCount();


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
		preheaderCode.append(&m_grammar[m_grammarTokenStart] + 2, m_grammarTokenLength - 4);
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



void dLexCompiler::CreateCodeFile (
	const char* const fileName, 
	const char* const className,
	int stateCount,
	string& userPreheaderCode, 
	dChatertSetMap& characterSet,
	dTree<dTransitionCountStart, int>& transitionsCountStartMap,
	dList<dTransitionType>& nextStateRun
//	int stateCount,
//	
//	string& nextCodeCases, 
//	string& automataCode, 

//	dTree<int, int>& transitionsCount,
//	dTree<dTree<int, int>, int>& nextStateMap,
//	dTree<dTree<char, int>, int>& characterTestMap,
//	dTree<dTree<int, int>, int>& testSetArrayIndexMap) const
) const
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



//	char stateCountString[256];
//	sprintf (stateCountString, "%d", stateCount + 1);
//	string statesCountText (stateCountString);
//	for (size_t i = templateHeader.find ("$(statesCount)"); i != -1; i = templateHeader.find ("$(statesCount)")) {
//		templateHeader.replace(i, 14, statesCountText);
//	}
//	position = templateHeader.find ("$(nextTokenStart)");
//	templateHeader.replace(position, 17, nextCodeCases);

//	for (int i = 0; i < stateCount; i ++) {
//		if (!transitionsCount.Find(i)) {
//			transitionsCount.Insert(0, i);
//		}
//	}

	
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
	transitionsCountString += "0";
	position = templateHeader.find ("$(transitionsCount)");
	templateHeader.replace(position, 19, transitionsCountString);

	transitionsStartString += "0";
	position = templateHeader.find ("$(transitionsStart)");
	templateHeader.replace(position, 19, transitionsStartString);

	string nextStateRunString ("");
	for (dList<dTransitionType>::dListNode* node = nextStateRun.GetFirst(); node; node = node->GetNext()) {
		char text[256];
		dTransitionType& info = node->GetInfo();
		sprintf (text, "%d, ", info.m_value);
		nextStateRunString += text;
		sprintf (text, "%d, ", info.m_transitionType);
		nextStateRunString += text;
		sprintf (text, "%d, ", info.m_targetState);
		nextStateRunString += text;
	}
	nextStateRunString += "0";
	position = templateHeader.find ("$(nextTranstionList)");
	templateHeader.replace(position, 20, nextStateRunString);

/*

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

#if 0
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
#endif

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
*/
	FILE* const headerFile = fopen (fileName, "w");
	_ASSERTE (headerFile);
	fprintf (headerFile, "%s", templateHeader.c_str());

	fclose (headerFile);
}

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
	char* const legal = "/* Copych1 (c) <2009> <Newton Game Dynamics>\n"
						"*\n" 
						"* This software is provided 'as-is', without any express or implied\n" 
						"* warranty. In no event will the authors be held liable for any damages\n" 
						"* arising from the use of this software.\n" 
						"*\n"  
						"* Permission is granted to anyone to use this software for any purpose,\n" 
						"* including commercial applications, and to alter it and redistribute it\n" 
						"* freely\n" 
						"*/\n\n"
						"//\n" 
						"//Auto generated Lexical Analyzer class: %s\n"
						"//\n";


	char cppFileName[256];
	strcpy (cppFileName, outputFileName);
	char* const dot = strrchr (cppFileName, '.');
	if (dot) {
		*dot = 0;
	}
	strcat (cppFileName, ".cpp");


	FILE* const cppFile = fopen (cppFileName, "w");
	_ASSERTE (cppFile);


	const char* fileName = strrchr (cppFileName, '/');
	if (fileName) {
		fileName ++;
	} else {
		fileName = strrchr (cppFileName, '\\');
		if (fileName) {
			fileName ++;
		} else {
			fileName = cppFileName;
		}
	}

	fprintf (cppFile, legal, fileName);


	char className[256];
	strcpy (className, fileName);
	strtok (className, ".");

	ParseDefinitions (cppFile, className);
	fclose (cppFile);



	*dot = 0;
	strcat (cppFileName, ".h");
	FILE* const headerFile = fopen (cppFileName, "w");
	_ASSERTE (headerFile);

	fprintf (headerFile, legal, fileName);

	fprintf (headerFile, "#ifndef __%s_d__\n"
						 "#define __%s_d__\n\n", className, className);


	fprintf (headerFile, "#include <string>\n"
						 "using namespace std;\n\n");

	fprintf (headerFile,"class %s\n" 
						"{\n"
						"\tpublic:\n"
						"\t%s(const char* const data);\n"
						"\t~%s();\n\n"
						"\tint NextToken ();\n"
						"\tconst char* GetTokeString () const;\n\n"
						"\tprotected:\n"
						"\tchar NextChar ();\n"
						"\tint NextPattern ();\n"
						"\tbool IsCharInSet (int ch, const char* const set);\n\n"
						"\t// local lexical variables\n"	
						"\tint m_token;\n"
						"\tint m_state;\n"
						"\tint m_lastState;\n"
						"\tint m_startState;\n"
						"\tint m_index;\n"
						"\tint m_startIndex;\n"
						"\tconst char* m_data;\n"
						"\tstring m_tokenString;\n"
						"};\n", className, className, className);

	fprintf (headerFile, "#endif\n");

	fclose (headerFile);
}

dLexCompiler::~dLexCompiler()
{
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
void dLexCompiler::ParseDefinitions (FILE* const file, const char* const className)
{

	string preheaderCode ("") ;
	// parse definitions
	{
		m_tokenList.AddTokenData (m_whiteSpace, "[ \t\v\n\f]+");
		m_tokenList.AddTokenData (m_comment, "(/\\*([^*]|[\r\n]|(\\*+([^*/]|[\r\n])))*\\*+/)|(//.*)");
		m_tokenList.AddTokenData (m_codeBlock, "%\\{([^%]|[\r\n]|(%+([^%}]|[\r\n])))*%+\\}");
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
	string nextTokeCode ("int ");
	nextTokeCode += className;
	nextTokeCode += "::NextPattern ()\n";
	nextTokeCode += "{\n";
	nextTokeCode += "\tm_index = m_startIndex;\n";
	nextTokeCode += "\tswitch (m_startState)\n";
	nextTokeCode += "\t{\n";

	string automataCode ("");

	int initialState = 0;
	dChatertSetMap characterSet; 
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
//		m_tokenList.AddTokenData (m_quatedString, "\"[a-zA-Z_][0-9a-zA-Z]*\"");
		m_tokenList.AddTokenData (m_quatedString, "\"[^\" \t\v\n\f]*\"");
		m_tokenList.AddTokenData (m_delimiter, "%%");
		m_tokenList.AddTokenData (m_extendedRegularExpresion, "((\\[[^\\]]+\\])|[^ \t\v\n\f[]+)+");
		
		
		for (NextToken(); (m_token != m_end) && (m_token != m_delimiter); ) {
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
						return;
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
						initialState = keywordAutomata.ConvertSwitchCaseStatements (automataCode, nextTokeCode, initialState, characterSet, userAction);
//						DTRACE ((automataCode.c_str()));
						break;
					}

					case m_extendedRegularExpresion:
					{
						//m_defintions
						ExpandedNFA expresionNFA (expression.c_str(), &m_defintions);
						dDeterministicFiniteAutonataCompiler expresionAutomata (expresionNFA);
						initialState = expresionAutomata.ConvertSwitchCaseStatements (automataCode, nextTokeCode, initialState, characterSet, userAction);
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


	string automataCodeStart ("int ");
	automataCodeStart += className;
	automataCodeStart += "::NextToken ()\n";
	automataCodeStart += "{\n";
	automataCodeStart += "\t//static strings patterns\n";
	
	dTree<dList <dChatertSetMap::ChatertSet>::dListNode*, int>::Iterator iter (characterSet.GetSets());
	for (iter.Begin(); iter; iter ++) {
		char name[2048];
		dChatertSetMap::ChatertSet& set = iter.GetNode()->GetInfo()->GetInfo();

		int count = set.GetLength();
//		sprintf (name, "\tstatic int textSize_%d = %d;\n", set.GetId() & 0x7fff, count);
//		automataCodeStart += name;

		sprintf (name, "\tstatic char text_%d[] = {", set.GetId() & 0x7fff);
		int length = strlen (name);
		const char* const str = set.GetSet();
		for (int i = 0; i < count; i ++) {
			char tmp[128];
			sprintf (tmp, "%d, ", str[i]);
			sprintf (&name[length], "%s", tmp);
			length += strlen (tmp);
		}
		sprintf (&name[length], "0};\n");
		automataCodeStart += name;
	}


	automataCodeStart += "\t\n";
	automataCodeStart += "\tm_state = 0;\n";
	automataCodeStart += "\tm_startState = 0;\n";
	automataCodeStart += "\tm_startIndex = m_index;\n";
	automataCodeStart += "\twhile (m_state != m_lastState)\n";
	automataCodeStart += "\t{\n";
	automataCodeStart += "\t\tswitch (m_state)\n";
	automataCodeStart += "\t\t{\n";



	automataCode += "\t\t}\n";
	automataCode += "\t}\n";
	automataCode += "\tm_tokenString = \"\";\n";
	automataCode += "\treturn -1;\n";
	automataCode += "}\n\n";

	nextTokeCode += "\t}\n";
	nextTokeCode += "\treturn m_startState;\n";
	nextTokeCode += "}\n\n";


	char lstState[256];
	sprintf (lstState, "\t,m_lastState(%d)\n", initialState);

	string supportFuntions ("");
	supportFuntions += "#include \"";
	supportFuntions += className;
	supportFuntions += ".h\"\n\n";
	supportFuntions += className;
	supportFuntions += "::";
	supportFuntions += className;
	supportFuntions += "(const char* const data)\n";
	supportFuntions += "\t:m_token(0)\n";
	supportFuntions += "\t,m_state(0)\n";
	supportFuntions += lstState;
	supportFuntions += "\t,m_startState(0)\n";
	supportFuntions += "\t,m_index(0)\n";
	supportFuntions += "\t,m_startIndex(0)\n";
	supportFuntions += "\t,m_data(data)\n";
	supportFuntions += "\t,m_tokenString(\"\")\n";
	supportFuntions += "{\n";
	supportFuntions += "}\n\n";

	supportFuntions += className;
	supportFuntions += "::~";
	supportFuntions += className;
	supportFuntions += "()\n";
	supportFuntions += "{\n";
	supportFuntions += "}\n\n";

	supportFuntions += "char ";
	supportFuntions += className;
	supportFuntions += "::NextChar()\n";
	supportFuntions += "{\n";
	supportFuntions += "\treturn m_data[m_index++];\n";
	supportFuntions += "}\n\n";


	supportFuntions += "const char* ";
	supportFuntions += className;
	supportFuntions += "::GetTokeString() const\n";
	supportFuntions += "{\n";
	supportFuntions += "\treturn m_tokenString.c_str();\n";
	supportFuntions += "}\n\n";



	supportFuntions += "bool ";
	supportFuntions += className;
	supportFuntions += "::IsCharInSet (int ch, const char* const set)\n";
	supportFuntions += "{\n";
	supportFuntions += "\tfor (int i = 0; i < set[i]; i ++) {\n";
	supportFuntions += "\t\tif (ch == set[i]) {\n";
	supportFuntions += "\t\t\t return true;\n";
	supportFuntions += "\t\t}\n";
	supportFuntions += "\t}\n";
	supportFuntions += "\treturn false;\n";
	supportFuntions += "}\n\n";


//	DTRACE ((supportFuntions.c_str()));
//	DTRACE ((nextTokeCode.c_str()));
//	DTRACE ((automataCodeStart.c_str()));
//	DTRACE ((automataCode.c_str()));

	fwrite (preheaderCode.c_str(), preheaderCode.length(), sizeof (char), file);
	fwrite (supportFuntions.c_str(), supportFuntions.length(), sizeof (char), file);
	fwrite (nextTokeCode.c_str(), nextTokeCode.length(), sizeof (char), file);
	fwrite (automataCodeStart.c_str(), automataCodeStart.length(), sizeof (char), file);
	fwrite (automataCode.c_str(), automataCode.length(), sizeof (char), file);

	if (m_token != m_delimiter) {
		NextToken();
	}

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

		for (dList<dAutomataState::Transition>::dListNode* node = statePair.GetStart()->m_transtions.GetFirst(); node; node = node->GetNext()) {
			dAutomataState::Transition& sourceTransition = node->GetInfo();
			dAutomataState* const targetState = sourceTransition.GetState();

			dTree<dAutomataState*,dAutomataState*>::dTreeNode* const mapMode = filter.Find(targetState);

			int ch = sourceTransition.GetCharater();
			if (ch & (1<<15)) {
				const dChatertSetMap::ChatertSet* const charSet = nfa->m_charaterSetMap.FindSet (ch);
				ch = m_charaterSetMap.AddSet(charSet->GetSet(), charSet->GetLength());
			}

			if (mapMode) {
				dAutomataState* const accepting = mapMode->GetInfo();
				statePair.GetAccepting()->m_transtions.Append(dAutomataState::Transition(ch, accepting));
			} else {
				pool[stack] = StateConstructPair (targetState, new dAutomataState(m_stateID));
				filter.Insert(pool[stack].GetAccepting(), targetState);
				statePair.GetAccepting()->m_transtions.Append(dAutomataState::Transition(ch, pool[stack].GetAccepting()));
				stack ++;
				m_stateID ++;
				_ASSERTE (stack < sizeof (pool)/sizeof (pool[0]));
			}
		}
	}

	dAutomataState* const startState = filter.Find(nfa->m_startState)->GetInfo();
	dAutomataState* const acceptingState = filter.Find(nfa->m_acceptingState)->GetInfo();

	m_stack.Push(startState, acceptingState);

	DTRACE(("Push %s\n", label));
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

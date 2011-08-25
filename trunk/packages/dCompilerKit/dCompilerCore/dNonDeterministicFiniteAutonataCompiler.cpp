/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include "dCompilerCore.h"
#include "dAutomataState.h"
#include "dNonDeterministicFiniteAutonataCompiler.h"

/*
char dNonDeterministicFiniteAutonataCompiler::m_asciiSet[] = 
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',  
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'W', 'V', 'X', 'Y', 'Z',  
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'w', 'v', 'x', 'y', 'z',  
	'@', '{', '|', '}', '[', ']', '^', '_', ':', ';', '<', '=', '>', '?',
	' ', '!', '#', '$', '%', '&', '(', ')', '*', '+', ',', '-', '.', '/',
	'\\', '\"', '\'', 
};
*/

char dNonDeterministicFiniteAutonataCompiler::m_asciiSet[] = {   1,  2,  3,  4,  5,  6,  7,  8,  9, 
																10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
																20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
																30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
																40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
																50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 
																60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
																70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 
																80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
																90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
																100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 
																110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
																120, 121, 122, 123, 124, 125, 126, 127, 0};

char dNonDeterministicFiniteAutonataCompiler::m_asciiSetButNewLine[] = {  1,  2,  3,  4,  5,  6,  7,  8,  9, 
																	    11, 12, 13, 14, 15, 16, 17, 18, 19, 
																		20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
																		30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
																		40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
																		50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 
																		60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
																		70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 
																		80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
																		90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
																		100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 
																		110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
																		120, 121, 122, 123, 124, 125, 126, 127, 0};


int GetScapeChar (int symbol)
{
	if ((symbol>>8) == '\\') {
		char low = char (symbol);

		static char scapeSquence[] = "nrtvf~!@#$%^&*()-+{}|:\"<>?`_=[]\\;\',./";
		static char ascciSquence[] = "\n\r\t\v\f~!@#$%^&*()-+{}|:\"<>?`_=[]\\;\',./";
		for (int i = 0; scapeSquence[i]; i ++) {
			if (scapeSquence[i] == low) {
				return ascciSquence[i];
			}
		}
		return low;
	}

	return symbol;
}


dNonDeterministicFiniteAutonataCompiler::dNonDeterministicFiniteAutonataCompiler()
	:m_error(true)
	,m_token (0)
	,m_stateID (0)
	,m_regularExpressionIndex(0)
	,m_startState(NULL) 
	,m_acceptingState(NULL) 
	,m_stack()
{
}

dNonDeterministicFiniteAutonataCompiler::dNonDeterministicFiniteAutonataCompiler(const char* const regularExpression)
	:m_error(false)
	,m_token (0)
	,m_stateID (0)
	,m_regularExpressionIndex(0)
	,m_startState(NULL) 
	,m_acceptingState(NULL) 
	,m_stack()
{
	CompileExpression(regularExpression);
}
/*
dNonDeterministicFiniteAutonataCompiler::dNonDeterministicFiniteAutonataCompiler(const dNonDeterministicFiniteAutonataCompiler& nfa)
	:m_error(false)
	,m_token (0)
	,m_stateID (0)
	,m_regularExpressionIndex(0)
	,m_startState(NULL) 
	,m_acceptingState(NULL) 
	,m_stack()
{

}
*/

dNonDeterministicFiniteAutonataCompiler::~dNonDeterministicFiniteAutonataCompiler(void)
{
	if (m_startState) {
		DeleteNFA (m_startState);
	}
}

bool dNonDeterministicFiniteAutonataCompiler::IsValid() const
{
	return !m_error;
}

const dChatertSetMap& dNonDeterministicFiniteAutonataCompiler::GetChatertSetMap() const
{
	return m_charaterSetMap;
}

void dNonDeterministicFiniteAutonataCompiler::CompileExpression(const char* const regularExpression)
{
//	m_asciiSet['\n' - 1] = 'a';
//	m_asciiSet[sizeof (m_asciiSet) - 1] = 0;

	// prepossess the expression for simples parsing 
	PreProcessExpression (regularExpression);


	// build an NFA graph
//	DTRACE(("\n"));
//	DTRACE(("Expression: %s\n", regularExpression));

	ParseExpresionToNFA ();
}


void dNonDeterministicFiniteAutonataCompiler::DeleteNFA (dAutomataState* const startState)
{
	dList<dAutomataState*> statesList;
	startState->GetStateArray (statesList);
	for (dList<dAutomataState*>::dListNode* node = statesList.GetFirst(); node; node = node->GetNext()) {
		dAutomataState* const state = node->GetInfo();
		delete state;
	}
	m_startState = NULL;
	m_acceptingState = NULL;
}

// Get the next character from the expression
// it should handle scape characters, and concatenations
int dNonDeterministicFiniteAutonataCompiler::GetChar()
{
	int ch = m_regularExpression[m_regularExpressionIndex];
	if (ch) {
		m_regularExpressionIndex ++;
		if (ch == '\\') {
			ch = (ch << 8) + m_regularExpression[m_regularExpressionIndex ++];
		} 
	}
	return ch;
}


bool dNonDeterministicFiniteAutonataCompiler::CheckInsertConcatenation (int left, int right) const
{
	bool test = (((!IsOperator(left)) || (left == m_closeParentesis) || (left == m_closeSquareBrakect) || (left == m_zeroOrMore) || (left == m_oneOrMore) || (left == m_zeroOrOne)) && 
		         ((!IsOperator(right))|| (right == m_openParentesis)  || (right == m_openSquareBrakect))); 
	return test;
}

void dNonDeterministicFiniteAutonataCompiler::PreProcessExpression (const char* const regularExpression)
{
	_ASSERTE (sizeof (m_regularExpression) > strlen (regularExpression));
	sprintf (m_regularExpression, "%s", regularExpression);

	char buffer[256];
	int ch0 = GetChar();
	int count = 0;
	for (int ch1 = GetChar(); ch1; ch1 = GetChar()) {
		if (ch0 > 256) {
			buffer[count ++] = char (ch0>>8);
			_ASSERTE (count < sizeof (buffer));
		}

		buffer[count ++] = char (ch0);
		_ASSERTE (count < sizeof (buffer));

		#ifdef _DEBUG
		bool test0 = CheckInsertConcatenation (ch0, ch1);
		bool test1 = (((!IsOperator(ch0)) || (ch0 == m_closeParentesis) || (ch0 == m_closeSquareBrakect) || (ch0 == m_zeroOrMore) || (ch0 == m_oneOrMore) || (ch0 == m_zeroOrOne)) && 
					 ((!IsOperator(ch1)) || (ch1 == m_openParentesis)  || (ch1 == m_openSquareBrakect)));
		_ASSERTE (test0 == test1);
		#endif

//		if (((!IsOperator(ch0)) || (ch0 == m_closeParentesis) || (ch0 == m_closeSquareBrakect) || (ch0 == m_zeroOrMore) || (ch0 == m_oneOrMore) || (ch0 == m_zeroOrOne)) && 
//			((!IsOperator(ch1)) || (ch1 == m_openParentesis)  || (ch1 == m_openSquareBrakect))) {
		if (CheckInsertConcatenation (ch0, ch1)) {
			buffer[count ++] = char (m_concatenation);
			_ASSERTE (count < sizeof (buffer));
		}
		ch0 = ch1;
	}
	if (ch0 > 256) {
		buffer[count ++] = char (ch0 >> 8);
		_ASSERTE (count < sizeof (buffer));
	}
	buffer[count ++] = char (ch0);
	_ASSERTE (count < sizeof (buffer));
	buffer[count ++] = 0;
	_ASSERTE (count < sizeof (buffer));

	m_regularExpressionIndex = 0;
	sprintf (m_regularExpression, "%s", buffer);
}

bool dNonDeterministicFiniteAutonataCompiler::IsOperator (int character) const
{
	static char operation[] = {m_union, char (m_concatenation), m_zeroOrMore, m_oneOrMore, m_zeroOrOne, 
							   m_openParentesis, m_closeParentesis, m_openSquareBrakect, m_closeSquareBrakect };
	for (int i = 0; i < sizeof (operation) / sizeof (operation[0]); i ++) {
		if (character == operation[i]) {
			return true;
		}
	}
	return false;
}

void dNonDeterministicFiniteAutonataCompiler::Match (int token)
{
	if (m_token == token) {
		m_token = GetChar();
	} else {
//		DTRACE(("parse error\n"));
		m_error = true;
	}
}


void dNonDeterministicFiniteAutonataCompiler::PushId (int charater)
{
	dAutomataState* const start = new dAutomataState (m_stateID ++);
	dAutomataState* const accepting = new dAutomataState (m_stateID ++);

	charater = GetScapeChar (charater);
	start->m_transtions.Append(dAutomataState::Transition(charater, accepting));

	m_stack.Push(start, accepting);

//	DTRACE(("Push ", charater));
//	if (charater > 256) {
//		DTRACE(("\\"));
//	}
//	DTRACE(("%c\n", charater));
}

void dNonDeterministicFiniteAutonataCompiler::PushSet (const char* const set, int size)
{
	dAutomataState* const start = new dAutomataState (m_stateID ++);
	dAutomataState* const accepting = new dAutomataState (m_stateID ++);
	
	int ch = m_charaterSetMap.AddSet(set, size);
	start->m_transtions.Append(dAutomataState::Transition(ch, accepting));

	m_stack.Push(start, accepting);

//	DTRACE(("Push charSet%d\n", ch & (0x7fff)));
}



void dNonDeterministicFiniteAutonataCompiler::ReduceUnionDiagram()
{
	if (m_stack.IsEmpty()) {
		m_error = true;
		DTRACE(("Parce stack underflow error\n"));
		return;
	}
	StateConstructPair rightOperand (m_stack.Pop());

	if (m_stack.IsEmpty()) {
		m_error = true;
		DTRACE(("Parce stack underflow error\n"));
		m_stack.Push(rightOperand.GetStart(), rightOperand.GetAccepting());
		return;
	}
	StateConstructPair leftOperand (m_stack.Pop());

	dAutomataState* const start = new dAutomataState (m_stateID ++);
	dAutomataState* const accepting = new dAutomataState (m_stateID ++);

	start->m_transtions.Append(dAutomataState::Transition(0, leftOperand.GetStart()));
	start->m_transtions.Append(dAutomataState::Transition(0, rightOperand.GetStart()));
	leftOperand.GetAccepting()->m_transtions.Append(dAutomataState::Transition(0, accepting));
	rightOperand.GetAccepting()->m_transtions.Append(dAutomataState::Transition(0, accepting));

	m_stack.Push(start, accepting);
//	DTRACE(("operator union\n"));
}

void dNonDeterministicFiniteAutonataCompiler::ReduceConcatenationDiagram()
{
	if (m_stack.IsEmpty()) {
		m_error = true;
		DTRACE(("Parce stack underflow error\n"));
		return;
	}
	StateConstructPair rightOperand (m_stack.Pop());

	if (m_stack.IsEmpty()) {
		m_error = true;
		DTRACE(("Parce stack underflow error\n"));
		m_stack.Push(rightOperand.GetStart(), rightOperand.GetAccepting());
		return;
	}
	StateConstructPair leftOperand (m_stack.Pop());

	leftOperand.GetAccepting()->m_transtions.Append(dAutomataState::Transition(0, rightOperand.GetStart()));

	m_stack.Push(leftOperand.GetStart(), rightOperand.GetAccepting());
//	DTRACE(("operator concatenation\n"));
}

void dNonDeterministicFiniteAutonataCompiler::ReduceZeroOrMoreDiagram()
{
	if (m_stack.IsEmpty()) {
		m_error = true;
		DTRACE(("Parce stack underflow error\n"));
		return;
	}
	StateConstructPair operand (m_stack.Pop());

	dAutomataState* const start = new dAutomataState (m_stateID ++);
	dAutomataState* const accepting = new dAutomataState (m_stateID ++);

	start->m_transtions.Append(dAutomataState::Transition(0, accepting));
	start->m_transtions.Append(dAutomataState::Transition(0, operand.GetStart()));
	operand.GetAccepting()->m_transtions.Append(dAutomataState::Transition(0, accepting));
	operand.GetAccepting()->m_transtions.Append(dAutomataState::Transition(0, operand.GetStart()));

	m_stack.Push(start, accepting);

//	DTRACE(("operator zeroOrMore\n"));
}

void dNonDeterministicFiniteAutonataCompiler::ReduceOneOrMoreDiagram()
{
	if (m_stack.IsEmpty()) {
		m_error = true;
		DTRACE(("Parce stack underflow error\n"));
		return;
	}
	StateConstructPair operand (m_stack.Pop());

	dAutomataState* const start = new dAutomataState (m_stateID ++);
	dAutomataState* const accepting = new dAutomataState (m_stateID ++);

	start->m_transtions.Append(dAutomataState::Transition(0, operand.GetStart()));
	operand.GetAccepting()->m_transtions.Append(dAutomataState::Transition(0, accepting));
	operand.GetAccepting()->m_transtions.Append(dAutomataState::Transition(0, operand.GetStart()));

	m_stack.Push(start, accepting);

//	DTRACE(("operator oneOrMore\n"));
}

void dNonDeterministicFiniteAutonataCompiler::ReduceZeroOrOneDiagram()
{
	if (m_stack.IsEmpty()) {
		m_error = true;
//		DTRACE(("Parce stack underflow error\n"));
		return;
	}
	StateConstructPair operand (m_stack.Pop());

	dAutomataState* const start = new dAutomataState (m_stateID ++);
	dAutomataState* const accepting = new dAutomataState (m_stateID ++);

	start->m_transtions.Append(dAutomataState::Transition(0, accepting));
	start->m_transtions.Append(dAutomataState::Transition(0, operand.GetStart()));
	operand.GetAccepting()->m_transtions.Append(dAutomataState::Transition(0, accepting));

	m_stack.Push(start, accepting);

//	DTRACE(("operator zeroOrOne\n"));
}


// UnionExpression				: ConcatenateExpresion MoreUnionExpression
// MoreUnionExpression			: m_union ConcatenateExpresion MoreUnionExpression | nothing
void dNonDeterministicFiniteAutonataCompiler::UnionExpression ()
{
	ConcatenationExpression ();
	for (;;) {
		if (m_token == m_union) {
			Match (m_union);
			ConcatenationExpression ();

			ReduceUnionDiagram();

		} else {
			break;
		}
	}
}


// ConcatenationExpression		: UnuaryExpression MoreConcatenationExpression
// MoreConcatenationExpression	: m_concatenation UnuaryExpression MoreConcatenationExpression | nothing
void dNonDeterministicFiniteAutonataCompiler::ConcatenationExpression ()
{
	UnuaryExpression ();
	for (;;) {
		if (m_token == m_concatenation) {
			Match (m_concatenation);
			UnuaryExpression ();

			ReduceConcatenationDiagram();
		} else {
			break;
		}
	}
}

// UnuaryExpresion : m_zeroOrMore | m_oneOrMore | m_zeroOrOne | BracketedExpression | nothing
void dNonDeterministicFiniteAutonataCompiler::UnuaryExpression ()
{
	ShiftID ();
	if (m_token == m_zeroOrMore) {
		Match (m_zeroOrMore);
		ReduceZeroOrMoreDiagram ();

	} else if (m_token == m_oneOrMore) {
		Match (m_oneOrMore);

		ReduceOneOrMoreDiagram ();

	} else if (m_token == m_zeroOrOne) {
		Match (m_zeroOrOne);

		ReduceZeroOrOneDiagram ();
	}
}

// BracketedExpression  : '^' BracketedExpression
// BracketedExpression  : '^' m_concatenation BracketedExpression
// BracketedExpression  : CHARACTER m_concatenation BracketedExpression
// BracketedExpression  : CHARACTER  m_concatenation '-' CHARACTER
// BracketedExpression  : CHARACTER
// BracketedExpression  : nothing
int dNonDeterministicFiniteAutonataCompiler::BracketedExpression (char* const set, int size)
{
	if (m_token != ']') {
		if (m_token == '^') {
			Match ('^');
			if (m_token == m_concatenation) {
				Match (m_concatenation);
			}
			char exclusiveAsciiSet[D_ASCII_SET_MAX_SIZE];
			int exclusiveSize = BracketedExpression (exclusiveAsciiSet, 0);

			for (int i = 0; i < m_asciiSet[i]; i ++) {
				char ch = m_asciiSet[i];
				int j = 0;
				for (; j < exclusiveSize; j ++) {
					if (ch == exclusiveAsciiSet[j]) {
						break;
					}
				}
				if (j == exclusiveSize) {
					set[size] = ch;
					size ++;
					_ASSERTE (size < D_ASCII_SET_MAX_SIZE);
				}
			}

		} else {
			int ch = m_token;
			Match (m_token);
			if (m_token == m_concatenation) {
				Match (m_token);
				if (m_token == '-') {
					Match (m_token);
					Match (m_token);

					int ch1 = m_token;
					for (int i = ch; i <= ch1; i ++) {
						set[size] = char (i);
						size ++;			
					}

					Match (m_token);
					if (m_token == m_concatenation) {
						Match (m_token);
					}
					size = BracketedExpression (set, size);
				

				} else {
					if ((ch>>8) == '\\') {
						ch = GetScapeChar (ch);
					}
					set[size] = char (ch);
					size ++;
					_ASSERTE (size < D_ASCII_SET_MAX_SIZE);
					size = BracketedExpression (set, size);
				}

			} else {
				if ((ch>>8) == '\\') {
					ch = GetScapeChar (ch);
				}
				set[size] = char (ch);
				size ++;
				_ASSERTE (size < D_ASCII_SET_MAX_SIZE);
			}
		}
	}

	return size;
}

// id	: m_openSquareBrakect BracketedExpression m_closeSquareBrakect
// id	: m_openParentesis UnionExpression m_closeParentesis
// id	: .
// id	: CHARACTER
void dNonDeterministicFiniteAutonataCompiler::ShiftID()
{
	if (m_token == m_openParentesis) {
		Match (m_openParentesis);
		UnionExpression ();
		Match (m_closeParentesis);

	} else if (m_token == m_openSquareBrakect) {
		char asciiSet[D_ASCII_SET_MAX_SIZE];
		Match (m_openSquareBrakect);
		int size = BracketedExpression (asciiSet, 0);
		_ASSERTE (size < D_ASCII_SET_MAX_SIZE);
		asciiSet[size] = 0;


		PushSet (asciiSet, size);
		Match (m_closeSquareBrakect);

	} else if (m_token == '.') {
		PushSet (m_asciiSetButNewLine, strlen (m_asciiSetButNewLine));

		Match (m_token);

	} else if (!IsOperator (m_token)) {
		PushId (m_token);

		Match (m_token);
	} else {
		Match (m_token);
		DTRACE(("parse error\n"));
	}
}

// Thompson's algorithm for constructing a Nondeterministic Finite Automaton (NFA) from a Basic Regular Expressions (BRE)
// algorithm from book Compilers Principles and Tools: by Alfred V.Aho, Ravi Sethi and Jeffrey D. Ullman 
//
// Productions Rule for a top down grammar, stack based Regular expression grammar
//
// UnionExpression				: ConcatenateExpresion MoreUnionExpression
// MoreUnionExpression			: m_union ConcatenateExpresion MoreUnionExpression | nothing
//
// ConcatenationExpression		: UnuaryExpresion MoreConcatenationExpression
// MoreConcatenationExpression	: m_concatenation UnuaryExpresion MoreConcatenationExpression  | nothing
//
// UnuaryExpresion				: m_zeroOrMore | m_oneOrMore | m_zeroOrOne| nothing
//
// BracketedExpression  		: '^' BracketedExpression
// BracketedExpression  		: '^' m_concatenation BracketedExpression
// BracketedExpression  		: CHARACTER m_concatenation BracketedExpression
// BracketedExpression  		: CHARACTER  m_concatenation '-' CHARACTER
// BracketedExpression  		: CHARACTER
// BracketedExpression  		: nothing
//
// id							: m_openSquareBrakect BracketedExpression m_closeSquareBrakect
// id							: m_openParentesis UnionExpression m_closeParentesis
// id							: .
// id							: CHARACTER 
void dNonDeterministicFiniteAutonataCompiler::ParseExpresionToNFA ()
{
	m_error = false;
	for (m_token = GetChar(); m_token;) {
		UnionExpression ();
	}

	if (m_error) {
		_ASSERTE (0);
		while (!m_stack.IsEmpty()) {
			StateConstructPair operand (m_stack.Pop());
			DeleteNFA (operand.GetStart());
		}
	} else {
		StateConstructPair operand (m_stack.Pop());
		m_startState = operand.GetStart(); 
		m_acceptingState = operand.GetAccepting();
		m_acceptingState->m_exitState = true;
	}
}



// Subset Construction for converting a Nondeterministic Finite Automaton (NFA) to a Deterministic Finite Automaton (DFA)
// algorithm from book Compilers Principles and Tools: by Alfred V.Aho, Ravi Sethi and Jeffrey D. Ullman 
dAutomataState* dNonDeterministicFiniteAutonataCompiler::CreateDeterministicFiniteAutomaton () const
{
	dList<dAutomataState*> stateArray;
	dTree<int, int> symbolsList;
	
	m_startState->GetStateArray(stateArray);
	for (dList<dAutomataState*>::dListNode* node = stateArray.GetFirst(); node; node = node->GetNext()) {
		dAutomataState* const state = node->GetInfo();
		for (dList<dAutomataState::Transition>::dListNode* transitionNode = state->m_transtions.GetFirst(); transitionNode; transitionNode = transitionNode->GetNext()) {
			dAutomataState::Transition& transition = transitionNode->GetInfo();
			int ch = transition.GetCharater();
			if (ch) {
				symbolsList.Insert(ch, ch);
			}
		}
	}

	dTree<dAutomataState*,dAutomataState*> NFAmap;
	dTree<dAutomataState*,dAutomataState*> subSet;


	int stateID = 0;
	NFAmap.Insert(m_startState, m_startState);

	dAutomataState* stackPool[2048];

	EmptyTransitionClosure (NFAmap, subSet);
	dAutomataState* const startState = new dAutomataState(subSet, stateID ++);

	dList<dAutomataState*> newStatesList;
	int stack = 1;
	stackPool[0] = startState;

	newStatesList.Append(startState);

	while (stack) {
		stack --;
		dAutomataState* const state = stackPool[stack];

		dTree<int, int>::Iterator iter (symbolsList);
		for (iter.Begin(); iter; iter ++) {
			int ch = iter.GetNode()->GetInfo();

			dTree<dAutomataState*,dAutomataState*> moveSet;
			MoveSymbol (ch, state, moveSet);

			if (moveSet.GetCount()) {

				dTree<dAutomataState*,dAutomataState*> subSet;
				EmptyTransitionClosure (moveSet, subSet);

				dAutomataState* foundState = NULL;
				for (dList<dAutomataState*>::dListNode* node = newStatesList.GetFirst(); node; node = node->GetNext()) {
					dAutomataState* const state = node->GetInfo();
					if (CompareSets(state->m_myNFANullStates, subSet)) {
						foundState = state;
						break;
					}
				}

				if (foundState) {
					state->m_transtions.Append(dAutomataState::Transition(ch, foundState));

				} else {
				
					dAutomataState* const targetState = new dAutomataState(subSet, stateID ++);
					stackPool[stack] = targetState;
					stack ++;
					_ASSERTE (stack < sizeof (stackPool)/sizeof (stackPool[0]));

					newStatesList.Append(targetState);
					state->m_transtions.Append(dAutomataState::Transition(ch, targetState));
				}
			}
		}
	}


//	int index = 0;
//	dAutomataState** const stateSort = new dAutomataState*[stateArray.GetCount()];
//	for (dList<dAutomataState*>::dListNode* node = stateArray.GetFirst(); node; node = node->GetNext()) {
//		stateSort[index] = node->GetInfo();
//		index ++;
//	}
//	qsort (stateSort, index, sizeof (dAutomataState*), SortStates);
//	for (int i = 0; i < index; i ++) {
//		dAutomataState* const state = stateSort[i];
//		state->m_id = i;
//	}
//	delete stateSort;

	return startState;
}

int dNonDeterministicFiniteAutonataCompiler::SortStates (const void *ptr0, const void *ptr1)
{
	dAutomataState* const state0 = *(dAutomataState**) ptr0;
	dAutomataState* const state1 = *(dAutomataState**) ptr1;

	if (state0->m_exitState == state1->m_exitState) {
		return 0;
	}
	if (state0->m_exitState) {
		return -1;
	}
	return 1;
}

bool dNonDeterministicFiniteAutonataCompiler::CompareSets (dList<dAutomataState*>& setA, dTree<dAutomataState*,dAutomataState*>& setB) const
{
	if (setA.GetCount() == setB.GetCount()) {
		for (dList<dAutomataState*>::dListNode* node = setA.GetFirst(); node; node = node->GetNext()) {
			if (!setB.Find(node->GetInfo())) {
				return false;
			}
		}
		return true;
	}
	return false;
}

void dNonDeterministicFiniteAutonataCompiler::MoveSymbol (int symbol, const dAutomataState* const state, dTree<dAutomataState*,dAutomataState*>& ouput) const
{
	for (dList<dAutomataState*>::dListNode* stateNode = state->m_myNFANullStates.GetFirst(); stateNode; stateNode = stateNode->GetNext()) {
		const dAutomataState* const state = stateNode->GetInfo();
		for (dList<dAutomataState::Transition>::dListNode* transitionNode = state->m_transtions.GetFirst(); transitionNode; transitionNode = transitionNode->GetNext()) {
			dAutomataState::Transition& thans = transitionNode->GetInfo();
			int ch = thans.GetCharater();
			if (ch == symbol) {
				dAutomataState* const target = thans.GetState();
				ouput.Insert(target, target);
			}
		}
	}
}

void dNonDeterministicFiniteAutonataCompiler::EmptyTransitionClosure (const dTree<dAutomataState*,dAutomataState*>& set, dTree<dAutomataState*,dAutomataState*>& closureStates) const
{
	int stack = 0;
	dAutomataState* stackPool[2048];

	dTree<dAutomataState*,dAutomataState*>::Iterator iter (set);
	for (iter.Begin(); iter; iter ++) {
		dAutomataState* const state = iter.GetNode()->GetInfo();
		stackPool[stack] = state;
		stack ++;
		_ASSERTE (stack  < sizeof (stackPool) / sizeof (stackPool[0]));
		closureStates.Insert(state, state);
	}

	while(stack) {
		stack --;
		dAutomataState* const state = stackPool[stack];
		for (dList<dAutomataState::Transition>::dListNode* node = state->m_transtions.GetFirst(); node; node = node->GetNext()) {
			dAutomataState::Transition& transition = node->GetInfo();
			if (transition.GetCharater() == 0) {
				dAutomataState* const targetState = transition.GetState();
				if(!closureStates.Find(targetState)) {
					closureStates.Insert(targetState, targetState);
					stackPool[stack] = targetState;
					stack ++;
					_ASSERTE (stack  < sizeof (stackPool) / sizeof (stackPool[0]));
				}
			}
		}
	}
}
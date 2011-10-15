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

//
//Auto generated Parser Generator class: dScriptCompiler.cpp
//

#include "dScriptCompiler.h"
#include "dNewtonScriptLexical.h"


void* operator new (size_t size)
{
	static int xxx;
	xxx ++;
	if (xxx == 83)
	xxx *=1;

	void* const ptr = malloc (size);
//	DTRACE (("%x %d %d\n", ptr, size, xxx));
	return ptr;
}


dScriptCompiler::dScriptCompiler(const char* const sourceFileName)
	:dNewtonScriptParser ()
	,m_fileName(sourceFileName)
	,m_currentFunction(NULL)
	,m_classList()
	,m_scopeStack()
	,m_allNodes()
{
}

dScriptCompiler::~dScriptCompiler()
{
	for(dList<dDAGClassNode*>::dListNode* node = m_classList.GetFirst(); node; node = node->GetNext() ) {
		dDAGClassNode* const classNode = node->GetInfo();
		classNode->Release();
	}

	for(dList<dDirectAcyclicgraphNode*>::dListNode* node = m_allNodes.GetFirst(); node; node = node->GetNext() ) {
		dDirectAcyclicgraphNode* const dagNode = node->GetInfo();
		dagNode->Release();
	}
}


void dScriptCompiler::DisplayError (const char* format, ...) const
{
	va_list v_args;
	char* const text = (char*) malloc (strlen (format) + 2048);

	text[0] = 0;
	va_start (v_args, format);     
	vsprintf(text, format, v_args);
	va_end (v_args);            

	fprintf (stderr, text);
	#ifdef _MSC_VER  
		OutputDebugStringA (text);
	#endif	

	free (text);
}


void qsort (int a[], int m, int n)
{
	if (n <= m) {
		return ;
	}

	int i = m - 1;
	int j = n;
	int v = a[n];
	int x;
	for (;;){
		do 
			i = i + 1;
		while (a[i] < v);

		do 
			j = j - 1;
		while (a[j] > v);

		if (i >= j) {
			break;
		}
		x = a[i];
		a[i] = a[j];
		a[j] = x;
	}
	x = a[i];
	a[i] = a[n];
	a[n] = x;
	qsort (a, m, j);
	qsort (a, i + 1, n);
}

void qsort (int a[], int size)
{
	for (int i = 1; i < size; i ++) {
		if (a[0] > a[i]) {
			int x = a[0];
			a[0] = a[i];
			a[i] = x;
		}
		if (a[size - i - 1] > a[size - 1]) {
			int x = a[size - i - 1];
			a[size - i - 1] = a[size - 1];
			a[size - 1] = x;
		}
	}
	qsort (a, 1, size-1);
}


int dScriptCompiler::CompileSource (const char* const source)
{
	dNewtonScriptLexical scanner (source);

//int aa[] = {4, 5,6,1,6,7,9,8,5};
//qsort(aa, sizeof(aa)/sizeof (int));
//int a = 1;
//int b = 1;
//int c = 1;
//int x = a - - ! ~ b * - c;
//x = a - - ~ ! b * - c;


	bool status = Parse(scanner);
	if (status) {
//		_ASSERTE (0);
	}
	return 0;
}


bool dScriptCompiler::Parse(dNewtonScriptLexical& scanner)
{
	return dNewtonScriptParser::Parse(scanner);
}


void dScriptCompiler::SyntaxError (const dNewtonScriptLexical& scanner, const dUserVariable& errorToken, const dUserVariable& errorTokenMarker)
{
	const char* const data = scanner.GetData();
	int start = errorToken.m_scannerIndex;
	int lineNumber = errorToken.m_scannerLine + 1;
	while (data[start] && isspace (data[start])) {
		if (data[start] == '\n') {
			lineNumber ++;
		}
		start ++;
	}

	int end = errorTokenMarker.m_scannerIndex;
	while (data[end] && isspace (data[end])) {
		end --;
	}
	_ASSERTE (end >= start);

	int length = end - start + 1;
	string errorLine (&data[start], length);
	DisplayError ("%s (%d) : syntax error on line: %s\n", m_fileName, lineNumber, errorLine.c_str());
}


dDAGClassNode* dScriptCompiler::GetCurrentClass() const
{
	_ASSERTE (m_classList.GetCount());
	return m_classList.GetLast()->GetInfo();
}

dDAGScopeBlockNode* dScriptCompiler::GetCurrentScope() const
{
	_ASSERTE (m_scopeStack.GetCount());
	return m_scopeStack.GetLast()->GetInfo();
}




void dScriptCompiler::SetParamameterAsPrivateVariable(const dUserVariable& variable)
{
//	dUserVariable returnNode;
	_ASSERTE (0);
/*
	_ASSERTE (m_currentClass);
	_ASSERTE (m_currentFunction);

	_ASSERTE (variable.m_node);
	_ASSERTE (variable.m_node->GetTypeId() == dDAGParameterNode::GetRttiType());
	dDAGParameterNode* const tmp = (dDAGParameterNode*) variable.m_node;
	tmp->m_isPublic = false;
*/
}


dScriptCompiler::dUserVariable dScriptCompiler::NewExpressionNodeConstant (const dUserVariable& value)
{
	dUserVariable returnNode;

	dDAGExpressionNodeConstant::dType type = dDAGExpressionNodeConstant::m_intValue;
	switch (int (value.m_token))
	{
		case INTEGER_VALUE:
			type = dDAGExpressionNodeConstant::m_intValue;
			break;
		case FLOAT_VALUE:
			type = dDAGExpressionNodeConstant::m_floatValue;
			break;
		case STRING_VALUE:
			type = dDAGExpressionNodeConstant::m_stringValue;
			break;
		default:
			_ASSERTE (0);
	}

	dDAGScopeBlockNode* const scope = GetCurrentScope();
	_ASSERTE (scope);
	dDAGExpressionNodeConstant* const node = scope->CreatedConstantNode(m_allNodes, type, value.m_data.c_str());
	returnNode.m_node = node;

	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::NewExpresionNodeAssigment (const dUserVariable& leftVariable, const dUserVariable& dimArray, const dUserVariable& expression)
{
	dUserVariable returnNode;
	dScriptCompiler::dUserVariable leftSideVar (NewExpressionNodeVariable (leftVariable, dimArray));

	dDAGExpressionNode* const exp = (dDAGExpressionNode*) expression.m_node;
	dDAGExpressionNodeVariable* const variable = (dDAGExpressionNodeVariable*) leftSideVar.m_node;
	_ASSERTE (exp->IsType(dDAGExpressionNode::GetRttiType()));
	_ASSERTE (variable->IsType(dDAGExpressionNodeVariable::GetRttiType()));
	
	dDAGFunctionStatementAssigment* const assigment = new dDAGFunctionStatementAssigment(m_allNodes, variable, exp);
	returnNode.m_node = assigment;
	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::BeginClassNode ()
{
	dUserVariable returnNode;
	dDAGClassNode* const classNode = new dDAGClassNode (m_allNodes);

	m_classList.Append(classNode);
	classNode->AddRef();

	returnNode.m_node = classNode;
	return returnNode ;
}

dScriptCompiler::dUserVariable dScriptCompiler::FinalizeClassNode (const dUserVariable& classNode, const dUserVariable& visibility, const dUserVariable& name, const dUserVariable& baseClass, const dUserVariable& guiInterface)
{
	dUserVariable returnNode;
	_ASSERTE (classNode.m_node == GetCurrentClass());
	GetCurrentClass()->FinalizeImplementation(visibility.m_data.c_str(), name.m_data.c_str(), (dDAGClassNode*)baseClass.m_node);
	returnNode.m_node = classNode.m_node;
	return returnNode;
}









dScriptCompiler::dUserVariable dScriptCompiler::NewParameterNode (const dUserVariable& primitiveType, const dUserVariable& identifier)
{
	dUserVariable returnNode;
	_ASSERTE (primitiveType.m_node && (primitiveType.m_node->GetTypeId() == dDAGTypeNode::GetRttiType()));
	dDAGParameterNode* const parameter = new dDAGParameterNode(m_allNodes, (dDAGTypeNode*) primitiveType.m_node, identifier.m_data.c_str());
	returnNode.m_node = parameter;
	return returnNode;
}

dScriptCompiler::dUserVariable dScriptCompiler::LinkParameters(const dUserVariable& parameterA, const dUserVariable& parameterB)
{
	dUserVariable returnNode (parameterA);

	_ASSERTE (parameterA.m_node->GetTypeId() == dDAGParameterNode::GetRttiType());
//	_ASSERTE (!parameterB.m_node || (parameterB.m_node->GetTypeId() == dDAGParameterNode::GetRttiType()));
	_ASSERTE (parameterB.m_node->GetTypeId() == dDAGParameterNode::GetRttiType());
	dDAGParameterNode* param = (dDAGParameterNode*) parameterA.m_node;

	for ( ;param->m_next; param = param->m_next);
	param->m_next = (dDAGParameterNode*) parameterB.m_node;
		
	return returnNode;
}

dScriptCompiler::dUserVariable dScriptCompiler::LinkExpressions(const dUserVariable& ExpressionA, const dUserVariable& ExpressionB)
{
	dUserVariable returnNode (ExpressionA);

	_ASSERTE (ExpressionA.m_node->IsType(dDAGExpressionNode::GetRttiType()));
//	_ASSERTE (!ExpressionB.m_node || (ExpressionB.m_node->IsType(dDAGExpressionNode::GetRttiType())));
	_ASSERTE (ExpressionB.m_node->IsType(dDAGExpressionNode::GetRttiType()));
	dDAGExpressionNode* expression = (dDAGExpressionNode*) ExpressionA.m_node;

	for ( ;expression->m_argumentListNext; expression = expression->m_argumentListNext);
	expression->m_argumentListNext = (dDAGExpressionNode*) ExpressionB.m_node;

	return returnNode;
}



dScriptCompiler::dUserVariable dScriptCompiler::BeginScopeBlock ()
{
	dUserVariable returnNode;
	dDAGScopeBlockNode* const scope = new dDAGScopeBlockNode (m_allNodes);
	m_scopeStack.Append(scope);
	returnNode.m_node = scope;
	return returnNode;
}

dScriptCompiler::dUserVariable dScriptCompiler::EndScopeBlock (const dUserVariable& block)
{
	dUserVariable returnNode;
	_ASSERTE (GetCurrentScope() == block.m_node);
	_ASSERTE (block.m_node->IsType(dDAGScopeBlockNode::GetRttiType()));
	returnNode.m_node = block.m_node;
	m_scopeStack.Remove(m_scopeStack.GetLast());
	return returnNode;
}








dScriptCompiler::dUserVariable dScriptCompiler::NewExpressionNodeVariable (const dUserVariable& identifier, const dUserVariable& dimArray)
{
	dUserVariable returnNode;

	dDAGScopeBlockNode* const scope = GetCurrentScope();
	_ASSERTE (scope);
	dDAGExpressionNodeVariable* const node = scope->CreatedVariableNode (m_allNodes, identifier.m_data.c_str());
	returnNode.m_node = node;

	return returnNode;
}



dScriptCompiler::dUserVariable dScriptCompiler::NewExpressionFunctionCall (const dUserVariable& functionName, const dUserVariable& argumnetList)
{
	dUserVariable returnNode;

	_ASSERTE (argumnetList.m_node->IsType(dDAGExpressionNode::GetRttiType()));
	dDAGExpressionNode* const argument = (dDAGExpressionNode*) argumnetList.m_node;
	dDAGExpressionNodeFunctionCall* const fntCall = new dDAGExpressionNodeFunctionCall(m_allNodes, functionName.m_data.c_str(), argument);
	returnNode.m_node = fntCall;
	return returnNode;
}




void dScriptCompiler::AddStatementToCurrentBlock(const dUserVariable& statement)
{
	if (statement.m_node) {
		_ASSERTE (statement.m_node && statement.m_node->IsType(dDAGFunctionStatement::GetRttiType()));
		dDAGFunctionStatement* const stmt = (dDAGFunctionStatement*) statement.m_node;

		dDAGScopeBlockNode* const block = GetCurrentScope();
		_ASSERTE (block);
		block->AddStatement(stmt);
	}
}


dScriptCompiler::dUserVariable dScriptCompiler::NewLocalVariableStamement(const dUserVariable& variable, const dUserVariable& initializationExpression)
{
	dUserVariable returnNode;

	dDAGParameterNode* const node = (dDAGParameterNode*)variable.m_node;
	_ASSERTE (node->GetTypeId() == dDAGParameterNode::GetRttiType());
	if (initializationExpression.m_node) {
		dDAGExpressionNode* const exp = (dDAGExpressionNode*) initializationExpression.m_node;
		_ASSERTE (exp->IsType(dDAGExpressionNode::GetRttiType()));
		node->SetInitializationExpression(exp);
	}
	returnNode.m_node = node;
	return returnNode;
}




dScriptCompiler::dUserVariable dScriptCompiler::NewReturnStamement(const dUserVariable& expression)
{
	dUserVariable returnNode;

	dDAGExpressionNode* exp = NULL;
	if (expression.m_node) {
		dDAGExpressionNode* const exp = (dDAGExpressionNode*) expression.m_node;
		_ASSERTE (exp->IsType(dDAGExpressionNode::GetRttiType()));
	}

	dDAGFunctionStatementReturn* const stmt = new dDAGFunctionStatementReturn(m_allNodes, exp);
	returnNode.m_node = stmt;
	return returnNode;
}

dScriptCompiler::dUserVariable dScriptCompiler::NewFunctionCallStamement(const dUserVariable& functionExpression)
{
	dUserVariable returnNode;

	dDAGExpressionNodeFunctionCall* const fnt  = (dDAGExpressionNodeFunctionCall*) functionExpression.m_node;
	_ASSERTE (fnt->IsType(dDAGExpressionNodeFunctionCall::GetRttiType()));

	dDAGFunctionStatementFunctionCall* const stmt = new dDAGFunctionStatementFunctionCall(m_allNodes, fnt);
	returnNode.m_node = stmt;
	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::NewExpressionNodeBinaryOperator (const dUserVariable& binaryOperator, const dUserVariable& expressionA, const dUserVariable& expressionB)
{
	dUserVariable returnNode;

	_ASSERTE (expressionA.m_node && expressionA.m_node->IsType (dDAGExpressionNode::GetRttiType()));
	_ASSERTE (expressionB.m_node && expressionB.m_node->IsType(dDAGExpressionNode::GetRttiType()));

	dDAGExpressionNodeBinaryOperator::dBinaryOperator binOperator = dDAGExpressionNodeBinaryOperator::m_add;
	switch (int (binaryOperator.m_token))
	{	
		case '+':
			binOperator = dDAGExpressionNodeBinaryOperator::m_add;
			break;

		case '-':
			binOperator = dDAGExpressionNodeBinaryOperator::m_sub;
			break;

		case '*':
			binOperator = dDAGExpressionNodeBinaryOperator::m_mul;
			break;

		case '/':
			binOperator = dDAGExpressionNodeBinaryOperator::m_div;
			break;

		case '<':
			binOperator = dDAGExpressionNodeBinaryOperator::m_less;
			break;

		case '>':
			binOperator = dDAGExpressionNodeBinaryOperator::m_greather;
			break;

		case LESS_EQUAL:
			binOperator = dDAGExpressionNodeBinaryOperator::m_lessEqual;
			break;

		case GREATHER_EQUAL:
			binOperator = dDAGExpressionNodeBinaryOperator::m_greatherEqual;
			break;

		case IDENTICAL:
			binOperator = dDAGExpressionNodeBinaryOperator::m_identical;
			break;

		case DIFFERENT:
			binOperator = dDAGExpressionNodeBinaryOperator::m_different;
			break;

		

		default:;
			_ASSERTE (0);

	}


	dDAGScopeBlockNode* const scope = GetCurrentScope();
	_ASSERTE (scope);

	dDAGExpressionNodeBinaryOperator* const node = scope->CreateBinaryOperatorNode(m_allNodes, binOperator, (dDAGExpressionNode*)expressionA.m_node, (dDAGExpressionNode*)expressionB.m_node);
	returnNode.m_node = node;
	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::NewExpressionNodeUnuaryOperator (const dUserVariable& unuaryOperator, const dUserVariable& expression)
{

	dUserVariable returnNode;

	_ASSERTE (expression.m_node && expression.m_node->IsType(dDAGExpressionNode::GetRttiType()));

	dDAGExpressionNodeUnuaryOperator::dUnuaryOperator unuOperator = dDAGExpressionNodeUnuaryOperator::m_minus;
	switch (int (unuaryOperator.m_token))
	{	
		case '-':
			unuOperator = dDAGExpressionNodeUnuaryOperator::m_minus;
			break;

		case '~':
			unuOperator = dDAGExpressionNodeUnuaryOperator::m_notBinay;
			break;

		case '!':
			unuOperator = dDAGExpressionNodeUnuaryOperator::m_notLogical;
			break;

		default:;
			_ASSERTE (0);

	}

	dDAGScopeBlockNode* const scope = GetCurrentScope();
	_ASSERTE (scope);

	dDAGExpressionNodeUnuaryOperator* const node = scope->CreateUnuaryOperatorNode(m_allNodes, unuOperator, (dDAGExpressionNode*)expression.m_node);
	returnNode.m_node = node;
	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::EmitTypeNode (const dUserVariable& type, const dUserVariable& modifier)
{
	dUserVariable returnNode;
	dDAGTypeNode* const typeNode = new dDAGTypeNode (m_allNodes, type.m_data.c_str(), modifier.m_data.c_str());
	returnNode.m_node = typeNode;
	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::NewDimensionNode(const dUserVariable& expression)
{
	dUserVariable returnNode;

	dDAGExpressionNode* const exp = (dDAGExpressionNode*)expression.m_node;
	_ASSERTE (!exp || (exp->IsType (dDAGExpressionNode::GetRttiType())));
	dDAGDimensionNode* const node = new dDAGDimensionNode (m_allNodes, exp);
	returnNode.m_node = node;
	return returnNode;
}

dScriptCompiler::dUserVariable dScriptCompiler::TypeAddDimention(const dUserVariable& type, const dUserVariable& dimension)
{
	dUserVariable returnNode(type);

	dDAGTypeNode* const typeNode = (dDAGTypeNode*) type.m_node;
	_ASSERTE (typeNode->IsType(dDAGTypeNode::GetRttiType()));

	dDAGDimensionNode* const dim = (dDAGDimensionNode*) dimension.m_node;
	_ASSERTE (dim->IsType(dDAGDimensionNode::GetRttiType()));

	typeNode->AddDimensions(dim);

	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::NewIFStamement(const dUserVariable& expression, const dUserVariable& thenBlock, const dUserVariable& elseBlock)
{
	dUserVariable returnNode;

	dDAGExpressionNode* const exp = (dDAGExpressionNode*) expression.m_node;
	dDAGFunctionStatement* const thenStmt = (dDAGFunctionStatement*) thenBlock.m_node;
	dDAGFunctionStatement* const elseStmt = (dDAGFunctionStatement*) elseBlock.m_node;
	_ASSERTE (exp->IsType(dDAGExpressionNode::GetRttiType()));
	_ASSERTE (thenStmt->IsType(dDAGFunctionStatement::GetRttiType()));
	_ASSERTE (!elseStmt || (elseStmt->IsType(dDAGFunctionStatement::GetRttiType())));

	dDAGFunctionStatementIF* const stmt = new dDAGFunctionStatementIF(m_allNodes, exp, thenStmt, elseStmt);
	returnNode.m_node = stmt;
	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::NewForStamement(const dUserVariable& forScope, const dUserVariable& init_exp, const dUserVariable& conditional, const dUserVariable& step_Exp, const dUserVariable& statement)
{
	dUserVariable returnNode;

	dDAGExpressionNode* const exp = (dDAGExpressionNode*) conditional.m_node;
	dDAGFunctionStatement* const firstStmt = (dDAGFunctionStatement*) init_exp.m_node;
	dDAGFunctionStatement* const thirdStmt = (dDAGFunctionStatement*) step_Exp.m_node;
	dDAGFunctionStatement* const forStmt = (dDAGFunctionStatement*) statement.m_node;
	_ASSERTE (!exp || exp->IsType(dDAGExpressionNode::GetRttiType()));
	_ASSERTE (!firstStmt || firstStmt->IsType(dDAGFunctionStatement::GetRttiType()));
	_ASSERTE (!thirdStmt || thirdStmt->IsType(dDAGFunctionStatement::GetRttiType()));
	_ASSERTE (!forStmt || forStmt->IsType(dDAGFunctionStatement::GetRttiType()));
	dDAGFunctionStatementFOR* const stmt = new dDAGFunctionStatementFOR(m_allNodes, firstStmt, exp, thirdStmt, forStmt);
	returnNode.m_node = stmt;

	AddStatementToCurrentBlock(returnNode);
	returnNode.m_node = GetCurrentScope();
	return EndScopeBlock (returnNode);
}


dScriptCompiler::dUserVariable dScriptCompiler::NewFunctionPrototype (const dUserVariable& returnType, const dUserVariable& functionName, const dUserVariable& parameterList, const dUserVariable& isConst)
{
	dUserVariable returnNode;

	dDAGTypeNode* const type = (dDAGTypeNode*) returnType.m_node;
	_ASSERTE (type->IsType(dDAGTypeNode::GetRttiType()));

	dDAGParameterNode* const parameters = (dDAGParameterNode*)parameterList.m_node;
	_ASSERTE (!parameters || parameters->IsType(dDAGParameterNode::GetRttiType()));

	dDAGFunctionNode* const function = new dDAGFunctionNode(m_allNodes, type, functionName.m_data.c_str(), parameters, isConst.m_data.c_str());

//	GetCurrentClass()->AddFunction(function);

	returnNode.m_node = function;
	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::NewConstructorPrototype (const dUserVariable& functionName, const dUserVariable& parameterList, const dUserVariable& initBaseClass)
{
	dUserVariable typeVar;
	typeVar.m_node = new dDAGTypeNode (m_allNodes, "void", "");

	dUserVariable returnNode (NewFunctionPrototype (typeVar, functionName, parameterList, dUserVariable()));

	dDAGFunctionNode* const function = (dDAGFunctionNode*) returnNode.m_node;
	function->m_isConstructor = true;

	_ASSERTE (!initBaseClass.m_node);

	return returnNode;
}

void dScriptCompiler::AddClassVariable(const dUserVariable& variable)
{
	_ASSERTE (variable.m_node);
	_ASSERTE (variable.m_node->GetTypeId() == dDAGParameterNode::GetRttiType());
	dDAGParameterNode* const var = (dDAGParameterNode*) variable.m_node;
	GetCurrentClass()->AddVariable(var);
}

void dScriptCompiler::AddClassFunction(const dUserVariable& function)
{
	_ASSERTE (function.m_node);
	_ASSERTE (function.m_node->GetTypeId() == dDAGFunctionNode::GetRttiType());
	dDAGFunctionNode* const fnt = (dDAGFunctionNode*) function.m_node;
	GetCurrentClass()->AddFunction(fnt);
}



dScriptCompiler::dUserVariable dScriptCompiler::NewClassFunction (const dUserVariable& modifier, const dUserVariable& functionProtype, const dUserVariable& functionBody)
{
	dUserVariable returnNode;

	dDAGFunctionNode* const function = (dDAGFunctionNode*) functionProtype.m_node;
	_ASSERTE (function && function->IsType(dDAGFunctionNode::GetRttiType()));

	if (functionBody.m_node) {
		dDAGScopeBlockNode* const block = (dDAGScopeBlockNode*) functionBody.m_node;
		_ASSERTE (block->IsType(dDAGScopeBlockNode::GetRttiType()));
		function->SetBody(block);
	}

	if (modifier.m_node) {
		dDAGFunctionModifier* const mod = (dDAGFunctionModifier*) modifier.m_node;
		_ASSERTE (mod->IsType(dDAGFunctionModifier::GetRttiType()));
		function->SetModifier (mod);
	}

	function->CalculateKey();

	returnNode.m_node = function;
	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::NewFunctionModifier (const dUserVariable& modifier, const dUserVariable& appendModifirer)
{
	dUserVariable returnNode;
	
	dDAGFunctionModifier* mod = (dDAGFunctionModifier*)modifier.m_node;
	if (!mod) {
		mod = new dDAGFunctionModifier(m_allNodes) ;
	}
	_ASSERTE (mod->IsType(dDAGFunctionModifier::GetRttiType()));

	if (appendModifirer.m_token == PRIVATE) {
		mod->m_private = true;
	}
	if (appendModifirer.m_token == NATIVE) {
		mod->m_native = true;
	}
	returnNode.m_node = mod;
	return returnNode;
}
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
	if (xxx == 77)
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
	OutputDebugStringA (text);

	free (text);
}


int dScriptCompiler::CompileSource (const char* const source)
{
	dNewtonScriptLexical scanner (source);

//	m_virtualMachine = virtualMachine;
	Parse(scanner);
	return 0;
}


bool dScriptCompiler::Parse(dNewtonScriptLexical& scanner)
{
//	m_codeSegmentSize = 0;
//	m_codeSegment = new short[D_CODE_SEGMNET_ALLOC_SHUNK_SIZE];


	dNewtonScriptParser::Parse(scanner);

//	scanner.ReStartScanner();
//	dAssemblerParser::Parse(scanner);

	return true;
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






void dScriptCompiler::SetParamameterAsPrivateVariable(const dUserVariable& variable)
{
//	dUserVariable returnNode;
	_ASSERTE (0);
/*
	_ASSERTE (m_currentClass);
	_ASSERTE (m_currentFunction);

	_ASSERTE (variable.m_node);
	_ASSERTE (variable.m_node->GetTypeId() ==  dDAGParameterNode::GetRttiType());
	dDAGParameterNode* const tmp = (dDAGParameterNode*) variable.m_node;
	tmp->m_isPublic = false;
*/
}







dScriptCompiler::dUserVariable dScriptCompiler::NewExpressionNodeVariable (const dUserVariable& identifier)
{
	dUserVariable returnNode;
	_ASSERTE (0);
/*
	_ASSERTE (m_currentFunction);
	dDAGScopeBlockNode* const block = m_currentFunction->GetCurrentBlock();
	dDAGExpressionNodeVariable* const node = block->CreatedVariableNode (identifier.m_data.c_str());
	returnNode.m_node = node;
*/
	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::NewExpressionNodeConstant (const dUserVariable& value)
{
	dUserVariable returnNode;
	_ASSERTE (0);
/*
	_ASSERTE (m_currentFunction);
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

	dDAGScopeBlockNode* const block = m_currentFunction->GetCurrentBlock();
	dDAGExpressionNodeConstant* const node = block->CreatedConstantNode(type, value.m_data.c_str());
	returnNode.m_node = node;
*/
	return returnNode;
}

dScriptCompiler::dUserVariable dScriptCompiler::NewExpressionNodeBinaryOperator (const dUserVariable& binaryOperator, const dUserVariable& expressionA, const dUserVariable& expressionB)
{
	dUserVariable returnNode;
	_ASSERTE (0);
/*
	_ASSERTE (m_currentFunction);
	
	_ASSERTE (expressionA.m_node);
	_ASSERTE (expressionB.m_node);
	_ASSERTE (expressionA.m_node->IsType (dDAGExpressionNode::GetRttiType()));
	_ASSERTE (expressionB.m_node->IsType(dDAGExpressionNode::GetRttiType()));

	dDAGExpressionNodeBinaryOperator::dBinaryOperator binOperator = dDAGExpressionNodeBinaryOperator::m_add;
	switch (int (binaryOperator.m_token))
	{	
		case '=':
			binOperator = dDAGExpressionNodeBinaryOperator::m_equal;
			break;

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

		case IDENTICAL:
			binOperator = dDAGExpressionNodeBinaryOperator::m_identical;
			break;

		case DIFFERENT:
			binOperator = dDAGExpressionNodeBinaryOperator::m_different;
			break;


		default:;
			_ASSERTE (0);

	}

	dDAGScopeBlockNode* const block = m_currentFunction->GetCurrentBlock();
	dDAGExpressionNodeBinaryOperator* const node = block->CreateBinaryOperatorNode(binOperator, (dDAGExpressionNode*)expressionA.m_node, (dDAGExpressionNode*)expressionB.m_node);
	expressionA.m_node->Release();
	expressionB.m_node->Release();

	returnNode.m_node = node;
*/
	return returnNode;
}

dScriptCompiler::dUserVariable dScriptCompiler::NewExpresionNodeAssigment (const dUserVariable& leftVariable, const dUserVariable& expression)
{
	_ASSERTE (0);
	dUserVariable binaryOperator;
	binaryOperator.m_token = dToken('=');

	dScriptCompiler::dUserVariable leftSideVar (NewExpressionNodeVariable (leftVariable));
	dScriptCompiler::dUserVariable returnNode (NewExpressionNodeBinaryOperator (binaryOperator, leftSideVar, expression));
	return returnNode;
}



void dScriptCompiler::AddLocalVaribleToCurrentBlock(const dUserVariable& variable, const dUserVariable& initExpression)
{
	_ASSERTE (0);
/*
	_ASSERTE (m_currentClass);
	_ASSERTE (m_currentFunction);

	_ASSERTE (variable.m_node);
	_ASSERTE (variable.m_node->GetTypeId() ==  dDAGParameterNode::GetRttiType());
	dDAGParameterNode* const var = (dDAGParameterNode*) variable.m_node;

	dDAGScopeBlockNode* const block = m_currentFunction->GetCurrentBlock();
	block->AddStatement(var);
	var->Release();

	if(initExpression.m_node) {
		dUserVariable tmp;
		tmp.m_data = var->m_name;
		dUserVariable assigmentStatement (NewExpresionNodeAssigment (tmp, initExpression));
		AddStatementToCurrentBlock(assigmentStatement);
	}
*/
}

void dScriptCompiler::AddStatementToCurrentBlock(const dUserVariable& statement)
{
	_ASSERTE (0);
/*
	dUserVariable returnNode;

	_ASSERTE (m_currentClass);
	_ASSERTE (m_currentFunction);

	_ASSERTE (statement.m_node);
	_ASSERTE (statement.m_node->IsType(dDirectAcyclicgraphNode::GetRttiType()));
	dDirectAcyclicgraphNode* const stmnt = (dDAGParameterNode*) statement.m_node;

	dDAGScopeBlockNode* const block = m_currentFunction->GetCurrentBlock();
	block->AddStatement(stmnt);
	stmnt->Release();
*/
}
void dScriptCompiler::AddStatementIFToCurrentBlock(const dUserVariable& expression, const dUserVariable& thenBlock, const dUserVariable& elseBlock)
{
	_ASSERTE (0);
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


dScriptCompiler::dUserVariable dScriptCompiler::EmitTypeNode (const dUserVariable& type, const dUserVariable& modifier)
{
	dUserVariable returnNode;
	dDAGTypeNode* const typeNode = new dDAGTypeNode (m_allNodes, type.m_data.c_str(), modifier.m_data.c_str());
	returnNode.m_node = typeNode;
	return returnNode;
}





void dScriptCompiler::AddClassVariable(const dUserVariable& variable)
{
	_ASSERTE (variable.m_node);
	_ASSERTE (variable.m_node->GetTypeId() ==  dDAGParameterNode::GetRttiType());
	dDAGParameterNode* const var = (dDAGParameterNode*) variable.m_node;
	GetCurrentClass()->AddVariable(var);
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

	_ASSERTE (parameterA.m_node->GetTypeId() ==  dDAGParameterNode::GetRttiType());
	_ASSERTE (!parameterB.m_node || (parameterB.m_node->GetTypeId() ==  dDAGParameterNode::GetRttiType()));
	dDAGParameterNode* param = (dDAGParameterNode*) parameterA.m_node;

	for ( ;param->m_next; param = param->m_next);
	param->m_next = (dDAGParameterNode*) parameterB.m_node;
		
	return returnNode;
}


/*
dScriptCompiler::dUserVariable dScriptCompiler::BeginBeginFunctionPrototypeNode ()
{
	dUserVariable returnNode;
	_ASSERTE (0);
	_ASSERTE (m_currentClass);
	dDAGFunctionNode* const functionNode = new dDAGFunctionNode ();
	returnNode.m_node = functionNode;
	m_currentFunction = functionNode;
	return returnNode;
}

dScriptCompiler::dUserVariable dScriptCompiler::FinalizePrototype (const dUserVariable& returnType, const dUserVariable& functionName, const dUserVariable& isConst)
{
	dUserVariable returnNode;
	_ASSERTE (0);
	_ASSERTE (returnType.m_node && (returnType.m_node->GetTypeId() == dDAGTypeNode::GetRttiType()));
	_ASSERTE (returnType.m_node);
	m_currentFunction->FinalizePrototype ((dDAGTypeNode*)returnType.m_node, functionName.m_data.c_str(), isConst.m_data.c_str());
	returnNode.m_node = m_currentFunction;
	return returnNode;
}
*/


/*
dScriptCompiler::dUserVariable dScriptCompiler::BeginScopeBlock ()
{
	dUserVariable returnNode;
	_ASSERTE (0);

	_ASSERTE (m_currentClass);
	_ASSERTE (m_currentFunction);

	dDAGScopeBlockNode* const scope = new dDAGScopeBlockNode;
	returnNode.m_node = scope;

	m_currentFunction->PushScope(scope);
	scope->Release();

	return returnNode;
}

dScriptCompiler::dUserVariable dScriptCompiler::FinalizeScopeBlock (const dUserVariable& scope)
{
	dUserVariable returnNode;
	_ASSERTE (0);

	_ASSERTE (m_currentClass);
	_ASSERTE (m_currentFunction);
	m_currentFunction->PopScope();
	returnNode.m_node = scope.m_node;

	return returnNode;
}
*/

dScriptCompiler::dUserVariable dScriptCompiler::NewScopeBlock (const dUserVariable& statementsList)
{
	dUserVariable returnNode;

	dDAGFunctionStatement* const statements = (dDAGFunctionStatement*) statementsList.m_node;
	_ASSERTE (!statements || statements->IsType(dDAGFunctionStatement::GetRttiType()));
	dDAGScopeBlockNode* const scope = new dDAGScopeBlockNode (m_allNodes, statements);

	returnNode.m_node = scope;
	return returnNode;
}


dScriptCompiler::dUserVariable dScriptCompiler::NewFunctionPrototype (const dUserVariable& returnType, const dUserVariable& functionName, const dUserVariable& parameterList, const dUserVariable& isConst)
{
	dUserVariable returnNode;

	dDAGTypeNode* const type = (dDAGTypeNode*) returnType.m_node;
	_ASSERTE (type->IsType(dDAGTypeNode::GetRttiType()));

	dDAGParameterNode* const parameters = (dDAGParameterNode*)parameterList.m_node;
	_ASSERTE (parameters->IsType(dDAGParameterNode::GetRttiType()));

	dDAGFunctionNode* const function = new dDAGFunctionNode(m_allNodes, type, functionName.m_data.c_str(), parameters, isConst.m_data.c_str());

	GetCurrentClass()->AddFunction(function);

	returnNode.m_node = function;
	return returnNode;
}



dScriptCompiler::dUserVariable dScriptCompiler::AddClassFunction (const dUserVariable& isPrivate, const dUserVariable& function, const dUserVariable& functionBlock)
{
	dUserVariable returnNode;

	dDAGScopeBlockNode* const block = (dDAGScopeBlockNode*) functionBlock.m_node;
	_ASSERTE (!block || block->IsType(dDAGScopeBlockNode::GetRttiType()));

	dDAGFunctionNode* const fnct = (dDAGFunctionNode*) function.m_node;
	_ASSERTE (fnct && fnct->IsType(dDAGFunctionNode::GetRttiType()));

	fnct->m_isPrivate = (isPrivate.m_data == "") ? false : true;
	fnct->CalculateKey();
	fnct->SetBody(block);

	returnNode.m_node = fnct;
	return returnNode;
}


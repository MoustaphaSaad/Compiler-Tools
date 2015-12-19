#include "RecursiveDescent.hpp"
#include <assert.h>
#include <iostream>
#include "Services.hpp"
using namespace CTools;
using namespace std;

void RecursiveDescent::error(std::string msg){
	throw runtime_error("Error: "+msg);
}

void RecursiveDescent::rassert(bool exp, string msg){
	if(!exp){
		error(msg);
	}
}


std::vector<std::string> RecursiveDescent::getErrors()
{
	return m_errors;
}

PNode* RecursiveDescent::match(std::string tag)
{
	if(m_cursor->tag == tag)
	{
		PNode* res = Services::memory->alloc<PNode>();
		new (res) PNode();
		res->Tag = m_cursor->tag;
		res->Value = m_cursor->lexeme;
		m_cursor++;
		return res;
	}else
	{
		//error
		error("Cannot match token, expected '"+tag+"' but found '"+m_cursor->tag+"'");
		return nullptr;
	}
}

RecursiveDescent::RecursiveDescent()
{

}

RecursiveDescent::~RecursiveDescent()
{

}

PNode* RecursiveDescent::Parse(TokenStream& stream)
{
	PNode* programNode = nullptr;
	//parse the code
	m_errors.clear();
	m_begin = stream.begin();
	m_end = stream.end();
	m_cursor = m_begin;
	programNode = program();
	return programNode;
}

/*
	program = stmt-sequence;
*/
PNode* RecursiveDescent::program()
{
	//Node stmt_seq = stmt_seq(walker,stream);
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "program";
	
	auto stmt_seq = stmt_sequence();
	rassert(stmt_seq != nullptr, "unable to match program, expected 'stmt-sequence' Node.");
	res->addChild(stmt_seq);
	return res;

}

/*
	stmt-sequence = stmt-sequence; statement | statement
	stmt-sequence = statement {; statement}
*/
PNode* RecursiveDescent::stmt_sequence()
{
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "";
	
	auto stmt = statement();
	rassert(stmt != nullptr, "unable to match stmt-sequence, expected 'statement' Node.");
	res->addChild(stmt);
	if(m_cursor != m_end){
		while(m_cursor != m_end && m_cursor->tag == "SEMICOLON")
		{
			auto semicolon = match("SEMICOLON");
			rassert(semicolon != nullptr, "unable to match token, expected 'SEMICOLON' Token.");
			res->addChild(semicolon);
			auto stmt2 = statement();
			rassert(stmt2 != nullptr, "unable to match stmt-sequence, expected 'statement' Node.");
			res->addChild(stmt2);
		}
	}
	return res;
}

/*
	statement = if-stmt | repeat-stmt | assign-stmt | read-stmt | write-stmt
*/
PNode* RecursiveDescent::statement()
{
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "";

	
	if(m_cursor->tag == "IF")
	{
		auto ifNode = if_stmt();
		rassert(ifNode != nullptr, "unable to match statement, expected 'if-stmt' node.");
		res->addChild(ifNode);
	}else if(m_cursor->tag == "REPEAT")
	{
		auto repeatNode = repeat_stmt();
		rassert(repeatNode != nullptr, "unable to match statement, expected 'repeat-stmt' node.");
		res->addChild(repeatNode);
	}else if(m_cursor->tag == "ID")
	{
		auto assignNode = assign_stmt();
		rassert(assignNode != nullptr, "unable to match statement, expected 'assign-stmt' node.");
		res->addChild(assignNode);
	}else if(m_cursor->tag == "READ")
	{
		auto readNode = read_stmt();
		rassert(readNode != nullptr, "unable to match statement, expected 'read-stmt' Node.");
		res->addChild(readNode);
	}else if(m_cursor->tag == "WRITE")
	{
		auto writeNode = write_stmt();
		rassert(writeNode != nullptr, "unable to match statement, expected 'write-stmt' node.");
		res->addChild(writeNode);
	}else{
		return nullptr;
	}

	return res;
}

/*
	if-stmt = if exp then stmt-sequence end |
			  if exp then stmt-sequence else stmt_sequence end
*/

PNode* RecursiveDescent::if_stmt(){
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "if-stmt";

	auto ifNode = match("IF");
	rassert(ifNode != nullptr, "unable to match if-stmt, expected 'IF' token.");
	res->addChild(ifNode);

	auto expNode = exp();
	rassert(expNode != nullptr, "unable to match if-stmt, expected 'exp' node.");
	res->addChild(expNode);

	auto thenNode = match("THEN");
	rassert(thenNode != nullptr, "unable to match if-stmt, expected 'THEN' token.");
	res->addChild(thenNode);

	auto stmt_seqNode = stmt_sequence();
	rassert(stmt_seqNode != nullptr, "unable to match if-stmt, expected 'stmt-sequence' node.");
	res->addChild(stmt_seqNode);

	if(m_cursor->tag == "ELSE"){
		auto elseNode = match("ELSE");
		rassert(elseNode != nullptr, "unable to match if-stmt, expected 'ELSE' token.");
		res->addChild(elseNode);

		auto stmt_seqNode2 = stmt_sequence();
		rassert(stmt_seqNode2 != nullptr, "unable to match if-stmt, expected 'stmt-sequence' node.");
		res->addChild(stmt_seqNode2);

		auto endNode = match("END");
		rassert(endNode != nullptr, "unable to match if-stmt, expected 'END' token.");
		res->addChild(endNode);
	}else{
		auto endNode = match("END");
		rassert(endNode != nullptr, "unable to match if-stmt, expected 'END' token.");
		res->addChild(endNode);
	}

	return res;
}


/*
	repeat-stmt = repeat stmt-sequence until exp
*/

PNode* RecursiveDescent::repeat_stmt(){
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "repeat-stmt";

	auto repeatToken = match("REPEAT");
	rassert(repeatToken != nullptr, "unable to match repeat-stmt, expected 'REPEAT' token.");
	res->addChild(repeatToken);

	auto stmt_seqNode = stmt_sequence();
	rassert(stmt_seqNode != nullptr, "unable to match repeat-stmt, expected 'stmt-sequence' node.");
	res->addChild(stmt_seqNode);

	auto untilNode = match("UNTIL");
	rassert(untilNode != nullptr, "unable to match repeat-stmt, expected 'UNTIL' token.");
	res->addChild(untilNode);

	auto expNode = exp();
	rassert(expNode != nullptr, "unable to match repeat-stmt, expected 'exp' node.");
	res->addChild(expNode);

	return res;
}

/*
	write-stmt = write exp
*/

PNode* RecursiveDescent::write_stmt(){
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "write-stmt";

	auto writeNode = match("WRITE");
	rassert(writeNode != nullptr, "unable to match write-stmt, expected 'WRITE' token.");
	res->addChild(writeNode);
	auto expNode = exp();
	rassert(expNode != nullptr, "unabel to match write-stmt, expected exp node.");
	res->addChild(expNode);

	return res;
}

/*
	read-stmt = READ ID
*/
PNode* RecursiveDescent::read_stmt()
{
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "read-stmt";

	
	auto read = match("READ");
	rassert(read != nullptr, "unable to match token, expected 'READ' token.");
	res->addChild(read);
	auto id = match("ID");
	rassert(id != nullptr, "unable to match token, expected 'ID' token.");
	res->addChild(id);
	return res;
}

/*
	assign-stmt = ID ':=' exp
*/
PNode* RecursiveDescent::assign_stmt(){

	auto id = match("ID");
	rassert(id != nullptr,"unable to match token, expected 'ID' token.");

	auto res = match("ASSIGNMENT_OP");
	rassert(res != nullptr,"unable to match token, expected 'ASSIGNMENT_OP' token.");

	auto expNode = exp();
	rassert(expNode != nullptr, "unable to match assign-stmt, expected 'exp' node.");
	
	res->addChild(id);
	res->addChild(expNode);
	return res;
}

/*
	exp = simple-exp comparison-op simple-exp | simple-exp
*/
PNode* RecursiveDescent::exp(){
	PNode* res;

	auto simpleNode = simple_exp();
	rassert(simpleNode != nullptr, "unable to match exp, expected 'simple-exp' node.");

	if(m_cursor != m_end){
		if(m_cursor->tag == "LESS_THAN" || m_cursor->tag == "EQUAL"){

			res = comparison_op();
			rassert(res != nullptr, "unable to match exp, expected 'comparison-op' node.");

			auto simpleNode2 = simple_exp();
			rassert(simpleNode2 != nullptr, "unable to match exp, expected 'simple-exp' node.");

			res->addChild(simpleNode);
			res->addChild(simpleNode2);
		}else
		{
			res = simpleNode;
		}
	}else
	{
		res = simpleNode;
	}

	return res;
}

/*
	comparison-op = '<' | '='
*/
PNode* RecursiveDescent::comparison_op(){
	PNode* res;
	if(m_cursor->tag == "LESS_THAN"){
		auto less = match("LESS_THAN");
		rassert(less != nullptr,"unable to match comparison-op, expected '<' token.");
		res = less;
	}else if(m_cursor->tag == "EQUAL"){
		auto eql = match("EQUAL");
		rassert(eql != nullptr,"unable to match comparison-op, expected '=' token.");
		res = eql;
	}else{
		rassert(false,"unable to match comparison-op, expected '<|=' token.");
		return nullptr;
	}
	return res;
}

/*
	addop = + | -
*/
PNode* RecursiveDescent::add_op(){
	PNode* res;
	if(m_cursor->tag == "ADD_OP"){
		auto less = match("ADD_OP");
		rassert(less != nullptr,"unable to match comparison-op, expected '+' token.");
		res = less;
	}else if(m_cursor->tag == "MINUS_OP"){
		auto eql = match("MINUS_OP");
		rassert(eql != nullptr,"unable to match comparison-op, expected '-' token.");
		res = eql;
	}else{
		rassert(false,"unable to match comparison-op, expected '+|-' token.");
		return nullptr;
	}
	return res;
}

/*
	mulop = *
*/
PNode* RecursiveDescent::mul_op(){
	PNode* res;
	auto mul = match("MULTIPLY_OP");
	rassert(mul != nullptr, "unable to match mulop, expected '*' token.");
	res = mul;
	return res;
}

/*
	factor = (exp) | number | identifier
*/

PNode* RecursiveDescent::factor(){
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "";
	if(m_cursor->tag == "ID"){
		auto id = match("ID");
		rassert(id != nullptr, "unable to match factor, expected 'ID' token.");
		res->addChild(id);
	}else if(m_cursor->tag == "NUMBER"){
		auto number = match("NUMBER");
		rassert(number != nullptr, "unable to match factor, expected 'NUMBER' token.");
		res->addChild(number);
	}else if(m_cursor->tag == "OPEN_PAREN"){
		auto open_paren = match("OPEN_PAREN");
		rassert(open_paren != nullptr, "unable to match factor, expected '(' token.");
		auto expNode = exp();
		rassert(expNode != nullptr, "unable to match factor, expected 'exp' Node.");
		auto close_paren = match("CLOSE_PAREN");
		rassert(close_paren != nullptr, "unable to match factor, expected ')' token.");

		res->addChild(open_paren);
		res->addChild(expNode);
		res->addChild(close_paren);
	}else{
		rassert(false,"unable to match factor, expected '(|Number|ID' token.");
	}
	return res;
}

/*
	term = term mulop factor | factor
	term = factor {mulop factor}
*/

PNode* RecursiveDescent::term(){
	PNode* res=nullptr;

	auto factorNode = factor();
	rassert(factorNode != nullptr, "unable to match term, expected 'factor' node.");

	if(m_cursor != m_end){
		while(m_cursor != m_end && m_cursor->tag == "MULTIPLY_OP"){
			auto mulop = mul_op();
			rassert(mulop != nullptr, "unable to match term, expected '*' token.");

			auto factorNode2 = factor();
			rassert(factorNode2 != nullptr, "unable to match term, expected 'factor' node.");

			res = mulop;
			res->addChild(factorNode);
			res->addChild(factorNode2);
		}
		if(res == nullptr)
			res = factorNode;
	}else
	{
		res = factorNode;
	}
	return res;
}

/*
	simple-exp = simple-exp addop term | term
	simple-exp = term {addop term}
*/
PNode* RecursiveDescent::simple_exp(){
	PNode* res = nullptr;

	auto termNode = term();
	rassert(termNode != nullptr, "unable to match simple-exp, expected 'term' node.");
	
	if(m_cursor != m_end){
		while(m_cursor != m_end && m_cursor->tag == "MINUS_OP" || m_cursor->tag == "ADD_OP"){
			auto addNode = add_op();
			rassert(addNode != nullptr, "unable to match simple-exp, expected 'addop' node.");

			auto termNode2 = term();
			rassert(termNode2 != nullptr, "unable to match simple-exp, expected 'term' node.");

			res= addNode;
			res->addChild(termNode);
			res->addChild(termNode2);
		}
		if(res == nullptr)
			res = termNode;
	}else
	{
		res = termNode;
	}
	return res;
}


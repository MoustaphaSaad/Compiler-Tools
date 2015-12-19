#include "SLRParser.hpp"
#include<stack>
#include <iostream>
#include <Services.hpp>
using namespace std;
using namespace CTools;

stackElement SLRParser::move(stackElement element, std::string str)
{
	stackElement s;
	s.num = -1;
	for(int i=0;i<m_table.table.size();i++)
	{
		if(m_table.table[i].col.number == element.num &&
			m_table.table[i].row.id == str)
		{
			s.num = m_table.table[i].col.number;
			s.cell = m_table.table[i];
			break;
		}
	}
	return s;
}

PNode* SLRParser::reduce(std::stack<stackElement>& parsing_stack, std::string id)
{
	if(id == "Readstmt")
	{
		PNode* res = reduceReadstmt(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}
	else if(id == "Statement")
	{
		PNode* res = reduceStatement(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}
	else if(id == "StSeq")
	{
		PNode* res = reduceStSeq(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "Factor")
	{
		PNode* res = reduceFactor(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "Mulop")
	{
		PNode* res = reduceMulop(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "Term")
	{
		PNode* res = reduceTerm(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "Addop")
	{
		PNode* res = reduceAddop(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "Comparison-op")
	{
		PNode* res = reduceComparison(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "Simple-exp")
	{
		PNode* res = reduceSimpleExp(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "Exp")
	{
		PNode* res = reduceExp(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "Assignstmt")
	{
		PNode* res = reduceAssignstmt(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "Writestmt")
	{
		PNode* res = reduceWritestmt(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "Repeatstmt")
	{
		PNode* res = reduceRepeatstmt(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "IFstmt")
	{
		PNode* res = reduceIFstmt(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}else if(id == "Program")
	{
		PNode* res = reduceProgram(parsing_stack);
		stackElement e;
		e.str = id;
		parsing_stack.push(e);
		return res;
	}
	else
	{
		throw runtime_error("Cannot reduce to "+id);
	}
}

PNode* SLRParser::reduceReadstmt(std::stack<stackElement>& parsing_stack)
{
	PNode* id = reduceID(parsing_stack);
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "read-stmt";
	res->addChild(id);
	parsing_stack.pop();
	return res;
}

PNode* SLRParser::reduceID(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "ID";
	res->Value = e.token.lexeme;
	parsing_stack.pop();
	return res;
}

PNode* SLRParser::reduceMatch(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = e.token.tag;
	res->Value = e.token.lexeme;
	parsing_stack.pop();
	return res;
}

PNode* SLRParser::reduceStatement(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "";
	res->addChild(parsing_stack.top().node);
	parsing_stack.pop();
	return res;
}

PNode* SLRParser::reduceStSeq(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "StSeq";
	//statement
	auto q_stmt = parsing_stack.top().node;
	parsing_stack.pop();
	if(parsing_stack.top().cell.row.id == "SEMICOLON"){
		//semicolon
		//res->addChild(reduceMatch(parsing_stack));
		auto q_semicolon = reduceMatch(parsing_stack);
		//stseq
		auto q_stseq = parsing_stack.top().node;
		parsing_stack.pop();

		res->addChild(q_stseq);
		res->addChild(q_stmt);
	}else{
		res->addChild(q_stmt);
	}
	return res;
}

PNode* SLRParser::reduceFactor(std::stack<stackElement>& parsing_stack)
{
	stackElement e= parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "Factor";

	if(e.cell.row.id == "OPEN_PAREN")
	{
		//OPEN_PAREN
		//res->addChild(reduceMatch(parsing_stack));
		auto q_openparen = reduceMatch(parsing_stack);
		//Exp
		res->addChild(parsing_stack.top().node);
		parsing_stack.pop();
		//CLOSE_PAREN
		//res->addChild(reduceMatch(parsing_stack));
		auto q_closeparen = reduceMatch(parsing_stack);
	}else if(e.cell.row.id == "NUMBER")
	{
		//NUMBER
		res->addChild(reduceMatch(parsing_stack));
	}else if(e.cell.row.id == "ID")
	{
		//ID
		res->addChild(reduceID(parsing_stack));
	}
	return res;
}

PNode* SLRParser::reduceMulop(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "Mulop";

	//MULTIPLY_OP
	res->addChild(reduceMatch(parsing_stack));

	return res;
}

PNode* SLRParser::reduceTerm(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "Term";

	//Factor
	auto q_factor = parsing_stack.top().node;
	//res->addChild(parsing_stack.top().node);
	parsing_stack.pop();
	if(parsing_stack.top().cell.row.id == "Mulop")
	{
		//Mulop
		auto q_mulop = parsing_stack.top().node;
		//res->addChild(parsing_stack.top().node);
		parsing_stack.pop();
		//Term
		auto q_term = parsing_stack.top().node;
		//res->addChild(parsing_stack.top().node);
		parsing_stack.pop();

		res->addChild(q_term);
		res->addChild(q_mulop);
		res->addChild(q_factor);
	}else
	{
		res->addChild(q_factor);
	}

	return res;
}

PNode* SLRParser::reduceAddop(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "Addop";

	//ADD_OP || MINUS_OP
	res->addChild(reduceMatch(parsing_stack));

	return res;
}

PNode* SLRParser::reduceComparison(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "Comparison-op";

	//LESS_THAN || EQUAL
	res->addChild(reduceMatch(parsing_stack));

	return res;
}

PNode* SLRParser::reduceSimpleExp(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "Simple-exp";

	//Term
	auto q_term = parsing_stack.top().node;
	//res->addChild(parsing_stack.top().node);
	parsing_stack.pop();
	if(parsing_stack.top().cell.row.id == "Addop")
	{
		//Addop
		auto q_addop = parsing_stack.top().node;
		//res->addChild(parsing_stack.top().node);
		parsing_stack.pop();
		//Simple-exp
		auto q_simpleexp = parsing_stack.top().node;
		//res->addChild(parsing_stack.top().node);
		parsing_stack.pop();

		res->addChild(q_simpleexp);
		res->addChild(q_addop);
		res->addChild(q_term);
	}else
	{
		res->addChild(q_term);
	}

	return res;
}

PNode* SLRParser::reduceExp(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "Exp";

	//Simple-exp
	auto q_simpleexp = parsing_stack.top().node;
	//res->addChild(parsing_stack.top().node);
	parsing_stack.pop();
	if(parsing_stack.top().cell.row.id == "Comparison-op")
	{
		//Comparison-op
		auto q_comparisonop = parsing_stack.top().node;
		//res->addChild(parsing_stack.top().node);
		parsing_stack.pop();
		//Simple-exp
		auto q_simpleexp2 = parsing_stack.top().node;
		//res->addChild(parsing_stack.top().node);
		parsing_stack.pop();

		res->addChild(q_simpleexp2);
		res->addChild(q_comparisonop);
		res->addChild(q_simpleexp);
	}else{
		res->addChild(q_simpleexp);
	}
	return res;
}

PNode* SLRParser::reduceAssignstmt(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "Assignstmt";

	//Exp
	auto q_exp = parsing_stack.top().node;
	//res->addChild(parsing_stack.top().node);
	parsing_stack.pop();
	//ASSIGNMENT_OP
	reduceMatch(parsing_stack);
	//res->addChild(reduceMatch(parsing_stack));
	//ID
	auto q_id = reduceID(parsing_stack);
	//res->addChild(reduceID(parsing_stack));

	res->addChild(q_id);
	res->addChild(q_exp);

	return res;
}

PNode* SLRParser::reduceWritestmt(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "Writestmt";

	//Exp
	res->addChild(parsing_stack.top().node);
	parsing_stack.pop();

	//WRITE
	reduceMatch(parsing_stack);
	//res->addChild(reduceMatch(parsing_stack));

	return res;
}

PNode* SLRParser::reduceRepeatstmt(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "Repeatstmt";

	//Exp
	auto q_exp = parsing_stack.top().node;
	//res->addChild(parsing_stack.top().node);
	parsing_stack.pop();
	//UNTIL
	reduceMatch(parsing_stack);
	//res->addChild(reduceMatch(parsing_stack));
	//StSeq
	auto q_stseq = parsing_stack.top().node;
	//res->addChild(parsing_stack.top().node);
	parsing_stack.pop();
	//REPEAT
	reduceMatch(parsing_stack);
	//res->addChild(reduceMatch(parsing_stack));

	res->addChild(q_stseq);
	res->addChild(q_exp);

	return res;
}

PNode* SLRParser::reduceIFstmt(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "IFstmt";

	//END
	reduceMatch(parsing_stack);
	//res->addChild(reduceMatch(parsing_stack));
	//StSeq
	auto q_stseq1 = parsing_stack.top().node;
	//res->addChild(parsing_stack.top().node);
	parsing_stack.pop();
	if(parsing_stack.top().cell.row.id == "THEN")
	{
		//THEN
		reduceMatch(parsing_stack);
		//res->addChild(reduceMatch(parsing_stack));
		//Exp
		auto q_exp = parsing_stack.top().node;
		//res->addChild(parsing_stack.top().node);
		parsing_stack.pop();
		//IF
		reduceMatch(parsing_stack);
		//res->addChild(reduceMatch(parsing_stack));

		res->addChild(q_exp);
		res->addChild(q_stseq1);
	}else if(parsing_stack.top().cell.row.id == "ELSE")
	{
		//ELSE
		reduceMatch(parsing_stack);
		//res->addChild(reduceMatch(parsing_stack));
		//StSeq
		auto q_stseq2 = parsing_stack.top().node;
		//res->addChild(parsing_stack.top().node);
		parsing_stack.pop();
		//THEN
		reduceMatch(parsing_stack);
		//res->addChild(reduceMatch(parsing_stack));
		//Exp
		auto q_exp = parsing_stack.top().node;
		//res->addChild(parsing_stack.top().node);
		parsing_stack.pop();
		//IF
		reduceMatch(parsing_stack);
		//res->addChild(reduceMatch(parsing_stack));

		res->addChild(q_exp);
		res->addChild(q_stseq2);
		res->addChild(q_stseq1);
	}

	return res;
}

PNode* SLRParser::reduceProgram(std::stack<stackElement>& parsing_stack)
{
	stackElement e = parsing_stack.top();
	PNode* res = Services::memory->alloc<PNode>();
	new (res) PNode();
	res->Tag = "Program";

	//StSeq
	res->addChild(parsing_stack.top().node);
	parsing_stack.pop();

	return res;
}

CTools::SLRParser::SLRParser(ParseTable table){
	m_table = table;
	m_state.num = 0;
}

CTools::SLRParser::~SLRParser(void)
{
	
}

CTools::PNode* CTools::SLRParser::Parse(TokenStream& stream)
{

	m_begin = stream.begin();
	m_cursor = m_begin;
	m_end = stream.end();



	PNode* res = ParseProgram();
	return res;
}

PNode* SLRParser::ParseProgram()
{
	stack<stackElement> parsing_stack;
	parsing_stack.push(m_state);

	while(true)
	{
		stackElement current = parsing_stack.top();
		stackElement next_state;
		if(m_cursor == m_end)
		{
			if(current.str.empty())
				next_state = move(current,"$");
			else
			{
				parsing_stack.pop();
				next_state = move(parsing_stack.top(),current.str);
				//current = parsing_stack.top();
			}
		}else if(current.str.empty())
			next_state = move(current,m_cursor->tag);
		else{
			parsing_stack.pop();
			next_state = move(parsing_stack.top(),current.str);
			//current = parsing_stack.top();
		}

		if(next_state.num == -1)
		{
			throw runtime_error("invalid transition from state: "+std::to_string(current.cell.col.number)+" and Terminal: "+current.cell.row.id);
		}

		if(next_state.cell.action == Action::ACCEPT)
		{
			cerr<<"ACCEPT"<<endl;
			break;
		}else if(next_state.cell.action == Action::GOTO)
		{
			cerr<<"GOTO"<<endl;
			next_state.num = next_state.cell.shift_state;
			next_state.node = current.node;
			parsing_stack.push(next_state);
		}else if(next_state.cell.action == Action::SHIFT)
		{
			cerr<<"shift"<<endl;
			next_state.num = next_state.cell.shift_state;
			next_state.token = *m_cursor;
			parsing_stack.push(next_state);
			m_cursor++;
		}
		else if(next_state.cell.action == Action::REDUCE)
		{
			cerr<<"reduce"<<endl;
			auto node = reduce(parsing_stack,next_state.cell.reduce_rule);
			parsing_stack.top().node = node;
		}else
		{
			cerr<<"ERROR"<<endl;
			break;
		}
	}

	return parsing_stack.top().node;
//	stack<stackElement> parsing_stack;
//	stackElement e;
//	e.state = 0;
//	parsing_stack.push(e);
//
//	while(true)
//	{
//		stackElement state = parsing_stack.top();
//
//		stackElement TableLookup = findAndGet(state,m_cursor->tag);
//
//		//parsing_stack.push(TableLookup);
//
//
//		if(TableLookup.ticket->AC)
//		{
//			if(TableLookup.ticket->value->isTerminal)
//			{
//				//reduce
//				cerr<<"reduce"<<endl;
//				int N = TableLookup.ticket->n-1;
//				for(int i=0;i<N;i++)
//					parsing_stack.pop();
//				
//				stackElement reduced = findAndGet(parsing_stack.top(),TableLookup.ticket->reduce_val->ID);
//
//				if(reduced.state == -1){
//					cerr<<"A7a"<<endl;
//				}
//
//				parsing_stack.push(reduced);
//
//				while(parsing_stack.top().ticket && parsing_stack.top().ticket->AC)
//				{
//					parsing_stack.pop();
//
//					reduced = findAndGet(parsing_stack.top(),reduced.ticket->reduce_val->ID);
//
//					if(reduced.state == -1){
//						cerr<<"A7a"<<endl;
//					}
//
//					parsing_stack.push(reduced);
//				}
//			}else 
//			{
//				//accept;
//				cerr<<"accept"<<endl;
//				break;
//			}
//		}else if(TableLookup.ticket->value->isTerminal)
//		{
//			//shift
//			cerr<<"shift"<<endl;
//			parsing_stack.push(TableLookup);
//		}else
//		{
//			//error
//			cerr<<"Error"<<endl;
//			break;
//		}
//
//		m_cursor++;
//	}
	return nullptr;
}
#include "RegEx.hpp"
#include "Memory.hpp"
#include "FSM.hpp"
#include <Services.hpp>
#include <memory>
using namespace std;
using namespace CTools;

bool RegEx::isOperator(char c)
{
		return 
		((c==42) || (c==124) || (c==40) || (c==41) || (c==8));
}

bool RegEx::isInput(char c){
	return !isOperator(c);
}

/**
 * Closure > Concat > Union
 * 	  *    >   8    >   |
 */
bool RegEx::presedence(char left, char right){
	if(left == right)
		return true;
	
	if(left == '*')
		return false;
	if(right == '*')
		return true;
		
	if(left == 8)
		return false;
	if(right == 8)
		return true;
	
	if(left == '|')
		return false;
	
	return true;
}

void RegEx::push(char c){
	State* s0 = Services::memory->alloc<State>();
	new (s0) State(++m_nNextStateID);
	State* s1 = Services::memory->alloc<State>();
	new (s1) State(++m_nNextStateID);

	s0->addTransition(c,s1);

	FSA_TABLE NFATable;
	NFATable.push_back(s0);
	NFATable.push_back(s1);

	m_OperandStack.push(NFATable);

	m_InputSet.insert(c);
}

bool RegEx::pop(FSA_TABLE& table)
{
	if(m_OperandStack.size()>0)
	{
		table = m_OperandStack.top();
		m_OperandStack.pop();
		return true;
	}
	return false;
}

bool RegEx::concat(){
	FSA_TABLE A, B;
	if(!pop(B) || !pop(A))
		return false;

	A.back()->addTransition(0,B[0]);
	A.insert(A.end(),B.begin(),B.end());

	m_OperandStack.push(A);
	return true;
}

bool RegEx::star()
{
	FSA_TABLE A;
	if(!pop(A))
		return false;

	State* pStart = Services::memory->alloc<State>();
	new (pStart) State(++m_nNextStateID);
	State* pEnd = Services::memory->alloc<State>();
	new (pEnd) State(++m_nNextStateID);

	pStart->addTransition(0,pEnd);

	pStart->addTransition(0,A[0]);

	A.back()->addTransition(0,pEnd);

	A.back()->addTransition(0,A[0]);

	A.push_back(pEnd);
	A.push_front(pStart);

	m_OperandStack.push(A);
	return true;
}

bool RegEx::Union()
{
	FSA_TABLE A, B;
	if(!pop(B) || !pop(A))
		return false;

	State* pStart = Services::memory->alloc<State>();
	new (pStart) State(++m_nNextStateID);
	State* pEnd = Services::memory->alloc<State>();
	new (pEnd) State(++m_nNextStateID);

	pStart->addTransition(0,A[0]);
	pStart->addTransition(0, B[0]);

	A.back()->addTransition(0,pEnd);
	B.back()->addTransition(0,pEnd);

	B.push_back(pEnd);
	A.push_front(pStart);
	A.insert(A.end(),B.begin(),B.end());

	m_OperandStack.push(A);

	return true;
}

std::string RegEx::concatExpand(std::string reg)
{
	string res;
	for(int i=0;i<reg.size()-1;++i)
	{
		char cLeft = reg[i];
		char cRight = reg[i+1];

		res += cLeft;

		if((isInput(cLeft)) || (cLeft == ')') || (cLeft == '*'))
			if((isInput(cRight)) || (cRight == '('))
				res += char(8);
	}
	res += reg[reg.size()-1];
	return res;
}

bool RegEx::createNFA(std::string reg)
{
	reg = concatExpand(reg);

	for(int i=0;i<reg.size();i++)
	{
		char c = reg[i];

		if(isInput(c))
			push(c);
		else if(m_OperatorStack.empty())
			m_OperatorStack.push(c);
		else if(c=='(')
			m_OperatorStack.push(c);
		else if(c==')')
		{
			while(m_OperatorStack.top()!='(')
				if(!Eval())
					return false;
			m_OperatorStack.pop();
		}else
		{
			while(!m_OperatorStack.empty() && presedence(c, m_OperatorStack.top()))
				if(!Eval())
					return false;
			m_OperatorStack.push(c);
		}
	}

	while(!m_OperatorStack.empty())
		if(!Eval())
			return false;

	if(!pop(m_NFATable))
		return false;

	m_NFATable.back()->setAccept(true);
	return true;
}

bool RegEx::Eval()
{
	if(m_OperatorStack.size()>0)
	{
		char Op = m_OperatorStack.top();
		m_OperatorStack.pop();

		switch (Op)
		{
		case 42:
			return star();
			break;
		case 124:
			return Union();
			break;
		case 8:
			return concat();
			break;
		}

		return false;
	}
	return false;
}


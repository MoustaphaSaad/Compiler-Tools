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
		((c==5) || (c==124) || (c==21) || (c==22) || (c==8));
}

bool RegEx::isInput(char c){
	return !isOperator(c);
}

/**
 * Closure > Concat > Union
 * 	  5    >   8    >   |
 */
bool RegEx::presedence(char left, char right){
	if(left == right)
		return true;
	
	if(left == 5)
		return false;
	if(right == 5)
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

void RegEx::Combine(RegEx& reg)
{
	FSA_TABLE::iterator it;
	for(it = reg.m_NFATable.begin();it != reg.m_NFATable.end();it++)
	{
		(*it)->setId(++m_nNextStateID);
	}
	set<char>::iterator c;
	for(c=reg.m_InputSet.begin();c!=reg.m_InputSet.end();c++)
	{
		m_InputSet.insert(*c);
	}
	m_OperandStack.push(m_NFATable);
	m_OperandStack.push(reg.m_NFATable);
	Union();
	m_NFATable = m_OperandStack.top();
	m_OperandStack.pop();
}

std::string RegEx::concatExpand(std::string reg)
{
	string res;
	for(int i=0;i<reg.size()-1;++i)
	{
		char cLeft = reg[i];
		char cRight = reg[i+1];

		res += cLeft;

		if((isInput(cLeft)) || (cLeft == 22) || (cLeft == 5))
			if((isInput(cRight)) || (cRight == 21))
				res += char(8);
	}
	res += reg[reg.size()-1];
	return res;
}

std::string RegEx::rangeExpand(std::string reg)
{
	string res;
	if(reg.size()>0)
		res.push_back(reg[0]);
	for(int i=1;i<reg.size()-1;i++)
	{
		char cLeft = reg[i-1];
		char c = reg[i];
		char cRight = reg[i+1];

		if(c == '-')
		{
			res.erase(res.size()-1,1);
			if(isInput(cLeft) && isInput(cRight))
			{
				for(int j=cLeft;j<=cRight;j++)
				{
					if(j!=cLeft)
						res.push_back('|');
					res.push_back((char)j);
				}
			}
			i++;
		}else
		{
			//res.push_back(cLeft);
			res.push_back(c);
			//res.push_back(cRight);
		}
	}
	if(reg.size()>1)
		res.push_back(reg.back());
	return res;
}

std::string RegEx::plusExpand(std::string reg)
{
	string res;
	if(reg.size()>0)
		res.push_back(reg[0]);
	for(int i=1;i<reg.size();i++)
	{
		char c = reg[i];

		if(c == '+')
		{
			if(reg[i-1] == 39)
			{
				if(i+1<reg.size() && reg[i+1] == 39)
				{
					res.erase(res.size()-1,1);
					res.push_back('+');
					i++;
					continue;
				}
			}
			string term;
			//res.push_back('|');
			for(int j=i-1;j>-1;j--)
			{
				char tc = res[j];
				if(isInput(tc))
				{
					term = tc;
					break;
				}else if(tc == 22)
				{
					int pc = 0;
					for(int k=j;k>-1;k--)
					{
						char kc = res[k];
						if(kc == 22)
						{
							pc++;
							term.push_back(kc);
						}else if(kc == 21)
						{
							pc--;
							term.push_back(kc);
						}else
						{
							term.push_back(kc);
						}

						if(pc == 0)
							break;
					}
					if(term.size()>0)
						break;
				}
			}
			std::reverse(term.begin(),term.end());
			res+=term;
			res.push_back((char)5);
		}else
		{
			res.push_back(c);
		}
	}
	return res;
}

std::string RegEx::operatorExpand(std::string reg)
{
	string res;
	for(int i=0;i<reg.size();i++)
	{
		char c = reg[i];

		if(i-1>=0 && reg[i-1] == 39 && i+1<reg.size() && reg[i+1] == 39)
		{
			//do nothing
			res.erase(res.size()-1,1);
			res.push_back(c);
			i++;
		}else
		{
			if(c == '*')
			{
				res.push_back(5);
			}else if(c=='(')
			{
				res.push_back(21);
			}else if(c==')')
			{
				res.push_back(22);
			}else
			{
				res.push_back(c);
			}
		}
	}
	return res;
}


bool RegEx::createNFA(std::string reg)
{
	reg = operatorExpand(reg);
	reg = rangeExpand(reg);
	reg = plusExpand(reg);
	reg = concatExpand(reg);

	for(int i=0;i<reg.size();i++)
	{
		char c = reg[i];

		if(isInput(c))
			push(c);
		else if(m_OperatorStack.empty())
			m_OperatorStack.push(c);
		else if(c==21)
			m_OperatorStack.push(c);
		else if(c==22)
		{
			while(m_OperatorStack.top()!=21)
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
		case 5:
			return star();
			break;
		case 124:
			return Union();
			break;
		case 8:
			return concat();
		}

		return false;
	}
	return false;
}

void RegEx::EpsilonClosure(std::set<State*> input, std::set<State*>& output)
{
	output.clear();

	output = input;

	stack<State*> unprocessedStack;
	set<State*>::iterator it;
	for(it = input.begin(); it != input.end(); it++)
	{
		unprocessedStack.push(*it);
	}

	while(!unprocessedStack.empty())
	{
		State* t = unprocessedStack.top();
		unprocessedStack.pop();

		vector<State*> epsilonStates;
		t->getTransition(0,epsilonStates);

		for(int i=0;i<epsilonStates.size();i++)
		{
			State* u = epsilonStates[i];

			if(output.find(u) == output.end())
			{
				output.insert(u);
				unprocessedStack.push(u);
			}
		}
	}
}

void RegEx::Move(char c, std::set<State*> input, std::set<State*>& output)
{
	output.clear();

	set<State*>::iterator it;
	for(it= input.begin();it!= input.end();it++)
	{
		State* u = *it;
		vector<State*> states;
		u->getTransition(c,states);

		for(int i = 0;i<states.size();i++)
		{
			output.insert(states[i]);
		}
	}
}

void RegEx::ConvertToDFA()
{
	for(int i=0;i<m_DFATable.size();++i)
		Services::memory->free(m_DFATable[i]);
	m_DFATable.clear();
	
	if(m_NFATable.size() == 0)
		return;
		
	m_nNextStateID = 0;
	
	vector<State*> unmarkedStates;
	
	set<State*> DFA_SSet;
	set<State*> NFA_SSet;
	
	NFA_SSet.insert(m_NFATable[0]);
	
	//start with start state and all EpsilonClosure states
	EpsilonClosure(NFA_SSet,DFA_SSet);
	
	State* DFA_SS = Services::memory->alloc<State>();
	new (DFA_SS) State(DFA_SSet,++m_nNextStateID);
	
	m_DFATable.push_back(DFA_SS);
	
	unmarkedStates.push_back(DFA_SS);
	
	while(!unmarkedStates.empty())
	{
		State* p_state = unmarkedStates.back();
		unmarkedStates.pop_back();
		
		set<char>::iterator it;
		
		for(it = m_InputSet.begin(); it!= m_InputSet.end(); it++){
			set<State*> move_res, ec_res;
			
			Move(*it,p_state->getNFAState(), move_res);
			EpsilonClosure(move_res,ec_res);
			
			bool found = false;
			State* p = nullptr;

			for(int i=0;i<m_DFATable.size(); ++i)
			{
				p = m_DFATable[i];

				if(p->getNFAState() == ec_res)
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				State* U = Services::memory->alloc<State>();
				new (U) State(ec_res,++m_nNextStateID);
				unmarkedStates.push_back(U);
				m_DFATable.push_back(U);

				//link two states together
				p_state->addTransition(*it,U);
			}
			else
			{
				p_state->addTransition(*it,p);
			}
		}
	}	
}

void RegEx::OptimizeDFA()
{
	set<State*> Traps;
	for(int i=0;i<m_DFATable.size();i++)
	{
		if(m_DFATable[i]->isDeadEnd())
			Traps.insert(m_DFATable[i]);
	}

	if(Traps.empty())
		return;

	set<State*>::iterator it;
	for(it = Traps.begin();it!=Traps.end();it++)
	{
		for(int i=0;i<m_DFATable.size();i++)
		{
			m_DFATable[i]->removeTransition(*it);
		}

		deque<State*>::iterator pos;
		for(pos = m_DFATable.begin();pos!=m_DFATable.end();pos++)
		{
			if(*pos == *it)
				break;
		}

		m_DFATable.erase(pos);

		Services::memory->free(*it);
	}
}
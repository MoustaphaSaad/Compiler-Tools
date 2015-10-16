#pragma once

#include "Globals.hpp"
#include "FSM.hpp"
#include <deque>
#include <string>
#include <stack>

namespace CTools{
	typedef std::deque<State*> FSA_TABLE;
	class API RegEx{
		u32 m_nNextStateID;
		FSA_TABLE m_NFATable, m_DFATable;

		std::stack<FSA_TABLE> m_OperandStack;
		std::stack<char> m_OperatorStack;
		std::set<char> m_InputSet;
	public:

		RegEx(){m_nNextStateID=0;}
		~RegEx(){}
		void push(char c);
		
		bool pop(FSA_TABLE& table);
		
		bool isOperator(char c);
		
		bool presedence(char left, char right);
		
		bool isInput(char c);
		
		bool concat();
		
		bool star();
		
		bool Union();
		
		std::string concatExpand(std::string reg);
		
		bool createNFA(std::string reg);		

		bool Eval();
	};
}
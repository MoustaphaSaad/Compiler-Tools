#include "FSM.hpp"
#include <assert.h>
using namespace std;
using namespace CTools;

State::State():m_id(-1), m_accept(false)
{}

State::State(s32 id):m_id(id), m_accept(false)
{}

State::State(s32 id, bool accept): m_id(id), m_accept(accept)
{}

State::State(std::set<State*> NFAState, s32 nID)
{
	m_NFAStates = NFAState;
	m_id = nID;

	m_accept = false;

	//check for accepting states
	set<State*>::iterator it;
	for(it = m_NFAStates.begin(); it != m_NFAStates.end();it++)
	{
		if((*it)->m_accept)
			m_accept = true;
	}
}

State::~State()
{}

void State::addTransition(char input, State* state)
{
	assert(state != NULL);
	m_transition.insert(make_pair(input, state));
}

void State::removeTransition(State* state)
{
	multimap<char, State*>::iterator it;
	for(it = m_transition.begin(); it!= m_transition.end();)
	{
		State* toState = it->second;
		if(toState == state)
			m_transition.erase(it++);
		else
			++it;
	}	
}

void State::getTransition(char input, std::vector<State*> &states)
{
	states.clear();
	
	multimap<char, State*>::iterator it;
	for(it = m_transition.lower_bound(input);
		it != m_transition.upper_bound(input);
		++it)
		{
			State* state = it->second;
			assert(state != NULL);
			states.push_back(state);
		}
}

State* State::Consume(char c)
{
	multimap<char, State*>::iterator it;
	for(it = m_transition.lower_bound(c);
		it!=m_transition.upper_bound(c);
		++it)
	{
		return (*it).second;
	}
	return nullptr;
}

s32 State::getId()
{
	return m_id;
}

void State::setId(s32 val)
{
	m_id = val;
}

bool State::isDeadEnd()
{
	if(m_accept)
		return false;
	
	if(m_transition.empty())
		return true;
		
	multimap<char, State*>::iterator it;
	for(it=m_transition.begin();it!=m_transition.end();++it)
	{
		State* state = it->second;
		if(state != this)
			return false;
	}
	
	return true;
}

bool State::isAcceptState()
{
	return m_accept;
}
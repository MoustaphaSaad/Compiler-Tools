#include "ScanMachine.hpp"
#include <Services.hpp>

using namespace std;
using namespace CTools;

void ScanMachine::insertLayer()
{
	Layer l;
	l.activeStates = m_activeStates;
	l.activeMIX = m_activeMIX;
	l.state = m_state;
	l.lexeme = m_word;
	m_StateStack.push(l);
}

ScanMachine::~ScanMachine()
{

}

void ScanMachine::addSubMachine(RegEx* machine, string tag)
{
	m_littleMachines.push_back(machine);
	m_tags.push_back(tag);
}

ScanState ScanMachine::consume(char c)
{
	decltype(m_activeStates) new_states;
	decltype(m_activeMIX) new_MIX;

	//update active states
	s32 i = 0;
	for(auto it = m_activeStates.begin();
		it != m_activeStates.end();
		++it)
	{
		State* pState = *it;
		auto result = pState->Consume(c);

		if(result)
		{
			new_states.push_back(result);
			new_MIX.push_back(m_activeMIX[i]);
		}
		i++;
	}

	//infer current scan state
	if(new_states.size() == 0)
	{
		m_state = ScanState::AMBIGUIOUS;
		return m_state;
	}else if(new_states.size() == 1 && new_states[0]->isAcceptState())
	{
		if(new_states[0]->getTransitionCount() == 0)
		{
			m_state = ScanState::CONVERGED;
		}else
		{
			m_state = ScanState::STABLE;
		}
	}else
	{
		m_state = ScanState::NONE;
	}

	//update state stack
	m_activeStates = new_states;
	m_activeMIX = new_MIX;
	if(m_StateStack.size()>0){
		m_word.push_back(c);
		insertLayer();
	}else
	{
		m_word.push_back(c);
		insertLayer();
	}

	return m_state;
}

void ScanMachine::reset()
{
	m_word.clear();
	m_state = ScanState::START;
	m_activeStates.clear();
	m_activeMIX.clear();
	while(!m_StateStack.empty())
		m_StateStack.pop();

	int i=0;
	for(auto it = m_littleMachines.begin();
		it != m_littleMachines.end();
		++it)
	{
		m_activeStates.push_back((*it)->getDFAStart());
		m_activeMIX.push_back(i);
		i++;
	}
}

ScanState ScanMachine::cut()
{
	for(auto it = m_activeStates.begin();
		it != m_activeStates.end();
		++it)
	{
		State* pState = *it;
		if(pState->isAcceptState())
		{
			m_state = ScanState::CONVERGED;
			insertLayer();
			return m_state;
		}
	}
	m_state = ScanState::AMBIGUIOUS;
	return m_state;
}

ScanState ScanMachine::recover()
{
	if(m_StateStack.size()==0)
		return ScanState::AMBIGUIOUS;

	auto layer = m_StateStack.top();
	m_activeStates = layer.activeStates;
	m_activeMIX = layer.activeMIX;
	m_state = layer.state;

	m_StateStack.pop();
	return m_state;
}

Token ScanMachine::getToken()
{
	if(m_state == ScanState::STABLE || m_state == ScanState::CONVERGED)
	{
		int i=0;
		for(auto it = m_activeStates.begin();
			it != m_activeStates.end();
			++it)
		{
			auto pState = *it;
			if(pState->isAcceptState())
			{
				Token token;
				token.lexeme = m_word;
				token.tag = m_tags[m_activeMIX[i]];
				token.valid = true;
				return token;
			}
			i++;
		}
	}

	return Token::INVALID();
}

ScanState ScanMachine::getCurrentState()
{
	return m_state;
}

std::string ScanMachine::getCurrentString()
{
	return m_word;
}
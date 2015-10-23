#include "MachineWalker.hpp"
#include <sstream>
#include <iostream>
#include <Services.hpp>
using namespace std;
using namespace CTools;

void MachineWalker::online_process(Token& t, std::istream& stream)
{
	if(online_processor == nullptr)
		return;
	online_processor->Process(t,stream,errorFunction);
}

MachineWalker::MachineWalker(IScanner* lex, ITokenProcessor* p)
{
	scanner = lex;
	online_processor = p;
}

Token MachineWalker::token(std::istream& stream)
{
	scanner->reset();

	int i = -1;
	ScanState state = ScanState::START;
	bool ignore = false;
	while(i = stream.peek())
	{
		ignore = false;

		if(i == -1){
			break;
		}

		char c = i;
		pos.absolute_location++;

		if(c == '\n' || c == ' ' || c == '\r' || c == '\t')
		{
			ignore = true;
			//ignore this characters
			

			if(c == '\n')
			{
				pos.row ++;
				pos.col=0;
			}else
			{
				pos.col++;
			}
			stream.get();
			if(scanner->getCurrentState() != ScanState::START){
				state = scanner->cut();
			}else
			{
				continue;
			}
		}else
		{
			state = scanner->consume(c);
		}
		
		if(state == ScanState::CONVERGED)
		{
			Token token = scanner->getToken();
			token.position = pos;
			online_process(token,stream);
			if(!ignore){
				pos.col++;
				stream.get();
			}
			return token;
		}else if(state == ScanState::AMBIGUIOUS)
		{
			state = scanner->recover();
			if(state == ScanState::AMBIGUIOUS){
				string word = scanner->getCurrentString();
				if(word.empty())
					word.push_back(c);
				errorFunction(word,pos);
				if(!ignore){
					pos.col++;
					stream.get();
				}
				scanner->reset();
			}else
			{
				state = scanner->cut();
				if(state == ScanState::CONVERGED)
				{
					Token token = scanner->getToken();
					token.position = pos;
					online_process(token,stream);
					return token;
				}else
				{
					string word = scanner->getCurrentString();
					if(word.empty())
						word.push_back(c);
					errorFunction(word,pos);
					if(!ignore){
						pos.col++;
						stream.get();
					}
					scanner->reset();
				}
			}
		}else
		{
			if(!ignore){
				pos.col++;
				stream.get();
			}
		}
	}
	if(scanner->getCurrentState() != ScanState::START)
	{
		state = scanner->cut();
		if(state == ScanState::CONVERGED)
		{
			Token token = scanner->getToken();
			token.position = pos;
			online_process(token,stream);
			return token;
		}else
		{
			errorFunction(scanner->getCurrentString(),pos);
		}
	}
	
	return Token::INVALID();
}

void MachineWalker::reset()
{
	pos = Position();
}
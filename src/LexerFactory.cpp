#include "LexerFactory.hpp"
#include <sstream>
#include <Services.hpp>
using namespace std;
using namespace CTools;

Tokens_Table ScannerFactory::parseTokens(const string& content){
	Tokens_Table output;
	auto pos = content.find("@Tokens");
	auto of = content.find("@",pos+1);
	stringstream input(content.substr(pos,of-pos));
	string regex,equal,tag;

	bool token_section = false;
	while(input>>regex)
	{
		if(regex == "@Tokens"){
			token_section = true;
			continue;
		}

		if(token_section){
			input>>equal;
			input>>tag;

			regex = regex.substr(1,regex.size()-2);
			output.push_back(make_pair(regex,tag));
		}
	}
	return output;
}

ScanMachine* ScannerFactory::createFromString(const std::string& content){
	Tokens_Table table = parseTokens(content);

	ScanMachine* res;
	res = Services::memory->alloc<ScanMachine>();
	new (res) ScanMachine();
	RegEx* tmp;
	Tokens_Table::iterator it;
	for(it = table.begin();it!=table.end();it++)
	{
		std::string regex = (*it).first;
		tmp = Services::memory->alloc<RegEx>();
		new (tmp) RegEx();
		tmp->createNFA(regex);
		tmp->ConvertToDFA();
		tmp->OptimizeDFA();
		res->addSubMachine(tmp,it->second);
	}
	return res;
}

ScanMachine* ScannerFactory::createFromFile(std::string path){
	std::string file = IO::read(path);
	return createFromString(file);
}
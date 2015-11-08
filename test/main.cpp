#include <iostream>
#include "CTools.hpp"
#include <thread>

using namespace std;
using namespace CTools;

class OnlineProcessor:public ITokenProcessor
{
public:
	OnlineProcessor(){}
	~OnlineProcessor(){}

	void Process(Token& tkn, std::istream& file, errorCallback error_stream) override
	{

		int n = 0;
		if(tkn.tag == "COMMENT")
		{
			tkn.tag = "COMMENT";
			int n = 1;
			char c;
			file.get();
			while(file.get(c))
			{
				if(c == '{')
				{
					n++;
				}else if(c=='}')
				{
					n--;
				}

				tkn.lexeme.push_back(c);

				if(n==0)
				{
					break;
				}
			}
			if(n!=0)
			{
				error_stream("Comment didn't end properly",tkn.position);
			}
		}
	}
};

void error(string c, Position p)
{
	cerr<<"ERROR: "<<c<<" error in location row: "<<p.row<<", col: "<<p.col<<endl;
}

void parse(TokenStream &tokens)
{
	IParser* parser = new RecursiveDescent();
	try{
		PNode* program = parser->Parse(tokens);
	}catch(exception e)
	{
		cerr<<e.what()<<endl;
	}
}
int main(){
	CTools::Services::init();

	/*
	RegEx r,d, ex00,ex01;
	r.createNFA("(a|b)*cd");
	d.createNFA("fg(k|o)*");
	ex01.createNFA("(a-z|(A-Z)*|0-9)+");
	r.Combine(d);
	r.ConvertToDFA();
	r.OptimizeDFA();
	*/

	ScanMachine* m = ScannerFactory::createFromFile("scanner.ctlex");
	OnlineProcessor* processor = Services::memory->alloc<OnlineProcessor>();
	MachineWalker walker(m,processor);
	walker.errorFunction = error;

	walker.reset();
	stringstream str(
		"Read x;\n"
		"Read y;\n"
		"z := 0;"
		"Repeat\n"
		"z := z + 1"
		"Until z = y;"
		"If x < y Then\n"
		"Write y\n"
		"Else\n"
		"Write x\n"
		"End\n"
		);
	
	TokenStream tokens;
	while(true)
	{
		Token tk = walker.token(str);
		if(!tk.valid)
			break;
		tokens.push_back(tk);

		cout<<tk.tag<<": "<<tk.lexeme<<endl;
	}
	parse(tokens);
//	walker.reset();
//	stringstream str2("x:=f..7+bfke");
//	while(true)
//	{
//		Token tk = walker.token(str2);
//		if(!tk.valid)
//			break;
//
//		cout<<tk.tag<<": "<<tk.lexeme<<endl;
//	}
	/*
	walker.walkContent("x:=3f7+b;");

	while(!walker.EndOfStream())
	{
		Token t = walker.pop();
		cout<<t.tag<<": "<<t.lexeme<<endl;
	}

	if(walker.ErrorsExist())
	{
		ErrorStream errors = walker.getErrors();
		ErrorStream::iterator it;
		for(it = errors.begin();it!=errors.end();it++)
		{
			cout<<it->first<<endl;
		}
	}
	*/
	cout<<"Hello, World!"<<endl;
	CTools::Services::terminate();
	return 0;
}

#include <iostream>
#include "CTools.hpp"

using namespace std;
using namespace CTools;

int main(){
	CTools::Services::init();
	
	RegEx r;
	r.createNFA("(a|b)*cd");
	
	cout<<"Hello, World!"<<endl;
	CTools::Services::terminate();
	return 0;
}
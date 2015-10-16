#include "Services.hpp"
using namespace std;
using namespace CTools;

Memory* Services::memory = nullptr;

void Services::init(){
	memory = new Memory(CTools::MEMORY_SIZE);
}

void Services::terminate(){
	if(memory)
		delete memory;
}
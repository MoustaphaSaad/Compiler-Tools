#include "Memory.hpp"
#include <assert.h>
#include <cstring>
using namespace std;
using namespace CTools;

Memory::Memory(u32 size){
	m_size = size;
	m_cursor = 0;
	m_buffer = new byte[size];
	memset(m_buffer,0,size);
	m_freeTable[m_buffer] = size;
}

Memory::~Memory(){
	terminate();
}

byte* Memory::c_alloc(u32 size)
{
	assert(size+m_cursor<m_size);
	byte* location = nullptr;
	for(auto block:m_freeTable){
		if(block.second >= size){
			m_freeTable[block.first] = block.second-size;
			location = block.first;
			m_allocTable[location] = size;
			m_cursor += size;
			break;
		}
	}
	assert(location != nullptr);
	return location;
}

void Memory::merge(){
	map<byte*, u32> m_newTable;
	map<byte*, u32>::iterator it, n_it;
	for(it = m_freeTable.begin(), n_it = ++m_freeTable.begin();
		it != m_freeTable.end() && n_it != m_freeTable.end();
		it++, n_it++)
		{
			if(it->first + it->second == n_it->first){
				m_newTable[it->first] = it->second + n_it->second;
			}else{
				m_newTable[it->first] = it->second;
			}
		}
	m_freeTable = m_newTable;
	return;
}

void Memory::terminate(){
	delete[] m_buffer;
}
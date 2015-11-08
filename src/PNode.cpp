#include "PNode.hpp"
#include "Services.hpp"
using namespace std;
using namespace CTools;

PNode::PNode(){
    m_parent = nullptr;
    m_childrens = vector<PNode*>();
    Tag = "";
    Value = "";
}

PNode::~PNode(){
    //should in fact free from memory module
    for(int i=0;i<m_childrens.size();++i)
        //delete m_childrens[i];
        Services::memory->free(m_childrens[i]);
}

void PNode::addChild(PNode* child){
    m_childrens.push_back(child);
    child->m_parent = this;
}

PNode* PNode::getParent(){
    return m_parent;
}

void PNode::setParent(PNode* parent){
    m_parent = parent;
}

PNode* PNode::operator[](u32 ix){
    return m_childrens[ix];
}

u32 PNode::size(){
    return m_childrens.size();
}

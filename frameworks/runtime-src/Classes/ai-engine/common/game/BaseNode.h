
#ifndef BASENODE_H
#define BASENODE_H 
#include "cocos2d.h"

class BaseNode:public cocos2d::Node 
{
public:
    BaseNode() {}
    ~BaseNode() {}
    void onEnter() {Node::onEnter();}
};
#endif

	
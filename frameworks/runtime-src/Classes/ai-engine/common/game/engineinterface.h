
#ifndef ENGINEINTERFACE_H
#define ENGINEINTERFACE_H 

#include "common/2D/Vector2D.h"
#include "BaseEntity.h"

#include "cocos2d.h"
#include "2d/CCDrawNode.h"
using namespace cocos2d;


typedef Node* pNode;

inline double getCurTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (double)tv.tv_sec + (double)tv.tv_usec/1000000;
}

inline Node *getUINode()
{
    return (Node *)Director::getInstance()->getRunningScene();
}

inline Node* getNewNode()
{
    return Node::create();
}

template<class T>
inline void addChildToNode(T *node, Node *child)
{
    (dynamic_cast<Node *>(node))->addChild(child);
}

template<class T>
inline void addChildToUI(T *child)
{
    auto scene = Director::getInstance()->getRunningScene();
    scene->addChild(child);
}


template<class T>
inline void removeChild(T *child)
{
    if (child)
    {
        child->removeFromParent();
    }
}

template<class T>
inline void setNodePos(T *node, Vector2D worldPos)
{
    (dynamic_cast<Node *>(node))->setPosition(Vec2(worldPos.x, worldPos.y));
}


template<class T>
inline Sprite *loadImgToNode(T *node, char *filePath)
{
    Sprite *sprite = Sprite::create(filePath);
    if (sprite)
    {
        (dynamic_cast<Node *>(node))->addChild(sprite);
    }

    return sprite;
}


template<class T>
inline Sprite *loadImgToNode(T *node, char *filePath, int tag, Vector2D leftBot, Vector2D rightTop)
{
    auto child = node->getChildByTag(tag);
    if (child)
    {
        (dynamic_cast<Sprite*>(child))->setTextureRect(Rect(leftBot.x, leftBot.y, rightTop.x-leftBot.x+1, rightTop.y-leftBot.y+1));
    }
    else
    {
        auto texture = Director::getInstance()->getTextureCache()->addImage(filePath);
        if (texture)
        {
            child = Sprite::createWithTexture(texture, Rect(leftBot.x, leftBot.y, rightTop.x-leftBot.x+1, rightTop.y-leftBot.y+1));
            if (child)
            {
                child->setTag(tag);
                node->addChild(child);
            }
        }
    }

    return dynamic_cast<Sprite*>(child);
}

template<class T>
inline void setNodeAnchorPoint( T *node, Vector2D &ap)
{
    node->setAnchorPoint(Vec2(ap.x, ap.y));
}

template<class T>
inline void setNodeAnchorPoint( T *node, int tag, Vector2D &ap)
{
    auto child = node->getChildByTag(tag);
    if (child)
    {
        child->setAnchorPoint(Vec2(ap.x, ap.y));
    }
}

template<class T>
void showString(T *node, int tag, std::string &str, Vector2D pos)
{
    auto child = node->getChildByTag(tag);
    if (child)
    {
        (dynamic_cast<Label *>(child))->setString(str);
    }
    else 
    {
        auto text = Label::createWithSystemFont(str, "arial", 20);
        text->setTag(tag);
        text->setPosition(Vec2(pos.x, pos.y));
        node->addChild(text);
    }
}


template<class T>
inline void drawLine(T *node, Vector2D &A, Vector2D &B)
{
    DrawNode *draw = DrawNode::create();
    draw->drawLine(Vec2(A.x, A.y), Vec2(B.x, B.y), Color4F::RED);
    (dynamic_cast<Node *>(node))->addChild(draw);    
}

template<class T>
inline void drawCircle(T *node, Vector2D &center, float radius)
{
    DrawNode *draw = DrawNode::create();
    draw->drawCircle(Vec2(center.x, center.y), radius, CC_DEGREES_TO_RADIANS(360), 60, false, Color4F(1.0, 0.0, 0.0, 0.5));
    node->addChild(draw);    
}

template<class T>
inline void enableScheduleUpdate(T *node)
{
    //parent::onEnter();
    (dynamic_cast<Node *>(node))->scheduleUpdate(); 
}

template<class T>
inline Vector2D getNodeSize(T *node)
{
    Size size =  node->getContentSize();
    return Vector2D(size.width, size.height);
}



#endif 


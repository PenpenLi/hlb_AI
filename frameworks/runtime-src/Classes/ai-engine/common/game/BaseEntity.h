
#ifndef BASE_ENTITY_H
#define BASE_ENTITY_H 
#include "common/2D/Vector2D.h"
#include "common/2D/InvertedAABBox2D.h"

#include <algorithm>

struct Telegram;
class BaseEntity
{
public:
    BaseEntity();
    virtual ~BaseEntity();

    virtual void onEnter() {}
    
    virtual void update(float dt){}

    virtual bool handleMessage(const Telegram& msg){return false;}
      
    int getID() const{return m_id;}
    
    void setPos(Vector2D pos);
    Vector2D getPos()const {return m_vPosition;}

    void setEntityType(int type) {m_type = type;}
    int getEntityType() {return m_type;}
    
    void setSize(Vector2D size) {m_dSize = size;}
    Vector2D getSize() {return m_dSize;}
   
    void tag() {m_tag = true;}
    void unTag() {m_tag = false;}
    bool isTag() {return m_tag;}
    
    float getBoundingRadius() const {return std::max(m_radius, m_dSize.x/2);}
    void setBoundingRadius(float r) {m_radius = r;}
    
protected:
    Vector2D m_vPosition;
    Vector2D m_dSize;
    float m_radius;
private:
    int m_id;
    int m_type;
    bool m_tag;
    static int m_totalID;
    
};
#endif 


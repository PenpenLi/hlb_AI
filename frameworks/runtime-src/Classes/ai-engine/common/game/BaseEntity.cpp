
#include "BaseEntity.h"
#include "EngineInterface.h"
#include "EntityManager.h"

int BaseEntity::m_totalID = 0;


BaseEntity::BaseEntity(): m_tag(false), m_dSize(Vector2D(0, 0)),m_radius(0)
{
    m_id = m_totalID;
    m_totalID++;

    m_vPosition.x = 0.0f;
    m_vPosition.y = 0.0f;

    EntityManager::instance()->addEntity(this);
}

BaseEntity::~BaseEntity()
{
    EntityManager::instance()->removeEntity(this);
}

void BaseEntity::setPos(Vector2D pos)
{
    if (pos.x != m_vPosition.x && pos.y != m_vPosition.y)
    {
        m_vPosition = pos;
        //setNodePos(this, pos);
    }
}


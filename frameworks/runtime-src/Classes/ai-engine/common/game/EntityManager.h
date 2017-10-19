#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   EntityManager.h
//
//  Desc:   Singleton class to handle the  management of Entities.          
//
//
//------------------------------------------------------------------------
#include <map>
#include <cassert>


class BaseEntity;

class EntityManager
{
public:
    static EntityManager* instance()
    {
        static EntityManager instance;

        return &instance;
    }
    
    void addEntity(BaseEntity* pEntity) 
    {
        m_EntityMap.insert(std::make_pair(pEntity->getID(), pEntity));
    }
    
    void removeEntity(BaseEntity* pEntity)
    {
        m_EntityMap.erase(m_EntityMap.find(pEntity->getID()));
    }

    //returns a pointer to the entity with the ID given as a parameter
    BaseEntity* getEntityByID(int id) const
    {
        //find the entity
        EntityMap::const_iterator ent = m_EntityMap.find(id);

        //assert that the entity is a member of the map
        assert ( (ent !=  m_EntityMap.end()) && "<EntityManager::getEntityByID>: invalid ID");
        
        return ent->second;    
    }
  
    //clears all entities from the entity map
    void reset(){m_EntityMap.clear();}


private:
    EntityManager(){}
    
    typedef std::map<int, BaseEntity*> EntityMap;

    //to facilitate quick lookup the entities are stored in a std::map, in which
    //pointers to entities are cross referenced by their identifying number
    EntityMap m_EntityMap;
  
};







#endif
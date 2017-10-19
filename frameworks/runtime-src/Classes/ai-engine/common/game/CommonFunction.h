#ifndef COMMONFUNCTION_H
#define COMMONFUNCTION_H
#include "common/2D/Vector2D.h"

//----------------------- TagNeighbors ----------------------------------
//
//  tags any entities contained in a std container that are within the
//  radius of the single entity parameter
//------------------------------------------------------------------------
template <class T, class conT>
void tagNeighbors(const T& entity, conT& ContainerOfEntities, float radius)
{
    //iterate through all entities checking for range
    for (typename conT::iterator curEntity = ContainerOfEntities.begin();
                                               curEntity != ContainerOfEntities.end(); ++curEntity)
    {
        //first clear any current tag
        (*curEntity)->unTag();
        
        Vector2D to = (*curEntity)->getPos() - entity->getPos();
        
        //the bounding radius of the other is taken into account by adding it to the range
        float range = radius + (*curEntity)->getBoundingRadius();
        
        //quick check , by hlb 
        if (std::fabs(to.x) > 2*range || std::fabs(to.y) > 2*range) 
        {
            continue;
        }
        
        //if entity within range, tag for further consideration. (working in
        //distance-squared space to avoid sqrts)
        if ( ((*curEntity) != entity) && (to.lengthSq() < range*range))
        {
            (*curEntity)->tag();
        }
    }//next entity
}


//------------------- invoidOverlap ---------------------
//
//  Given a pointer to an entity and a std container of pointers to nearby
//  entities, this function checks to see if there is an overlap between
//  entities. If there is, then the entities are moved away from each
//  other
//------------------------------------------------------------------------
template <class T, class conT>
void invoidOverlap(const T &entity, const conT &ContainerOfEntities)
{
    //iterate through all entities checking for any overlap of bounding radii
    for (typename conT::const_iterator curEntity =ContainerOfEntities.begin();
                                            curEntity != ContainerOfEntities.end(); ++curEntity)
    {
        //make sure we don't check against the individual
        if (*curEntity == entity) continue;

        //calculate the distance between the positions of the entities
        Vector2D ToEntity = entity->getPos() - (*curEntity)->getPos();

        //quick check , by hlb 
        float ra = entity->getBoundingRadius() + (*curEntity)->getBoundingRadius();
        if (std::fabs(ToEntity.x) > ra || std::fabs(ToEntity.y) > ra) 
        {
            continue;
        }
        
        float DistFromEachOther = ToEntity.length();

        //if this distance is smaller than the sum of their radii then this
        //entity must be moved away in the direction parallel to the ToEntity vector   
        float AmountOfOverLap = ra - DistFromEachOther;
        if (AmountOfOverLap >= 0)
        {
            //move the entity a distance away equivalent to the amount of overlap.
            entity->setPos(entity->getPos() + (ToEntity/DistFromEachOther) *AmountOfOverLap);
        }
    }//next entity
}


//treats a window as a toroid
inline void wrapAround(Vector2D &pos, float MaxX, float MaxY)
{
    if (pos.x > MaxX) {pos.x = 0.0;}

    if (pos.x < 0)    {pos.x = MaxX;}

    if (pos.y < 0)    {pos.y = MaxY;}

    if (pos.y > MaxY) {pos.y = 0.0;}
}

#endif 


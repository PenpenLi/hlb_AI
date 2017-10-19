#ifndef CELLSPACEPARTITION_H
#define CELLSPACEPARTITION_H
//-----------------------------------------------------------------------------
//  Desc:   class to divide a 2D space into a grid of cells each of which
//          may contain a number of entities. Once created and initialized 
//          with entities, fast proximity querys can be made by calling the
//          CalculateNeighbors method with a position and proximity radius.
//
//          If an entity is capable of moving, and therefore capable of moving
//          between cells, the Update method should be called each update-cycle
//          to sychronize the entity and the cell space it occupies
//
//-----------------------------------------------------------------------------
#pragma warning (disable:4786)

#include <vector>
#include <list>

#include "common/2D/Vector2D.h"
#include "common/2D/InvertedAABBox2D.h"



//------------------------------------------------------------------------
//
//  defines a cell containing a list of pointers to entities
//------------------------------------------------------------------------
template <class entity>
struct Cell
{
    //all the entities inhabiting this cell
    std::list<entity> members;

    //the cell's bounding box (it's inverted because the Window's default
    //co-ordinate system has a y axis that increases as it descends)
    InvertedAABBox2D BBox;

    Cell(Vector2D topleft, Vector2D botright):BBox(InvertedAABBox2D(topleft, botright))
    {
    }
};

/////////// //////////////////////////////////////////////////////////////////
//  the subdivision class
///////////////////////////////////////////////////////////////////////////////

template <class entity>
class CellSpacePartition
{
public:
    CellSpacePartition(Vector2D& spaceSize,
                                         int cellsNumX,       //number of cells horizontally
                                         int cellsNumY,       //number of cells vertically
                                         int maxEntitys);  //maximum number of entities to add

    //adds entities to the class by allocating them to the appropriate cell
    inline void addEntity(const entity& ent);
    inline void removeEntity(entity& ent);
    
    //update an entity's cell by calling this from your entity's update method 
    inline void updateEntity(const entity& ent, Vector2D prePos);
    
    //this method calculates all a target's neighbors and stores them in
    //the neighbor vector. After you have called this method use the begin, 
    //next and end methods to iterate through the vector.
    inline void calculateNeighbors(Vector2D targetPos, float radius);
    
    //clear the cells of entities
    void clearCells();
    
    //returns a reference to the entity at the front of the neighbor vector
    inline entity& begin(){m_curNeighbor = m_neighbors.begin(); return *m_curNeighbor;}

    //this returns the next entity in the neighbor vector
    inline entity& next(){++m_curNeighbor; return *m_curNeighbor;}

    //returns true if the end of the vector is found (a zero value marks the end)
    inline bool end(){return (m_curNeighbor == m_neighbors.end()) || (*m_curNeighbor == 0);}   
  
private:
    int positionToIndex(const Vector2D& pos)const;
        
    //the required amount of cells in the space
    std::vector<Cell<entity>> m_Cells;

    //this is used to store any valid neighbors when an agent searches its neighboring space
    std::vector<entity> m_neighbors;

    //this iterator will be used by the methods next and begin to traverse
    //through the above vector of neighbors
    typename std::vector<entity>::iterator m_curNeighbor;
  
    //this iterator will be used by the methods next and begin to traverse
    //through the above vector of neighbors
    typename std::vector<entity>::iterator m_curIndex;
    
    //the width and height of the world space
    Vector2D m_spaceSize;
    
    //unit size 
    Vector2D m_cellSize; 
    
    //the number of cells the space is going to be divided up into
    int m_cellsNUmX;
    int m_cellsNUmY;
    
    //given a position in the game space this method determines the relevant cell's index

};



//----------------------------- ctor ---------------------------------------
//--------------------------------------------------------------------------
template<class entity>
CellSpacePartition<entity>::CellSpacePartition(Vector2D& spaceSize,
                                         int cellsNumX,       //number of cells horizontally
                                         int cellsNumY,       //number of cells vertically
                                         int maxEntitys):  //maximum number of entities to partition
                              m_spaceSize(spaceSize),
                              m_cellsNUmX(cellsNumX),
                              m_cellsNUmY(cellsNumY),
                              m_neighbors(maxEntitys, entity())
{
    //calculate bounds of each cell
    m_cellSize.x = spaceSize.x /cellsNumX;
    m_cellSize.y = spaceSize.y /cellsNumY;
  
    //create the cells 
    float left, right, top, bot;
    for (int i=0; i<m_cellsNUmY; ++i)
    {
        for (int j=0; j<m_cellsNUmX; ++j)
        {
            left  = j * m_cellSize.x;
            right = left + m_cellSize.x;
            top   = i * m_cellSize.y;
            bot   = top + m_cellSize.y;
            m_Cells.push_back(Cell<entity>(Vector2D(left, top), Vector2D(right, bot)));
        }
    }
}



//----------------------- AddEntity --------------------------------------
//  Used to add the entitys to the data structure
//------------------------------------------------------------------------
template<class entity>
inline void CellSpacePartition<entity>::addEntity(const entity& ent)
{ 
    int idx = positionToIndex(ent->getPos());
    m_Cells[idx].members.push_back(ent);
}

//----------------------- AddEntity --------------------------------------
//  Used to add the entitys to the data structure
//------------------------------------------------------------------------
template<class entity>
inline void CellSpacePartition<entity>::removeEntity(entity& ent)
{ 
    int idx = positionToIndex(ent->getPos());
    m_Cells[idx].members.remove(ent);
}

//----------------------- UpdateEntity -----------------------------------
//
//  Checks to see if an entity has moved cells. If so the data structure
//  is updated accordingly
//------------------------------------------------------------------------
template<class entity>
inline void CellSpacePartition<entity>::updateEntity(const entity&  ent, Vector2D prePos)
{
    //if the index for the old pos and the new pos are not equal then
    //the entity has moved to another cell.
    int OldIdx = positionToIndex(prePos);
    int NewIdx = positionToIndex(ent->getPos());

    if (NewIdx == OldIdx) return;

    //the entity has moved into another cell so delete from current cell
    //and add to new one
    m_Cells[OldIdx].members.remove(ent);
    m_Cells[NewIdx].members.push_back(ent);
}


//----------------------- CalculateNeighbors ----------------------------
//
//  This must be called to create the vector of neighbors.This method 
//  examines each cell within range of the target, If the 
//  cells contain entities then they are tested to see if they are situated
//  within the target's neighborhood region. If they are they are added to
//  neighbor list
//------------------------------------------------------------------------
template<class entity>
void CellSpacePartition<entity>::calculateNeighbors(Vector2D targetPos, float radius)
{
    //create an iterator and set it to the beginning of the neighbor vector
    std::vector<entity>::iterator curNbor = m_neighbors.begin();
  
    //create the query box that is the bounding box of the target's query area
    InvertedAABBox2D QueryBox(targetPos - Vector2D(radius, radius), targetPos + Vector2D(radius, radius));

    //iterate through each cell and test to see if its bounding box overlaps
    //with the query box. If it does and it also contains entities then
    //make further proximity tests.
    std::vector<Cell<entity> >::iterator curCell; 
    for (curCell=m_Cells.begin(); curCell!=m_Cells.end(); ++curCell)
    {
        //test to see if this cell contains members and if it overlaps the query box
        if (curCell->BBox.isOverlappedWith(QueryBox) && !curCell->members.empty())
        {
            //add any entities found within query radius to the neighbor list
            std::list<entity>::iterator it = curCell->members.begin();
            for (it; it!=curCell->members.end(); ++it)
            { 
                if (Vec2DistanceSq((*it)->getPos(), targetPos) < radius*radius)
                {
                    *curNbor++ = *it;
                }
            }
        }
    }//next cell

    //mark the end of the list with a zero.
    *curNbor = 0;
}


template<class entity>
void CellSpacePartition<entity>::clearCells()
{
    std::vector<Cell<entity>>::iterator it = m_Cells.begin();

    for (it; it!=m_Cells.end(); ++it)
    {
        it->members.clear();
    }
}


//--------------------- positionToIndex ----------------------------------
//  Given a 2D vector representing a position within the game world, this
//  method calculates an index into its appropriate cell
//------------------------------------------------------------------------
template<class entity>
inline int CellSpacePartition<entity>::positionToIndex(const Vector2D& pos)const
{
    int idx = (int)(m_cellsNUmX * pos.x / m_spaceSize.x) + 
            ((int)((m_cellsNUmY) * pos.y / m_spaceSize.y) * m_cellsNUmX);
    
    //if the entity's position is equal to vector2d(m_spaceWidth, m_spaceHeight)
    //then the index will overshoot. We need to check for this and adjust
    if (idx > (int)m_Cells.size()-1)
    {
        idx = (int)m_Cells.size()-1;
    }
    
    return idx; 
}


#endif
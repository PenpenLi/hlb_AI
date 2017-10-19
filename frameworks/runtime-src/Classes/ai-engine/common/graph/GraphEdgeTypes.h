#ifndef GRAPH_EDGE_TYPES_H
#define GRAPH_EDGE_TYPES_H
//-----------------------------------------------------------------------------
//
//  Name:   GraphEdgeTypes.h
//
//
//  Desc:   Class to define an edge connecting two nodes.
//          
//          An edge has an associated cost.
//-----------------------------------------------------------------------------



class GraphEdge
{
protected:
    //An edge connects two nodes. Valid node indices are always positive.
    int m_iFrom;
    int m_iTo;

    //the cost of traversing the edge
    float m_dCost;

public:

    //ctors
    GraphEdge(int from, int to, float cost):m_dCost(cost),m_iFrom(from),m_iTo(to)
    {
    }
  
    GraphEdge(int from, int  to):m_dCost(1.0),m_iFrom(from),m_iTo(to)
    {
    }
  
    GraphEdge():m_dCost(1.0),m_iFrom(-1),m_iTo(-1)
    {
    }

    virtual ~GraphEdge(){}

    int from()const{return m_iFrom;}
    void setFrom(int NewIndex){m_iFrom = NewIndex;}

    int to()const{return m_iTo;}
    void setTo(int NewIndex){m_iTo = NewIndex;}

    float cost()const{return m_dCost;}
    void setCost(float NewCost){m_dCost = NewCost;}

    //these two operators are required
    bool operator==(const GraphEdge& rhs)
    {
        return  rhs.m_iFrom == this->m_iFrom &&
                    rhs.m_iTo   == this->m_iTo   &&
                    rhs.m_dCost == this->m_dCost;
    }

    bool operator!=(const GraphEdge& rhs)
    {
        return !(*this == rhs);
    }
};


class NavGraphEdge : public GraphEdge
{
public:
    //examples of typical flags
    enum
    {
        normal  = 0,
        swim    = 1 << 0,
        crawl   = 1 << 1,
        creep   = 1 << 3,
        jump    = 1 << 3,
        fly         = 1 << 4,
        grapple = 1 << 5,
        goes_through_door = 1 << 6
    };

protected:
    int m_iFlags;

    //if this edge intersects with an object (such as a door or lift), then
    //this is that object's ID. 
    int m_intersectingEntityID;

public:
    NavGraphEdge( int from,
                                    int to,
                                    float cost,
                                    int flags = 0,
                                    int id = -1):GraphEdge(from,to,cost),
                                                   m_iFlags(flags),
                                                   m_intersectingEntityID(id)

    {
    } 

    int  getFlags()const{return m_iFlags;}
    void setFlags(int flags){m_iFlags = flags;}
  
    int  getIntersectingEntityID()const{return m_intersectingEntityID;}
    void setIIntersectingEntityID(int id){m_intersectingEntityID = id;} 
};


#endif
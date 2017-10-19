#ifndef TELEGRAM_H
#define TELEGRAM_H
//------------------------------------------------------------------------
//
//  Name:   Telegram.h
//
//  Desc:   This defines a telegram. A telegram is a data structure that
//          records information required to dispatch messages. Messages 
//          are used by game agents to communicate with each other.
//
//
//------------------------------------------------------------------------
#include <iostream>
#include <math.h>
#include <chrono>
#include <queue>

struct Telegram
{
    //the entity that sent this telegram
    int m_sender;

    //the entity that is to receive this telegram
    int m_receiver;
    
    //the message itself. These are all enumerated in the file
    //"MessageTypes.h"
    int m_msgId;

    //messages can be dispatched immediately or delayed for a specified amount
    //of time. If a delay is necessary this field is stamped with the time 
    //the message should be dispatched.
    std::chrono::steady_clock::time_point m_dispatchTime;

    //any additional information that may accompany the message
    void* m_extraInfo;

    Telegram(float delay,
           int sender,
           int receiver,
           int msg,
           void* info = NULL): m_sender(sender),
                                         m_receiver(receiver),
                                         m_msgId(msg),
                                         m_extraInfo(info)
    {
        if (delay >= 0.0)
        {
            m_dispatchTime = std::chrono::steady_clock::now() + std::chrono::milliseconds((int)(delay*1000));
        }
    }

    //for std::priority_queue customize sort , by hlb
    friend bool operator<(const Telegram &a,const Telegram &b)
    {
        return a.m_dispatchTime > b.m_dispatchTime;  
    }    
};

//typedef std::priority_queue<Telegram> PriorityQueue; //STL queue


//these telegrams will be stored in a priority queue. Therefore the >
//operator needs to be overloaded so that the PQ can sort the telegrams
//by time priority. Note how the times must be smaller than
//SmallestDelay apart before two Telegrams are considered unique.

inline bool operator==(const Telegram& t1, const Telegram& t2)
{
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1.m_dispatchTime - t2.m_dispatchTime).count();
    return ( std::abs(dt) < 250 && (t1.m_sender == t2.m_sender) && 
        (t1.m_receiver == t2.m_receiver) && (t1.m_msgId == t2.m_msgId));
}

//handy helper function for dereferencing the ExtraInfo field of the Telegram 
//to the required type.
template <class T>
inline T dereferenceToType(void* p)
{
    return *(T*)(p);
}


#endif
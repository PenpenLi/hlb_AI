#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <vector>
#include <cassert>

//----------------------- swap -------------------------------------------
//  used to swap two values
//------------------------------------------------------------------------
template<class T>
void swap(T &a, T &b)
{
    T temp = a;
    a = b;
    b = temp;
}

//-------------------- reorderUpwards ------------------------------------
//  given a heap and a node in the heap, this function moves upwards
//  through the heap swapping elements until the heap is ordered
//------------------------------------------------------------------------
template<class T>
void reorderUpwards(std::vector<T>& heap, int nd)
{
    //move up the heap swapping the elements until the heap is ordered
    while ( (nd>1) && (heap[nd/2] < heap[nd]))
    {
        swap(heap[nd/2], heap[nd]);
        nd /= 2;
    }
}

//--------------------- reorderDownwards ---------------------------------
//
//  given a heap, the heapsize and a node in the heap, this function
//  reorders the elements in a top down fashion by moving down the heap
//  and swapping the current node with the greater of its two children
//  (provided a child is larger than the current node)
//------------------------------------------------------------------------
template<class T>
void reorderDownwards(std::vector<T>& heap, int nd, int HeapSize)
{
    //move down the heap from node nd swapping the elements until
    //the heap is reordered
    while (2*nd <= HeapSize)
    {
        int child = 2 * nd;

        //set child to largest of nd's two children
        if ( (child < HeapSize) && (heap[child] < heap[child+1]) )
        {
            ++child;
        }

        //if this nd is smaller than its child, swap
        if (heap[nd] < heap[child])
        {
            swap(heap[child], heap[nd]);

            //move the current node down the tree
            nd = child;
        }

        else
        {
            break;
        }
    }
}



//--------------------- PriorityQ ----------------------------------------
//
//  basic heap based priority queue implementation
//------------------------------------------------------------------------
template<class T>
class PriorityQ
{
private:
    std::vector<T> m_Heap;

    int m_iSize;

    int m_iMaxSize;
    
    //given a heap and a node in the heap, this function moves upwards
    //through the heap swapping elements until the heap is ordered
    void reorderUpwards(std::vector<T>& heap, int nd)
    {
        //move up the heap swapping the elements until the heap is ordered
        while ( (nd>1) && (heap[nd/2] < heap[nd]))
        {
            swap(heap[nd/2], heap[nd]);
            nd /= 2;
        }
    }

    //given a heap, the heapsize and a node in the heap, this function
    //reorders the elements in a top down fashion by moving down the heap
    //and swapping the current node with the greater of its two children
    //(provided a child is larger than the current node)
    void reorderDownwards(std::vector<T>& heap, int nd, int heapSize)
    {
        //move down the heap from node nd swapping the elements until
        //the heap is reordered
        while (2*nd <= heapSize)
        {
            int child = 2 * nd;

            //set child to largest of nd's two children
            if ( (child < heapSize) && (heap[child] < heap[child+1]) )
            {
                ++child;
            }
            
            //if this nd is smaller than its child, swap
            if (heap[nd] < heap[child])
            {
                swap(heap[child], heap[nd]);

                //move the current node down the tree
                nd = child;
            }
            else
            {
                break;
            }
        }
    }

public:
    PriorityQ(int maxSize):m_iMaxSize(maxSize), m_iSize(0)
    {
        m_Heap.assign(maxSize+1, T());
    }
    
    bool isEmpty()const{return (m_iSize==0);}

    //to insert an item into the queue it gets added to the end of the heap
    //and then the heap is reordered
    void insert(const T item)
    {
        assert (m_iSize+1 <= m_iMaxSize);

        ++m_iSize;

        m_Heap[m_iSize] = item;

        reorderUpwards(m_Heap, m_iSize);
    }

    //to get the max item the first element is exchanged with the lowest
    //in the heap and then the heap is reordered from the top down. 
    T pop()
    {
        swap(m_Heap[1], m_Heap[m_iSize]);

        reorderDownwards(m_Heap, 1, m_iSize-1);
        
        return m_Heap[m_iSize--];
    }

    //so we can take a peek at the first in line
    const T& peek()const{return m_Heap[1];}
};

//--------------------- PriorityQLow -------------------------------------
//
//  basic 2-way heap based priority queue implementation. This time the priority
//  is given to the lowest valued key
//------------------------------------------------------------------------
template<class T>
class PriorityQLow
{
private:
    std::vector<T> m_Heap;

    int m_iSize;

    int m_iMaxSize;

    //given a heap and a node in the heap, this function moves upwards
    //through the heap swapping elements until the heap is ordered
    void reorderUpwards(std::vector<T>& heap, int nd)
    {
        //move up the heap swapping the elements until the heap is ordered
        while ( (nd>1) && (heap[nd/2] > heap[nd]))
        {
            swap(heap[nd/2], heap[nd]);
            nd /= 2;
        }
    }

    //given a heap, the heapsize and a node in the heap, this function
    //reorders the elements in a top down fashion by moving down the heap
    //and swapping the current node with the smaller of its two children
    //(provided a child is larger than the current node)
    void reorderDownwards(std::vector<T>& heap, int nd, int HeapSize)
    {
        //move down the heap from node nd swapping the elements until
        //the heap is reordered
        while (2*nd <= HeapSize)
        {
            int child = 2 * nd;

            //set child to largest of nd's two children
            if ( (child < HeapSize) && (heap[child] > heap[child+1]) )
            {
                ++child;
            }

            //if this nd is smaller than its child, swap
            if (heap[nd] > heap[child])
            {
                swap(heap[child], heap[nd]);

                //move the current node down the tree
                nd = child;
            }
            else
            {
                break;
            }
        }
    }

public:
    PriorityQLow(int MaxSize):m_iMaxSize(MaxSize), m_iSize(0)
    {
        m_Heap.assign(MaxSize+1, T());
    }

    bool isEmpty()const{return (m_iSize==0);}

    //to insert an item into the queue it gets added to the end of the heap
    //and then the heap is reordered
    void insert(const T item)
    {
        assert (m_iSize+1 <= m_iMaxSize);

        ++m_iSize;

        m_Heap[m_iSize] = item;

        reorderUpwards(m_Heap, m_iSize);
    }

    //to get the max item the first element is exchanged with the lowest
    //in the heap and then the heap is reordered from the top down. 
    T pop()
    {
        swap(m_Heap[1], m_Heap[m_iSize]);

        reorderDownwards(m_Heap, 1, m_iSize-1);

        return m_Heap[m_iSize--];
    }

    //so we can take a peek at the first in line
    const T& peek()const{return m_Heap[1];}
};

//----------------------- IndexedPriorityQLow ---------------------------
//
//  Priority queue based on an index into a set of keys. The queue is
//  maintained as a 2-way heap.
//
//  The priority in this implementation is the lowest valued key
//------------------------------------------------------------------------
template<class KeyType>
class indexedPriorityQLow
{
private:
    std::vector<KeyType>& m_vecKeys;

    std::vector<int> m_Heap;

    std::vector<int> m_invHeap;

    int m_iSize, m_iMaxSize;

    void swap(int a, int b)
    {
        int temp = m_Heap[a]; m_Heap[a] = m_Heap[b]; m_Heap[b] = temp;

        //change the handles too
        m_invHeap[m_Heap[a]] = a; m_invHeap[m_Heap[b]] = b;
    }

    void reorderUpwards(int nd)
    {
        //move up the heap swapping the elements until the heap is ordered
        while ( (nd>1) && (m_vecKeys[m_Heap[nd/2]] > m_vecKeys[m_Heap[nd]]) )
        {      
            swap(nd/2, nd);

            nd /= 2;
        }
    }

    void reorderDownwards(int nd, int HeapSize)
    {
        //move down the heap from node nd swapping the elements until
        //the heap is reordered
        while (2*nd <= HeapSize)
        {
            int child = 2*nd;

            //set child to smaller of nd's two children
            if ((child < HeapSize) && (m_vecKeys[m_Heap[child]] > m_vecKeys[m_Heap[child+1]]))
            {
                ++child;
            }

            //if this nd is larger than its child, swap
            if (m_vecKeys[m_Heap[nd]] > m_vecKeys[m_Heap[child]])
            {
                swap(child, nd);

                //move the current node down the tree
                nd = child;
            }
            else
            {
                break;
            }
        }
    }


public:
    //you must pass the constructor a reference to the std::vector the PQ
    //will be indexing into and the maximum size of the queue.
    indexedPriorityQLow(std::vector<KeyType>& keys,int MaxSize):m_vecKeys(keys),
                                                                                                                m_iMaxSize(MaxSize),
                                                                                                                m_iSize(0)
    {
        m_Heap.assign(MaxSize+1, 0);
        m_invHeap.assign(MaxSize+1, 0);
    }

    bool isEmpty()const{return (m_iSize==0);}

    //to insert an item into the queue it gets added to the end of the heap
    //and then the heap is reordered from the bottom up.
    void insert(const int idx)
    {
        assert (m_iSize+1 <= m_iMaxSize);

        ++m_iSize;

        m_Heap[m_iSize] = idx;

        m_invHeap[idx] = m_iSize;

        reorderUpwards(m_iSize);
    }

    //to get the min item the first element is exchanged with the lowest
    //in the heap and then the heap is reordered from the top down. 
    int pop()
    {
        swap(1, m_iSize);

        reorderDownwards(1, m_iSize-1);

        return m_Heap[m_iSize--];
    }

    //if the value of one of the client key's changes then call this with 
    //the key's index to adjust the queue accordingly
    void changePriority(const int idx)
    {
        reorderUpwards(m_invHeap[idx]);
    }
};
#endif
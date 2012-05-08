#ifndef __QUEUE_H_
#define __QUEUE_H_

#include <pthread.h>
#include <cstdlib>
#include <iostream>

template <typename T>
class Queue {
    struct Node 
    {
        struct Node *next;
        T value;
        Node():next(NULL)
        {}
    };
    
    struct Node *head;
    struct Node *tail;
    pthread_mutex_t hlock;
    pthread_mutex_t tlock;
    struct Node s;//satellite node
    unsigned int size;
public:
    Queue():head(&s),tail(&s),size(0)
    {
        //hlock(PTHREAD_MUTEX_INITIALIZER),tlock(PTHREAD_MUTEX_INITIALIZER)
        hlock = PTHREAD_MUTEX_INITIALIZER;
        tlock = PTHREAD_MUTEX_INITIALIZER;
    }
    
    int enQueue(const T & elem)
    {
        struct Node *pNode = new struct Node;
        
        pNode->value = elem;
        
        pthread_mutex_lock(&tlock);
        
        tail->next = pNode;
        tail = pNode;
        
        pthread_mutex_unlock(&tlock);
        
        __sync_fetch_and_add(&size,1);
        
        return 0;
        
    }
    
    int deQueue(T *t)
    {
        struct Node *h1,*h2;
        
        if(!t) return -1;
        
        pthread_mutex_lock(&hlock);

        h1 = head;
        h2 = h1->next;
        if(!h2) {
            pthread_mutex_unlock(&hlock);
            #ifdef DEBUG
            std::cout << "queue is empty" << std::endl;
            #endif
            return -1;
        }
        
        *t = h2->value;
        head = h2;
        
        pthread_mutex_unlock(&hlock);
        
        __sync_fetch_and_sub(&size,1);
        if(h1 != &s)
            delete h1;
        
        return 0;
    }

    bool isEmpty(void)
    {
        return 0==__sync_fetch_and_add(&size,0);
    }
    
    ~Queue()
    {
        T t;
        while(0 == deQueue(&t))
            ;
    }
    
};

#endif
